#ifndef BSP__H
#define BSP__H

#define LEDTIMEOUTFLAG      (1 << 0)/*LED超时标志位*/
#define DHT22TIMEOUTFLAG    (1 << 1)/*DHT22超时标志位*/
#define OLEDTIMEOUTFLAG     (1 << 2)/*OLED超时标志位*/
#define SGP30TIMEOUTFLAG    (1 << 3)/*OLED超时标志位*/

#include "stm32f4xx.h" 	

#include "sys.h"

#include "delay.h"

#include "stdio.h"

// #include "usart.h"

#include "bsp_led.h"

#include "EventRecorder.h"

#include "bsp_timer.h"

// #include "bsp_dhtxx.h"

// #include "bsp_oled.h"

// #include "bsp_buzzer.h"

#include "bsp_swi2c.h"

// #include "bsp_sgp30.h"

// #include "bsp_pwm.h"

// #include "bsp_adc.h"

#include "bsp_pcf8574.h"

#include "bsp_sdio_sdcard.h"   

#include "CRC.h" /*CRC8_CRC16_CRC32检验*/

void bspInit(void);

#endif



