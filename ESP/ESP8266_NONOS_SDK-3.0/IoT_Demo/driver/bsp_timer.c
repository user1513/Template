#include "driver/bsp_timer.h"




//注意os_timer必须是全局变量
void ICACHE_FLASH_ATTR bsp_SoftTimer_init(os_timer_t* os_timer,
                                            os_timer_func_t* func,
                                            uint32 ms,
                                            uint32 reg,
                                            bool repeat_flag)
{
    os_timer_disarm(os_timer); //取消定时器定时
    os_timer_setfn(os_timer,
                  (os_timer_func_t *)func,
                  (void *)reg);
    os_timer_arm(os_timer, ms, repeat_flag);
}

void ICACHE_FLASH_ATTR bsp_SoftTimer_close(os_timer_t* os_timer)
{
    os_timer_disarm(os_timer); //取消定时器定时
}
//软件定时器重新启动
void ICACHE_FLASH_ATTR bsp_SoftTimer_Restart(os_timer_t* os_timer,
                                                uint32 ms,
                                                void* reg,
                                                bool repeat_flag)
{
    os_timer_disarm(os_timer); //取消定时器定时
    os_timer->timer_arg = reg;
    os_timer_arm(os_timer, ms, repeat_flag);
}


void HW_Timer_INT(void)		// ②：硬件定时器中断回调函数
{
	os_printf("\r\n--- HW_Timer_INT ---\r\n");	// 进入硬件定时器中断回调函数标志
}
void ICACHE_FLASH_ATTR bsp_HwTimer_init(void (* user_hw_timer_cb_set)(void),uint32_t us)
{
    hw_timer_init(0, 1);			// ①：初始化硬件定时器【参数1：中断源】【参数2：是否重复】
	hw_timer_set_func(user_hw_timer_cb_set);// ③：注册硬件定时器中断回调函数
	hw_timer_arm(us);			// ④：设置定时器参数(单位us，参数必须<=1,677,721)
}




