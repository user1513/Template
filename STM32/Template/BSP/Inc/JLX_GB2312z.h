#ifndef _JLX_GB2312Z_H
#define _JLX_GB2312Z_H

#include "sys.h"

#define ROM_CS PAout(8)

#define FONT_15X16_GB2312_BASEADDER 0X000000   //15*16��GB2312��׼�����ֿ�
#define FONT_7X8_ASCLL_BASEADDER 0X0066C0	   //7*8��ASCLL�ַ�
#define FONT_8X16_ASCLL_BASEADDER 0X03B7C0	   //8*16��ASCLL�ַ�
#define FONT_5X7_ASCLL_BASEADDER 0X03BFC0	   //5*7��ASCLL�ַ�
#define FONT_8X16_ASCLLBOLD_BASEADDER 0X03CF80 //8*16��ASCLL�����ַ�

uint8_t Get_JLX_5X7_Ascll(uint8_t uAscll, uint8_t* sAscll, uint8_t Convet); //sAscll>12

uint8_t Get_JLX_7X8_Ascll(uint8_t uAscll, uint8_t* sAscll, uint8_t Convet);//sAscll>12

uint8_t Get_JLX_8X16_Ascll(uint8_t uAscll, uint8_t* sAscll, uint8_t Convet);//sAscll>20

uint8_t Get_JLX_8X16_Bold_Ascll(uint8_t uAscll, uint8_t* sAscll, uint8_t Convet);//sAscll>20

uint8_t Get_JLX_15X16_GB2312(char *uGb2312, uint8_t* sAscll, uint8_t Convet);//sAscll>36


#endif

