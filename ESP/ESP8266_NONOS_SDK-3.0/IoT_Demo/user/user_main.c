#include "driver/inc.h"






//==============================

// 类型定义
//=================================
typedef unsigned long 		u32_t;

//=================================


// 全局变量
//============================================================================
MQTT_Client mqttClient;			// MQTT客户端_结构体【此变量非常重要】
uint32_t g_ultotal = 0;

// MQTT已成功连接：ESP8266发送【CONNECT】，并接收到【CONNACK】
//============================================================================
void mqttConnectedCb(uint32_t *args)
{
    MQTT_Client* client = (MQTT_Client*)args;	// 获取mqttClient指针

    INFO("MQTT: Connected\r\n");

    // 【参数2：主题过滤器 / 参数3：订阅Qos】
    //-----------------------------------------------------------------
	MQTT_Subscribe(client, "SW_LED", 0);	// 订阅主题"SW_LED"，QoS=0
//	MQTT_Subscribe(client, "SW_LED", 1);
//	MQTT_Subscribe(client, "SW_LED", 2);

	// 【参数2：主题名 / 参数3：发布消息的有效载荷 / 参数4：有效载荷长度 / 参数5：发布Qos / 参数6：Retain】
	//-----------------------------------------------------------------------------------------------------------------------------------------
//	MQTT_Publish(client, "SW_LED", "ESP8266_Online", strlen("ESP8266_Online"), 0, 0);	// 向主题"SW_LED"发布"ESP8266_Online"，Qos=0、retain=0
//	MQTT_Publish(client, "SW_LED", "ESP8266_Online", strlen("ESP8266_Online"), 1, 0);
//	MQTT_Publish(client, "SW_LED", "ESP8266_Online", strlen("ESP8266_Online"), 2, 0);
	//MQTT_Subscribe(client, "Will", 0);

//	MQTT_Publish(client, "SW_LED", "ESP8266_Online", strlen("ESP8266_Online"), 0, 0);
}
//============================================================================

// MQTT成功断开连接
//============================================================================
void mqttDisconnectedCb(uint32_t *args)
{
    MQTT_Client* client = (MQTT_Client*)args;
    INFO("MQTT: Disconnected\r\n");
}
//============================================================================

// MQTT成功发布消息
//============================================================================
void mqttPublishedCb(uint32_t *args)
{
    MQTT_Client* client = (MQTT_Client*)args;
    INFO("MQTT: Published\r\n");
}
//============================================================================

// 【接收MQTT的[PUBLISH]数据】函数		【参数1：主题 / 参数2：主题长度 / 参数3：有效载荷 / 参数4：有效载荷长度】
//===============================================================================================================
void mqttDataCb(uint32_t *args, const char* topic, uint32_t topic_len, const char *data, uint32_t data_len)
{
    char *topicBuf = (char*)os_zalloc(topic_len+1);		// 申请【主题】空间
    char *dataBuf  = (char*)os_zalloc(data_len+1);		// 申请【有效载荷】空间


    MQTT_Client* client = (MQTT_Client*)args;	// 获取MQTT_Client指针


    os_memcpy(topicBuf, topic, topic_len);	// 缓存主题
    topicBuf[topic_len] = 0;				// 最后添'\0'

    os_memcpy(dataBuf, data, data_len);		// 缓存有效载荷
    dataBuf[data_len] = 0;					// 最后添'\0'

    INFO("Receive topic: %s, data: %s \r\n", topicBuf, dataBuf);	// 串口打印【主题】【有效载荷】
    INFO("Topic_len = %d, Data_len = %d\r\n", topic_len, data_len);	// 串口打印【主题长度】【有效载荷长度】


// 【技小新】添加
//########################################################################################
    // 根据接收到的主题名/有效载荷，控制LED的亮/灭
    //-----------------------------------------------------------------------------------
    if( os_strcmp(topicBuf,"SW_LED") == 0 && data_len == 4)			// 主题 == "SW_LED"
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


    os_free(topicBuf);	// 释放【主题】空间
    os_free(dataBuf);	// 释放【有效载荷】空间
}
//软件定时器结构体
os_timer_t led_os_timer;
//os_timer_t udp_start_os_timer;
os_timer_t wifi_statue_timer;		/*上电上电联网后自动发送一次联网状态,当上电20s后联网失败自动发送联网失败状态之后每隔10s发送状态直到联网成功停止数据发送*/
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

uint8_t * wifi_statue_str = NULL;//用于存放联网状态数据包

void ICACHE_FLASH_ATTR user_init(void)
{
	// 管脚功能选择(注意：参数1【PIN_NAME】管脚名、参数2【FUNC】管脚功能)
	//--------------------------------------------------------------------------
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_SD_DATA3_U,	FUNC_GPIO10);	// GPIO4设为IO口

	// 将相应管脚设为输出模式，并输出对应电平(参数1【gpio_no】、参数2：输出电平)
	//--------------------------------------------------------------------------
	GPIO_OUTPUT_SET(GPIO_ID_PIN(10),0);			// IO4 = 1(高电平)

    UART_SetPrintPort(UART1);
        //设置串口初始化
	uart_init(BIT_RATE_460800,BIT_RATE_460800);
    os_printf("READ_PERI_REG1:%d\n", READ_PERI_REG(UART_CONF1(UART0)));
    /*设置full阈值为100*/
    WRITE_PERI_REG(UART_CONF1(UART0),((100 & UART_RXFIFO_FULL_THRHD) << UART_RXFIFO_FULL_THRHD_S) |
    (READ_PERI_REG(UART_CONF1(UART0)) & (~UART_RXFIFO_FULL_THRHD)));
    os_printf("READ_PERI_REG2:%d\n", READ_PERI_REG(UART_CONF1(UART0)));
    /*设置timeout阈值为100,在bps = 460800下 为1.736ms后发送中断*/
    WRITE_PERI_REG(UART_CONF1(UART0),((125 & UART_RX_TOUT_THRHD) << UART_RX_TOUT_THRHD_S) |
    (READ_PERI_REG(UART_CONF1(UART0)) & (~(UART_RX_TOUT_THRHD<< UART_RX_TOUT_THRHD_S))));
    os_printf("READ_PERI_REG3:%d\n", READ_PERI_REG(UART_CONF1(UART0)));
    /*设置接收超时中断,接收帧错误中断,和full中断*/
    SET_PERI_REG_MASK(UART_INT_ENA(UART0), UART_RXFIFO_TOUT_INT_ENA | UART_FRM_ERR_INT_ENA | UART_RXFIFO_FULL_INT_ENA);
    os_printf("SET_PERI_REG_MASK:%d\n", READ_PERI_REG(UART_INT_ENA(UART0)));

    //打印sdk版本信息
    os_printf("SDK version:%s\n", system_get_sdk_version());
    //led初始化
    bsp_led_init();
    
    softap_station_scan(STATION_MODE, 1);

    /*AP模式下设置
    //设置AP ,SAT, AP + SAT
    softap_station_scan(SOFTAP_MODE, 1)
    //获取Ap模式下的参数信息
    My_Get_softap_Status(1);
    //设置AP模式下的参数
    My_Set_softap_Status("ILOVEYOU", "123456789", 1);
    */
    
    //设置软件定时
	wifi_statue_str = (uint8_t *)os_malloc( sizeof(uint8_t) * 10);/*用于存放检测联网状态的数据包*/
	
	UartDataPacking(wifi_statue_str, 0x05, 1, 0);
	
	wifi_statue_str[6] = 1;

    Led_SoftTimer_init(&led_os_timer,Led_Soft_Timer_Handle,500);

    //bsp_SoftTimer_init(&udp_start_os_timer,udp_Soft_Timer_Handle,2000,1,1);

	bsp_SoftTimer_init(&wifi_statue_timer, wifi_statue_timer_Handle, 20000, (uint32_t)wifi_statue_str, 0);


	CFG_Load();	// 加载/更新系统参数【WIFI参数、MQTT参数】


	// 网络连接参数赋值：服务端域名【mqtt_test_jx.mqtt.iot.gz.baidubce.com】、网络连接端口【1883】、安全类型【0：NO_TLS】
	//-------------------------------------------------------------------------------------------------------------------
	MQTT_InitConnection(&mqttClient, sysCfg.mqtt_host, sysCfg.mqtt_port, sysCfg.security);

	// MQTT连接参数赋值：客户端标识符【..】、MQTT用户名【..】、MQTT密钥【..】、保持连接时长【120s】、清除会话【1：clean_session】
	//----------------------------------------------------------------------------------------------------------------------------
	MQTT_InitClient(&mqttClient, sysCfg.device_id, sysCfg.mqtt_user, sysCfg.mqtt_pass, sysCfg.mqtt_keepalive, 0);

	// 设置遗嘱参数(如果云端没有对应的遗嘱主题，则MQTT连接会被拒绝)
	//--------------------------------------------------------------
	//MQTT_InitLWT(&mqttClient, "Will", "ESP8266_Offline", 0, 0);


	// 设置MQTT相关函数
	//--------------------------------------------------------------------------------------------------
	MQTT_OnConnected(&mqttClient, mqttConnectedCb);			// 设置【MQTT成功连接】函数的另一种调用方式
	MQTT_OnDisconnected(&mqttClient, mqttDisconnectedCb);	// 设置【MQTT成功断开】函数的另一种调用方式
	MQTT_OnPublished(&mqttClient, mqttPublishedCb);			// 设置【MQTT成功发布】函数的另一种调用方式
	MQTT_OnData(&mqttClient, mqttDataCb);					// 设置【接收MQTT数据】函数的另一种调用方式


	// 连接WIFI：SSID[..]、PASSWORD[..]、WIFI连接成功函数[wifiConnectCb]
	//--------------------------------------------------------------------------
	WIFI_Connect(sysCfg.sta_ssid, sysCfg.sta_pwd, wifiConnectCb);


	INFO("\r\nSystem started ...\r\n");

}



void wifi_statue_timer_Handle (void * reg)

{
	uint8_t * ulpStr = (uint8_t *)reg;

	if(wifi_statue_flag == 1)
	{
		os_printf("ESP联网成功\n");
		os_free(wifi_statue_str);
		bsp_SoftTimer_close(&wifi_statue_timer);
	}
	else
	{
		os_printf("ESP联网失败\n");
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

    // IP地址获取失败
	//----------------------------------------------------------------
    else
    {
          MQTT_Disconnect(&mqttClient);	// WIFI连接出错，TCP断开连接
    }
}

// void scan_All_Ap_Info_done(void *arg, STATUS status)
// {
//     struct bss_info* pBss_info = (struct bss_info*)arg;
//     switch (status)
//     {
//     case OK:os_printf("扫描所有AP配置参数--成功\n");break;
//     case FAIL:os_printf("扫描所有AP配置参数--失败\n");break;
//     case PENDING:os_printf("扫描所有AP配置参数--待处理\n");break;
//     case BUSY:os_printf("扫描所有AP配置参数--繁忙\n");break;
//     case CANCEL:os_printf("扫描所有AP配置参数--取消\n");break;
//     default:break;
//     }
//     if(status == OK)
//     {
//         while (pBss_info)
//         {
//             os_printf("AP名:%s\t\t\t", pBss_info->ssid);
//             os_printf("通道:%d\t  ", pBss_info->channel);
//             os_printf("隐藏:%d\t  ", pBss_info->is_hidden);
//             os_printf("加密方式:%d\n", pBss_info->authmode);
//             pBss_info = pBss_info->next.stqe_next;
//         }

//         My_Set_station_Status("2601b-2.4G", "19728888", 1);
//         bsp_SoftTimer_Restart(&udp_start_os_timer, 100, (void *)2, 1);
//     }
// }




// SNTP定时函数：获取当前网络时间
void ICACHE_FLASH_ATTR user_check_sntp_stamp(void *arg)
{
    uint32 current_stamp = 1;

    current_stamp = sntp_get_current_timestamp();

	os_printf("current time : %s\n", sntp_get_real_time(current_stamp));	// 获取真实时间

    if(current_stamp == 0)
    {
		os_printf("did not get a valid time from sntp server\n");
    } 
    else
    {
        os_timer_disarm(&sntp_timer);	// 关闭SNTP定时器

        MQTT_Connect(&mqttClient);		// 开始MQTT连接

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
			if(uartstr[3] == 6)//接收mqtt数据
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
					os_printf("动态分配空间失败\n");
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





