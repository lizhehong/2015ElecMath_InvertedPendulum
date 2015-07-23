#include "TIM2_CH4_PWM.h"
#include "StepperMotor.h"
#include "Delay.h"
#include "FreePole.h"
#include "Led.h"
//TIM2配置
void TIM2_Configuration(void) 
{ 
	TIM_TimeBaseInitTypeDef TIM_BaseInitStructure; 
	TIM_OCInitTypeDef  TIM2_OCInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	
	TIM_DeInit( TIM2);                               
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);   
	
	TIM_BaseInitStructure.TIM_Period = 799; 		//重装值
	TIM_BaseInitStructure.TIM_Prescaler = 54;  		//分频系数
	TIM_BaseInitStructure.TIM_ClockDivision = 0; 
	TIM_BaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;   
	TIM_BaseInitStructure.TIM_RepetitionCounter = 0; 
	TIM_TimeBaseInit(TIM2, &TIM_BaseInitStructure); 
	
	TIM2_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;//TIMx_CNT<TIMx_CCRx时通道x为有效电平
	TIM2_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM2_OCInitStructure.TIM_Pulse = TIM2->ARR*0.9;			//高电平有效
	TIM2_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;//规定有效电平是高
	TIM_OC4Init(TIM2,&TIM2_OCInitStructure);
	
	//用于PWM输出
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;    
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;    //优先级      
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
void SET_TIM2_CH4_Fre_AND_PULSENUM(uint16_t fre,uint16_t  pulseNum,char dir){
		//脉冲数设置
		if(pulseNum>0){//只要期望的脉冲数0，就调节
			StepperMotor.PULSE_SETPOINT = pulseNum;
			StepperMotor.PULSE_ACTUAL = 0;
		}
		//频率设置
		if(fre>0){//只要频率不为0，就调节		
			TIM_SetAutoreload(TIM2,1309091/fre);//依据72000 000 /55(分频系数) =  1309091
			//清空定时器计数值，防止前一时刻的累加效果
			//TIM_SetCounter(TIM2,0);
			StepperMotor.FRE = fre;
			TIM_SetCompare4(TIM2,TIM2->ARR*0.9);//保持党频率发送变化的时候，占空比总是占据90%
		}else if(fre<=0){
			TIM_SetCompare4(TIM2,0);//保持党频率发送变化的时候，占空比总是占据90%
		}
		//顺时针判断
		if(dir == anticlockwise){
			PBout(10) = StepperMotor.DIR = anticlockwise;
		}
		//逆时针判断
		else if(dir == clockwise){
			PBout(10) = StepperMotor.DIR = clockwise;
		}
		//delay_ms((u16)pulseNum/fre);
		//printf("BigMotor.AcutalAngle=%f \n",FreePole.AcutalAngle);
}
//TIM2的中断函数 此处为PWM
//个人觉得：因为根据PWM生成的原理
//在一个计数周期内
//PWM1下 只有当
//TIMx_CNT<TIMx_CCRX时通道x为有效电平
//PWM1 下只有当
//TIMx_CNT>=TMx_CCRX时通道x为无效电平
//所以总体来说，只是一个计数周期对应一个电平变化
//①高->低②低->高
//所以为了记住PWM脉冲的个数，可以中断中计数
//因为一个计数的溢出 代表中断一次
//而一个计数周期 又只是一个PWM中 高低电平变化一次
//PWM开始
//对于任何的外设来说 都是低电平
//______此刻是开始--------|______中断一次|--------|_______中断两次|--------|____中断三次|------
//所以 对应的脉冲数 与实际的中断数是一样的
void TIM2_IRQHandler()
{

	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET) 
    {       
		if(StepperMotor.PULSE_ACTUAL>=StepperMotor.PULSE_SETPOINT){//实际的脉冲和期望的脉冲数量一致
			//关闭PWM，其实也就是，持续低电平就行了
			TIM_SetCompare4(TIM2,0);
			LED1 = 0;
		}else{
			StepperMotor.PULSE_ACTUAL++;//代表实际的脉冲数
			LED1 = 1;
		}
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);		
	}
}
