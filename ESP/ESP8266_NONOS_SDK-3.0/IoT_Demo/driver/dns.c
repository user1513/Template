#include "driver/dns.h"

ip_addr_t ip_addr;

extern struct espconn stcp_Con;

void ESP8266_dns_found_callback(const char *name, ip_addr_t *ipaddr, void *callback_arg)
{
    char str[16];
    struct espconn *pespconn = (struct espconn *)callback_arg;
    if(ipaddr != NULL)
    {
        os_printf("ÓòÃû½âÎöÍê±Ï~~~\n");
        os_printf( IPSTR "\n", IP2STR(ipaddr));
        os_sprintf(str, IPSTR ,IP2STR(ipaddr));
        Create_Tcp_Connect_Client(&stcp_Con, str, 80);  
    }  
}

void dns_parse(struct espconn* pEspconn, const char * str)
{
    espconn_gethostbyname(pEspconn, str, &ip_addr, (dns_found_callback)ESP8266_dns_found_callback);
}
