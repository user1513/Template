#include "fattester.h"	 
#include "bsp_sdio_sdcard.h"
#include "usmart.h"
#include "usart.h"
#include "exfuns.h"  
#include "ff.h"
#include "string.h"

   
//Ϊ����ע�Ṥ����	 
//Ĭ��fs[0]
//_path:����·��������"0:"��"1:"
//_mt:0��������ע�ᣨ�Ժ�ע�ᣩ��1������ע��
//����ֵ:ִ�н��
u8 mf_mount(u8* path,u8 mt)
{	  
	return f_mount(fs[0],(const TCHAR*)path,mt); 
}
//��·���µ��ļ�
//path:·��+�ļ���
//mode:��ģʽ
//����ֵ:ִ�н��
u8 mf_open(u8* path,u8 mode)
{
	u8 res;	 
	res=f_open(file,(const TCHAR*)path,mode);//���ļ���
	return res;
} 
//�ر��ļ�
//����ֵ:ִ�н��
u8 mf_close(void)
{
	f_close(file);
	return 0;
}
//��������
//len:�����ĳ���
//����ֵ:ִ�н��
u8 mf_read(u16 len)
{
	u16 i,t;
	u8 res=0;
	u16 tlen=0;
	printf("\r\nRead file data is:\r\n");
	for(i=0;i<len/512;i++)
	{
		res=f_read(file,fatbuf,512,&br);
		if(res)
		{
			printf("Read Error:%d\r\n",res);
			break;
		}else
		{
			tlen+=br;
			for(t=0;t<br;t++)printf("%c",fatbuf[t]); 
		}
	}
	if(len%512)
	{
		res=f_read(file,fatbuf,len%512,&br);
		if(res)	//�����ݳ�����
		{
			printf("\r\nRead Error:%d\r\n",res);   
		}else
		{
			tlen+=br;
			for(t=0;t<br;t++)printf("%c",fatbuf[t]); 
		}	 
	}
	if(tlen)printf("\r\nReaded data len:%d\r\n",tlen);//���������ݳ���
	printf("Read data over\r\n");	 
	return res;
}

//д������
//dat:���ݻ�����
//len:д�볤��
//����ֵ:ִ�н��
u8 mf_write(u8*dat, u16 len)
{			    
	uint8_t res;	   					   

	printf("\r\nBegin Write file...\r\n");
	printf("Write data len:%d\r\n",len);	 
	res=f_write(file,dat,len,&bw);
	if(res)
	{
		printf("Write Error:%d\r\n",res);   
	}else printf("Writed data len:%d\r\n",bw);
	printf("Write data over.\r\n");
	return res;
}

//��Ŀ¼
//path:·��
//����ֵ:ִ�н��
u8 mf_opendir(u8* path)
{
	return f_opendir(&dir,(const TCHAR*)path);	
}
//�ر�Ŀ¼ 
//����ֵ:ִ�н��
u8 mf_closedir(void)
{
	return f_closedir(&dir);	
}
//��ȡ�ļ�����Ϣ
//����ֵ:ִ�н��
u8 mf_readdir(void)
{
	u8 res;
	FILINFO fileinfo;	//�ļ���Ϣ		 
	res=f_readdir(&dir,&fileinfo);//��ȡһ���ļ�����Ϣ
	if(res!=FR_OK||fileinfo.fname[0]==0)
	{
		return res;//������.
	}

	printf("\r\n");
	printf("File Name is:%s\r\n",fileinfo.fname);
	printf("File Size is:%d\r\n",fileinfo.fsize);
	printf("File data is:%d\r\n",fileinfo.fdate);
	printf("File time is:%d\r\n",fileinfo.ftime);
	printf("File Attr is:%d\r\n",fileinfo.fattrib);
	printf("\r\n");
	return 0;
}		

 /* Start node to be scanned (***also used as work area***) */
 /*
 depth = 0ֻɨ��path·����ǰĿ¼
 depth = 1ɨ��path·���������ļ�
 */
u8 scan_files (char* path,u8 depth)
	
{
    u8 res;
	
    UINT i;
	
    static FILINFO fileinfo;

    res = f_opendir(&dir, path);                       		/* Open the directory */
	
    if (res == FR_OK) 
	{
        for (;;) 
		{
            res = f_readdir(&dir, &fileinfo);                   	/* Read a directory item */
			
            if (res != FR_OK || fileinfo.fname[0] == 0) break;  	/* Break on error or end of dir */
			
            if (fileinfo.fattrib & AM_DIR && depth) 				/* It is a directory */
			{                    
                i = strlen(path);
				
                sprintf(&path[i], "/%s", fileinfo.fname);
				
                res = scan_files(path, 1);                    	/* Enter the directory */
				
                if (res != FR_OK) break;
				
                path[i] = 0;
				
            }
			else                                        	/* It is a file. */
			{
                printf("%s/%s\r\n", path, fileinfo.fname);
            }
        }
        f_closedir(&dir);
    }
    return res;
}

//��ʾʣ������
//drv:�̷�
//����ֵ:ʣ������(�ֽ�)
u32 mf_showfree(u8 *drv)
{
	FATFS *fs1;
	u8 res;
    u32 fre_clust=0, fre_sect=0, tot_sect=0;
    //�õ�������Ϣ�����д�����
    res = f_getfree((const TCHAR*)drv,(DWORD*)&fre_clust, &fs1);
    if(res==0)
	{											   
	    tot_sect = (fs1->n_fatent - 2) * fs1->csize;//�õ���������
	    fre_sect = fre_clust * fs1->csize;			//�õ�����������	   
#if FF_MAX_SS!=512
		tot_sect*=fs1->ssize/512;
		fre_sect*=fs1->ssize/512;
#endif	  
		if(tot_sect<20480)//������С��10M
		{
		    /* Print free space in unit of KB (assuming 512 bytes/sector) */
		    printf("\r\n����������:%d KB\r\n"
		           "���ÿռ�:%d KB\r\n",
		           tot_sect>>1,fre_sect>>1);
		}else
		{
		    /* Print free space in unit of KB (assuming 512 bytes/sector) */
		    printf("\r\n����������:%d MB\r\n"
		           "���ÿռ�:%d MB\r\n",
		           tot_sect>>11,fre_sect>>11);
		}
	}
	return fre_sect;
}		    
//�ļ���дָ��ƫ��
//offset:����׵�ַ��ƫ����
//����ֵ:ִ�н��.
u8 mf_lseek(u32 offset)
{
	return f_lseek(file,offset);
}
//��ȡ�ļ���ǰ��дָ���λ��.
//����ֵ:λ��
u32 mf_tell(void)
{
	return f_tell(file);
}
//��ȡ�ļ���С
//����ֵ:�ļ���С
u32 mf_size(void)
{
	return f_size(file);
} 
//����Ŀ¼
//pname:Ŀ¼·��+����
//����ֵ:ִ�н��
u8 mf_mkdir(u8*pname)
{
	return f_mkdir((const TCHAR *)pname);
}
//��ʽ��
//path:����·��������"0:"��"1:"
//au:�ش�С
//����ֵ:ִ�н��
u8 mf_fmkfs(u8* path,u16 au)
{
	//��������˿�ָ�룬����Ϊ�����ṩĬ��ֵ������ѡ��
	return f_mkfs((const TCHAR*)path, 0, NULL, au);//��ʽ��,drv:�̷�;mode:ģʽ;au:�ش�С
} 
//ɾ���ļ�/Ŀ¼
//pname:�ļ�/Ŀ¼·��+����
//����ֵ:ִ�н��
u8 mf_unlink(u8 *pname)
{
	return  f_unlink((const TCHAR *)pname);
}

//�޸��ļ�/Ŀ¼����(���Ŀ¼��ͬ,�������ƶ��ļ�Ŷ!)
//oldname:֮ǰ������
//newname:������
//����ֵ:ִ�н��

/*example
/ *������Ĭ���������еĶ���* / 
f_rename���� oldname.txt������ newname.txt������

/ *������������2�еĶ���* / 
f_rename���� 2��oldname.txt������ newname.txt����;

/ *���������󲢽����ƶ����������е���һ��Ŀ¼* / 
f_rename���� log.txt������ old / log0001.txt������
*/
u8 mf_rename(u8 *oldname,u8* newname)
{
	return  f_rename((const TCHAR *)oldname,(const TCHAR *)newname);
}
//��ȡ�̷����������֣�
//path:����·��������"0:"��"1:"  
void mf_getlabel(u8 *path)
{
	u8 buf[20];
	u32 sn=0;
	u8 res;
	res=f_getlabel ((const TCHAR *)path,(TCHAR *)buf,(DWORD*)&sn);
	if(res==FR_OK)
	{
		printf("\r\n����%s ���̷�Ϊ:%s\r\n",path,buf);
		printf("����%s �����к�:%X\r\n\r\n",path,sn); 
	}else printf("\r\n��ȡʧ�ܣ�������:%X\r\n",res);
}
//�����̷����������֣����11���ַ�������֧�����ֺʹ�д��ĸ����Լ����ֵ�
//path:���̺�+���֣�����"0:ALIENTEK"��"1:OPENEDV"  
void mf_setlabel(u8 *path)
{
	u8 res;
	res=f_setlabel ((const TCHAR *)path);
	if(res==FR_OK)
	{
		printf("\r\n�����̷����óɹ�:%s\r\n",path);
	}else printf("\r\n�����̷�����ʧ�ܣ�������:%X\r\n",res);
} 

//���ļ������ȡһ���ַ���
//size:Ҫ��ȡ�ĳ���
void mf_gets(u16 size)
{
	TCHAR* rbuf;
	rbuf=f_gets((TCHAR*)fatbuf,size,file);
	if(*rbuf==0)return  ;//û�����ݶ���
	else
	{
		printf("\r\nThe String Readed Is:%s\r\n",rbuf);  	  
	}			    	
}
//��Ҫ_USE_STRFUNC>=1
//дһ���ַ����ļ�
//c:Ҫд����ַ�
//����ֵ:ִ�н��
u8 mf_putc(u8 c)
{
	return f_putc((TCHAR)c,file);
}
//д�ַ������ļ�
//c:Ҫд����ַ���
//����ֵ:д����ַ�������
u8 mf_puts(u8*c)
{
	return f_puts((TCHAR*)c,file);
}



	// FATFS g_fat;

	// FIL g_fil;

	// uint32_t length = 0;

	// char* g_pStr = NULL;

/*1. f_mount()ע��/ȡ��ע����Ĺ�����*/
	// res = f_mount(&g_fat,     	/* [IN]�ļ�ϵͳ����*/ 
	// 		"0:",   		/* [IN]�߼���������*/
	// 		1); 		    /* [IN]��ʼ��ѡ��*/
	// mf_showfree("0:");
	// g_pStr = (char*)malloc(sizeof(char) * 256);
	// if (res == FR_OK) 
	// {
    //     strcpy(g_pStr, "0:");
		
	// 	scan_files(g_pStr, 0);	/*ɨ������·���ļ�*/
    // }
	
	// free(g_pStr);
	
	// f_open(&g_fil, "0:readme.txt", FA_READ | FA_WRITE | FA_OPEN_ALWAYS);

	// char* cpTmp = NULL;
	// if(f_size(&g_fil) > 512)
	// {	
	// 	cpTmp = (char*)malloc(sizeof(char) * 513);
	// 	memset(cpTmp,0,513);
	// }	
	// else
	// {
	// 	cpTmp = (char*)malloc(sizeof(char) * f_size(&g_fil));
	// 	memset(cpTmp,0,f_size(&g_fil));
	// }
	
	// int32_t num = f_size(&g_fil);
	// while(num > 0)
	// {
	// 	if(num > 512)
	// 	{
	// 		f_read(&g_fil, cpTmp, 512, &length);
	// 		num -= 512;
	// 	}
	// 	else
	// 	{
	// 		memset(cpTmp + num,0,1);
	// 		f_read(&g_fil, cpTmp, num, &length);
	// 		num = 0;
	// 	}
	// 	printf("%s", cpTmp);
	// }
	
	// free(cpTmp);
	// num = f_size(&g_fil);
	// int i = 10;
	// while(i--)
	// {
	// 	res = f_lseek(&g_fil,f_size(&g_fil));	/*�����ļ�ĩβ��׷������*/ 
		
	// 	f_write(&g_fil, "test 8020 04 08 13: 26\r\n", strlen("test 1020 04 08 13: 25\r\n") ,&length);

	// }	
	// 	f_close(&g_fil);






