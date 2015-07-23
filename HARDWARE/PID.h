#ifndef __PID_H
#define __PID_H

#define USART_Commod_PID 0x93
#define PID_k 2
#define IMU_UPDATE_DT 100

typedef struct{
	float Kp;
	float Ki;
	float Kd;
	double velocity;
	double error_p;
	double error_d;
	double error_i;
	double sp;//期望值
	double speed;
	double lastAngle;
	double Angle;
}PIDtypedef;

typedef struct{
	double Kp;
	double Ki;
	double Kd;
	double last_error;//e[-1]
	double prev_error;//e[-2]
	double sum_error;//误差累计
	double sp;
}PIDINCREMENTtypedef;
extern PIDtypedef pid;
extern PIDINCREMENTtypedef pid2;
void PID_Control_V0_0(void);
void PID_Control_V1_0(void);


#endif