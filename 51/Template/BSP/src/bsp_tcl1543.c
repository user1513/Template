#include "bsp_tcl1543.h"

static uint adc_val = 0;
static void delay(uchar n)
{
	uchar i;
	for(i=0;i<n;i++)
	{
		 _nop_();
	}
}

uint read2543(uchar port)
{
	uint ad=0,i;
	CLOCK=0;
	_CS=0;
	port<<=4;
	for(i=0;i<12;i++)
	{
		if(D_OUT) ad|=0x01;
		D_IN=(bit)(port&0x80);
		CLOCK=1;
		delay(3);
		CLOCK=0;
		delay(3);
		port<<=1;
		ad<<=1;
	}
	_CS=1;
	ad>>=1;
	adc_val = ad;
	return(ad);
}

uint GetTcl1543(void)
{
	return(adc_val);
}
