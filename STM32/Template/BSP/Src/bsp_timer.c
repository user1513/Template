#include "bsp_timer.h"




void bsp_TIMx_Init(TimerStructInitDef * _TimerStructInitDef)
{
	uint16_t arr = 0, psc = 84;
	/*�������ֵ������1000000����ʱ�ᵼ�¶�ʱʱ��ƫ��1*/
	arr = 84000000 / psc / _TimerStructInitDef->TIM_InterruptFrequency;	/*���arr�Զ���װ��ֵ*/
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(_TimerStructInitDef->RCC_PeriphClock,ENABLE);  ///ʹ��TIMʱ��
	
  	TIM_TimeBaseInitStructure.TIM_Period = arr - 1; 	//�Զ���װ��ֵ
	TIM_TimeBaseInitStructure.TIM_Prescaler = psc - 1;  //��ʱ����Ƶ
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //���ϼ���ģʽ
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(_TimerStructInitDef->TIMx,&TIM_TimeBaseInitStructure);//��ʼ��TIM
	
	if(_TimerStructInitDef->TIM_InterruptSources)
	{
		TIM_ITConfig(_TimerStructInitDef->TIMx,_TimerStructInitDef->TIM_InterruptSources,ENABLE); //����ʱ�������ж�

		NVIC_InitStructure.NVIC_IRQChannel =_TimerStructInitDef->NVIC_IRQChannel; //��ʱ���ж�
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = _TimerStructInitDef->NVIC_IRQChannelPreemptionPriority; //��ռ���ȼ�1
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = _TimerStructInitDef->NVIC_IRQChannelSubPriority; //�����ȼ�3
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);
	}
	TIM_Cmd(_TimerStructInitDef->TIMx,ENABLE); //ʹ�ܶ�ʱ��	
}

