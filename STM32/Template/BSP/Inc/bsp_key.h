#ifndef _BSP_KEY_H
#define _BSP_KEY_H	 
#include "sys.h" 

#define config_USE_ZDYZ 0	/*ʹ������ԭ��f407����*/
#define config_USE_DIY	1	/*ʹ�÷�����İ���*/

#define key_DEVICE_NAME config_USE_DIY

#if key_DEVICE_NAME == config_USE_ZDYZ
/*����ķ�ʽ��ͨ��ֱ�Ӳ����⺯����ʽ��ȡIO*/
#define KEY0 		GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_4) //PE4
#define KEY1 		GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_3)	//PE3 
#define KEY2 		GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_2) //PE2
#define WK_UP 	GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)	//PA0


/*���淽ʽ��ͨ��λ��������ʽ��ȡIO*/
/*
#define KEY0 		PEin(4)   	//PE4
#define KEY1 		PEin(3)		//PE3 
#define KEY2 		PEin(2)		//P32
#define WK_UP 	PAin(0)		//PA0
*/



#else

#define KEY 	PAin(15)
#define KEY1 	PAin(10)
#endif

#define KEY0_PRES 	1
#define KEY1_PRES	2
#define KEY2_PRES	3
#define WKUP_PRES   4

void bspKeyInit(void);			//IO��ʼ��

u8 KEY_Scan(u8);  				//����ɨ�躯��	

u8 uasrt_key_send(u8 _ucKey);	//ͨ������ģ�ⷢ�Ͱ���ֵ

u8 uasrt_key_get(void);			//��ȡ���ڷ��Ͱ���ֵ

#endif
