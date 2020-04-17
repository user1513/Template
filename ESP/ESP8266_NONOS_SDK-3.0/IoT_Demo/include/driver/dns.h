#ifndef __DNS__H
#define __DNS__H

#include "driver/inc.h"

void dns_parse(struct espconn* pEspconn, const char * str);
void multiple_dns_parse(GetDnsInfo* _DnsInfo, struct espconn*  pEspconn, char _count);
#endif
