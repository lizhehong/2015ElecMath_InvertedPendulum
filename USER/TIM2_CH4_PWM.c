#include "TIM2_CH4_PWM.h"
#include "StepperMotor.h"
#include "Delay.h"
#include "FreePole.h"
//TIM2配置
void TIM2_Configuration(void) 
{ 
	TIM_TimeBaseInitTypeDef TIM_BaseInitStructure; 
	TIM_OCInitTypeDef  TIM2_OCInitStructure;
	TIM_DeInit( TIM2);                               
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);   
	
	TIM_BaseInitStructure.TIM_Period = 800; 		//ÕâÀïÐèÒª¸Ä¶¯
	TIM_BaseInitStructure.TIM_Prescaler = 54;  		//ÕâÀïÐèÒª¸Ä¶¯
	TIM_BaseInitStructure.TIM_ClockDivision = 0; 
	TIM_BaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;   
	TIM_BaseInitStructure.TIM_RepetitionCounter = 0; 
	TIM_TimeBaseInit(TIM2, &TIM_BaseInitStructure); 
	
	TIM2_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;//TIMx_CNT<TIMx_CCRxÊ±Í¨µÀxÎªÓÐÐ§µçÆ½
	TIM2_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM2_OCInitStructure.TIM_Pulse = TIM2->ARR*0.9;			//ÕâÀïÐèÒª¸Ä¶¯ ¸Ä¶¯Îªºã¶¨90% ÕâÀïÊÇÒª¸ßµçÆ½(ÓÐÐ§µçÆ½)µÄÕ¼ÓÐÂÊ
	TIM2_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;//¹æ¶¨ÓÐÐ§µçÆ½ÊÇ¸ß
	TIM_OC4Init(TIM2,&TIM2_OCInitStructure);
	

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
		if(pulseNum>0){//只要期望的脉冲数不为0，就调节
			StepperMotor.PULSE_SETPOINT = pulseNum;
			StepperMotor.PULSE_ACTUAL = 0;
		}
		//频率设置
		if(fre>0){//只要频率不为0，就调节
			TIM_SetAutoreload(TIM2,1309091/fre);//依据72000 000 /55(分频系数) =  1309091
			StepperMotor.FRE = fre;
			TIM_SetCompare4(TIM2,TIM2->ARR*0.9);//保持党频率发送变化的时候，占空比总是占据90%
		}
		//顺时针判断
		if(dir == anticlockwise){
			PBout(10) = StepperMotor.DIR = anticlockwise;
		}
		//逆时针判断
		else if(dir == clockwise){
			PBout(10) = StepperMotor.DIR = clockwise;
		}
		delay_ms((u16)pulseNum/fre);
		printf("BigMotor.AcutalAngle=%f \n",StepperMotor.AcutalAngle);
}

