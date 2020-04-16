#ifndef __DNS__H
#define __DNS__H

#include "driver/inc.h"

void dns_parse(struct espconn* pEspconn, const char * str);

#endif
