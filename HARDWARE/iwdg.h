#ifndef __IWDG_H_
#define __IWDG_H_
#include "stm32f10x.h"

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

void wdt_init(void);
void vIWDGTask( void *pvParameters );

#endif