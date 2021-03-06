#include "wkup.h"
#include "delay.h"
#include "n32g45x_exti.h"
#include "n32g45x_pwr.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK战舰STM32开发板
//待机唤醒 代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/9/7
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
//////////////////// //////////////////////////////////////////////////////////////
	 
void Sys_Standby(void)
{  
	RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_PWR, ENABLE);//使能PWR外设时钟
//	PWR_WakeUpPinCmd(ENABLE);  //使能唤醒管脚功能
	PWR_EnterStandbyState();	//进入待命（STANDBY）模式 	 
}
//系统进入待机模式
void Sys_Enter_Standby(void)
{			 
	RCC_EnableAPB2PeriphReset(0X01FC, DISABLE);
	Sys_Standby();
}


void EXTI0_IRQHandler(void)
{ 		    		    				     		    
	{		  
		//Sys_Enter_Standby();  
	}
} 
// PA0 WKUP唤醒初始化
void WKUP_Init(void)
{	
//   GPIO_InitTypeDef  GPIO_InitStructure;  		  
	NVIC_InitType 	NVIC_InitStructure;
//	EXTI_InitTypeDef EXTI_InitStructure;

//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);//使能GPIOA和复用功能时钟


////------------EXTI17 ?? -------------------牋?
//		EXTI_InitStructure.EXTI_Line = EXTI_Line17;
//		EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
//		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
//		EXTI_InitStructure.EXTI_LineCmd = ENABLE;
//		EXTI_Init(&EXTI_InitStructure);
		//------------?? ??-------------------?
	    NVIC_InitStructure.NVIC_IRQChannel            = RTCAlarm_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelCmd         = ENABLE;
		NVIC_Init(&NVIC_InitStructure);  
}
















