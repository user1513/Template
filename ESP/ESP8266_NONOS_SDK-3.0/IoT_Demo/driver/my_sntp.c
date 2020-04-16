#include "driver/my_sntp.h"



void my_sntp_init(struct sntp_No_str* tSruct)
{
    sntp_setservername(tSruct[0].num,tSruct[0].str);
    sntp_setservername(tSruct[1].num,tSruct[1].str);
    sntp_setservername(tSruct[2].num,tSruct[2].str);

    sntp_init();
    os_printf("当前处于的时区:%d\n", sntp_get_timezone());//查詢當前時區

}
