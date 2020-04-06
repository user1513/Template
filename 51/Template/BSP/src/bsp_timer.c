/**
  ************************************* Copyright ****************************** 
  *
  *                 (C) Copyright 2019,ZERO,China, NANJING.
  *                            All Rights Reserved
  *                              
  *                     
  *                     https://user1513.github.io/Codes/
  *    
  * FileName   : timer.c   
  * Version    : v1.0		
  * Author     : ZERO			
  * Date       : 2019-04-11         
  * Description:    
  * Function List:  
  	1. ....
  	   <version>: 		
  <modify staff>:
  		  <data>:
   <description>:  
  	2. ...
  ******************************************************************************
 */
#include "bsp_timer.h"


void Timer0Init(void)				//定时器初始化  10毫秒@12.000MHz
{
    TMOD &= 0xF0;					//设置定时器模式
    
	TMOD |= 0x01;					//设置定时器模式
    
	TL0 = 0x00;						//设置定时初值
	
	TH0 = 0x4C;						//设置定时初值
    
	TF0 = 0;						//清除TF0标志
    
	TR0 = 1;						//定时器0开始计时
	
    IE |=0X82;    					//EA , ET0 中断打开
}


extern Info Dishes_Info;
void tim0() interrupt 1
{
	//static uint8_t timeout = 0;
    TL0 = 0x00;																							//设置定时初值
    TH0 = 0x4C;																							//设置定时初值
	
	// if(timeout++ == 2)
	// {
	// 	timeout = 0;
    
		uGetKeyPadAction();
		Electronic_scale_scan(Dishes_Info.WhichES,(float)read2543(0) / 819.0 );
	// }
	
}

