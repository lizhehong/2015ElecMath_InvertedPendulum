#include "MINI256Z.h"
#include "stm32f10x.h"
#include "delay.h"
#include <stdlib.h>
#include "FreePole.h"
#include "Led.h"
#include "stdio.h"
#include "TIM2_CH4_PWM.h"
#include "StepperMotor.h"
//MINI256 引脚配置
void MINI256_GPIO_Config(){
/* Encoder unit connected to TIM3, 4X mode */    
  GPIO_InitTypeDef GPIO_InitStructure;
	  /* Enable GPIOA, clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	 GPIO_StructInit(&GPIO_InitStructure);
  /* Configure PA.06,07 as encoder input */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

}
//MINI256 定时器3配置
void MINI256Z_TIM3_Init(void)
{
		TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
		TIM_ICInitTypeDef TIM_ICInitStructure;

		/* TIM3 clock source enable */
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

		/* Timer configuration in Encoder mode */
		TIM_DeInit(TIM3);					//设置ENCODER_TIMER为编码器模式
		TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);	//ENCODER_TIMER 复位

		TIM_TimeBaseStructure.TIM_Prescaler = 0x0;  // 没有分频
		TIM_TimeBaseStructure.TIM_Period = (ENCODER_PPR*4)-1; //计数器重装值 设置在下一个更新时间装入活动的自动重装寄存器周期的值
		TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;//设置时钟分割
		TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;   	//向上计数
		TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);


		//外设 2,3,4 这里是TIM3，触发源由TI1和TI2同时，TIM的TI1上升沿输入捕获,TI2上升沿输入捕获，两个通道设置哦，
		//所以对应两个引脚，在没有映射的时候 
		//TI1对应PA6 TI2对应PA7
		TIM_EncoderInterfaceConfig(TIM3, TIM_EncoderMode_TI12, 
												 TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);//编码器接口初始化
		TIM_ICStructInit(&TIM_ICInitStructure);
		//滤波设置
		TIM_ICInitStructure.TIM_ICFilter = ICx_FILTER;//TIM_ICFilter选择输入比较滤波器。该参数取值在0x0和0xf之间
																							//0110 采集频率Fsampling = Fdts/4 ,N=6
																							//在规定频率下 采集6次 如果都是指定电平 就算有效电平
		TIM_ICInit(TIM3, &TIM_ICInitStructure);
		// Clear all pending interrupts
		TIM_ClearFlag(TIM3, TIM_FLAG_Update);//清除TIMx的待处理标志位
		
		TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);//使能指定TIM中断
		//Reset counter
		TIM3->CNT = COUNTER_RESET;
		
		TIM_Cmd(TIM3, ENABLE);  
}
//0.00549 = 180°/32767;
//返回实时角度
double Get_Electrical_Angle_MINI256Z(void)
{
//  s32 temp;
//  temp = (s32)(TIM_GetCounter(TIM3)) * (s32)(U32_MAX / (4*ENCODER_PPR)); 
//	return ((temp/65536)*0.00549);
	//这里设置成0-360 前面的注释是前面的代码的
		double temp;
		temp = TIM_GetCounter(TIM3);
		temp = temp*0.3515625;
		return temp;
}
//返回编码器的实时位置
int Get_Electrical_Position_MINI256Z(void){
		int temp;
		temp = TIM_GetCounter(TIM3);
		return temp;
}
//作为计算自由摆的角速度 放于中断函数中
//count 时间计数  从1开始 UnitTime 作为最小时间单位
//所以count和UnitTime(秒做单位) 一起就能算出时间

//实时角度和
double sum_AcutalAngle = 0.0;
//实时角速度和
double AngleDifference = 0.0;
double tmp = 0.0;
//自由杆角速度差 累加次数
int  sum_count = 0;
void calcAngleSpeed_MINI256(int count,double UnitTime){
	sum_AcutalAngle+=Get_Electrical_Angle_MINI256Z();
	//count == 0 不要进入,5个计数 进入
	if(count%5 == 0){//每5个计数器时间
		//5次实时角度平均值 滤波 作为 自由杆的真正实时角度
		FreePole.AcutalAngle = sum_AcutalAngle/5.0;
		//清除累加和 待下次累加做准备 否则 下次累加后会叠加上一次的数据
		sum_AcutalAngle=0;	
		//计算角度差 正为顺时针 负为逆时针
		AngleDifference = FreePole.AcutalAngle-FreePole.lastAngle;
		tmp =  AngleDifference/(UnitTime*5.0);
		//FreePole.AngleSpeed = AngleDifference/(UnitTime*5.0);
		//归一处理
		if(-2000<= tmp <= 2000){
			FreePole.AngleSpeed = tmp;
		}else if(tmp>2000){
			FreePole.AngleSpeed = 2000;
		}else if(tmp<-2000){
			FreePole.AngleSpeed = -2000;
		}
		//本次的值 作为下一次计算的上一次角度值
		FreePole.lastAngle = FreePole.AcutalAngle;
	}
}
//如果经过期望的角度 返回 时刻的角度
//否则返回 0 
double getAngleMaxpointFlag_MINI256(double SetpointAngle){
	
	double tmp_AngleSpeed = 0;//临时角速度
	double tmp_AcutalAngle = 0;
	//因为角速度计算也需要时间嘛
	delay_ms(2);
	//填充自由摆期望的
	FreePole.SetpointAngle =  SetpointAngle;
	//得到此时刻的角速度
	tmp_AngleSpeed = FreePole.AngleSpeed;
	//得到此时刻的角度
	tmp_AcutalAngle = FreePole.AcutalAngle;
	if(0<tmp_AngleSpeed<1 && abs(tmp_AcutalAngle)>FreePole.SetpointAngle-0.1 && abs(tmp_AcutalAngle)<FreePole.SetpointAngle+1){
		//可能有两次 ，也有可能一次，所以一有就返回位置
		FreePole.AngleMaxpoint = tmp_AcutalAngle;
		return tmp_AcutalAngle;
	}
	return 0 ;	
}
//用于环境零点检测 起点必须是手动能控制的第一个点 也就是演示的起点
//顺时针启动
void EnviromentTest_ZEROPOINT(){
	int TestRange = 360/StepperMotor.StepAngle;//算出所需要侦测零点的范围
	while(TestRange>=1){
		SET_TIM2_CH4_Fre_AND_PULSENUM(3000,1,clockwise);//顺时针
		TestRange--;
		delay_ms(1000);
		delay_ms(1000);
		printf("Actual_ZP_Angle = %f In TestRange = %d\n",Get_Electrical_Angle_MINI256Z(),TestRange);			
	}
}
//编码器运动才进入
void TIM3_IRQHandler()
{  
	/* Clear the interrupt pending flag */
	TIM_ClearFlag(TIM3, TIM_FLAG_Update); 		
}
