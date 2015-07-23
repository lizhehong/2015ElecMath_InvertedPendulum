#include "PID.h"
#include "TIM2_CH4_PWM.h"
#include "StepperMotor.h"
#include "Delay.h"
#include "FreePole.h"
#include "MINI256Z.h"
#include "Led.h"
#include <stdlib.h>
PIDtypedef pid;//老一种的算法用的数据结构
PIDINCREMENTtypedef pid2;//增量型
//这个是依据群兄说的 
//但是有个疑问 明明返回的是角度 ，但是为什么调节的是频率呢
//不是应该对角度修正吗
//所以自己想吧这个思想写在V0_1上
//特此说明
//pid控制的函数 V 为版本
//第一位数字为大版本
//第二位数字为大版本下的修改
//第一位不同 说明是思想上的分歧
void PID_Control_V0_0(){
		pid.Kp =-500;//只有它 其他两为零时 自由杆从180°下来的方向要与电机的运动方向一致 负的
			pid.Ki = 0;
			pid.Kd = 600;
			pid.sp = 180;//只有它 其他两为零时 自由杆从180°下来的方向要与电机的运动方向一致 正的
			pid.lastAngle = pid.Angle;
			pid.Angle = Get_Electrical_Angle_MINI256Z();
			pid.velocity = pid.Angle - pid.lastAngle;
			pid.error_p = pid.sp - pid.Angle;
			pid.error_d = pid.velocity;//我这里直接给角度差
			pid.error_i = pid.error_i + (pid.error_p*0.003);//这里是乘以延时的时间
			pid.speed = -(pid.Kp * pid.error_p 
										+ 
										pid.Ki * pid.error_i 
										+ 
										pid.Kd* pid.error_d);
			printf("%f\n",pid.speed);
			if(pid.speed >=0){
				pid.speed = abs(pid.speed);
				SET_TIM2_CH4_Fre_AND_PULSENUM(pid.speed,0.003/pid.speed,clockwise);
				}else{
				pid.speed = abs(pid.speed);
				SET_TIM2_CH4_Fre_AND_PULSENUM(pid.speed,0.003/pid.speed,anticlockwise);
			}
			delay_ms(3);
}
//PIDINCREMENTtypedef pid2; 使用这个结构 增量型pid的结构
//电机的可控频率 应该在负载确定下 测试出来 稳定的工作频率范围(定下是 编码器部分是否稳定和工作时是够稳定)
//然后依据所要的角度 算出在一个pid运行周期下 需要的频率(确定与采样周期内顺利完成)
//如果不算出本次位置累加的pid需要修复的角度所要用的时间，也就是有可能
//当次的累加角度 可能被打断 就是没完成的时候，另一个pid来了
//或者在采样时间内完成了 那就可能失去动力 没有控制 增加了系统的稳定性
//按照上面的说法 在理想状态下 除了系统计算误差
//每次的迭代误差 只要是在规定的采样频率下 能完成的 都能修复角度 而且
//不会做到 系统毫无输出 而由重力主导系统
//但是为了测试的方便写成了V1_0 只有定频
//V1_1变频
//而且应该引起注意的是 编码器算出的是自由杆的角度 和角度差
//而控制的却是电机的运行角度 所以
//电机的运行角度 与自由杆的运行角度的关系 应该折算出来
void PID_Control_V1_0(){
	double iError = 0.0;//当前误差
	double iincpid = 0.0;
	double ActualAngle = 0.0;
	double TmpTime = 0.0;
	//Kp,Ki,Kd三个数的正负 与 摇杆倒下的方向要一致
	pid2.Kp = 1;
	pid2.Ki = 0.015;
	pid2.Kd = -0.2;
	pid2.sp = 180.3515625;
	ActualAngle = Get_Electrical_Angle_MINI256Z();
	if(ActualAngle<165 || ActualAngle>195){
		return;
	}
	//计算当前误差
	iError = pid2.sp - ActualAngle;
	//计算出pid的数
	iincpid = pid2.Kp*(iError-pid2.last_error)
						+
						pid2.Ki*(iError)
						+
						pid2.Kd*(iError-2*pid2.last_error+pid2.prev_error);
	pid2.prev_error = pid2.last_error;
	pid2.last_error = iError;
	
	//pid处理
	pid2.sum_error += iincpid;//计算增量并累加 
		
	
	//频率只需要考虑最大稳定就行
	//这里我需要控制的是角度
	//所以我只要确定在下一个执行之前完成对应的角度就可以了
	if(pid2.sum_error>=0){//需要修正的角度是正的，当然我需要顺时针转
		//pid2.sum_error = pid2.sum_error/0.1125;
		SET_TIM2_CH4_Fre_AND_PULSENUM(abs(pid2.sum_error)/0.003,abs(pid2.sum_error),clockwise);
		//printf("pid2.sum_error=%f ActualAngle=%f \n",pid2.sum_error,ActualAngle);
	}
	else{//需要修正的角度是负的，当然我需要逆时针转
		//pid2.sum_error = pid2.sum_error/0.1125;
		SET_TIM2_CH4_Fre_AND_PULSENUM(abs(pid2.sum_error)/0.003,abs(pid2.sum_error),anticlockwise);
		//printf("pid2.sum_error=%f ActualAngle=%f \n",pid2.sum_error,ActualAngle);
	}
	delay_ms(2);
}
