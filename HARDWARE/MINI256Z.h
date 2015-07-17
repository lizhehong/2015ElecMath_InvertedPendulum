#ifndef __MINI256Z_H
#define __MINI256Z_H

#define ENCODER_PPR           (u16)(256)   // number of pulses per revolution
#define SPEED_BUFFER_SIZE 8

#define COUNTER_RESET   (u16)0
#define ICx_FILTER      (u8) 6 // 6<-> 670nsec

#define TIMx_SUB_PRIORITY 0

#define SPEED_SAMPLING_FREQ (u16)(2000/(SPEED_SAMPLING_TIME+1)) //抽样频率
#define U16_MAX    ((u16)65535u) 
#define S16_MAX    ((s16)32767) 
#define U32_MAX    ((u32)4294967295uL) 
#define SPEED_SAMPLING_TIME  9     // (9+1)*500usec = 10msec 抽样时间 两个脉冲对应点的时间间隔



void MINI256_GPIO_Config(void);
void MINI256Z_TIM3_Init(void);
void calcAngleSpeed_MINI256(int count,double UnitTime);
double getAngleMaxpointFlag_MINI256(double SetpointAngle);
void TIM3_IRQHandler(void);
#endif
