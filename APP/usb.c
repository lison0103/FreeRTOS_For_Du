#include "usb.h"


extern u8 LANGUAGE;
extern const u8 *Status_Item_Descrip[][2];
extern const u8 *Menu_Item_Descrip[][2];

u8 offline_cnt=0;
u8 tct=0;
u8 Divece_STA;
extern void DisconnectUsb_process(void);

/*******************************************************************************
���ܣ�ʹ��USB�ڵ�Դ
������1 ʹ�� 0 �ر�
*******************************************************************************/
void usb_port_set(u8 enable)
{
  	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);    //ʹ��PORTAʱ��	   	 
	if(enable)_SetCNTR(_GetCNTR()&(~(1<<1)));     //�˳��ϵ�ģʽ
	else{		  
		_SetCNTR(_GetCNTR()|(1<<1));  // �ϵ�ģʽ
		GPIOA->CRH&=0XFFF00FFF;
		GPIOA->CRH|=0X00033000;
		PAout(12)=0;	    		  
	}

} 

/*******************************************************************************
���ܣ�USBʹ�ܽų�ʼ��
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
���ܣ�USB����
*******************************************************************************/
void UsbMassStor_init(void)
{
        offline_cnt=0;
	tct=0;
        Divece_STA = UNCONNECTED;

        printf("\r\n USB MASS STORAGE init! \r\n");
        
        USBD_Init();

#if defined(USE_MYMALLOC)	       
	Data_Buffer=mymalloc(BULK_MAX_PACKET_SIZE*2*4*8);	//ΪUSB���ݻ����������ڴ� 4k
	Bulk_Data_Buff=mymalloc(BULK_MAX_PACKET_SIZE);	        //�����ڴ�               64�ֽ�
#endif
        
 	//USB����
                
        delay_ms(200);
 	usb_port_set(0); 	//USB�ȶϿ�
        
	delay_ms(300);
   	usb_port_set(1);	//USB�ٴ�����	
        
        
        Mass_Memory_Size[0]=4*1024*1024;//��4M�ֽ�
        Mass_Block_Size[0] =4096;//һ������Ϊ512�ֽڣ�����W25Q64һ��������СΪ4k��������������Ϊ4kb
        Mass_Block_Count[0]=Mass_Memory_Size[0]/Mass_Block_Size[0];
        
        
 	USB_Interrupts_Config();    
 	Set_USBClock();   
 	USB_Init();             
}

/*******************************************************************************
���ܣ�USB״̬��ʾ
*******************************************************************************/
void UsbMassStor_Status(void)
{
 						   			  	   	 										 
    if(Divece_STA!=bDeviceState) 
    {
        if(bDeviceState==CONFIGURED)
        {
            //��ʾUSB�����Ѿ�����
            
            TXM_StringDisplay(0,20,250,24,1,RED ,BLUE, (void*)Status_Item_Descrip[9][LANGUAGE]);  
        }
        else 
        {
            //��ʾUSB���γ���
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
                offline_cnt=0;//USB������,�����offline������
                bDeviceState=CONFIGURED;
            }
            else//û�еõ���ѯ 
            {
                offline_cnt++;  
                if(offline_cnt>1)
                {
                    bDeviceState=UNCONNECTED;//2s��û�յ����߱��,����USB���γ���
                    DisconnectUsb_process(); //�����϶Ͽ����ӣ����öϿ������ӳ���
                }
            }
            USB_STATUS_REG=0;
        }      
    }
        
}