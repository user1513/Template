#include "includes.h"


/*
**********************************************************************************************************
											函数声明
**********************************************************************************************************
*/
static void vTaskTaskInit(void *pvParameters);

static void vTaskLED(void *pvParameters);

//static void vTaskFileWrite(void *pvParameters);

static void vTaskFileRead(void *pvParameters);

/*
**********************************************************************************************************
											变量声明
**********************************************************************************************************
*/
static TaskHandle_t xHandleTaskInit = NULL;

static TaskHandle_t xHandleTaskLED = NULL;

static TaskHandle_t xHandleTaskRead = NULL;

//static TaskHandle_t xHandleTaskWrite = NULL;
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
	
	xTaskCreate( vTaskLED,    		/* 任务函数  */
                 "vTaskLED",  		/* 任务名    */
                 64,         		/* 任务栈大小，单位word，也就是4字节 */
                 NULL,        		/* 任务参数  */
                 5,           		/* 任务优先级*/
                 &xHandleTaskLED ); /* 任务句柄  */
	
	
	xTaskCreate( vTaskFileRead,    		/* 任务函数  */
                 "vTaskFileRead",  		/* 任务名    */
                 512,         		/* 任务栈大小，单位word，也就是4字节 */
                 NULL,        		/* 任务参数  */
                 3,           		/* 任务优先级*/
                 &xHandleTaskRead ); /* 任务句柄  */
	
//	xTaskCreate( vTaskFileWrite,    		/* 任务函数  */
//                 "vTaskFileWrite",  		/* 任务名    */
//                 128,         		/* 任务栈大小，单位word，也就是4字节 */
//                 NULL,        		/* 任务参数  */
//                 4,           		/* 任务优先级*/
//                 &xHandleTaskWrite ); /* 任务句柄  */
	
    vTaskDelete(xHandleTaskInit); //删除开始任务
	
    taskEXIT_CRITICAL();            //退出临界区
}



static void vTaskFileWrite(void *pvParameters)
{
	static float float_num=0.00;
	uint32_t length = 0;
	while(1)
	{
		f_open(file, "0:float.txt", FA_WRITE | FA_OPEN_APPEND);

		char* str = (char*)pvPortMalloc(sizeof(30));

		sprintf(str, "float_num的值为: %.2f\r\n",float_num);

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






