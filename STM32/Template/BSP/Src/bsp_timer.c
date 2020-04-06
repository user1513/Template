#include "bsp_timer.h"



//通用定时器3中断初始化
//arr：自动重装值。
//psc：时钟预分频数
//定时器溢出时间计算方法:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=定时器工作频率,单位:Mhz  84Mhz
//这里使用的是定时器3!
void TIM3_Int_Init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);  ///使能TIM3时钟
	
  TIM_TimeBaseInitStructure.TIM_Period = arr; 	//自动重装载值
	TIM_TimeBaseInitStructure.TIM_Prescaler=psc;  //定时器分频
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStructure);//初始化TIM3
	
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE); //允许定时器3更新中断
	TIM_Cmd(TIM3,ENABLE); //使能定时器3
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM3_IRQn; //定时器3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x01; //抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x03; //子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
}
extern uint8_t g_ucStateFlag;

uint16_t g_usDht22Timeout = 0;			//DHT22间隔计数器
#define DHT22TIMEOUT 2000				//DHT22间隔2000MS

uint16_t g_usLedTimeout = 0;			//LED闪烁间隔计数器
#define LEDTIMEOUT 500					//LED闪烁间隔500MS

uint16_t g_usOledTimeout = 0;			//Oled update时间计数器
#define OLEDTIMEOUT 500					//Oled 更新画面间隔500MS

uint16_t g_usSgp30Timeout = 0;			//Sgp30 update时间计数器
#define SGP30TIMEOUT 1000				//Sgp30 更新画面间隔1000MS

//定时器3中断服务函数
void TIM3_IRQHandler(void)
{

	if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET) //溢出中断
	{
		if(g_usLedTimeout == 0)
			EventStartA(0);

		g_usLedTimeout++;
		
		if(g_usLedTimeout > LEDTIMEOUT)  //计数器超出设定闪烁间隔
		{
			g_usLedTimeout = 0;
			EventStopA(0);
			g_ucStateFlag &= ~LEDTIMEOUTFLAG;//将第零位清零
		}
//		if(g_usDht22Timeout == 0)
//			EventStartA(0);
		g_usDht22Timeout++;	

		if(g_usDht22Timeout > DHT22TIMEOUT)  //计数器超出设定
		{
			g_usDht22Timeout = 0;	

			g_ucStateFlag &= ~DHT22TIMEOUTFLAG;//将第一位清零	
		}
//		if(g_usOledTimeout == 0)
//			EventStartB(0);
		g_usOledTimeout++;

		if(g_usOledTimeout > OLEDTIMEOUT)  //计数器超出设定
		{
			g_usOledTimeout = 0;
			
			g_ucStateFlag &= ~OLEDTIMEOUTFLAG;//将第二位清零
		}


		g_usSgp30Timeout++;

		if(g_usSgp30Timeout > SGP30TIMEOUT)  //计数器超出设定
		{
			g_usSgp30Timeout = 0;
			
			g_ucStateFlag &= ~SGP30TIMEOUTFLAG;//将第三位清零
		}


//		bspBeepHandler(); 				/*beep蜂鸣器处理函数*/
	
	}
	TIM_ClearITPendingBit(TIM3,TIM_IT_Update);  //清除中断标志位
}
