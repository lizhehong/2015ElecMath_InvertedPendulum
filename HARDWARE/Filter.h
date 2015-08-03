#ifndef __FILTER_H
#define __FILTER_H
#include "stm32f10x.h"
#define FilterLen 5
typedef struct{
	u16 Data[FilterLen];
}Filtertypedef;

extern Filtertypedef WindowFilter ;

double windowSlideFilter(double now);
void Filter_Init(void);
#endif

