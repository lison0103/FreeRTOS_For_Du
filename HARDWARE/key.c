#include "sys.h" 
#include "key.h" 
#include "delay.h"

/***************************************************************************************************
***************************************************************************************************/
void key_init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
  
  if(HARDWARE_V2 == GetHardwareVerison())
  {
      //上拉
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU; // 
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC , &GPIO_InitStruct);
  }
  else if(HARDWARE_V1 == GetHardwareVerison())
  {
    //开漏输出
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_OD; // 
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC , &GPIO_InitStruct);
    GPIOC->BSRR |= 0X0000000F;
    //GPIOC->BRR |= 0X00000001;
    
    //上拉
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU; // 
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC , &GPIO_InitStruct);
  }
}


/*
//72M
//n=2000 T=392us
//n=1000 T=196us
//n=500  T=98us
//n=100  T=20.6us
*/ 
void Delay_us(uint32_t n)
{ 
	uint32_t m=0;
  m=n*50;
	
	while(m)
	{
		m--;
	}	
}

/***************************************************************************************************
功能：按键扫描函数
参数：mode 1：支持连按  0：不支持连按
***************************************************************************************************/
u16 key_scan(u8 mode)
{
    static u8 key_up = 1;
    static u32 count = 0;

    if(HARDWARE_V2 == GetHardwareVerison())
    {
          if(mode) 
          {
              count++;
              if(count == 200000)//通过计数来实现连按模式，比较简单
              {
                count = 0;
                key_up = 1;     
              }
          }
                    
          if(key_up && (KEY_SW2==0||KEY_SW3==0||KEY_SW4==0||KEY_SW5==0||KEY_SW6==0||KEY_SW7==0||KEY_SW8==0||KEY_SW9==0||KEY_SW10==0))
          {
                  delay_ms(10);//去抖动 
                  key_up = 0;

                  if(KEY_SW2==0)return KEY_F1;
                  else if(KEY_SW3==0)return KEY_F2;
                  else if(KEY_SW4==0)return KEY_F3; 
                  else if(KEY_SW5==0)return KEY_UP;
                  else if(KEY_SW6==0)return KEY_LEFT;
                  else if(KEY_SW7==0)return KEY_RIGHT;
                  else if(KEY_SW8==0)return KEY_DOWN;
                  else if(KEY_SW9==0)return KEY_SET;
                  else if(KEY_SW10==0)return KEY_ESC;
          }else if(KEY_SW2==1&&KEY_SW3==1&&KEY_SW4==1&&KEY_SW5==1&&KEY_SW6==1&&KEY_SW7==1&&KEY_SW8==1&&KEY_SW9==1&&KEY_SW10==1)key_up = 1;
          
          return 0;// 无按键按下
    
    }
    else if(HARDWARE_V1 == GetHardwareVerison())
    {
        u32 i=0,j=0,k=0,key_mask[4]={0x000000f0,0x000000b0,0x00000030,0x000000B0};
        u32 key_temp=0;      
        
        j=1;
        key_temp=0;
        for(i=0;i<4;i++)
        {
          GPIOC->BRR |= j;

          Delay_us(50); 

          k = GPIOC->IDR&key_mask[i];
          if(k != key_mask[i])
          {
            key_temp = k+i;
            key_temp &= 0x000000ff; 
          }                             
          GPIOC->BSRR |= j;
            
          j<<=1;
        } 
        
        if(mode) 
        {
            count++;
            if(count == 800)//通过计数来实现连按模式，比较简单
            {
              count = 0;
              key_up = 1;     
            }
        }
        
        if(key_up&&key_temp)
        {
          key_up = 0;
          return((u16)key_temp);
        }
        else if(key_temp == 0)key_up = 1;

        return 0;
    }
    else
    {
        return 0;
    }
}
QueueHandle_t xQueue1;
void vKEYTask( void *pvParameters )
{

      u32 key_data;    
      
      portBASE_TYPE xStatus;
      
      xQueue1 = xQueueCreate( 1, sizeof( uint32_t ) );
      
      for( ;; )
      {
          key_data = key_scan(0);
          
          xStatus = xQueueSendToFront(xQueue1,&key_data,0);
          
          if( xStatus != pdPASS);
          
          vTaskDelay(5);
      
      }
      
}





















