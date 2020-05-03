#ifndef _BH1750FVI_H
#define _BH1750FVI_H

#include "bsp.h"

/*ע��bh1750������ѹ2.4~3.6v*/

#define BH1750FVI_LOW_LEVEL_ADDR    0X46                    /*bh1750fvi<--->ADD���Žӵ͵�ƽʱ���I2C��ַ ע�����λΪ��д״̬λ*/
#define BH1750FVI_HIGH_LEVEL_ADDR   0XB8                    /*bh1750fvi<--->ADD���ŽӸߵ�ƽʱ���I2C��ַ ע�����λΪ��д״̬λ*/

/*����ADDR�ĵ�ƽ�����豸��ַ*/
#define BH1750FVI_DEVICE_ADDR   BH1750FVI_LOW_LEVEL_ADDR    /*ʹ�õ��豸��ַ*/

#define BH1750FVI_H_RESOLUTION_MEASURE_TIME  120            /*bh1750fviH�ֱ���ģʽ����ʱ����*/
#define BH1750FVI_L_RESOLUTION_MEASURE_TIME  16             /*bh1750fviH�ֱ���ģʽ����ʱ����*/

/*���弸����Ҫ�Ĺ��ܴ���*/
#define BH1750FVI_POWER_OFF         0X00                    /*�豸�ϵ�*/
#define BH1750FVI_POWER_ON          0X01                    /*�豸�ϵ�*/
#define BH1750FVI_H_RESOLUTION      0X10                    /*����H�ֱ���ģʽ*/
#define BH1750FVI_H_RESOLUTION_2    0X11                    /*����H�ֱ���ģʽ2*/
#define BH1750FVI_L_RESOLUTION      0X13                    /*����L�ֱ���ģʽ*/
#define BH1750FVI_ONLY_H_RESOLUTION      0X20               /*����H�ֱ���ģʽ���������Զ�����Ϊ�ϵ�ģʽ*/
#define BH1750FVI_ONLY_H_RESOLUTION_2    0X21               /*����H�ֱ���ģʽ2���������Զ�����Ϊ�ϵ�ģʽ*/
#define BH1750FVI_ONLY_L_RESOLUTION      0X23               /*����L�ֱ���ģʽ���������Զ�����Ϊ�ϵ�ģʽ*/

//IO�����ź궨��
#define BH1750FVI_I2C_SCL       PDout(15) 
#define BH1750FVI_I2C_SDA_IN    PCin(7)
#define BH1750FVI_I2C_SDA_OUT   PCout(7)


//IO��������
#define BH1750FVI_IO_IN()  {GPIOC->MODER&=(~((uint32_t)3<<(7*2)));GPIOC->MODER|=((uint32_t)0<<(7*2));}	//PA4����ģʽ
#define BH1750FVI_IO_OUT() {GPIOC->MODER&=(~((uint32_t)3<<(7*2)));GPIOC->MODER|=((uint32_t)1<<(7*2));} 	//PA4���ģʽ


/*Ӧ�ú���*/
void bsp_bh1750fvi_io_init(void);                           /*bh1750fvi��IO���ų�ʼ��*/  

uint16_t bh1750fvi_send_measure(uint8_t ucMode);            /*bh1750fvi��ʼ���������һ�ȡ����*/

#endif


