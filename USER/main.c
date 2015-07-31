#include "Sys.h"
#include "Delay.h"
#include "Led.h"
#include "usart.h"
#include "Key.h"
#include "TIM2_CH4_PWM.h"
#include "StepperMotor.h"
#include "FreePole.h"
#include "string.h"
#include <stdlib.h>
#include "MINI256Z.h"
#include "TIM4.h"
#include "PID.h"
#include "MYDMA.h"
int main(){
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	usartParamsInit();
	usartConfigInit(115200);
	usartParamsInit();
	delay_init();
	LED_Init();
	keyInit();
	TIM2_Configuration();
	FreePoleParamsInit();	
	Motor_GPIO_Configuration();
	MotorParamsInit();
	TIM4_Configuration();
	//MINI256_EXIT_Config();
	MINI256Z_TIM3_Init();
	MINI256_GPIO_Config();
	MYDMA_Config(DMA1_Channel4,(u32)&USART1->DR,(u32)SendBuff,DMA_DataLen);//DMA1通道4，外设串口1，存储器为SendBuff   
	while(1){
		while(Usart_Commod_Flag == 0x20){
			LED2 = 0;
			USART_DMA1_Send("AP=",Get_Electrical_Position_MINI256Z());
			delay_ms(500);
		}while(Usart_Commod_Flag == 0xff){//空闲状态
				LED2 = 1;
				SET_TIM2_CH4_Fre(0,StepperMotor.DIR);
		}while(Usart_Commod_Flag == 0xfe){
			__set_FAULTMASK(1);//清除所有中断
			NVIC_SystemReset();//stm32软复位
		}
		//此范围内，不要用阻塞的printf影响实时控制，或者其他通信破坏
		while(Usart_Commod_Flag == 0x40){
			LED2 = 0;
			if((TIM4_TASK & 0x01) == 0x01){
				TIM4_TASK = TIM4_TASK&0xfe;
				PID_Control_V0_4();
			}
		}
	}
}


