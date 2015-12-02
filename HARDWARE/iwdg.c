#include "iwdg.h"

u32 count = 3125; //���ֵ0xFFF / 4095    Tout = ((4*2^prer)*count) / 40

void wdt_init(void)
{
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);  //ʹ�ܶԼĴ���IWDG_PR��IWDG_RLR��д����
    
    IWDG_SetPrescaler(IWDG_Prescaler_256);  //����IWDGԤ��Ƶֵ:����IWDGԤ��ƵֵΪ256
    
    IWDG_SetReload(count);  //����IWDG��װ��ֵ
    
    IWDG_ReloadCounter();  //����IWDG��װ�ؼĴ�����ֵ��װ��IWDG������
    
    IWDG_Enable();  //ʹ��IWDG 
}
