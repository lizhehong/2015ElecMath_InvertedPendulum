#include "TIM4.h"
#include "stm32f10x.h"
#include "MINI256Z.h"
#include "FreePole.h"
#include "PID.h"
#include <stdlib.h>
#include "TIM2_CH4_PWM.h"
#include "StepperMotor.h"
#include "Led.h"

int  TIM4_TASK = 0;


//定时器四配置
void TIM4_Configuration(void) 
{ 
	TIM_TimeBaseInitTypeDef TIM_BaseInitStructure; 
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_DeInit( TIM4);                               
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);   
	
	TIM_BaseInitStructure.TIM_Period = 9; 		
	TIM_BaseInitStructure.TIM_Prescaler = 7199;  		
	TIM_BaseInitStructure.TIM_ClockDivision = 0; 
	TIM_BaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;   
	TIM_BaseInitStructure.TIM_RepetitionCounter = 0; 
	TIM_TimeBaseInit(TIM4, &TIM_BaseInitStructure); 
	
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;    
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;   //抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;     //优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;            
	NVIC_Init(&NVIC_InitStructure);  
	
	TIM_ClearFlag(TIM4, TIM_FLAG_Update); 

	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE); 

	TIM_Cmd(TIM4, ENABLE); 
}
//1ms 进来一次
int count = 0;
char timeout = 0;
void TIM4_IRQHandler(){
	if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET){
		count++;
		if(count%3){//PID 控制时间到了 3ms
			TIM4_TASK = 0x01;
		}
		if(count % 500){
			LED1 = ~LED1;
		}
		if(count == 60000){
			count = 0;
		}
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update); 				
	}
}
