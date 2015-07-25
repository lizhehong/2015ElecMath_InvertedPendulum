#ifndef __USART_H
#define __USART_H
#include "stm32f10x.h"
#include "stdio.h"	

#define USART_Commod_STOP 0x91

#define USART_REC_LEN  			200  

extern u8 TMP_Flag_End ;
extern u8 TMP_Flag_Head;
extern u16 TMP_STA ;
extern u8 USART_RX_BUF[USART_REC_LEN]; 
extern int Usart_Commod_Flag; //用于得到电脑需要返回角度的命令

void usartConfigInit(u32 bound);
void usartParamsInit(void);
void USART_IRQHandler(void);
#endif



