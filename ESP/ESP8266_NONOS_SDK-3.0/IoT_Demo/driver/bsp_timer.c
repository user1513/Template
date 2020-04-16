#include "driver/bsp_timer.h"




//ע��os_timer������ȫ�ֱ���
void ICACHE_FLASH_ATTR bsp_SoftTimer_init(os_timer_t* os_timer,
                                            os_timer_func_t* func,
                                            uint32 ms,
                                            uint32 reg,
                                            bool repeat_flag)
{
    os_timer_disarm(os_timer); //ȡ����ʱ����ʱ
    os_timer_setfn(os_timer,
                  (os_timer_func_t *)func,
                  (void *)reg);
    os_timer_arm(os_timer, ms, repeat_flag);
}

void ICACHE_FLASH_ATTR bsp_SoftTimer_close(os_timer_t* os_timer)
{
    os_timer_disarm(os_timer); //ȡ����ʱ����ʱ
}
//�����ʱ����������
void ICACHE_FLASH_ATTR bsp_SoftTimer_Restart(os_timer_t* os_timer,
                                                uint32 ms,
                                                void* reg,
                                                bool repeat_flag)
{
    os_timer_disarm(os_timer); //ȡ����ʱ����ʱ
    os_timer->timer_arg = reg;
    os_timer_arm(os_timer, ms, repeat_flag);
}


void HW_Timer_INT(void)		// �ڣ�Ӳ����ʱ���жϻص�����
{
	os_printf("\r\n--- HW_Timer_INT ---\r\n");	// ����Ӳ����ʱ���жϻص�������־
}
void ICACHE_FLASH_ATTR bsp_HwTimer_init(void (* user_hw_timer_cb_set)(void),uint32_t us)
{
    hw_timer_init(0, 1);			// �٣���ʼ��Ӳ����ʱ��������1���ж�Դ��������2���Ƿ��ظ���
	hw_timer_set_func(user_hw_timer_cb_set);// �ۣ�ע��Ӳ����ʱ���жϻص�����
	hw_timer_arm(us);			// �ܣ����ö�ʱ������(��λus����������<=1,677,721)
}




