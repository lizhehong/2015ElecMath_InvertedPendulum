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
		//delay_ms(10);
	}
}


