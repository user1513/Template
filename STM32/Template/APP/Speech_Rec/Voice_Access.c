#include "Voice_Access.h"
#include "audioplay.h"
#include "ff.h"
#include "usart.h"
#include "bsp_wm8978.h"
#include "bsp_i2s.h"
#include "delay.h"
#include "bsp_key.h"
#include "exfuns.h"  
#include "string.h"  
#include "fattester.h"	 
#include "stdlib.h"
#include "base64.h"

u8 *i2srecbuf1;
u8 *i2srecbuf2; 

#if config_ENABLE_MODE == SAVE_TYPE_MAV
#endif
FIL* f_rec=0;		//¼���ļ�	
u32 wavsize;		//wav���ݴ�С(�ֽ���,�������ļ�ͷ!!)
u8 rec_sta=0;		//¼��״̬
					//[7]:0,û�п���¼��;1,�Ѿ�����¼��;
					//[6:1]:����
					//[0]:0,����¼��;1,��ͣ¼��;


//¼�� I2S_DMA�����жϷ�����.���ж�����д������
void rec_i2s_dma_rx_callback(void) 
{    
	u16 bw;
	u8 res;
	#if config_ENABLE_MODE == SAVE_TYPE_PCM
    unsigned int length = 0;
	unsigned char* cpTmp;
	#endif
	if(rec_sta==0X80)//¼��ģʽ
	{  
		if(DMA1_Stream3->CR&(1<<19))
		{	
			#if voiceNumOfChannels == 1
			for(int i=2;i<I2S_RX_DMA_BUF_SIZE/2;i++)
		    {
			 i2srecbuf1[i]=i2srecbuf1[i*2];//i2srecbuf1=i2srecbuf1[i*2];
			 i2srecbuf1[i+1] = i2srecbuf1[i*2+1];//
			  i++;
		     }
                #if config_ENABLE_MODE == SAVE_TYPE_PCM

                cpTmp = base64_array_encoding(I2S_RX_DMA_BUF_SIZE/2, i2srecbuf1, &length);
                
                res=f_write(f_rec,cpTmp,length,(UINT*)&bw);//д���ļ�
            
                free(cpTmp);

                #else
                
                res=f_write(f_rec,i2srecbuf1,I2S_RX_DMA_BUF_SIZE/2,(UINT*)&bw);//д���ļ�
                
                #endif
		    #else

			res=f_write(f_rec,i2srecbuf1,I2S_RX_DMA_BUF_SIZE,(UINT*)&bw);//д���ļ�
		    
            #endif
			if(res)
			{
				printf("write error:%d\r\n",res);
			}
			 
		}else 
		{
			#if voiceNumOfChannels == 1

			for(int i=2;i<I2S_RX_DMA_BUF_SIZE/2;i++)
		    {
			 i2srecbuf2[i]=i2srecbuf2[i*2];//i2srecbuf1=i2srecbuf1[i*2];

			 i2srecbuf2[i+1] = i2srecbuf2[i*2+1];//
			  i++;
		    }
                #if config_ENABLE_MODE == SAVE_TYPE_PCM

                cpTmp = base64_array_encoding(I2S_RX_DMA_BUF_SIZE/2, i2srecbuf2, &length);
                
                res=f_write(f_rec,cpTmp,length,(UINT*)&bw);//д���ļ�
            
                free(cpTmp);

                #else

                res=f_write(f_rec,i2srecbuf2,I2S_RX_DMA_BUF_SIZE/2,(UINT*)&bw);//д���ļ�
            
                #endif
            
            #else
			res=f_write(f_rec,i2srecbuf2,I2S_RX_DMA_BUF_SIZE,(UINT*)&bw);//д���ļ�
		    #endif
			if(res)
			{
				printf("write error:%d\r\n",res);
			}
		}
		#if voiceNumOfChannels == 1
            #if config_ENABLE_MODE == SAVE_TYPE_PCM
                wavsize += length;
            #else
                wavsize += I2S_RX_DMA_BUF_SIZE/2;
            #endif
		#else
		    wavsize+=I2S_RX_DMA_BUF_SIZE;
		#endif
	} 
}  
const u16 i2splaybuf[2]={0X0000,0X0000};//2��16λ����,����¼��ʱI2S Master����.ѭ������0.
//����PCM ¼��ģʽ 		  
void recoder_enter_rec_mode(void)
{
	WM8978_ADDA_Cfg(0,1);		//����ADC
	WM8978_Input_Cfg(1,1,0);	//��������ͨ��(MIC&LINE IN)
	WM8978_Output_Cfg(0,1);		//����BYPASS��� 
	WM8978_MIC_Gain(46);		//MIC�������� 
	
	WM8978_I2S_Cfg(2,0);		//�����ֱ�׼,16λ���ݳ���
	I2S2_Init(I2S_Standard_Phillips,I2S_Mode_MasterTx,I2S_CPOL_Low,I2S_DataFormat_16b);			//�����ֱ�׼,��������,ʱ�ӵ͵�ƽ��Ч,16λ֡���� 
	I2S2ext_Init(I2S_Standard_Phillips,I2S_Mode_SlaveRx,I2S_CPOL_Low,I2S_DataFormat_16b);		//�����ֱ�׼,�ӻ�����,ʱ�ӵ͵�ƽ��Ч,16λ֡����	
	I2S2_SampleRate_Set(16000);	//���ò����� 
 	I2S2_TX_DMA_Init((u8*)&i2splaybuf[0],(u8*)&i2splaybuf[1],1); 		//����TX DMA 
	DMA1_Stream4->CR&=~(1<<4);	//�رմ�������ж�(���ﲻ���ж�������) 
	I2S2ext_RX_DMA_Init(i2srecbuf1,i2srecbuf2,I2S_RX_DMA_BUF_SIZE/2); 	//����RX DMA
  	i2s_rx_callback=rec_i2s_dma_rx_callback;//�ص�����ָwav_i2s_dma_callback
 	I2S_Play_Start();	//��ʼI2S���ݷ���(����)
	I2S_Rec_Start(); 	//��ʼI2S���ݽ���(�ӻ�)
	//recoder_remindmsg_show(0);
}  
//����PCM ����ģʽ 		  
void recoder_enter_play_mode(void)
{
	WM8978_ADDA_Cfg(1,0);		//����DAC 
	WM8978_Input_Cfg(0,0,0);	//�ر�����ͨ��(MIC&LINE IN)
	WM8978_Output_Cfg(1,0);		//����DAC��� 
	WM8978_MIC_Gain(0);			//MIC��������Ϊ0 
	I2S_Play_Stop();			//ֹͣʱ�ӷ���
	I2S_Rec_Stop(); 			//ֹͣ¼��
	//recoder_remindmsg_show(1);
}

#if config_ENABLE_MODE == SAVE_TYPE_WAV
//��ʼ��WAVͷ.
void recoder_wav_init(__WaveHeader* wavhead) //��ʼ��WAVͷ			   
{
	wavhead->riff.ChunkID=0X46464952;	//"RIFF"
	wavhead->riff.ChunkSize=0;			//��δȷ��,�����Ҫ����
	wavhead->riff.Format=0X45564157; 	//"WAVE"
	wavhead->fmt.ChunkID=0X20746D66; 	//"fmt "
	wavhead->fmt.ChunkSize=16; 			//��СΪ16���ֽ�
	wavhead->fmt.AudioFormat=0X01; 		//0X01,��ʾPCM;0X01,��ʾIMA ADPCM
	wavhead->fmt.SampleRate=16000;		//16Khz������ ��������
	#if voiceNumOfChannels == 1
	wavhead->fmt.NumOfChannels=1;		//˫����
	wavhead->fmt.ByteRate=wavhead->fmt.SampleRate*2;//�ֽ�����=������*ͨ����*(ADCλ��/8)
 	wavhead->fmt.BlockAlign=2;			//���С=ͨ����*(ADCλ��/8)
	#else 
	wavhead->fmt.NumOfChannels=2;		//˫����
	wavhead->fmt.ByteRate=wavhead->fmt.SampleRate*4;//�ֽ�����=������*ͨ����*(ADCλ��/8)
 	wavhead->fmt.BlockAlign=4;			//���С=ͨ����*(ADCλ��/8)
	#endif
 	wavhead->fmt.BitsPerSample=16;		//16λPCM
   	wavhead->data.ChunkID=0X61746164;	//"data"
 	wavhead->data.ChunkSize=0;			//���ݴ�С,����Ҫ����  
} 
	
//ͨ��ʱ���ȡ�ļ���
//������SD������,��֧��FLASH DISK����
//��ϳ�:����"0:RECORDER/REC20120321210633.wav"���ļ���
void recoder_new_pathname(u8 *pname)
{	 
	u8 res;					 
	u16 index=0;
	while(index<0XFFFF)
	{
		sprintf((char*)pname,"0:RECORDER/REC%05d.wav",index);
		res=f_open(ftemp,(const TCHAR*)pname,FA_READ);//���Դ�����ļ�
		if(res==FR_NO_FILE)break;		//���ļ���������=����������Ҫ��.
		index++;
	}
} 
#endif

#if config_ENABLE_MODE == SAVE_TYPE_WAV  
//WAV¼�� 
void wav_recorder(void)
{ 
	u8 res;
	u8 key;
	u8 rval=0;

 	DIR recdir;	 					//Ŀ¼  
  	while(f_opendir(&recdir,"0:/RECORDER"))//��¼���ļ���
 	{	 
		delay_ms(200);				       
		delay_ms(200);				  
		f_mkdir("0:/RECORDER");				//������Ŀ¼   
	}   
	i2srecbuf1=malloc(I2S_RX_DMA_BUF_SIZE);//I2S¼���ڴ�1����
	i2srecbuf2=malloc(I2S_RX_DMA_BUF_SIZE);//I2S¼���ڴ�2����  
  	f_rec=(FIL *)malloc(sizeof(FIL));		//����FIL�ֽڵ��ڴ�����  
    __WaveHeader *wavhead=0; 
 	u8 *pname=0;
 	wavhead=(__WaveHeader*)malloc(sizeof(__WaveHeader));//����__WaveHeader�ֽڵ��ڴ����� 
    pname=malloc(30);						//����30���ֽ��ڴ�,����"0:RECORDER/REC00001.wav" 
	pname[0]=0;					//pnameû���κ��ļ���
    if(!i2srecbuf1||!i2srecbuf2||!f_rec)rval=1; 	if(rval==0)		
	{
		recoder_enter_rec_mode();	//����¼��ģʽ,��ʱ��������������ͷ�ɼ�������Ƶ    
 	   	while(rval==0)	
		{
			key=uasrt_key_get();
			switch(key)
			{		
				case KEY2_PRES:	//STOP&SAVE
					if(rec_sta&0X80)//��¼��
					{
						rec_sta=0;	//�ر�¼��
                            wavhead->riff.ChunkSize=wavsize+36;		//�����ļ��Ĵ�С-8;
                            wavhead->data.ChunkSize=wavsize;		//���ݴ�С
                            f_lseek(f_rec,0);						//ƫ�Ƶ��ļ�ͷ.
                            f_write(f_rec,(const void*)wavhead,sizeof(__WaveHeader),&bw);//д��ͷ����
                        f_close(f_rec);
						wavsize=0;
					}
					rec_sta=0;
						break;	 
				case KEY0_PRES:	//REC/PAUSE
						recoder_new_pathname(pname);			//�õ��µ�����
				 		recoder_wav_init(wavhead);				//��ʼ��wav����	
	 					res=f_open(f_rec,(const TCHAR*)pname, FA_CREATE_ALWAYS | FA_WRITE); 
                        if(res)			//�ļ�����ʧ��
						{
							rec_sta=0;	//�����ļ�ʧ��,����¼��
							rval=0XFE;	//��ʾ�Ƿ����SD��
						}else 
						{
							res=f_write(f_rec,(const void*)wavhead,sizeof(__WaveHeader),&bw);//д��ͷ����
 							rec_sta|=0X80;	//��ʼ¼��	 
						} 
					break;  
				case WKUP_PRES:	//�������һ��¼��
					if(rec_sta!=0X80)//û����¼��
					{   	 		 				  
						if(pname[0])//�����������������,��pname��Ϊ��
						{				 
							recoder_enter_play_mode();	//���벥��ģʽ
							audio_play_song(pname);		//����pname
							recoder_enter_rec_mode();	//���½���¼��ģʽ 
						}
					}
					break;
			case 5:	//��ӡ�ļ�����
					mf_open(pname, FA_READ| FA_OPEN_ALWAYS);
			        printf("filesize%d\n", mf_size());
					mf_read_type("%02X ",mf_size());
					printf("filesize%d\n", mf_size());
					mf_read_type("%c",mf_size());
					break;
			} 
			delay_ms(5);

		}		 
	}    
	free(i2srecbuf1);	//�ͷ��ڴ�
	free(i2srecbuf2);	//�ͷ��ڴ�  
	free(f_rec);		//�ͷ��ڴ�
	free(wavhead);		//�ͷ��ڴ�  
	free(pname);		//�ͷ��ڴ� 
}

#else

//WAV¼�� 
u8 voice_Receive(void)
{ 

	u8 res;
	u8 rval=0;

	i2srecbuf1=malloc(I2S_RX_DMA_BUF_SIZE);//I2S¼���ڴ�1����
	i2srecbuf2=malloc(I2S_RX_DMA_BUF_SIZE);//I2S¼���ڴ�2����  
  	f_rec=(FIL *)malloc(sizeof(FIL));		//����FIL�ֽڵ��ڴ�����  
    if(!i2srecbuf1||!i2srecbuf2||!f_rec)rval=1; 	
    if(rval==0)		
	{
		recoder_enter_rec_mode();	//����¼��ģʽ,��ʱ��������������ͷ�ɼ�������Ƶ    

        res=f_open(f_rec,(const TCHAR*)"Speech_Rec.pcm", FA_CREATE_ALWAYS | FA_WRITE);
  
        if(res)			//�ļ�����ʧ��
        {
            rec_sta=0;	//�����ļ�ʧ��,����¼��
            rval=0XFE;	//��ʾ�Ƿ����SD��
        }else 
        {
            #if config_ENABLE_MODE == SAVE_TYPE_WAV 
            res=f_write(f_rec,(const void*)wavhead,sizeof(__WaveHeader),&bw);//д��ͷ����
            #endif
            rec_sta|=0X80;	//��ʼ¼��	 
        } 
				 
	}
    else
    {
        free(i2srecbuf1);	//�ͷ��ڴ�
        free(i2srecbuf2);	//�ͷ��ڴ�  
        free(f_rec);		//�ͷ��ڴ�
    }
    return rval;
}

void voice_End_Receive(void)
{
    if(rec_sta&0X80)//��¼��
    {
        rec_sta=0;	//�ر�¼��
        f_close(f_rec);
        wavsize=0;
    }

    recoder_enter_play_mode();	//�رս���DMA		
	free(i2srecbuf1);	        //�ͷ��ڴ�
	free(i2srecbuf2);	        //�ͷ��ڴ�  
	free(f_rec);		        //�ͷ��ڴ�
}

void voice_info_Send(void)
{
    mf_open("Speech_Rec.pcm", FA_READ| FA_OPEN_ALWAYS);
    printf("filesize%d\n", mf_size());
    mf_read_type("%c",mf_size());
}

uint32_t voice_info_size(void)
{
	uint32_t size = 0;
	
	mf_open("Speech_Rec.pcm", FA_READ| FA_OPEN_ALWAYS);
	
	size = mf_size();
	
	mf_close();
	
	return size;
}

#endif

