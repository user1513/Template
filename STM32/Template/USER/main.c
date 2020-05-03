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

//QueueHandle_t xDmaModeMutexHandle  = NULL;			/*���������ź���,ȷ��dma�Ĺ���ģʽ�������*/

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
	//xDmaModeMutexHandle = xSemaphoreCreateMutex();
	/*�������д����Ҫ���ŵ���Ƶ���,���Դ��5����СΪ4���ֽڵ�item*/
	AudioNoQueueHandle = xQueueCreate( 5, sizeof(int));

	xTaskCreate( vTaskLED,    				/* ������  */
                 "vTaskLED",  				/* ������    */
                 128,         				/* ����ջ��С����λword��Ҳ����4�ֽ� */
                 NULL,        				/* �������  */
                 5,           				/* �������ȼ�*/
                 &xHandleTaskLED ); 		/* ������  */
	
	
	xTaskCreate( vTaskSpeechRec,    		/* ������  */
                 "vTaskSpeechRec",  		/* ������    */
                 256,         				/* ����ջ��С����λword��Ҳ����4�ֽ� */
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
                 128,         				/* ����ջ��С����λword��Ҳ����4�ֽ� */
                 NULL,        				/* �������  */
                 7,           				/* �������ȼ�*/
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

uint8_t speech_rec_statue = 0 ;	
/*����ʶ������*/
static void vTaskSpeechRec(void *pvParameters)

{
//	SpeechRecUartPack();
	static uint8_t SpeechRecNum = 0;
	uint8_t queueval = 1;
	while(1)
	{
		xSemaphoreTake(xSpeechRecSemaphoreHandle, portMAX_DELAY);		/*�ȴ��ź���*/
		
		printf("go to TaskSpeechRec %d\n", (int)SpeechRecNum);
		
		if(!LastEspStatue)
		{
			speech_rec_statue = 1;
			
			Speech_Handle(SpeechRecNum++ % 2);							/*��������ʶ����*/
			
			if(speech_rec_statue && (!(SpeechRecNum % 2)))
			{
				speech_rec_statue = 0;
			}
		}
			
		else
		{
			queueval = 1;
			xQueueSend(AudioNoQueueHandle, (void*)&queueval, portMAX_DELAY);/*����ʧ��*/
		}
	}
}

/*������Ϣ��ȡ*/
static void vTaskKeyGet(void *pvParameters)

{
	while(1)
	{
		xSemaphoreTake(xKeySemaphoreHandle, portMAX_DELAY);		/*�ȴ������жϷ����ź���*/
		printf("go to vTaskKeyGet \n");
		vTaskDelay(500);

		if((KEY == 0) || (KEY1 == 0))
		{
			xSemaphoreGive(xSpeechRecSemaphoreHandle);			/*��������ʶ������*/
			printf("go to vTaskKeyGet ---->>> ok\n");
		}
		ENABLE_EXIT_LINE(EXTI_Line15);
		ENABLE_EXIT_LINE(EXTI_Line10);
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
	uint32_t ulMessage = 0;
	uint8_t ucTmp = 0;
	while(1)
	{
		xQueueReceive(AudioNoQueueHandle,&ulMessage, portMAX_DELAY);/*��ȡ��Ƶ���Ŷ���*/
		

		ucTmp = ulMessage/1000;
		if((ulMessage - (ucTmp * 1000)) < 10 && ulMessage > 10)							/*�ж��Ƿ��Ǵ�MQTT���յ�ָ��*/
		{
			if(ulMessage % 2)
				bsp_pcf8974x_bit_set(0, ucTmp + 3,0);
			else
				bsp_pcf8974x_bit_set(0, ucTmp + 3,1);
		}
		else
		{
			if(ulMessage == 1011)/*���ҵ�*/
				bsp_pcf8974x_bit_set(0, 4 + 3,0);
			if(ulMessage == 1012)/*���ҵ�*/
				bsp_pcf8974x_bit_set(0, 4 + 3,1);
			if(ulMessage == 2011)/*������*/
				bsp_pcf8974x_bit_set(0, 2 + 3,0);
			if(ulMessage == 2012)/*������*/
				bsp_pcf8974x_bit_set(0, 2 + 3,1);
			if(ulMessage == 4011)/*�鷿��*/
				bsp_pcf8974x_bit_set(0, 3 + 3,0);
			if(ulMessage == 4012)/*�鷿��*/
				bsp_pcf8974x_bit_set(0, 3 + 3,1);
			if(ulMessage == 5011)/*������*/
				bsp_pcf8974x_bit_set(0, 1 + 3,1);
			if(ulMessage == 5012)/*������*/
				bsp_pcf8974x_bit_set(0, 1 + 3,1);
			
			sprintf(sprintf_str,"0:AudioPlay/%04d.wav",ulMessage);		/*�����յ�����Ƶ����������*/
	
			recoder_enter_play_mode();								/*���벥��ģʽ*/
			
			wav_play_song((u8*)sprintf_str);						/*������Ƶ*/
			
			vTaskDelay(500);
		}
		
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
uint8_t count = 0;
double DHTxx_Tab[2];
static void vTaskLED(void *pvParameters)

{
    while(1)
    {
		bspLedToggle();

		uint8_t pcf8974x_state = bsp_pcf8974x_receive(0);
		
		bsp_pcf8974x_test(0, (pcf8974x_state&0xf0) + (count & 0x0f));

		count++;
		
		if((!(speech_rec_statue)) && (!LastEspStatue))
		{
			if(timeout++ > 10)				/*ÿ5s�ϴ�һ������*/
			{
				clear_screen();				/*oled����*/
				
				DHTxx_Get_Data(DHTxx_Tab);	/*��ȡdhtxx����*/
				
				uint16_t bh1750fvi_val = bh1750fvi_send_measure(BH1750FVI_H_RESOLUTION);/*��ȡbh1750fvi����*/
				
				char str[30];
				
				lx_Gb2312g_Str("  [������INFO]  ", 1, 0, 1);
				
				sprintf(str,"�¶�:%2.1f��",DHTxx_Tab[1]);
				
				lx_Gb2312g_Str(str, 3, 0, 0);
				
				sprintf(str,"ʪ��:%2.1f%%",DHTxx_Tab[0]);
				
				lx_Gb2312g_Str(str, 5, 0, 0);
				
				sprintf(str,"����ǿ��:%dlx",bh1750fvi_val);
				
				lx_Gb2312g_Str(str, 7, 0, 0);
				
				sprintf(&str[7], "%2.1f,%2.1f,%d,%d",DHTxx_Tab[1] , DHTxx_Tab[0] , bh1750fvi_val, count + 100);
				
				printf("%s\n", &str[7]);
				
				uint8_t length = strlen(&str[7]);
				
				UartDataPacking(str, 6, length, 0);
				
				usartSendStart((uint8_t*)str, 9 + length);/*���ݳ���+���ݰ��ṹ*/
				
				timeout = 0;
				//GetEspInfo();
				
			}
		}
		else
		{
			timeout = 0;
		}
        vTaskDelay(500);		
    }
}






