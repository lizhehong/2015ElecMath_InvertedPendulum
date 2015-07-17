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
int main(){

	uint8_t Key;
	usartParamsInit();
	usartConfigInit(9600);
	usartParamsInit();//串口用到的命令头
	delay_init();
	LED_Init();
	keyInit();
	TIM2_Configuration();
	FreePoleParamsInit();	
	Motor_GPIO_Configuration();
	MotorParamsInit();
	while(1){

		LED2 = ~LED2;
	}
}


