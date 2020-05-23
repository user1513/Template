#include "SYSInfoTest.h"

/* ��ϵͳ���� */
volatile uint32_t ulHighFrequencyTimerTicks = 0UL;

/*
*********************************************************************************************************
*	�� �� ��: vSetupTimerTest
*	����˵��: ������ʱ��
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void vSetupSysInfoTest(void)
{
	TimerStructInitDef _TIM_StructInitDef;
	_TIM_StructInitDef.TIM_InterruptFrequency = 20000;
	_TIM_StructInitDef.TIMx =	(void*)TIM5;
	_TIM_StructInitDef.RCC_PeriphClock = RCC_APB1Periph_TIM5;	/*ѡ��ʱ��ʱ����*/
	_TIM_StructInitDef.NVIC_IRQChannel = TIM5_IRQn;				/*�����жϺ�*/
	_TIM_StructInitDef.NVIC_IRQChannelPreemptionPriority = 5;	/*����ռ��ʽ���ȼ�*/
	_TIM_StructInitDef.NVIC_IRQChannelSubPriority = 0;			/*���������ȼ�*/
	_TIM_StructInitDef.TIM_InterruptSources =  TIM_IT_Update;	/*�ж�����*/
	bsp_TIMx_Init(&_TIM_StructInitDef);
}

/*
*********************************************************************************************************
*	�� �� ��: TIM5_IRQHandler
*	����˵��: TIM5�жϷ������
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void TIM5_IRQHandler(void)
{

	if(TIM_GetITStatus(TIM5,TIM_IT_Update)==SET) //����ж�
	{
		ulHighFrequencyTimerTicks++;
	}
	TIM_ClearITPendingBit(TIM5,TIM_IT_Update);  //����жϱ�־λ
}

