#include "includes.h"


/*
**********************************************************************************************************
											函数声明
**********************************************************************************************************
*/
static void vTaskTaskInit(void *pvParameters);

static void vTaskLED(void *pvParameters);

static void vTaskGetKey(void *pvParameters);		/*按键处理任务*/

static void  App_Printf(char *format, ...);

static void TimerCallbackFunction( TimerHandle_t xTimer );/*定时器回调函数*/

/*
**********************************************************************************************************
											变量声明
**********************************************************************************************************
*/
static TaskHandle_t xHandleTaskInit = NULL;

static TaskHandle_t xHandleTaskLED = NULL;

static TaskHandle_t xHandleTaskGetKey = NULL;

//******************************************************************************************************//

QueueHandle_t xAppPrintfMutex = NULL;		/*创建printf互斥信号量，保证线程安全*/

QueueHandle_t xKeyGetSemBin = NULL;			/*创建按键信号量，用于通知按键处理函数*/

TimerHandle_t xOnlyTimer = NULL;

TimerHandle_t xRepetitionTimer = NULL;

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

	xAppPrintfMutex = xSemaphoreCreateMutex();		/*创建互斥信号量*/ 
	
	xKeyGetSemBin = xSemaphoreCreateBinary();		/*创建二值信号量*/

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
	xTaskCreate( vTaskLED,    				/* 任务函数  */
                 "vTaskLED",  				/* 任务名    */
                 64,         				/* 任务栈大小，单位word，也就是4字节 */
                 NULL,        				/* 任务参数  */
                 5,           				/* 任务优先级*/
                 &xHandleTaskLED ); 		/* 任务句柄  */
	
	xTaskCreate( vTaskGetKey,    			/* 任务函数  */
                 "vTaskGetKey",  			/* 任务名    */
                 256,         				/* 任务栈大小，单位word，也就是4字节 */
                 NULL,        				/* 任务参数  */
                 4,           				/* 任务优先级*/
                 &xHandleTaskGetKey ); 		/* 任务句柄  */
	
    vTaskDelete(xHandleTaskInit); 			//删除开始任务
	
    taskEXIT_CRITICAL();            		//退出临界区
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


/*
*********************************************************************************************************
*	函 数 名: vTaskGetKey
*	功能说明: 获取按键	
*	形    参: pvParameters 是在创建该任务时传递的形参
*	返 回 值: 无
*   优 先 级: 2  
*********************************************************************************************************
*/

static void vTaskGetKey(void *pvParameters)
	
{

	uint8_t pcWriteBuffer[500];
	while(1)
	{
		xSemaphoreTake(xKeyGetSemBin, portMAX_DELAY);				/*用于同步按键处理*/
		vTaskDelay(20);
		
		switch(KEY_Scan(0))
		{
			case 1:
				xTimerStart(xOnlyTimer, 0);
				App_Printf("=================================================\r\n");
				App_Printf("任务名      任务状态 优先级   剩余栈 任务序号\r\n");
				vTaskList((char *)&pcWriteBuffer);
				App_Printf("%s\r\n", pcWriteBuffer);
			
				App_Printf("\r\n任务名       运行计数         使用率\r\n");
				vTaskGetRunTimeStats((char *)&pcWriteBuffer);
				App_Printf("%s\r\n", pcWriteBuffer);
				App_Printf("当前动态内存剩余大小 = %d字节\r\n", xPortGetFreeHeapSize());
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
		case 1: App_Printf("定时器单次回调\n");break;
		default: {App_Printf("定时器重复回调%d,单次状态%d\n", ulCount, xTimerIsTimerActive(xOnlyTimer));
		vTimerSetTimerID(xTimer, (void*) ++ulCount) ;
		xTimerChangePeriod(xRepetitionTimer, ulCount * 1000, 0);}
	}
}


/*
*********************************************************************************************************
*	函 数 名: App_Printf
*	功能说明: 线程安全的printf方式		  			  
*	形    参: 同printf的参数。
*             在C中，当无法列出传递函数的所有实参的类型和数目时,可以用省略号指定参数表
*	返 回 值: 无
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

	/* 互斥信号量 */
	xSemaphoreTake(xAppPrintfMutex, portMAX_DELAY);

    printf("%s", buf_str);

   	xSemaphoreGive(xAppPrintfMutex);
}





