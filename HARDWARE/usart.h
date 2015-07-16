#ifndef __USART_H
#define __USART_H
#include "stm32f10x.h"
#include "stdio.h"	


#define USART_REC_LEN  			200  

extern u8 TMP_Flag_End ;
extern u8 TMP_Flag_Head;
extern u16 TMP_STA ;
extern u8 USART_RX_BUF[USART_REC_LEN]; 





void usartInit(u32 bound);
void USART_IRQHandler(void);
#endif



