#include "myapp.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "gbk2utf2uni.h" /*UTF-8��GBK��ת*/
#include "string.h"

extern QueueHandle_t AudioNoQueueHandle;

/*����ؼ���*/
typedef struct
{
	uint8_t No;
	const char * str;
}KeyChar;

/*������*/
KeyChar g_MainKeyChar[5]=
{
	{1<<0, "����"},
	{1<<1, "����"},
	{1<<2, "������"},
	{1<<3, "�鷿"},
	{1<<4, "����"}
};
/*��Ҫ������*/
KeyChar g_DeviceKeyChar[6]=
{
	{1<<0, "��"},
	{1<<1, "̨��"},
	{1<<2, "����"},
	{1<<3, "����"},
	{1<<4, "���ӻ�"},
	{1<<5, "�յ�"},
};

/*��Ҫ״̬��*/
KeyChar g_StateKeyChar[2]=
{
	{1<<0, "��"},
	{1<<1, "��"}
};


uint8_t usart_parse(uint8_t * _ucpTmp)
{
	uint8_t i = 0;
	while(_ucpTmp[i] != 0x55 && _ucpTmp[i+1] != 0xaa)
	{
		i++;
		if(i>=99)
			break;
	}
	if(i >= 99) return 1; 	/*û�м�⵽֡ͷ*/
	
	i += 4;					/*����֡ͷ��Դ��ַ������*/
	
	uint32_t length = (_ucpTmp[i] << 16) + (_ucpTmp[i + 1] << 8) + _ucpTmp[i + 2];
	
	if(length > 100) return 2; 	/*������֡��ȡ��Χ*/

	if(_ucpTmp[ i + 4 + length] != 0xa5) return 3/*֡β���ݴ���*/

	switch(_ucpTmp[i - 1])//ͨ���������ҵ���Ӧ��ִ�з���
	{
		case 0x03:SpeechRecUartParse(&_ucpTmp[i + 3] ,length); break;		/*����ʶ����Ϣ*/
		case 0x05:break;		/*ESP������Ϣ*/
	}
}

void SpeechRecUartParse(uint8_t* ucpTmp ,uint8_t length)

{
	if(!length) 
	{
		xQueueSend(AudioNoQueueHandle, (void *) 1, portMAX_DELAY);
		return;
	}
	char cpTmp = pvPortMalloc(sizeof(char) * 100);

	utf82gbk(cpTmp, ucpTmp);
	/*���ڴ��ڲ�ͬ��ִ�з�ʽ,����˵����ʱ�����˵ȫ��
	����:�����ҵ�,�ر���̨��(��ȷ)
		 �رտ����յ�(��ȷ)
		 �򿪿յ�(����:ȱ�ٹؼ�����)*/
	/*ע��һ����ֻ��ʶ�����4���ؼ���*/
	for(uint8_t i = 0; i < sizeof(g_MainKeyChar)/sizeof(g_MainKeyChar[0]); i++)
	{
		if(strstr(cpTmp, g_MainKeyChar[i].str) != NULL)
		{
			
		}
	}
	

	
}


