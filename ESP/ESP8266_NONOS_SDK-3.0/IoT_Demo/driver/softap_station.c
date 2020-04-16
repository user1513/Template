#include "driver/softap_station.h"


/*
_set_wifi_mode 选项
#define NULL_MODE       0x00
#define STATION_MODE    0x01
#define SOFTAP_MODE     0x02
#define STATIONAP_MODE  0x03

_select
0--->查询 Wi-Fi 当前?工作模式,设置 Wi-Fi 工作模式（Station，SoftAP 或者 Station + SoftAP），不?保存到 Flash。
1--->查询保存在 Flash 中的 Wi-Fi ?工作模式设置,设置 Wi-Fi ?工作模式（Station，SoftAP 或者 Station+SoftAP），并保存到 Flash。


*/
bool softap_station_scan(uint8_t _set_wifi_mode, uint8_t _select)
{
    bool _bool = false;
    uint8_t wifi_mode = 0;
    if(_select)
    {
        //查询保存在 Flash 中的 Wi-Fi ?工作模式设置
        wifi_mode = wifi_get_opmode_default();
        //本设置如果与原设置不同，会更新保存到 Flash 系统参数区。
        _bool = wifi_set_opmode(_set_wifi_mode);
    }
    else
    {
        //查询 Wi-Fi 当前?工作模式
        wifi_mode = wifi_get_opmode();
        //设置 Wi-Fi ?工作模式（Station，SoftAP 或者 Station + SoftAP），不?保存到 Flash。
        _bool = wifi_set_opmode_current(_set_wifi_mode);
    }
    switch(wifi_mode)
    {
        case 1: os_printf("当前wifi工作模式>>Station\n");break;
        case 2: os_printf("当前wifi工作模式>>SoftAP\n");break;
        case 3: os_printf("当前wifi工作模式>>Station+SoftAP\n");break;
        default : os_printf("当前wifi工作模式>>ERR\n");break;
    }
    return _bool;
}


//读取station模式下的状态
bool My_Get_station_Status(uint8_t _select)
{
    bool _bool = false;
    struct station_config _Station_config;
    if(_select)
        _bool = wifi_station_get_config_default(&_Station_config);
    else
        _bool = wifi_station_get_config(&_Station_config);
    
    if(_bool == false) return _bool;
    os_printf("AP名:%s\n",_Station_config.ssid); 
    os_printf("密码:%s\n",_Station_config.password);
    return _bool;
}

#define FuncInUser_initUse  0 
#define Static_Ip_Enable    0   //0不使能静态ip,1使能静态ip
//设置station模式下的状态
bool My_Set_station_Status(uint8_t* ssid, uint8_t* password,uint8_t _select)
{
    bool _bool = false;
    
    struct station_config _Station_config;
    strcpy(_Station_config.ssid, ssid);
    strcpy(_Station_config.password, password);
    _bool = wifi_station_set_config (&_Station_config);

    #if Static_Ip_Enable == 1 
    struct ip_info info;
    IP4_ADDR(&info.ip, 192, 168, 2, 200);
    IP4_ADDR(&info.gw, 192, 168, 2, 1);
    IP4_ADDR(&info.netmask, 255, 255, 255, 0);
    wifi_station_dhcpc_stop();
    wifi_set_ip_info(STATION_IF, &info);
    #endif

    #if FuncInUser_initUse == 0
    wifi_station_connect();
    #endif
}
/*
struct softap_config {
    uint8 ssid[32];
    uint8 password[64];
    uint8 ssid_len;    // Note: Recommend to set it according to your ssid
    uint8 channel;    // Note: support 1 ~ 13
    AUTH_MODE authmode;    // Note: Don't support AUTH_WEP in softAP mode.
    uint8 ssid_hidden;    // Note: default 0
    uint8 max_connection;    // Note: default 4, max 4
    uint16 beacon_interval;    // Note: support 100 ~ 60000 ms, default 100
};
*/
//获取AP模式下参数


bool My_Get_softap_Status(uint8_t _select)
{
    struct softap_config Softap_Config;
    bool _bool = false;

    if(_select)
    {
        //查询 ESP8266 Wi-Fi SoftAP 接?口保存在 Flash 中的配置
        _bool = wifi_softap_get_config_default(&Softap_Config);
    }
    else
    {
        //查询 ESP8266 Wi-Fi SoftAP 接?口的当前配置
        _bool = wifi_softap_get_config(&Softap_Config);
    }
    if(_bool == false) return _bool;
    os_printf("ssid_name:%s\n", Softap_Config.ssid);
    os_printf("password:%s\n", Softap_Config.password);
    os_printf("ssid_len:%d\n", Softap_Config.ssid_len);
    os_printf("max_connection:%d\n", Softap_Config.max_connection);
    return _bool;
}




//请勿在 user_init 中调用本接口，本接口必须在系统初始化完成后，并且 ESP8266 Station 接
//口使能的情况下调用。
bool Get_All_Ap_Info(scan_done_cb_t _scan_All_Ap_Info_done)
{
    wifi_station_scan(NULL, (scan_done_cb_t)_scan_All_Ap_Info_done);
}

//设置AP模式下的参数
bool My_Set_softap_Status(uint8_t* ssid, uint8_t* password,uint8_t _select)
{
    bool _bool = false;
    struct softap_config Softap_Config;

    memset(&Softap_Config,0,sizeof(struct softap_config));      //将struct softap_config Softap_Config;全赋值为0
    //this
    os_strcpy(&Softap_Config.ssid, ssid);        //设置wifi名
    os_strcpy(&Softap_Config.password, password);//设置wifi密码
    //this

    //注意ssid_len的长度是可见的长度,不能把'\0'加入
    Softap_Config.ssid_len = os_strlen(ssid) ;                  //设置wifi名长度
    Softap_Config.channel = 1;    // Note: support 1 ~ 13
    Softap_Config.authmode = AUTH_WPA_WPA2_PSK;    // Note: Don't support AUTH_WEP in softAP mode.
    Softap_Config.ssid_hidden = 0;    // Note: default 0 默认不隐藏wifi名
    Softap_Config.max_connection = 4;    // Note: default 4, max 4
    //无线路由器的信标是无线路由器发送的周期信号，无线网卡扫描到该信标后则发现相应的路由器并显示出来，默认是100ms。
    Softap_Config.beacon_interval = 100;    // Note: support 100 ~ 60000 ms, default 100


    if(_select)
    {
        //查询 ESP8266 Wi-Fi SoftAP 接口保存在 Flash 中的配置
        _bool = wifi_softap_set_config(&Softap_Config);
    }
    else
    {
        //查询 ESP8266 Wi-Fi SoftAP 接?口的当前配置
        _bool = wifi_softap_set_config_current(&Softap_Config);
    }
    return _bool;
}


/*
struct station_info {
    STAILQ_ENTRY(station_info)    next;

    uint8 bssid[6];
    struct ip_addr ip;
};
*/
void Get_station_info_from_AP(void)
{
    struct station_info * pStation_info = NULL;
    struct station_info * next_station = NULL;
    uint8_t* Pip = NULL;
    int num = wifi_softap_get_station_num();
    os_printf("station 终端连接个数:%d\n", num);

    pStation_info = wifi_softap_get_station_info();
    while(num--)
    {
        pStation_info->bssid;
        os_printf("MAC:"MACSTR"\n",MAC2STR(pStation_info->bssid)) ;
        Pip = (uint8_t*)&pStation_info->ip.addr;
        os_printf("IP:%d.%d.%d.%d\n",Pip[0],Pip[1],Pip[2],Pip[3]);
        pStation_info = pStation_info->next.stqe_next;
    }
    wifi_softap_free_station_info(); // Free it by calling functions
    /*#define STATION_IF 0x00--------#define SOFTAP_IF 0x01*/
}

// #define STATION_IF 0x00
// #define SOFTAP_IF 0x01
bool Get_localhost_info(uint8_t mode)
{
    struct ip_info ST_ESP8266_IP;	// IP信息结构体
    uint8_t* Pip = NULL;
    
    if(wifi_get_ip_info(mode,&ST_ESP8266_IP))	// 参数2：IP信息结构体指针
    {
        os_printf("localhost (本地主机号):\n");
        Pip = (uint8_t*)& ST_ESP8266_IP.ip.addr;
        os_printf("IP:%d.%d.%d.%d\n",Pip[0],Pip[1],Pip[2],Pip[3]);
        return true;
    }
    return false;
}


	
