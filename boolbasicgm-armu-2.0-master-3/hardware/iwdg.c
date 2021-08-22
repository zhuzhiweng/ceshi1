#include "iwdg.h"
#include "n32g45x_iwdg.h"

void IWDG_Init(u8 prer,u16 rlr) 
{	
	IWDG_WriteConfig(IWDG_WRITE_ENABLE);			//ʹ�ܶԼĴ���IWDG_PR��IWDG_RLR��д����
	IWDG_SetPrescalerDiv(prer);						//����IWDGԤ��Ƶֵ:����IWDGԤ��ƵֵΪ64
	IWDG_CntReload(rlr);							//����IWDG��װ��ֵ
	IWDG_ReloadKey();								//����IWDG��װ�ؼĴ�����ֵ��װ��IWDG������
	IWDG_Enable();									//ʹ��IWDG
}
//ι�������Ź�
void IWDG_Feed(void)
{   
 	IWDG_ReloadKey();								//����IWDG��װ�ؼĴ�����ֵ��װ��IWDG������								   
}
