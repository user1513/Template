#include "bsp_timer.h"




void bsp_TIMx_Init(TimerStructInitDef * _TimerStructInitDef)
{
	uint16_t arr = 0, psc = 84;
	/*当输入的值不能与1000000整除时会导致定时时间偏差1*/
	arr = 84000000 / psc / _TimerStructInitDef->TIM_InterruptFrequency;	/*求出arr自动重装载值*/
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(_TimerStructInitDef->RCC_PeriphClock,ENABLE);  ///使能TIM时钟
	
  	TIM_TimeBaseInitStructure.TIM_Period = arr - 1; 	//自动重装载值
	TIM_TimeBaseInitStructure.TIM_Prescaler = psc - 1;  //定时器分频
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(_TimerStructInitDef->TIMx,&TIM_TimeBaseInitStructure);//初始化TIM
	
	if(_TimerStructInitDef->TIM_InterruptSources)
	{
		TIM_ITConfig(_TimerStructInitDef->TIMx,_TimerStructInitDef->TIM_InterruptSources,ENABLE); //允许定时器更新中断

		NVIC_InitStructure.NVIC_IRQChannel =_TimerStructInitDef->NVIC_IRQChannel; //定时器中断
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = _TimerStructInitDef->NVIC_IRQChannelPreemptionPriority; //抢占优先级1
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = _TimerStructInitDef->NVIC_IRQChannelSubPriority; //子优先级3
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);
	}
	TIM_Cmd(_TimerStructInitDef->TIMx,ENABLE); //使能定时器	
}

