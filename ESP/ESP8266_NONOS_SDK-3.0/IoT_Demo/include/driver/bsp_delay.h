#ifndef __BSP_DELAY_H
#define __BSP_DELAY_H

#include "driver/inc.h"

void ICACHE_FLASH_ATTR delay_ms(u32 _time);
void ICACHE_FLASH_ATTR delay_us(u32 _time);
#endif