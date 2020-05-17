#include "base64.h"
#include "stdlib.h"
#include "float.h"
#include "math.h"
const char base64_tab[64] = {
'A','B','C','D','E','F','G','H',
'I','J','K','L','M','N','O','P',
'Q','R','S','T','U','V','W','X',
'Y','Z','a','b','c','d','e','f',
'g','h','i','j','k','l','m','n',
'o','p','q','r','s','t','u','v',
'w','x','y','z','0','1','2','3',
'4','5','6','7','8','9','+','/' };


static char* base64_encoding(unsigned char count, unsigned char* scr_base, unsigned char* dest_base)
{
	unsigned int tmp = 0;

	if(count == 1)
	{
		tmp = (scr_base[0] << 16);
		dest_base[0] = base64_tab[tmp >> 18];
		dest_base[1] = base64_tab[(tmp & 0x0003FFFF) >> 12];
		dest_base[2] = '=';
		dest_base[3] = '=';
	}
	else if (count == 2)
	{
		tmp = (scr_base[0] << 16) + (scr_base[1] << 8);
		dest_base[0] = base64_tab[tmp >> 18];
		dest_base[1] = base64_tab[(tmp & 0x0003FFFF) >> 12];
		dest_base[2] = base64_tab[(tmp & 0x00000FFF) >> 6];
		dest_base[3] = '=';
	}
	else if(count == 3)
	{
		tmp = (scr_base[0] << 16) + (scr_base[1] << 8) + scr_base[2];
		dest_base[0] = base64_tab[tmp >> 18];
		dest_base[1] = base64_tab[(tmp & 0x0003FFFF) >> 12];
		dest_base[2] = base64_tab[(tmp & 0x00000FFF) >> 6];
		dest_base[3] = base64_tab[(tmp & 0x0000003F)];
	}
	return 0;
}


unsigned char* base64_array_encoding(unsigned int total_size, unsigned char* scr_base, unsigned int* base64_size)
{

	unsigned int uiTmp = total_size / 3;						   			/*����3�ı���*/

	if (fabs((total_size / 3.0) - (total_size / 3)) > DBL_EPSILON)			/*�ж��Ƿ�������*/
		uiTmp += 1;

	unsigned char* cpTmp = (unsigned char*)malloc(4 * uiTmp * sizeof(char));/*����ռ�*/
    
    if(cpTmp == NULL)
    {
        return 0;
    }

    *base64_size = 4 * uiTmp * sizeof(char);								/*����base64�������ֽ�����*/
	
    unsigned char* cpTmp1 = cpTmp;

	unsigned char* scr_base1 = scr_base;

	for (unsigned i = 0; i < total_size / 3; i++)							/*ͨ���������ֽڵ�base64����*/
	{
		base64_encoding(3, scr_base1, cpTmp1 );								/*����base64����*/

		scr_base1 += 3 ;

		cpTmp1 += 4;
	}

	base64_encoding(total_size % 3, scr_base1, cpTmp1);						/*������3��������ʱ��Ҫ����*/

	return cpTmp;/*ע��ռ�Ҫfree*/
}

