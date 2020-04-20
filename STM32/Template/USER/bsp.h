#ifndef BSP__H
#define BSP__H

#include "stm32f4xx.h" 	

#include "sys.h"

#include "delay.h"

#include "stdio.h"

#include "usart.h"	

#include "usmart.h"

#include "bsp_led.h"

#include "bsp_key.h"

//#include "EventRecorder.h"

//#include "bsp_timer.h"

// #include "bsp_dhtxx.h"

// #include "bsp_oled.h"

// #include "bsp_buzzer.h"

#include "bsp_swi2c.h"

// #include "bsp_sgp30.h"

// #include "bsp_pwm.h"

// #include "bsp_adc.h"

#include "bsp_pcf8574.h"

#include "bsp_sdio_sdcard.h"   

#include "CRC.h" /*CRC8_CRC16_CRC32����*/

#include "ff.h" /*fatfs*/

#include "exfuns.h"

#include "fattester.h"	

#include "string.h"

#include "stdlib.h"

#include "bsp_wm8978.h"	 

#include "audioplay.h"	

#include "myapp.h"

#include "Speech_Rec.h" /*����ʶ�����api*/

#include "bsp_exti.h"

#include "bsp_dma.h"	

#include "bsp_usart.h"

/*�����ʼ������*/
void bspInit(void);

#endif



