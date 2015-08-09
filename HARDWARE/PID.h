#ifndef __PID_H
#define __PID_H
#include "stm32f10x.h"

#define USART_Commod_PID 0xfe
#define USART_Commod_PID_SET_P 0xfd
#define USART_Commod_PID_SET_i 0xfc
#define USART_Commod_PID_SET_d 0xfb
#define USART_Commod_PID_SET_P_Motor 0xf9
#define USART_Commod_PID_SET_I_Motor 0xf8
#define USART_Commod_PID_SET_D_Motor 0xf7
#define	USART_Commod_PID_SET_SP_Motor 0xf6
#define PID_k 2
#define IMU_UPDATE_DT 100

typedef struct{
	float Kp;
	float Ki;
	float Kd; //应该和pid.Kp一个数据级的
	          //先调d，p和i=0，然后不停的加大参数
					  //直到松手杆会抖动，然后就取一半的参数
	double velocity;
	double error_p;
	double error_d;
	double error_i;
	double sp;//期望值
	double speed;
	double lastAngle;
	double Angle;
}PIDtypedef;
extern PIDtypedef pid;
extern char PID_SET_PID_OK ;
extern PIDtypedef pid4Motor;
void PID_Control_V0_4(void);
void PID_Init(void);
void PID_Motor_Position_Control(void);
#endif
