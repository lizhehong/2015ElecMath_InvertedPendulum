#include "StepperMotor.h"
#include "string.h"
#include <stdlib.h>
#include "TIM2_CH4_PWM.h"
#include "Led.h"
//步进电机声明
MOTORtypedef StepperMotor;



void MotorParamsInit(){
	//这里多出的StepAngle[0] 只是为了设置方便， 实际没有任何作用
	float StepAngle[5]={0,1.8,0.9,0.45,0.225};
	StepperMotor.subdivide = 1;
	//依据细分读书设置 电机步转角
	StepperMotor.StepAngle = StepAngle[StepperMotor.subdivide];
	//顺时针
	StepperMotor.DIR=clockwise; 
	StepperMotor.FRE = 0;
	StepperMotor.PULSE_ACTUAL = 0;
	StepperMotor.PULSE_SETPOINT = 0;
}

//通过串口设置电机的参数
void getMotorParams4Usart(){
	u8 t;
	u8 len;
	if(TMP_Flag_End ==3 && TMP_Flag_Head == 1){
		LED1 = 1;
		//USART_SendData(USART1,0x99);
		len = TMP_STA-2; //去掉0x90 数据头和命令头 剩下的内容的长度
		//printf("len = %i \n",len);
		memset(StepperMotor.USART_DATA,0,15*sizeof(char));
		if(len < 15){
			for(t=0;t<len;t++){
				StepperMotor.USART_DATA[t] = USART_RX_BUF[t+2];//除去命令头和第二位的命令位
				USART_SendData(USART1,StepperMotor.USART_DATA[t]);
				}
				TMP_STA = 0;
				TMP_Flag_End = 0;
				USART_SendData(USART1,USART_RX_BUF[1]);
		}
		switch(USART_RX_BUF[1]){//判断是什么命令的数值
			case USART_TB6560_DIR:			StepperMotor.DIR = atoi(StepperMotor.USART_DATA);
																	SET_TIM2_CH4_Fre_AND_PULSENUM(StepperMotor.FRE,StepperMotor.PULSE_SETPOINT,StepperMotor.DIR);
																	USART_SendData(USART1,StepperMotor.DIR);
																	break;
			case USARt_TB6560_FRE:			StepperMotor.FRE = atoi(StepperMotor.USART_DATA);
																	SET_TIM2_CH4_Fre_AND_PULSENUM(StepperMotor.FRE,StepperMotor.PULSE_SETPOINT,StepperMotor.DIR);
																	USART_SendData(USART1,StepperMotor.FRE );
																	break;
			case USART_TB6560_PULSENUM:	StepperMotor.PULSE_SETPOINT = atoi(StepperMotor.USART_DATA);
																	SET_TIM2_CH4_Fre_AND_PULSENUM(StepperMotor.FRE,StepperMotor.PULSE_SETPOINT,StepperMotor.DIR);
																	USART_SendData(USART1,StepperMotor.DIR);
																	break;
		}
		
	}
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

