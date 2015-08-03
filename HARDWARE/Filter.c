#include "Filter.h"
#include "PID.h"
Filtertypedef WindowFilter ;
void Filter_Init(void){
	char i = 0;
	for(i=0;i<FilterLen;i++)
		WindowFilter.Data[i] = pid.sp;
}
//窗口滑动滤波 加大分辨率 
//就是滞后性增强 平滑增强
double windowSlideFilter(double now){
	char i = 0;
	double tmp =0;
	for(i=FilterLen-1;i>0;i--)
		WindowFilter.Data[i] = WindowFilter.Data[i-1];
	
	tmp = now - WindowFilter.Data[FilterLen-1];
	
	WindowFilter.Data[0] = now;
	return tmp;
}

