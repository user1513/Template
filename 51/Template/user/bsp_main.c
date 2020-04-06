#include "inc.h"


xdata Dishes Dishes_temp[] ={
{"qingcai",2.2},
{"dabaicai",2.8},
{"xiguan",4.9},
{"xiangjiao",71.3},
{"xihongshi",88.2},
{"niunan",99},
{"jinju",19},
{"niunan",22},
{"qiezi",33},
{"huasheng",55},
{"niunai",88},};

Info Dishes_Info = {0,0,2.2,0,0,0};

void Delay200ms()		//@11.0592MHz
{
	unsigned char i, j, k;
	_nop_();
	i = 2;
	j = 103;
	k = 147;
	do
	{
		do
		{
			while (--k);
		} while (--j);
	} while (--i);
}


static void bsp_init(void);
static void lcd_display(void);
char xdata str[50] = 0;
void main(void)
{
	unsigned char key = 0;
	uint8_t flag = 0,flag_temp = 0;
	bsp_init();
	while(1)
	{
		key = Get_KeyPad() ;
		Dishes_Info.weight = Get_ES_Val() - Dishes_Info.Peeled;
		sprintf(str, "通道号:%d,重量:%05.3fkg,菜品编号:%d,""总价:",(int)(Dishes_Info.WhichES + 1), Dishes_Info.weight, (int)Dishes_Info.Num);
		sprintf(str + strlen(str), "%06.2f;",Dishes_Info.money);
		Send_String(str);
		
		if(flag < 4)
		{
			
			sprintf(str, "WE:%05.3f PR:%04.1f",Dishes_Info.weight,Dishes_Info.price);
			LcdShowStr(0x80,str);
			Dishes_Info.money = Dishes_Info.weight * Dishes_Info.price;
			sprintf(str, "MONEY:%06.2fNo:%d", Dishes_Info.money, (int)Dishes_Info.Num);
			LcdShowStr(0x80 + 0x40,str);
			switch(key)
			{
				case 'S':  flag = 1;  uClear_KeyPad();break;
				case '=':  flag = 2;  uClear_KeyPad();break;
				case 'C':  flag = 3;  flag += 1<<4 ;
						   LcdShowStr(0x80,"                ");
						   LcdShowStr(0x80+ 0x40,"                ");
						   uClear_KeyPad();break;
				case 0xff:break;
				case '0':
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '7':
				case '8':
				case '9':
							if(flag == 1)
							{
								flag += 1<<4 ;
								LcdShowStr(0x80,"                ");
								LcdShowStr(0x80+ 0x40,"                ");
							}else if(flag == 2)
							{
								flag += 1<<4;
							}
							break;
				default:break;
			}			
		}
		if((flag & (1<<4)))
		{
			flag_temp = flag ;
			flag_temp &= ~(1<<4); 
			if(key == 'S')
			{
				flag = 0;
				uClear_KeyPad();
			}	
			switch(flag_temp)
			{
				case 1:
			
				if((key != 'S' ) && (key != 0xff)) 
				{
					Dishes_Info.price = Dishes_temp[key-'0'].price;
					Dishes_Info.Num = key - '0';
					//sprintf(str, "菜名:%s, 单价:%2.1f\n",Dishes_temp[key-'0'].DisheName, Dishes_Info.price);
					//Send_String(str);
					
					sprintf(str, " name:%s",Dishes_temp[key-'0'].DisheName);
					LcdShowStr(0x80,str);
					
					sprintf(str, "price:%2.1f      ",Dishes_Info.price);	
					LcdShowStr(0x80 + 0x40,str);
					uClear_KeyPad();	
				}
				break;
				case 2:
				if((key >= '0') && (key <= '9'))
				{
					LcdShowStr(0x80 ,"Electronic scale");
					sprintf(str, "USE_MODE: NO:%d  ",(int)(key-'0'));
					LcdShowStr(0x80 + 0x40 ,str);	
					Dishes_Info.WhichES = key-'0';
					uClear_KeyPad();
				}
				break;
				case 3:
					if(key != 'S')
					{
						Dishes_Info.Peeled = Get_ES_Val();
						//sprintf(str, "Peeled:%f01.3", Dishes_Info.Peeled);
						//
						Send_String(str);
						LcdShowStr(0x80,"  name:Peeled  ");
						
						sprintf(str, "val:%2.3f      ",Dishes_Info.Peeled);	
						LcdShowStr(0x80 + 0x40,str);	
					}
				break;
				default :break;
			}
			
			
			
		}
		
		Delay200ms();

	}
}

static void lcd_display(void)
{
	LcdInit();	
	LcdShowStr(0x80 ,"   Welcome use  ");
	LcdShowStr(0x80 + 0x40 ,"Electronic scale");
	Delay200ms();
	Delay200ms();
	LcdShowStr(0x80 ,"  class: b16   ");
	LcdShowStr(0x80+ 0x40," name: W X D");
	Delay200ms();
	Delay200ms();
}

static void bsp_init(void)
{
	lcd_display();
	Timer0Init();
	UartInit();
	sprintf(str, "通道号:%d,重量:%05.3fkg,菜品编号:%d,总价:%06.2f;",(int)1, Dishes_Info.weight, (int)Dishes_Info.Num, Dishes_Info.money);
	Send_String(str);

	sprintf(str, "通道号:%d,重量:%05.3fkg,菜品编号:%d,总价:%06.2f;",(int)2, Dishes_Info.weight, (int)Dishes_Info.Num, Dishes_Info.money);
	Send_String(str);
			
	sprintf(str, "通道号:%d,重量:%05.3fkg,菜品编号:%d,总价:%06.2f;",(int)3, Dishes_Info.weight, (int)Dishes_Info.Num, Dishes_Info.money);
	Send_String(str);

	sprintf(str, "通道号:%d,重量:%05.3fkg,菜品编号:%d,总价:%06.2f;",(int)4, Dishes_Info.weight, (int)Dishes_Info.Num, Dishes_Info.money);
	Send_String(str);	
}

































