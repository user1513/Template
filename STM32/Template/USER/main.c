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

/*
**********************************************************************************************************
											��������
**********************************************************************************************************
*/
static TaskHandle_t xHandleTaskInit = NULL;

static TaskHandle_t xHandleTaskLED = NULL;

static TaskHandle_t xHandleTaskSpeechRec = NULL;

static TaskHandle_t xHandleTaskKeyGet = NULL;

SemaphoreHandle_t xKeySemaphoreHandle = NULL;		/*�����������ж�ͬ����ֵ�ź���*/

SemaphoreHandle_t xSpeechRecSemaphoreHandle = NULL;	/*��������������ʶ��ͬ���ź���*/

QueueHandle_t xDmaModeMutexHandle  = NULL;
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
	/*���������ź���,ȷ��dma�Ĺ���ģʽ�������*/
	xDmaModeMutexHandle = xSemaphoreCreateMutex();

	xTaskCreate( vTaskLED,    			/* ������  */
                 "vTaskLED",  			/* ������    */
                 64,         			/* ����ջ��С����λword��Ҳ����4�ֽ� */
                 NULL,        			/* �������  */
                 5,           			/* �������ȼ�*/
                 &xHandleTaskLED ); 	/* ������  */
	
	
	xTaskCreate( vTaskSpeechRec,    		/* ������  */
                 "vTaskSpeechRec",  		/* ������    */
                 128,         			/* ����ջ��С����λword��Ҳ����4�ֽ� */
                 NULL,        			/* �������  */
                 3,           			/* �������ȼ�*/
                 &xHandleTaskSpeechRec ); 	/* ������  */
	
	xTaskCreate( vTaskKeyGet,    		/* ������  */
                 "vTaskKeyGet",  		/* ������    */
                 64,         			/* ����ջ��С����λword��Ҳ����4�ֽ� */
                 NULL,        			/* �������  */
                 6,           			/* �������ȼ�*/
                 &xHandleTaskKeyGet ); 	/* ������  */
	
    vTaskDelete(xHandleTaskInit); //ɾ����ʼ����
	
    taskEXIT_CRITICAL();            //�˳��ٽ���
}







static void vTaskSpeechRec(void *pvParameters)

{
		test();
	static uint8_t SpeechRecNum = 0;
	while(1)
	{
		xSemaphoreTake(xSpeechRecSemaphoreHandle, portMAX_DELAY);
		
		Speech_Handle(SpeechRecNum++ % 2);
		
		//wav_recorder();
	}
}

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



/*
*********************************************************************************************************
*	�� �� ��: vTaskLED
*	����˵��: LED��˸	
*	��    ��: pvParameters ���ڴ���������ʱ���ݵ��β�
*	�� �� ֵ: ��
*   �� �� ��: 2  
*********************************************************************************************************
*/
static void vTaskLED(void *pvParameters)

{
    while(1)
    {
		bspLedToggle();
        vTaskDelay(500);
    }
}






