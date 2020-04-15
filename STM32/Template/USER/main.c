#include "includes.h"


/*
**********************************************************************************************************
											函数声明
**********************************************************************************************************
*/
static void vTaskTaskInit(void *pvParameters);

static void vTaskLED(void *pvParameters);

static void vTaskKeyGet(void *pvParameters);

static void vTaskSpeechRec(void *pvParameters);

/*
**********************************************************************************************************
											变量声明
**********************************************************************************************************
*/
static TaskHandle_t xHandleTaskInit = NULL;

static TaskHandle_t xHandleTaskLED = NULL;

static TaskHandle_t xHandleTaskSpeechRec = NULL;

static TaskHandle_t xHandleTaskKeyGet = NULL;

SemaphoreHandle_t xKeySemaphoreHandle = NULL;		/*创建按键与中断同步二值信号量*/

SemaphoreHandle_t xSpeechRecSemaphoreHandle = NULL;	/*创建按键与语音识别同步信号量*/

QueueHandle_t xDmaModeMutexHandle  = NULL;
/*
*********************************************************************************************************
*	函 数 名: main
*	功能说明: 标准c程序入口。
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
int main(void)
{ 
	/* 
	  在启动调度前，为了防止初始化STM32外设时有中断服务程序执行，这里禁止全局中断(除了NMI和HardFault)。
	  这样做的好处是：
	  1. 防止执行的中断服务程序中有FreeRTOS的API函数。
	  2. 保证系统正常启动，不受别的中断影响。
	  3. 关于是否关闭全局中断，大家根据自己的实际情况设置即可。
	  在移植文件port.c中的函数prvStartFirstTask中会重新开启全局中断。通过指令cpsie i开启，__set_PRIMASK(1)
	  和cpsie i是等效的。
     */
	//__set_PRIMASK(1);
	
	/*外设初始化*/
	bspInit();
	/* 创建初始任务 */
	 xTaskCreate( vTaskTaskInit,   		/* 任务函数  */
                 "vTaskTaskInit",     	/* 任务名    */
                 128,               	/* 任务栈大小，单位word，也就是4字节 */
                 NULL,              	/* 任务参数  */
                 1,                 	/* 任务优先级*/
                 &xHandleTaskInit );  	/* 任务句柄  */
	
    /* 启动调度，开始执行任务 */
    vTaskStartScheduler();
	/* 
	  如果系统正常启动是不会运行到这里的，运行到这里极有可能是用于定时器任务或者空闲任务的
	  heap空间不足造成创建失败，此要加大FreeRTOSConfig.h文件中定义的heap大小：
	  #define configTOTAL_HEAP_SIZE	      ( ( size_t ) ( 17 * 1024 ) )
	*/
	while(1);
}

static void vTaskTaskInit(void *pvParameters)

{
	taskENTER_CRITICAL();           //进入临界区
	
	/*1. f_mount()注册/取消注册卷的工作区*/
	mf_mount("0:", 1);
	
	/*创建二值信号量,专门用于按键和中断同步*/
	/*首次创建二值信号量,值为0*/
	xKeySemaphoreHandle = xSemaphoreCreateBinary();	
	/*创建二值信号量,专门用于按键和语音识别任务同步*/
	xSpeechRecSemaphoreHandle = xSemaphoreCreateBinary();
	/*创建互斥信号量,确保dma的工作模式不会出错*/
	xDmaModeMutexHandle = xSemaphoreCreateMutex();

	xTaskCreate( vTaskLED,    			/* 任务函数  */
                 "vTaskLED",  			/* 任务名    */
                 64,         			/* 任务栈大小，单位word，也就是4字节 */
                 NULL,        			/* 任务参数  */
                 5,           			/* 任务优先级*/
                 &xHandleTaskLED ); 	/* 任务句柄  */
	
	
	xTaskCreate( vTaskSpeechRec,    		/* 任务函数  */
                 "vTaskSpeechRec",  		/* 任务名    */
                 128,         			/* 任务栈大小，单位word，也就是4字节 */
                 NULL,        			/* 任务参数  */
                 3,           			/* 任务优先级*/
                 &xHandleTaskSpeechRec ); 	/* 任务句柄  */
	
	xTaskCreate( vTaskKeyGet,    		/* 任务函数  */
                 "vTaskKeyGet",  		/* 任务名    */
                 64,         			/* 任务栈大小，单位word，也就是4字节 */
                 NULL,        			/* 任务参数  */
                 6,           			/* 任务优先级*/
                 &xHandleTaskKeyGet ); 	/* 任务句柄  */
	
    vTaskDelete(xHandleTaskInit); //删除开始任务
	
    taskEXIT_CRITICAL();            //退出临界区
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
		xSemaphoreTake(xKeySemaphoreHandle, portMAX_DELAY);		/*等待按键中断发送信号量*/

		vTaskDelay(10);

		if(KEY == 0)
		{
			xSemaphoreGive(xSpeechRecSemaphoreHandle);			/*启动语音识别任务*/
		}
	}
}



/*
*********************************************************************************************************
*	函 数 名: vTaskLED
*	功能说明: LED闪烁	
*	形    参: pvParameters 是在创建该任务时传递的形参
*	返 回 值: 无
*   优 先 级: 2  
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






