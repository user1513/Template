#ifndef _SOFTAP_UDP_TCP_H
#define _SOFTAP_UDP_TCP_H

#include "inc.h"
//����udp������
void Create_Udp_Connect_Server(struct espconn* pEspconn, int local_port);
//����udp�ͻ���
void Create_Udp_Connect_Client(struct espconn* pEspconn, const char * remote_ip_str, int remote_port);
//����tcp������
void Create_Tcp_Connect_Server(struct espconn* pEspconn, int local_port);
//����tcp�ͻ���
void Create_Tcp_Connect_Client(struct espconn* pEspconn, const char * remote_ip_str, int remote_port);
//�l��wifi����
void Send_Wifi_Data(const char* str);
#endif