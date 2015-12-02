
#include "time_display.h"

/*******************************************************************************
功能：获取当前日期
参数：通过RTCC_GetTime(TimeBuff)；获得的TimeBuff
*******************************************************************************/
u32 GetDate(u8 *tb)
{
  u8 date[6];
	date[0] = (tb[0]/10);
	date[1] = (tb[0]%10);
	date[2] = (tb[1]/10);
	date[3] = (tb[1]%10);
	date[4] = (tb[2]/10);
	date[5] = (tb[2]%10);  
               
        return (date[0]*(100000) + date[1]*(10000) + date[2]*(1000) + date[3]*(100) + date[4]*(10) + date[5]);
}

/*******************************************************************************
功能：指定位置时间显示
*******************************************************************************/
void time_display(u16 dw,u16 tw,u8 *tb)
{
  u8 dis_data[31];

	dis_data[0] = ' ';
	dis_data[1] = ' ';
	dis_data[2] = 'D';
	dis_data[3] = 'U';
	dis_data[4] = ' ';

  dis_data[5] = '2';
	dis_data[6] = '0';
	dis_data[7] = (tb[0]/10)+0x30;
	dis_data[8] = (tb[0]%10)+0x30;
	dis_data[9] = '-';
	dis_data[10] = (tb[1]/10)+0x30;
	dis_data[11] = (tb[1]%10)+0x30;
	dis_data[12] = '-';
	dis_data[13] = (tb[2]/10)+0x30;
	dis_data[14] = (tb[2]%10)+0x30;
	dis_data[15] = ' ';
	dis_data[16] = ' ';
	dis_data[17] = (tb[3]/10)+0x30;
	dis_data[18] = (tb[3]%10)+0x30;
	dis_data[19] = ':';
	dis_data[20] = (tb[4]/10)+0x30;
	dis_data[21] = (tb[4]%10)+0x30;
	dis_data[22] = ':';
	dis_data[23] = (tb[5]/10)+0x30;														
	dis_data[24] = (tb[5]%10)+0x30;																						

	dis_data[25] = ' ';
	dis_data[26] = ' ';
	dis_data[27] = ' ';
	dis_data[28] = ' ';
	dis_data[29] = ' ';

	dis_data[30] = 0;

  dis_data[2] = 'D';
  dis_data[3] = 'a';
  dis_data[4] = 't';
  dis_data[5] = 'e';
  dis_data[6] = ' ';  
  dis_data[15] = 0;
  TXM_FillTextWidget (dw, (void*)&dis_data[2]);
  
  dis_data[12] = 'T';
  dis_data[13] = 'i';
  dis_data[14] = 'm';
  dis_data[15] = 'e';
  dis_data[16] = ' ';
  dis_data[25] = 0;
  TXM_FillTextWidget (tw, (void*)&dis_data[12]);
}

/*******************************************************************************
*******************************************************************************/
