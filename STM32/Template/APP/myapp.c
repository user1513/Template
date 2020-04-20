#include "myapp.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "gbk2utf2uni.h" /*UTF-8与GBK互转*/
#include "string.h"

extern QueueHandle_t AudioNoQueueHandle;

/*定义关键词*/
typedef struct
{
	uint8_t No;
	const char * str;
}KeyChar;

/*房间名*/
KeyChar g_MainKeyChar[5]=
{
	{1<<0, "卧室"},
	{1<<1, "客厅"},
	{1<<2, "卫生间"},
	{1<<3, "书房"},
	{1<<4, "厨房"}
};
/*主要电器名*/
KeyChar g_DeviceKeyChar[6]=
{
	{1<<0, "灯"},
	{1<<1, "台灯"},
	{1<<2, "窗帘"},
	{1<<3, "风扇"},
	{1<<4, "电视机"},
	{1<<5, "空调"},
};

/*主要状态名*/
KeyChar g_StateKeyChar[2]=
{
	{1<<0, "开"},
	{1<<1, "关"}
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

	if(_ucpTmp[ i + 4 + length] != 0xa5) return 3/*帧尾数据错误*/

	switch(_ucpTmp[i - 1])//通过类型来找到对应的执行方法
	{
		case 0x03:SpeechRecUartParse(&_ucpTmp[i + 3] ,length); break;		/*语音识别信息*/
		case 0x05:break;		/*ESP联网信息*/
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
	/*由于存在不同的执行方式,所以说话的时候必须说全套
	比如:打开卧室灯,关闭阳台灯(正确)
		 关闭客厅空调(正确)
		 打开空调(错误:缺少关键内容)*/
	/*注意一次性只能识别最多4个关键词*/
	for(uint8_t i = 0; i < sizeof(g_MainKeyChar)/sizeof(g_MainKeyChar[0]); i++)
	{
		if(strstr(cpTmp, g_MainKeyChar[i].str) != NULL)
		{
			
		}
	}
	

	
}


