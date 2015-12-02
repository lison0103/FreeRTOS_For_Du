#ifndef __MENU_H
#define __MENU_H

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "sys.h"
#include "ZTM.h"
#include "key.h"
#include "iap.h"
#include "delay.h"
#include "usart.h"
#include "usb.h"
#include "fatfs.h"
#include "bsp_rtc_1302.h"
#include "time_display.h"
#include "lcd.h"
#include "timer.h"
#include "bkp.h"
#include "flash.h"

void RCC_init(void);
void menu_init(void);
void menu_pocess(void);
void enter_menu(void);
void Version_init(void);
void Language_init(void);
void RCC_Configuration(uint32_t PLLMul);
extern u8 menu_item;

#endif
