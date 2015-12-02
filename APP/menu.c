#include "menu.h"


u8 LANGUAGE = 0;
u8 menu_item = 0;
u8 lcd_sleep = 0;
u8 usb_connect = 0;
extern u8 sleepcount;
u8 App_Version[2] = {1,0};

const u8 *Menu_Item_Descrip[][2] =
{   
  {"Lang","����"},
  {" ��:���ӵ���      "," ��:Connect to PC "},
  {" ��:�Ͽ���������  "," ��:Disconnect PC "},
  {" ��:����APP       "," ��:Update APP    "},
  {" SET:����APP      "," SET:Enter APP    "},
  
};

const u8 *Status_Item_Descrip[][2] =
{                                          
  
  {" ״̬�� ��ѡ��    "," Status:  Select  "},
  {" ״̬�����ӵ���   "," Status: Connect  "},
  {" ״̬����ʼ����APP"," Status:  Update  "},
  {" ״̬��APP�������"," Status:  Finish  "},
  {" ״̬������ʧ��   "," Status:  Fail    "},
  {"״̬���޿ɸ��¹̼�"," Status: NO FILE  "},
  {" ״̬������APP    "," Status: Enter APP"},
  {" ״̬����APP����  "," Status:  NO APP  "},
  {" ״̬���ѶϿ����� "," Status:Disconnect"},
  {" ״̬��USB������  "," Status:  Connect "},
  {" ״̬��USB���γ���"," Status:Disconnect"},
  {" ״̬���ǺϷ����� "," Status:  Illegal "},
};

const u8 *Status_Descrip[][2] =
{                                          
  
  {" ������..."  ," Connecting..."},
  {" ���ӶϿ���" ," Disconnect! "},
  {" ���¹̼�..."," Update..."},
  {" ����APP��"  ," Enter APP!"},
  {" USB����д������!"," USB is writing!"},
  {" USB���ڶ�������!"," USB is read!   "},
  {" ���Ժ�...       "," Please wait... "},  
  {" �޿ɸ��¹̼�!"," No Update File!"},
  {" FATFS��ʼ��ʧ��!"," FATFS init fail!"},
};

extern u8 *Version;

/*******************************************************************************
���ܣ�ͼ����ʾ
������
*******************************************************************************/
void Framebuffer_display(u16 x1, u16 y1, u16 x2, u16 y2, u8 size, const u8 *buf1, const u8 *buf2, u16 tms)
{
    TIM_Cmd(TIM3, DISABLE);                             //ʱ����ʾ����˸����ʱ�޷����������ͣ��ʱ��
    
    ZTM_DisBufSwitch(0x00);                                             /* ȡ���Զ��������             */
    ZTM_DisBufSwitch(0x20);                                             /* дָ��ָ���û���           */
    
    TXM_StringDisplay(0,x1,y1,size,1,WHITE ,DGRAY,(void*)buf1);
    TXM_StringDisplay(0,x2,y2,size,1,WHITE ,DGRAY,(void*)buf2);
    
    ZTM_DisBufSwitch(0x40);                                             /* ����ʾ��������Ϊ���û���     */
    delay_ms(tms);
    vTaskDelay( tms / portTICK_RATE_MS);
    
//    ZTM_DisBufSwitch(0x20);                                             /* дָ��ָ��������             */
//    ZTM_DisBufSwitch(0x40);                                             /* ����ʾ��������Ϊ������       */
//    ZTM_DisBufSwitch(0x10);                                             /* ʹ���Զ��������             */   
//
//    TIM_Cmd(TIM3, ENABLE);
}
/*******************************************************************************
���ܣ��ر�ͼ����ʾ
*******************************************************************************/
void Closeframebuffer(void)
{    
    ZTM_DisBufSwitch(0x20);                                             /* дָ��ָ��������             */
    ZTM_DisBufSwitch(0x40);                                             /* ����ʾ��������Ϊ������       */
    ZTM_DisBufSwitch(0x10);                                             /* ʹ���Զ��������             */   

    TIM_Cmd(TIM3, ENABLE);
}
/*******************************************************************************
���ܣ���ȡ��������
*******************************************************************************/
void Language_init(void)
{
    u8 addr[1] = {0};
    SPI_Flash_Read(addr,11,1);// 11ΪAPP���õı������Եĵ�ַ
    LANGUAGE = addr[0];
}

/*******************************************************************************
���ܣ�APP�汾�ż�¼
*******************************************************************************/
void Version_init(void)
{
    if(1 != BKP_Read(BKP_ADDR1(1)))
    {
        BKP_Write(BKP_ADDR1(1),1);   
        BKP_Write(BKP_ADDR1(2),'1');
        BKP_Write(BKP_ADDR1(3),'0');
        App_Version[0] = '1';
        App_Version[1] = '0';
    }  
    else
    {
        App_Version[0] = BKP_Read(BKP_ADDR1(2));
        App_Version[1] = BKP_Read(BKP_ADDR1(3));
    }
}

/*******************************************************************************
���ܣ�RCCʱ�ӳ�ʼ��
*******************************************************************************/
void RCC_init(void)
{
//    RCC_DeInit();
    
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE); //for dma  
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);//Ϊ�ⲿ�ж�
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2 , ENABLE); //USART2 ʱ��ʹ��
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA  
                           |RCC_APB2Periph_GPIOB   
                             |RCC_APB2Periph_GPIOC,
                             
                             ENABLE); //�˿�ʱ��ʹ��  
  
}
void RCC_Configuration(uint32_t PLLMul)
{
  ErrorStatus HSEStatus;
  
  SystemInit();
  
  RCC_DeInit(); //ʱ�ӳ�ʼ��
  RCC_HSEConfig(RCC_HSE_ON); //���ⲿʱ��
  HSEStatus = RCC_WaitForHSEStartUp(); //��ȡ�ⲿʱ��״̬
  if(HSEStatus == SUCCESS) //�����ɹ�
  {
      FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable); //Ԥȡָ����ʹ�� 
      FLASH_SetLatency(FLASH_Latency_2); //���ô�����ʱ�������Ƕ�������
      RCC_HCLKConfig(RCC_SYSCLK_Div1);//����AHB��ʱ��HCLK 	  
      RCC_PCLK2Config(RCC_HCLK_Div1); //��������ѡ�õľ���HCLK	
      RCC_PCLK1Config(RCC_HCLK_Div2);	  //���������õ���HCLK��2��Ƶ
      RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);//ѡ��PLL������ʱ���Լ����ķ�Ƶϵ��	
      RCC_PLLCmd(ENABLE);	//ʹ��PLL���໷
      while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
      {
        ;
      }			 //�ȴ�LL���໷�ȶ�
      RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);//ѡ��PLL���໷Ϊϵͳʱ��	
      while(RCC_GetSYSCLKSource() != 0x08) //���������ϵͳʱ���ǲ��������໷�ṩ��
      {
        ;
      }
  }
  
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE); //for dma  
  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);//Ϊ�ⲿ�ж�
  
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2 , ENABLE); //USART2 ʱ��ʹ��
  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA  
                         |RCC_APB2Periph_GPIOB   
                           |RCC_APB2Periph_GPIOC,
                           
                           ENABLE); //�˿�ʱ��ʹ��  
}
/*******************************************************************************
���ܣ��˵���ʾ
*******************************************************************************/
void menu_init(void)
{
   
  if(0 == menu_item)//�˵���ʼ��
  {
      ZTM_RectangleFill (0, 0, 239, 319,LGRAY); 
      delay_ms(5);
      ZTM_FullScreenImageDisp(300);
      delay_ms(5);
      ZTM_RectangleFill (0, 280,239, 319,DGRAY); 
      delay_ms(5);
      TXM_StringDisplay(0,180,290,24,0,RED ,WHITE, (void*)Menu_Item_Descrip[0][LANGUAGE]);       
      delay_ms(5);  
      
      TXM_StringDisplay(0,5,35,16,0,BLACK ,BLUE,  (void*)Version);
      delay_ms(5);                 
      
      for(u8 i = 0;i < 4;i++)
      {
          TXM_StringDisplay(0,20,70 + (i*45),24,1,YELLOW ,BLUE,  (void*)Menu_Item_Descrip[i+1][LANGUAGE]);
          delay_ms(5);                
      }
      
      TXM_StringDisplay(0,20,250,24,1,RED ,BLUE,    (void*)Status_Item_Descrip[0][LANGUAGE]);
      delay_ms(5);      
      
      menu_item = 1;
  
  }
  else
  {
    
      TXM_StringDisplay(0,180,290,24,1,RED ,DGRAY, (void*)Menu_Item_Descrip[0][LANGUAGE]);       
      delay_ms(5); 
      
      for(u8 i = 0;i < 4;i++)
      {
          TXM_StringDisplay(0,20,70 + (i*45),24,1,YELLOW ,BLUE,  (void*)Menu_Item_Descrip[i+1][LANGUAGE]);
          delay_ms(5);                
      }    
      
      if(1 == menu_item)//�����л�
      {
          TXM_StringDisplay(0,20,250,24,1,RED ,BLUE,    (void*)Status_Item_Descrip[0][LANGUAGE]);
          delay_ms(5);            
      }
      else if(2 == menu_item)//ѡ�����ӵ���
      {
          TXM_StringDisplay(0,20,70,24,1,YELLOW ,RED,  (void*)Menu_Item_Descrip[1][LANGUAGE]);
          delay_ms(5); 
          TXM_StringDisplay(0,20,250,24,1,RED ,BLUE, (void*)Status_Item_Descrip[1][LANGUAGE]);
          delay_ms(5);       
      }  
      else if(3 == menu_item)//ѡ��Ͽ�����
      {   
          TXM_StringDisplay(0,20,115,24,1,YELLOW ,RED,  (void*)Menu_Item_Descrip[2][LANGUAGE]);
          delay_ms(5);      
          TXM_StringDisplay(0,20,250,24,1,RED ,BLUE, (void*)Status_Item_Descrip[8][LANGUAGE]);
          delay_ms(5);  
      } 
      else if(4 == menu_item)//ѡ�����APP
      {  
          TXM_StringDisplay(0,20,160,24,1,YELLOW ,RED,  (void*)Menu_Item_Descrip[3][LANGUAGE]);
          delay_ms(5);
          TXM_StringDisplay(0,20,250,24,1,RED ,BLUE, (void*)Status_Item_Descrip[2][LANGUAGE]);
          delay_ms(5); 
      
      } 
      else if(5 == menu_item)//ѡ�����APP
      {  
          TXM_StringDisplay(0,20,205,24,1,YELLOW ,RED,  (void*)Menu_Item_Descrip[4][LANGUAGE]);
          delay_ms(5);      
          TXM_StringDisplay(0,20,250,24,1,RED ,BLUE, (void*)Status_Item_Descrip[6][LANGUAGE]);
          delay_ms(5);  
      }   
  }
  
}

/*******************************************************************************
���ܣ��ȴ�����loader����Լ1s����F1����
*******************************************************************************/
void enter_menu(void)
{
          u8 key = 0;
          u8 timeover = 0;
          
          if(1 == BKP_Read(BKP_ADDR1(5)))
          {
              BKP_Write(BKP_ADDR1(5),0); 
              delay_ms(1000);
          }
          else
          {          
              while(1)
              {           
                  key=key_scan(0);
                  if(key==KEY_F1)			//KEY_F1��������
                  {                
                      break;                                   
                  }        
                  delay_ms(100);
                  timeover++;
                  printf("enter_menu:timeover = %d\r\n", timeover);
                  
                  if(timeover >= 10)
                  {                
                      printf("��ʼִ��FLASH�û�����!!\r\n");
                      if(((*(vu32*)(FLASH_APP1_ADDR+4))&0xFF000000)==0x08000000)//�ж��Ƿ�Ϊ0X08XXXXXX.
                      {	 
                          iap_load_app(FLASH_APP1_ADDR);//ִ��FLASH APP����
                      }
                      else 
                      {
                          printf("��FLASHӦ�ó���,�޷�ִ��!\r\n");
                          break;                   
                      }                 
                  }
              }
          }
}

/*******************************************************************************
���ܣ����ӵ����ӳ���
*******************************************************************************/
void ConnectToPc_process(void)
{
    if(usb_connect == 0)
    {
        usb_connect = 1;
        menu_item = 2;
        menu_init();
        
        Framebuffer_display(40,130,30,154,24,Status_Descrip[0][LANGUAGE],"",100);
        printf("\r\n��ʼ��FATFS!!\r\n");
        //fatfs��ʼ��ʧ��
        if(Fatfs_init())
        {
            usb_connect = 0;
            Closeframebuffer();
            Framebuffer_display(25,130,30,154,24,Status_Descrip[8][LANGUAGE],"",1000);
        }
        //fatfs��ʼ���ɹ�
        else
        {
            printf("\r\n��ʼ��USB!!\r\n");
            UsbMassStor_init();    
        }
        Closeframebuffer();
    }  
}
/*******************************************************************************
���ܣ��Ͽ�USB���������ӳ���
*******************************************************************************/
u8 DisconnectUsb_process(void)
{
      if(usb_connect && (USB_STATUS_REG&0x01))
      {
          Framebuffer_display(25,130,25,154,24,Status_Descrip[4][LANGUAGE],Status_Descrip[6][LANGUAGE],2000);
          Closeframebuffer();
      }
      else if(usb_connect && (USB_STATUS_REG&0x02))
      {
          Framebuffer_display(25,130,25,154,24,Status_Descrip[5][LANGUAGE],Status_Descrip[6][LANGUAGE],2000);
          Closeframebuffer();
      }
      else if(usb_connect)
      {
            usb_connect = 0;
            menu_item = 3;
            menu_init();
            printf("\r\n exit usb mass \r\n");
            
            Framebuffer_display(40,130,30,154,24,Status_Descrip[1][LANGUAGE],"",100);
            usb_port_set(0);       
        #if defined(USE_MYMALLOC)
            myfree(Data_Buffer);
            myfree(Bulk_Data_Buff);
        #endif
            delay_ms(500);
            vTaskDelay( 500 / portTICK_RATE_MS);
            
            
            Closeframebuffer();
            return 1;
      }     
      return 0;
}

/*******************************************************************************
���ܣ�����APP�ӳ���
*******************************************************************************/
u8 UpdateApp_process(void)
{
    if(usb_connect == 0)
    {
        menu_item = 4;
        menu_init();                            
        printf("��ʼ��FATFS!!\r\n");                       
        
        u8 count = 3;
        u8 res = 0;   
        char filestr[20] = {0};
        while(count)//ʧ����������
        { 
//            IWDG_ReloadCounter();  //ι��
            
            if(Fatfs_init())
            {
                  Framebuffer_display(25,130,30,154,24,Status_Descrip[8][LANGUAGE],"",1000);
                  Closeframebuffer();
            }
            else
            {
                  ReadDir("0:/", filestr);
                  if(!isFileExist(filestr))//�жϹ̼��Ƿ����
                  {                                 
                      printf("��ʼ���¹̼�...\r\n");
                      Framebuffer_display(40,130,30,154,24,Status_Descrip[2][LANGUAGE],"",100);
                      
                      res = UpdateApp(filestr);
                      Closeframebuffer();
                      if(!res)//��spi flash����APP��stmflash��
                      {	    

                          TXM_StringDisplay(0,20,250,24,1,RED ,BLUE, (void*)Status_Item_Descrip[3][LANGUAGE]);//״̬��APP�������
                          printf("�̼��������!\r\n");	
                          
                          DeleteFile(filestr);                          //�̼����³ɹ���ɾ��
                          BKP_Write(BKP_ADDR1(2),App_Version[0]);       //��¼���¹̼��汾��
                          BKP_Write(BKP_ADDR1(3),App_Version[1]);
                          
                          return 1;                  
                      }
                      else if(FR_INVALID_OBJECT == res)
                      {
                          TXM_StringDisplay(0,20,250,24,1,RED ,BLUE, (void*)Status_Item_Descrip[11][LANGUAGE]);//״̬���ǺϷ�����                  
                      }
                      else 
                      {
                          TXM_StringDisplay(0,20,250,24,1,RED ,BLUE, (void*)Status_Item_Descrip[4][LANGUAGE]);//״̬������ʧ��
                          printf("��FLASHӦ�ó���!\r\n");
                      }
                  }
                  else 
                  {
                      TXM_StringDisplay(0,20,250,24,1,RED ,BLUE, (void*)Status_Item_Descrip[5][LANGUAGE]);//״̬���޿ɸ��¹̼�
                      Framebuffer_display(40,130,30,154,24,Status_Descrip[7][LANGUAGE],"",1000);
                      Closeframebuffer();
                      printf("û�п��Ը��µĹ̼�!\r\n");
                      break;                        
                  }
            }
            count--;
        }
    } 
    
    return 0;
}

/*******************************************************************************
���ܣ���ת����APP�ӳ���
*******************************************************************************/
void JumpToApp_process(void)
{
  if(usb_connect == 0)
  {
//      IWDG_ReloadCounter();  //ι��
      
      menu_item = 5;
      menu_init();
      delay_ms(100);
      
      printf("��ʼִ��FLASH�û�����!!\r\n");
      if(((*(vu32*)(FLASH_APP1_ADDR+4))&0xFF000000)==0x08000000)//�ж��Ƿ�Ϊ0X08XXXXXX.
      {	 
          Framebuffer_display(40,130,30,154,24,Status_Descrip[3][LANGUAGE],"",1000);
          Closeframebuffer();
          TIM_Cmd(TIM3, DISABLE);      
          delay_ms(100);
          INTX_DISABLE();          
//          __set_FAULTMASK(1);
//          NVIC_SystemReset();
          iap_load_app(FLASH_APP1_ADDR);//ִ��FLASH APP����
      }
      else 
      {
          printf("��FLASHӦ�ó���,�޷�ִ��!\r\n");
          TXM_StringDisplay(0,30,250,24,1,RED ,BLUE, (void*)Status_Item_Descrip[7][LANGUAGE]);//״̬����APP����                   
      }	               
  }
}

/*******************************************************************************
���ܣ��ȴ��������룬������Ӧ����
*******************************************************************************/
void menu_pocess(void)
{
            u8 key = 0;
    
            key=key_scan(0);
            if(key)                                          //�������ⰴ��
            {
                sleepcount = 0;
                ZTM_SetBuzzer(10);                          //�������£���������һ��
                                
                if(lcd_sleep)                               //��lcd���ߣ����˳�
                {
                    lcd_sleep = 0;                    
                    LCM_Light_Setting(50);
                }              
                else if(key == KEY_F3)                          
                {                 
                    LANGUAGE = (~LANGUAGE) & 0x01;          //�����л�
                    menu_init();                 
                }
                else if(key==KEY_LEFT)			        
                {                                
                    ConnectToPc_process();                 //���ӵ���                    
                }
                else if(key==KEY_RIGHT)			        
                {                   
                  if(DisconnectUsb_process())                //�Ͽ�����
                  {                     
                      delay_ms(500);
                      if(UpdateApp_process())
                      {
                          JumpToApp_process();
                      }
                  }
                }
                else if(key==KEY_UP)			        
                {                  
                    if(UpdateApp_process())                  //����APP���ɹ���������
                    {
                        JumpToApp_process();
                    }
                }
                else if(key==KEY_SET)			        
                {      
                    JumpToApp_process();                  //����APP
                } 
            }
            else if(usb_connect)
            {
                UsbMassStor_Status();                       //usb״̬��ʾ
//                if(usb_connect == 0)
//                {
//                    delay_ms(1000);
//                    UpdateApp_process();
//                }
            }
            
            if(sleepcount > 1*120)                      //1�����޲�����������Ļ����
            {
                sleepcount = 0;
                lcd_sleep = 1;
                LCM_Light_Setting(5);
            }
//            delay_ms(10);
}



