#include "Speech_Rec.h"
#include "Voice_Access.h"
#include "stdint.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "FreeRTOS.h"
#include "bsp_dma.h"
#include "exfuns.h" 

#define FH_NAME 0X55AA  /*Frame Header֡ͷ*/
#define FT_NAME 0XA5  	/*Frame Tail֡ͷ*/
#define DEVICE_ADDR 0X10/*�豸��ַ*/

#define SPEECH_REC_TYPE_START		0/*��TCP����,�ȴ�����ʶ�������*/
#define SPEECH_REC_TYPE_END			2/*�ر�TCP����*/

#define POST_NAME 	"http://vop.baidu.com/server_api" 	/*����ʶ��POST�˿�*/
#define HOST_NAME 	"vop.baidu.com"						/*����ʶ��������*/
#define FORMAT 		"pcm"								/*����ʶ���ļ�����*/
#define DEV_PID		1537								/*dev_pid���뷨ģ��*/

/*token:����ƽ̨��ȡ���Ŀ�����[access_token]��ȡ Access Token "access_token")
һ���»����һ��*/
#define TOKEN		"25.32445ba57e0f796aa7db9ff78f2d888a.315360000.1903679091.282335-19369127"

/*�û�Ψһ��ʶ�����������û�������UVֵ��������д�������û��Ļ��� MAC ��ַ�� IMEI �룬����Ϊ60�ַ����ڡ�*/
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

/*����ǰ�����*/
static void UartDataFrontPacking(char* str, uint8_t type, uint32_t length);
/*����β�����*/
static void UartDataTailPacking(char* str, uint32_t length, uint8_t Offset, uint8_t Check);

char g_Str[20];

/*����ʶ������*/
void Speech_Handle(uint8_t _ucMode)

{
    static uint8_t status = 0xff;
    switch(_ucMode)
    {
        case 0:
		UartDataPacking(g_Str,SPEECH_REC_TYPE_START,0,0);	
		usartSendStart((uint8_t*)g_Str, 9);/*���ݳ���+���ݰ��ṹ*/
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
	int	file_count;

	restart_send:	
	
	file_count = 0;
	
	char* str = (char*)pvPortMalloc(sizeof(char)*400);/*����ռ�*/

	char* str1 = (char*)pvPortMalloc(sizeof(char)*200);/*����ռ�*/
	
	printf("��ʼ��������ʶ���\n");

	uint32_t length = 0;

	uint32_t  filelength = voice_info_size();/*��ȡbase64���ܺ���ļ���С*/
	
	sprintf(str1,SpeechRecStrBody,FORMAT,DEV_PID,TOKEN,CUID, (int)(filelength * 0.75));
	
	length = strlen(str1);
	
	sprintf(str,SpeechRecStr,POST_NAME,HOST_NAME,length + filelength + 2,str1);
	
	SetDmaStatus(0x80);		/*����dmaģʽ*/
	
	length = strlen(str);
	
	printf("total data: %d\n",length + filelength + 2);
		
	usartSendStart((uint8_t*)str, length);/*���ݳ���+���ݰ��ṹ*/
	
	vPortFree(str1);
	
	while(GetDmaStatus()&0x80);
	
	vPortFree(str);
	
	vTaskDelay(100);	
	
	str = (char*)pvPortMalloc(sizeof(char)*2000);

	str1 = (char*)pvPortMalloc(sizeof(char)*2000);

	f_open(file, "Speech_Rec.pcm", FA_READ| FA_OPEN_ALWAYS);
	
	uint32_t br = 0;
	
	uint8_t ret = 0;

	ret = f_read(file, str, 2000, &br);
	
	file_count += br;
	
	if(ret)
	{
		printf("��ȡʧ��\n");
	}
	
	uint8_t flag = 1;
	
	while(br == 2000 || file_count < filelength)
	{
		if(flag)
		{
			SetDmaStatus(0x80);		/*����dmaģʽ*/
			
			usartSendStart((uint8_t*)str, br);
			
			ret = f_read(file, str1, 2000, &br);
			
			if(ret)
			{
				printf("��ȡʧ��\n");
			}
		
		}
		else
		{
			SetDmaStatus(0x80);		/*����dmaģʽ*/
			
			usartSendStart((uint8_t*)str1, br);
			
			ret = f_read(file, str, 2000, &br);
				
			if(ret)
			{
				printf("��ȡʧ��\n");
			}
		}
		while(GetDmaStatus()&0x80);
		flag = !flag;
		file_count += br;
	}
	
	f_close(file);
	
	if(file_count < filelength)
	{
		UartDataPacking(g_Str,SPEECH_REC_TYPE_END,0,0);	
		usartSendStart((uint8_t*)g_Str, 9);/*���ݳ���+���ݰ��ṹ*/
		vPortFree(str1);
		vPortFree(str);
		printf("pcm���ݶ�ȡС��ʵ��ֵ\n");
		vTaskDelay(500);
		UartDataPacking(g_Str,SPEECH_REC_TYPE_START,0,0);	
		usartSendStart((uint8_t*)g_Str, 9);/*���ݳ���+���ݰ��ṹ*/
		goto restart_send;
	}
	sprintf(str, "\"}");

	usartSendStart((uint8_t*)str, 2);
	
	vPortFree(str1);
	
	while(GetDmaStatus()&0x80);
	
	vPortFree(str);

}

/*���ڽ�һ�����ݴ���ɺϸ�����ݰ��ṹ*/
/*ע��lengthֻ�������ݳ���,�������������ݰ�����*/
uint32_t UartDataPacking(char* str, uint8_t type, uint32_t length, uint8_t Check)

{
	UartDataFrontPacking(str, type, length);

	UartDataTailPacking(str, length, 7, Check);
	
	return length + 9;
}
/*����ǰ�����*/
static void UartDataFrontPacking(char* str, uint8_t type, uint32_t length)

{

	str[0] = FH_NAME >> 8;			/*д������֡ͷ�߰�λ*/

	str[1] = FH_NAME & 0Xff;		/*д������֡ͷ�Ͱ�λ*/

	str[2] = DEVICE_ADDR;			/*д���豸��ַ��λ*/

	str[3] = type;/*д�뷢�����ݰ�����*/

	str[4] = (length & 0x00ff0000) >> 16;/*д�����ݳ��ȸ�ʮ��λ*/

	str[5] = (length & 0x0000ff00) >> 8;/*д�����ݳ��ȸ߰�λ*/

	str[6] = length & 0x000000ff;		/*д�����ݳ��ȵͰ�λ*/

}
/*����β�����*/
static void UartDataTailPacking(char* str, uint32_t length, uint8_t Offset, uint8_t Check)

{
	switch(Check)
	{
		case 0: 
		str[length + Offset] = 0x00;			/*��ʹ��У��*/
		break;
		case 1: 
		break;
		case 2: 
		break;
		default:break;
	}
	str[length + Offset + 1] = FT_NAME;		/*д��֡β*/
}



