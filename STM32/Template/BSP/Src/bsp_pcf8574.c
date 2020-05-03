#include "bsp_pcf8574.h"

/*����bsp_swi2c.h�ڵ�Swi2c_T�ṹ�����*/

static void bsp_pcf8974x_out(void);
static void bsp_pcf8974x_in(void);

#if PCF8574_INT_STATE
static void pcf8974x_int_init(void);
static void bsp_pcf8974x_int_init(void);
#endif

Swi2c_T t_pcf8574_swi2c ={
	(unsigned long *)&PCF8974X_I2C_SCL,
	(unsigned long *)&PCF8974X_I2C_SDA_OUT,
	(unsigned long *)&PCF8974X_I2C_SDA_IN,
	bsp_pcf8974x_in,
	bsp_pcf8974x_out
};


void bsp_pcf8974x_io_init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);//ʹ��GPIOAʱ��

	//GPIOA3,4��ʼ������
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;//LED0��LED1��ӦIO��
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//��ͨ���ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
	GPIO_Init(GPIOD, &GPIO_InitStructure);//��ʼ��GPIO

    GPIO_SetBits(GPIOD, GPIO_Pin_0 | GPIO_Pin_1);//GPIOA3,4�ø�

    #if PCF8574_INT_STATE

    pcf8974x_int_init();
    
    #endif

}

static void bsp_pcf8974x_out(void)
{
    PCF8974X_IO_OUT()
}

static void bsp_pcf8974x_in(void)
{
    PCF8974X_IO_IN()
}

static void bsp_pcf8974x_header(uint8_t _ucDeviceNo,uint8_t _ucWR)

{									
	swi2cSendByte(&t_pcf8574_swi2c, PCF8574X_ADDR +  ((_ucDeviceNo & 0x07) << 1) + (_ucWR & (0x01)));/*����sgp30ͷ����Ϣ*/
 
	swi2cWaitAck(&t_pcf8574_swi2c, 1);									/*�ȴ�ack*/
}

/*ע��_ucBitNum��0��ʼ��7����*/
void bsp_pcf8974x_bit_set(uint8_t _ucDeviceNo,uint8_t _ucBitNum, uint8_t level)
{
	uint8_t tmp = bsp_pcf8974x_receive(_ucDeviceNo);
	if(level)
		tmp |= 1 << _ucBitNum;
	else
		tmp &= ~(1 << _ucBitNum);
		
	bsp_pcf8974x_test(_ucDeviceNo, tmp);
}

void bsp_pcf8974x_test(uint8_t _ucDeviceNo, uint8_t _ucData)
	
{
    /*���i2c��ʼ�ź�*/
    swi2cStart(&t_pcf8574_swi2c);

    /*дpcf8974x��I2Cͷ���ź�*/
    bsp_pcf8974x_header(_ucDeviceNo, 0);

    /*��������*/
    swi2cSendByte(&t_pcf8574_swi2c, _ucData);

    /*�ȴ�ack*/
    swi2cWaitAck(&t_pcf8574_swi2c, 1);									

    /*���i2cֹͣ�ź�*/
    swi2cStop(&t_pcf8574_swi2c);
}

/*��ȡpcf8974x ��I2c����*/

uint8_t bsp_pcf8974x_receive(uint8_t _ucDeviceNo) 
{
	uint8_t ucData = 0x00;
    /*���i2c��ʼ�ź�*/
    swi2cStart(&t_pcf8574_swi2c);

    /*дpcf8974x��I2Cͷ���ź�*/
    bsp_pcf8974x_header(_ucDeviceNo, 1);
	
	/*��ȡi2c����*/
	ucData = swi2cReceiveByte(&t_pcf8574_swi2c);
	
    /*�ȴ�ack*/
    swi2cAck(&t_pcf8574_swi2c, 1);									

    /*���i2cֹͣ�ź�*/
    swi2cStop(&t_pcf8574_swi2c);
	
	return ucData;
}

#if PCF8574_INT_STATE

//�ⲿ�ж�2�������
void EXTI9_5_IRQHandler(void)
{
    if(EXTI_GetITStatus(EXTI_Line6) == SET)
    {
     	uint8_t ucData = 0;

		ucData = bsp_pcf8974x_receive(0x01);	
		
		printf("bsp_pcf8974x_receive = %x\n", ucData);

	    EXTI_ClearITPendingBit(EXTI_Line6);//���LINE2�ϵ��жϱ�־λ 
    }
}

static void bsp_pcf8974x_int_init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//ʹ��GPIOAʱ��

	//GPIOA3,4��ʼ������
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;//LED0��LED1��ӦIO��
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//��ͨ���ģʽ
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIO
}
 
//�ⲿ�жϳ�ʼ������
//��ʼ��PE2~4,PA0Ϊ�ж�����.
static void pcf8974x_int_init(void)
{
	NVIC_InitTypeDef   NVIC_InitStructure;
	EXTI_InitTypeDef   EXTI_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);//ʹ��SYSCFGʱ��

	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource6);//PA6 ���ӵ��ж���6

    bsp_pcf8974x_int_init();

	/* ����EXTI_Line0 */
	EXTI_InitStructure.EXTI_Line = EXTI_Line6;//LINE0
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;//�ж��¼�
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; //�½��ش��� 
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;//ʹ��LINE0
	EXTI_Init(&EXTI_InitStructure);//����

	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;//�ⲿ�ж�6
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;//��ռ���ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;//�����ȼ�2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//ʹ���ⲿ�ж�ͨ��
	NVIC_Init(&NVIC_InitStructure);//����
}

#endif




