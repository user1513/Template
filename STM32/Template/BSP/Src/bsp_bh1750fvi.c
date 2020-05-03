#include "bsp_bh1750fvi.h"
#include "bsp_swi2c.h"

static void bsp_bh1750fvi_out(void);

static void bsp_bh1750fvi_in(void);

/*����bh1750fvi��I2Cͷ��*/
static void bh1750fvi_send_header(uint8_t _ucWR);

/*����bh1750fvi��I2C����*/
static void bh1750fvi_send_cmd(uint8_t _ucCmd);

/*����bh1750fvi���豸�ϵ�*/
static void bh1750fvi_send_power_on(void);

/*����bh1750fvi��I2C���ݶ�ȡ*/
static void bh1750fvi_read_data(uint8_t* _ucTable, uint8_t length);

Swi2c_T t_bh1750fvi_swi2c ={
	(unsigned long *)&BH1750FVI_I2C_SCL,
	(unsigned long *)&BH1750FVI_I2C_SDA_OUT,
	(unsigned long *)&BH1750FVI_I2C_SDA_IN,
	(I2C_FUNC)bsp_bh1750fvi_in,
	(I2C_FUNC)bsp_bh1750fvi_out
};


void bsp_bh1750fvi_io_init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOD, ENABLE);//ʹ��GPIOC,GPIODʱ��

	//GPIOA3,4��ʼ������
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;//LED0��LED1��ӦIO��
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//��ͨ���ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
	GPIO_Init(GPIOC, &GPIO_InitStructure);//��ʼ��GPIO

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;//LED0��LED1��ӦIO��
    GPIO_Init(GPIOD, &GPIO_InitStructure);//��ʼ��GPIO
    
    GPIO_SetBits(GPIOC, GPIO_Pin_7);//GPIOC7�ø�   
    GPIO_SetBits(GPIOD, GPIO_Pin_15);//GPIOC7�ø�

    bh1750fvi_send_power_on();

}


static void bsp_bh1750fvi_out(void)
{
    BH1750FVI_IO_OUT()
}

static void bsp_bh1750fvi_in(void)
{
    BH1750FVI_IO_IN()
}



/*����bh1750fvi��I2Cͷ��*/
static void bh1750fvi_send_header(uint8_t _ucWR)

{
    swi2cStart(&t_bh1750fvi_swi2c);											        /*I2C��ʼ�ź�*/

	swi2cSendByte(&t_bh1750fvi_swi2c, BH1750FVI_DEVICE_ADDR + _ucWR);               /*����BH1750FVIͷ����Ϣ*/
 
	swi2cWaitAck(&t_bh1750fvi_swi2c, 1);									        /*�ȴ�BH1750FVI��ack*/
}

/*����bh1750fvi��I2C����*/
static void bh1750fvi_send_cmd(uint8_t _ucCmd)

{
	swi2cSendByte(&t_bh1750fvi_swi2c, _ucCmd);                                      /*����BH1750FVIͷ����Ϣ*/
 
	swi2cWaitAck(&t_bh1750fvi_swi2c, 1);									        /*�ȴ�BH1750FVI��ack*/
}

/*����bh1750fvi���豸�ϵ�*/
static void bh1750fvi_send_power_on(void)
{
    bh1750fvi_send_header(0);                                                      /*����ͷ��*/

    bh1750fvi_send_cmd(BH1750FVI_POWER_OFF);                                       /*�����ϵ�����*/     

    swi2cStop(&t_bh1750fvi_swi2c);
}

/*����bh1750fvi��I2C���ݶ�ȡ*/
static void bh1750fvi_read_data(uint8_t* _ucTable, uint8_t length)

{
	for(int i = 0; i < length; i++)
	{
		_ucTable[i] = swi2cReceiveByte(&t_bh1750fvi_swi2c);						/*��ȡsgp30����Ϣ*/

		swi2cAck(&t_bh1750fvi_swi2c, i < (length - 1) ? 1 : 0);						/*master����ack*/
	}
								
}

/*ע��������͵���������Σ���Ҫÿ�β���ǰ���豸�ϵ�*/
uint16_t bh1750fvi_send_measure(uint8_t ucMode)

{
    if(ucMode & 0x20)                                                               /*�ж��Ƿ��ǵ��β���*/
        bh1750fvi_send_power_on();                                                  /*�豸�ϵ�*/

    bh1750fvi_send_header(0);                                                       /*����ͷ��*/
    
    bh1750fvi_send_cmd(ucMode);                                                     /*���Ͳ���ģʽ*/

    swi2cStop(&t_bh1750fvi_swi2c);                                                  /*ֹͣI2C*/

    if(ucMode & 0x03)                                                               /*�ж��Ƿ��ǵ��β���*/
        delay_ms(BH1750FVI_L_RESOLUTION_MEASURE_TIME);  
    else
        delay_ms(BH1750FVI_H_RESOLUTION_MEASURE_TIME); 
    
    bh1750fvi_send_header(1);                                                       /*����ͷ��*/                                                                   

    uint8_t ucaTmp[2] = {0};

    bh1750fvi_read_data(ucaTmp, 2);                                                 /*��ȡ��������*/

    swi2cStop(&t_bh1750fvi_swi2c);                                                  /*ֹͣI2C*/

    return ((ucaTmp[0] << 8) + ucaTmp[1]) / 1.2;                                    /*���ز���ֵ��λlx*/
}
