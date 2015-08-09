#ifndef __StepperMotor_H
#define __StepperMotor_H
#include "stm32f10x.h"
#include "usart.h"

#define  clockwise 1	//顺时针
#define  anticlockwise 0 	//逆时针
#define USART_TB6560_DIR 0x99 //串口命令中的 方向
#define USART_TB6560_FRE 0x97 //串口命令中的 电机工作频率
#define USART_TB6560_STOP 0x89	//电机停止工作
#define USART_Commod_TB6560_PULSENUM 0x93 //电机脉冲数
#define USART_Commod_Motor_Position_Contrl 0x9a
#define MAX_Motor_Fre 180 //电机不失步最高频率
#define MIN_Motor_Fre 130 //电机不失步最低频率

typedef struct{
	unsigned short int PULSE_SETPOINT ;//期望的脉冲数
	unsigned short int DIR;//运动方向
	unsigned int FRE;//工作频率HZ
	char USART_DATA[15];//串口对应的电机有效数据缓存
	float StepAngle;//电机步转角
	unsigned char subdivide; //电机的细分数 整部 半步 1/8 1/16
	int ActualPulseNum;//得到的实时脉冲数
}MOTORtypedef;

extern MOTORtypedef StepperMotor;


//初始化电机参数
void MotorParamsInit(void);
//初始化电机引脚
void Motor_GPIO_Configuration(void);
#endif
