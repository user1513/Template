#ifndef _BSP_DMA_H
#define _BSP_DMA_H
#include "bsp.h"

void bspDmaUsartInit(void);
uint8_t GetDmaStatus(void);

/*USART数据开始发送*/
void usartSendStart(uint8_t* buf,uint16_t buf_count);

/*USART数据停止发送*/
void usartSendStop(void);

/*设置dma模式*/
void SetDmaStatus(uint8_t _ucMode);

uint8_t GetDmaStatus(void);

#endif

