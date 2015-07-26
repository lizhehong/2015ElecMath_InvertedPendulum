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
int main(){

	uint8_t Key;
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
	MINI256Z_TIM3_Init();
	MINI256_GPIO_Config();
	while(1){
		while(Usart_Commod_Flag == 0x10){
			LED1 = 0;
			printf("AcutalAngle = %f \n",Get_Electrical_Angle_MINI256Z());
			delay_ms(500);
		}while(Usart_Commod_Flag == 0x20){
			LED1 = 0;
			printf("AcutalAngle = %d \n",Get_Electrical_Position_MINI256Z());
			delay_ms(500);
		}while(Usart_Commod_Flag == 0x80){
			LED1 = 0;
			EnviromentTest_ZEROPOINT();
			Usart_Commod_Flag = 0xff;//零点检测完毕 进入空闲状态
		}while(Usart_Commod_Flag == 0xff){//空闲状态
				LED1 = 1;
		}//此范围内，不要用阻塞的printf影响实时控制，或者其他通信破坏
		while(Usart_Commod_Flag == 0x40){
			LED1 = 0;
			if((TIM4_TASK & 0x01) == 0x01){
				TIM4_TASK = TIM4_TASK&0xfe;
				PID_Control_V0_3();
			}
		}
	}
}


