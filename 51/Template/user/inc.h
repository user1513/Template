#ifndef _INC_H
#define _INC_H

typedef unsigned int uint16_t ;
typedef unsigned char uint8_t;
typedef unsigned long int uint32_t;
	
typedef unsigned int uint;
typedef unsigned char uchar;
	
typedef struct _Dishe
{
	char* DisheName;
	float price;
}Dishes;

typedef struct _info
{
	float weight;
	float Peeled;
	float price;
	float money;
	char Num;
	char WhichES;
}Info;


#include <REGX52.H>
#include "bsp_tcl1543.h"
#include "intrins.h"
#include "bsp_usart.h"
#include "string.h"
#include "stdio.h"
#include "bsp_keypad.h"
#include "bsp_lcd.h"
#include "bsp_timer.h"
#include "bsp_405x.h"
//#include "bsp_ds1302.h"
//#include "bsp_sht11.h"
#endif

