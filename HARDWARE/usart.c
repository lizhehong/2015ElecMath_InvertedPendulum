#include "usart.h"
#include "StepperMotor.h"
#include "string.h"
#include <stdlib.h>
#include "Led.h"
#include "TIM2_CH4_PWM.h"
#include "FreePole.h"
#include "PID.h"
#include "MINI256Z.h"
#include "MYDMA.h"
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
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1 ;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			
	NVIC_Init(&NVIC_InitStructure);	
   
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

  USART_Cmd(USART1, ENABLE); 
}
//这里的数组长度请保持与初始化长度的一致
#define usartHeadLen 4
u8 usartHead[usartHeadLen];
void usartParamsInit(){
	usartHead[0] = 0x90;//用于设置电机控制的串口命令头
	usartHead[1] = 0x70;//用于HMI串口屏幕的命令头
	usartHead[2] = 0x80;//得到PID数据的命令头
	usartHead[3] = 0x91;//stm32本身命令
}
//串口中断函数 需要的数据结构定义
u8 TMP_Flag_End = 0;
u8 TMP_Flag_Head = 0;
u16 TMP_STA = 0 ;
u8 USART_RX_BUF[USART_REC_LEN];  
int Usart_Commod_Flag = 0;
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
					if(USART_RX_BUF[0] == usartHead[0]){//判断命令归属 电机归属
						switch(USART_RX_BUF[1]){//判断是什么命令的数值
							case USART_TB6560_DIR:			StepperMotor.DIR = atoi(StepperMotor.USART_DATA);
																					SET_TIM2_CH4_Fre(StepperMotor.FRE,StepperMotor.DIR);
																					//USART_SendData(USART1,0x99);
																					break;
							case USART_TB6560_STOP:			TIM_SetCompare4(TIM2,0);//占空比总是占据0%
																					break;
							case USART_TB6560_FRE:			StepperMotor.FRE = atoi(StepperMotor.USART_DATA);
																					SET_TIM2_CH4_Fre(StepperMotor.FRE,StepperMotor.DIR);
																					//USART_SendData(USART1,0x98);	
																					break;
							case USART_MINI256Z_ActualPosition:
																					Usart_Commod_Flag = 0x20;//可以读自由杆 的实时位置
																					break;
							case USART_Commod_STOP:
																					Usart_Commod_Flag = 0xff;//空闲状态
																					break;
							default: break;
						}
					}else if(USART_RX_BUF[0] == usartHead[2]){
						switch(USART_RX_BUF[1]){
							case USART_Commod_PID: 			if(PID_SET_PID_OK == 0x07){
																						Usart_Commod_Flag = 0x40;//可以进行PID运算
																						PID_SET_PID_OK = 0;
																					}	
																					break;
							case USART_Commod_PID_SET_P: 
																					pid.Kp = 	atof(StepperMotor.USART_DATA);
																					PID_SET_PID_OK |= 0x01;
																					break;
							case USART_Commod_PID_SET_i: 
																					pid.Ki = 	atof(StepperMotor.USART_DATA);
																					PID_SET_PID_OK |= 0x02;
																					break;
							case USART_Commod_PID_SET_d: 
																					pid.Kd =  atof(StepperMotor.USART_DATA);
																					PID_SET_PID_OK |= 0x04;
																					break;
							default: break;
						}
					}else if(USART_RX_BUF[0] == usartHead[3]){//stm32 本身
						switch(USART_RX_BUF[1]){
						case USART_Commod_Sys_Reset:
																					Usart_Commod_Flag = 0xfe;
																					break;
						default: break;
						}
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
//串口发送一个字符串
void USART1_Send_Byte(u16 Data)
{ 
   	while (!(USART1->SR & USART_FLAG_TXE));
   	USART1->DR = (Data & (uint16_t)0x01FF);	   
}
//串口发送字符串
//串口发送 0d 0a 即回车换行
void USART_Send_Enter(void)
{
	USART1_Send_Byte(0x0d);
	USART1_Send_Byte(0x0a);
}
//串口发送字符串
void USART_Send_Str(const char* data)
{
	u16 i;
	u16 len = strlen(data)-1;
	for (i=0; i<len; i++)
	{
		USART1_Send_Byte(data[i]);
	}
	if(data[i]=='\n') 
	{
		USART_Send_Enter();
	}
	else
	{
		USART1_Send_Byte(data[i]);
	}		
}
//将一个32位的变量dat转为字符串
//比如1234 转为"1234"
//dat:带转的long型变量
//str:指向字符数组的指针，转换后的字符串放在其中
void u32tostr(u32 dat,char *str) 
{
 	char temp[20];
 	u8 i=0,j=0;
 	i=0;
 	while(dat)
 	{
  		temp[i]=dat%10+0x30;
  		i++;
  		dat/=10;
 	}
 	j=i;
 	for(i=0;i<j;i++)
 	{
  		str[i]=temp[j-i-1];
 	}
 	if(!i) {str[i++]='0';}
 	str[i]=0;
}
//串口发送数值
void USART_Put_Num(u32 dat)
{
 	char temp[20];
 	u32tostr(dat,temp);
 	USART_Send_Str(temp);
}
//串口发送调试信息
void USART_Put_Inf(char *inf,u32 dat)
{
 	USART_Send_Str(inf);
 	USART_Put_Num(dat);
 	USART_Send_Str("\n");  
}
