#ifndef __LED_H
#define __LED_H
#include "sys.h"
	


//LED�˿ڶ���
#define LED0 PCout(13)	// DS0 

void bspLedInit(void);//��ʼ��	

void bspLedToggle(void);//PC0������˸

#endif
