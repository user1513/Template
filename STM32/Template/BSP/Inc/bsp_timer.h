#ifndef _TIMER_H
#define _TIMER_H
#include "bsp.h"

//ͨ�ö�ʱ���жϳ�ʼ��
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��
//��ʱ�����ʱ����㷽��:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=��ʱ������Ƶ��,��λ:Mhz  84Mhz

typedef struct _TimerStructInitDef
{
	
	uint32_t RCC_PeriphClock;						/*RCC TIMER����*/
	uint32_t TIM_InterruptFrequency;				/*�����ж϶�ʱƵ��,���1MHZ,����ͨ���޸ĳ����psc����Ƶ��*/
	void * TIMx;									/*���ö�ʱ��*/
	/*��TIM_interrupt_sources��ֵΪ��0ʱ�����ж�*/
	uint16_t TIM_InterruptSources;					/*�жϵķ�ʽ*/
	uint8_t  NVIC_IRQChannel;						/*�жϺ�*/
	uint8_t	 NVIC_IRQChannelPreemptionPriority;		/*ռ���ȼ�*/
	uint8_t  NVIC_IRQChannelSubPriority;  			/*�����ȼ�*/
	
}TimerStructInitDef;



//	TimerStructInitDef _TIM_StructInitDef;
//	_TIM_StructInitDef.TIM_InterruptFrequency = 20000;
//	_TIM_StructInitDef.TIMx =	(void*)TIM5;
//	_TIM_StructInitDef.RCC_PeriphClock = RCC_APB1Periph_TIM5;	/*ѡ��ʱ��ʱ����*/
//	_TIM_StructInitDef.NVIC_IRQChannel = TIM5_IRQn;				/*�����жϺ�*/
//	_TIM_StructInitDef.NVIC_IRQChannelPreemptionPriority = 5;	/*����ռ��ʽ���ȼ�*/
//	_TIM_StructInitDef.NVIC_IRQChannelSubPriority = 0;			/*���������ȼ�*/
//	_TIM_StructInitDef.TIM_InterruptSources =  TIM_IT_Update;	/*�ж�����*/

void bsp_TIMx_Init(TimerStructInitDef * _TimerStructInitDef);

#endif
