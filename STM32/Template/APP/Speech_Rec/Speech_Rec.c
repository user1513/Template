#include "Speech_Rec.h"
#include "Voice_Access.h"
#include "stdint.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "FreeRTOS.h"
#include "bsp_dma.h"
#include "exfuns.h" 

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



/*语音识别处理函数*/
void Speech_Handle(uint8_t _ucMode)

{
    static uint8_t status = 0xff;
    switch(_ucMode)
    {
        case 0:status = voice_Receive(); break;
        case 1://if(!status) {status = 0xff; voice_End_Receive(); voice_info_Send();} break;
			if(!status) 
				{
					status = 0xff; 
					voice_End_Receive(); 
					test();	
				}
				break;
        default: break;
    }
}

extern void (*USART_tx_callback)(void);



void test(void)
{
	char* str = (char*)pvPortMalloc(sizeof(char)*400);

	char* str1 = (char*)pvPortMalloc(sizeof(char)*200);

	uint32_t length = 0;

	uint32_t filelength = voice_info_size();
	
	sprintf(str1,SpeechRecStrBody,FORMAT,DEV_PID,TOKEN,CUID, (int)(filelength * 0.75));
	
	length = strlen(str1) + 2;

	strlen(str1);
	
	sprintf(str,SpeechRecStr,POST_NAME,HOST_NAME,length + filelength,str1);
	
	SetDmaStatus(0x81);		/*设置dma模式*/
	
	usartSendStart((uint8_t*)str, strlen(str));
	//printf("strlen: %d\n str:%s",strlen((const char *)str), str);
	
	vPortFree(str1);
	
	while(GetDmaStatus()&0x80);
	
	vPortFree(str);
	
	str = (char*)pvPortMalloc(sizeof(char)*2000);

	str1 = (char*)pvPortMalloc(sizeof(char)*2000);
	
	f_open(file, "Speech_Rec.pcm", FA_READ| FA_OPEN_ALWAYS);
	
	uint32_t br = 0;
	
	uint32_t tlen = 0;
	
	f_read(file, str, 2000, &br);
	
	tlen += br;
	
	uint8_t flag = 1;
	
	while(tlen < filelength)
	{
		if(flag)
		{
		SetDmaStatus(0x80);		/*设置dma模式*/
		
		usartSendStart((uint8_t*)str, strlen(str));
		
		f_read(file, str1, 2000, &br);
		
		while(GetDmaStatus()&0x80);
		}
		else
		{
		SetDmaStatus(0x80);		/*设置dma模式*/
		
		usartSendStart((uint8_t*)str1, strlen(str1));
		
		f_read(file, str, 2000, &br);
		
		while(GetDmaStatus()&0x80);
		}
		flag = !flag;
		tlen += br;
	}
	sprintf(str, "\"}");
	
	usartSendStart((uint8_t*)str, strlen(str));
	
	vPortFree(str1);
	
	while(GetDmaStatus()&0x80);
	
	vPortFree(str);

}

void SpeechRecUartPack()
{
	uint8_t * buf1 = (uint8_t *)malloc(sizeof(4000));
	
	uint8_t * buf2 = buf1 + 2000;
}



