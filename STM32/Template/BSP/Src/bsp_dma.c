#include "bsp_dma.h"
#include "semphr.h"

/*dmaģʽ*/
static uint8_t g_DmaMode = 0; 

extern QueueHandle_t xDmaModeMutexHandle;

static void bspDmaUsartSendCfg(uint8_t* buf0,uint16_t num);/*DMA���ڷ������ú���*/

static void bspDmaUsartReadCfg(uint8_t* buf0,uint16_t num);/*DMA���ڽ������ú���*/

static uint8_t g_DmaTable1[100];

static uint8_t g_DmaTable2[100];

static uint8_t* g_ucCurrentPoint = g_DmaTable1;

/*DMA���ڳ�ʼ��*/

void bspDmaUsartInit(void)
{
	bspDmaUsartSendCfg(NULL,0);
	
	bspDmaUsartReadCfg(g_ucCurrentPoint,100);
}

/*DMA���ڷ������ú���*/

static void bspDmaUsartSendCfg(uint8_t* buf0,uint16_t num)
	
{

    NVIC_InitTypeDef   NVIC_InitStructure;

	DMA_InitTypeDef  DMA_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);//DMA1ʱ��ʹ�� 

    USART_DMACmd(USART2, USART_DMAReq_Tx, ENABLE);	/*ʹ��д����dma*/
    
    DMA_Cmd(DMA1_Stream6,DISABLE);

    DMA_InitStructure.DMA_Channel = DMA_Channel_4;  //ͨ��0 SPI2_TXͨ�� 
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&USART2->DR;//�����ַΪ:(u32)&SPI2->DR
    DMA_InitStructure.DMA_Memory0BaseAddr = (u32)buf0;//DMA �洢��0��ַ
    DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;//�洢��������ģʽ
    DMA_InitStructure.DMA_BufferSize = num;//���ݴ����� 
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//���������ģʽ
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//�洢������ģʽ
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;//�������ݳ���:8λ
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;//�洢�����ݳ��ȣ�8λ 
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;// ʹ������ģʽ 
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;//�����ȼ�
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable; //��ʹ��FIFOģʽ        
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull;
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;//����ͻ�����δ���
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;//�洢��ͻ�����δ���
    DMA_Init(DMA1_Stream6, &DMA_InitStructure);//��ʼ��DMA Stream
	
    DMA_ITConfig(DMA1_Stream6,DMA_IT_TC,ENABLE);//������������ж�
 
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream6_IRQn; 
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x04;//��ռ���ȼ�0
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;//�����ȼ�0
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//ʹ���ⲿ�ж�ͨ��
    NVIC_Init(&NVIC_InitStructure);//����
}


/*DMA���ڽ������ú���*/

static void bspDmaUsartReadCfg(uint8_t* buf0,uint16_t num)
	
{

    NVIC_InitTypeDef   NVIC_InitStructure;

	DMA_InitTypeDef  DMA_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);//DMA1ʱ��ʹ�� 

    USART_DMACmd(USART2, USART_DMAReq_Rx, ENABLE);	/*ʹ��д����dma*/
    
    DMA_Cmd(DMA1_Stream5,DISABLE);

    DMA_InitStructure.DMA_Channel = DMA_Channel_4;  //ͨ��0 SPI2_TXͨ�� 
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&USART2->DR;//�����ַΪ:(u32)&SPI2->DR
    DMA_InitStructure.DMA_Memory0BaseAddr = (u32)buf0;//DMA �洢��0��ַ
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;//���赽�洢��ģʽ
    DMA_InitStructure.DMA_BufferSize = num;//���ݴ����� 
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//���������ģʽ
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//�洢������ģʽ
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;//�������ݳ���:8λ
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;//�洢�����ݳ��ȣ�8λ 
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;// ʹ������ģʽ 
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;//�����ȼ�
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable; //��ʹ��FIFOģʽ        
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull;
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;//����ͻ�����δ���
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;//�洢��ͻ�����δ���
    DMA_Init(DMA1_Stream5, &DMA_InitStructure);//��ʼ��DMA Stream
	
    DMA_ITConfig(DMA1_Stream5,DMA_IT_TC,ENABLE);//������������ж�
 
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream5_IRQn; 
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x04;//��ռ���ȼ�0
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;//�����ȼ�0
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//ʹ���ⲿ�ж�ͨ��
    NVIC_Init(&NVIC_InitStructure);//����
	
	DMA_Cmd(DMA1_Stream5,ENABLE);//����DMA TX����,��ʼ��������	
}

/*USART���ݷ��Ϳ�ʼ*/
void usartSendStart(uint8_t* buf,uint16_t buf_count)
{   	  
    DMA_Cmd(DMA1_Stream6,DISABLE);
    DMA_SetCurrDataCounter(DMA1_Stream6, buf_count);
	DMA1_Stream6->M0AR = (uint32_t)buf;
	DMA_Cmd(DMA1_Stream6,ENABLE);//����DMA TX����,��ʼ��������	
}

/*USART���ݷ��ͽ���*/
void usartSendStop(void)
{   	  
    DMA_Cmd(DMA1_Stream6,DISABLE);
}

/*USART�������ݽ��տ�ʼ*/
void usartReadStart(uint8_t* buf,uint16_t buf_count)
{   	  
    DMA_Cmd(DMA1_Stream5,DISABLE);
    DMA_SetCurrDataCounter(DMA1_Stream5, buf_count);
	DMA1_Stream5->M0AR = (uint32_t)buf;
	DMA_Cmd(DMA1_Stream5,ENABLE);//����DMA TX����,��ʼ��������	
}

/*USART���ݽ���ֹͣ*/
void usartReadStop(void)
{   	  
    DMA_Cmd(DMA1_Stream5,DISABLE);
}


void SetDmaStatus(uint8_t _ucMode)
	
{
	xSemaphoreTake(xDmaModeMutexHandle, portMAX_DELAY);
	
	g_DmaMode |= _ucMode;
	
	xSemaphoreGive(xDmaModeMutexHandle);
}

uint8_t GetDmaStatus(void)
{
	return g_DmaMode;
}

//DMA1_Stream6�жϷ�����
void DMA1_Stream6_IRQHandler(void)
{
	if(DMA_GetITStatus(DMA1_Stream6,DMA_IT_TCIF6)==SET)	//DMA1_Stream6,������ɱ�־
	{ 
		DMA_ClearITPendingBit(DMA1_Stream6,DMA_IT_TCIF6);	//�����������ж�	
		
		uint8_t mode = GetDmaStatus();
		
		if(mode & 0x80)		/*������ͱ�־*/
		{
			g_DmaMode &= ~0x80;
		}		
	}  											 
} 

//DMA1_Stream5�жϷ�����
void DMA1_Stream5_IRQHandler(void)
{
	if(DMA_GetITStatus(DMA1_Stream5,DMA_IT_TCIF5)==SET)	//DMA1_Stream5,������ɱ�־
	{ 
		DMA_ClearITPendingBit(DMA1_Stream5,DMA_IT_TCIF5);	//�����������ж�	
	}  											 
}

/*�л���ǰdma����ַ*/
uint8_t* swich_current_read_point(void)
{
	g_ucCurrentPoint = (g_ucCurrentPoint == g_DmaTable1) ? g_DmaTable2 : g_DmaTable1;

	return g_ucCurrentPoint;	
}

/*��ȡ��ǰ���ڶ�ȡ��ɵ�ָ��*/
uint8_t* read_current_finaliy_point(void)
{
	return (g_ucCurrentPoint == g_DmaTable1) ? g_DmaTable2 : g_DmaTable1;
}

