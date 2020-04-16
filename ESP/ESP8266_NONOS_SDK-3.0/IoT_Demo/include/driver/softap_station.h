#ifndef __SOFTAP_STATION_H
#define __SOFTAP_STATION_H

#include "driver/inc.h"

#define Wifi_Sta_status() wifi_station_get_connect_status()
//设置AP ,SAT, AP + SAT
bool softap_station_scan(uint8_t _set_wifi_mode, uint8_t _select);

//获取AP模式下参数
bool My_Get_softap_Status(uint8_t _select);

//设置AP模式下的参数
bool My_Set_softap_Status(uint8_t* ssid, uint8_t* password,uint8_t _select);

//获取AP模式下终端的信息
void Get_station_info_from_AP(void);

//查询本地端口信息
bool Get_localhost_info(uint8_t mode);


//请勿在 user_init 中调用本接口，本接口必须在系统初始化完成后，并且 ESP8266 Station 接
//口使能的情况下调用。
bool Get_All_Ap_Info(scan_done_cb_t _scan_All_Ap_Info_done);
//读取station模式下的状态
bool My_Get_station_Status(uint8_t _select);
//设置station模式下的状态
bool My_Set_station_Status(uint8_t* ssid, uint8_t* password,uint8_t _select);
#endif
