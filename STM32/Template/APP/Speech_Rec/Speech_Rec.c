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

#define SPEECH_REC_TYPE_START 0X00		/*��������ʶ������Ϳ�ʼ*/
#define SPEECH_REC_TYPE_RUNNING 0X01	/*��������ʶ���������*/
#define SPEECH_REC_TYPE_STOP 0X02		/*��������ʶ������ͽ���*/


#define POST_NAME 	"http://vop.baidu.com/server_api" 	/*����ʶ��POST�˿�*/
#define HOST_NAME 	"vop.baidu.com"						/*����ʶ��������*/
#define FORMAT 		"pcm"								/*����ʶ���ļ�����*/
#define DEV_PID		1537								/*dev_pid���뷨ģ��*/

/*token:����ƽ̨��ȡ���Ŀ�����[access_token]��ȡ Access Token "access_token")
һ���»����һ��*/
#define TOKEN		"24.91af5b7738399578bc9c7920f4558d06.2592000.1589172592.282335-19369127"

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

/*���ڽ�һ�����ݴ���ɺϸ�����ݰ��ṹ*/
static uint32_t UartDataPacking(char* str, uint8_t type, uint32_t length);

/*����ʶ��������*/
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
					delay_ms(50);
					SpeechRecUartPack();	
				}
				break;
        default: break;
    }
}




void SpeechRecUartPack(void)
{
	char* str = (char*)pvPortMalloc(sizeof(char)*400);/*����ռ�*/

	char* str1 = (char*)pvPortMalloc(sizeof(char)*200);/*����ռ�*/
	
	printf("��ʼ��������ʶ���\n");

	uint32_t length = 0;

	uint32_t filelength = voice_info_size();/*��ȡbase64���ܺ���ļ���С*/
	
	sprintf(str1,SpeechRecStrBody,FORMAT,DEV_PID,TOKEN,CUID, (int)(filelength * 0.75));
	
	length = strlen(str1);
	
	sprintf(str + 6,SpeechRecStr,POST_NAME,HOST_NAME,length + filelength + 2,str1);
	
	length = UartDataPacking(str, SPEECH_REC_TYPE_START, strlen(str + 6));
	
	SetDmaStatus(0x81);		/*����dmaģʽ*/
	
	usartSendStart((uint8_t*)str, length);/*���ݳ���+���ݰ��ṹ*/
	
	vPortFree(str1);
	
	while(GetDmaStatus()&0x80);
	
	vPortFree(str);
	
	str = (char*)pvPortMalloc(sizeof(char)*2008);

	str1 = (char*)pvPortMalloc(sizeof(char)*2008);

	UartDataPacking(str, SPEECH_REC_TYPE_RUNNING, 2000);

	UartDataPacking(str1, SPEECH_REC_TYPE_RUNNING, 2000);

	f_open(file, "Speech_Rec.pcm", FA_READ| FA_OPEN_ALWAYS);
	
	uint32_t br = 0;
	
	uint8_t ret = 0;

	ret = f_read(file, str + 6, 2000, &br);

	if(ret)
	{
		printf("��ȡʧ��\n");
	}
	
	uint8_t flag = 1;
	
	while(br == 2000)
	{
		if(flag)
		{
		SetDmaStatus(0x80);		/*����dmaģʽ*/
		
		usartSendStart((uint8_t*)str, br + 8);
		
		ret = f_read(file, str1 + 6, 2000, &br);
		
		if(ret)
		{
			printf("��ȡʧ��\n");
		}
		
		while(GetDmaStatus()&0x80);
		}
		else
		{
		SetDmaStatus(0x80);		/*����dmaģʽ*/
		
		usartSendStart((uint8_t*)str1, br + 8);
		
		ret = f_read(file, str + 6, 2000, &br);
			
		if(ret)
		{
			printf("��ȡʧ��\n");
		}
		
		while(GetDmaStatus()&0x80);
		}
		flag = !flag;
	}
	
	f_close(file);

	UartDataPacking(str, SPEECH_REC_TYPE_STOP, 2);

	sprintf(str + 6, "\"}");
	
	usartSendStart((uint8_t*)str, 10);
	
	vPortFree(str1);
	
	while(GetDmaStatus()&0x80);
	
	vPortFree(str);

}

/*���ڽ�һ�����ݴ���ɺϸ�����ݰ��ṹ*/
/*ע��lengthֻ�������ݳ���,�������������ݰ�����*/
static uint32_t UartDataPacking(char* str, uint8_t type, uint32_t length)

{

	str[0] = FH_NAME >> 8;			/*д������֡ͷ�߰�λ*/

	str[1] = FH_NAME & 0Xff;		/*д������֡ͷ�Ͱ�λ*/

	str[2] = DEVICE_ADDR;			/*д���豸��ַ��λ*/

	str[3] = type;/*д�뷢�����ݰ�����*/

	str[4] = (length & 0xff00) >> 8;/*д�����ݳ��ȸ߰�λ*/

	str[5] = length & 0xff;			/*д�����ݳ��ȵͰ�λ*/

	str[length + 6] = 0x00;			/*��ʹ��У��*/

	str[length + 7] = FT_NAME;		/*д��֡β*/

	return length + 8;
}


