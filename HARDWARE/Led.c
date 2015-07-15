#include "led.h"

//初始化PA2 和 PA3作为LED的输出端口
void LED_Init(void)
{
 
		GPIO_InitTypeDef  GPIO_InitStructure;

		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	 //Ê¹ÄÜPB,PE¶Ë¿ÚÊ±ÖÓ

		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;		//Í¨ÓÃÍÆÍìÊä³öÄ£Ê½
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		//Êä³ö×î´óÆµÂÊÎª50MHz
		GPIO_Init(GPIOA, &GPIO_InitStructure);					//³õÊ¼»¯PA2¡¢PA3
}

