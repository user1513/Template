#ifndef _SOFTAP_UDP_TCP_H
#define _SOFTAP_UDP_TCP_H

#include "inc.h"
//创建udp服务器
void Create_Udp_Connect_Server(struct espconn* pEspconn, int local_port);
//创建udp客户端
void Create_Udp_Connect_Client(struct espconn* pEspconn, const char * remote_ip_str, int remote_port);
//建立tcp服务器
void Create_Tcp_Connect_Server(struct espconn* pEspconn, int local_port);
//创建tcp客户端
void Create_Tcp_Connect_Client(struct espconn* pEspconn, const char * remote_ip_str, int remote_port);
//發送wifi數據
void Send_Wifi_Data(const char* str);
#endif