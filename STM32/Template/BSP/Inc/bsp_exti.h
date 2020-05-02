#ifndef __BSP_EXTI_H
#define __BSP_EXIT_H	 
#include "sys.h"  	
void bspExitInit(void);	//�ⲿ�жϳ�ʼ��	

/*DISABLE_EXIT_LINE(EXTI_Line15) ����EXTI_Line15���ϵ��ж�*/
#define DISABLE_EXIT_LINE(line_x)	EXTI->IMR &= ~(line_x)
#define ENABLE_EXIT_LINE(line_x)		EXTI->IMR |= line_x


#endif

























