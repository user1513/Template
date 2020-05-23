#include "bsp_exti.h"
#include "FreeRTOS.h"
#include "semphr.h"			/*�����ź���*/

extern QueueHandle_t xKeyGetSemBin;
//�ⲿ�ж�15�������
void EXTI15_10_IRQHandler(void)
{
	 if(EXTI_GetITStatus(EXTI_Line15) == SET)
	 {
		DISABLE_EXIT_LINE(EXTI_Line15);
		BaseType_t xHigherPriorityTaskWoken = pdFALSE;
		xSemaphoreGiveFromISR(xKeyGetSemBin,&xHigherPriorityTaskWoken);				/*����ͬ����������*/
		portYIELD_FROM_ISR( xHigherPriorityTaskWoken);	 
		printf("�ⲿ�ж�\n");
		EXTI_ClearITPendingBit(EXTI_Line15);//���LINE2�ϵ��жϱ�־λ 
	 }
}
 
//�ⲿ�жϳ�ʼ������
//��ʼ��PE2~4,PA0Ϊ�ж�����.
void bspExitInit(void)
{
	NVIC_InitTypeDef   NVIC_InitStructure;
	EXTI_InitTypeDef   EXTI_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);//ʹ��SYSCFGʱ��

	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource15);//PA6 ���ӵ��ж���6

	/* ����EXTI_Line15 */
	EXTI_InitStructure.EXTI_Line = EXTI_Line15;//LINE15
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;//�ж��¼�
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; //�½��ش��� 
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;//ʹ��LINE0
	EXTI_Init(&EXTI_InitStructure);//����
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;//�ⲿ�ж�15
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x06;//��ռ���ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;//�����ȼ�2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//ʹ���ⲿ�ж�ͨ��
	NVIC_Init(&NVIC_InitStructure);//����
	
}












