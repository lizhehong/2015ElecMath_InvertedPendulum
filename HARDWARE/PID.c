#include "PID.h"
#include "TIM2_CH4_PWM.h"
#include "StepperMotor.h"
#include "Delay.h"
#include "FreePole.h"
PIDtypedef pid;

//通过角度的位置判断 来进行步进电机的速度的执行
//实测 按照2015年/7/18 电机根据负载可调频率为 130HZ-180HZ
//电机工作音质稳定 振动少
//所以定速为 130HZ 最低速 180HZ最高速
//PID 其实就是 三次的误差即可
//e(K) e(K-1) e(K-2)
void PID_Control(){
	
}