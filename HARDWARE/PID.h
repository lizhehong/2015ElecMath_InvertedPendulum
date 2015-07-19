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

extern PIDtypedef pid;
void PID_Control(void);


#endif