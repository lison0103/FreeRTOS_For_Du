#ifndef __TIMER_H
#define __TIMER_H

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

//void TIM3_Int_Init(u16 arr,u16 psc);
void vTIMETask( void *pvParameters );

#endif