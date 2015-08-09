#ifndef __MYDMA_H
#define	__MYDMA_H	
#include "stm32f10x.h"

#define DMA_DataLen 8


extern u8 SendBuff[DMA_DataLen];

void MYDMA_Config(DMA_Channel_TypeDef*DMA_CHx,u32 cpar,u32 cmar,u16 cndtr);//配置DMA1_CHx

void MYDMA_Enable(DMA_Channel_TypeDef*DMA_CHx);//使能DMA1_CHx

void USART_DMA1_Send(char *inf,u32 dat);

void DMA_Data_Listen(void);
		  
#endif

