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
		//printf("FreePole.AngleSpeed =%f \n",FreePole.AngleSpeed );
		LED2 = ~LED2;
		while(Usart_Commod_Flag == 0x08){
			printf("%.3f\n",FreePole.AngleSpeed );
			delay_ms(5);
		}
		while(Usart_Commod_Flag == 0x10){
			printf("%.3f\n",FreePole.AcutalAngle );
			delay_ms(5);
		}
		while(Usart_Commod_Flag == 0x20){
			printf("%i\n",TIM_GetCounter(TIM3));
			delay_ms(5);
		}
	}
}


