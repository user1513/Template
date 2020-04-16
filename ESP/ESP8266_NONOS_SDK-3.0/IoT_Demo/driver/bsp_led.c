#include "driver/bsp_led.h"

void bsp_led_init(void)
{
   gpio16_output_conf() ;
}

void bsp_led_flash(void)
{
    static bool led_state = 0; 
    led_state = !led_state;
    gpio16_output_set(led_state);
}