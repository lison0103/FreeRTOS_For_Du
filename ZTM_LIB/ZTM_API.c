/****************************************Copyright (c)****************************************************
**                               Guangzhou ZHIYUAN electronics Co.,LTD.
**
**                                 http://www.embedtools.com
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:           ZTM_API.c
** Last modified Date:  2011-06-01
** Last Version:        v1.00
** Description:         ZTM������Ӧ�ò���������
**
**--------------------------------------------------------------------------------------------------------
** Created By:          He Zengfu
** Created date:        2011-06-01
** Version:             v1.00
** Descriptions:        �����ļ�
**
**--------------------------------------------------------------------------------------------------------
** Modified by:         WangWenXiong
** Modified date:       2012-02-24
** Version:             V1.01
** Description:         ���� ZTM_HandOn������
**                      ���� ZTM_SetIdleType������
**                      ���� ZTM_SetSysMouseDis������
**                      �޸� ZTM_ClrScreen������
**                      ɾ�� ZTM_TouchMode������
**                      ���� ZTM_CheckTouchPos������
**                      ɾ�� ZTM_BeepOnOff������
**                      ���� ZTM_SetBuzzer������
**                      �޸� ZTM_LightTimeTouch������
**                      ���� ZTM_SetBaudrate������
**                      ���� ZTM_SetDisplayBuf������
**                      ���� ZTM_SetAck������
**                      ���� ZTM_SetTouchType������
**                      ���� ZTM_SetBuzzerType������
**                      ���� ZTM_SetTxtBackgroundType������
**                      ���� ZTM_SetSysMouseType������
**                      ���� ZTM_SetSumCheckType������
**                      ���� ZTM_SetTouchDelay������
**                      ���� ZTM_ReadDeviceInfo������
**                      ���� ZTM_OptDisBufDirectly������
**                      ���� ZTM_FileDownload������
**
*********************************************************************************************************/
#include "ZTM.h"

/*********************************************************************************************************
  ��������
*********************************************************************************************************/
INT8U const __GucFrameHead[1]         = {0xAA};                         /* ֡ͷ 1�ֽ�                   */
INT8U const __GucFrameTail[4]         = {0xCC, 0x33, 0xC3, 0x3C};       /* ֡β 4�ֽ�                   */
INT8U const __GucFrameTouchCal[5]     = {0xE4, 0x55, 0xAA, 0x5A, 0xA5}; /* ������У׼����               */
INT8U const __GucFrameLight[4]        = {0x55, 0xAA, 0x5A, 0xA5};       /* ���ر�������                 */
INT8U const __GucFrameFileDownload[4] = {0xF2, 0xF2, 0x5A, 0xA5};       /* �ֿ�������ļ����������   */
                                                                        /* ����                         */
INT8U ZtmSendBuff[1500];
INT16U ZtmSendCnt=0;

/*********************************************************************************************************
** ��������:  __SendByte
** ��������:  ���ֽڷ��ͺ���
** �������:  ucData��       ����
** ��������� ��
** �� �� ֵ:  ��
*********************************************************************************************************/
static void __SendByte (INT8U ucData)
{
	ZtmSendBuff[++ZtmSendCnt] = ucData;
	
/*	
    if (ZTM_BusyStat()) {                                               // ������æµ                   
        do {                                                            // �ȴ�ֱ���ָ�����              
            if (!ZTM_BusyStat()) {
                uartSendChar(ucData);
                break;
            }
            ZTM_Delay(1);
        } while (1);
    } else {                                                            // ���������У�ֱ�ӷ���          
        uartSendChar(ucData);
    }
*/
}

/*********************************************************************************************************
** ��������:  __SendDByte
** ��������:  ˫�ֽڷ��ͺ���
** �������:  usData��  ����
** ��������� ��
** �� �� ֵ:  ��
*********************************************************************************************************/
static void __SendDByte (INT16U usData)
{
	ZtmSendBuff[++ZtmSendCnt] = usData>>8;
	ZtmSendBuff[++ZtmSendCnt] = usData;
  //  __SendByte((usData>> 8) & 0xFF);                                    /* ���͸�8λ����                */
  //  __SendByte(usData & 0xFF);                                          /* ���͵�8λ����                */
}

/*********************************************************************************************************
** ��������:  __SendByteStr
** ��������:  ���ֽ��ַ������ͺ���
** �������:  *pucStr:       Ҫ���͵������ַ���ָ��
**            ulNum:         Ҫ���͵����ݸ���
** ��������� ��
** �� �� ֵ:  ��
*********************************************************************************************************/
static void __SendByteStr (INT8U const *pucStr, INT32U ulNum)
{
    INT32U i;

    for (i = 0; i < ulNum; i++) {                                       /* ����ָ���ֽ�����             */
        __SendByte(*pucStr++);
    }
}

/*********************************************************************************************************
** ��������:  __SendDByteStr
** ��������:  ˫�ֽ��ַ������ͺ���
** �������:  pucStr��  Ҫ���͵������ַ���ָ��
**            ulNum��   Ҫ���͵����ݸ���
** ��������� ��
** �� �� ֵ:  ��
*********************************************************************************************************/
static void __SendDByteStr (INT16U *pucStr, INT32U ulNum)
{
    INT32U i;

    for (i = 0; i < ulNum; i++) {                                       /* ����ָ���ֽ�����             */
        __SendDByte(*pucStr++);
    }
}

/*********************************************************************************************************
** ��������:  __SendHead
** ��������:  ��������֡ͷ
** �������:  ��
** �������:  ��
** �� �� ֵ:  ��
*********************************************************************************************************/
static void __SendHead (void)
{
	ZtmSendCnt=0;
	ZtmSendBuff[ZtmSendCnt] = 0XAA;
	
  //  __SendByteStr(__GucFrameHead, 1);
}

/*********************************************************************************************************
** ��������:  __SendTail
** ��������:  ��������֡β
** �������:  ��
** �������:  ��
** �� �� ֵ:  ��
*********************************************************************************************************/
static void __SendTail (void)
{
	ZtmSendBuff[++ZtmSendCnt] = 0xCC,
	ZtmSendBuff[++ZtmSendCnt] = 0x33,
	ZtmSendBuff[++ZtmSendCnt] = 0xC3,
	ZtmSendBuff[++ZtmSendCnt] = 0x3C,
	
	uartSendBuff(ZtmSendBuff,ZtmSendCnt+1);	
	//__SendByteStr(__GucFrameTail, 4);
}

/****************************** ������ʾ�ն�Ӧ�ò㺯�� **************************************************/

/*
 * ����ָ��
 */

/*********************************************************************************************************
** ��������:  ZTM_HandOn
** ��������:  ��������ָ��
** �������:  ��
** �������:  ��
** �� �� ֵ:  ��
*********************************************************************************************************/
void ZTM_HandOn (void)
{
    __SendHead();                                                       /* ����֡ͷ                     */
	
    __SendByte(0x00);                                                   /* ��������                     */
	
    __SendTail();                                                       /* ����֡β                     */
}

/*********************************************************************************************************
** ��������:  ZTM_SetIdleType
** ��������:  �ն�ָ���״�����ţ�Busy����Ч��ƽ����
** �������:  ucIdleType���ն�ָ���״��������Ч��ƽ���ò�����0x00�����͵�ƽ��Ч��0xFF�����ߵ�ƽ��Ч
** �������:  ��
** �� �� ֵ:  ��ȷ����1�����󷵻�0
*********************************************************************************************************/
INT8U ZTM_SetIdleType (INT8U ucIdleType)
{
	/*
     *  �������
     */
	if ((ucIdleType > 0x00) && (ucIdleType < 0xFF)) {
		return (0);
	}
	
    __SendHead();                                                       /* ����֡ͷ                     */
	
    __SendByte(0x00);                                                   /* ��������                     */
	__SendByte(ucIdleType);                                             /* ���Ͳ���ֵ                   */
	
    __SendTail();                                                       /* ����֡β                     */
	
	return (1);
}

/*
 *  ��ʾϵͳ���
 */
/*********************************************************************************************************
** ��������:  ZTM_SetSysMouseDis
** ��������:  ϵͳ�����ʾ����
** �������:  ucMouseID��ϵͳ���ͼƬID��
**            usX      ����ʾ��ʼ���X����
**            usY      ����ʾ��ʼ���Y����
** �������:  ��
** �� �� ֵ:  ��ȷ����1�����󷵻�0
*********************************************************************************************************/
INT8U ZTM_SetSysMouseDis (INT8U ucMouseID, INT16U usX, INT16U usY)
{
	/*
     *  �������
     */
	if ((usX >= ZTM_XADDR_MAX) || (usY >= ZTM_YADDR_MAX)) {
		return (0);
	}
	
	ZtmSendBuff[0] = 0xAA;					     /* ֡ͷ                     */                                                   
	ZtmSendBuff[1] = 0x21;
	
	ZtmSendBuff[2] = ucMouseID;		/* ����ϵͳ���ͼƬID           */
	ZtmSendBuff[3] = usX;					/* ������ʾ��ʼ��X����          */
	ZtmSendBuff[4] = usX;					
	ZtmSendBuff[5] = usY;					/* ������ʾ��ʼ��Y����          */
	ZtmSendBuff[6] = usY;
	
	ZtmSendCnt = 6;                     /* ����֡β                     */
	ZtmSendBuff[++ZtmSendCnt] = 0xcc;
	ZtmSendBuff[++ZtmSendCnt] = 0x33;
	ZtmSendBuff[++ZtmSendCnt] = 0xc3;
	ZtmSendBuff[++ZtmSendCnt] = 0x3c;

	uartSendBuff(ZtmSendBuff,ZtmSendCnt+1);
	
	return (1);
}

/*
 * ��ʾ��������
 */
/*********************************************************************************************************
** ��������:  ZTM_SetColor
** ��������:  ���õ�ɫ��
** �������:  usFcolor��     ǰ��ɫ
**            usBcolor��     ����ɫ
** �������:  ��
** �� �� ֵ:  ��
*********************************************************************************************************/
void ZTM_SetColor (INT16U usFcolor, INT16U usBcolor)
{
	ZtmSendBuff[0] = 0xAA;					     /* ֡ͷ                     */                                                   
	ZtmSendBuff[1] = 0x40;
		
	ZtmSendBuff[2] = usFcolor>>8;				/* ����ǰ��ɫ                   */
	ZtmSendBuff[3] = usFcolor;			
	ZtmSendBuff[4] = usBcolor>>8;				/* ���ͱ���ɫ                   */
	ZtmSendBuff[5] = usBcolor;			
 
	ZtmSendCnt = 5;                     /* ����֡β                     */
	ZtmSendBuff[++ZtmSendCnt] = 0xcc;
	ZtmSendBuff[++ZtmSendCnt] = 0x33;
	ZtmSendBuff[++ZtmSendCnt] = 0xc3;
	ZtmSendBuff[++ZtmSendCnt] = 0x3c;

	uartSendBuff(ZtmSendBuff,ZtmSendCnt+1);
}

/*********************************************************************************************************
** ��������:  ZTM_SetCharDis
** ��������:  �����ַ���ʾ���
** �������:  ucDx��         �ּ�ࣨ���� 0-255
**            ucDy��         �м�ࣨ���� 0-255
** �������:  ��ȷ����1�����򷵻�0
** �� �� ֵ:  ��
*********************************************************************************************************/
INT8U ZTM_SetCharDis (INT8U ucDx, INT8U ucDy)
{
	/*
	 *  �������
	 */
//	if ((ucDx >= CHAR_XSTEP_MAX) || (ucDy  >= CHAR_YSTEP_MAX)) return (0);
		
	ZtmSendBuff[0] = 0xAA;					     /* ֡ͷ                     */                                                   
	ZtmSendBuff[1] = 0x41;
		
	ZtmSendBuff[2] = ucDx;				 
	ZtmSendBuff[3] = ucDy;			

	ZtmSendCnt = 3;                     /* ����֡β                     */
	ZtmSendBuff[++ZtmSendCnt] = 0xcc;
	ZtmSendBuff[++ZtmSendCnt] = 0x33;
	ZtmSendBuff[++ZtmSendCnt] = 0xc3;
	ZtmSendBuff[++ZtmSendCnt] = 0x3c;

	uartSendBuff(ZtmSendBuff,ZtmSendCnt+1);

	return (1);
}

/*********************************************************************************************************
** ��������:  ZTM_SetCursor
** ��������:  ���ù����ʾ
** �������:  ucMode��       �����ʾģʽ  0:��   1:��
**            usX��          �����꣨λ�ã�
**            usY��          ������
**            ucWide��       ����ȣ�0x01~0x1f
**            ucHeight��     ���߶ȣ�0x01~0x1f
** �������:  ��ȷ����1�����򷵻�0
** �� �� ֵ:  ��
*********************************************************************************************************/
INT8U ZTM_SetCursor (INT8U ucMode, INT16U usX, INT16U usY, INT8U ucWide, INT8U ucHeight)
{
    /*
     * �������
     */
    if ((ucMode > 1) || (usX > ZTM_XADDR_MAX) || (usY > ZTM_YADDR_MAX) || (ucWide > CURSOR_WIDE_MAX) ||
        (ucHeight > CURSOR_HEIGHT_MAX) ) {
            return (0);
    }

		
	ZtmSendBuff[0] = 0xAA;							/* ֡ͷ                     */                                                   
	ZtmSendBuff[1] = 0x44;
		
	ZtmSendBuff[2] = ucMode;				 		/* ���͹����ʾģʽ             */
	
	ZtmSendBuff[3] = usX>>8;						/* ����x����                    */
	ZtmSendBuff[4] = usX;
	ZtmSendBuff[5] = usY>>8;						/* ����y����                    */
	ZtmSendBuff[6] = usY;
	
	ZtmSendBuff[7] = ucWide;						/* ���͹����                 */
	ZtmSendBuff[8] = ucHeight;					/* ���͹��߶�                 */

	ZtmSendCnt = 8;                     /* ����֡β                     */
	ZtmSendBuff[++ZtmSendCnt] = 0xcc;
	ZtmSendBuff[++ZtmSendCnt] = 0x33;
	ZtmSendBuff[++ZtmSendCnt] = 0xc3;
	ZtmSendBuff[++ZtmSendCnt] = 0x3c;

	uartSendBuff(ZtmSendBuff,ZtmSendCnt+1);
 
	return (1);
}

/*********************************************************************************************************
** ��������:  ZTM_SetFilterColor
** ��������:  ���ù���ɫ
** �������:  usFilterColorLow ~ usFilterColorHigh  ������ɫ��Χ
** �������:  ��
** �� �� ֵ:  ��
*********************************************************************************************************/
void ZTM_SetFilterColor (INT16U usFilterColorLow, INT16U usFilterColorHigh)
{
	ZtmSendBuff[0] = 0xAA;					     /* ֡ͷ                     */                                                   
	ZtmSendBuff[1] = 0x10;
		
	ZtmSendBuff[2] = usFilterColorLow>>8;				 
	ZtmSendBuff[3] = usFilterColorLow;				 
	ZtmSendBuff[4] = usFilterColorHigh>>8;;			
	ZtmSendBuff[5] = usFilterColorHigh;			

	ZtmSendCnt = 5;                     /* ����֡β                     */
	ZtmSendBuff[++ZtmSendCnt] = 0xcc;
	ZtmSendBuff[++ZtmSendCnt] = 0x33;
	ZtmSendBuff[++ZtmSendCnt] = 0xc3;
	ZtmSendBuff[++ZtmSendCnt] = 0x3c;

	uartSendBuff(ZtmSendBuff,ZtmSendCnt+1);
}

/*
 *  �ı���ʾ
 */
/*********************************************************************************************************
** ��������:  ZTM_PutText
** ��������:  ָ������λ��(x, y)д���֡�ASCII���ַ���
** �������:  usX��          x����
**            usY��          y����
**            *pucStr��      �ַ���ָ��
**            ucType��       �ֿ�����
** �������:  ��
** �� �� ֵ:  ��ȷ����1�����򷵻�0
*********************************************************************************************************/
INT8U ZTM_PutText (INT16U usX, INT16U usY, INT8U *pucStr, INT8U ucType)
{
	INT16U i,j;
    /*
     *  �������
     */
    if ((usX >= ZTM_XADDR_MAX) || (usY  >= ZTM_YADDR_MAX)) {
        return (0);
    }

	ZtmSendBuff[0] = 0xAA;					     /* ֡ͷ                     */                                                   
	
	switch (ucType) /* �ֿ�����ѡ��                 */
	{                                                   
		case ASC_6x8:
			ZtmSendBuff[1] = 0x53;                                               /* 6*8����ASCII����ʾ           */
			break;

		case GBK_12x12:                                                 /* 12*12�����ַ�����ʾ          */
    	ZtmSendBuff[1] = 0x6E;
			break;

		case GBK_16x16:                                                 /* 16*16�����ַ�����ʾ          */
			ZtmSendBuff[1] = 0x54;
			break;

		case GBK2132_24x24:                                             /* 24*24�����ַ�����ʾ          */
			ZtmSendBuff[1] = 0x6F;
			break;

		case GBK2132_32x32:                                             /* 32*32�����ַ�����ʾ          */
			ZtmSendBuff[1] = 0x55;
			break;

			default:
			break;
	}

	ZtmSendBuff[2] = usX>>8; 		/* ����x����                    */
	ZtmSendBuff[3] = usX;
	ZtmSendBuff[4] = usY>>8; 		/* ����y����                    */
	ZtmSendBuff[5] = usY;
  
	j = strlen((char *)pucStr);	/* ������ʾ�ַ�                 */
	for(i=0;i<j;i++)
	{
		ZtmSendBuff[6+i] = pucStr[i];
	}		

	ZtmSendCnt = 5+i;         	/* ����֡β                     */
	ZtmSendBuff[++ZtmSendCnt] = 0xcc;
	ZtmSendBuff[++ZtmSendCnt] = 0x33;
	ZtmSendBuff[++ZtmSendCnt] = 0xc3;
	ZtmSendBuff[++ZtmSendCnt] = 0x3c;
		
	uartSendBuff(ZtmSendBuff,ZtmSendCnt+1);
		
	return (1);
}

/*
 *  �ı���ʾ
 */
/*********************************************************************************************************
** ��������:  ZTM_PutText
** ��������:  ָ������λ��(x, y)д���֡�ASCII���ַ���
** �������:  usX��          x����
**            usY��          y����
**            *pucStr��      �ַ���ָ��
**            ucType��       �ֿ�����
** �������:  ��
** �� �� ֵ:  ��ȷ����1�����򷵻�0
*********************************************************************************************************/
INT8U TXM_StringDisplay(u16 tid, INT16U usX, INT16U usY,
      INT8U size,INT8U BCdis,INT16U usFcolor ,INT16U usBcolor, INT8U *pucStr)
{
	INT16U i,j;
  /*
  *  �������
  */
  if ((usX >= ZTM_XADDR_MAX) || (usY  >= ZTM_YADDR_MAX)) {
//    return (0);
  }
  
  if(tid)
  {  
    TXM_FillTextWidget (tid, (void*)pucStr);
  
    return(1);
  } 
  
	ZtmSendBuff[0] = 0xAA;	  /* ֡ͷ                     */                                                   
	ZtmSendBuff[1] = 0x98;    /* �û��ֿ�                 */

	ZtmSendBuff[2] = usX>>8; 		/* ����x����                    */
	ZtmSendBuff[3] = usX;
	ZtmSendBuff[4] = usY>>8; 		/* ����y����                    */
	ZtmSendBuff[5] = usY;
  
  switch(size)
  {  
    case 12:
      ZtmSendBuff[6] = 0x20;
      ZtmSendBuff[7] = 0x02;
      ZtmSendBuff[8] = 0x00;
      break;
    case 32:
      ZtmSendBuff[6] = 0x23;
      ZtmSendBuff[7] = 0x01;
      ZtmSendBuff[8] = 0x03;
      break;
    case 24:
      ZtmSendBuff[6] = 0x22;
      ZtmSendBuff[7] = 0x01;
      ZtmSendBuff[8] = 0x02;
      break;
    default: //16
      ZtmSendBuff[6] = 0x21;
      ZtmSendBuff[7] = 0x02;
      ZtmSendBuff[8] = 0x01;   
  }
  
  if(BCdis)
  {
    ZtmSendBuff[7] |= 0xd0;
  } 
  else
  {
    ZtmSendBuff[7] |= 0x90;
  }  
  
	ZtmSendBuff[9] = usFcolor>>8; 		/*                      */
	ZtmSendBuff[10] = usFcolor;
	ZtmSendBuff[11] = usBcolor>>8; 		/*                     */
	ZtmSendBuff[12] = usBcolor;
  
	j = strlen((char *)pucStr);	/* ������ʾ�ַ�                 */
	for(i=0;i<j;i++)
	{
		ZtmSendBuff[13+i] = pucStr[i];
	}		

	ZtmSendCnt = 12+i;         	/* ����֡β                     */
	ZtmSendBuff[++ZtmSendCnt] = 0xcc;
	ZtmSendBuff[++ZtmSendCnt] = 0x33;
	ZtmSendBuff[++ZtmSendCnt] = 0xc3;
	ZtmSendBuff[++ZtmSendCnt] = 0x3c;
		
	uartSendBuff(ZtmSendBuff,ZtmSendCnt+1);
		
	return (1);
}

 

INT8U TXM_DigitDisplay(u16 tid,u16 x,u16 y,u8 size,u8 BCdis,u16 usFcolor,u16 usBcolor,u32 dat,u8 bits,u8 dot,u8 align)
{
  u8 a[20]={0,0,0,0,0,0,0,0,0,0};
	INT16U i,j;
    
	for(i=0;i<bits;i++)
	{		
		a[bits-i-1] = (dat%10) + 0x30;
		dat = dat/10; 
	}
  
  if(dot)
  {
    for(i=0;i<dot;i++)
    {
      a[bits-i] = a[bits-i-1]; 
    }  
    
    a[bits-dot] = '.';
    bits++;
  }  
  
  if(align != ALIGN_ZERO_RIGHT)
  {  
    for(i=0;i<bits-1;i++)
    {
      if((a[i]==0x30) && (a[i+1] != '.'))
      {
        a[i] = 0x20;
      } 
      else
      {
        break;
      }  
    }  
    if(align == ALIGN_LEFT) 
    {
      for(i=0;i<bits-1;i++)
      {
        if(a[0]==0x20)
        {
          //����һλ
          for(j=0;j<bits-1;j++)
          {
            a[j] = a[j+1];
          }    
          a[bits-1] = 0x20;
        }  
        else
        {
          break;
        }  
      }
    }  
  }

  if(tid)
  {  
    TXM_FillTextWidget (tid, (void*)a);
  }
  else
  {  
    TXM_StringDisplay (0,x, y, size, BCdis, usFcolor, usBcolor, a);
  }
  
  return (1);
}

/*
 *  �ı��ؼ�
 */
/*********************************************************************************************************
** ��������:  TXM_FillTextWidget
** ��������:  ָ������λ��(x, y)д���֡�ASCII���ַ���
** �������:  usX��          x����
**            usY��          y����
**            *pucStr��      �ַ���ָ��
**            ucType��       �ֿ�����
** �������:  ��
** �� �� ֵ:  ��ȷ����1�����򷵻�0
*********************************************************************************************************/
INT8U TXM_FillTextWidget (INT16U Tid, char *pucStr)
{
	INT16U i,j;
  /*
  *  �������
  */
 // if ((usX >= ZTM_XADDR_MAX) || (usY  >= ZTM_YADDR_MAX)) {
//    return (0);
//  }

	ZtmSendBuff[0] = 0xAA;	  /* ֡ͷ                     */                                                   
	ZtmSendBuff[1] = 0x97;    /* ������                   */

	ZtmSendBuff[2] = Tid>>8; 		 
	ZtmSendBuff[3] = Tid;
  
	j = strlen((char *)pucStr);	/* ������ʾ�ַ�                 */
  
  if(j>50)
  {
    while(1);
  }  
  
	for(i=0;i<j;i++)
	{
		ZtmSendBuff[4+i] = pucStr[i];
	}		
	
	ZtmSendCnt = 3+i;         	/* ����֡β                     */
	ZtmSendBuff[++ZtmSendCnt] = 0xcc;
	ZtmSendBuff[++ZtmSendCnt] = 0x33;
	ZtmSendBuff[++ZtmSendCnt] = 0xc3;
	ZtmSendBuff[++ZtmSendCnt] = 0x3c;
		
	uartSendBuff(ZtmSendBuff,ZtmSendCnt+1);
		
	return (1);
}



/*
 *  ������ʾ
 */
/*********************************************************************************************************
** ��������:  TXM_DigitDisplay
** ��������:  ָ������λ��(x, y)д���֡�ASCII���ַ���
** �������:  usX��          x����
**            usY��          y����
**            *pucStr��      �ַ���ָ��
**            ucType��       �ֿ�����
** �������:  ��
** �� �� ֵ:  ��ȷ����1�����򷵻�0
*********************************************************************************************************/
INT8U TXM_DigitDisplayWidget (INT16U Tid, INT16U len, INT32U dat)
{
	INT16U i,j;
	
	INT8U a[10]={0,0,0,0,0,0,0,0,0,0};
	
	for(i=0;i<len;i++)
	{		
		a[8-i] = dat%10;
		dat = dat/10;
	}
   
	j=8;
	for(i=9-len;i<8;i++)
	{		
		if(a[i] != 0)
		{
			j = i;
			break;
		}	
	}
	
	for(;i<9;i++)
	{		
		a[i] += 0x30;
	}
	
	
	
	TXM_FillTextWidget (Tid, (void*)&a[j]);
		
	return (1);
}



/*
 *  �õ�
 */
/*********************************************************************************************************
** ��������:  ZTM_PutDot
** ��������:  ָ��λ�ú���ɫ����
** �������:  usxAddr��      x������
**            usyaddr��      y������
**            usColor��      �������ɫ
** �������:   ��
** �� �� ֵ:      ��ȷ����1�����򷵻�0
*********************************************************************************************************/
INT8U ZTM_PutDot (INT16U usxAddr, INT16U usyAddr, INT16U usColor)
{
    /*
     *  �������
     */
    if ((usxAddr >= ZTM_XADDR_MAX) || (usyAddr  >= ZTM_YADDR_MAX)) {
        return (0);
    }

    __SendHead();                                                       /* ����֡ͷ                     */

    __SendByte(0x4F);                                                   /* ��������                     */
    __SendDByte(usColor);                                               /* ������ɫֵ                   */
    __SendDByte(usxAddr);                                               /* ����x����                    */
    __SendDByte(usyAddr);                                               /* ����y����                    */

    __SendTail();                                                       /* ����֡β                     */
    return (1);
}

/*********************************************************************************************************
** ��������:  ZTM_PutNDot
** ��������:  ָ��λ�ú���ɫ�������
** �������:  *pusStr��      ������ָ��
**            ulNum��        �����
**            usColor��      �������ɫ
** �������:  ��
** �� �� ֵ:  ��ȷ����1�����򷵻�0
*********************************************************************************************************/
void ZTM_PutNDot (INT16U *pusStr, INT32U ulNum, INT16U usColor)
{
    __SendHead();                                                       /* ����֡ͷ                     */

    __SendByte(0x4F);                                                   /* ��������                     */
    __SendDByte(usColor);                                               /* ������ɫֵ                   */
    __SendDByteStr(pusStr, ulNum * 2);                                  /* ���͵�����                   */

    __SendTail();                                                       /* ����֡β                     */
}

/*********************************************************************************************************
** ��������:  ZTM_BColorPutNDot
** ��������:  ����ɫ�������(ɾ����)
** �������:  *pusStr��      ������ָ��
**            ulNum��        �����
** �������:  ��
** �� �� ֵ:  ��ȷ����1�����򷵻�0
*********************************************************************************************************/
void ZTM_BColorPutNDot (INT16U *pusStr, INT32U ulNum)
{
    __SendHead();                                                       /* ����֡ͷ                     */

    __SendByte(0x50);                                                   /* ��������                     */
    __SendDByteStr(pusStr, ulNum * 2);                                  /* ���͵�����                   */

    __SendTail();                                                       /* ����֡β                     */
}

/*********************************************************************************************************
** ��������:  ZTM_FColorPutNDot
** ��������:  ǰ��ɫ�������
** �������:  *pusStr��      ������ָ��
**            ulNum��        �����
** �������:  ��
** �� �� ֵ:  ��ȷ����1�����򷵻�0
*********************************************************************************************************/
void ZTM_FColorPutNDot (INT16U *pusStr, INT32U ulNum)
{
    __SendHead();                                                       /* ����֡ͷ                     */

    __SendByte(0x51);                                                   /* ��������                     */
    __SendDByteStr(pusStr, ulNum * 2);                                  /* ���͵�����                   */

    __SendTail();                                                       /* ����֡β                     */
}


/*
 *  ���߶κͶ����
 */
/*********************************************************************************************************
** ��������:  ZTM_FColorPutLine
** ��������:  ָ������(x1, y1)��(x2, y2)��ǰ��ɫ����
** �������:  usX1��         x1����
**            usY1��         y1����
**            usX2��         x2����
**            usY2��         y2����
** �������:  ��
** �� �� ֵ:  ��ȷ����1�����򷵻�0
*********************************************************************************************************/
INT8U ZTM_FColorPutLine (INT16U usX1, INT16U usY1, 
			             INT16U usX2, INT16U usY2)
{
    /*
     *  �������
     */
    if ((usX1 > ZTM_XADDR_MAX) || (usX2 > ZTM_XADDR_MAX) || (usY1 > ZTM_YADDR_MAX) ||
        (usY2 > ZTM_YADDR_MAX) ) {
            return (0);
        }
    __SendHead();                                                       /* ����֡ͷ                     */

    __SendByte(0x56);                                                   /* ��������                     */
    __SendDByte(usX1);                                                  /* ���͵�����                   */
    __SendDByte(usY1);
    __SendDByte(usX2);
    __SendDByte(usY2);

    __SendTail();                                                       /* ����֡β                     */
    return (1);
}

/*********************************************************************************************************
** ��������:  ZTM_BColorPutLine
** ��������:  ָ������(x1, y1)��(x2, y2)�ñ���ɫ����
** �������:  usX1��         x1����
**            usY1��         y1����
**            usX2��         x2����
**            usY2��         y2����
** �������:  ��
** �� �� ֵ:  ��ȷ����1�����򷵻�0
*********************************************************************************************************/
INT8U ZTM_BColorPutLine (INT16U usX1, INT16U usY1, 
			             INT16U usX2, INT16U usY2)
{
    /*
     *  �������
     */
    if ((usX1 > ZTM_XADDR_MAX) || (usX2 > ZTM_XADDR_MAX) || (usY1 > ZTM_YADDR_MAX) ||
        (usY2 > ZTM_YADDR_MAX) ) {
            return (0);
        }
    __SendHead();                                                       /* ����֡ͷ                     */

    __SendByte(0x6D);                                                   /* ��������                     */
    __SendDByte(usX1);                                                  /* ���͵�����                   */
    __SendDByte(usY1);
    __SendDByte(usX2);
    __SendDByte(usY2);

    __SendTail();                                                       /* ����֡β                     */
    return (1);
}

/*********************************************************************************************************
** ��������:  ZTM_PutSpectral
** ��������:  ǰ��ɫ������ʾƵ��
** �������:  usX��usY:      �������
**            usHeightMax:   Ƶ������ֵ
**            *pusStr:       usHeight0��n����Ƶ��ͼX�᷽�����ص�����Ӧ��Ƶ�׷�ֵ
**            ulNum��        Ƶ��������
** ��������� ��
** �� �� ֵ:  ��
*********************************************************************************************************/
void ZTM_PutSpectral (INT16U usX, INT16U usY, 
		              INT16U usHeightMax, INT16U *pusStr, 
		              INT32U ulNum)
{
    __SendHead();                                                       /* ����֡ͷ                     */

    __SendByte(0x75);                                                   /* ��������                     */
    __SendDByte(usX);                                                   /* �������X����                */
    __SendDByte(usY);                                                   /* �������Y����                */
    __SendDByte(usHeightMax);                                           /* ����Ƶ������ֵ             */
    __SendDByteStr(pusStr, ulNum);

    __SendTail();                                                       /* ����֡β                     */

}

/*********************************************************************************************************
** ��������:  ZTM_PutFoldline
** ��������:  ǰ��ɫ������ʾ����ͼ
** �������:  usX��          �������
**            ucXdis��       X�Ჽ��������
**            *pusStr��      Y0��Yn��������ͼyֵָ��
**            ulNum��        ���߹յ���
** ��������� ��
** �� �� ֵ:  ��ȷ����1�����򷵻�0
*********************************************************************************************************/
INT8U ZTM_PutFoldline (INT16U usX, INT8U ucXdis, 
		               INT16U *pusStr, INT32U ulNum)
{
    /*
     *  �������
     */
/*	
    if ((usX >= ZTM_XADDR_MAX) || (ucXdis >= ZTM_XADDR_MAX)) {
        return (0);
    }
*/	
    __SendHead();                                                       /* ����֡ͷ                     */

    __SendByte(0x76);                                                   /* ��������                     */
    __SendDByte(usX);                                                   /* �������x����                */
    __SendByte(ucXdis);                                                 /* ����x���򲽾�                */
    __SendDByteStr(pusStr, ulNum);                                      /* ����ͼy����ֵ                */

    __SendTail();                                                       /* ����֡β                     */

    return (1);
}

/*
 *  Բ����Բ��
 */
/*********************************************************************************************************
** ��������:  ZTM_Circle
** ��������:  ��Բ
** �������:  ucType��       0x00:ǰ��ɫ��ɫ��ʾԲ��
**                           0x01:ǰ��ɫ��ʾԲ��
**                           0x02:ǰ��ɫ��ɫ��ʾԲ��
**                           0x03:ǰ��ɫ��ʾԲ��
**            usx0Addr��     Բ��x0����
**            usy0Addr��     Բ��y0����
**            usR��          �뾶
** �������:  ��
** �� �� ֵ:  ��ȷ����1�����򷵻�0
*********************************************************************************************************/
INT8U ZTM_Circle (INT8U ucType, INT16U usx0Addr, 
	              INT16U usy0Addr, INT16U usR)
{
    if ((ucType > 1) || (usx0Addr >= ZTM_XADDR_MAX) || (usy0Addr  >= ZTM_YADDR_MAX)
    || ((usR * 2)  >= ZTM_YADDR_MAX)) {
        return (0);
    }
    __SendHead();                                                       /* ����֡ͷ                     */

    __SendByte(0x57);                                                   /* ��������                     */
    __SendByte(ucType);                                                 /* ָ��type                     */
    __SendDByte(usx0Addr);                                              /* ��������                     */
    __SendDByte(usy0Addr);
    __SendDByte(usR);                                                   /* ����ֱ��                     */

    __SendTail();                                                       /* ����֡β                     */
    return (1);
}


/*
 * ����Բ
 */
/*********************************************************************************************************
** ��������:  ZTM_EllipseFill
** ��������:  ��ʵ����Բ
** �������:  usx0Addr��     Բ��x0����
**            usy0Addr��     Բ��y0����
**            usRL��         ����
**            usRS��         ����
** �������:  ��
** �� �� ֵ:  ��ȷ����1�����򷵻�0
*********************************************************************************************************/
INT8U ZTM_EllipseFill (INT16U usx0Addr, INT16U usy0Addr, INT16U usRL, INT16U usRS)
{
    if ((usx0Addr >= ZTM_XADDR_MAX) || (usy0Addr  >= ZTM_YADDR_MAX)
    || ((usRL * 2)  >= ZTM_XADDR_MAX) || ((usRS * 2)  >= ZTM_YADDR_MAX)) {
//        return (0);
    }
    __SendHead();                                                       /* ����֡ͷ                     */

    __SendByte(0x60);                                                   /* ��������                     */
    __SendDByte(usx0Addr);                                              /* ��������                     */
    __SendDByte(usy0Addr);
    __SendDByte(usRL);                                                  /* ���ͳ�����                   */
    __SendDByte(usRS);

    __SendTail();                                                       /* ����֡β                     */
    return (1);
}

/*********************************************************************************************************
** ��������:  ZTM_Ellipse
** ��������:  ��������Բ
** �������:  usx0Addr��     Բ��x0����
**            usy0Addr��     Բ��y0����
**            usRL��         ����
**            usRS��         ����
** �������:  ��
** �� �� ֵ:  ��ȷ����1�����򷵻�0
*********************************************************************************************************/
INT8U ZTM_Ellipse (INT16U usx0Addr, INT16U usy0Addr, INT16U usRL, INT16U usRS)
{
    if ((usx0Addr >= ZTM_XADDR_MAX) || (usy0Addr  >= ZTM_YADDR_MAX)
    || ((usRL * 2)  >= ZTM_XADDR_MAX) || ((usRS * 2)  >= ZTM_YADDR_MAX)) {
        return (0);
    }
    __SendHead();                                                       /* ����֡ͷ                     */

    __SendByte(0x61);                                                   /* ��������                     */
    __SendDByte(usx0Addr);                                              /* ��������                     */
    __SendDByte(usy0Addr);
    __SendDByte(usRL);                                                  /* ���ͳ�����                   */
    __SendDByte(usRS);

    __SendTail();                                                       /* ����֡β                     */
    return (1);
}


/*
 *  ���ο�
 */
/*********************************************************************************************************
** ��������:  ZTM_FcolorRectangle
** ��������:  ǰ��ɫ��ʾ���ο�
** �������:  usx0Addr��     x0����
**            usy0Addr��     y0����
**            usx1Addr��     x1����
**            usy1Addr��     y1����
** �������:  ��
** �� �� ֵ:  ��ȷ����1�����򷵻�0
*********************************************************************************************************/
INT8U  ZTM_FcolorRectangle (INT16U usx0Addr, INT16U usy0Addr, 
		                    INT16U usx1Addr, INT16U usy1Addr)
{
    if ((usx0Addr >= ZTM_XADDR_MAX) || (usy0Addr  >= ZTM_YADDR_MAX)
    || (usx1Addr  >= ZTM_XADDR_MAX) || (usy1Addr  >= ZTM_YADDR_MAX)) {
        return (0);
    }
    __SendHead();                                                       /* ����֡ͷ                     */

    __SendByte(0x59);                                                   /* ��������                     */
    __SendDByte(usx0Addr);                                              /* ��������                     */
    __SendDByte(usy0Addr);
    __SendDByte(usx1Addr);
    __SendDByte(usy1Addr);

    __SendTail();                                                       /* ����֡β                     */

    return (1);
}

/*********************************************************************************************************
** ��������:  ZTM_Square
** ��������:  ��������
** �������:  usx0Addr��     x0����
**            usy0Addr��     y0����
**            usWidth��      �߳�
** �������:  ��
** �� �� ֵ:  ��
*********************************************************************************************************/
void ZTM_Square (INT16U usx0Addr, INT16U usy0Addr, INT16U usWidth)
{
    ZTM_FcolorRectangle(usx0Addr, usy0Addr, (usx0Addr + usWidth), (usy0Addr + usWidth));
}


/*********************************************************************************************************
** ��������:  ZTM_BcolorRectangle
** ��������:  ����ɫ��ʾ���ο�
** �������:  usx0Addr��     x0����
**            usy0Addr��     y0����
**            usx1Addr��     x1����
**            usy1Addr��     y1����
** �������:  ��
** �� �� ֵ:  ��ȷ����1�����򷵻�0
*********************************************************************************************************/
INT8U  ZTM_BcolorRectangle (INT16U usx0Addr, INT16U usy0Addr, 
	                        INT16U usx1Addr, INT16U usy1Addr)
{
    if ((usx0Addr >= ZTM_XADDR_MAX) || (usy0Addr  >= ZTM_YADDR_MAX)
    || (usx1Addr  >= ZTM_XADDR_MAX) || (usy1Addr  >= ZTM_YADDR_MAX)) {
        return (0);
    }
    __SendHead();                                                       /* ����֡ͷ                     */

    __SendByte(0x69);                                                   /* ��������                     */
    __SendDByte(usx0Addr);                                              /* ��������                     */
    __SendDByte(usy0Addr);
    __SendDByte(usx1Addr);
    __SendDByte(usy1Addr);

    __SendTail();                                                       /* ����֡β                     */

    return (1);
}

/*
 *  �������
 */
/*********************************************************************************************************
** ��������:  ZTM_RectangleFill
** ��������:  ָ���������
** �������:  usx0Addr:      x0����
**            usy0Addr��     y0����
**            usx1Addr��     x1����
**            usy1Addr��     y1����
**            uiColor:       �����ɫ
** �������:  ��
** �� �� ֵ:  ��ȷ����1�����򷵻�0
*********************************************************************************************************/
INT8U ZTM_RectangleFill (INT16U usx0Addr, INT16U usy0Addr, 
                         INT16U usx1Addr, INT16U usy1Addr, 
                         INT16U usColor)
{
   u16 y0,y1; 
   y0 = 239-usx1Addr;
   y1 = 239-usx0Addr;
   usx0Addr = usy1Addr;
   usx1Addr = usy0Addr;
   usy0Addr = y0;
   usy1Addr = y1;
   
    if ((usx0Addr >= ZTM_XADDR_MAX) || (usy0Addr  >= ZTM_YADDR_MAX)
    || (usx1Addr  >= ZTM_XADDR_MAX) || (usy1Addr  >= ZTM_YADDR_MAX)) {
        return (0);
    }
    __SendHead();                                                       /* ����֡ͷ                     */

    __SendByte(0x64);                                                   /* ��������                     */
    __SendDByte(usColor);                                               /* ָ����ɫ                     */
    __SendDByte(usx0Addr);                                              /* ��������                     */
    __SendDByte(usy0Addr);
    __SendDByte(usx1Addr);
    __SendDByte(usy1Addr);

    __SendTail();                                                       /* ����֡β                     */
    return (1);
}

/*********************************************************************************************************
** ��������:  ZTM_SquareFill
** ��������:  ���������
** �������:  usx0Addr��     x0����
**            usy0Addr��     y0����
**            usWidth��      �߳�
**            usColor��      �����ɫ
** �������:  ��
** �� �� ֵ:  ��
*********************************************************************************************************/
void ZTM_SquareFill (INT16U usx0Addr, INT16U usy0Addr, INT16U usWidth, INT16U usColor)
{
    ZTM_RectangleFill(usx0Addr, usy0Addr, (usx0Addr + usWidth), (usy0Addr + usWidth), usColor);
}

/*
 * ��������
 */
/*********************************************************************************************************
** ��������:  ZTM_ClrScreen
** ��������:  ��ָ����ɫ����
** �������:  usColor��      ������ɫ
** �������:  ��
** �� �� ֵ:  ��
*********************************************************************************************************/
void ZTM_ClrScreen (INT16U usColor)
{
    __SendHead();                                                       /* ����֡ͷ                     */

    __SendByte(0x52);                                                   /* ��������                     */
    __SendDByte(usColor);                                               /* ����������ɫֵ               */

    __SendTail();                                                       /* ����֡β                     */
}

/*
 * ͼƬͼ����ʾ
 */
/*********************************************************************************************************
** ��������:  ZTM_FullScreenImageDisp
** ��������:  ��ʾһ��ȫ��ͼ��
** �������:  usPictureID��  Flash��ͼƬ���0~(PICTURE_ID_MAX - 1)
** �������:  ��
** �� �� ֵ:  ��ȷ����1�����򷵻�0
*********************************************************************************************************/
INT8U ZTM_FullScreenImageDisp (INT16U usPictureID)
{
/*
	if (usPictureID >= PICTURE_ID_MAX) {
        return (0);
	}
*/	
	ZtmSendBuff[0] = 0xAA;					     /* ֡ͷ                     */                                                   
	ZtmSendBuff[1] = 0x70;
		
	ZtmSendBuff[2] = usPictureID>>8;
	ZtmSendBuff[3] = usPictureID;			
 
	ZtmSendCnt = 3;                     /* ����֡β                     */
	ZtmSendBuff[++ZtmSendCnt] = 0xcc;
	ZtmSendBuff[++ZtmSendCnt] = 0x33;
	ZtmSendBuff[++ZtmSendCnt] = 0xc3;
	ZtmSendBuff[++ZtmSendCnt] = 0x3c;

	uartSendBuff(ZtmSendBuff,ZtmSendCnt+1);
			
	return (1);
}

/*********************************************************************************************************
** ��������:  ZTM_PartImageDisp
** ��������:  ����ͼ���һ������ʾ
** �������:  usPictureID��  Flash��ͼƬ���0~(PICTURE_ID_MAX - 1)
**            usx0Addr��     ͼƬ���Ͻ�X����
**            usy0Addr��     ͼƬ���Ͻ�Y����
**            usx1Addr��     ͼƬ���½�X����
**            usy1Addr��     ͼƬ���½�Y����
**            usxAddrDisp��  ͼƬ��ʾλ�����Ͻ�����
**            usyAddrDisp��  ͼƬ��ʾλ�����Ͻ�����
**            ucType         0:��������ɫ
**                           1:������ɫ,͸��Ч��
** �������:  ��
** �� �� ֵ:  ��ȷ����1�����򷵻�0
*********************************************************************************************************/
INT8U ZTM_PartImageDisp (INT16U usPictureID, INT16U usx0Addr, 
                         INT16U usy0Addr, INT16U usx1Addr,
                         INT16U usy1Addr, INT16U usxAddrDisp, 
                         INT16U usyAddrDisp, INT8U ucType)
{
    /*
     * �������
     */
/*	
    if ((usPictureID >= PICTURE_ID_MAX)
    || (usx0Addr >= ZTM_XADDR_MAX) || (usy0Addr >= ZTM_YADDR_MAX)
    || (usx1Addr >= ZTM_XADDR_MAX) || (usy1Addr >= ZTM_YADDR_MAX)
    || (usxAddrDisp >= ZTM_XADDR_MAX) || (usyAddrDisp >= ZTM_YADDR_MAX)
    || (ucType >= 2)) {
        return (0);
    }
*/	
    __SendHead();                                                       /* ����֡ͷ                     */

    if (0 == ucType) {                                                  /* ��������                     */
        __SendByte(0x71);
    } else if (1 == ucType) {
        __SendByte(0x9C);
    }

    __SendDByte(usPictureID);                                           /* ����ͼƬ���                 */
    __SendDByte(usx0Addr);                                              /* ��������                     */
    __SendDByte(usy0Addr);
    __SendDByte(usx1Addr);
    __SendDByte(usy1Addr);
    __SendDByte(usxAddrDisp);
    __SendDByte(usyAddrDisp);

    __SendTail();                                                       /* ����֡β                     */
    return (1);
}

/*********************************************************************************************************
** ��������:  ZTM_ImageDisp
** ��������:  ָ��λ����ʾͼƬ
** �������:  usPictureID:   Flash��ͼƬ���0~(PICTURE_ID_MAX - 1)
**            usxAddrDisp:   ͼƬ��ʾλ�����Ͻ�X����
**            usyAddrDisp��  ͼƬ��ʾλ�����Ͻ�Y����
**            ucType��       0:��������ɫ
**                           1:������ɫ,͸��Ч��
** �������:  ��
** �� �� ֵ:  ��ȷ����1�����򷵻�0
*********************************************************************************************************/
INT8U ZTM_ImageDisp (INT8U usPictureID,INT16U usxAddrDisp,
                     INT16U usyAddrDisp,INT8U ucType)
{
    /*
     * �������
	
    if ((usPictureID >= PICTURE_ID_MAX)
    || (usxAddrDisp >= ZTM_XADDR_MAX)
    || (usyAddrDisp >= ZTM_YADDR_MAX)
    || (ucType >= 2)) {
        return (0);
    }
     */
    __SendHead();                                                       /* ����֡ͷ                     */

    if (0 == ucType) {                                                  /* ��������                     */
        __SendByte(0x9D);
    } else if (1 == ucType) {
        __SendByte(0x9E);
    }
    __SendDByte(usPictureID);
    __SendDByte(usxAddrDisp);                                           /* ��������                     */
    __SendDByte(usyAddrDisp);
    __SendTail();                                                       /* ����֡β                     */

    return (1);
}

/*
 * ��������
 */
/*********************************************************************************************************
** ��������:  ZTM_Cartoon
** ��������:  ��Ԥ��ʱ����ж�����ʾ
** �������:  *pusPictureID��Flash��ͼƬ���0~(PICTURE_ID_MAX - 1)
**            usNum��        ������ʾͼƬ����
** ��������� ��
** �� �� ֵ:  ��ȷ����1�����򷵻�0
*********************************************************************************************************/
INT8U ZTM_Cartoon (INT16U *pusPictureID, INT16U usNum)
{
    /*
     * �������
     */
/*	
    if (usNum >= PICTURE_ID_MAX) {
        return (0);
    }
*/	
    __SendHead();                                                       /* ����֡ͷ                     */

    __SendByte(0x9A);
    __SendDByteStr(pusPictureID, usNum);
    __SendTail();                                                       /* ����֡β                     */

    return (1);
}

/*********************************************************************************************************
** ��������:  ZTM_SetCartoon
** ��������:  ָ���������ж�����ʾ
** �������:  ucTime��            ȡֵΪ 0��255����ʾÿ��ͼƬ���ŵ�ʱ�䳤�ȣ���λΪ 100���롣
**            ucType��            0x00����ԭͼ��ʾ����������ɫ���ˣ�
**                                0x01��������ɫ��ʾ������Ϊ usColorPicId ͼƬ��
**                                0x02��������ɫ��ʾ������Ϊ��� usColorPicId ��ɫ
**                                0x04��������ɫ��ʾ��ֱ��ǰ����ʾ��
**            usColorOrPicId��    ���������ɫ��16 λɫ���򱳾�ͼƬ ID��
**            usFilterColorLow��  ��ɫ���˵���ֵ������ʾ����Ϊ 00 ʱ���˲�����Ч��
**            usFilterColorHight����ɫ���˸���ֵ������ʾ����Ϊ 00 ʱ���˲�����Ч��
**            usX��               ������ʾ����ʼ X���ꣻ
**            usY��               ������ʾ����ʼ Y���ꣻ
**            pusPictureID��      Flash��ͼƬ���0~(PICTURE_ID_MAX - 1)
**            usNum��             ������ʾͼƬ����
** ��������� ��
** �� �� ֵ:  ��ȷ����1�����򷵻�0
*********************************************************************************************************/
INT8U ZTM_SetCartoon (INT8U ucTime, INT8U ucType, INT16U usColorOrPicId,
                      INT16U usFilterColorLow, INT16U usFilterColorHight,
                      INT16U usX, INT16U usY,
                      INT16U *pusPictureID, INT16U usNum)
{
    /*
     * �������
     */
/*	
    if ((ucTime > LIGHT_TIME_MAX) || (ucType > 4)
     || (usX > ZTM_XADDR_MAX) || (usY > ZTM_YADDR_MAX)
     || (usNum > PICTURE_ID_MAX)) {
         return (0);
     }
	*/
	
    __SendHead();                                                       /* ����֡ͷ                     */

    __SendByte(0x9F);
    __SendByte(ucTime);                                                 /* ����ÿ�Ų��ŵ�ʱ�䳤��       */
    __SendByte(ucType);                                                 /* ���͹���ɫ����               */
    __SendDByte(usColorOrPicId);                                        /* ����ͼƬID                   */
    __SendDByte(usFilterColorLow);                                      /* ��ɫ���˵���ֵ               */
    __SendDByte(usFilterColorHight);                                    /* ��ɫ���˸���ֵ               */
    __SendDByte(usX);                                                   /* ���Ͷ�����ʾX����            */
    __SendDByte(usY);                                                   /* ���Ͷ�����ʾY����            */
    __SendDByteStr(pusPictureID, usNum);
    __SendTail();                                                       /* ����֡β                     */

    return (1);
}

/*
 * ����������
 */
 
/*********************************************************************************************************
** ��������:  ZTM_TouchCalibration
** ��������:  ������У׼(ע��ֻ�е�һ��ʹ�ô�����ʱ��ҪУ׼)
** �������:  ��
** �������:  ��
** �� �� ֵ:  ��
*********************************************************************************************************/
void ZTM_TouchCalibration (void)
{
    __SendHead();                                                       /* ����֡ͷ                     */

    __SendByteStr(__GucFrameTouchCal, 5);                               /* ��������                     */

    __SendTail();                                                       /* ����֡β                     */
}

/*********************************************************************************************************
** ��������:  ZTM_CheckTouchPos
** ��������:  ������ѯ�����ʵ�ǰλ��
** �������:  ��
** �������:  ��
** �� �� ֵ:  ��
** ˵    ���� 2012-02-22
*********************************************************************************************************/
void ZTM_CheckTouchPos (void)
{

    __SendHead();                                                       /* ����֡ͷ                     */

    __SendByte(0x73);                                                   /* ����������                   */

    __SendTail();                                                       /* ����֡β                     */
}

/*
 * ����������
 */
/*********************************************************************************************************
** ��������:  ZTM_SetBuzzer
** ��������:  ����������
** �������:  ucTime �������������ʱ�䳤�ȣ���λ��10ms��ֵΪ0~255��
** �������:  ��
** �� �� ֵ:  ��
*********************************************************************************************************/
void ZTM_SetBuzzer (INT8U ucTime)
{
    __SendHead();                                                       /* ����֡ͷ                     */

    __SendByte(0x79);                                                   /* ����ָ����                   */
    __SendByte(ucTime);                                                 /* ����ʱ��ֵ                   */
	
    __SendTail();                                                       /* ����֡β                     */
}

/*
 * �������
 */
/*********************************************************************************************************
** ��������:  ZTM_LightOnOff
** ��������:  �������
** �������:  ucOnOff��      0:  �ر���
**                           1:  ������
** �������:  ��
** �� �� ֵ:  ��ȷ����1�����򷵻�0
*********************************************************************************************************/
INT8U ZTM_LightOnOff (INT8U ucOnOff)
{
    if (ucOnOff > 1) {
        return (0);
    }
    __SendHead();                                                       /* ����֡ͷ                     */
    if (0 == ucOnOff)     {
        __SendByte(0x5E);                                               /* ���ͱ��⿪����               */
    } else if (1 == ucOnOff) {
        __SendByte(0x5F);                                               /* ���ͱ��������               */
    }
    __SendTail();                                                       /* ����֡β                     */
    return (1);
}

/*********************************************************************************************************
** ��������:  ZTM_LightTimeTouch
** ��������:  ��������
** �������:  ucPWMOn ��������Чʱ������
**            ucPWMOff��������Чʱ������
**            ucOnTime: ������Чʱ�䣬��λΪ��
** �������:  ��
** �� �� ֵ:  ��ȷ����1�����򷵻�0
*********************************************************************************************************/
INT8U ZTM_LightTimeTouch (INT8U ucPWMOn, INT8U ucPWMOff, INT8U ucOnTime)
{
	/*
     *  �������
     */
    if ((ucPWMOn > LIGHT_PWMLEVEL_MAX) || 
	    (ucPWMOff > LIGHT_PWMLEVEL_MAX)) {
        return (0);
    }
	
    __SendHead();                                                       /* ����֡ͷ                     */

    __SendByte(0x5E);                                                   /* ��������                     */
    __SendByteStr(__GucFrameLight, 4);                                  /* ���ͼ��ر�������             */
	__SendByte(ucPWMOn);                                                /* ���ͱ�����Чʱ������         */
	__SendByte(ucPWMOff);                                               /* ���ͱ�����Чʱ������         */
    __SendByte(ucOnTime);                                               /* ���ͱ�����Чʱ��             */
	
    __SendTail();                                                       /* ����֡β                     */

    return (1);
}

/*********************************************************************************************************
** ��������:  ZTM_LightLevel
** ��������:  �������ȵȼ�
** �������:  ucPWMLevel��   0-100
** �������:  ��
** �� �� ֵ:  ��ȷ����1�����򷵻�0
*********************************************************************************************************/
INT8U ZTM_LightLevel (INT8U ucPWMLevel)
{
    if (ucPWMLevel >= LIGHT_PWMLEVEL_MAX) {
        return (0);
    }
    __SendHead();                                                       /* ����֡ͷ                     */

    __SendByte(0x5F);
    __SendByte(ucPWMLevel);
    __SendTail();                                                       /* ����֡β                     */

    return (1);
}

/*
 * ��������
 */
/*********************************************************************************************************
** ��������:  ZTM_SetPara
** ��������:  ��������
** �������:  ulBaud��             ���ڲ����ʣ�             1200-3152000bps
**            ucTouchType��        �����������ϴ���ʽ:      0x72��0x73
**            ucBackLightType��    �������ģʽ:            0x5E��0x5F
**            ucTxtBackgroundType���ı�������ʾģʽ         0:�ı�����ɫΪϵͳ����ɫ(Ĭ��)
**                                                          1:�ı�����ɫΪ͸��
**            INT8U ucMouseType��  ���������ʱ�����ʾģʽ 0:����ʾ���(Ĭ��)
**                                                          :��ʾ���
** �������:  ��
** �� �� ֵ:  ��ȷ����1�����򷵻�0
*********************************************************************************************************/
INT8U ZTM_SetPara (INT32U ulBaud, INT8U ucTouchType, INT8U ucBackLightType,
                   INT8U ucTxtBackgroundType, INT8U ucMouseType)
{
    if (((ulBaud < 1200) && (ulBaud > 3152000))
    || (ucTxtBackgroundType > 1)
    || (ucMouseType > 1)) {
        return (0);
    }
    __SendHead();                                                       /* ����֡ͷ                     */


    __SendByte(0xE0);
                                                                        /* 1.���Ͳ�����ֵ               */
    __SendByte((ulBaud >> 24) & 0xFF);
    __SendByte((ulBaud >> 16) & 0xFF);
    __SendByte((ulBaud >> 8) & 0xFF);
    __SendByte(ulBaud & 0xFF);
                                                                        /* 2.���ʹ����������ϴ���ʽ     */
    __SendByte(ucTouchType);
                                                                        /* 3.���ͱ������ģʽ           */
    __SendByte(ucBackLightType);
                                                                        /* 4.�����ı�������ʾģʽ       */
    __SendByte(ucTxtBackgroundType);
                                                                        /* 5.���������ʾģʽ           */
    __SendByte(ucMouseType);

    __SendTail();                                                       /* ����֡β                     */

    return (1);
}

/*********************************************************************************************************
** ��������:  ZTM_SetBaudrate
** ��������:  ����ͨ�Ų�����
** �������:  ulBaud�����ڲ����ʣ�1200-3152000bps
** �������:  ��
** �� �� ֵ:  ��ȷ����1�����򷵻�0
*********************************************************************************************************/
INT8U ZTM_SetBaudrate (INT32U ulBaud)
{
	/*
     *  �������
     */
	if ((ulBaud < 1200) || (ulBaud > 3152000)) {
		return (0);
	}
	
	__SendHead();                                                       /* ����֡ͷ                     */

    __SendDByte(0xE000);                                                /* ����ָ����                   */                                                                   
    __SendByte((ulBaud >> 24) & 0xFF);                                  /* ���Ͳ�����ֵ                 */
    __SendByte((ulBaud >> 16) & 0xFF);
    __SendByte((ulBaud >> 8) & 0xFF);
    __SendByte(ulBaud & 0xFF);

    __SendTail();                                                       /* ����֡β                     */
	
	return (1);
}

/*********************************************************************************************************
** ��������:  ZTM_SetDisplayBuf
** ��������:  ������ʾ��������
** �������:  ucDisplayBuf��Ŀ����ʾ��������ֵ
** �������:  ��
** �� �� ֵ:  ��
*********************************************************************************************************/
void ZTM_SetDisplayBuf (INT8U ucDisplayBuf)
{
	/*
     *  ��������
     */
	ucDisplayBuf &= 0xF0;                                               /* �ض���Ϊ����λ�ĵ�4λ        */
	
	__SendHead();                                                       /* ����֡ͷ                     */

    __SendDByte(0xE001);                                                /* ����ָ����                   */                                                                   
    __SendByte(ucDisplayBuf);                                           /* ����Ŀ����ʾ��������ֵ       */

    __SendTail();                                                       /* ����֡β                     */
}

/*********************************************************************************************************
** ��������:  ZTM_SetAck
** ��������:  ���ô�����Ӧ����
** �������:  ucAckEn��Ӧ��������ֵ��0������������Ӧ��1����ʹ������Ӧ��
** �������:  ��
** �� �� ֵ:  ��ȷ����1�����򷵻�0
*********************************************************************************************************/
INT8U ZTM_SetAck (INT8U ucAckEn)
{
    /*
     *  �������
     */
	if (ucAckEn > 1) {
	
		return (0);
    }
	
	__SendHead();                                                       /* ����֡ͷ                     */

    __SendDByte(0xE002);                                                /* ����ָ����                   */                                                                   
    __SendByte(ucAckEn);                                                /* ����Ӧ��������ֵ           */

    __SendTail();                                                       /* ����֡β                     */
	
	return (1);
}

/*********************************************************************************************************
** ��������:  ZTM_SetTouchType
** ��������:  ���ô�������Ӧ����
** �������:  ucTouchType����������Ӧ����ֵ��0x72�����Ǵ���ģʽ�£�������̧��ʱ���ϴ���������
**                                           0x73�����Ǵ���ģʽ�£������ʰ���ʱ���ϴ���������
**                                           0x74�����Ǵ���ģʽ�£��������ƶ�ʱ���ϴ���������
**                                           0x78��������ģʽ�£�������̧��ʱ���ϴ�Ԥ�败�ؼ���
**                                           0x79��������ģʽ�£������ʰ���ʱ���ϴ�Ԥ�败�ؼ���
**                                           0x7A��������ģʽ�£������ʰ��º�̧��ʱ���ϴ�Ԥ�败�ؼ���
** �������:  ��
** �� �� ֵ:  ��ȷ����1�����򷵻�0
*********************************************************************************************************/
INT8U ZTM_SetTouchType(INT8U ucTouchType)
{
	/*
     *  �������
     */
	if (!((ucTouchType == 0x72) ||
		  (ucTouchType == 0x73) ||
		  (ucTouchType == 0x74) ||
		  (ucTouchType == 0x78) ||
		  (ucTouchType == 0x79) ||
		  (ucTouchType == 0x7A))  ) {
	
		return (0);
    }
	
	__SendHead();                                                       /* ����֡ͷ                     */

    __SendDByte(0xE003);                                                /* ����ָ����                   */                                                                   
    __SendByte(ucTouchType);                                            /* ���ʹ�������Ӧ����ֵ         */

    __SendTail();                                                       /* ����֡β                     */
	
	return (1);
}

/*********************************************************************************************************
** ��������:  ZTM_SetBuzzerType
** ��������:  ���÷���������
** �������:  ucBuzzerType������������ֵ
** �������:  ��
** �� �� ֵ:  ��ȷ����1�����򷵻�0
*********************************************************************************************************/
INT8U ZTM_SetBuzzerType (INT8U ucBuzzerType)
{
	/*
     *  �������
     */
	if (ucBuzzerType > 1) {
	
		return (0);
    }
	
	__SendHead();                                                       /* ����֡ͷ                     */

    __SendDByte(0xE004);                                                /* ����ָ����                   */                                                                   
    __SendByte(ucBuzzerType);                                           /* ���ͷ���������ֵ             */

    __SendTail();                                                       /* ����֡β                     */
	
	return (1);
}

/*********************************************************************************************************
** ��������:  ZTM_SetTxtBackgroundType
** ��������:  �����ı��ؼ���ʾ��������
** �������:  ucTxtBackgroundType���ı���ʾ��������ֵ��0�����ı��ı���ɫΪϵͳ�ı���ɫ��Ĭ�ϣ�
**                                                     1�����ı��ı���ɫΪ͸��
** �������:  ��
** �� �� ֵ:  ��ȷ����1�����򷵻�0
*********************************************************************************************************/
INT8U ZTM_SetTxtBackgroundType(INT8U ucTxtBackgroundType)
{
	/*
     *  �������
     */
	if (ucTxtBackgroundType > 1) {
		return (0);
	}
	
	__SendHead();                                                       /* ����֡ͷ                     */
	
	__SendDByte(0xE005);                                                /* ����ָ����                   */
	__SendByte(ucTxtBackgroundType);                                    /* �����ı���ʾ��������ֵ       */
	
	__SendTail();                                                       /* ����֡β                     */
	
	return (1);
}

/*********************************************************************************************************
** ��������:  ZTM_SetSysMouseType
** ��������:  ���õ��������ʱϵͳ����Ƿ���ʾ
** �������:  ucSysMouseType��ָʾ���������ʱϵͳͼ���Ƿ���ʾ��0��������ʾ��Ĭ�ϣ�
**                                                              1������ʾ
** �������:  ��
** �� �� ֵ:  ��ȷ����1�����򷵻�0
*********************************************************************************************************/
INT8U ZTM_SetSysMouseType(INT8U ucSysMouseType)
{
	/*
     *  �������
     */
	if (ucSysMouseType > 1) {
		return (0);
	}
	
	__SendHead();                                                       /* ����֡ͷ                     */
	
	__SendDByte(0xE006);                                                /* ����ָ����                   */
	__SendByte(ucSysMouseType);                                         /* �����ı���ʾ��������ֵ       */
	
	__SendTail();                                                       /* ����֡β                     */
	
	return (1);
}

/*********************************************************************************************************
** ��������:  ZTM_SetSumCheckType
** ��������:  ����ͨ�ź�У������
** �������:  ucSumCheckype��ͨ�ź�У������ֵ��0������У�飨Ĭ�ϣ�
**                                             1����8λ��У��
**                                             2����16λ��У��
** �������:  ��
** �� �� ֵ:  ��ȷ����1�����򷵻�0
*********************************************************************************************************/
INT8U ZTM_SetSumCheckType (INT8U ucSumCheckType)
{
	/*
     *  �������
     */
	if (ucSumCheckType > 2) {
		return (0);
	}
	
	__SendHead();                                                       /* ����֡ͷ                     */
	
	__SendDByte(0xE007);                                                /* ����ָ����                   */
	__SendByte(ucSumCheckType);                                         /* ����ͨ�ź�У������ֵ         */
	
	__SendTail();                                                       /* ����֡β                     */
	
	return (1);
}

/*********************************************************************************************************
** ��������:  ZTM_SetTouchDelay
** ��������:  ���ô�����������Ӧʱ��
** �������:  ulTouchDly��������������Ӧʱ��
** �������:  ��
** �� �� ֵ:  ��ȷ����1�����򷵻�0
*********************************************************************************************************/
INT8U ZTM_SetTouchDelay (INT32U ulTouchDly)
{
	/*
     *  �������
     */

	
	__SendHead();                                                       /* ����֡ͷ                     */

    __SendDByte(0xE010);                                                /* ����ָ����                   */                                                                   
    __SendByte((ulTouchDly >> 24) & 0xFF);                              /* ���ʹ�����������Ӧʱ��       */
    __SendByte((ulTouchDly >> 16) & 0xFF);
    __SendByte((ulTouchDly >> 8) & 0xFF);
    __SendByte(ulTouchDly & 0xFF);

    __SendTail();                                                       /* ����֡β                     */
	
	return (1);
}

/*
 * ������Ϣ
 */
/*********************************************************************************************************
** ��������:  ZTM_ReadDeviceInfo
** ��������:  ���Ͷ�ȡ������������Ϣ
** �������:  ��
** �������:  ��
** �� �� ֵ:  ��
*********************************************************************************************************/
void ZTM_ReadDeviceInfo (void)
{
    __SendHead();                                                       /* ����֡ͷ                     */

    __SendByte(0xA0);                                                   /* ��������                     */

    __SendTail();                                                       /* ����֡β                     */
}

/*
 *  ֱ���Դ����
 */
/*********************************************************************************************************
** ��������:  ZTM_OptDisBufDirectly
** ��������:  ֱ���Դ����
** �������:  usX0         ������������ʼ��X���ꣻ
**            usY0         ������������ʼ��Y���ꣻ
**            usX1         ���������������X���ꣻ
**            usY1         ���������������Y���ꣻ
**            usXRef       �����������ڲο���X���ꣻ
**            usYRef       �����������ڲο���Y���ꣻ
**            ulNBytes     �������ɫֵ�ֽ�����
**            pucColorValue��������ɫֵ����
** �������:  ��
** �� �� ֵ:  �ɹ�����1�����򷵻�0
*********************************************************************************************************/
INT8U ZTM_OptDisBufDirectly (INT16U usX0, 
							 INT16U usY0,
							 INT16U usX1,
							 INT16U usY1,
							 INT16U usXRef,
							 INT16U usYRef,
							 INT32U ulNBytes,
							 INT8U *pucColorValue) {

	INT32U ulColorBytesIdx;
	
	/*
     *  �������
     */
	if ((usX0   >= ZTM_XADDR_MAX) ||
	    (usX1   >= ZTM_XADDR_MAX) ||
		(usXRef >= ZTM_XADDR_MAX) ||
		(usY0   >= ZTM_YADDR_MAX) ||
	    (usY1   >= ZTM_YADDR_MAX) ||
		(usYRef >= ZTM_YADDR_MAX)   ) {
		
		return (0);
	}
	
	__SendHead();                                                       /* ����֡ͷ                     */
	
	__SendByte(0xC3);                                                   /* ����ָ����                   */
	__SendDByte(usX0);                                                  /* ���Ͳ���������ʼ��X����      */
	__SendDByte(usY0);                                                  /* ���Ͳ���������ʼ��Y����      */
	__SendDByte(usX1);                                                  /* ���Ͳ������������X����      */
    __SendDByte(usY1);                                                  /* ���Ͳ������������X����      */
	__SendDByte(usXRef);                                                /* ���Ͳ��������ڲο���X����    */
	__SendDByte(usYRef);                                                /* ���Ͳ��������ڲο���Y����    */
	__SendByte((ulNBytes >> 24) & 0xFF);                                /* ���������ɫֵ�ֽ���         */
    __SendByte((ulNBytes >> 16) & 0xFF);
    __SendByte((ulNBytes >> 8) & 0xFF);
    __SendByte(ulNBytes & 0xFF);
	
	/*
     *  ��������������ɫֵ����
     */
	for (ulColorBytesIdx = 0; ulColorBytesIdx < ulNBytes; ulColorBytesIdx++) {
		__SendByte(pucColorValue[ulColorBytesIdx]);
	}
	
	__SendTail();                                                       /* ����֡β                     */
	
	return (1);
}

/*
 *  �ֿ�������ļ�����
 */
/*********************************************************************************************************
** ��������:  ZTM_FileDownload
** ��������:  �ֿ�������ļ�����
** �������:  ucType     �������ļ�����ֵ��
**            ulSize     �������ļ����ȣ�
**            pucOption  ����ѡ�����б�ָ�룻
**            ucOptNBytes����ѡ�����б��ȡ�
** �������:  ��
** �� �� ֵ:  ��ȷ����1�����򷵻�0
*********************************************************************************************************/
INT8U ZTM_FileDownload (INT8U ucType, INT32U ulSize, INT8U *pucOption, INT8U ucOptNBytes)
{
	INT8U ucOptByteIdx;
	
	/*
     *  �������
     */
	if (ucOptNBytes > 24) {
		return (0);
	}
	
	__SendHead();                                                       /* ����֡ͷ                     */
	
	__SendByte(0xF2);                                                   /* ����ָ����                   */
	__SendByteStr(__GucFrameFileDownload, 4);                           /* ��������������               */
	__SendByte(ucType);                                                 /* ���������ļ�����ֵ           */
	__SendByte((ulSize >> 24) & 0xFF);                                  /* �����ļ�����ֵ               */
    __SendByte((ulSize >> 16) & 0xFF);
    __SendByte((ulSize >> 8) & 0xFF);
    __SendByte(ulSize & 0xFF);
	
	/*
     *  ���Ϳ�ѡ����
     */
	for (ucOptByteIdx = 0; ucOptByteIdx < ucOptNBytes; ucOptByteIdx++) {
		__SendByte(pucOption[ucOptByteIdx]);
	}
	
	__SendTail();                                                       /* ����֡β                     */
	
	return (1);
}
/*********************************************************************************************************
** ��������:  ZTM_DisBufSwitch
** ��������:  ��ʾ�������ָ��
** �������:  ucCmd������������ֵ
** �������:  ��
** �� �� ֵ:  ��
*********************************************************************************************************/
void ZTM_DisBufSwitch (INT8U ucCmd)
{
    __SendHead();                                                       /* ����֡ͷ                     */

    __SendDByte(0xE001);                                                /* ��������                     */
    
    __SendByte(ucCmd);                                                  /* ���Ͳ���                     */

    __SendTail();                                                       /* ����֡β                     */
}
/*********************************************************************************************************
end of file
*********************************************************************************************************/
