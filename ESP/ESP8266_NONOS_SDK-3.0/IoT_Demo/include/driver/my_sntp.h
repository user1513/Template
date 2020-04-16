#ifndef __MY_SNTP_H
#define __MY_SNTP_H

#include "driver/inc.h"

struct sntp_No_str
{
    /* data */
    char num;
    char *str;
};

void my_sntp_init(struct sntp_No_str* tSruct);

#endif
