#include "Sys.h"
#include "Delay.h"
#include "Led.h"
#include "usart.h"
int main(){
	delay_init();
	LED_Init();
	usartInit(9600);
	while(1){
		LED1 = ~LED1;
		LED2 = ~LED2;
		delay_ms(1000);
	}
}