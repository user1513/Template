#ifndef __LED_H
#define __LED_H
#include "sys.h"
	


//LED�˿ڶ���
//#define LED0 PCout(13)	// DS0 
#define LED0 PFout(9)	// DS0
#define LED1 PFout(10)	// DS1	 

void bspLedInit(void);//��ʼ��	

void bspLedToggle(void);//PC0������˸

#endif
