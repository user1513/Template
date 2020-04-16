#ifndef _BSP_LED_H
#define _BSP_LED_H

#include "driver/inc.h"

#define LED(x) gpio16_output_set(x)

void bsp_led_init(void);
void bsp_led_flash(void);

#endif
