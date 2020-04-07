#include "bsp.h"

uint8_t g_ucStateFlag = 0xff;		/*全局状态标志位*/

uint8_t g_uctStr[17];				/*sprintf专用数组*/

//通过串口打印SD卡相关信息
void show_sdcard_info(void)
{
	switch(SDCardInfo.CardType)
	{
		case SDIO_STD_CAPACITY_SD_CARD_V1_1:printf("Card Type:SDSC V1.1\r\n");break;
		case SDIO_STD_CAPACITY_SD_CARD_V2_0:printf("Card Type:SDSC V2.0\r\n");break;
		case SDIO_HIGH_CAPACITY_SD_CARD:printf("Card Type:SDHC V2.0\r\n");break;
		case SDIO_MULTIMEDIA_CARD:printf("Card Type:MMC Card\r\n");break;
	}	
  	printf("Card ManufacturerID:%d\r\n",SDCardInfo.SD_cid.ManufacturerID);	//制造商ID
 	printf("Card RCA:%d\r\n",SDCardInfo.RCA);								//卡相对地址
	printf("Card Capacity:%d MB\r\n",(u32)(SDCardInfo.CardCapacity>>20));	//显示容量
 	printf("Card BlockSize:%d\r\n\r\n",SDCardInfo.CardBlockSize);			//显示块大小
}

int main(void)
{ 
	uint8_t num = 0;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	/*外设初始化*/
	bspInit();
	
	while(SD_Init())//检测不到SD卡
	{
		printf("SD Card Error!\n");
		delay_ms(500);					
		printf("Please Check!\n");
		delay_ms(500);
	}
	show_sdcard_info();	//打印SD卡相关信息
	
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

gpio_write


