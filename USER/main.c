#include "Sys.h"
#include "Delay.h"
#include "Led.h"
#include "usart.h"
#include "Key.h"
int main(){
	uint8_t Key;
	delay_init();
	LED_Init();
	usartInit(9600);
	keyInit();
	while(1){
		
		Key = KEY_Scan();
		if(Key == 1)
		LED1 = ~LED1;
		if(Key == 2)
		LED2 = ~LED2;
	}
}


