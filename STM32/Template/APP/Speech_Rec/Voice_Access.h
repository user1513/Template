#ifndef _VOICE_ACCESS_H
#define _VOICE_ACCESS_H

#include "wavplay.h"
/*ע��:
����ʶ��ֻ����ͨ����Ϊ1
�����ʽΪPCM�²ſ���ʵ��,
PCM�ڲ���������Ϊbase64����������
���������ͨ¼���ļ�
*/
#define voiceNumOfChannels 1/*����¼��ͨ����*/

#define SAVE_TYPE_PCM   0   /*�����ʽΪPCM*/

#define SAVE_TYPE_WAV   1   /*�����ʽΪMAV*/

#define config_ENABLE_MODE SAVE_TYPE_PCM    /*ʹ��ģʽ*/

#define I2S_RX_DMA_BUF_SIZE    	3000		//����RX DMA �����С

void rec_i2s_dma_rx_callback(void);
void recoder_enter_rec_mode(void);
#if config_ENABLE_MODE == SAVE_TYPE_WAV
void recoder_wav_init(__WaveHeader* wavhead);
void recoder_msg_show(u32 tsec,u32 kbps);
void recoder_remindmsg_show(u8 mode);
void recoder_new_pathname(u8 *pname); 
void wav_recorder(void);
#else
u8 voice_Receive(void);
void voice_End_Receive(void);
void voice_info_Send(void);
#endif
#endif

