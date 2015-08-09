#include "TIM2_CH4_PWM.h"
#include "StepperMotor.h"
#include "Delay.h"
#include "FreePole.h"
#include "Led.h"
#include <stdlib.h>
#include "MYDMA.h"
TIM_TimeBaseInitTypeDef TIM_BaseInitStructure; 
//TIM2配置
void TIM2_Configuration(void) 
{ 

	TIM_OCInitTypeDef  TIM2_OCInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	TIM_DeInit( TIM2);                               
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);   
	
	TIM_BaseInitStructure.TIM_Period = 0; 		//这里设置无所谓 但是为了单片机能够立即响应 我应该清零
	TIM_BaseInitStructure.TIM_Prescaler = 7199;  		//分频系数 决定最高频率 3k 最低频率 0.3662109375HZ
	TIM_BaseInitStructure.TIM_ClockDivision = 0; 
	TIM_BaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;   
	TIM_BaseInitStructure.TIM_RepetitionCounter = 0; 
	TIM_TimeBaseInit(TIM2, &TIM_BaseInitStructure); 
	
	TIM2_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;//TIMx_CNT<TIMx_CCRx时通道x为有效电平
	TIM2_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM2_OCInitStructure.TIM_Pulse = TIM2->ARR*0.9;			//高电平有效
	TIM2_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;//规定有效电平是高
	TIM_OC4Init(TIM2,&TIM2_OCInitStructure);
	
	//用于PWM输出时 记录脉冲数
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;    
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;    //优先级      
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;            
	NVIC_Init(&NVIC_InitStructure);
	
	TIM_ClearFlag(TIM2, TIM_FLAG_Update); 

	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE); 
	
	
	TIM_Cmd(TIM2, ENABLE);
	//关闭PWM 防止开机启动
	TIM_SetCompare4(TIM2,0);	
}
//实现 在什么时候设置此函数
//则PWM运作 会以 此刻开始执行 抛弃前面设置的执行过程
void SET_TIM2_CH4_Fre(uint16_t fre,char dir){
		//频率设置
		if(fre>0){//只要频率不为0，就调节		
			TIM_SetAutoreload(TIM2,abs((10000/fre)-1));//就是总共需要多少这样的计数值
			StepperMotor.FRE = fre;
			TIM_SetCompare4(TIM2,TIM2->ARR*0.5);//保持党频率发送变化的时候，占空比总是占据50%
			//清空定时器计数值，防止前一时刻的累加效果 而且能够尽可能的减少反应时间
			//TIM_SetCounter(TIM2,0);//不要加这个 会影响低频 而且高频的精度都会影响 请测试！！！！！！！！2015/8/5		
		}else if(fre<=0){
			TIM_SetCompare4(TIM2,0);//保持党频率发送变化的时候，占空比总是占据0%
			StepperMotor.FRE = 0;
		}
		//顺时针判断
		if(dir == anticlockwise){
			PBout(10) = StepperMotor.DIR = anticlockwise;
		}
		//逆时针判断
		else if(dir == clockwise){
			PBout(10) = StepperMotor.DIR = clockwise;
		}
}
//目的是为了记录脉冲数
void TIM2_IRQHandler()
{

	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET){  
		if((StepperMotor.FRE != 0 )&& ((TIM2->ARR/TIM2->CCR4) >1)){//电机工作的时候 才进行脉冲计数
			if(StepperMotor.DIR == clockwise){//顺时针
				StepperMotor.ActualPulseNum++;//代表实际的脉冲数
				//USART_DMA1_Send("PN=",++StepperMotor.ActualPulseNum);
			}
			else if(StepperMotor.DIR == anticlockwise){//逆时针
				StepperMotor.ActualPulseNum--;//代表实际的脉冲数
				//USART_DMA1_Send("PN=",--StepperMotor.ActualPulseNum);
			}
			if(StepperMotor.ActualPulseNum <0){
				StepperMotor.ActualPulseNum = 3200;
			}
			if(StepperMotor.ActualPulseNum >3200){
				StepperMotor.ActualPulseNum = 0;
			}
		}
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);		
	}
}
