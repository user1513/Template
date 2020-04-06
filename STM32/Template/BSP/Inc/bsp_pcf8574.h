#ifndef _BSP_PCF8974_H
#define _BSP_PCF8974_H

#include "bsp.h"

/*�Ƿ�ʹ���жϻ�ȡPCF8574״̬*/
#define PCF8574_INT_STATE 0

/*PCF8574��PCF8574AΨһ��������������ַ��ͬ*/
#define PCF8574     0
#define PCF8574A     1

/*�궨���豸����*/
#define DEVICE_NAME PCF8574

#if DEVICE_NAME == PCF8574

#define PCF8574X_ADDR 0x40

#else

#define PCF8574X_ADDR 0x70

#endif

//IO�����ź궨��
#define PCF8974X_I2C_SCL       PBout(0) 
#define PCF8974X_I2C_SDA_IN    PBin(1)
#define PCF8974X_I2C_SDA_OUT   PBout(1)


//IO��������
#define PCF8974X_IO_IN()  {GPIOB->MODER&=~(3<<(1*2));GPIOB->MODER|=0<<1*2;}	//PA4����ģʽ
#define PCF8974X_IO_OUT() {GPIOB->MODER&=~(3<<(1*2));GPIOB->MODER|=1<<1*2;} 	//PA4���ģʽ


/*��������*/

void bsp_pcf8974x_io_init(void);

void bsp_pcf8974x_test(uint8_t _ucDeviceNo, uint8_t _ucData);

uint8_t bsp_pcf8974x_receive(uint8_t _ucDeviceNo) ;


#endif

