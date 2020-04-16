#include "driver/bsp_exit.h"

void EXTI_handle(void *arg)//�жϴ��������ܼ�ICACHE_FLASH_ATTR
{
    uint32 gpio_status;
    gpio_status = GPIO_REG_READ(GPIO_STATUS_ADDRESS);//��ȡ�ж�״̬
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
    
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO4_U,FUNC_GPIO4);//��������Ϊ��ͨio
    GPIO_DIS_OUTPUT(GPIO_ID_PIN(4));                    //����Ϊ����ģʽ
    //PIN_PULLUP_DIS(PERIPHS_IO_MUX_GPIO4_U) ;          //������ʹ��
    PIN_PULLUP_EN(PERIPHS_IO_MUX_GPIO4_U) ;             //����ʹ��
    ETS_GPIO_INTR_DISABLE();                            //�ر��ж�
    ETS_GPIO_INTR_ATTACH((ets_isr_t)EXTI_handle, (void *)520);//�����жϻص�����
    gpio_pin_intr_state_set(GPIO_ID_PIN(4),GPIO_PIN_INTR_NEGEDGE);//���������жϴ�����ʽ
    ETS_GPIO_INTR_ENABLE();                             //ʹ���ⲿ�����ж�
}


