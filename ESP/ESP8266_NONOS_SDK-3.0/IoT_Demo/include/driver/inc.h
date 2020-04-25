#ifndef __INC__H
#define __INC__H

typedef struct
{
	const char* dns_name;
	char ip_name[16];
	unsigned short port;
}GetDnsInfo;

#include "ets_sys.h"
#include "osapi.h"
#include "spi_flash.h"
#include "user_interface.h"
#include "espconn.h"
#include "driver/uart.h"
#include "eagle_soc.h"  	//����gpio
#include "driver/gpio16.h"	//������������gpio16����
#include "driver/hw_timer.h"//������������Ӳ����ʱ��
#include "sntp.h"
#include "mem.h"
#include "mqtt.h"
#include "wifi.h"
#include "config.h"
#include "debug.h"
//�Լ���д����
#include "driver/softap_station.h"
#include "driver/softap_udp_tcp.h"
#include "driver/dns.h"
#include "driver/my_sntp.h"

#include "driver/bsp_delay.h"
#include "driver/bsp_exit.h"
#include "driver/bsp_led.h"
#include "driver/bsp_timer.h"


#define RTS_FLAG(a)  GPIO_OUTPUT_SET(GPIO_ID_PIN(10),a)
#endif
