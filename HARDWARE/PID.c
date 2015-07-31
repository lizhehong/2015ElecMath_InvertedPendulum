#include "PID.h"
#include "TIM2_CH4_PWM.h"
#include "StepperMotor.h"
#include "Delay.h"
#include "FreePole.h"
#include "MINI256Z.h"
#include "Led.h"
#include <stdlib.h>
PIDtypedef pid;//PID算法用的数据结构
//PID 三个参数由 USART设置
char PID_SET_PID_OK  = 0x00;
void PID_Control_V0_4(){
		//p i d 同符号
		pid.sp = 512;
		pid.lastAngle = pid.Angle;
		pid.Angle = Get_Electrical_Position_MINI256Z();//使用位置值
		//最高点的正负15度方位内才PID运行
		if(pid.Angle<341 || pid.Angle>683){
			SET_TIM2_CH4_Fre(0,clockwise);
			pid.error_i = 0;
			return;
		}
		pid.velocity = pid.Angle - pid.lastAngle;//e(k)-e(k-1)
		//注意 是实时减去期望哦 其他版本有写错的记得改正 没有改正 就是可能导致d为负数的原因了
		pid.error_p = pid.Angle - pid.sp ;//e(K) 比例向 
		pid.error_i += pid.error_p;//积分  不要所有的常数
		pid.error_d = pid.velocity;//微分		不要所有的常数
	
		pid.speed = (pid.Kp * pid.error_p) 
								+ 
								(pid.Ki * pid.error_i) 
								+ 
								(pid.Kd* pid.error_d);
		//printf("pid.speed = %f \n",pid.speed);
			if(-3000<=pid.speed <= 0){
				SET_TIM2_CH4_Fre(abs(pid.speed),clockwise);//频率没有正负的 要取绝对值
				}else if(0 < pid.speed <= 3000){
				SET_TIM2_CH4_Fre(pid.speed,anticlockwise);
			}
}

