#include "bsp.h"


uint8_t g_ucStateFlag = 0xff;		/*全局状态标志位*/

uint8_t g_uctStr[17];				/*sprintf专用数组*/



int main(void)
{ 
	FRESULT res;
	
	/*外设初始化*/
	bspInit();
	
	/*1. f_mount()注册/取消注册卷的工作区*/
	mf_mount("0:", 1);
	
	while(1)
	{
		
		if(!(g_ucStateFlag & LEDTIMEOUTFLAG))//LED超时标志位被清零
		{
			g_ucStateFlag |= LEDTIMEOUTFLAG;

			bspLedFlash();
		}

		if(!(g_ucStateFlag & DHT22TIMEOUTFLAG))//DHT22超时标志位被清零
		{
			g_ucStateFlag |= DHT22TIMEOUTFLAG;
		}

		if(!(g_ucStateFlag & OLEDTIMEOUTFLAG))//OLED超时更新标志位被清零
		{
			g_ucStateFlag |= OLEDTIMEOUTFLAG;
			
		}
	

		if(!(g_ucStateFlag & SGP30TIMEOUTFLAG))//SGP30超时更新标志位被清零
		{
			g_ucStateFlag |= SGP30TIMEOUTFLAG;
		}
		
	}
}


