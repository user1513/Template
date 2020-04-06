#include "inc.h"

/****************定义函数****************/
static void s_transstart(void);               //启动传输函数
static char s_write_byte(unsigned char value);//DHT90写函数
static char s_read_byte(unsigned char ack);   //DHT90读函数
static char s_measure(unsigned char *p_value, unsigned char *p_checksum, unsigned char mode);//测量温湿度函数
static void calc_dht90(float *p_humidity ,float *p_temperature);//温湿度补偿


float fHumi = 0,fTemp = 0;

typedef union  
{ unsigned int i;      //定义了两个共用体
  float f;
} value;


/*--------------------------------------
;模块名称:s_transstart();
;功    能:启动传输函数
;占用资源:--
;参数说明:--
;创建日期:2008.08.15
;版    本:FV1.0(函数版本Function Version)
;修改日期:--
;修改说明:--
;-------------------------------------*/  
static void s_transstart(void)
// generates a transmission start  
//       _____         ________
// DATA:      |_______|
//           ___     ___
// SCK : ___|   |___|   |______
{   
   SHT11_DATA=1; SHT11_SCK=0;                   //Initial state
   _nop_();
   SHT11_SCK=1;
   _nop_();
   SHT11_DATA=0;
   _nop_();
   SHT11_SCK=0;   
   _nop_();_nop_();_nop_();
   SHT11_SCK=1;
   _nop_();
   SHT11_DATA=1;        
   _nop_();
   SHT11_SCK=0;        
}


/*--------------------------------------
;模块名称:s_connectionreset();
;功    能:连接复位函数
;占用资源:--
;参数说明:--
;创建日期:2008.08.15
;版    本:FV1.0(函数版本Function Version)
;修改日期:--
;修改说明:--
;-------------------------------------*/
void s_connectionreset(void)
// communication reset: DATA-line=1 and at least 9 SCK cycles followed by transstart
//       _____________________________________________________         ________
// DATA:                                                      |_______|
//          _    _    _    _    _    _    _    _    _        ___     ___
// SCK : __| |__| |__| |__| |__| |__| |__| |__| |__| |______|   |___|   |______
{   
  unsigned char i;  
  SHT11_DATA=1; SHT11_SCK=0;                    //Initial state
  for(i=0;i<9;i++)                  //9 SCK cycles
  {
    SHT11_SCK=1;
    SHT11_SCK=0;
  }
  s_transstart();                   //transmission start
}


/*--------------------------------------
;模块名称:s_write_byte();
;功    能:DHT90写函数
;占用资源:--
;参数说明:--
;创建日期:2008.08.15
;版    本:FV1.0(函数版本Function Version)
;修改日期:--
;修改说明:--
;-------------------------------------*/
static char s_write_byte(unsigned char value)
//----------------------------------------------------------------------------------
// writes a byte on the Sensibus and checks the acknowledge  
{  
  unsigned char i,error=0;   
  for (i=0x80;i>0;i/=2)             //shift bit for masking
  {  
    if (i & value) SHT11_DATA=1;          //masking value with i , write to SENSI-BUS
    else SHT11_DATA=0;                        
    SHT11_SCK=1;                          //clk for SENSI-BUS
    _nop_();_nop_();_nop_();        //pulswith approx. 3 us     
    SHT11_SCK=0;
  }
  SHT11_DATA=1;                           //release DATA-line
  SHT11_SCK=1;                            //clk #9 for ack  
  error=SHT11_DATA;                       //check ack (DATA will be pulled down by DHT90),DATA在第9个上升沿将被DHT90自动下拉为低电平。
  _nop_();_nop_();_nop_();
  SHT11_SCK=0;
  SHT11_DATA=1;                           //release DATA-line
  return error;                     //error=1 in case of no acknowledge //返回：0成功，1失败
}

/*--------------------------------------
;模块名称:s_read_byte();
;功    能:DHT11读函数
;占用资源:--
;参数说明:--
;创建日期:2008.08.15
;版    本:FV1.0(函数版本Function Version)
;修改日期:--
;修改说明:--
;-------------------------------------*/
static char s_read_byte(unsigned char ack)  
// reads a byte form the Sensibus and gives an acknowledge in case of "ack=1"  
{  
  unsigned char i,val=0;
  SHT11_DATA=1;                           //release DATA-line
  for (i=0x80;i>0;i/=2)             //shift bit for masking
  { SHT11_SCK=1;                          //clk for SENSI-BUS
    if (SHT11_DATA) val=(val | i);        //read bit   
        _nop_();_nop_();_nop_();        //pulswith approx. 3 us
    SHT11_SCK=0;              
  }
  if(ack==1)SHT11_DATA=0;                 //in case of "ack==1" pull down DATA-Line
  else SHT11_DATA=1;                      //如果是校验(ack==0)，读取完后结束通讯
  _nop_();_nop_();_nop_();          //pulswith approx. 3 us
  SHT11_SCK=1;                            //clk #9 for ack
  _nop_();_nop_();_nop_();          //pulswith approx. 3 us  
  SHT11_SCK=0;                 
  _nop_();_nop_();_nop_();          //pulswith approx. 3 us
  SHT11_DATA=1;                           //release DATA-line
  return val;
}




/*--------------------------------------
;模块名称:s_measure();
;功    能:测量温湿度函数
;占用资源:--
;参数说明:--
;创建日期:2008.08.15
;版    本:FV1.0(函数版本Function Version)
;修改日期:--
;修改说明:--
;-------------------------------------*/
static char s_measure(unsigned char *p_value, unsigned char *p_checksum, unsigned char mode)
// makes a measurement (humidity/temperature) with checksum
{  
  unsigned error=0;
  unsigned int i;

  s_transstart();                   //transmission start
  switch(mode){                     //send command to sensor
    case TEMP  : error+=s_write_byte(MEASURE_TEMP); break;
    case HUMI  : error+=s_write_byte(MEASURE_HUMI); break;
    default     : break;   
  }
  for (i=0;i<65535;i++) if(SHT11_DATA==0) break; //wait until sensor has finished the measurement
  if(SHT11_DATA) error+=1;                // or timeout (~2 sec.) is reached
  *(p_value)  =s_read_byte(ACK);    //read the first byte (MSB)
  *(p_value+1)=s_read_byte(ACK);    //read the second byte (LSB)
  *p_checksum =s_read_byte(noACK);  //read checksum
  return error;
}


/*--------------------------------------
;模块名称:calc_dht90();
;功    能:温湿度补偿函数
;占用资源:--
;参数说明:--
;创建日期:2008.08.15
;版    本:FV1.0(函数版本Function Version)
;修改日期:--
;修改说明:--
;-------------------------------------*/
static void calc_dht90(float *p_humidity ,float *p_temperature)
// calculates temperature [C] and humidity [%RH]
// input :  humi [Ticks] (12 bit)
//          temp [Ticks] (14 bit)
// output:  humi [%RH]
//          temp [C]
{ const float C1=-4.0;              // for 12 Bit
  const float C2=+0.0405;           // for 12 Bit
  const float C3=-0.0000028;        // for 12 Bit
  const float T1=+0.01;             // for 14 Bit @ 5V
  const float T2=+0.00008;           // for 14 Bit @ 5V

  float rh=*p_humidity;             // rh:      Humidity [Ticks] 12 Bit
  float t=*p_temperature;           // t:       Temperature [Ticks] 14 Bit
  float rh_lin;                     // rh_lin:  Humidity linear
  float rh_true;                    // rh_true: Temperature compensated humidity
  float t_C;                        // t_C   :  Temperature [C]

  t_C=t*0.01 - 40;                  //calc. temperature from ticks to [C]
  rh_lin=C3*rh*rh + C2*rh + C1;     //calc. humidity from ticks to [%RH]
  rh_true=(t_C-25)*(T1+T2*rh)+rh_lin-3;   //calc. temperature compensated humidity [%RH]
  if(rh_true>100)rh_true=100;       //cut if the value is outside of
  if(rh_true<0.1)rh_true=0.1;       //the physical possible range

  *p_temperature=t_C;               //return temperature [C]
  *p_humidity=rh_true;              //return humidity[%RH]
}

void vAutoGetSht11(char isprintf){
	value humi_val,temp_val;
	unsigned char checkval = 0;
	s_measure((unsigned char *)&humi_val.i, (unsigned char *)&checkval, HUMI);
	s_measure((unsigned char *)&temp_val.i, (unsigned char *)&checkval, TEMP);
	humi_val.f = humi_val.i;
	temp_val.f = temp_val.i;
	calc_dht90(&humi_val.f, &temp_val.f);
	fTemp = temp_val.f;
	fHumi = humi_val.f;
	if(isprintf)
		printf("温度:%05.1f℃,湿度%05.1f%%\n", fTemp, fHumi);
}

float fGetHumi(void){
	return fHumi;
}

float fGetTemp(void){
	return fTemp;
}


