#ifndef _BSP_DMA_H
#define _BSP_DMA_H
#include "bsp.h"

void bspDmaUsartInit(void);

/*USART���ݿ�ʼ����*/
void usartSendStart(uint8_t* buf,uint16_t buf_count);

/*USART����ֹͣ����*/
void usartSendStop(void);

/*USART�������ݽ��տ�ʼ*/
void usartReadStart(uint8_t* buf,uint16_t buf_count);

/*USART���ݽ���ֹͣ*/
void usartReadStop(void);

/*����dmaģʽ*/
void SetDmaStatus(uint8_t _ucMode);

/*��ȡdmaģʽ*/
uint8_t GetDmaStatus(void);

/*�л���ǰdma����ַ*/
uint8_t* swich_current_read_point(void);

/*��ȡ��ǰ���ڶ�ȡ��ɵ�ָ��*/
uint8_t* read_current_finaliy_point(void);

#endif

