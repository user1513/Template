#ifndef _SPPECH_REC_H
#define _SPPECH_REC_H

#include "stdint.h"
/*����ʶ������*/
void Speech_Handle(uint8_t _ucMode);

void SpeechRecUartPack(void);
/*���ڽ�һ�����ݴ���ɺϸ�����ݰ��ṹ*/
uint32_t UartDataPacking(char* str, uint8_t type, uint32_t length, uint8_t Check);
#endif
