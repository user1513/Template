#include "includes.h"


/*
**********************************************************************************************************
											��������
**********************************************************************************************************
*/
static void vTaskTaskInit(void *pvParameters);

static void vTaskLED(void *pvParameters);

//static void vTaskFileWrite(void *pvParameters);

static void vTaskFileRead(void *pvParameters);

/*
**********************************************************************************************************
											��������
**********************************************************************************************************
*/
static TaskHandle_t xHandleTaskInit = NULL;

static TaskHandle_t xHandleTaskLED = NULL;

static TaskHandle_t xHandleTaskRead = NULL;

//static TaskHandle_t xHandleTaskWrite = NULL;
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
	
	xTaskCreate( vTaskLED,    		/* ������  */
                 "vTaskLED",  		/* ������    */
                 64,         		/* ����ջ��С����λword��Ҳ����4�ֽ� */
                 NULL,        		/* �������  */
                 5,           		/* �������ȼ�*/
                 &xHandleTaskLED ); /* ������  */
	
	
	xTaskCreate( vTaskFileRead,    		/* ������  */
                 "vTaskFileRead",  		/* ������    */
                 512,         		/* ����ջ��С����λword��Ҳ����4�ֽ� */
                 NULL,        		/* �������  */
                 3,           		/* �������ȼ�*/
                 &xHandleTaskRead ); /* ������  */
	
//	xTaskCreate( vTaskFileWrite,    		/* ������  */
//                 "vTaskFileWrite",  		/* ������    */
//                 128,         		/* ����ջ��С����λword��Ҳ����4�ֽ� */
//                 NULL,        		/* �������  */
//                 4,           		/* �������ȼ�*/
//                 &xHandleTaskWrite ); /* ������  */
	
    vTaskDelete(xHandleTaskInit); //ɾ����ʼ����
	
    taskEXIT_CRITICAL();            //�˳��ٽ���
}



static void vTaskFileWrite(void *pvParameters)
{
	static float float_num=0.00;
	uint32_t length = 0;
	while(1)
	{
		f_open(file, "0:float.txt", FA_WRITE | FA_OPEN_APPEND);

		char* str = (char*)pvPortMalloc(sizeof(30));

		sprintf(str, "float_num��ֵΪ: %.2f\r\n",float_num);

		printf("%s", str);

		f_write(file, str, strlen(str), &length);

		vPortFree(str);

		float_num+=0.01f;

		f_close(file);

        vTaskDelay(1000);
	}
}

//static void vTaskFileRead(void *pvParameters)
//{
//	while(1)
//	{
//		f_open(file, "0:float.txt", FA_READ | FA_OPEN_ALWAYS);

//		mf_read( mf_size());

//		f_close(file);

//        vTaskDelay(10000);
//	}
//}

static void vTaskFileRead(void *pvParameters)
{
	while(1)
	{
		wav_recorder();
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






