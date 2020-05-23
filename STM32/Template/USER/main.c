#include "includes.h"


/*
**********************************************************************************************************
											��������
**********************************************************************************************************
*/
static void vTaskTaskInit(void *pvParameters);

static void vTaskLED(void *pvParameters);

static void vTaskGetKey(void *pvParameters);		/*������������*/

static void  App_Printf(char *format, ...);

static void TimerCallbackFunction( TimerHandle_t xTimer );/*��ʱ���ص�����*/

/*
**********************************************************************************************************
											��������
**********************************************************************************************************
*/
static TaskHandle_t xHandleTaskInit = NULL;

static TaskHandle_t xHandleTaskLED = NULL;

static TaskHandle_t xHandleTaskGetKey = NULL;

//******************************************************************************************************//

QueueHandle_t xAppPrintfMutex = NULL;		/*����printf�����ź�������֤�̰߳�ȫ*/

QueueHandle_t xKeyGetSemBin = NULL;			/*���������ź���������֪ͨ����������*/

TimerHandle_t xOnlyTimer = NULL;

TimerHandle_t xRepetitionTimer = NULL;

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

	xAppPrintfMutex = xSemaphoreCreateMutex();		/*���������ź���*/ 
	
	xKeyGetSemBin = xSemaphoreCreateBinary();		/*������ֵ�ź���*/

	xOnlyTimer = xTimerCreate
                 ( "timeronly",
                   5000,
                   pdFALSE,
                   (void *)1,
                   TimerCallbackFunction);
				   
	xRepetitionTimer = xTimerCreate
                 ( "timerrepe",
                   1000,
                   pdTRUE,
                   (void *)2,
                   TimerCallbackFunction);
				   
	xTimerStart(xOnlyTimer, 0);
	xTimerStart(xRepetitionTimer, 0);				   
	xTaskCreate( vTaskLED,    				/* ������  */
                 "vTaskLED",  				/* ������    */
                 64,         				/* ����ջ��С����λword��Ҳ����4�ֽ� */
                 NULL,        				/* �������  */
                 5,           				/* �������ȼ�*/
                 &xHandleTaskLED ); 		/* ������  */
	
	xTaskCreate( vTaskGetKey,    			/* ������  */
                 "vTaskGetKey",  			/* ������    */
                 256,         				/* ����ջ��С����λword��Ҳ����4�ֽ� */
                 NULL,        				/* �������  */
                 4,           				/* �������ȼ�*/
                 &xHandleTaskGetKey ); 		/* ������  */
	
    vTaskDelete(xHandleTaskInit); 			//ɾ����ʼ����
	
    taskEXIT_CRITICAL();            		//�˳��ٽ���
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


/*
*********************************************************************************************************
*	�� �� ��: vTaskGetKey
*	����˵��: ��ȡ����	
*	��    ��: pvParameters ���ڴ���������ʱ���ݵ��β�
*	�� �� ֵ: ��
*   �� �� ��: 2  
*********************************************************************************************************
*/

static void vTaskGetKey(void *pvParameters)
	
{

	uint8_t pcWriteBuffer[500];
	while(1)
	{
		xSemaphoreTake(xKeyGetSemBin, portMAX_DELAY);				/*����ͬ����������*/
		vTaskDelay(20);
		
		switch(KEY_Scan(0))
		{
			case 1:
				xTimerStart(xOnlyTimer, 0);
				App_Printf("=================================================\r\n");
				App_Printf("������      ����״̬ ���ȼ�   ʣ��ջ �������\r\n");
				vTaskList((char *)&pcWriteBuffer);
				App_Printf("%s\r\n", pcWriteBuffer);
			
				App_Printf("\r\n������       ���м���         ʹ����\r\n");
				vTaskGetRunTimeStats((char *)&pcWriteBuffer);
				App_Printf("%s\r\n", pcWriteBuffer);
				App_Printf("��ǰ��̬�ڴ�ʣ���С = %d�ֽ�\r\n", xPortGetFreeHeapSize());
				break;
				
			default:break;
			
		}
		ENABLE_EXIT_LINE(EXTI_Line15);
	}
}


static void TimerCallbackFunction( TimerHandle_t xTimer )
{
	 uint32_t ulCount;

    /* Optionally do something if the pxTimer parameter is NULL. */
    configASSERT(xTimer);

    ulCount = ( uint32_t ) pvTimerGetTimerID(xTimer);
	
	switch(ulCount)
	{
		case 1: App_Printf("��ʱ�����λص�\n");break;
		default: {App_Printf("��ʱ���ظ��ص�%d,����״̬%d\n", ulCount, xTimerIsTimerActive(xOnlyTimer));
		vTimerSetTimerID(xTimer, (void*) ++ulCount) ;
		xTimerChangePeriod(xRepetitionTimer, ulCount * 1000, 0);}
	}
}


/*
*********************************************************************************************************
*	�� �� ��: App_Printf
*	����˵��: �̰߳�ȫ��printf��ʽ		  			  
*	��    ��: ͬprintf�Ĳ�����
*             ��C�У����޷��г����ݺ���������ʵ�ε����ͺ���Ŀʱ,������ʡ�Ժ�ָ��������
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void  App_Printf(char *format, ...)
{
    char  buf_str[200 + 1];
    va_list   v_args;


    va_start(v_args, format);
   (void)vsnprintf((char       *)&buf_str[0],
                   (size_t      ) sizeof(buf_str),
                   (char const *) format,
                                  v_args);
    va_end(v_args);

	/* �����ź��� */
	xSemaphoreTake(xAppPrintfMutex, portMAX_DELAY);

    printf("%s", buf_str);

   	xSemaphoreGive(xAppPrintfMutex);
}





