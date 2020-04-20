#include "myapp.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "gbk2utf2uni.h" /*UTF-8与GBK互转*/
#include "string.h"
extern QueueHandle_t AudioNoQueueHandle;

#define config_MAX_REC_NUM 4	/*定义可以识别的关键词个数*/

/*按照出现关键词的先后进行排序*/
static uint8_t keychartimesort(uint8_t* ucpTmp);
static void SpeechRecUartParse(uint8_t* ucpTmp, uint8_t length);
/*定义关键词*/
typedef struct
{
	uint8_t No;
	const char * str;
}KeyChar;

/*房间名*/
KeyChar g_MainKeyChar[]=
{
	{1, "卧室"},
	{2, "客厅"},
	{3, "卫生间"},
	{4, "书房"},
	{5, "厨房"}
};
/*主要电器名*/
KeyChar g_DeviceKeyChar[]=
{
	{1, "灯"},
	{2, "台灯"},
	{3, "窗帘"},
	{4, "风扇"},
	{5, "电视机"},
	{6, "空调"},
};

/*主要状态名*/
KeyChar g_StatueKeyChar[]=
{
	{1, "开"},
	{2, "关"}
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
	if(i >= 99) return 1; 	/*没有检测到帧头*/
	
	i += 4;					/*跳过帧头和源地址和类型*/
	
	uint32_t length = (_ucpTmp[i] << 16) + (_ucpTmp[i + 1] << 8) + _ucpTmp[i + 2];
	
	if(length > 100) return 2; 	/*超出单帧读取范围*/

	if(_ucpTmp[ i + 4 + length] != 0xa5) return 3;/*帧尾数据错误*/

	switch(_ucpTmp[i - 1])//通过类型来找到对应的执行方法
	{
		case 0x03:SpeechRecUartParse(&_ucpTmp[i + 3] ,length); break;		/*语音识别信息*/
		case 0x05:break;		/*ESP联网信息*/
	}
	return 0;
}

static void SpeechRecUartParse(uint8_t* ucpTmp, uint8_t length)

{
	if (!length)
	{
		//xQueueSend(AudioNoQueueHandle, (void*)1, portMAX_DELAY);
		return;
	}
	char* cpTmp = (char *)pvPortMalloc(sizeof(char) * 100);
	//char cpTmp[100];

	char cpTmpSort1[2][config_MAX_REC_NUM] = {0};
	char cpTmpSort2[2][config_MAX_REC_NUM] = {0};
	char cpTmpSort3[2][config_MAX_REC_NUM] = {0};

	utf82gbk((unsigned char *)cpTmp, ucpTmp);

	length = strlen(cpTmp);

	char* tmp = NULL;
	/*由于存在不同的执行方式,所以说话的时候必须说全套
	比如:打开卧室灯,关闭阳台灯(正确)
		 关闭客厅空调(正确)
		 打开空调(错误:缺少关键内容)*/
		 /*注意一次性只能识别最多4个关键词*/
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
	/*按照关键词出现先后排序*/
	cpTmpSort1[0][0] = keychartimesort((uint8_t*)cpTmpSort1[0]);
	/*配对设备名*/
	for (uint8_t i = 0, j = 0; i < sizeof(g_DeviceKeyChar) / sizeof(g_DeviceKeyChar[0]); i++)
	{
		tmp = cpTmp;
		while ((tmp = strstr(tmp, g_DeviceKeyChar[i].str)) != NULL)
		{
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
	/*按照关键词出现先后排序*/
	cpTmpSort2[0][0] = keychartimesort((uint8_t*)cpTmpSort2[0]);
	/*配对状态名*/
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

	/*按照关键词出现先后排序*/
	cpTmpSort3[0][0] = keychartimesort((uint8_t*)cpTmpSort3[0]);

	if (cpTmpSort3[0][0] != cpTmpSort2[0][0] && cpTmpSort3[0][0] != cpTmpSort1[0][0])
	{
		//xQueueSend(AudioNoQueueHandle, (void*)0001, portMAX_DELAY);
	}
	else
	{
		while (cpTmpSort3[0][0]--)
		{
			xQueueSend(AudioNoQueueHandle, (void*)(cpTmpSort1[1][0] * 1000 +
													cpTmpSort2[1][0] * 10 +
													cpTmpSort3[1][0]), 
													portMAX_DELAY);
		}
	}

	/*
	code
	可以在这里写oled显示
	*/

	vPortFree(cpTmp);

}
/*按照出现关键词的先后进行排序*/
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
				ucTmp = oneline[j];			/*寻找最先出现的关键词*/

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


 








 
