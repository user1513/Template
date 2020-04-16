#include "driver/softap_udp_tcp.h"

extern struct espconn stcp_Con;

static int ICACHE_FLASH_ATTR My_Pow(int x, int y);
//将类型为XX.XX.XX.XX的IP字符串转换成uint8_t[4]的数组
static bool ICACHE_FLASH_ATTR IpStr2Tab(uint8_t* ip_table,const char * ip_str);



void ICACHE_FLASH_ATTR ESP8266_WIFI_recv_callback(void *arg, char *pdata, unsigned short len)
{
    struct espconn* pEspconn = (struct espconn*)arg;
    remot_info * P_port_info = NULL;	// 远端连接信息结构体指针


    if(espconn_get_connection_info(pEspconn, &P_port_info, 0)==ESPCONN_OK)	// 获取远端信息
	{
		pEspconn->proto.udp->remote_port  = P_port_info->remote_port;		// 获取对方端口号
		pEspconn->proto.udp->remote_ip[0] = P_port_info->remote_ip[0];		// 获取对方IP地址
		pEspconn->proto.udp->remote_ip[1] = P_port_info->remote_ip[1];
		pEspconn->proto.udp->remote_ip[2] = P_port_info->remote_ip[2];
		pEspconn->proto.udp->remote_ip[3] = P_port_info->remote_ip[3];

        os_printf("remote_ip:" IPSTR "\n",pEspconn->proto.udp->remote_ip[0],
                    pEspconn->proto.udp->remote_ip[1],
                    pEspconn->proto.udp->remote_ip[2],
                    pEspconn->proto.udp->remote_ip[3]);
	}

        

    espconn_send(pEspconn,"ESP8266_WIFI_Recv_OK",os_strlen("ESP8266_WIFI_Recv_OK"));	// 向对方发送应答

    os_printf("remote_port:%d",pEspconn->proto.udp->remote_port);

    os_printf("pdata = %s\n", pdata);

    if(!strcmp(pdata,"open"))
    {
        os_printf("开灯!!!!\n");
    }
    if(!strcmp(pdata,"close"))
    {
        os_printf("关灯!!!!\n");
    }
    os_printf("\nESP8266_WIFI_Recelve_OK\n");
}

void ICACHE_FLASH_ATTR ESP8266_WIFI_sent_callback(void *arg)
{
    os_printf("\nESP8266_WIFI_Send_OK\n");
}

//连接tcp连接成功回调函数
void ICACHE_FLASH_ATTR ESP8266_WIFI_connect_callback(void *arg)
{
    os_printf("TCP连接成功\n");

    os_printf("GET http://www.openedv.com/ HTTP/1.1\r\nHost: http://www.openedv.com/\r\nConnection: keep-alive\r\n\r\n\n");
    Send_Wifi_Data("GET http://www.openedv.com/ HTTP/1.1\r\nHost: http://www.openedv.com/\r\nConnection: keep-alive\r\n\r\n");

}

//连接tcp断开连接回调函数
void ICACHE_FLASH_ATTR ESP8266_WIFI_disconnect_callback(void *arg)
{
    os_printf("TCP连接断开\n");

}



/*---------------------------------------------------------------------------------*/

esp_udp _esp_udp;
//创建udp服务器
void Create_Udp_Connect_Server(struct espconn* pEspconn, int local_port)
{
    _esp_udp.local_port = local_port;
    pEspconn->proto.udp = &_esp_udp;
    pEspconn->type = ESPCONN_UDP;

	// ③：注册/定义回调函数
    //也可以使用
    //pEspconn->recv_callback = (espconn_recv_callback)ESP8266_WIFI_recv_callback;
    //pEspconn->sent_callback = (espconn_sent_callback)ESP8266_WIFI_sent_callback;
	espconn_regist_sentcb(pEspconn,ESP8266_WIFI_sent_callback);	// 注册网络数据发送成功的回调函数
	espconn_regist_recvcb(pEspconn,ESP8266_WIFI_recv_callback);	// 注册网络数据接收成功的回调函数
    espconn_create(pEspconn);
}
//创建udp客户端
void Create_Udp_Connect_Client(struct espconn* pEspconn, const char * remote_ip_str, int remote_port)
{
    //_esp_udp.local_port = local_port;
    if(IpStr2Tab(_esp_udp.remote_ip, remote_ip_str) != true) return ;
    os_printf(IPSTR "\n",(uint16)_esp_udp.remote_ip[0],(uint16)_esp_udp.remote_ip[1],(uint16)_esp_udp.remote_ip[2],(uint16)_esp_udp.remote_ip[3]);
    _esp_udp.remote_port = remote_port;
    pEspconn->proto.udp = &_esp_udp;
    pEspconn->type = ESPCONN_UDP;

	// ③：注册/定义回调函数
	//---------------------------------------------------------------------------------------
    //也可以使用
    //pEspconn->recv_callback = (espconn_recv_callback)ESP8266_WIFI_recv_callback;
    //pEspconn->sent_callback = (espconn_sent_callback)ESP8266_WIFI_sent_callback;

	espconn_regist_sentcb(pEspconn,ESP8266_WIFI_sent_callback);	// 注册网络数据发送成功的回调函数
	espconn_regist_recvcb(pEspconn,ESP8266_WIFI_recv_callback);	// 注册网络数据接收成功的回调函数
    espconn_create(pEspconn);
}


esp_tcp _esp_tcp;



//创建tcp服务器
void Create_Tcp_Connect_Server(struct espconn* pEspconn, int local_port)
{
    _esp_tcp.local_port = local_port;
    pEspconn->proto.tcp = &_esp_tcp;
    pEspconn->type = ESPCONN_TCP;

	// ③：注册/定义回调函数
    //也可以使用
    //pEspconn->recv_callback = (espconn_recv_callback)ESP8266_WIFI_recv_callback;
    //pEspconn->sent_callback = (espconn_sent_callback)ESP8266_WIFI_sent_callback;
	espconn_regist_sentcb(pEspconn, ESP8266_WIFI_sent_callback);	// 注册网络数据发送成功的回调函数
	espconn_regist_recvcb(pEspconn, ESP8266_WIFI_recv_callback);	// 注册网络数据接收成功的回调函数
    espconn_accept(pEspconn);

    //注册
    espconn_regist_time(pEspconn, 3000, 0);

    //注册连接成功回调函数
    espconn_regist_connectcb(pEspconn,(espconn_connect_callback)ESP8266_WIFI_connect_callback);

    //注册断开连接回调函数
    espconn_regist_disconcb(pEspconn, (espconn_connect_callback)ESP8266_WIFI_disconnect_callback);

}


//创建tcp客户端
void Create_Tcp_Connect_Client(struct espconn* pEspconn, const char * remote_ip_str, int remote_port)
{

    _esp_tcp.local_port = espconn_port();
    if(IpStr2Tab(_esp_tcp.remote_ip, remote_ip_str) != true) return ;
    os_printf(IPSTR "\n",_esp_tcp.remote_ip[0],_esp_tcp.remote_ip[1],_esp_tcp.remote_ip[2],_esp_tcp.remote_ip[3]);
    _esp_tcp.remote_port = remote_port;
    pEspconn->proto.tcp = &_esp_tcp;
    pEspconn->type = ESPCONN_TCP;

	// ③：注册/定义回调函数
	//---------------------------------------------------------------------------------------
    //也可以使用
    //pEspconn->recv_callback = (espconn_recv_callback)ESP8266_WIFI_recv_callback;
    //pEspconn->sent_callback = (espconn_sent_callback)ESP8266_WIFI_sent_callback;

	espconn_regist_sentcb(pEspconn, ESP8266_WIFI_sent_callback);	// 注册网络数据发送成功的回调函数
	espconn_regist_recvcb(pEspconn, ESP8266_WIFI_recv_callback);	// 注册网络数据接收成功的回调函数
    espconn_connect(pEspconn);

    espconn_regist_connectcb(pEspconn,(espconn_connect_callback)ESP8266_WIFI_connect_callback);
    
        //注册断开连接回调函数
    espconn_regist_disconcb(pEspconn, (espconn_connect_callback)ESP8266_WIFI_disconnect_callback);
}






//發送wifi數據
void Send_Wifi_Data(const char* str)
{
    espconn_send(&stcp_Con,str,os_strlen(str));
}

// 将类型为XX.XX.XX.XX的IP字符串转换成uint8_t[4]的数组
static bool IpStr2Tab(uint8_t* ip_table,const char * ip_str)
{
    int i = 0;
    uint8_t temp[4] = {0,0,0,0};
    for(; i < 4; i++)
    {
        while((*ip_str != '.') && (*ip_str != '\0'))
        {
            if((*ip_str >= '0') && (*ip_str <= '9'))
            {
                temp[i] = My_Pow(temp[i],1) + (*ip_str - '0');
            }
  
            ip_str++;   
        } 
        ip_str++;    
    }  
    if(i != 4)
        return false;
    os_printf("获取的参数:" IPSTR "\n",(uint16)temp[0],(uint16)temp[1],(uint16)temp[2],(uint16)temp[3]);
    os_memcpy(ip_table, temp, sizeof(temp));
    return true;
}

static int My_Pow(int x, int y)
{
    while(y--)
    {
        x *= 10; 
    }
    return x;
} 
