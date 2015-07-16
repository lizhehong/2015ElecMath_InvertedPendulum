#include "Sys.h"
#include "Delay.h"
#include "Led.h"
#include "usart.h"
#include "Key.h"
#include "TIM2_CH4_PWM.h"
#include "StepperMotor.h"
#include "FreePole.h"
int main(){
	uint8_t Key;
	usartInit(9600);
	delay_init();
	LED_Init();
	keyInit();
	TIM2_Configuration(); 
	StepperMotorInit();
	FreePoleParamsInit();
	while(1){
		
		Key = KEY_Scan();
		if(Key == 1)
		LED1 = ~LED1;
		if(Key == 2)
		LED2 = ~LED2;
	}
}


