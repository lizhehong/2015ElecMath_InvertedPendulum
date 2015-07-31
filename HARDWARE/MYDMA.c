#include "MYDMA.h"
#include "stdio.h"
#include "string.h"
DMA_InitTypeDef DMA_InitStructure;
u8 SendBuff[DMA_DataLen];
u16 DMA1_MEM_LEN;   

void MYDMA_Config(DMA_Channel_TypeDef* DMA_CHx,u32 cpar,u32 cmar,u16 cndtr)
{
 	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);	
	
  DMA_DeInit(DMA_CHx);  
	DMA1_MEM_LEN=cndtr;
	DMA_InitStructure.DMA_PeripheralBaseAddr = cpar;  
	DMA_InitStructure.DMA_MemoryBaseAddr = cmar; 
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;  
	DMA_InitStructure.DMA_BufferSize = cndtr;  
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; 
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; 
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;  
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium; 
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  
	DMA_Init(DMA_CHx, &DMA_InitStructure);  
	  	
} 
//开启一次DMA传输
void MYDMA_Enable(DMA_Channel_TypeDef*DMA_CHx)
{ 
	DMA_Cmd(DMA_CHx, DISABLE );    
 	DMA_SetCurrDataCounter(DMA1_Channel4,DMA1_MEM_LEN);
 	DMA_Cmd(DMA_CHx, ENABLE);  
}	  
//param 1 需要补充的文字，param2 数据，param3 偏移地址
u16 DMABuffcount = 0;//该缓存区存储范围的累计数量
char DMA_GO = 0;//DMA是否正在发送
void USART_DMA1_Send(char *inf,u32 dat){
		char str[20];
		int i = 0,len,j;	
		//除了缓冲区没填满 并且 DMA传输结束 才进行填充缓冲区
		if((DMABuffcount < DMA_DataLen) && (DMA_GO == 0)){
			len =  sprintf(str,"%s%4d\n",inf,dat);// 得到总字节
			for(i=0,j=DMABuffcount;i<len;i++,j++)
				SendBuff[j] = str[i];
			DMABuffcount+=len;
		}
		
}
//希望能放于定时器中断中执行
void DMA_Data_Listen(){
	if((DMABuffcount >= DMA_DataLen) && (DMA_GO == 0)){
			USART_DMACmd(USART1,USART_DMAReq_Tx,ENABLE); //使能串口1的DMA发送
			MYDMA_Enable(DMA1_Channel4);//开始一次DMA传输
			DMA_GO = 1;//DMA正在发送中
		}
	if(DMA_GetFlagStatus(DMA1_FLAG_TC4)!=RESET){	//查看DMA1 串口是否发送完毕
				DMABuffcount = 0;
				DMA_GO = 0;//可以再次进行发送
				memset(SendBuff, 0x0, DMA_DataLen);
				DMA_ClearFlag(DMA1_FLAG_TC4);//清除中断位
		}
}
