#ifndef __USART_H
#define __USART_H
#include "stm32f10x.h"
#include "stdio.h"	






void usartInit(u32 bound);
void USART_IRQHandler(void);
#endif



