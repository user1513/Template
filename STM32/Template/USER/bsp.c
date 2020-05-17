#include "bsp.h"

void bspInit(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//����ϵͳ�ж����ȼ�����4
	
    delay_init(168);		        /*��ʼ����ʱ����*/

	bspLedInit();		            /*	*/

	bspKeyInit();					/*��ʼ�������˿�*/
	
	bspExitInit();					/*��ʼ���������ڵ��ж�*/
	
	/*ʹ��EventRecorder��printf��Ҫ����usart.c�к궨��*/
//	EventRecorderInitialize(EventRecordAll, 1U);/*�¼���¼����ʼ��*/

//	EventRecorderStart();           /*�¼���¼������*/

//    TIM3_Int_Init(9,8399);          /*timer3 = 84Mhz, �趨���ʱ��Ϊ1ms*/

	uart_init(460800);				/*����0��ʼ��*/
	
    vDhtxx_Init();                	/*DHT22��ʼ������*/
	
	//OLED_Init();					/*OLED��ʼ������*/

	//bspBuzzerInit();				/*��������ʼ��*/

	//bspBuzzerFreq(50, 1500);		/*��������������100ms,����Ӧʱ��Ϊ5000ms*/

	//bsp_sgp30_io_init();			/*sgp30�����������ʼ��*/

	//bsp_adc_init();				/*adc1ͨ��0��ʼ��*/

	//TIM2_PWM_Init(999, 839);		/*PWM���ڳ�ʼ��*/

	//TIM_SetCompare2(TIM2, 32);	/*PWMռ�ձȳ�ʼ��*/

	//bsp_pcf8974x_io_init();		/*pcf8974�����ʼ��*/

	usmart_init(84);				/*��ʼ��usmart*/
	
	WM8978_Init();				//��ʼ��WM8978
	WM8978_HPvol_Set(63,63);	//������������
	WM8978_SPKvol_Set(63);		//������������

	exfuns_init();					/*fatfs��������*/
	
	while(SD_Init())//��ⲻ��SD��	/*SD����ʼ��*/
	{
		printf("SD Card Error!\n");
		delay_ms(500);					
		printf("Please Check!\n");
		delay_ms(500);
	}
	
	bspUsartInit(460800);			/*���ڳ�ʼ��*/
	
	bspDmaUsartInit();				/*DMA��ʼ��*/

	//bsp_encoder_init();			/*��ʱ��������ģʽ����*/
	
	bsp_pcf8974x_io_init();			/*pcf8974io��ʼ��*/
	
	bsp_bh1750fvi_io_init();		/*���նȴ�������ʼ��*/
	
	SPI1_Init();					/*SPI1��ʼ��*/
	
	initial_lcd();					/*LCD ģ���ʼ��*/
	
	bsp_pcf8974x_test(0,0xff);		/*�趨ledȫ��*/
	
	clear_screen();					/*����*/
	
	lx_Gb2312g_Str("<�����������Ƶ�>", 1, 0, 0);
	
	lx_Gb2312g_Str("���ܼҾ�ϵͳ���", 3, 0, 0);
	
	lx_Gb2312g_Str("�ȴ��豸�����ɹ�", 5, 0, 0);
	
	lx_Gb2312g_Str("    �����ߣ�СԬ", 7, 0, 0);
	
	printf("����ʼ!!!\n");

}

