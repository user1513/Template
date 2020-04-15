#ifndef _BSP_DMA_H
#define _BSP_DMA_H
#include "bsp.h"

void bspDmaUsartInit(void);
uint8_t GetDmaStatus(void);

/*USART���ݿ�ʼ����*/
void usartSendStart(uint8_t* buf,uint16_t buf_count);

/*USART����ֹͣ����*/
void usartSendStop(void);

/*����dmaģʽ*/
void SetDmaStatus(uint8_t _ucMode);

uint8_t GetDmaStatus(void);

#endif

