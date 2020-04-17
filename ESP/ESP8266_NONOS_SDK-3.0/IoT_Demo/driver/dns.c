#include "driver/dns.h"

extern GetDnsInfo g_DnsInfo[];

extern struct espconn stcp_Con;

char* dns_ip = NULL;

char count = 0;
ip_addr_t ip_addr;
void ESP8266_dns_found_callback(const char *name, ip_addr_t *ipaddr, void *callback_arg)
{
    struct espconn *pespconn = (struct espconn *)callback_arg;
    if(ipaddr != NULL)
    {
        os_printf("ÓòÃû½âÎöÍê±Ï~~~\n");
        os_printf( IPSTR "\n", IP2STR(ipaddr));
        os_sprintf(dns_ip, IPSTR ,IP2STR(ipaddr));
        if(count--)
        {
        	dns_ip = g_DnsInfo[count].ip_name;
        	dns_parse(&stcp_Con, g_DnsInfo[count].dns_name);
        }
        //Create_Tcp_Connect_Client(&stcp_Con, str, 80);  
        //Create_Tcp_Connect_Client(&stcp_Con, "192.168.0.105", 8888);
    }  
}

void multiple_dns_parse(GetDnsInfo* _DnsInfo, struct espconn*  pEspconn, char _count)
{
	dns_ip = _DnsInfo->ip_name;
	count = _count - 1;
	dns_parse(pEspconn, _DnsInfo->dns_name);
}

void dns_parse(struct espconn* pEspconn, const char * str)
{
    espconn_gethostbyname(pEspconn, str, &ip_addr, (dns_found_callback)ESP8266_dns_found_callback);
}



