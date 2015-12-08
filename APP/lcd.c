#include "lcd.h"

u8 buff_temp[50];

u8 LCM_HandOn_Check(void)
{
  u8 i=0,state = 0;
  u16 len = 0;
  
  ZTM_HandOn();
  
  for(i=0;i<10;i++)
  {
    delay_ms(10); 
    
    len = BSP_USART_Receive(LCM_COM_PORT,buff_temp,50);
    if((len==1) && (buff_temp[0]==0x55))
    {
      state = 1;
      break;
    }
  }  
  
  return(state);
}
void LCM_Light_Setting(u8 level)
{
  ZTM_LightLevel(level);
}


/*******************************************************************************
*******************************************************************************/
void LCM_Init(void)
{

  delay_ms(100);
  
  //与显示板握手,等待
  while(!LCM_HandOn_Check());
//  LCM_HandOn_Check();
  LCM_Light_Setting(50);
  
  ZTM_FullScreenImageDisp(2);
 
}