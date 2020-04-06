#ifndef __BSP_USART_H_
#define __BSP_USART_H_


#include <inc.H>


void UartInit(void);
void SendByte(unsigned char byte);
void Send_String(unsigned char *str);
void Send_String_Len(unsigned char *str, unsigned char Len);
#endif
