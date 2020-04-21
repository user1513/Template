#include "myapp.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "gbk2utf2uni.h" /*UTF-8��GBK��ת*/
#include "string.h"
#include "Speech_Rec.h"
#include "bsp_dma.h"
extern QueueHandle_t AudioNoQueueHandle;

#define config_MAX_REC_NUM 4	/*�������ʶ��Ĺؼ��ʸ���*/

/*���ճ��ֹؼ��ʵ��Ⱥ��������*/
static uint8_t keychartimesort(uint8_t* ucpTmp);
static void SpeechRecUartParse(uint8_t* ucpTmp, uint8_t length);
/*����ؼ���*/
typedef struct
{
	uint8_t No;
	const char * str;
}KeyChar;

/*������*/
KeyChar g_MainKeyChar[]=
{
	{1, "����"},
	{2, "����"},
	{3, "������"},
	{4, "�鷿"},
	{5, "����"}
};
/*��Ҫ������*/
KeyChar g_DeviceKeyChar[]=
{
	{1, "��"},
	{2, "̨��"},
	{3, "����"},
	{4, "����"},
	{5, "���ӻ�"},
	{6, "�յ�"},
};

/*��Ҫ״̬��*/
KeyChar g_StatueKeyChar[]=
{
	{1, "��"},
	{2, "��"}
};

static uint8_t LastEspStatue = 0xff;/*����һ���ϴ�esp��Ϣ*/
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

	if(_ucpTmp[ i + 4 + length] != 0xa5) return 3;/*֡β���ݴ���*/
	int queuetmp = 1;
	switch(_ucpTmp[i - 1])//ͨ���������ҵ���Ӧ��ִ�з���
	{
		case 0x03:
		if(length)
		{
			SpeechRecUartParse(&_ucpTmp[i + 3] ,length);/*����ʶ����Ϣ*/
		}
		else
		{
			queuetmp = 3;
			xQueueSend(AudioNoQueueHandle, (void*)&queuetmp, portMAX_DELAY);
		}			
		break;		
		case 0x05:
		if(_ucpTmp[i + 3] != LastEspStatue)
		{
			if(_ucpTmp[i + 3])/*ESP������Ϣ*/
			{
				xQueueSend(AudioNoQueueHandle, (void*)&queuetmp, portMAX_DELAY);
			}
			else
			{
				queuetmp = 2;
				xQueueSend(AudioNoQueueHandle, (void*)&queuetmp, portMAX_DELAY);
			}
			LastEspStatue = _ucpTmp[i + 3];/*����״ֵ̬*/
		}
		break;		
	}
	return 0;
}

/*����ʶ�𴮿����ݽ���*/
static void SpeechRecUartParse(uint8_t* ucpTmp, uint8_t length)

{
	uint32_t queueval = 0;
	if (!length)
	{
		queueval = 3;
		xQueueSend(AudioNoQueueHandle, (void*)&queueval, portMAX_DELAY);
		return;
	}
	char* cpTmp = (char *)pvPortMalloc(sizeof(char) * 100);
	//char cpTmp[100];

	char cpTmpSort1[2][config_MAX_REC_NUM] = {0};
	char cpTmpSort2[2][config_MAX_REC_NUM] = {0};
	char cpTmpSort3[2][config_MAX_REC_NUM] = {0};

	utf82gbk((unsigned char *)cpTmp, ucpTmp);

	printf("\ngbk:%s\n", cpTmp);/*��ӡutf8ת����gbk*/
	
	length = strlen(cpTmp);

	char* tmp = NULL;
	/*���ڴ��ڲ�ͬ��ִ�з�ʽ,����˵����ʱ�����˵ȫ��
	����:�����ҵ�,�ر���̨��(��ȷ)
		 �رտ����յ�(��ȷ)
		 �򿪿յ�(����:ȱ�ٹؼ�����)*/
		 /*ע��һ����ֻ��ʶ�����4���ؼ���*/
	for (uint8_t i = 0, j = 0; i < sizeof(g_MainKeyChar) / sizeof(g_MainKeyChar[0]); i++)
	{
		tmp = cpTmp;
		while((tmp = strstr(tmp, g_MainKeyChar[i].str)) != NULL)
		{
			cpTmpSort1[0][j] = tmp - cpTmp;

			cpTmpSort1[1][j] = g_MainKeyChar[i].No;

			if (cpTmpSort1[0][j] + strlen(g_MainKeyChar[i].str) > (length - 1))
			{
				break;
			}
			else
			{
				tmp += strlen(g_MainKeyChar[i].str);
			}

			j++;
		}
	}
	/*���չؼ��ʳ����Ⱥ�����*/
	cpTmpSort1[0][0] = keychartimesort((uint8_t*)cpTmpSort1[0]);
	/*����豸��*/
	for (uint8_t i = 0, j = 0; i < sizeof(g_DeviceKeyChar) / sizeof(g_DeviceKeyChar[0]); i++)
	{
		tmp = cpTmp;
		
		while ((tmp = strstr(tmp, g_DeviceKeyChar[i].str)) != NULL)
		{
			if(!strcmp(g_DeviceKeyChar[i].str,"��"))
			{
				char str[] = "̨��";
				if(strstr(tmp - 2,str) == (tmp - 2))
				{
					if (cpTmpSort2[0][j] + strlen(g_DeviceKeyChar[i].str) > (length - 1))
					{
						break;
					}
					else
					{
						tmp += strlen(g_DeviceKeyChar[i].str);
					}
					continue;
				}
			}
			cpTmpSort2[0][j] = tmp - cpTmp;
			
			cpTmpSort2[1][j] = g_DeviceKeyChar[i].No;
			
			if (cpTmpSort2[0][j] + strlen(g_DeviceKeyChar[i].str) > (length - 1))
			{
				break;
			}
			else
			{
				tmp += strlen(g_DeviceKeyChar[i].str);
			}
			j++;

		}
	}
	/*���չؼ��ʳ����Ⱥ�����*/
	cpTmpSort2[0][0] = keychartimesort((uint8_t*)cpTmpSort2[0]);
	/*���״̬��*/
	for (uint8_t i = 0, j = 0; i < sizeof(g_StatueKeyChar) / sizeof(g_StatueKeyChar[0]); i++)
	{
		tmp = cpTmp;
		while ((tmp = strstr(tmp, g_StatueKeyChar[i].str)) != NULL)
		{
			cpTmpSort3[0][j] = tmp - cpTmp;

			cpTmpSort3[1][j] = g_StatueKeyChar[i].No;

			if (cpTmpSort3[0][j] + strlen(g_StatueKeyChar[i].str) > (length - 1))
			{
				break;
			}
			else
			{
				tmp += strlen(g_StatueKeyChar[i].str);
			}
			j++;
		}
	}

	/*���չؼ��ʳ����Ⱥ�����*/
	cpTmpSort3[0][0] = keychartimesort((uint8_t*)cpTmpSort3[0]);

	if (cpTmpSort3[0][0] != cpTmpSort2[0][0] || cpTmpSort3[0][0] != cpTmpSort1[0][0] || cpTmpSort1[0][0] == 0 || cpTmpSort3[0][0] == 0 || cpTmpSort2[0][0] == 0)
	{
		queueval = 3;
		xQueueSend(AudioNoQueueHandle, (void*)&queueval, portMAX_DELAY);
	}
	else
	{
		uint8_t i = 0;
		while (cpTmpSort3[0][0]--)
		{
			queueval = (cpTmpSort1[1][i] * 1000 + cpTmpSort2[1][i] * 10 + cpTmpSort3[1][i]);
			xQueueSend(AudioNoQueueHandle, (void*)&queueval, portMAX_DELAY);
			i++;
		}
	}

	/*
	code
	����������дoled��ʾ
	*/

	vPortFree(cpTmp);

}
/*���ճ��ֹؼ��ʵ��Ⱥ��������*/
static uint8_t keychartimesort(uint8_t* ucpTmp)
{
	uint8_t* oneline = ucpTmp;
	uint8_t* twoline = ucpTmp + config_MAX_REC_NUM;
	uint8_t ucTmp = 0;
	uint8_t num = 0;
	for (int i = 0; i < config_MAX_REC_NUM; i++)
	{
		if (twoline[i] == 0) break;
		num++;
	}

	for (int i = 0; i < num; i++)
	{
		for (int j = 1 + i; j < num; j++)
		{
			if (oneline[i] > oneline[j])
			{
				ucTmp = oneline[j];			/*Ѱ�����ȳ��ֵĹؼ���*/

				oneline[j] = oneline[i];

				oneline[i] = ucTmp;

				ucTmp = twoline[j];

				twoline[j] = twoline[i];

				twoline[i] = ucTmp;
			}
		}
	}
	return num;
}

char cTmp[8] = {0};

void GetEspInfo(void)
{

	UartDataPacking(cTmp, 0x04, 0, 0);/*����Ϊ4,���ݳ���0,��У��*/
	
	usartSendStart((uint8_t *)cTmp, 9);
}




 

