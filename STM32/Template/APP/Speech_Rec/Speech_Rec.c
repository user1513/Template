#include "Speech_Rec.h"
#include "Voice_Access.h"
#include "stdint.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "FreeRTOS.h"
#include "bsp_dma.h"
#include "exfuns.h" 

#define FH_NAME 0X55AA  /*Frame Header帧头*/
#define FT_NAME 0XA5  	/*Frame Tail帧头*/
#define DEVICE_ADDR 0X10/*设备地址*/

#define SPEECH_REC_TYPE_START		0/*打开TCP连接,等待语音识别包发送*/
#define SPEECH_REC_TYPE_END			2/*关闭TCP连接*/

#define POST_NAME 	"http://vop.baidu.com/server_api" 	/*语音识别POST端口*/
#define HOST_NAME 	"vop.baidu.com"						/*语音识别主机名*/
#define FORMAT 		"pcm"								/*语音识别文件类型*/
#define DEV_PID		1537								/*dev_pid输入法模型*/

/*token:开放平台获取到的开发者[access_token]获取 Access Token "access_token")
一个月会更换一次*/
#define TOKEN		"24.91af5b7738399578bc9c7920f4558d06.2592000.1589172592.282335-19369127"

/*用户唯一标识，用来区分用户，计算UV值。建议填写能区分用户的机器 MAC 地址或 IMEI 码，长度为60字符以内。*/
#define CUID		"DC4A3EE6B0D9"


const char * SpeechRecStr = "\
POST %s HTTP/1.1\r\n\
Host: %s\r\n\
Connection: keep-alive\r\n\
Content-Type: application/json\r\n\
Content-length: %d\r\n\
\r\n%s";

const char * SpeechRecStrBody = "\
{\"format\":\"%s\",\"rate\":16000,\"dev_pid\":%d,\"channel\":1,\"token\":\"%s\",\"cuid\":\"%s\",\"len\":%d,\"speech\":\"";

/*用于将一段数据打包成合格的数据包结构*/
static uint32_t UartDataPacking(char* str, uint8_t type, uint32_t length, uint8_t Check);
/*数据前部打包*/
static void UartDataFrontPacking(char* str, uint8_t type, uint32_t length);
/*数据尾部打包*/
static void UartDataTailPacking(char* str, uint32_t length, uint8_t Offset, uint8_t Check);

char g_Str[20];

/*语音识别处理函数*/
void Speech_Handle(uint8_t _ucMode)

{
    static uint8_t status = 0xff;
    switch(_ucMode)
    {
        case 0:
		UartDataPacking(g_Str,SPEECH_REC_TYPE_START,0,0);	
		usartSendStart((uint8_t*)g_Str, 9);/*数据长度+数据包结构*/
		status = voice_Receive(); break;
        case 1://if(!status) {status = 0xff; voice_End_Receive(); voice_info_Send();} break;
			if(!status) 
				{
					status = 0xff; 
					voice_End_Receive(); 
					delay_ms(50);
					SpeechRecUartPack();	
				}
				break;
        default: break;
    }
}




void SpeechRecUartPack(void)
{
	char* str = (char*)pvPortMalloc(sizeof(char)*400);/*分配空间*/

	char* str1 = (char*)pvPortMalloc(sizeof(char)*200);/*分配空间*/
	
	printf("开始发送语音识别包\n");

	uint32_t length = 0;

	uint32_t filelength = voice_info_size();/*获取base64加密后的文件大小*/
	
	sprintf(str1,SpeechRecStrBody,FORMAT,DEV_PID,TOKEN,CUID, (int)(filelength * 0.75));
	
	length = strlen(str1);
	
	sprintf(str,SpeechRecStr,POST_NAME,HOST_NAME,length + filelength + 2,str1);
	
	SetDmaStatus(0x80);		/*设置dma模式*/
	
	usartSendStart((uint8_t*)str, strlen(str));/*数据长度+数据包结构*/
	
	vPortFree(str1);
	
	while(GetDmaStatus()&0x80);
	
	vPortFree(str);
	
	str = (char*)pvPortMalloc(sizeof(char)*2000);

	str1 = (char*)pvPortMalloc(sizeof(char)*2000);

	f_open(file, "Speech_Rec.pcm", FA_READ| FA_OPEN_ALWAYS);
	
	uint32_t br = 0;
	
	uint8_t ret = 0;

	ret = f_read(file, str, 2000, &br);

	if(ret)
	{
		printf("读取失败\n");
	}
	
	uint8_t flag = 1;
	
	while(br == 2000)
	{
		if(flag)
		{
		SetDmaStatus(0x80);		/*设置dma模式*/
		
		usartSendStart((uint8_t*)str, br);
		
		ret = f_read(file, str1, 2000, &br);
		
		if(ret)
		{
			printf("读取失败\n");
		}
		
		while(GetDmaStatus()&0x80);
		}
		else
		{
		SetDmaStatus(0x80);		/*设置dma模式*/
		
		usartSendStart((uint8_t*)str1, br);
		
		ret = f_read(file, str, 2000, &br);
			
		if(ret)
		{
			printf("读取失败\n");
		}
		
		while(GetDmaStatus()&0x80);
		}
		flag = !flag;
	}
	
	f_close(file);

	sprintf(str, "\"}");

	usartSendStart((uint8_t*)str, 2);
	
	vPortFree(str1);
	
	while(GetDmaStatus()&0x80);
	
	vPortFree(str);

}

/*用于将一段数据打包成合格的数据包结构*/
/*注意length只代表数据长度,不代表整个数据包长度*/
static uint32_t UartDataPacking(char* str, uint8_t type, uint32_t length, uint8_t Check)

{
	UartDataFrontPacking(str, type, length);

	UartDataTailPacking(str, length, 7, Check);
	
	return length + 9;
}
/*数据前部打包*/
static void UartDataFrontPacking(char* str, uint8_t type, uint32_t length)

{

	str[0] = FH_NAME >> 8;			/*写入数据帧头高八位*/

	str[1] = FH_NAME & 0Xff;		/*写入数据帧头低八位*/

	str[2] = DEVICE_ADDR;			/*写入设备地址八位*/

	str[3] = type;/*写入发送数据包类型*/

	str[4] = (length & 0x00ff0000) >> 16;/*写入数据长度高十六位*/

	str[5] = (length & 0x0000ff00) >> 8;/*写入数据长度高八位*/

	str[6] = length & 0x000000ff;		/*写入数据长度低八位*/

}
/*数据尾部打包*/
static void UartDataTailPacking(char* str, uint32_t length, uint8_t Offset, uint8_t Check)

{
	switch(Check)
	{
		case 0: 
		str[length + Offset] = 0x00;			/*不使用校验*/
		break;
		case 1: 
		break;
		case 2: 
		break;
		default:break;
	}
	str[length + Offset + 1] = FT_NAME;		/*写入帧尾*/
}



