#ifndef _BSP_TCL1543_H
#define _BSP_TCL1543_H

#include	"inc.h"
#define CLOCK 		P3_2 /*2543时钟*/
#define D_IN 		P3_3 /*2543输入*/
#define D_OUT 		P3_4 /*2543输出*/
#define _CS 		P3_5 /*2543片选*/

uint read2543(uchar port);
uint GetTcl1543(void);
#endif

