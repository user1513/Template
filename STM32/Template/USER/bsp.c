#include "bsp.h"

void bspInit(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//����ϵͳ�ж����ȼ�����2
	
    delay_init(168);		        /*��ʼ����ʱ����*/

	bspLedInit();		            /*��ʼ��LED�˿�*/

	bspKeyInit();					/*��ʼ�������˿�*/
	
	bspExitInit();					/*��ʼ���������ڵ��ж�*/
	
	/*ʹ��EventRecorder��printf��Ҫ����usart.c�к궨��*/
//	EventRecorderInitialize(EventRecordAll, 1U);/*�¼���¼����ʼ��*/

//	EventRecorderStart();           /*�¼���¼������*/

//    TIM3_Int_Init(9,8399);          /*timer3 = 84Mhz, �趨���ʱ��Ϊ1ms*/

	uart_init(512000);				/*����0��ʼ��*/
	
    //vDhtxx_Init();                /*DHT22��ʼ������*/
	
	//OLED_Init();					/*OLED��ʼ������*/

	//bspBuzzerInit();				/*��������ʼ��*/

	//bspBuzzerFreq(50, 1500);		/*��������������100ms,����Ӧʱ��Ϊ5000ms*/

	//bsp_sgp30_io_init();			/*sgp30�����������ʼ��*/

	//bsp_adc_init();				/*adc1ͨ��0��ʼ��*/

	//TIM2_PWM_Init(999, 839);		/*PWM���ڳ�ʼ��*/

	//TIM_SetCompare2(TIM2, 32);	/*PWMռ�ձȳ�ʼ��*/

//	bsp_pcf8974x_io_init();			/*pcf8974�����ʼ��*/

	usmart_init(84);				/*��ʼ��usmart*/
	
	WM8978_Init();				//��ʼ��WM8978
	WM8978_HPvol_Set(40,40);	//������������
	WM8978_SPKvol_Set(63);		//������������

	exfuns_init();					/*fatfs��������*/
	
	while(SD_Init())//��ⲻ��SD��	/*SD����ʼ��*/
	{
		printf("SD Card Error!\n");
		delay_ms(500);					
		printf("Please Check!\n");
		delay_ms(500);
	}
	
	bspUsartInit(460800);
	
	bspDmaUsartInit();				/*DMA��ʼ��*/
	
	printf("����ʼ!!!\n");

}

