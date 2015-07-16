#ifndef __KEY_H
#define __KEY_H
#include "sys.h"

//按键定义
#define KEY1   PAin(0)	  
#define KEY2   PAin(1)	 

void keyInit(void);
uint8_t KEY_Scan(void);

#endif
