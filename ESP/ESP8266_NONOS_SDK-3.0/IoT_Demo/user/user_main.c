#include "driver/inc.h"






//==============================

// ���Ͷ���
//=================================
typedef unsigned long 		u32_t;

//=================================


// ȫ�ֱ���
//============================================================================
MQTT_Client mqttClient;			// MQTT�ͻ���_�ṹ�塾�˱����ǳ���Ҫ��
uint32_t g_ultotal = 0;

// MQTT�ѳɹ����ӣ�ESP8266���͡�CONNECT���������յ���CONNACK��
//============================================================================
void mqttConnectedCb(uint32_t *args)
{
    MQTT_Client* client = (MQTT_Client*)args;	// ��ȡmqttClientָ��

    INFO("MQTT: Connected\r\n");

    // ������2����������� / ����3������Qos��
    //-----------------------------------------------------------------
	MQTT_Subscribe(client, "SW_LED", 0);	// ��������"SW_LED"��QoS=0
//	MQTT_Subscribe(client, "SW_LED", 1);
//	MQTT_Subscribe(client, "SW_LED", 2);

	// ������2�������� / ����3��������Ϣ����Ч�غ� / ����4����Ч�غɳ��� / ����5������Qos / ����6��Retain��
	//-----------------------------------------------------------------------------------------------------------------------------------------
//	MQTT_Publish(client, "SW_LED", "ESP8266_Online", strlen("ESP8266_Online"), 0, 0);	// ������"SW_LED"����"ESP8266_Online"��Qos=0��retain=0
//	MQTT_Publish(client, "SW_LED", "ESP8266_Online", strlen("ESP8266_Online"), 1, 0);
//	MQTT_Publish(client, "SW_LED", "ESP8266_Online", strlen("ESP8266_Online"), 2, 0);
	//MQTT_Subscribe(client, "Will", 0);

//	MQTT_Publish(client, "SW_LED", "ESP8266_Online", strlen("ESP8266_Online"), 0, 0);
}
//============================================================================

// MQTT�ɹ��Ͽ�����
//============================================================================
void mqttDisconnectedCb(uint32_t *args)
{
    MQTT_Client* client = (MQTT_Client*)args;
    INFO("MQTT: Disconnected\r\n");
}
//============================================================================

// MQTT�ɹ�������Ϣ
//============================================================================
void mqttPublishedCb(uint32_t *args)
{
    MQTT_Client* client = (MQTT_Client*)args;
    INFO("MQTT: Published\r\n");
}
//============================================================================

// ������MQTT��[PUBLISH]���ݡ�����		������1������ / ����2�����ⳤ�� / ����3����Ч�غ� / ����4����Ч�غɳ��ȡ�
//===============================================================================================================
void mqttDataCb(uint32_t *args, const char* topic, uint32_t topic_len, const char *data, uint32_t data_len)
{
    char *topicBuf = (char*)os_zalloc(topic_len+1);		// ���롾���⡿�ռ�
    char *dataBuf  = (char*)os_zalloc(data_len+1);		// ���롾��Ч�غɡ��ռ�


    MQTT_Client* client = (MQTT_Client*)args;	// ��ȡMQTT_Clientָ��


    os_memcpy(topicBuf, topic, topic_len);	// ��������
    topicBuf[topic_len] = 0;				// �����'\0'

    os_memcpy(dataBuf, data, data_len);		// ������Ч�غ�
    dataBuf[data_len] = 0;					// �����'\0'

    INFO("Receive topic: %s, data: %s \r\n", topicBuf, dataBuf);	// ���ڴ�ӡ�����⡿����Ч�غɡ�
    INFO("Topic_len = %d, Data_len = %d\r\n", topic_len, data_len);	// ���ڴ�ӡ�����ⳤ�ȡ�����Ч�غɳ��ȡ�


// ����С�¡����
//########################################################################################
    // ���ݽ��յ���������/��Ч�غɣ�����LED����/��
    //-----------------------------------------------------------------------------------
    if( os_strcmp(topicBuf,"SW_LED") == 0 && data_len == 4)			// ���� == "SW_LED"
    {
    	char uartsendstr[100];
		UartDataPacking(uartsendstr, 0x07, data_len, 0);
		os_memcpy(&uartsendstr[7], data, data_len);
		for(int i = 0; i < (9 + data_len); i++)
		{
			uart_tx_one_char(UART0, uartsendstr[i]);
		}
    }
//########################################################################################


    os_free(topicBuf);	// �ͷš����⡿�ռ�
    os_free(dataBuf);	// �ͷš���Ч�غɡ��ռ�
}
//�����ʱ���ṹ��
os_timer_t led_os_timer;
//os_timer_t udp_start_os_timer;
os_timer_t wifi_statue_timer;		/*�ϵ��ϵ��������Զ�����һ������״̬,���ϵ�20s������ʧ���Զ���������ʧ��״̬֮��ÿ��10s����״ֱ̬�������ɹ�ֹͣ���ݷ���*/
os_timer_t sntp_timer;
//
struct espconn stcp_Con;

void  wifiConnectCb(uint8_t status);
void Led_Soft_Timer_Handle(void * reg);
void wifi_statue_timer_Handle (void * reg);
void ICACHE_FLASH_ATTR user_check_sntp_stamp(void *arg);

uint8_t wifi_statue_flag = 0;
GetDnsInfo g_DnsInfo[2] ={
{"vop.baidu.com","0",80},
{"www.baidu.com","0",80}};

struct sntp_No_str tSntp[3] = {
{0,"cn.pool.ntp.org"},
{1,"asia.pool.ntp.org"},
{2,"cn.ntp.org.cn"}};

uint8_t * wifi_statue_str = NULL;//���ڴ������״̬���ݰ�

void ICACHE_FLASH_ATTR user_init(void)
{
	// �ܽŹ���ѡ��(ע�⣺����1��PIN_NAME���ܽ���������2��FUNC���ܽŹ���)
	//--------------------------------------------------------------------------
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_SD_DATA3_U,	FUNC_GPIO10);	// GPIO4��ΪIO��

	// ����Ӧ�ܽ���Ϊ���ģʽ���������Ӧ��ƽ(����1��gpio_no��������2�������ƽ)
	//--------------------------------------------------------------------------
	GPIO_OUTPUT_SET(GPIO_ID_PIN(10),0);			// IO4 = 1(�ߵ�ƽ)

    UART_SetPrintPort(UART1);
        //���ô��ڳ�ʼ��
	uart_init(BIT_RATE_460800,BIT_RATE_460800);
    os_printf("READ_PERI_REG1:%d\n", READ_PERI_REG(UART_CONF1(UART0)));
    /*����full��ֵΪ100*/
    WRITE_PERI_REG(UART_CONF1(UART0),((100 & UART_RXFIFO_FULL_THRHD) << UART_RXFIFO_FULL_THRHD_S) |
    (READ_PERI_REG(UART_CONF1(UART0)) & (~UART_RXFIFO_FULL_THRHD)));
    os_printf("READ_PERI_REG2:%d\n", READ_PERI_REG(UART_CONF1(UART0)));
    /*����timeout��ֵΪ100,��bps = 460800�� Ϊ1.736ms�����ж�*/
    WRITE_PERI_REG(UART_CONF1(UART0),((125 & UART_RX_TOUT_THRHD) << UART_RX_TOUT_THRHD_S) |
    (READ_PERI_REG(UART_CONF1(UART0)) & (~(UART_RX_TOUT_THRHD<< UART_RX_TOUT_THRHD_S))));
    os_printf("READ_PERI_REG3:%d\n", READ_PERI_REG(UART_CONF1(UART0)));
    /*���ý��ճ�ʱ�ж�,����֡�����ж�,��full�ж�*/
    SET_PERI_REG_MASK(UART_INT_ENA(UART0), UART_RXFIFO_TOUT_INT_ENA | UART_FRM_ERR_INT_ENA | UART_RXFIFO_FULL_INT_ENA);
    os_printf("SET_PERI_REG_MASK:%d\n", READ_PERI_REG(UART_INT_ENA(UART0)));

    //��ӡsdk�汾��Ϣ
    os_printf("SDK version:%s\n", system_get_sdk_version());
    //led��ʼ��
    bsp_led_init();
    
    softap_station_scan(STATION_MODE, 1);

    /*APģʽ������
    //����AP ,SAT, AP + SAT
    softap_station_scan(SOFTAP_MODE, 1)
    //��ȡApģʽ�µĲ�����Ϣ
    My_Get_softap_Status(1);
    //����APģʽ�µĲ���
    My_Set_softap_Status("ILOVEYOU", "123456789", 1);
    */
    
    //���������ʱ
	wifi_statue_str = (uint8_t *)os_malloc( sizeof(uint8_t) * 10);/*���ڴ�ż������״̬�����ݰ�*/
	
	UartDataPacking(wifi_statue_str, 0x05, 1, 0);
	
	wifi_statue_str[6] = 1;

    Led_SoftTimer_init(&led_os_timer,Led_Soft_Timer_Handle,500);

    //bsp_SoftTimer_init(&udp_start_os_timer,udp_Soft_Timer_Handle,2000,1,1);

	bsp_SoftTimer_init(&wifi_statue_timer, wifi_statue_timer_Handle, 20000, (uint32_t)wifi_statue_str, 0);


	CFG_Load();	// ����/����ϵͳ������WIFI������MQTT������


	// �������Ӳ�����ֵ�������������mqtt_test_jx.mqtt.iot.gz.baidubce.com�����������Ӷ˿ڡ�1883������ȫ���͡�0��NO_TLS��
	//-------------------------------------------------------------------------------------------------------------------
	MQTT_InitConnection(&mqttClient, sysCfg.mqtt_host, sysCfg.mqtt_port, sysCfg.security);

	// MQTT���Ӳ�����ֵ���ͻ��˱�ʶ����..����MQTT�û�����..����MQTT��Կ��..������������ʱ����120s��������Ự��1��clean_session��
	//----------------------------------------------------------------------------------------------------------------------------
	MQTT_InitClient(&mqttClient, sysCfg.device_id, sysCfg.mqtt_user, sysCfg.mqtt_pass, sysCfg.mqtt_keepalive, 0);

	// ������������(����ƶ�û�ж�Ӧ���������⣬��MQTT���ӻᱻ�ܾ�)
	//--------------------------------------------------------------
	//MQTT_InitLWT(&mqttClient, "Will", "ESP8266_Offline", 0, 0);


	// ����MQTT��غ���
	//--------------------------------------------------------------------------------------------------
	MQTT_OnConnected(&mqttClient, mqttConnectedCb);			// ���á�MQTT�ɹ����ӡ���������һ�ֵ��÷�ʽ
	MQTT_OnDisconnected(&mqttClient, mqttDisconnectedCb);	// ���á�MQTT�ɹ��Ͽ�����������һ�ֵ��÷�ʽ
	MQTT_OnPublished(&mqttClient, mqttPublishedCb);			// ���á�MQTT�ɹ���������������һ�ֵ��÷�ʽ
	MQTT_OnData(&mqttClient, mqttDataCb);					// ���á�����MQTT���ݡ���������һ�ֵ��÷�ʽ


	// ����WIFI��SSID[..]��PASSWORD[..]��WIFI���ӳɹ�����[wifiConnectCb]
	//--------------------------------------------------------------------------
	WIFI_Connect(sysCfg.sta_ssid, sysCfg.sta_pwd, wifiConnectCb);


	INFO("\r\nSystem started ...\r\n");

}



void wifi_statue_timer_Handle (void * reg)

{
	uint8_t * ulpStr = (uint8_t *)reg;

	if(wifi_statue_flag == 1)
	{
		os_printf("ESP�����ɹ�\n");
		os_free(wifi_statue_str);
		bsp_SoftTimer_close(&wifi_statue_timer);
	}
	else
	{
		os_printf("ESP����ʧ��\n");
		for(int i = 0; i < 10; i++)
		{
			uart_tx_one_char(UART0, ulpStr[i]);
		}
		//multiple_dns_parse(&g_DnsInfo[sizeof(g_DnsInfo)/sizeof(g_DnsInfo[0]) - 1], &stcp_Con, sizeof(g_DnsInfo)/sizeof(g_DnsInfo[0]));
		bsp_SoftTimer_Restart(&wifi_statue_timer,5000,(uint32_t)ulpStr, 0);
	}
	
}

void Led_Soft_Timer_Handle(void * reg)
{
    bsp_led_flash();
    os_printf("RSSI:%d total:%d\n", (int)wifi_station_get_rssi(), (int)g_ultotal);
}

void  wifiConnectCb(uint8_t status)
{
    if(status == STATION_GOT_IP)
    {
		my_sntp_init(tSntp);
		os_timer_disarm(&sntp_timer);
		os_timer_setfn(&sntp_timer, (os_timer_func_t *)user_check_sntp_stamp, NULL);
		os_timer_arm(&sntp_timer, 1000, 1);
    }

    // IP��ַ��ȡʧ��
	//----------------------------------------------------------------
    else
    {
          MQTT_Disconnect(&mqttClient);	// WIFI���ӳ���TCP�Ͽ�����
    }
}

// void scan_All_Ap_Info_done(void *arg, STATUS status)
// {
//     struct bss_info* pBss_info = (struct bss_info*)arg;
//     switch (status)
//     {
//     case OK:os_printf("ɨ������AP���ò���--�ɹ�\n");break;
//     case FAIL:os_printf("ɨ������AP���ò���--ʧ��\n");break;
//     case PENDING:os_printf("ɨ������AP���ò���--������\n");break;
//     case BUSY:os_printf("ɨ������AP���ò���--��æ\n");break;
//     case CANCEL:os_printf("ɨ������AP���ò���--ȡ��\n");break;
//     default:break;
//     }
//     if(status == OK)
//     {
//         while (pBss_info)
//         {
//             os_printf("AP��:%s\t\t\t", pBss_info->ssid);
//             os_printf("ͨ��:%d\t  ", pBss_info->channel);
//             os_printf("����:%d\t  ", pBss_info->is_hidden);
//             os_printf("���ܷ�ʽ:%d\n", pBss_info->authmode);
//             pBss_info = pBss_info->next.stqe_next;
//         }

//         My_Set_station_Status("2601b-2.4G", "19728888", 1);
//         bsp_SoftTimer_Restart(&udp_start_os_timer, 100, (void *)2, 1);
//     }
// }




// SNTP��ʱ��������ȡ��ǰ����ʱ��
void ICACHE_FLASH_ATTR user_check_sntp_stamp(void *arg)
{
    uint32 current_stamp = 1;

    current_stamp = sntp_get_current_timestamp();

	os_printf("current time : %s\n", sntp_get_real_time(current_stamp));	// ��ȡ��ʵʱ��

    if(current_stamp == 0)
    {
		os_printf("did not get a valid time from sntp server\n");
    } 
    else
    {
        os_timer_disarm(&sntp_timer);	// �ر�SNTP��ʱ��

        MQTT_Connect(&mqttClient);		// ��ʼMQTT����

		multiple_dns_parse(&g_DnsInfo[sizeof(g_DnsInfo)/sizeof(g_DnsInfo[0]) - 1], &stcp_Con, sizeof(g_DnsInfo)/sizeof(g_DnsInfo[0]));
    }
}






/******************************************************************************
 * FunctionName : uart0_rx_intr_handler
 * Description  : Internal used function
 *                UART0 interrupt handler, add self handle code inside
 * Parameters   : void *para - point to ETS_UART_INTR_ATTACH's arg
 * Returns      : NONE
*/

uint16_t g_total = 0;
char * g_CurrentPoint = NULL;
char * g_UartPoint1 = NULL;
char * g_UartPoint2 = NULL;
char uartstr[128];
uint8 uartflag = 0;
uint8 uartOKflag = 1;
uint32_t mqtt_rec_length ;
void uart0_rx_intr_handler(void *para)
{

	uint8 RcvChar;
	uint8 uart_no = UART0;//UartDev.buff_uart_no;
	uint8 fifo_len = 0;
	uint8 buf_idx = 0;
	uint8 temp,cnt;
	char * pTmp = NULL;
	//RcvMsgBuff *pRxBuff = (RcvMsgBuff *)para;

		/*ATTENTION:*/
	/*IN NON-OS VERSION SDK, DO NOT USE "ICACHE_FLASH_ATTR" FUNCTIONS IN THE WHOLE HANDLER PROCESS*/
	/*ALL THE FUNCTIONS CALLED IN INTERRUPT HANDLER MUST BE DECLARED IN RAM */
	/*IF NOT , POST AN EVENT AND PROCESS IN SYSTEM TASK */
	if(UART_FRM_ERR_INT_ST == (READ_PERI_REG(UART_INT_ST(uart_no)) & UART_FRM_ERR_INT_ST)){
		os_printf("FRM_ERR\r\n");
		WRITE_PERI_REG(UART_INT_CLR(uart_no), UART_FRM_ERR_INT_CLR);
	}else if(UART_RXFIFO_FULL_INT_ST == (READ_PERI_REG(UART_INT_ST(uart_no)) & UART_RXFIFO_FULL_INT_ST)){
        fifo_len = RF_FIFO_LEN(UART0);
        buf_idx = 0;
		//os_printf("UART_RXFIFO_FULL_INT_ST %d!!!\n", fifo_len);
        while(buf_idx < fifo_len)
        {
        	RcvChar = READ_FIFO;
            //uart_tx_one_char(UART0, RcvChar);
        	g_CurrentPoint[g_total] = RcvChar;
			buf_idx++;
			g_total++;
			if(g_total >= 2800)
			{	
				//system_soft_wdt_feed();
				if(uartOKflag)
				{						
					uartOKflag = 0;
					sint8 val = 0;
					val = espconn_send(&stcp_Con,g_CurrentPoint,g_total);
					if(val)
					{
						RTS_FLAG(1);
						os_printf("espconn_send1 ----> %d!!!\n", (int)val);
					}
					else
					{
						g_ultotal += g_total;
						g_CurrentPoint = ((g_CurrentPoint == g_UartPoint1) ? g_UartPoint2 : g_UartPoint1);	
						g_total = 0;
						RTS_FLAG(0);
					}
				}
				else
				{
					RTS_FLAG(1);
					os_printf("espconn_no_send!!!\n");
				}
			}
        }



		//uart_rx_intr_disable(UART0);
		WRITE_PERI_REG(UART_INT_CLR(UART0), UART_RXFIFO_FULL_INT_CLR);
	}else if(UART_RXFIFO_TOUT_INT_ST == (READ_PERI_REG(UART_INT_ST(uart_no)) & UART_RXFIFO_TOUT_INT_ST)){

        fifo_len = RF_FIFO_LEN(UART0);
        buf_idx = 0;
		os_printf("UART_RXFIFO_TOUT_INT_ST %d!!!\n", fifo_len);
        while(buf_idx < fifo_len)
		{
			RcvChar = READ_FIFO;

			uartstr[buf_idx] = RcvChar;

			if(uartflag)
			{
				g_CurrentPoint[g_total] = RcvChar;
				g_total++;
			}

			buf_idx++;
		}
		uint8_t tmp_flag = 0;
		if(uartstr[0] == 0x55 && uartstr[1] == 0xAA)
		{
			if(uartstr[3] == 6)//����mqtt����
			{
				os_memset(uartstr,0,2);
				mqtt_rec_length = (uartstr[4] << 16) + (uartstr[5] << 8) + uartstr[6];
				MQTT_Publish(&mqttClient, "Test1", &uartstr[7], mqtt_rec_length, 0, 0);
				tmp_flag = 1;
			}
			if(uartstr[3] == 2 || uartstr[3] == 0)
			{
				uartflag = 0 ;
			}
		}
		if(!tmp_flag)
		{
			if(uartflag&0x01)
			{
				g_ultotal += g_total;
				if(uartOKflag == 1)
				{
					uartOKflag = 0;
					sint8 val = 0;
					val = espconn_send(&stcp_Con,g_CurrentPoint,g_total);
					if(val)
					{
						RTS_FLAG(1);
						os_printf("espconn_send2 ----> %d!!!\n", (int)val);
						uartflag = 0X81;
					}
					else
					{
						g_total = 0;
						g_CurrentPoint = ((g_CurrentPoint == g_UartPoint1) ? g_UartPoint2 : g_UartPoint1);
						RTS_FLAG(0);
					}
				}
				else
				{

					if(g_total != 0)
					{
					RTS_FLAG(1);
					uartflag = 0X81;
					os_printf("espconn_no_send_again!!!\n");
					}

				}
			}
		}

        if(uartstr[0] == 0x55 && uartstr[1] == 0xAA)
		{
			os_memset(uartstr,0,2);
			switch(uartstr[3])
			{
			case 0:
				g_ultotal = 0;
				uartOKflag = 1;
				uartflag = 1 ;
				g_total = 0;
				RTS_FLAG(0);
				if( g_UartPoint1 != NULL || g_UartPoint2 != NULL)
				{
					os_free(g_UartPoint1);
					os_free(g_UartPoint2);
				}
				//Create_Tcp_Connect_Client(&stcp_Con, "192.168.43.15", 8888);
				//Create_Tcp_Connect_Client(&stcp_Con, "114.222.59.85", 12345);
				//MQTT_Disconnect(&mqttClient);
				Create_Tcp_Connect_Client(&stcp_Con,g_DnsInfo[0].ip_name , g_DnsInfo[0].port);
				os_printf("ip_name:%s,port:%d", g_DnsInfo[0].ip_name, g_DnsInfo[0].port);
				g_UartPoint1 = (char *)os_malloc(sizeof(char) * (2800 + 100));
				g_UartPoint2 = (char *)os_malloc(sizeof(char) * (2800 + 100));
				g_CurrentPoint = g_UartPoint1;
				if(g_UartPoint1 == NULL || g_UartPoint2 == NULL)
				{
					os_printf("��̬����ռ�ʧ��\n");
				}
				break;
			case 1:break;
			case 2:
				RTS_FLAG(0);
				uartflag = 0 ;
				espconn_disconnect(&stcp_Con);
				os_free(g_UartPoint1);
				os_free(g_UartPoint2);
				g_UartPoint1 = NULL;
				g_UartPoint2 = NULL;
				//MQTT_Connect(&mqttClient);
				break;
			default:break;
			}
		}


		os_printf("\nESP8266_USART\n");
		WRITE_PERI_REG(UART_INT_CLR(UART0), UART_RXFIFO_TOUT_INT_CLR);
	}else if(UART_TXFIFO_EMPTY_INT_ST == (READ_PERI_REG(UART_INT_ST(uart_no)) & UART_TXFIFO_EMPTY_INT_ST)){
		os_printf("EMPTY_INT_ST\r\n");
	/* to output uart data from uart buffer directly in empty interrupt handler*/
	/*instead of processing in system event, in order not to wait for current task/function to quit */
	/*ATTENTION:*/
	/*IN NON-OS VERSION SDK, DO NOT USE "ICACHE_FLASH_ATTR" FUNCTIONS IN THE WHOLE HANDLER PROCESS*/
	/*ALL THE FUNCTIONS CALLED IN INTERRUPT HANDLER MUST BE DECLARED IN RAM */
	CLEAR_PERI_REG_MASK(UART_INT_ENA(UART0), UART_TXFIFO_EMPTY_INT_ENA);
	#if UART_BUFF_EN
		tx_start_uart_buffer(UART0);
	#endif
		//system_os_post(uart_recvTaskPrio, 1, 0);
		WRITE_PERI_REG(UART_INT_CLR(uart_no), UART_TXFIFO_EMPTY_INT_CLR);

	}else if(UART_RXFIFO_OVF_INT_ST  == (READ_PERI_REG(UART_INT_ST(uart_no)) & UART_RXFIFO_OVF_INT_ST)){
		WRITE_PERI_REG(UART_INT_CLR(uart_no), UART_RXFIFO_OVF_INT_CLR);
		os_printf("RX OVF!!\r\n");
	}

}



// uint16_t g_total = 0;
// char * g_CurrentPoint = NULL;
// char * g_UartPoint1 = NULL;
// char * g_UartPoint2 = NULL;
// char uartstr[110];
// uint8 uartflag = 0;
// uint8 uartOKflag = 1;
// uint32_t mqtt_rec_length ;
// void uart0_rx_intr_handler(void *para)
// {

// 	uint8 RcvChar;
// 	uint8 uart_no = UART0;//UartDev.buff_uart_no;
// 	uint8 fifo_len = 0;
// 	uint8 buf_idx = 0;
// 	uint8 temp,cnt;
// 	char * pTmp = NULL;
// 	//RcvMsgBuff *pRxBuff = (RcvMsgBuff *)para;

// 		/*ATTENTION:*/
// 	/*IN NON-OS VERSION SDK, DO NOT USE "ICACHE_FLASH_ATTR" FUNCTIONS IN THE WHOLE HANDLER PROCESS*/
// 	/*ALL THE FUNCTIONS CALLED IN INTERRUPT HANDLER MUST BE DECLARED IN RAM */
// 	/*IF NOT , POST AN EVENT AND PROCESS IN SYSTEM TASK */
// 	if(UART_FRM_ERR_INT_ST == (READ_PERI_REG(UART_INT_ST(uart_no)) & UART_FRM_ERR_INT_ST)){
// 		os_printf("FRM_ERR\r\n");
// 		WRITE_PERI_REG(UART_INT_CLR(uart_no), UART_FRM_ERR_INT_CLR);
// 	}else if(UART_RXFIFO_FULL_INT_ST == (READ_PERI_REG(UART_INT_ST(uart_no)) & UART_RXFIFO_FULL_INT_ST)){
//         fifo_len = RF_FIFO_LEN(UART0);
//         buf_idx = 0;
//         while(buf_idx < fifo_len)
//         {
//         	RcvChar = READ_FIFO;
//             //uart_tx_one_char(UART0, RcvChar);
// 			buf_idx++;
// 			g_total++;
// 			if(g_total >= 200)
// 			{

// 			}
// 					CLEAR_PERI_REG_MASK(UART_INT_ENA(UART0), UART_RXFIFO_FULL_INT_ENA);
// 					//CLEAR_PERI_REG_MASK(UART_INT_ENA(UART0), UART_RXFIFO_TOUT_INT_ENA);
// 					os_printf("espconn_no_send!!!\n");
// 					RTS_FLAG(1);
// 					break;

//         }



// 		//uart_rx_intr_disable(UART0);
// 		WRITE_PERI_REG(UART_INT_CLR(UART0), UART_RXFIFO_FULL_INT_CLR);
// 	}else if(UART_RXFIFO_TOUT_INT_ST == (READ_PERI_REG(UART_INT_ST(uart_no)) & UART_RXFIFO_TOUT_INT_ST)){

//         fifo_len = RF_FIFO_LEN(UART0);
//         buf_idx = 0;
//         while(buf_idx < fifo_len)
// 		{
// 			RcvChar = READ_FIFO;
// 			uartstr[buf_idx] = RcvChar;	
// 			buf_idx++;
// 		}
// 		os_printf("\nESP8266_USART\n");
// 		WRITE_PERI_REG(UART_INT_CLR(UART0), UART_RXFIFO_TOUT_INT_CLR);
// 	}else if(UART_TXFIFO_EMPTY_INT_ST == (READ_PERI_REG(UART_INT_ST(uart_no)) & UART_TXFIFO_EMPTY_INT_ST)){
// 		os_printf("EMPTY_INT_ST\r\n");
// 	/* to output uart data from uart buffer directly in empty interrupt handler*/
// 	/*instead of processing in system event, in order not to wait for current task/function to quit */
// 	/*ATTENTION:*/
// 	/*IN NON-OS VERSION SDK, DO NOT USE "ICACHE_FLASH_ATTR" FUNCTIONS IN THE WHOLE HANDLER PROCESS*/
// 	/*ALL THE FUNCTIONS CALLED IN INTERRUPT HANDLER MUST BE DECLARED IN RAM */
// 	CLEAR_PERI_REG_MASK(UART_INT_ENA(UART0), UART_TXFIFO_EMPTY_INT_ENA);
// 	#if UART_BUFF_EN
// 		tx_start_uart_buffer(UART0);
// 	#endif
// 		//system_os_post(uart_recvTaskPrio, 1, 0);
// 		WRITE_PERI_REG(UART_INT_CLR(uart_no), UART_TXFIFO_EMPTY_INT_CLR);

// 	}else if(UART_RXFIFO_OVF_INT_ST  == (READ_PERI_REG(UART_INT_ST(uart_no)) & UART_RXFIFO_OVF_INT_ST)){
// 		WRITE_PERI_REG(UART_INT_CLR(uart_no), UART_RXFIFO_OVF_INT_CLR);
// 		os_printf("RX OVF!!\r\n");
// 	}

// }















#if ((SPI_FLASH_SIZE_MAP == 0) || (SPI_FLASH_SIZE_MAP == 1))
#error "The flash map is not supported"
#elif (SPI_FLASH_SIZE_MAP == 2)
#define SYSTEM_PARTITION_OTA_SIZE							0x6A000
#define SYSTEM_PARTITION_OTA_2_ADDR							0x81000
#define SYSTEM_PARTITION_RF_CAL_ADDR						0xfb000
#define SYSTEM_PARTITION_PHY_DATA_ADDR						0xfc000
#define SYSTEM_PARTITION_SYSTEM_PARAMETER_ADDR				0xfd000
#define SYSTEM_PARTITION_CUSTOMER_PRIV_PARAM_ADDR           0x7c000
#elif (SPI_FLASH_SIZE_MAP == 3)
#define SYSTEM_PARTITION_OTA_SIZE							0x6A000
#define SYSTEM_PARTITION_OTA_2_ADDR							0x81000
#define SYSTEM_PARTITION_RF_CAL_ADDR						0x1fb000
#define SYSTEM_PARTITION_PHY_DATA_ADDR						0x1fc000
#define SYSTEM_PARTITION_SYSTEM_PARAMETER_ADDR				0x1fd000
#define SYSTEM_PARTITION_CUSTOMER_PRIV_PARAM_ADDR           0x7c000
#elif (SPI_FLASH_SIZE_MAP == 4)
#define SYSTEM_PARTITION_OTA_SIZE							0x6A000
#define SYSTEM_PARTITION_OTA_2_ADDR							0x81000
#define SYSTEM_PARTITION_RF_CAL_ADDR						0x3fb000
#define SYSTEM_PARTITION_PHY_DATA_ADDR						0x3fc000
#define SYSTEM_PARTITION_SYSTEM_PARAMETER_ADDR				0x3fd000
#define SYSTEM_PARTITION_CUSTOMER_PRIV_PARAM_ADDR           0x7c000
#elif (SPI_FLASH_SIZE_MAP == 5)
#define SYSTEM_PARTITION_OTA_SIZE							0x6A000
#define SYSTEM_PARTITION_OTA_2_ADDR							0x101000
#define SYSTEM_PARTITION_RF_CAL_ADDR						0x1fb000
#define SYSTEM_PARTITION_PHY_DATA_ADDR						0x1fc000
#define SYSTEM_PARTITION_SYSTEM_PARAMETER_ADDR				0x1fd000
#define SYSTEM_PARTITION_CUSTOMER_PRIV_PARAM_ADDR           0xfc000
#elif (SPI_FLASH_SIZE_MAP == 6)
#define SYSTEM_PARTITION_OTA_SIZE							0x6A000
#define SYSTEM_PARTITION_OTA_2_ADDR							0x101000
#define SYSTEM_PARTITION_RF_CAL_ADDR						0x3fb000
#define SYSTEM_PARTITION_PHY_DATA_ADDR						0x3fc000
#define SYSTEM_PARTITION_SYSTEM_PARAMETER_ADDR				0x3fd000
#define SYSTEM_PARTITION_CUSTOMER_PRIV_PARAM_ADDR           0xfc000
#else
#error "The flash map is not supported"
#endif

#define SYSTEM_PARTITION_CUSTOMER_PRIV_PARAM                SYSTEM_PARTITION_CUSTOMER_BEGIN

uint32 priv_param_start_sec;

static const partition_item_t at_partition_table[] = {
    { SYSTEM_PARTITION_BOOTLOADER, 						0x0, 												0x1000},
    { SYSTEM_PARTITION_OTA_1,   						0x1000, 											SYSTEM_PARTITION_OTA_SIZE},
    { SYSTEM_PARTITION_OTA_2,   						SYSTEM_PARTITION_OTA_2_ADDR, 						SYSTEM_PARTITION_OTA_SIZE},
    { SYSTEM_PARTITION_RF_CAL,  						SYSTEM_PARTITION_RF_CAL_ADDR, 						0x1000},
    { SYSTEM_PARTITION_PHY_DATA, 						SYSTEM_PARTITION_PHY_DATA_ADDR, 					0x1000},
    { SYSTEM_PARTITION_SYSTEM_PARAMETER, 				SYSTEM_PARTITION_SYSTEM_PARAMETER_ADDR, 			0x3000},
    { SYSTEM_PARTITION_CUSTOMER_PRIV_PARAM,             SYSTEM_PARTITION_CUSTOMER_PRIV_PARAM_ADDR,          0x1000},
};

void ICACHE_FLASH_ATTR user_pre_init(void)
{
    if(!system_partition_table_regist(at_partition_table, sizeof(at_partition_table)/sizeof(at_partition_table[0]),SPI_FLASH_SIZE_MAP)) {
		os_printf("system_partition_table_regist fail\r\n");
		while(1);
	}
}





