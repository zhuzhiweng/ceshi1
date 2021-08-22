#include "iwdg.h"
#include "n32g45x_iwdg.h"

void IWDG_Init(u8 prer,u16 rlr) 
{	
	IWDG_WriteConfig(IWDG_WRITE_ENABLE);			//使能对寄存器IWDG_PR和IWDG_RLR的写操作
	IWDG_SetPrescalerDiv(prer);						//设置IWDG预分频值:设置IWDG预分频值为64
	IWDG_CntReload(rlr);							//设置IWDG重装载值
	IWDG_ReloadKey();								//按照IWDG重装载寄存器的值重装载IWDG计数器
	IWDG_Enable();									//使能IWDG
}
//喂独立看门狗
void IWDG_Feed(void)
{   
 	IWDG_ReloadKey();								//按照IWDG重装载寄存器的值重装载IWDG计数器								   
}
