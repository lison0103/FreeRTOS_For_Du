#include "string.h"
#include "exfuns.h"
//#include "fattester.h"	
#include "malloc.h"
//#include "usart.h"


///////////////////////////////公共文件区,使用malloc的时候////////////////////////////////////////////
FATFS *fs[1];  		//逻辑磁盘工作区.	 
UINT br,bw;			//读写变量
FILINFO fileinfo;	//文件信息
DIR dir;  			//目录
///////////////////////////////////////////////////////////////////////////////////////


//为exfuns申请内存
//返回值:0,成功
//1,失败
u8 exfuns_init(void)
{
	fs[0]=(FATFS*)mymalloc(sizeof(FATFS));	//为磁盘0工作区申请内存	
	if(fs[0])return 0;  //申请有一个失败,即失败.
	else return 1;	
}	 


//得到磁盘剩余容量
//drv:磁盘编号("0:"/"1:")
//total:总容量	 （单位KB）
//free:剩余容量	 （单位KB）
//返回值:0,正常.其他,错误代码
u8 exf_getfree(u8 *drv,u32 *total,u32 *free)
{
	FATFS *fs1;
	u8 res;
    u32 fre_clust=0, fre_sect=0, tot_sect=0;
    //得到磁盘信息及空闲簇数量
    res =(u32)f_getfree((const TCHAR*)drv, (DWORD*)&fre_clust, &fs1);
    if(res==0)
	{											   
	    tot_sect=(fs1->n_fatent-2)*fs1->csize;	//得到总扇区数
	    fre_sect=fre_clust*fs1->csize;			//得到空闲扇区数	   
//#if _MAX_SS!=512				  				//扇区大小不是512字节,则转换为512字节
//		tot_sect*=fs1->ssize/512;
//		fre_sect*=fs1->ssize/512;
//#endif	  
		*total=tot_sect>>1;	//单位为KB
		*free=fre_sect>>1;	//单位为KB 
 	}
	return res;
}		   
/////////////////////////////////////////////////////////////////////////////////////////////////////////////




















