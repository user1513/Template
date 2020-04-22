#include "includes.h"


/*
**********************************************************************************************************
											��������
**********************************************************************************************************
*/
static void vTaskTaskInit(void *pvParameters);

static void vTaskLED(void *pvParameters);

static void vTaskKeyGet(void *pvParameters);

static void vTaskSpeechRec(void *pvParameters);

static void vTaskUsartParse(void *pvParameters);

static void vTaskAudioPlay(void *pvParameters);
/*
**********************************************************************************************************
											��������
**********************************************************************************************************
*/
static TaskHandle_t xHandleTaskInit = NULL;

static TaskHandle_t xHandleTaskLED = NULL;

static TaskHandle_t xHandleTaskSpeechRec = NULL;

static TaskHandle_t xHandleTaskKeyGet = NULL;

static TaskHandle_t xHandleTaskUsartParse = NULL;

static TaskHandle_t xHandleTaskAudioPlay = NULL;

//******************************************************************************************************//

SemaphoreHandle_t xKeySemaphoreHandle = NULL;		/*�����������ж�ͬ����ֵ�ź���*/

SemaphoreHandle_t xSpeechRecSemaphoreHandle = NULL;	/*��������������ʶ��ͬ���ź���*/

SemaphoreHandle_t xUsartParseSemaphoreHandle = NULL;/*������ֵ�ź���,ר�����ڴ��ڿ����ж��봮�ڽ�������ͬ��*/

QueueHandle_t xDmaModeMutexHandle  = NULL;			/*���������ź���,ȷ��dma�Ĺ���ģʽ�������*/

QueueHandle_t AudioNoQueueHandle = NULL; 			/*�����������ڴ����Ƶ��������*/
/*
*********************************************************************************************************
*	�� �� ��: main
*	����˵��: ��׼c������ڡ�
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
int main(void)
{ 
	/* 
	  ����������ǰ��Ϊ�˷�ֹ��ʼ��STM32����ʱ���жϷ������ִ�У������ֹȫ���ж�(����NMI��HardFault)��
	  �������ĺô��ǣ�
	  1. ��ִֹ�е��жϷ����������FreeRTOS��API������
	  2. ��֤ϵͳ�������������ܱ���ж�Ӱ�졣
	  3. �����Ƿ�ر�ȫ���жϣ���Ҹ����Լ���ʵ��������ü��ɡ�
	  ����ֲ�ļ�port.c�еĺ���prvStartFirstTask�л����¿���ȫ���жϡ�ͨ��ָ��cpsie i������__set_PRIMASK(1)
	  ��cpsie i�ǵ�Ч�ġ�
     */
	//__set_PRIMASK(1);
	
	/*�����ʼ��*/
	bspInit();
	/* ������ʼ���� */
	 xTaskCreate( vTaskTaskInit,   		/* ������  */
                 "vTaskTaskInit",     	/* ������    */
                 128,               	/* ����ջ��С����λword��Ҳ����4�ֽ� */
                 NULL,              	/* �������  */
                 1,                 	/* �������ȼ�*/
                 &xHandleTaskInit );  	/* ������  */
	
    /* �������ȣ���ʼִ������ */
    vTaskStartScheduler();
	/* 
	  ���ϵͳ���������ǲ������е�����ģ����е����Ｋ�п��������ڶ�ʱ��������߿��������
	  heap�ռ䲻����ɴ���ʧ�ܣ���Ҫ�Ӵ�FreeRTOSConfig.h�ļ��ж����heap��С��
	  #define configTOTAL_HEAP_SIZE	      ( ( size_t ) ( 17 * 1024 ) )
	*/
	while(1);
}

static void vTaskTaskInit(void *pvParameters)

{
	taskENTER_CRITICAL();           //�����ٽ���
	
	/*1. f_mount()ע��/ȡ��ע���Ĺ�����*/
	mf_mount("0:", 1);
	
	/*������ֵ�ź���,ר�����ڰ������ж�ͬ��*/
	/*�״δ�����ֵ�ź���,ֵΪ0*/
	xKeySemaphoreHandle = xSemaphoreCreateBinary();	
	/*������ֵ�ź���,ר�����ڰ���������ʶ������ͬ��*/
	xSpeechRecSemaphoreHandle = xSemaphoreCreateBinary();
	/*������ֵ�ź���,ר�����ڴ��ڿ����ж��봮�ڽ�������ͬ��*/
	xUsartParseSemaphoreHandle = xSemaphoreCreateBinary();
	/*���������ź���,ȷ��dma�Ĺ���ģʽ�������*/
	xDmaModeMutexHandle = xSemaphoreCreateMutex();
	/*�������д����Ҫ���ŵ���Ƶ���,���Դ��5����СΪ4���ֽڵ�item*/
	AudioNoQueueHandle = xQueueCreate( 5, sizeof(int));

	xTaskCreate( vTaskLED,    				/* ������  */
                 "vTaskLED",  				/* ������    */
                 64,         				/* ����ջ��С����λword��Ҳ����4�ֽ� */
                 NULL,        				/* �������  */
                 5,           				/* �������ȼ�*/
                 &xHandleTaskLED ); 		/* ������  */
	
	
	xTaskCreate( vTaskSpeechRec,    		/* ������  */
                 "vTaskSpeechRec",  		/* ������    */
                 128,         				/* ����ջ��С����λword��Ҳ����4�ֽ� */
                 NULL,        				/* �������  */
                 3,           				/* �������ȼ�*/
                 &xHandleTaskSpeechRec ); 	/* ������  */
	
	xTaskCreate( vTaskKeyGet,    			/* ������  */
                 "vTaskKeyGet",  			/* ������    */
                 64,         				/* ����ջ��С����λword��Ҳ����4�ֽ� */
                 NULL,        				/* �������  */
                 6,           				/* �������ȼ�*/
                 &xHandleTaskKeyGet ); 		/* ������  */
				 
	xTaskCreate( vTaskUsartParse,    		/* ������  */
                 "vTaskUsartParse",  		/* ������    */
                 64,         				/* ����ջ��С����λword��Ҳ����4�ֽ� */
                 NULL,        				/* �������  */
                 6,           				/* �������ȼ�*/
                 &xHandleTaskUsartParse ); 	/* ������  */
				 
	xTaskCreate( vTaskAudioPlay,    		/* ������  */
                 "vTaskAudioPlay",  		/* ������    */
                 128,         				/* ����ջ��С����λword��Ҳ����4�ֽ� */
                 NULL,        				/* �������  */
                 2,           				/* �������ȼ�*/
                 &xHandleTaskAudioPlay ); 	/* ������  */
	
    vTaskDelete(xHandleTaskInit); 			//ɾ����ʼ����
	
    taskEXIT_CRITICAL();            		//�˳��ٽ���
}

extern uint8_t LastEspStatue ;	/*�ⲿ����*/
/*����ʶ������*/
static void vTaskSpeechRec(void *pvParameters)

{
//	SpeechRecUartPack();
	static uint8_t SpeechRecNum = 0;
	uint8_t queueval = 1;
	while(1)
	{
		xSemaphoreTake(xSpeechRecSemaphoreHandle, portMAX_DELAY);
		if(!LastEspStatue)
			Speech_Handle(SpeechRecNum++ % 2);
		else
		{
			queueval = 1;
			xQueueSend(AudioNoQueueHandle, (void*)&queueval, portMAX_DELAY);
		}
	}
}

/*������Ϣ��ȡ*/
static void vTaskKeyGet(void *pvParameters)

{
	while(1)
	{
		xSemaphoreTake(xKeySemaphoreHandle, portMAX_DELAY);		/*�ȴ������жϷ����ź���*/

		vTaskDelay(10);

		if(KEY == 0)
		{
			xSemaphoreGive(xSpeechRecSemaphoreHandle);			/*��������ʶ������*/
		}
	}
}

/*�������ݽ���*/
static void vTaskUsartParse(void *pvParameters)

{
	while(1)
	{
	xSemaphoreTake(xUsartParseSemaphoreHandle, portMAX_DELAY);		/*�ȴ������жϷ����ź���*/
		
	usart_parse(read_current_finaliy_point());
		
	}
}
//e6 89 93 e5 bc 80 e5 8d a7 e5 ae a4 e7 81 af 2c e5 85 b3 e9 97 ad e5 ae a2 e5 8e 85 e7 a9 ba e8 b0 83

char sprintf_str[30] = {0};	

static void vTaskAudioPlay(void *pvParameters)
{
	uint32_t ucTmp = 0;
	
	while(1)
	{
		xQueueReceive(AudioNoQueueHandle,&ucTmp, portMAX_DELAY);/*��ȡ��Ƶ���Ŷ���*/
		
		sprintf(sprintf_str,"0:AudioPlay/%04d.wav",ucTmp);		/*�����յ�����Ƶ����������*/
		
		recoder_enter_play_mode();								/*���벥��ģʽ*/
		
		wav_play_song((u8*)sprintf_str);						/*������Ƶ*/
		
		vTaskDelay(500);
	}
}



/*
*********************************************************************************************************
*	�� �� ��: vTaskLED
*	����˵��: LED��˸	
*	��    ��: pvParameters ���ڴ���������ʱ���ݵ��β�
*	�� �� ֵ: ��
*   �� �� ��: 2  
*********************************************************************************************************
*/
uint8_t timeout = 0;
static void vTaskLED(void *pvParameters)

{
    while(1)
    {
		bspLedToggle();
		
		if(timeout++ > 20)				/*ÿ10s��ѯһ��esp��״̬*/
		{
			timeout = 0;
			//GetEspInfo();
		}
        vTaskDelay(500);		
    }
}






