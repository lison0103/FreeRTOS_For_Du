#include "menu.h"


u8 LANGUAGE = 0;
u8 menu_item = 0;
u8 lcd_sleep = 0;
u8 usb_connect = 0;
extern u8 sleepcount;
u8 App_Version[2] = {1,0};

const u8 *Menu_Item_Descrip[][2] =
{   
  {"Lang","语言"},
  {" ←:连接电脑      "," ←:Connect to PC "},
  {" →:断开电脑连接  "," →:Disconnect PC "},
  {" ↑:更新APP       "," ↑:Update APP    "},
  {" SET:进入APP      "," SET:Enter APP    "},
  
};

const u8 *Status_Item_Descrip[][2] =
{                                          
  
  {" 状态： 请选择    "," Status:  Select  "},
  {" 状态：连接电脑   "," Status: Connect  "},
  {" 状态：开始更新APP"," Status:  Update  "},
  {" 状态：APP更新完成"," Status:  Finish  "},
  {" 状态：更新失败   "," Status:  Fail    "},
  {"状态：无可更新固件"," Status: NO FILE  "},
  {" 状态：进入APP    "," Status: Enter APP"},
  {" 状态：无APP程序  "," Status:  NO APP  "},
  {" 状态：已断开电脑 "," Status:Disconnect"},
  {" 状态：USB已连接  "," Status:  Connect "},
  {" 状态：USB被拔出了"," Status:Disconnect"},
  {" 状态：非合法程序 "," Status:  Illegal "},
};

const u8 *Status_Descrip[][2] =
{                                          
  
  {" 连接中..."  ," Connecting..."},
  {" 连接断开！" ," Disconnect! "},
  {" 更新固件..."," Update..."},
  {" 进入APP！"  ," Enter APP!"},
  {" USB正在写入数据!"," USB is writing!"},
  {" USB正在读出数据!"," USB is read!   "},
  {" 请稍后...       "," Please wait... "},  
  {" 无可更新固件!"," No Update File!"},
  {" FATFS初始化失败!"," FATFS init fail!"},
};

extern u8 *Version;

/*******************************************************************************
功能：图层显示
参数：
*******************************************************************************/
void Framebuffer_display(u16 x1, u16 y1, u16 x2, u16 y2, u8 size, const u8 *buf1, const u8 *buf2, u16 tms)
{
    TIM_Cmd(TIM3, DISABLE);                             //时间显示会闪烁，暂时无法解决，先暂停定时器
    
    ZTM_DisBufSwitch(0x00);                                             /* 取消自动缓存管理             */
    ZTM_DisBufSwitch(0x20);                                             /* 写指针指向备用缓存           */
    
    TXM_StringDisplay(0,x1,y1,size,1,WHITE ,DGRAY,(void*)buf1);
    TXM_StringDisplay(0,x2,y2,size,1,WHITE ,DGRAY,(void*)buf2);
    
    ZTM_DisBufSwitch(0x40);                                             /* 将显示缓存设置为备用缓存     */
    delay_ms(tms);
    vTaskDelay( tms / portTICK_RATE_MS);
    
//    ZTM_DisBufSwitch(0x20);                                             /* 写指针指向主缓存             */
//    ZTM_DisBufSwitch(0x40);                                             /* 将显示缓存设置为主缓存       */
//    ZTM_DisBufSwitch(0x10);                                             /* 使能自动缓存管理             */   
//
//    TIM_Cmd(TIM3, ENABLE);
}
/*******************************************************************************
功能：关闭图层显示
*******************************************************************************/
void Closeframebuffer(void)
{    
    ZTM_DisBufSwitch(0x20);                                             /* 写指针指向主缓存             */
    ZTM_DisBufSwitch(0x40);                                             /* 将显示缓存设置为主缓存       */
    ZTM_DisBufSwitch(0x10);                                             /* 使能自动缓存管理             */   

    TIM_Cmd(TIM3, ENABLE);
}
/*******************************************************************************
功能：读取语言设置
*******************************************************************************/
void Language_init(void)
{
    u8 addr[1] = {0};
    SPI_Flash_Read(addr,11,1);// 11为APP设置的保存语言的地址
    LANGUAGE = addr[0];
}

/*******************************************************************************
功能：APP版本号记录
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
功能：RCC时钟初始化
*******************************************************************************/
void RCC_init(void)
{
//    RCC_DeInit();
    
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE); //for dma  
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);//为外部中断
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2 , ENABLE); //USART2 时钟使能
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA  
                           |RCC_APB2Periph_GPIOB   
                             |RCC_APB2Periph_GPIOC,
                             
                             ENABLE); //端口时钟使能  
  
}
void RCC_Configuration(uint32_t PLLMul)
{
  ErrorStatus HSEStatus;
  
  SystemInit();
  
  RCC_DeInit(); //时钟初始化
  RCC_HSEConfig(RCC_HSE_ON); //打开外部时钟
  HSEStatus = RCC_WaitForHSEStartUp(); //读取外部时钟状态
  if(HSEStatus == SUCCESS) //启动成功
  {
      FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable); //预取指缓存使能 
      FLASH_SetLatency(FLASH_Latency_2); //设置代码延时。这里是二个周期
      RCC_HCLKConfig(RCC_SYSCLK_Div1);//设置AHB的时钟HCLK 	  
      RCC_PCLK2Config(RCC_HCLK_Div1); //高速总线选用的就是HCLK	
      RCC_PCLK1Config(RCC_HCLK_Div2);	  //低速总线用的是HCLK的2分频
      RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);//选择PLL的输入时钟以及它的分频系数	
      RCC_PLLCmd(ENABLE);	//使能PLL锁相环
      while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
      {
        ;
      }			 //等待LL锁相环稳定
      RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);//选择PLL锁相环为系统时钟	
      while(RCC_GetSYSCLKSource() != 0x08) //检验输出的系统时钟是不是由锁相环提供的
      {
        ;
      }
  }
  
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE); //for dma  
  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);//为外部中断
  
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2 , ENABLE); //USART2 时钟使能
  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA  
                         |RCC_APB2Periph_GPIOB   
                           |RCC_APB2Periph_GPIOC,
                           
                           ENABLE); //端口时钟使能  
}
/*******************************************************************************
功能：菜单显示
*******************************************************************************/
void menu_init(void)
{
   
  if(0 == menu_item)//菜单初始化
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
      
      if(1 == menu_item)//语言切换
      {
          TXM_StringDisplay(0,20,250,24,1,RED ,BLUE,    (void*)Status_Item_Descrip[0][LANGUAGE]);
          delay_ms(5);            
      }
      else if(2 == menu_item)//选择连接电脑
      {
          TXM_StringDisplay(0,20,70,24,1,YELLOW ,RED,  (void*)Menu_Item_Descrip[1][LANGUAGE]);
          delay_ms(5); 
          TXM_StringDisplay(0,20,250,24,1,RED ,BLUE, (void*)Status_Item_Descrip[1][LANGUAGE]);
          delay_ms(5);       
      }  
      else if(3 == menu_item)//选择断开电脑
      {   
          TXM_StringDisplay(0,20,115,24,1,YELLOW ,RED,  (void*)Menu_Item_Descrip[2][LANGUAGE]);
          delay_ms(5);      
          TXM_StringDisplay(0,20,250,24,1,RED ,BLUE, (void*)Status_Item_Descrip[8][LANGUAGE]);
          delay_ms(5);  
      } 
      else if(4 == menu_item)//选择更新APP
      {  
          TXM_StringDisplay(0,20,160,24,1,YELLOW ,RED,  (void*)Menu_Item_Descrip[3][LANGUAGE]);
          delay_ms(5);
          TXM_StringDisplay(0,20,250,24,1,RED ,BLUE, (void*)Status_Item_Descrip[2][LANGUAGE]);
          delay_ms(5); 
      
      } 
      else if(5 == menu_item)//选择进入APP
      {  
          TXM_StringDisplay(0,20,205,24,1,YELLOW ,RED,  (void*)Menu_Item_Descrip[4][LANGUAGE]);
          delay_ms(5);      
          TXM_StringDisplay(0,20,250,24,1,RED ,BLUE, (void*)Status_Item_Descrip[6][LANGUAGE]);
          delay_ms(5);  
      }   
  }
  
}

/*******************************************************************************
功能：等待进入loader程序，约1s，按F1进入
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
                  if(key==KEY_F1)			//KEY_F1按键按下
                  {                
                      break;                                   
                  }        
                  delay_ms(100);
                  timeover++;
                  printf("enter_menu:timeover = %d\r\n", timeover);
                  
                  if(timeover >= 10)
                  {                
                      printf("开始执行FLASH用户代码!!\r\n");
                      if(((*(vu32*)(FLASH_APP1_ADDR+4))&0xFF000000)==0x08000000)//判断是否为0X08XXXXXX.
                      {	 
                          iap_load_app(FLASH_APP1_ADDR);//执行FLASH APP代码
                      }
                      else 
                      {
                          printf("非FLASH应用程序,无法执行!\r\n");
                          break;                   
                      }                 
                  }
              }
          }
}

/*******************************************************************************
功能：连接电脑子程序
*******************************************************************************/
void ConnectToPc_process(void)
{
    if(usb_connect == 0)
    {
        usb_connect = 1;
        menu_item = 2;
        menu_init();
        
        Framebuffer_display(40,130,30,154,24,Status_Descrip[0][LANGUAGE],"",100);
        printf("\r\n初始化FATFS!!\r\n");
        //fatfs初始化失败
        if(Fatfs_init())
        {
            usb_connect = 0;
            Closeframebuffer();
            Framebuffer_display(25,130,30,154,24,Status_Descrip[8][LANGUAGE],"",1000);
        }
        //fatfs初始化成功
        else
        {
            printf("\r\n初始化USB!!\r\n");
            UsbMassStor_init();    
        }
        Closeframebuffer();
    }  
}
/*******************************************************************************
功能：断开USB电脑连接子程序
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
功能：更新APP子程序
*******************************************************************************/
u8 UpdateApp_process(void)
{
    if(usb_connect == 0)
    {
        menu_item = 4;
        menu_init();                            
        printf("初始化FATFS!!\r\n");                       
        
        u8 count = 3;
        u8 res = 0;   
        char filestr[20] = {0};
        while(count)//失败重试三次
        { 
//            IWDG_ReloadCounter();  //喂狗
            
            if(Fatfs_init())
            {
                  Framebuffer_display(25,130,30,154,24,Status_Descrip[8][LANGUAGE],"",1000);
                  Closeframebuffer();
            }
            else
            {
                  ReadDir("0:/", filestr);
                  if(!isFileExist(filestr))//判断固件是否存在
                  {                                 
                      printf("开始更新固件...\r\n");
                      Framebuffer_display(40,130,30,154,24,Status_Descrip[2][LANGUAGE],"",100);
                      
                      res = UpdateApp(filestr);
                      Closeframebuffer();
                      if(!res)//从spi flash复制APP到stmflash中
                      {	    

                          TXM_StringDisplay(0,20,250,24,1,RED ,BLUE, (void*)Status_Item_Descrip[3][LANGUAGE]);//状态：APP更新完成
                          printf("固件更新完成!\r\n");	
                          
                          DeleteFile(filestr);                          //固件更新成功后删除
                          BKP_Write(BKP_ADDR1(2),App_Version[0]);       //记录更新固件版本号
                          BKP_Write(BKP_ADDR1(3),App_Version[1]);
                          
                          return 1;                  
                      }
                      else if(FR_INVALID_OBJECT == res)
                      {
                          TXM_StringDisplay(0,20,250,24,1,RED ,BLUE, (void*)Status_Item_Descrip[11][LANGUAGE]);//状态：非合法程序                  
                      }
                      else 
                      {
                          TXM_StringDisplay(0,20,250,24,1,RED ,BLUE, (void*)Status_Item_Descrip[4][LANGUAGE]);//状态：更新失败
                          printf("非FLASH应用程序!\r\n");
                      }
                  }
                  else 
                  {
                      TXM_StringDisplay(0,20,250,24,1,RED ,BLUE, (void*)Status_Item_Descrip[5][LANGUAGE]);//状态：无可更新固件
                      Framebuffer_display(40,130,30,154,24,Status_Descrip[7][LANGUAGE],"",1000);
                      Closeframebuffer();
                      printf("没有可以更新的固件!\r\n");
                      break;                        
                  }
            }
            count--;
        }
    } 
    
    return 0;
}

/*******************************************************************************
功能：跳转进入APP子程序
*******************************************************************************/
void JumpToApp_process(void)
{
  if(usb_connect == 0)
  {
//      IWDG_ReloadCounter();  //喂狗
      
      menu_item = 5;
      menu_init();
      delay_ms(100);
      
      printf("开始执行FLASH用户代码!!\r\n");
      if(((*(vu32*)(FLASH_APP1_ADDR+4))&0xFF000000)==0x08000000)//判断是否为0X08XXXXXX.
      {	 
          Framebuffer_display(40,130,30,154,24,Status_Descrip[3][LANGUAGE],"",1000);
          Closeframebuffer();
          TIM_Cmd(TIM3, DISABLE);      
          delay_ms(100);
          INTX_DISABLE();          
//          __set_FAULTMASK(1);
//          NVIC_SystemReset();
          iap_load_app(FLASH_APP1_ADDR);//执行FLASH APP代码
      }
      else 
      {
          printf("非FLASH应用程序,无法执行!\r\n");
          TXM_StringDisplay(0,30,250,24,1,RED ,BLUE, (void*)Status_Item_Descrip[7][LANGUAGE]);//状态：无APP程序                   
      }	               
  }
}

/*******************************************************************************
功能：等待按键输入，处理相应功能
*******************************************************************************/
void menu_pocess(void)
{
            u8 key = 0;
    
            key=key_scan(0);
            if(key)                                          //按下任意按键
            {
                sleepcount = 0;
                ZTM_SetBuzzer(10);                          //按键按下，蜂鸣器响一声
                                
                if(lcd_sleep)                               //若lcd休眠，则退出
                {
                    lcd_sleep = 0;                    
                    LCM_Light_Setting(50);
                }              
                else if(key == KEY_F3)                          
                {                 
                    LANGUAGE = (~LANGUAGE) & 0x01;          //语言切换
                    menu_init();                 
                }
                else if(key==KEY_LEFT)			        
                {                                
                    ConnectToPc_process();                 //连接电脑                    
                }
                else if(key==KEY_RIGHT)			        
                {                   
                  if(DisconnectUsb_process())                //断开连接
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
                    if(UpdateApp_process())                  //更新APP，成功则进入程序
                    {
                        JumpToApp_process();
                    }
                }
                else if(key==KEY_SET)			        
                {      
                    JumpToApp_process();                  //进入APP
                } 
            }
            else if(usb_connect)
            {
                UsbMassStor_Status();                       //usb状态显示
//                if(usb_connect == 0)
//                {
//                    delay_ms(1000);
//                    UpdateApp_process();
//                }
            }
            
            if(sleepcount > 1*120)                      //1分钟无操作，调低屏幕亮度
            {
                sleepcount = 0;
                lcd_sleep = 1;
                LCM_Light_Setting(5);
            }
//            delay_ms(10);
}



