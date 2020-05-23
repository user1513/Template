#ifndef __BSP_EXTI_H
#define __BSP_EXIT_H	 
#include "sys.h"  	
void bspExitInit(void);	//外部中断初始化	

/*DISABLE_EXIT_LINE(EXTI_Line15) 屏蔽EXTI_Line15线上的中断*/
#define DISABLE_EXIT_LINE(EXTI_Linex)	EXTI->IMR &= ~(EXTI_Linex)
#define ENABLE_EXIT_LINE(EXTI_Linex)		EXTI->IMR |= EXTI_Linex


#endif

























