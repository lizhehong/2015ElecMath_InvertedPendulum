#ifndef __FREEOLOE_H
#define __FREEOLOE_H
typedef struct {
	double	LineSpeed;//自由杆的线速度
	double AngleSpeed;//自由杆的角速度
	double lastAngle;//自由杆上一次的角度
	float WorkDistance;//运动的距离 
	double AcutalAngle;//自由杆的实时角度
	double Len; //自由杆的长度cm
	double SetpointAngle;	//自由杆的期望角度
	double AngleMaxpoint; //用于记录 起摆后达到单向的最近位置 是超过设定的位置的
}POLEtypedef;

extern POLEtypedef FreePole;


void FreePoleParamsInit(void);

#endif
