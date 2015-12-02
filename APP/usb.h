#ifndef __USB_H
#define __USB_H

#include "sys.h"

#include "mass_mal.h"
#include "usb_lib.h"
#include "hw_config.h"
#include "usb_pwr.h"
#include "memory.h"	    
#include "usb_bot.h"  

#include "malloc.h"  
#include "delay.h"
#include "usart.h"
#include "key.h"
#include "ZTM.h"

void usb_port_set(u8 enable);
void USBD_Init(void);
void UsbMassStor_init(void);
void UsbMassStor_Status(void);
#endif