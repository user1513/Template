#include "bsp_keypad.h"

static unsigned char key_val = 0XFF;

code char hexaKeys[4][4] = {
  {'7','4','1','S'},
  {'8','5','2','0'},
  {'9','6','3','='},
  {'.','R','L','C'}
};

static void Delay5ms()		//@11.0592MHz
{
	unsigned char i, j;

	i = 9;
	j = 244;
	do
	{
		while (--j);
	} while (--i);
}



uint8_t uGetKeyPadAction(void)
{
	uint8_t i = 1, temp = 1;
	uint8_t uReturn = 0xff;
	SET_KEYPAD(0x0f);
	for(i = 0; i < 4; i++)
	{
		RESET_KEYPAD(temp);
		Delay5ms();
		switch(GET_KEYPAD_VAL)
		{
			case 1:uReturn = 0; break;
			case 2:uReturn = 1; break;
			case 4:uReturn = 2; break;
			case 8:uReturn = 3; break;		
			default : break;
		}
		SET_KEYPAD(temp);
		temp <<= 1; 
		if(uReturn != 0xff)
		{
			uReturn = 4 * i + uReturn;
			break;
		}
	}
	if(key_val != uReturn && uReturn != 0xff)
	{
		key_val = (hexaKeys[i][uReturn % 4] );
	}
	return uReturn;
}

unsigned char Get_KeyPad(void)
{
	return key_val;
}

void uClear_KeyPad(void)
{
	key_val = 0xff;
}




