
#include "bsp_rtc_1302.h" 

#define RTCC_SDA_PORT   GPIOB
#define RTCC_SCL_PORT   GPIOB
#define RTCC_RST_PORT   GPIOB

#define RTCC_SDA_PIN    GPIO_Pin_8
#define RTCC_SCL_PIN    GPIO_Pin_7
#define RTCC_RST_PIN    GPIO_Pin_9

/******************************************************************************* 
*******************************************************************************/
void uDelay(uint32_t n)
{ 
	uint32_t m=0;
  m=n*10;
	
	while(m)
	{
		m--;
	}	
}

/******************************************************************************* 
*******************************************************************************/
void SendDat_1302(u8 Dat)
{
  u8 i;
  
  for(i=0;i<8;i++)
  { 
    if(Dat&0x01)
      GPIO_SetBits(RTCC_SDA_PORT,RTCC_SDA_PIN);
    else
      GPIO_ResetBits(RTCC_SDA_PORT,RTCC_SDA_PIN);
    
    Dat>>=1;
    GPIO_SetBits(RTCC_SCL_PORT,RTCC_SCL_PIN);
    uDelay(5);
    GPIO_ResetBits(RTCC_SCL_PORT,RTCC_SCL_PIN);
		uDelay(5);
  }
}

void WriteByte_1302(u8 CmdDat,u8 Num,u8 *pSend)
{ 
  u8 i=0;
  
  GPIO_ResetBits(RTCC_RST_PORT,RTCC_RST_PIN);
  uDelay(50);
  GPIO_SetBits(RTCC_RST_PORT,RTCC_RST_PIN);
  
  SendDat_1302(CmdDat);
	
  for(i=0;i<Num;i++)
  { 
    SendDat_1302(*(pSend+i));
  }
  
  GPIO_ResetBits(RTCC_RST_PORT,RTCC_RST_PIN);
}

void ReadByte_1302(u8 CmdDat,u8 Num,u8 *pRec)
{ 
  u8 i,j,tmp,cTmp;
  
  GPIO_ResetBits(RTCC_RST_PORT,RTCC_RST_PIN); //��λ����Ϊ�͵�ƽ
  uDelay(10);
  GPIO_SetBits(RTCC_SDA_PORT,RTCC_SDA_PIN);
  uDelay(10);    
  GPIO_SetBits(RTCC_RST_PORT,RTCC_RST_PIN);
  
  SendDat_1302(CmdDat); //��������
  
  for(i=0;i<Num;i++)
  { 
    for(j=0;j<8;j++)
    { 
      tmp >>= 1;
      cTmp=GPIO_ReadInputDataBit(RTCC_SDA_PORT,RTCC_SDA_PIN);
       
      if(cTmp) tmp|=0x80;
       
      GPIO_SetBits(RTCC_SCL_PORT,RTCC_SCL_PIN);
      uDelay(5);
      GPIO_ResetBits(RTCC_SCL_PORT,RTCC_SCL_PIN);      
    }
    *(pRec+i)=tmp;
  }
  
  uDelay(10);
  GPIO_ResetBits(RTCC_RST_PORT,RTCC_RST_PIN);//��λ����Ϊ�͵�ƽ
}

void WrCmd_1302(u8 CmdDat,u8 CmdWord)
{ 
  u8 CmdBuf[2];
  
  CmdBuf[0] = CmdWord;
  WriteByte_1302(CmdDat,1,CmdBuf);
}

/******************************************************************************* 
�����գ�ʱ�����ʽ
*******************************************************************************/
void RTCC_SetTime( unsigned char *pcBuff )
{
	u8 tbf[10];
		
	//second
	tbf[0] = ((pcBuff[5]/10)<<4) | (pcBuff[5]%10);
	tbf[1] = ((pcBuff[4]/10)<<4) | (pcBuff[4]%10);
	tbf[2] = ((pcBuff[3]/10)<<4) | (pcBuff[3]%10);

	tbf[3] = ((pcBuff[2]/10)<<4) | (pcBuff[2]%10);
	tbf[4] = ((pcBuff[1]/10)<<4) | (pcBuff[1]%10);
	tbf[5] = 1;
	tbf[6] = ((pcBuff[0]/10)<<4) | (pcBuff[0]%10);
	tbf[7] = 0;
	
	WrCmd_1302(0x8e,0); //д����	
	
	WriteByte_1302(0xBe,9,tbf);
	
	WrCmd_1302(0x8e,0x80); //д������
}

u8 RTCC_GetTime( unsigned char *pcBuff )
{
	u8 tbf[10];
  static u8 read_cnt=0,second_old=0xff,rtcc_valid=0; 
	
	ReadByte_1302(0xBf,7,tbf);	
	
	pcBuff[0] = ((tbf[6]>>4)*10) + (tbf[6]&0x0f);
	pcBuff[1] = ((tbf[4]>>4)*10) + (tbf[4]&0x0f);
	pcBuff[2] = ((tbf[3]>>4)*10) + (tbf[3]&0x0f);

	pcBuff[3] = ((tbf[2]>>4)*10) + (tbf[2]&0x0f);
	pcBuff[4] = ((tbf[1]>>4)*10) + (tbf[1]&0x0f);
	pcBuff[5] = ((tbf[0]>>4)*10) + (tbf[0]&0x0f);
  
  if(read_cnt++>15)
  {
    read_cnt = 0;
    
    if((second_old != pcBuff[5]) && (pcBuff[0]>=14))
    {
      rtcc_valid = 1;
    }
    else
    {
      rtcc_valid = 0;
    }  
    
    second_old = pcBuff[5]; 
  }   
  
  return(rtcc_valid);
}

/******************************************************************************* 
*******************************************************************************/
void RTCC_Init(void)	 
{
	u8 a[10];
	GPIO_InitTypeDef GPIO_InitStruct;
	
	GPIO_InitStruct.GPIO_Pin = RTCC_SDA_PIN;             
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz; 
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_OD; 	
	GPIO_Init(RTCC_SDA_PORT , &GPIO_InitStruct);	
  GPIO_ResetBits(RTCC_SDA_PORT,RTCC_SDA_PIN);

	GPIO_InitStruct.GPIO_Pin = RTCC_SCL_PIN;             
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP; 	
	GPIO_Init(RTCC_SCL_PORT , &GPIO_InitStruct);	
  GPIO_ResetBits(RTCC_SCL_PORT,RTCC_SCL_PIN);

	GPIO_InitStruct.GPIO_Pin = RTCC_RST_PIN;             
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP; 	
	GPIO_Init(RTCC_RST_PORT , &GPIO_InitStruct);	  
  GPIO_ResetBits(RTCC_RST_PORT,RTCC_RST_PIN);
  
  //WrEnDisCmd    0x8e  //д����/��ָֹ�����
	WrCmd_1302(0x8e,0); //д����
	
	//WrCmd_1302(0x80,0x80); //ʱ�Ӳ�����
	ReadByte_1302(0x81,7,a);	
	WrCmd_1302(0x80,a[0]&0x7f); //ʱ������
	if(a[2]&0x80) WrCmd_1302(0x84,a[0]&0x7f); // 24Сʱ��
	
	WrCmd_1302(0x90,0); //�����
	
	//���ʼ���Ƿ���ȷ
  WrCmd_1302(0x8e,0x80); //д������
  

	RTCC_GetTime(a);
	
	if((a[0]<10) || (a[0]>99) || (!a[1]) || (!a[2]))
	{
		a[0] = 15;
		a[1] = 1;
		a[2] = 1;

		a[3] = 0;
		a[4] = 0;
		a[5] = 0;
		
		RTCC_SetTime(a);
	}
}	

/******************************************************************************* 
*******************************************************************************/