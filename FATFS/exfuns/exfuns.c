#include "string.h"
#include "exfuns.h"
//#include "fattester.h"	
#include "malloc.h"
//#include "usart.h"


///////////////////////////////�����ļ���,ʹ��malloc��ʱ��////////////////////////////////////////////
FATFS *fs[1];  		//�߼����̹�����.	 
UINT br,bw;			//��д����
FILINFO fileinfo;	//�ļ���Ϣ
DIR dir;  			//Ŀ¼
///////////////////////////////////////////////////////////////////////////////////////


//Ϊexfuns�����ڴ�
//����ֵ:0,�ɹ�
//1,ʧ��
u8 exfuns_init(void)
{
	fs[0]=(FATFS*)mymalloc(sizeof(FATFS));	//Ϊ����0�����������ڴ�	
	if(fs[0])return 0;  //������һ��ʧ��,��ʧ��.
	else return 1;	
}	 


//�õ�����ʣ������
//drv:���̱��("0:"/"1:")
//total:������	 ����λKB��
//free:ʣ������	 ����λKB��
//����ֵ:0,����.����,�������
u8 exf_getfree(u8 *drv,u32 *total,u32 *free)
{
	FATFS *fs1;
	u8 res;
    u32 fre_clust=0, fre_sect=0, tot_sect=0;
    //�õ�������Ϣ�����д�����
    res =(u32)f_getfree((const TCHAR*)drv, (DWORD*)&fre_clust, &fs1);
    if(res==0)
	{											   
	    tot_sect=(fs1->n_fatent-2)*fs1->csize;	//�õ���������
	    fre_sect=fre_clust*fs1->csize;			//�õ�����������	   
//#if _MAX_SS!=512				  				//������С����512�ֽ�,��ת��Ϊ512�ֽ�
//		tot_sect*=fs1->ssize/512;
//		fre_sect*=fs1->ssize/512;
//#endif	  
		*total=tot_sect>>1;	//��λΪKB
		*free=fre_sect>>1;	//��λΪKB 
 	}
	return res;
}		   
/////////////////////////////////////////////////////////////////////////////////////////////////////////////




















