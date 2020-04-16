#ifndef __INC__H
#define __INC__H

#include "ets_sys.h"
#include "osapi.h"
#include "spi_flash.h"
#include "user_interface.h"
#include "espconn.h"
#include "driver/uart.h"
#include "eagle_soc.h"  	//设置gpio
#include "driver/gpio16.h"	//单独用来配置gpio16引脚
#include "driver/hw_timer.h"//单独用来配置硬件定时器
#include "sntp.h"

//自己改写代码
#include "driver/softap_station.h"
#include "driver/softap_udp_tcp.h"
#include "driver/dns.h"
#include "driver/my_sntp.h"

#include "driver/bsp_delay.h"
#include "driver/bsp_exit.h"
#include "driver/bsp_led.h"
#include "driver/bsp_timer.h"
#endif