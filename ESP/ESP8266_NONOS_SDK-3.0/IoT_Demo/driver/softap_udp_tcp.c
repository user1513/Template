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

        

    //espconn_send(pEspconn,"ESP8266_WIFI_Recv_OK",os_strlen("ESP8266_WIFI_Recv_OK"));	// 向对方发送应答

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
    Send_Wifi_Data("\
POST http://vop.baidu.com/server_api HTTP/1.1\r\n\
Host: vop.baidu.com\r\n\
Connection: keep-alive\r\n\
Content-Type: application/json\r\n\
Content-length: 3115\r\n\r\n\
{\"format\":\"pcm\",\"rate\":16000,\"dev_pid\":1537,\"channel\":1,\"token\":\"24.91af5b7738399578bc9c7920f4558d06.2592000.1589172592.282335-19369127\",\"cuid\":\"DC4A3EE6B0D9\",\"len\":156000,\"speech\":\"ZQNQA2oDOQMlA84CxQIaAwED5wJsAkUDLAPtAukCIAM4A9gC7wKTAtkCAQOIAtACtwKRAloCQAIuAnwCOAIGAjgCVQJnAkICNAL3AUYCjQLmARgCJQIuAhwCRwJsAngCLAImAqUCvQJiApcCfgLyAukC0ALUAiEDPAMSAxYDKwPuAgsDrALfAtsC0AJpAt4C0QK6AqsClgJtArEC2QK3Ao4CPAIQAnEC8QFCAjYCgQJAApcCUQJfAl0COgI9AugBDgIGAjECfgJGAp0CuAJ8AqACrQKcArECpALQAqkC8wI3A2gDcQN9AwwE/QMCBHEEeQRaBDIEOwR0BIgEVQRkBL4ExQTeBP0ErATNBJwEjASBBHMEEQRZBG4EcQTJBG0EdgRTBGMEIAT5A80DiAMUBPQDVARwBHQELgRJBA8EYgQkBAMEHAT+AyYEAAQ9BO0D3gNmA9YDgAObA50DKgMjAzkDZQM8AzcDRwN1A48DiwPTA2wDzAPBA7UD1QOEA4UDjQOkA7UDSgNIA20DIAMeAykDSAMiAyEDEwNQAwoDMAPhAqgCuQKmAvoCvgKxApIC0wJmAu4CrwLSAsQCuAKmAoACtQJ8Ar8CeQLJAooCUQJfAhQCMQJXAncCnQLHAsUC8gL/Aq4CwQLcAo0CWAKEAkACbwIlAiACaQKLAlQCWAKJAk8CcAJCAosC0QLCAvQCvgLBArUCxAKUAogCvwK3AlUCOgIzAlcCZwI5AiUCHgIFAu4BBAILAmcCZQKDAjcC4gKyAsICygKMArsCvAK3An8C+QJ+ArQCYQKgAooCkwLIAnMCzQKUAsUC4QLjAtEC1wLpAsYCsAI9AnkCVAJtAmsCfAKqAq0CmwKpAgwDtgKmArMCbQI1AisCHAIxAhoCJAJLArQCsAIRAxoDYANhA3oDTwP8AukCkAKJApQCtgKsArICrgK/AvcCxgLNAs4C3gLTApkChwJMAlECVAKLApIC6QLeAvIC7wIQAzQD9QLVAuACJwP4AjADBQMeA1cDEAMCAwED4AKJAgoDrwLoAgkDtQL4AuQCrwK5ApsChQKzAlQCdwKCAqECnQJ7AtsC/ALVAr8CvQJ7AnkCfgKIAsACmALdAtgCOAP/AgAD7wIsA94C9QK+As0CywJgArcCxgLIAqkCpwLMAusC2QLRAu4CBwMlAyoDaANwA6QDYgOLA4oDkwN6A0oDSwMoAx4DCAP1Ah8DMgMRAyoDdQNQA1oDPAMzAyADHgMVAzcDUQP1AhkDDgMnA5wCxwK0AsgCuAKIAqYCvgL+Au0ChgNrA2sDfgNeA0kD4gLaAu8CFQP7Ah8DNwNiA2EDWwM+AwID7wL5At0CsQK5AuACvQIAA9cC7QKzAq0ChwLCAngCZAJeAlICjAJDAnwCZwJ4AoACgwKBAnkCYQKdAnUCjQKFAlgCOwI8Ak4CNQKPAjQCagJJAj0CPwITAvsBHwIpAjoCbQJMAmoCTwJSAl4CWQJWAiMCSAJHAmQCcwKLAmwCugLTAs0CygL9AhED6gL2AvwCBwPsAt8CxgL0AtgCsgLmAr0CkQJEAkUCVgJ2AkcCHQJQAjoCKgJHAjECUwJYAl4CkAKEAkYCSgJGAlAChAJfAnICYgJiAjECKAJPAiMCTAI4AnUCSwI6AkECWAJcAmYCZwKHApQClQKHArMC5ALOArwCoAK0AqEC1wLDAt8CBwMMAwYD5wLVAusC5gLnAuoCFAMwA+0CBgP9Ag4D2gK0AssC2QKrArsC9gIIAzQDIAMeAycDKgMaA/kC5wKrAswCrAKIAtYC0gIRA/wC6gLoAgwDCQMWAyoDHQMeAwoDCwMpA14DWgNZA24DaQN1AykDNQNDAzkDSgNoA18DXANgA0EDQwMdA4MDVANqA5MDjgOZA3IDMANVA0MDGQMYAyIDVgMvAz8DQANZA00DVQM6A0YDGAMYAxID+gIyAwkDEAMtA/4CwgKaAmwCYQJjAkkCegJbAnYCfAKeArQC4AIUA0QDZwM1AykDPgNDAxsD/QIEAwEDAAMZA1sDSgN1AzcDbQOhA28DcANWA4IDbwM3AzsDVwNeA3QDaQNpA4oDWQNAAycDKgM5Ax0DMQM/AzwDOQNQA4UDoAPFA9EDyAO2A4cDaQNvA18DigO4A6YDhwOLA2UDPQNDAywDFQM8Az4DTQNLAzsDGwM1AwEDGQP4AsgCvwL+Av0C+QISAwgDFwMeAzwDLQM8A1oDdgOvA2sDhAOPA54DmAPLA8sDAATWA8UDvAOqA7oDqgOOA3ADOwM7AzwDUQMwAzoDMwM5AyID5wL7AtQC5wL6ArYCqAKcAtMCvQLGAt0CygLKAu8C7wIEA/gC3QLVAsEC4gKWAn0CqgKmApQCyQK2AsACkQJRAn4CawJkAi4CUQJ9ApYChgKFApsCwwK6ArgCzwLTAugC7ALzAswCxwLQAqYCsQJ9AmIChwK/AqQCyQKzAsgCAAMSAwcDDQPpAh0D0QLVAuUC8gIlAwkDPgNLAxwDFwP+AtsC5gKiArYCpwK7AtACvAK1AuoC8gLvAvQCLgN/A3kDVgM9A00DFwMNAxgDNwMWAzMDTgNxA3YDaANTA1wDKAP3Av8CIAMoAxwDFgP9AukCAwPsAhwDGANAA1IDYgOOA7oDjgOqA7QDmwO4A5QDmgOGA2kDgwOYA4wDqAOaA6EDtQOrA6sDpgN7A3gDdANuA1wDZQN4A2ADjgN5A2oDcANbA0cDaAOMA1oDZQNlA3kDZANeAzgDNQNDAxADAgP6AtgCvQLMAtkC5gIKA6UCjQKPAqoC0gK5ApUCpgKlApQClAKmAn8CfQKVAqMCrQKdAnkCYAJ7ApECagJPAmUCWAJwAn0CfgJ1Am4CXwIzAjoCMQJOAksCSAJFAkkCHgJQAlECOQJbAkACQgJUAn8CogK0AtIC1wL\"}");
    os_printf("test\n");
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
