#ifndef _BSP_DS1302_H
#define _BSP_DS1302_H

#include "inc.h"
//DS1302引脚定义,可根据实际情况自行修改端口定义
sbit DS1302_RST=P1^0;
sbit DS1302_IO=P1^2;
sbit DS1302_SCK=P1^1;
//DS1302地址定义
#define ds1302_sec_add			0x80		//秒数据地址
#define ds1302_min_add			0x82		//分数据地址
#define ds1302_hr_add			0x84		//时数据地址
#define ds1302_date_add			0x86		//日数据地址
#define ds1302_month_add		0x88		//月数据地址
#define ds1302_day_add			0x8a		//星期数据地址
#define ds1302_year_add			0x8c		//年数据地址
#define ds1302_control_add		0x8e		//控制数据地址
#define ds1302_charger_add		0x90 					 
#define ds1302_clkburst_add		0xbe

/*默认使用  ds1302_write_time()一般只使用一次
 ds1302_init(); 		//DS1302初始化
 ds1302_write_time(); 	//写入初始值
*/
uchar* GetTime(void);
void ds1302_init(void);

void ds1302_write_time(void) ;
//从DS302读出时钟数据
void ds1302_read_time(void) ;
#endif