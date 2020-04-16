#ifndef __SOFTAP_STATION_H
#define __SOFTAP_STATION_H

#include "driver/inc.h"

#define Wifi_Sta_status() wifi_station_get_connect_status()
//����AP ,SAT, AP + SAT
bool softap_station_scan(uint8_t _set_wifi_mode, uint8_t _select);

//��ȡAPģʽ�²���
bool My_Get_softap_Status(uint8_t _select);

//����APģʽ�µĲ���
bool My_Set_softap_Status(uint8_t* ssid, uint8_t* password,uint8_t _select);

//��ȡAPģʽ���ն˵���Ϣ
void Get_station_info_from_AP(void);

//��ѯ���ض˿���Ϣ
bool Get_localhost_info(uint8_t mode);


//������ user_init �е��ñ��ӿڣ����ӿڱ�����ϵͳ��ʼ����ɺ󣬲��� ESP8266 Station ��
//��ʹ�ܵ�����µ��á�
bool Get_All_Ap_Info(scan_done_cb_t _scan_All_Ap_Info_done);
//��ȡstationģʽ�µ�״̬
bool My_Get_station_Status(uint8_t _select);
//����stationģʽ�µ�״̬
bool My_Set_station_Status(uint8_t* ssid, uint8_t* password,uint8_t _select);
#endif
