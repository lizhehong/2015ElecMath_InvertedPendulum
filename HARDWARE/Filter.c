#include "Filter.h"

Filtertypedef WindowFilter ;

//���ڻ����˲� �Ӵ�ֱ��� 
//�����ͺ�����ǿ ƽ����ǿ
double windowSlideFilter(double now){
	char i = 0;
	double tmp =0;
	for(i=FilterLen-1;i>0;i--)
		WindowFilter.Data[i] = WindowFilter.Data[i-1];
	
	tmp = now - WindowFilter.Data[FilterLen-1];
	
	WindowFilter.Data[0] = now;
	return tmp;
}

