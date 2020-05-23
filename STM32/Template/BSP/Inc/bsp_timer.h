#ifndef _TIMER_H
#define _TIMER_H
#include "bsp.h"

//通用定时器中断初始化
//arr：自动重装值。
//psc：时钟预分频数
//定时器溢出时间计算方法:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=定时器工作频率,单位:Mhz  84Mhz

typedef struct _TimerStructInitDef
{
	
	uint32_t RCC_PeriphClock;						/*RCC TIMER设置*/
	uint32_t TIM_InterruptFrequency;				/*设置中断定时频率,最高1MHZ,可以通过修改程序的psc增高频率*/
	void * TIMx;									/*设置定时器*/
	/*当TIM_interrupt_sources的值为非0时启动中断*/
	uint16_t TIM_InterruptSources;					/*中断的方式*/
	uint8_t  NVIC_IRQChannel;						/*中断号*/
	uint8_t	 NVIC_IRQChannelPreemptionPriority;		/*占优先级*/
	uint8_t  NVIC_IRQChannelSubPriority;  			/*子优先级*/
	
}TimerStructInitDef;



//	TimerStructInitDef _TIM_StructInitDef;
//	_TIM_StructInitDef.TIM_InterruptFrequency = 20000;
//	_TIM_StructInitDef.TIMx =	(void*)TIM5;
//	_TIM_StructInitDef.RCC_PeriphClock = RCC_APB1Periph_TIM5;	/*选择定时器时钟线*/
//	_TIM_StructInitDef.NVIC_IRQChannel = TIM5_IRQn;				/*设置中断号*/
//	_TIM_StructInitDef.NVIC_IRQChannelPreemptionPriority = 5;	/*设置占先式优先级*/
//	_TIM_StructInitDef.NVIC_IRQChannelSubPriority = 0;			/*设置子优先级*/
//	_TIM_StructInitDef.TIM_InterruptSources =  TIM_IT_Update;	/*中断类型*/

void bsp_TIMx_Init(TimerStructInitDef * _TimerStructInitDef);

#endif
