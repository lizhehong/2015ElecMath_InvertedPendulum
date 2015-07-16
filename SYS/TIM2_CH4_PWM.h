#ifndef __TIM2_CH4_PWM_H
#define __TIM2_CH4_PWM_H
#include "stm32f10x.h"
#include "Sys.h"

//定时器2PWM设置频率和脉冲数
void SET_TIM2_CH4_Fre_AND_PULSENUM(uint16_t fre,uint16_t  pulseNum,char dir);
//定时器2配置 PWM
void TIM2_Configuration(void);

#endif