#ifndef __LCD_H
#define __LCD_H

#include "ZTM.h"
#include "sys.h"
#include "delay.h"

#define LCM_COM_PORT      USART2

u8 LCM_HandOn_Check(void);
void LCM_Init(void);
void LCM_Light_Setting(u8 level);


#endif