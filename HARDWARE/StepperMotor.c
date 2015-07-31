#include "StepperMotor.h"
#include "string.h"
#include <stdlib.h>
#include "TIM2_CH4_PWM.h"
#include "Led.h"
//步进电机声明
MOTORtypedef StepperMotor;



void MotorParamsInit(){
	StepperMotor.subdivide = 16;//先使用16细分
	//依据细分读书设置 电机步转角
	StepperMotor.StepAngle = 1.8/16;
	//顺时针
	StepperMotor.DIR=clockwise; 
	StepperMotor.FRE = 0;
}
//电机引脚配置
void Motor_GPIO_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO , ENABLE);
	GPIO_PinRemapConfig(GPIO_FullRemap_TIM2 , ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);
		
	//PWM输出引脚
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	//电机方向引脚
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

