#include "driver/softap_udp_tcp.h"

extern struct espconn stcp_Con;

static int ICACHE_FLASH_ATTR My_Pow(int x, int y);
//������ΪXX.XX.XX.XX��IP�ַ���ת����uint8_t[4]������
static bool ICACHE_FLASH_ATTR IpStr2Tab(uint8_t* ip_table,const char * ip_str);

#define FH_NAME 0X55AA  /*Frame Header֡ͷ*/
#define FT_NAME 0XA5  	/*Frame Tail֡ͷ*/
#define DEVICE_ADDR 0X20/*�豸��ַ*/

#define SPEECH_REC_TYPE_PACK		3/*����api��ȡ����*/

/*���ڽ�һ�����ݴ���ɺϸ�����ݰ��ṹ*/
static uint32_t UartDataPacking(char* str, uint8_t type, uint32_t length, uint8_t Check);
/*����ǰ�����*/
static void UartDataFrontPacking(char* str, uint8_t type, uint32_t length);
/*����β�����*/
static void UartDataTailPacking(char* str, uint32_t length, uint8_t Offset, uint8_t Check);

uint16_t str_parse(char *src_data,char *dest_data)
{
	char * pTmp = src_data;
	uint16_t offset = 0;
	uint16_t data_start_offset = 0;
	uint16_t data_end_offset = 0;
	while((*pTmp) != '[') {pTmp++; offset++;}
	while(*pTmp)
	{
		if(pTmp[0] == '[' && pTmp[1] == '\"')
		{
			data_start_offset = offset + 2;
			offset += 1;
			pTmp += 1;
		}

		if(pTmp[1] == ']' && pTmp[0] == '\"')
		{
			data_end_offset = offset;
			break;
		}
		offset++;
		pTmp++;
	}
	uint16_t weight = data_end_offset - data_start_offset;

	UartDataPacking(dest_data, SPEECH_REC_TYPE_PACK,weight,0);

	os_memcpy(dest_data + 7,src_data + data_start_offset, weight);

	return weight;
}

/*���ڽ�һ�����ݴ���ɺϸ�����ݰ��ṹ*/
/*ע��lengthֻ�������ݳ���,�������������ݰ�����*/
static uint32_t UartDataPacking(char* str, uint8_t type, uint32_t length, uint8_t Check)

{
	UartDataFrontPacking(str, type, length);

	UartDataTailPacking(str, length, 7, Check);

	return length + 9;
}

/*����ǰ�����*/
static void UartDataFrontPacking(char* str, uint8_t type, uint32_t length)

{

	str[0] = FH_NAME >> 8;			/*д������֡ͷ�߰�λ*/

	str[1] = FH_NAME & 0Xff;		/*д������֡ͷ�Ͱ�λ*/

	str[2] = DEVICE_ADDR;			/*д���豸��ַ��λ*/

	str[3] = type;/*д�뷢�����ݰ�����*/

	str[4] = (length & 0x00ff0000) >> 16;/*д�����ݳ��ȸ�ʮ��λ*/

	str[5] = (length & 0x0000ff00) >> 8;/*д�����ݳ��ȸ߰�λ*/

	str[6] = length & 0x000000ff;		/*д�����ݳ��ȵͰ�λ*/

}
/*����β�����*/
static void UartDataTailPacking(char* str, uint32_t length, uint8_t Offset, uint8_t Check)

{
	switch(Check)
	{
		case 0:
		str[length + Offset] = 0x00;			/*��ʹ��У��*/
		break;
		case 1:
		break;
		case 2:
		break;
		default:break;
	}
	str[length + Offset + 1] = FT_NAME;		/*д��֡β*/
}

void ICACHE_FLASH_ATTR ESP8266_WIFI_recv_callback(void *arg, char *pdata, unsigned short len)
{
	char str[50] = {0};
	char *pstr = str;

    struct espconn* pEspconn = (struct espconn*)arg;
    remot_info * P_port_info = NULL;	// Զ��������Ϣ�ṹ��ָ��


    if(espconn_get_connection_info(pEspconn, &P_port_info, 0)==ESPCONN_OK)	// ��ȡԶ����Ϣ
	{
		pEspconn->proto.udp->remote_port  = P_port_info->remote_port;		// ��ȡ�Է��˿ں�
		pEspconn->proto.udp->remote_ip[0] = P_port_info->remote_ip[0];		// ��ȡ�Է�IP��ַ
		pEspconn->proto.udp->remote_ip[1] = P_port_info->remote_ip[1];
		pEspconn->proto.udp->remote_ip[2] = P_port_info->remote_ip[2];
		pEspconn->proto.udp->remote_ip[3] = P_port_info->remote_ip[3];

        os_printf("remote_ip:" IPSTR "\n",pEspconn->proto.udp->remote_ip[0],
                    pEspconn->proto.udp->remote_ip[1],
                    pEspconn->proto.udp->remote_ip[2],
                    pEspconn->proto.udp->remote_ip[3]);
	}

        

    //espconn_send(pEspconn,"ESP8266_WIFI_Recv_OK",os_strlen("ESP8266_WIFI_Recv_OK"));	// ��Է�����Ӧ��

    os_printf("remote_port:%d",pEspconn->proto.udp->remote_port);

    os_printf("pdata = %s\n", pdata);

    uint16_t length = str_parse(pdata,pstr) + 9;

    while(length--)
    {
    	uart_tx_one_char(UART0,*pstr);
    	pstr++;
    }
    os_printf("\nESP8266_WIFI_Recelve_OK\n");
}



extern uint8 uartflag;
extern uint16_t g_total;
extern char * g_CurrentPoint;
extern uint8 uartOKflag;
extern char * g_UartPoint1 ;
extern char * g_UartPoint2 ;

void  ESP8266_WIFI_sent_callback(void *arg)
{
    os_printf("\nESP8266_WIFI_Send_OK\n");

}

//����tcp���ӳɹ��ص�����
void ICACHE_FLASH_ATTR ESP8266_WIFI_write_finish_callback(void *arg)
{
//	os_printf("ESP8266_WIFI_write_finish_callback\n");
	 uartOKflag = 1;
	    if(uartflag == 0x81)
	    {
	    	uartflag = 0;
	    	espconn_send(&stcp_Con,g_CurrentPoint,g_total);
	    	g_total = 0;
	    }
	    if(uartflag & 0x40)
	    {

			SET_PERI_REG_MASK(UART_INT_ENA(UART0), UART_RXFIFO_TOUT_INT_ENA);
	    	RTS_FLAG(0);
	        uartOKflag = 0;
	        uartflag = 0x01;
	        os_printf("wait uartOKflag!!!\n");
	        espconn_send(&stcp_Con,((g_CurrentPoint == g_UartPoint1) ? g_UartPoint2 : g_UartPoint1),2800);
	    }
}

//����tcp���ӳɹ��ص�����
void ICACHE_FLASH_ATTR ESP8266_WIFI_connect_callback(void *arg)
{
    os_printf("TCP���ӳɹ�\n");
    espconn_regist_write_finish(&stcp_Con, (espconn_connect_callback) ESP8266_WIFI_write_finish_callback);
    espconn_set_opt(&stcp_Con,0x04);

}

//����tcp�Ͽ����ӻص�����
void ICACHE_FLASH_ATTR ESP8266_WIFI_disconnect_callback(void *arg)
{
    os_printf("TCP���ӶϿ�\n");

}



/*---------------------------------------------------------------------------------*/

esp_udp _esp_udp;
//����udp������
void Create_Udp_Connect_Server(struct espconn* pEspconn, int local_port)
{
    _esp_udp.local_port = local_port;
    pEspconn->proto.udp = &_esp_udp;
    pEspconn->type = ESPCONN_UDP;

	// �ۣ�ע��/����ص�����
    //Ҳ����ʹ��
    //pEspconn->recv_callback = (espconn_recv_callback)ESP8266_WIFI_recv_callback;
    //pEspconn->sent_callback = (espconn_sent_callback)ESP8266_WIFI_sent_callback;
	espconn_regist_sentcb(pEspconn,ESP8266_WIFI_sent_callback);	// ע���������ݷ��ͳɹ��Ļص�����
	espconn_regist_recvcb(pEspconn,ESP8266_WIFI_recv_callback);	// ע���������ݽ��ճɹ��Ļص�����
    espconn_create(pEspconn);
}
//����udp�ͻ���
void Create_Udp_Connect_Client(struct espconn* pEspconn, const char * remote_ip_str, int remote_port)
{
    //_esp_udp.local_port = local_port;
    if(IpStr2Tab(_esp_udp.remote_ip, remote_ip_str) != true) return ;
    os_printf(IPSTR "\n",(uint16)_esp_udp.remote_ip[0],(uint16)_esp_udp.remote_ip[1],(uint16)_esp_udp.remote_ip[2],(uint16)_esp_udp.remote_ip[3]);
    _esp_udp.remote_port = remote_port;
    pEspconn->proto.udp = &_esp_udp;
    pEspconn->type = ESPCONN_UDP;

	// �ۣ�ע��/����ص�����
	//---------------------------------------------------------------------------------------
    //Ҳ����ʹ��
    //pEspconn->recv_callback = (espconn_recv_callback)ESP8266_WIFI_recv_callback;
    //pEspconn->sent_callback = (espconn_sent_callback)ESP8266_WIFI_sent_callback;

	espconn_regist_sentcb(pEspconn,ESP8266_WIFI_sent_callback);	// ע���������ݷ��ͳɹ��Ļص�����
	espconn_regist_recvcb(pEspconn,ESP8266_WIFI_recv_callback);	// ע���������ݽ��ճɹ��Ļص�����
    espconn_create(pEspconn);
}




esp_tcp _esp_tcp;

//����tcp������
void Create_Tcp_Connect_Server(struct espconn* pEspconn, int local_port)
{
    _esp_tcp.local_port = local_port;
    pEspconn->proto.tcp = &_esp_tcp;
    pEspconn->type = ESPCONN_TCP;

	// �ۣ�ע��/����ص�����
    //Ҳ����ʹ��
    //pEspconn->recv_callback = (espconn_recv_callback)ESP8266_WIFI_recv_callback;
    //pEspconn->sent_callback = (espconn_sent_callback)ESP8266_WIFI_sent_callback;
	espconn_regist_sentcb(pEspconn, ESP8266_WIFI_sent_callback);	// ע���������ݷ��ͳɹ��Ļص�����
	espconn_regist_recvcb(pEspconn, ESP8266_WIFI_recv_callback);	// ע���������ݽ��ճɹ��Ļص�����
    espconn_accept(pEspconn);

    //ע��
    espconn_regist_time(pEspconn, 3000, 0);

    //ע�����ӳɹ��ص�����
    espconn_regist_connectcb(pEspconn,(espconn_connect_callback)ESP8266_WIFI_connect_callback);

    //ע��Ͽ����ӻص�����
    espconn_regist_disconcb(pEspconn, (espconn_connect_callback)ESP8266_WIFI_disconnect_callback);

}


//����tcp�ͻ���
void Create_Tcp_Connect_Client(struct espconn* pEspconn, const char * remote_ip_str, int remote_port)
{

    _esp_tcp.local_port = espconn_port();
    if(IpStr2Tab(_esp_tcp.remote_ip, remote_ip_str) != true) return ;
    os_printf(IPSTR "\n",_esp_tcp.remote_ip[0],_esp_tcp.remote_ip[1],_esp_tcp.remote_ip[2],_esp_tcp.remote_ip[3]);
    _esp_tcp.remote_port = remote_port;
    pEspconn->proto.tcp = &_esp_tcp;
    pEspconn->type = ESPCONN_TCP;

	// �ۣ�ע��/����ص�����
	//---------------------------------------------------------------------------------------
    //Ҳ����ʹ��
    //pEspconn->recv_callback = (espconn_recv_callback)ESP8266_WIFI_recv_callback;
    //pEspconn->sent_callback = (espconn_sent_callback)ESP8266_WIFI_sent_callback;

	espconn_regist_sentcb(pEspconn, ESP8266_WIFI_sent_callback);	// ע���������ݷ��ͳɹ��Ļص�����
	espconn_regist_recvcb(pEspconn, ESP8266_WIFI_recv_callback);	// ע���������ݽ��ճɹ��Ļص�����
    espconn_connect(pEspconn);

    espconn_regist_connectcb(pEspconn,(espconn_connect_callback)ESP8266_WIFI_connect_callback);
    
        //ע��Ͽ����ӻص�����
    espconn_regist_disconcb(pEspconn, (espconn_connect_callback)ESP8266_WIFI_disconnect_callback);
}






//�l��wifi����
void Send_Wifi_Data(const char* str)
{
    espconn_send(&stcp_Con,str,os_strlen(str));
}

// ������ΪXX.XX.XX.XX��IP�ַ���ת����uint8_t[4]������
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
    os_printf("��ȡ�Ĳ���:" IPSTR "\n",(uint16)temp[0],(uint16)temp[1],(uint16)temp[2],(uint16)temp[3]);
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
