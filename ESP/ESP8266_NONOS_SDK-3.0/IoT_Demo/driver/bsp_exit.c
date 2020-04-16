#include "driver/bsp_exit.h"

void EXTI_handle(void *arg)//中断处理函数不能加ICACHE_FLASH_ATTR
{
    uint32 gpio_status;
    gpio_status = GPIO_REG_READ(GPIO_STATUS_ADDRESS);//获取中断状态
    //clear interrupt status
    GPIO_REG_WRITE(GPIO_STATUS_W1TC_ADDRESS, gpio_status);

    if(gpio_status & (1 << 4))
    {
        os_printf("This is exit test!!\n");
        os_printf("void * = %d\n" ,(int)arg);
    }
}

void ICACHE_FLASH_ATTR bsp_exit_init(void)
{
    
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO4_U,FUNC_GPIO4);//设置引脚为普通io
    GPIO_DIS_OUTPUT(GPIO_ID_PIN(4));                    //设置为输入模式
    //PIN_PULLUP_DIS(PERIPHS_IO_MUX_GPIO4_U) ;          //上拉不使能
    PIN_PULLUP_EN(PERIPHS_IO_MUX_GPIO4_U) ;             //上拉使能
    ETS_GPIO_INTR_DISABLE();                            //关闭中断
    ETS_GPIO_INTR_ATTACH((ets_isr_t)EXTI_handle, (void *)520);//设置中断回调函数
    gpio_pin_intr_state_set(GPIO_ID_PIN(4),GPIO_PIN_INTR_NEGEDGE);//设置引脚中断触发方式
    ETS_GPIO_INTR_ENABLE();                             //使能外部触发中断
}


