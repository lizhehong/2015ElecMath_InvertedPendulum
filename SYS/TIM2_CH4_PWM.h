#ifndef __TIM2_CH4_PWM_H
#define __TIM2_CH4_PWM_H
#include "stm32f10x.h"
#include "Sys.h"

#define USART_MINI256Z_AngleSpeed 0x96
#define USART_MINI256Z_ActualPosition 0x94

//定时器2PWM设置频率和脉冲数
void SET_TIM2_CH4_Fre(uint16_t fre,char dir);
//定时器2配置 PWM
void TIM2_Configuration(void);
//TIM2中断函数
void TIM2_IRQHandler(void);
#endif

