#include "JLX_GB2312G_1016.h"
#include "delay.h"
#include "JLX_GB2312z.h"


static void lx_Gb2312g_Write_Cmd(uint8_t uCmdData);
static void lx_Gb2312g_Write_Data(uint8_t uCmdData);
static void lcd_address(uint8_t page,uint8_t column);


void Jlx_gb2312g_io_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(	RCC_AHB1Periph_GPIOE, ENABLE );//PORTBʱ��ʹ�� 
 
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4;

	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;  //PB13/15����������� 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOE, &GPIO_InitStructure);//��ʼ��GPIOB

    GPIO_SetBits(GPIOE, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4);
}


static void lx_Gb2312g_Write_Cmd(uint8_t uCmdData)
{
    JLX_CS_GB2312G = 0;
    JLX_RS_GB2312G = 0;
    for(int i = 0; i < 8; i++)
    {
        JLX_SCLK_GB2312G = 0;
		//delay_us(5);
		JLX_SDA_GB2312G = (uCmdData & 0x80) >> 7;
		//delay_us(5);
		JLX_SCLK_GB2312G = 1;
        uCmdData <<= 1;
    }
    JLX_CS_GB2312G = 1;
}

static void lx_Gb2312g_Write_Data(uint8_t uCmdData)
{
    JLX_CS_GB2312G = 0;
    JLX_RS_GB2312G = 1;
    for(int i = 0; i < 8; i++)
    {
        JLX_SCLK_GB2312G = 0;
		//delay_us(5);
        JLX_SDA_GB2312G = (uCmdData & 0x80) >> 7;
		//delay_us(5);
		JLX_SCLK_GB2312G = 1;
        uCmdData <<= 1;
    }
    JLX_CS_GB2312G = 1;
}
static void lcd_address(uint8_t page,uint8_t column)
{
    column=column-0x01;
    lx_Gb2312g_Write_Cmd(0xb0 + page-1); //����ҳ��ַ��ÿ8 ��Ϊһҳ��ȫ����64 �У����ֳ�8 ҳ
    lx_Gb2312g_Write_Cmd(0x10 + ((column>>4) & 0x0f)); //�����е�ַ�ĸ�4 λ
    lx_Gb2312g_Write_Cmd(column & 0x0f); //�����е�ַ�ĵ�4 λ
}


//LCD ģ���ʼ��
void initial_lcd(void)
{
    Jlx_gb2312g_io_init();
    
    JLX_RESET_GB2312G = 0; //�͵�ƽ��λ
    delay_ms(100);
    JLX_RESET_GB2312G = 1; //��λ���
    delay_ms(1);
    lx_Gb2312g_Write_Cmd(0xe2); //��λ
    delay_ms(1);
    lx_Gb2312g_Write_Cmd(0x2c); //��ѹ����1
    delay_ms(1);
    lx_Gb2312g_Write_Cmd(0x2e); //��ѹ����2
    delay_ms(1);
    lx_Gb2312g_Write_Cmd(0x2f); //��ѹ����3
    delay_ms(1);
    lx_Gb2312g_Write_Cmd(0x23); //�ֵ��Աȶȣ������÷�Χ0x20��0x27
    lx_Gb2312g_Write_Cmd(0x81); //΢���Աȶ�
    lx_Gb2312g_Write_Cmd(0x28); //΢���Աȶȵ�ֵ�������÷�Χ0x00��0x3f
    lx_Gb2312g_Write_Cmd(0xa2); //1/9 ƫѹ�ȣ�bias��
    lx_Gb2312g_Write_Cmd(0xc8); //��ɨ��˳�򣺴��ϵ���
    lx_Gb2312g_Write_Cmd(0xa0); //��ɨ��˳�򣺴�����
    lx_Gb2312g_Write_Cmd(0x40); //��ʼ�У���һ�п�ʼ
    lx_Gb2312g_Write_Cmd(0xaf); //����ʾ
}


//ȫ������
void clear_screen(void)
{
    unsigned char i,j;
    for(i=0;i<8;i++)
    {
        lx_Gb2312g_Write_Cmd(0xb0+i);
        lx_Gb2312g_Write_Cmd(0x10);
        lx_Gb2312g_Write_Cmd(0x00);
        for(j=0;j<128;j++)
        {
            lx_Gb2312g_Write_Data(0x00);
        }
    }
}

void lx_Gb2312g_Write_7X8_Ascll(char uAscll, uint8_t page, uint8_t column, uint8_t Convet)
{
    uint8_t sAscll[12] = {0};
    lcd_address(page, column);
    Get_JLX_7X8_Ascll(uAscll, sAscll, Convet);
    for(int i = 0; i < 8; i++)
    {
        lx_Gb2312g_Write_Data(sAscll[i]);
    }
}

void lx_Gb2312g_Write_5X7_Ascll(char uAscll, uint8_t page, uint8_t column, uint8_t Convet)
{
    uint8_t sAscll[12] = {0};
    lcd_address(page, column);
    Get_JLX_5X7_Ascll(uAscll, sAscll, Convet);
    for(int i = 0; i < 8; i++)
    {
        lx_Gb2312g_Write_Data(sAscll[i]);
    }
}

void lx_Gb2312g_Write_8X16_Ascll(char uAscll, uint8_t page, uint8_t column, uint8_t uIsBold, uint8_t Convet)
{
    uint8_t sAscll[20] = {0};
    lcd_address(page, column);
    if(uIsBold)
        Get_JLX_8X16_Bold_Ascll(uAscll, sAscll, Convet);
    else
        Get_JLX_8X16_Ascll(uAscll, sAscll, Convet);
    for(int i = 0; i < 8; i++)
    {
        lx_Gb2312g_Write_Data(sAscll[i]);
    }
    lcd_address(page + 1, column);
    for(int i = 0; i < 8; i++)
    {
        lx_Gb2312g_Write_Data(sAscll[i + 8]);
    }
}

void lx_Gb2312g_Write_15X16_Chinese(char *uAscll, uint8_t page, uint8_t column, uint8_t Convet)
{
    uint8_t sAscll[36] = {0};
    lcd_address(page, column);
    Get_JLX_15X16_GB2312(uAscll, sAscll, Convet);
    for(int i = 0; i < 16; i++)
    {
        lx_Gb2312g_Write_Data(sAscll[i]);
    }
    lcd_address(page + 1, column);
    for(int i = 0; i < 16; i++)
    {
        lx_Gb2312g_Write_Data(sAscll[i + 16]);
    }
}

void lx_Gb2312g_Str(char *uAscll,uint8_t page, uint8_t column, uint8_t Convet)
{

    uint8_t sAscll[36] = {0};
	
	while(*uAscll)
	{
		if(0 == Get_JLX_15X16_GB2312(uAscll, sAscll, Convet))
		{
 			lcd_address(page, column);
			for(int i = 0; i < 16; i++)
			{
				lx_Gb2312g_Write_Data(sAscll[i]);
			}
			lcd_address(page + 1, column);
			for(int i = 0; i < 16; i++)
			{
				lx_Gb2312g_Write_Data(sAscll[i + 16]);
			}
			column += 16;
			uAscll += 2;
		}
		else if(0 == Get_JLX_8X16_Bold_Ascll(*uAscll, sAscll, Convet))
		{
			lcd_address(page, column);
			for(int i = 0; i < 8; i++)
			{
				lx_Gb2312g_Write_Data(sAscll[i]);
			}
			lcd_address(page + 1, column);
			for(int i = 0; i < 8; i++)
			{
				lx_Gb2312g_Write_Data(sAscll[i + 8]);
			}
			column += 8;
			uAscll += 1;
		}
	}    
}

void lx_Gb2312g_Write_5X7_Ascll_Str(char* pAscll, uint8_t page, uint8_t column, uint8_t Convet)
{
	while(*pAscll)
	{
		lx_Gb2312g_Write_5X7_Ascll(*pAscll,page,column, Convet);
		column += 8;
		pAscll++;
	}
}




