#include "PID.h"
#include "TIM2_CH4_PWM.h"
#include "StepperMotor.h"
#include "Delay.h"
#include "FreePole.h"
#include "MINI256Z.h"
#include "Led.h"
#include <stdlib.h>
#include "Filter.h"
PIDtypedef pid;//PID算法用的数据结构
PIDtypedef pid4Motor;

void PID_Init(void){
	pid.Angle =pid4Motor.Angle= 0;
	pid.error_d =pid4Motor.error_d= 0;
	pid.error_i =pid4Motor.error_i=0;
	pid.error_p =pid4Motor.error_p= 0;
	pid.Kd =pid4Motor.Kd= 0;
	pid.Ki =pid4Motor.Ki= 0;
	pid.Kp =pid4Motor.Kp= 0;
	pid.lastAngle=pid4Motor.lastAngle= 0;
	pid.sp =512;//以最高点作为期望值
	pid4Motor.sp=0;//以开机时刻作为期望值
	pid.speed =pid4Motor.speed= 0;
	pid.velocity =pid4Motor.velocity= 0;
	
}
//PID 三个参数由 USART设置
char PID_SET_PID_OK  = 0x00;
//200 180 1 平衡时间 14s <90°平衡时间 近8s
//200 200 1 平衡时间 18s <90°平衡实际 10s
//200 220 1 平滑时间 20s <90°平滑时间 近11s
void PID_Control_V0_4(){
		//p i d 同符号
		pid.sp = 512;
		pid.lastAngle = pid.Angle;
		pid.Angle = Get_Electrical_Position_MINI256Z();//使用位置值
		//最高点的正负60度方位内才PID运行
		if(pid.Angle<469 || pid.Angle>555){
			SET_TIM2_CH4_Fre(0,clockwise);
			pid.error_i = 0;
			return;
		}
		pid.velocity = windowSlideFilter(pid.Angle);
		//pid.velocity = pid.Angle - pid.lastAngle;//e(k)-e(k-1)
		//注意 是实时减去期望哦 其他版本有写错的记得改正 没有改正 就是可能导致d为负数的原因了
		pid.error_p = pid.Angle - pid.sp ;//e(K) 比例向 
		pid.error_i += pid.error_p;//积分  不要所有的常数
		pid.error_d = pid.velocity;//微分		不要所有的常数

		pid.speed = (pid.Kp * pid.error_p) 
								+ 
								(pid.Ki * pid.error_i) 
								+ 
								(pid.Kd* pid.error_d);

		
		if((pid.speed <= 0) && (pid.speed>=-3000)){
			SET_TIM2_CH4_Fre(abs(pid.speed),clockwise);//频率没有正负的 要取绝对值
		}else if((0 < pid.speed) && (pid.speed<= 3000)){
			SET_TIM2_CH4_Fre(pid.speed,anticlockwise);
		}
		else if(pid.speed<-3000){
				pid.speed = -3000;
				pid.error_i = 0;
				SET_TIM2_CH4_Fre(3000,clockwise);//频率没有正负的 要取绝对值
		}else if(pid.speed> 3000){
				pid.speed = 3000;
				pid.error_i = 0;
				SET_TIM2_CH4_Fre(3000,anticlockwise);//频率没有正负的 要取绝对值
		}
}
//电机位置PID控制 用于定住电机的位置
//需要在串口加期望值 才能运动
void PID_Motor_Position_Control(){
	//p i d 同符号
		pid4Motor.lastAngle = pid4Motor.Angle;
		pid4Motor.Angle = StepperMotor.ActualPulseNum;//使用位置值
		//最高点的正负60度方位内才PID运行
		//pid4Motor.velocity = windowSlideFilter(pid4Motor.Angle);
		pid4Motor.velocity = pid4Motor.Angle - pid4Motor.lastAngle;//e(k)-e(k-1)
		//注意 是实时减去期望哦 其他版本有写错的记得改正 没有改正 就是可能导致d为负数的原因了
		pid4Motor.error_p = pid4Motor.Angle - pid4Motor.sp ;//e(K) 比例向 
		pid4Motor.error_i += pid4Motor.error_p;//积分  不要所有的常数
		pid4Motor.error_d = pid4Motor.velocity;//微分		不要所有的常数
		
	
		pid4Motor.speed = (pid4Motor.Kp * pid4Motor.error_p) 
											+ 
											(pid4Motor.Ki * pid4Motor.error_i) 
											+ 
											(pid4Motor.Kd* pid4Motor.error_d);

		
		if((pid4Motor.speed <= 0) && (pid4Motor.speed>=-3000)){
			SET_TIM2_CH4_Fre(abs(pid4Motor.speed),clockwise);//频率没有正负的 要取绝对值
		}else if((0 < pid4Motor.speed) && (pid4Motor.speed<= 3000)){
			SET_TIM2_CH4_Fre(pid4Motor.speed,anticlockwise);
		}
		else if(pid4Motor.speed<-3000){
				pid4Motor.speed = -3000;
				pid4Motor.error_i = 0;
				SET_TIM2_CH4_Fre(3000,clockwise);//频率没有正负的 要取绝对值
		}else if(pid4Motor.speed> 3000){
				pid4Motor.speed = 3000;
				pid4Motor.error_i = 0;
				SET_TIM2_CH4_Fre(3000,anticlockwise);//频率没有正负的 要取绝对值
		}

}


