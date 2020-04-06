#ifndef __LED_H
#define __LED_H
#include "sys.h"
	


//LED端口定义
#define LED0 PCout(0)	// DS0 

#define FS0 PAout(6)	// DS0 

void bspLedInit(void);//初始化	

void bspLedFlash(void);//PC0引脚闪烁

#endif
