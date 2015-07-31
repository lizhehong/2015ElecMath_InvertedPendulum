#ifndef __FREEOLOE_H
#define __FREEOLOE_H
#include "stm32f10x.h"
typedef struct {
	double	LineSpeed;//自由杆的线速度
	double AngleSpeed;//自由杆的角速度
	u16 lastPosition;//自由杆上一次的位置
	float WorkDistance;//运动的距离 
	u16 AcutalPosition;//自由杆的实时位置
	double Len; //自由杆的长度cm
	u16 SetpointPosition;	//自由杆的期望位置
	u16 AngleMaxpoint; //用于记录 起摆后达到单向的最近位置 是超过设定的位置的
}POLEtypedef;

extern POLEtypedef FreePole;


void FreePoleParamsInit(void);

#endif
