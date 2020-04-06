#ifndef _BSP_KEYPAD_H
#define _BSP_KEYPAD_H

#include "inc.h"

#define GET_KEYPAD_VAL (~(P1>>4))&0X0F
#define SET_KEYPAD(_VAL) P1 |= _VAL&0X0F
#define RESET_KEYPAD(_VAL) P1 &= ~(_VAL&0X0F)

uint8_t uGetKeyPadAction(void);
unsigned char Get_KeyPad(void);
void uClear_KeyPad(void);
#endif

