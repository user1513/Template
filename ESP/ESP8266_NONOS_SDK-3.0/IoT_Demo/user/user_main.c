#include "driver/inc.h"


//软件定时器结构体
os_timer_t led_os_timer;
os_timer_t udp_start_os_timer;
os_timer_t sntp_timer;
//
struct espconn stcp_Con;

void scan_All_Ap_Info_done(void *arg, STATUS status);
void Led_Soft_Timer_Handle(void * reg);
void  udp_Soft_Timer_Handle(void * reg);
void ICACHE_FLASH_ATTR user_check_sntp_stamp(void *arg);


GetDnsInfo g_DnsInfo[2] ={
{"vop.baidu.com","0",80},
{"www.baidu.com","0",80}};

struct sntp_No_str tSntp[3] = {
	{0,"cn.pool.ntp.org"},
	{1,"asia.pool.ntp.org"},
	{2,"cn.ntp.org.cn"}};



void ICACHE_FLASH_ATTR user_init(void)
{
    UART_SetPrintPort(UART1);
        //设置串口初始化
	uart_init(BIT_RATE_460800,BIT_RATE_460800);
    os_printf("READ_PERI_REG1:%d\n", READ_PERI_REG(UART_CONF1(UART0)));
    /*设置full阈值为100*/
    WRITE_PERI_REG(UART_CONF1(UART0),((100 & UART_RXFIFO_FULL_THRHD) << UART_RXFIFO_FULL_THRHD_S) |
    (READ_PERI_REG(UART_CONF1(UART0)) & (~UART_RXFIFO_FULL_THRHD)));
    os_printf("READ_PERI_REG2:%d\n", READ_PERI_REG(UART_CONF1(UART0)));
    /*设置timeout阈值为100,在bps = 460800下 为1.736ms后发送中断*/
    WRITE_PERI_REG(UART_CONF1(UART0),((100 & UART_RX_TOUT_THRHD) << UART_RX_TOUT_THRHD_S) |
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
    Led_SoftTimer_init(&led_os_timer,Led_Soft_Timer_Handle,500);
    bsp_SoftTimer_init(&udp_start_os_timer,udp_Soft_Timer_Handle,2000,1,1);
}





void Led_Soft_Timer_Handle(void * reg)
{
    bsp_led_flash();
}

void  udp_Soft_Timer_Handle(void * reg)
{
    if((int)reg == 1)
    {
        Get_All_Ap_Info(scan_All_Ap_Info_done);
        bsp_SoftTimer_close(&udp_start_os_timer);
    }
    else
    {
    
		if( Wifi_Sta_status() == STATION_GOT_IP)
		{
			Get_localhost_info(STATION_IF);
			//获取AP模式下终端的信息
			//Get_station_info_from_AP();
			//获取AP模式下参数
			//My_Get_softap_Status(0);
			//创建udp连接
			//Create_Udp_Connect_Server(1314);
			//Create_Udp_Connect_Client("192.168.4.2",8888);
			//Send_Wifi_Data("hello i am yuan\n");
			//Create_Tcp_Connect_Server(1314);

			//读取station模式下的状态
			My_Get_station_Status(1);
			multiple_dns_parse(&g_DnsInfo[sizeof(g_DnsInfo)/sizeof(g_DnsInfo[0]) - 1], &stcp_Con, sizeof(g_DnsInfo)/sizeof(g_DnsInfo[0]));
			bsp_SoftTimer_close(&udp_start_os_timer);
			my_sntp_init(tSntp);
            os_timer_disarm(&sntp_timer);
            os_timer_setfn(&sntp_timer, (os_timer_func_t *)user_check_sntp_stamp, NULL);
            os_timer_arm(&sntp_timer, 100, 0);
		}
    }
}

void scan_All_Ap_Info_done(void *arg, STATUS status)
{
    struct bss_info* pBss_info = (struct bss_info*)arg;
    switch (status)
    {
    case OK:os_printf("扫描所有AP配置参数--成功\n");break;
    case FAIL:os_printf("扫描所有AP配置参数--失败\n");break;
    case PENDING:os_printf("扫描所有AP配置参数--待处理\n");break;
    case BUSY:os_printf("扫描所有AP配置参数--繁忙\n");break;
    case CANCEL:os_printf("扫描所有AP配置参数--取消\n");break;
    default:break;
    }
    if(status == OK)
    {
        while (pBss_info)
        {
            os_printf("AP名:%s\t\t\t", pBss_info->ssid);
            os_printf("通道:%d\t  ", pBss_info->channel);
            os_printf("隐藏:%d\t  ", pBss_info->is_hidden);
            os_printf("加密方式:%d\n", pBss_info->authmode);
            pBss_info = pBss_info->next.stqe_next;
        }

        My_Set_station_Status("WIWH", "19728888", 1);
        bsp_SoftTimer_Restart(&udp_start_os_timer, 100, (void *)2, 1);
    }
}





void ICACHE_FLASH_ATTR user_check_sntp_stamp(void *arg)
{
    uint32 current_stamp;
    current_stamp = sntp_get_current_timestamp();
    if(current_stamp == 0)
    {
        os_timer_arm(&sntp_timer, 100, 0);
    } 
    else
    {
        os_timer_disarm(&sntp_timer);
        os_printf("sntp: %d, %s \n",current_stamp,sntp_get_real_time(current_stamp));
        os_timer_arm(&sntp_timer, 500, 1);
    }
}


/******************************************************************************
 * FunctionName : uart0_rx_intr_handler
 * Description  : Internal used function
 *                UART0 interrupt handler, add self handle code inside
 * Parameters   : void *para - point to ETS_UART_INTR_ATTACH's arg
 * Returns      : NONE
*/
void uart0_rx_intr_handler(void *para)
{
	uint8 RcvChar;
	uint8 uart_no = UART0;//UartDev.buff_uart_no;
	uint8 fifo_len = 0;
	uint8 buf_idx = 0;
	uint8 temp,cnt;
	//RcvMsgBuff *pRxBuff = (RcvMsgBuff *)para;

		/*ATTENTION:*/
	/*IN NON-OS VERSION SDK, DO NOT USE "ICACHE_FLASH_ATTR" FUNCTIONS IN THE WHOLE HANDLER PROCESS*/
	/*ALL THE FUNCTIONS CALLED IN INTERRUPT HANDLER MUST BE DECLARED IN RAM */
	/*IF NOT , POST AN EVENT AND PROCESS IN SYSTEM TASK */
	if(UART_FRM_ERR_INT_ST == (READ_PERI_REG(UART_INT_ST(uart_no)) & UART_FRM_ERR_INT_ST)){
		os_printf("FRM_ERR\r\n");
		WRITE_PERI_REG(UART_INT_CLR(uart_no), UART_FRM_ERR_INT_CLR);
	}else if(UART_RXFIFO_FULL_INT_ST == (READ_PERI_REG(UART_INT_ST(uart_no)) & UART_RXFIFO_FULL_INT_ST)){
		os_printf("FULL_INT_ST\r\n");
        fifo_len = RF_FIFO_LEN(UART0);
        buf_idx = 0;
        while(buf_idx < fifo_len)
        {
            uart_tx_one_char(UART0, READ_FIFO);
            buf_idx++;
        }
		//uart_rx_intr_disable(UART0);
		WRITE_PERI_REG(UART_INT_CLR(UART0), UART_RXFIFO_FULL_INT_CLR);
	}else if(UART_RXFIFO_TOUT_INT_ST == (READ_PERI_REG(UART_INT_ST(uart_no)) & UART_RXFIFO_TOUT_INT_ST)){
		os_printf("TOUT_INT_ST\r\n");
        os_printf("RF_FIFO_LEN:%d",(int)RF_FIFO_LEN(uart_no));
        fifo_len = RF_FIFO_LEN(UART0);
        buf_idx = 0;
        while(buf_idx < fifo_len)
        {
            uart_tx_one_char(UART0, READ_FIFO);
            buf_idx++;
        }
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





