/****************************************Copyright (c)****************************************************
** File Name:           main.c
** Created By:          lisonchen
** Created date:        2015-09-08
** Version:             v0.2
** Descriptions:        Bootloader
** Last Update:         2015-10-27           
*********************************************************************************************************/



#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "flash.h"
#include "malloc.h"  
#include "menu.h"
#include "usb.h"
#include "fatfs.h"
#include "lcd.h"
#include "iwdg.h"
#include "bkp.h"

u8 *Version = {"V0.2"};


///* Task priorities. */
//#define mainQUEUE_POLL_PRIORITY				( tskIDLE_PRIORITY + 2 )
//#define mainCHECK_TASK_PRIORITY				( tskIDLE_PRIORITY + 3 )
//#define mainSEM_TEST_PRIORITY				( tskIDLE_PRIORITY + 1 )
//#define mainBLOCK_Q_PRIORITY				( tskIDLE_PRIORITY + 2 )
//#define mainCREATOR_TASK_PRIORITY           ( tskIDLE_PRIORITY + 3 )
//#define mainFLASH_TASK_PRIORITY				( tskIDLE_PRIORITY + 1 )
//#define mainCOM_TEST_PRIORITY				( tskIDLE_PRIORITY + 1 )
//#define mainINTEGER_TASK_PRIORITY           ( tskIDLE_PRIORITY )
//
///* Constants related to the LCD. */
//#define mainMAX_LINE						( 240 )
//#define mainROW_INCREMENT					( 24 )
//#define mainMAX_COLUMN						( 20 )
//#define mainCOLUMN_START					( 319 )
//#define mainCOLUMN_INCREMENT 				( 16 )
//
///* The maximum number of message that can be waiting for display at any one
//time. */
//#define mainLCD_QUEUE_SIZE					( 3 )
//
///* The check task uses the sprintf function so requires a little more stack. */
//#define mainCHECK_TASK_STACK_SIZE			( configMINIMAL_STACK_SIZE + 50 )
//
///* Dimensions the buffer into which the jitter time is written. */
//#define mainMAX_MSG_LEN						25
//
///* The time between cycles of the 'check' task. */
//#define mainCHECK_DELAY						( ( TickType_t ) 5000 / portTICK_PERIOD_MS )
//
///* The number of nano seconds between each processor clock. */
//#define mainNS_PER_CLOCK ( ( unsigned long ) ( ( 1.0 / ( double ) configCPU_CLOCK_HZ ) * 1000000000.0 ) )
//
///* Baud rate used by the comtest tasks. */
//#define mainCOM_TEST_BAUD_RATE		( 115200 )
//
///* The LED used by the comtest tasks. See the comtest.c file for more
//information. */
//#define mainCOM_TEST_LED			( 3 )


void vLCDTask( void *pvParameters )
{


	for( ;; )
	{
          menu_pocess();
	}
}

int main(void)
{ 
  

        RCC_Configuration(RCC_PLLMul_9);//ʱ�ӳ�ʼ��
	NVIC_Configuration();	 //�жϷ�������
	delay_init();	    	 //��ʱ������ʼ��	          					
        hardware_init();        //Ӳ���汾ȷ�ϳ�ʼ��       	               
        key_init();             //������ʼ��                               
        USART2_Init(115200);          //USART2��ʼ��������LCD���ݴ���  

        LCM_Init();             //ZTM LCD��ʼ��
                     
        enter_menu();           //�ж��Ƿ����Loader���ȴ�Լ1s������F1���룬�������APP
        mem_init();		//��ʼ���ڴ��
        SPI_Flash_Init();       //�ⲿflash��ʼ��        
        RTCC_Init();            //RTC��ʼ����rt1302
        BKP_Init();             //BKP�Ĵ�����ʼ��
        Version_init();         //��ȡAPP���µ��İ汾��
        Language_init();        //��ȡAPP���õ�����
        
 	if(exfuns_init())		//Ϊfatfs��ر��������ڴ�
        {
            //�����ڴ�ʧ��
        }
        menu_init();            //��ʼ���˵�       
        
        //��ʼ��һ����ʱ������ʱ����ʾ
        TIM3_Int_Init(4999,7199);//10Khz�ļ���Ƶ�ʣ�������5000Ϊ500ms       
//        wdt_init();             //��ʼ���������Ź���20s��λ
     
/********************************************************************************************/	
               
        xTaskCreate( vLCDTask, "LCD", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL );
        
        vTaskStartScheduler();
        
        
        return 0;
}
	   



