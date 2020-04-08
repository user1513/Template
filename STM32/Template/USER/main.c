#include "bsp.h"


uint8_t g_ucStateFlag = 0xff;		/*ȫ��״̬��־λ*/

uint8_t g_uctStr[17];				/*sprintfר������*/



int main(void)
{ 
	FRESULT res;
	
	/*�����ʼ��*/
	bspInit();
	
	/*1. f_mount()ע��/ȡ��ע���Ĺ�����*/
	mf_mount("0:", 1);
	
	while(1)
	{
		
		if(!(g_ucStateFlag & LEDTIMEOUTFLAG))//LED��ʱ��־λ������
		{
			g_ucStateFlag |= LEDTIMEOUTFLAG;

			bspLedFlash();
		}

		if(!(g_ucStateFlag & DHT22TIMEOUTFLAG))//DHT22��ʱ��־λ������
		{
			g_ucStateFlag |= DHT22TIMEOUTFLAG;
		}

		if(!(g_ucStateFlag & OLEDTIMEOUTFLAG))//OLED��ʱ���±�־λ������
		{
			g_ucStateFlag |= OLEDTIMEOUTFLAG;
			
		}
	

		if(!(g_ucStateFlag & SGP30TIMEOUTFLAG))//SGP30��ʱ���±�־λ������
		{
			g_ucStateFlag |= SGP30TIMEOUTFLAG;
		}
		
	}
}


