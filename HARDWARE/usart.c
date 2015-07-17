#include "usart.h"
#include "StepperMotor.h"
#include "string.h"
#include <stdlib.h>
#include "Led.h"
#include "TIM2_CH4_PWM.h"
#define USART_REC_LEN  			200  //串口缓存器的长度
//串口配置初始化
void usartConfigInit(u32 bound){
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	
 	USART_DeInit(USART1);  
 //USART1_TX   PA.9
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure); 
 
	//USART1_RX	  PA.10
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//串口配置
	USART_InitStructure.USART_BaudRate = bound;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	

  USART_Init(USART1, &USART_InitStructure); 
 
 //中断配置
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			
	NVIC_Init(&NVIC_InitStructure);	
   
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

  USART_Cmd(USART1, ENABLE); 
}
//这里的数组长度请保持与初始化长度的一致
#define usartHeadLen 2
u8 usartHead[usartHeadLen];
void usartParamsInit(){
	usartHead[0] = 0x90;//用于电机控制的串口命令头
	usartHead[1] = 0x70;//用于HMI串口屏幕的命令头
}
//串口中断函数 需要的数据结构定义
u8 TMP_Flag_End = 0;
u8 TMP_Flag_Head = 0;
u16 TMP_STA = 0 ;
u8 USART_RX_BUF[USART_REC_LEN];  
//串口中断函数
void USART1_IRQHandler(){
	u8 t;
	u8 len;
	u8 Res,i;
	//--------------------串口接受数据-------------------------------------------------
	if(USART_GetFlagStatus(USART1,USART_IT_RXNE)!=RESET){
		//获取系统串口数据
		Res =USART_ReceiveData(USART1);
		//得到数据尾
		if(Res == 0xff){
			TMP_Flag_End++;
		}
		//得到数据头 
		//采用轮训的方式 但是必须在初始化的时候 初始命令头的值
		for(i=0;i<usartHeadLen;i++){
			if(Res == usartHead[i]){
				TMP_Flag_Head = 1;
				TMP_Flag_End = 0;
			}
		}
		//得到数据头 并且没有 得到数据尾时 这里的缓冲区 
		//包含命令头
		if(TMP_Flag_End == 0 && TMP_Flag_Head ==1){
			//串口数据进入缓存区
			USART_RX_BUF[TMP_STA]=Res ;	
			//缓存区指针叠加
			TMP_STA ++;
			//判断指针溢出
			if(TMP_STA > (USART_REC_LEN -1)){
				TMP_Flag_Head = TMP_Flag_End = TMP_STA = 0;
			}
		}	
		//----------------电机数据操作---------------------------------------------
			//电机参数设置
		if(TMP_Flag_End ==3 && TMP_Flag_Head == 1){
				//去掉0x90 数据头和命令头 剩下的内容的长度
				len = TMP_STA-2; 
				//清空电机的串口缓存区
				memset(StepperMotor.USART_DATA,0,15*sizeof(char));
				if(len < 15){
					for(t=0;t<len;t++){
						//除去命令头和第二位的命令位
						StepperMotor.USART_DATA[t] = USART_RX_BUF[t+2];
						}
						//收到了命令 肯定清除标志位啦
						TMP_STA = 0;
						TMP_Flag_End = TMP_Flag_Head = 0;
						//LED1 = ~LED1;
					switch(USART_RX_BUF[1]){//判断是什么命令的数值
						case USART_TB6560_DIR:			StepperMotor.DIR = atoi(StepperMotor.USART_DATA);
																				SET_TIM2_CH4_Fre_AND_PULSENUM(StepperMotor.FRE,StepperMotor.PULSE_SETPOINT,StepperMotor.DIR);
																				USART_SendData(USART1,StepperMotor.DIR); 																				
																				break;
						case USARt_TB6560_FRE:			StepperMotor.FRE = atoi(StepperMotor.USART_DATA);
																				SET_TIM2_CH4_Fre_AND_PULSENUM(StepperMotor.FRE,StepperMotor.PULSE_SETPOINT,StepperMotor.DIR);
																				USART_SendData(USART1,StepperMotor.FRE);
																				break;
						case USART_TB6560_PULSENUM:	StepperMotor.PULSE_SETPOINT = atoi(StepperMotor.USART_DATA);
																				SET_TIM2_CH4_Fre_AND_PULSENUM(StepperMotor.FRE,StepperMotor.PULSE_SETPOINT,StepperMotor.DIR);
																				USART_SendData(USART1,StepperMotor.PULSE_SETPOINT);
																				break;
						default: break;
					}
				}
			}
		USART_ClearITPendingBit(USART1,USART_IT_RXNE);        //清除中断标志
	}

}
//用于printf的时候
int fputc(int ch,FILE *f){
	USART_SendData(USART1,(u8) ch);
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC) == RESET);
	return ch;
		
}
//当使用scanf的时候
int fgetc(FILE *f){
	while(!(USART_GetFlagStatus(USART1,USART_FLAG_RXNE) == SET));
	return (USART_ReceiveData(USART1));
}
