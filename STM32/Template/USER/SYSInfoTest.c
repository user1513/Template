#include "SYSInfoTest.h"

/* 被系统调用 */
volatile uint32_t ulHighFrequencyTimerTicks = 0UL;

/*
*********************************************************************************************************
*	函 数 名: vSetupTimerTest
*	功能说明: 创建定时器
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void vSetupSysInfoTest(void)
{
	TimerStructInitDef _TIM_StructInitDef;
	_TIM_StructInitDef.TIM_InterruptFrequency = 20000;
	_TIM_StructInitDef.TIMx =	(void*)TIM5;
	_TIM_StructInitDef.RCC_PeriphClock = RCC_APB1Periph_TIM5;	/*选择定时器时钟线*/
	_TIM_StructInitDef.NVIC_IRQChannel = TIM5_IRQn;				/*设置中断号*/
	_TIM_StructInitDef.NVIC_IRQChannelPreemptionPriority = 5;	/*设置占先式优先级*/
	_TIM_StructInitDef.NVIC_IRQChannelSubPriority = 0;			/*设置子优先级*/
	_TIM_StructInitDef.TIM_InterruptSources =  TIM_IT_Update;	/*中断类型*/
	bsp_TIMx_Init(&_TIM_StructInitDef);
}

/*
*********************************************************************************************************
*	函 数 名: TIM5_IRQHandler
*	功能说明: TIM5中断服务程序。
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void TIM5_IRQHandler(void)
{

	if(TIM_GetITStatus(TIM5,TIM_IT_Update)==SET) //溢出中断
	{
		ulHighFrequencyTimerTicks++;
	}
	TIM_ClearITPendingBit(TIM5,TIM_IT_Update);  //清除中断标志位
}

