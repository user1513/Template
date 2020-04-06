#ifndef _BSP_405X_H
#define _BSP_405X_H

#include "inc.h"

//输入端4051引脚配置
#define _4051_IO 		P2
#define _4051_INPUT_A 	(1<<3)
#define _4051_INPUT_B 	(1<<4)
#define _4051_INPUT_C 	(1<<5)

#define INPUT_ONE_ENALBE  	{_4051_IO &= ~(_4051_INPUT_A + _4051_INPUT_B + _4051_INPUT_C);}
#define INPUT_TWO_ENALBE	{_4051_IO &= ~(_4051_INPUT_B + _4051_INPUT_C) ; _4051_IO |= _4051_INPUT_A;}
#define INPUT_THREE_ENALBE	{_4051_IO &= ~(_4051_INPUT_A + _4051_INPUT_C) ; _4051_IO |= _4051_INPUT_B;}
#define INPUT_FOUR_ENALBE	{_4051_IO &= ~(_4051_INPUT_C) ; _4051_IO |= _4051_INPUT_B +_4051_INPUT_A;}
#define INPUT_CLEAR_ENALBE	{_4051_IO &= ~(_4051_INPUT_B + _4051_INPUT_A) ; _4051_IO |= _4051_INPUT_C;}

//运放放大段4052引脚配置
#define _4052_IO 		P3
#define _4052_INPUT_A 	(1<<6)
#define _4052_INPUT_B 	(1<<7)

#define OP_dw1_ENALBE  		{_4052_IO &= ~(_4052_INPUT_A + _4052_INPUT_B);}
#define OP_dw2_ENALBE			{_4052_IO &= ~_4052_INPUT_B; _4052_IO |= _4052_INPUT_A;}
#define OP_dw3_ENALBE		{_4052_IO &= ~_4052_INPUT_A ; _4052_IO |= _4052_INPUT_B;}

double Get_ES_Val(void);
uint8_t Electronic_scale_scan(uint8_t _scan,float val);
#endif

