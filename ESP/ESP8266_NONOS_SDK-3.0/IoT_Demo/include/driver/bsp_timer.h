#ifndef __BSP_TIMER_H
#define __BSP_TIMER_H

#include "driver/inc.h"

#define Led_SoftTimer_init(os_timer,func,ms) bsp_SoftTimer_init(os_timer,func,ms,520,1)


//注意os_timer必须是全局变量
void ICACHE_FLASH_ATTR bsp_SoftTimer_init(os_timer_t* os_timer,
                                            os_timer_func_t* func,
                                            uint32 ms,
                                            uint32 reg,
                                            bool repeat_flag);

//关闭软件定时器
void ICACHE_FLASH_ATTR bsp_SoftTimer_close(os_timer_t* os_timer);

//重新启动软件定时器(可以用来更改定时时间)
void ICACHE_FLASH_ATTR bsp_SoftTimer_Restart(os_timer_t* os_timer,
                                                uint32 ms,
                                                void* reg,
                                                bool repeat_flag);

//硬件定时器初始化
void ICACHE_FLASH_ATTR bsp_HwTimer_init(void (* user_hw_timer_cb_set)(void),uint32_t us);

#endif
