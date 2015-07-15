#include "usart.h"
#include "Delay.h"
#include "Sys.h"
#include "Led.h"
 int main(void)
 {
	 delay_init(); 
	 usartInit(9600);
	 LED_Init();
	 while(1){
			LED1 = ~LED1;
			delay_ms(1000);
	}
 }

