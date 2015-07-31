#ifndef __USART_H
#define __USART_H
#include "stm32f10x.h"
#include "stdio.h"	

#define USART_Commod_STOP 0x91
#define USART_Commod_Sys_Reset 0xfe
#define USART_REC_LEN  			200  

extern u8 TMP_Flag_End ;
extern u8 TMP_Flag_Head;
extern u16 TMP_STA ;
extern u8 USART_RX_BUF[USART_REC_LEN]; 
extern int Usart_Commod_Flag; //用于得到电脑需要返回角度的命令

void usartConfigInit(u32 bound);
void usartParamsInit(void);
void USART1_IRQHandler(void);
void USART_Send_Enter(void);
void USART_Put_Inf(char *inf,u32 dat);
void USART_Put_Num(u32 dat);
void u32tostr(u32 dat,char *str); 
void USART_Send_Str(const char* data);
#endif



