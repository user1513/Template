#ifndef __BSP_SPI_H
#define __BSP_SPI_H
#include "sys.h"
			  	    													  
void SPI1_Init(void);			 //��ʼ��SPI��
void SPI1_SetSpeed(u8 SpeedSet); //����SPI�ٶ�   
//void SPI2_ReadWriteByte(u16 TxData);//SPI���߶�дһ���ֽ�
u16 SPI1_ReadWriteByte(u16 TxData);
//u8 SPI2_ReadWriteByte(u8 TxData);
		 
#endif
