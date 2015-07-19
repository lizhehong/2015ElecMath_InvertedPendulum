#include "TIM4.h"
#include "stm32f10x.h"
#include "MINI256Z.h"
#include "FreePole.h"
#include "PID.h"
#include <stdlib.h>
#include "TIM2_CH4_PWM.h"
#include "StepperMotor.h"
//定时器四配置
void TIM4_Configuration(void) 
{ 
	TIM_TimeBaseInitTypeDef TIM_BaseInitStructure; 
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_DeInit( TIM4);                               
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);   
	
	TIM_BaseInitStructure.TIM_Period = 799; 		
	TIM_BaseInitStructure.TIM_Prescaler = 17;  		
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
//200us 进来一次
int count = 0;
char timeout = 0;
void TIM4_IRQHandler(){
	if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET){
		count++;
		timeout++;
		calcAngleSpeed_MINI256(count,0.0002);
		if(timeout == 15 && Usart_Commod_Flag == 0x40){
			pid.Kp =4;
			pid.Ki = 0;
			pid.Kd = 0;
			pid.sp = 180;
			pid.lastAngle = pid.Angle;
			pid.Angle = FreePole.AcutalAngle;
			pid.velocity = FreePole.AngleSpeed;
			if(abs(pid.Angle - pid.sp ) <= 4){
					pid.error_p = 0;
			}
			else{
					pid.error_p = pid.sp - pid.Angle;
					pid.error_d = pid.velocity;//我这里直接给角速度
					pid.error_i = pid.error_i + (pid.error_p*0.003);
					pid.speed = pid.Kp * pid.error_p 
													+ 
													pid.Ki * pid.error_i 
													+ 
													pid.Kp* pid.error_p;
												
			}
			printf("pid.speed = %f",pid.speed);
			if(pid.speed >=0)
				SET_TIM2_CH4_Fre_AND_PULSENUM(abs(pid.speed),30,clockwise);
			else{
				SET_TIM2_CH4_Fre_AND_PULSENUM(abs(pid.speed),30,anticlockwise);
			}
			timeout = 0;
		}
		if(count == 60000){
			count = 0;//防止 计数溢出
		}
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update); 				
	}	
}
