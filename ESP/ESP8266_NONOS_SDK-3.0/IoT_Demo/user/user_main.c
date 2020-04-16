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







void ICACHE_FLASH_ATTR user_init(void)
{
    SET_PERI_REG_MASK(UART_INT_ENA(uart_no), UART_RXFIFO_TOUT_INT_ENA | UART_FRM_ERR_INT_ENA);
    
    WRITE_PERI_REG
	UART_INT_ENA UART_RXFIFO_OVF_INT_ENA (BIT(4))
	UART_SetPrintPort(UART1);
    //设置串口初始化
	uart_init(115200,115200);
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
    struct sntp_No_str tSntp[3] = {
        {0,"cn.pool.ntp.org"},
        {1,"asia.pool.ntp.org"},
        {2,"cn.ntp.org.cn"}}; 
    my_sntp_init(tSntp);
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
			dns_parse(&stcp_Con, "vop.baidu.com");
			bsp_SoftTimer_close(&udp_start_os_timer);

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
        bsp_SoftTimer_Restart(&udp_start_os_timer, 5000, (void *)2, 1);
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

void uart0_rx_intr_handler(void *para)
{

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





