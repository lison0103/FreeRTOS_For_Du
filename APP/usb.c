#include "usb.h"


extern u8 LANGUAGE;
extern const u8 *Status_Item_Descrip[][2];
extern const u8 *Menu_Item_Descrip[][2];

u8 offline_cnt=0;
u8 tct=0;
u8 Divece_STA;
extern void DisconnectUsb_process(void);

/*******************************************************************************
功能：使能USB口电源
参数：1 使能 0 关闭
*******************************************************************************/
void usb_port_set(u8 enable)
{
  	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);    //使能PORTA时钟	   	 
	if(enable)_SetCNTR(_GetCNTR()&(~(1<<1)));     //退出断电模式
	else{		  
		_SetCNTR(_GetCNTR()|(1<<1));  // 断电模式
		GPIOA->CRH&=0XFFF00FFF;
		GPIOA->CRH|=0X00033000;
		PAout(12)=0;	    		  
	}

} 

/*******************************************************************************
功能：USB使能脚初始化
*******************************************************************************/
void USBD_Init(void)
{
 
   GPIO_InitTypeDef  GPIO_InitStructure;
          
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	 
   
   if(HARDWARE_V2 == GetHardwareVerison())
   {
       GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;				 
       GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 
       GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		
       GPIO_Init(GPIOC, &GPIO_InitStructure);	
       
       GPIO_SetBits(GPIOC,GPIO_Pin_12);
   
   }
   else if(HARDWARE_V1 == GetHardwareVerison())
   {
       GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;				 
       GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 
       GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		
       GPIO_Init(GPIOC, &GPIO_InitStructure);	
       
       GPIO_SetBits(GPIOC,GPIO_Pin_8);
   
   }
}

/*******************************************************************************
功能：USB连接
*******************************************************************************/
void UsbMassStor_init(void)
{
        offline_cnt=0;
	tct=0;
        Divece_STA = UNCONNECTED;

        printf("\r\n USB MASS STORAGE init! \r\n");
        
        USBD_Init();

#if defined(USE_MYMALLOC)	       
	Data_Buffer=mymalloc(BULK_MAX_PACKET_SIZE*2*4*8);	//为USB数据缓存区申请内存 4k
	Bulk_Data_Buff=mymalloc(BULK_MAX_PACKET_SIZE);	        //申请内存               64字节
#endif
        
 	//USB配置
                
        delay_ms(200);
 	usb_port_set(0); 	//USB先断开
        
	delay_ms(300);
   	usb_port_set(1);	//USB再次连接	
        
        
        Mass_Memory_Size[0]=4*1024*1024;//后4M字节
        Mass_Block_Size[0] =4096;//一般设置为512字节，但是W25Q64一个扇区大小为4k，所以这里设置为4kb
        Mass_Block_Count[0]=Mass_Memory_Size[0]/Mass_Block_Size[0];
        
        
 	USB_Interrupts_Config();    
 	Set_USBClock();   
 	USB_Init();             
}

/*******************************************************************************
功能：USB状态显示
*******************************************************************************/
void UsbMassStor_Status(void)
{
 						   			  	   	 										 
    if(Divece_STA!=bDeviceState) 
    {
        if(bDeviceState==CONFIGURED)
        {
            //提示USB连接已经建立
            
            TXM_StringDisplay(0,20,250,24,1,RED ,BLUE, (void*)Status_Item_Descrip[9][LANGUAGE]);  
        }
        else 
        {
            //提示USB被拔出了
            TXM_StringDisplay(0,20,250,24,1,RED ,BLUE, (void*)Status_Item_Descrip[10][LANGUAGE]); 
        }
        Divece_STA=bDeviceState;
    }
    if(bDeviceState==CONFIGURED)
    {
        tct++;
        delay_ms(15);
        if(tct==200)
        {
            tct=0;
            if(USB_STATUS_REG&0x10)
            {
                offline_cnt=0;//USB连接了,则清除offline计数器
                bDeviceState=CONFIGURED;
            }
            else//没有得到轮询 
            {
                offline_cnt++;  
                if(offline_cnt>1)
                {
                    bDeviceState=UNCONNECTED;//2s内没收到在线标记,代表USB被拔出了
                    DisconnectUsb_process(); //电脑上断开连接，调用断开连接子程序
                }
            }
            USB_STATUS_REG=0;
        }      
    }
        
}