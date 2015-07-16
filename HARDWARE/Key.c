#include "Key.h"
#include "Delay.h"
//键盘初始化
void keyInit(){
	GPIO_InitTypeDef GPIO_InitStructure;
	
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);

	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;			
	GPIO_Init(GPIOA, &GPIO_InitStructure);	
}


uint8_t KEY_Scan(void)
{ 
	static uint8_t KEY_UP=1;	
	if(KEY_UP&&(KEY1==0||KEY2==0))
	{
		delay_ms(10);			
		KEY_UP=0;
		if(KEY1==0)return 1;
		else if(KEY2==0)return 2;
	}else if(KEY1==1&&KEY2==1)KEY_UP=1; 	    
	return 0;					
}
