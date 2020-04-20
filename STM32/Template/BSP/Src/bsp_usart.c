#include "bsp_usart.h"
#include "semphr.h"

extern SemaphoreHandle_t xUsartParseSemaphoreHandle;

void bspUsartInit(uint32_t bound){
	//GPIO�˿�����
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); //ʹ��GPIOAʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);//ʹ��USART1ʱ��
 
	//����2��Ӧ���Ÿ���ӳ��
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_USART2); //GPIOA9����ΪUSART1
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_USART2); //GPIOA10����ΪUSART1
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource0,GPIO_AF_USART2); //GPIOA9����ΪUSART1	
	//USART2�˿�����
    //���˸���ADC��DAC����������ʱ��modeѡģ�������⣬���������modeȫ��ѡAF
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_2 | GPIO_Pin_3; //GPIOA2��GPIOA3
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//�ٶ�50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
	GPIO_Init(GPIOA,&GPIO_InitStructure); //��ʼ��PA2��PA3

    //USART2 ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;//����������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_CTS;//ʹ�ܷ�������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
	USART_Init(USART2, &USART_InitStructure); //��ʼ������2

	//USART_ClearFlag(USART1, USART_FLAG_TC);
    //��ʹ��DMA����ʱ���Բ�ʹ�ý����ж�,ֱ��ʹ��IDLE�����ж�,��������ʵ�ֽ������ⳤ������
	//USART_ITConfig(USART2,  USART_IT_RXNE, ENABLE);//��������ж�	 
	USART_ITConfig(USART2,  USART_IT_IDLE, ENABLE);//��������ж�

	//Usart2 NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;//����2�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=7;//��ռ���ȼ�7
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;		//�����ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ�����

	USART_Cmd(USART2, ENABLE);  //ʹ�ܴ���2 
}


void USART2_IRQHandler(void)                	//����2�жϷ������
{
	u8 Res;
	if(USART_GetITStatus(USART2, USART_IT_IDLE) != RESET)  //�����ж�
	{
		Res = USART2->DR; 
		
		usartReadStart(swich_current_read_point(), 100);
		
		BaseType_t xHigherPriorityTaskWoken = pdFALSE;

		xSemaphoreGiveFromISR(xUsartParseSemaphoreHandle, &xHigherPriorityTaskWoken);

		portYIELD_FROM_ISR( xHigherPriorityTaskWoken);		 
	} 
} 

