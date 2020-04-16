#include "driver/bsp_delay.h"


void ICACHE_FLASH_ATTR delay_ms(u32 _time)
{
    while(_time--)
        os_delay_us(1000);
}

void ICACHE_FLASH_ATTR delay_us(u32 _time)
{
    os_delay_us(_time);
}