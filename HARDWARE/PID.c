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
			//不要用阻塞的printf影响实时控制
			//printf("%f\n",pid.speed);
			if(pid.speed >=0){
				pid.speed = abs(pid.speed);
				SET_TIM2_CH4_Fre_AND_PULSENUM(pid.speed,0.003/pid.speed,clockwise);
				}else{
				pid.speed = abs(pid.speed);
				SET_TIM2_CH4_Fre_AND_PULSENUM(pid.speed,0.003/pid.speed,anticlockwise);
			}
			delay_ms(3);
}
//由版本1 升级 在积分相乘以积分时间
//位置式
void PID_Control_V0_1(){
		int T = 3;//ms
		pid.Kp =1;//4 只有它 其他两为零时 自由杆从180°下来的方向要与电机的运动方向一致 负的
		pid.Ki = 0.001;//加大运动的趋势
		pid.Kd = -0.0001;//-0.0001,越小越能阻碍自转
		pid.sp = 512;//只有它 其他两为零时 自由杆从180°下来的方向要与电机的运动方向一致 正的
		pid.lastAngle = pid.Angle;
		pid.Angle = Get_Electrical_Position_MINI256Z();//使用位置值
		//最高点的正负15度方位内才PID运行
		if(pid.Angle<469 || pid.Angle>555){
			return;
		}
		pid.velocity = pid.Angle - pid.lastAngle;//e(k)-e(k-1)
		pid.error_p = pid.sp - pid.Angle;//e(K) 比例向
		pid.error_i += (pid.error_p*T/1000);//积分 
		pid.error_d = pid.velocity*1000/T;//微分
	
		pid.speed = pid.Kp * pid.error_p 
								+ 
								pid.Ki * pid.error_i 
								+ 
								pid.Kd* pid.error_d;
		//不要用阻塞的printf影响实时控制
		//printf("PulseName = %d fre = %d PID = %f\n",abs(pid.speed *3.125),abs((pid.speed*3.125*1000)/T),pid.speed );
		if(pid.speed >=0){
			//SET_TIM2_CH4_Fre_AND_PULSENUM(abs((pid.speed*3.125*1000)/T),abs(pid.speed *3.125),clockwise);
			SET_TIM2_CH4_Fre_AND_PULSENUM(2000,abs(pid.speed *3.125),clockwise);
			}else{
			//SET_TIM2_CH4_Fre_AND_PULSENUM(abs((pid.speed*3.125*1000)/T),abs(pid.speed *3.125),anticlockwise);
				SET_TIM2_CH4_Fre_AND_PULSENUM(2000,abs(pid.speed *3.125),anticlockwise);
		}
		delay_ms(T);//采样时间
}
//1 0 0.05平衡10s
//1 0 0.1 平衡10s
//1 0 0.5 平衡10s
//1 0 1		平衡10s
//1 0 1.5 平衡6s
//1 0 2   平衡3s
void PID_Control_V0_2(){
		//p i d 同符号
		int T = 3;//ms
		pid.Kp =1;//
		pid.Ki = 0;
		pid.Kd =1.5;//应该和pid.Kp一个数据级的
	                   //先调d，p和i=0，然后不停的加大参数
										//直到松手杆会抖动，然后就取一半的参数
		pid.sp = 512;
		pid.lastAngle = pid.Angle;
		pid.Angle = Get_Electrical_Position_MINI256Z();//使用位置值
		//最高点的正负60度方位内才PID运行
		if(pid.Angle<341 || pid.Angle>683){
			TIM_SetCompare4(TIM2,0);//防止电机因超出角度 还在运行
			pid.speed = 0;//倒下去不做PID 所以清空
			return;
		}
		pid.velocity = pid.Angle - pid.lastAngle;//e(k)-e(k-1)
		//注意 是实时减去期望哦 其他版本有写错的记得改正 没有改正 就是可能导致d为负数的原因了
		pid.error_p = pid.Angle - pid.sp ;//e(K) 比例向 
		pid.error_i += (pid.error_p*T)/1000;//积分 
		pid.error_d = (pid.velocity*1000)/T;//微分
	
		pid.speed = (pid.Kp * pid.error_p) 
								+ 
								(pid.Ki * pid.error_i) 
								+ 
								(pid.Kd* pid.error_d);
		if(pid.speed <= 0){
			SET_TIM2_CH4_Fre_AND_PULSENUM(2000,abs(pid.speed *3.125),clockwise);
			}else{
				SET_TIM2_CH4_Fre_AND_PULSENUM(2000,abs(pid.speed *3.125),anticlockwise);
		}
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
	//Kp,Ki,Kd三个数的正负 与 摇杆倒下的方向要一致 1 0.001 -0.2
	pid2.Kp = 1;
	pid2.Ki = 0.0002;
	pid2.Kd = -0.2;
	pid2.sp = 180;
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
	if(pid2.sum_error<=0){//需要修正的角度是正的，当然我需要顺时针转
		//pid2.sum_error = pid2.sum_error/0.1125;
		SET_TIM2_CH4_Fre_AND_PULSENUM(abs(pid2.sum_error)/0.002,abs(pid2.sum_error),clockwise);
		//printf("pid2.sum_error=%f ActualAngle=%f \n",pid2.sum_error,ActualAngle);
		//printf("abs(pid2.sum_error) = %.0f\n",pid2.sum_error);
	}
	else{//需要修正的角度是负的，当然我需要逆时针转
		//pid2.sum_error = pid2.sum_error/0.1125;
		SET_TIM2_CH4_Fre_AND_PULSENUM(abs(pid2.sum_error)/0.002,abs(pid2.sum_error),anticlockwise);
		//不要用阻塞的printf影响实时控制
		//printf("pid2.sum_error=%f ActualAngle=%f \n",pid2.sum_error,ActualAngle);
		//printf("abs(pid2.sum_error) = %.0f\n",pid2.sum_error);
	}
	delay_ms(2);
}
//用于测试能否将电机的运行角度 则算出编码器的修正角度
//因为V1_0 只是将编码器的返回角度 直接作为步进电机的运行角度
//为了计算方便 直接用位置值计算
void PID_Control_V1_1(){
	double iError = 0.0;//当前误差
	double iincpid = 0.0;
	int ActualPosition = 0.0;
	double TmpTime = 0.0;
	int Delay_Time = 50;
	//Kp,Ki,Kd三个数的正负 与 摇杆倒下的方向要一致 1 0.001 -0.2
	pid2.Kp = 2.5;
	pid2.Ki = 0;
	pid2.Kd = -1;
	pid2.sp = 512;//这里应该为期望的位置值
	ActualPosition = Get_Electrical_Position_MINI256Z();
	//正负43 约等于 15.1171875°
	if(ActualPosition<469 || ActualPosition>555){
		return;
	}
	//计算当前误差
	iError = pid2.sp - ActualPosition;
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
	//1.8/16 = 0.1125 步转矩
	//360/1024 = 0.3515625 编码器度数
	//所以 每一个位置值 对应的脉冲数应该是 3.125
	//不要用阻塞的printf影响实时控制
	//printf("PID Value=%f Fre = %d \n",pid2.sum_error,abs((pid2.sum_error*3.125*1000)/Delay_Time));
	if(pid2.sum_error>=0){	
		SET_TIM2_CH4_Fre_AND_PULSENUM(abs((pid2.sum_error*3.125*1000)/Delay_Time),abs(pid2.sum_error*3),clockwise);
		////不要用阻塞的printf影响实时控制
		//printf("abs(pid2.sum_error/StepperMotor.StepAngle/0.02) = %d\n",abs(pid2.sum_error/StepperMotor.StepAngle/0.02));
	}
	else{
		SET_TIM2_CH4_Fre_AND_PULSENUM(abs((pid2.sum_error*3.125*1000)/Delay_Time),abs(pid2.sum_error*3),anticlockwise);
		//不要用阻塞的printf影响实时控制
		//printf("abs(pid2.sum_error/StepperMotor.StepAngle/0.02) = %d\n",abs(pid2.sum_error/StepperMotor.StepAngle/0.02));
	}
	delay_ms(Delay_Time);
}
