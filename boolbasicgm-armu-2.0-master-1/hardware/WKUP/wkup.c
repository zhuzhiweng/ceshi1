#include "wkup.h"
#include "delay.h"
#include "n32g45x_exti.h"
#include "n32g45x_pwr.h"
//////////////////////////////////////////////////////////////////////////////////	 
//±¾³ÌÐòÖ»¹©Ñ§Ï°Ê¹ÓÃ£¬Î´¾­×÷ÕßÐí¿É£¬²»µÃÓÃÓÚÆäËüÈÎºÎÓÃÍ¾
//ALIENTEKÕ½½¢STM32¿ª·¢°å
//´ý»ú»½ÐÑ ´úÂë	   
//ÕýµãÔ­×Ó@ALIENTEK
//¼¼ÊõÂÛÌ³:www.openedv.com
//ÐÞ¸ÄÈÕÆÚ:2012/9/7
//°æ±¾£ºV1.0
//°æÈ¨ËùÓÐ£¬µÁ°æ±Ø¾¿¡£
//Copyright(C) ¹ãÖÝÊÐÐÇÒíµç×Ó¿Æ¼¼ÓÐÏÞ¹«Ë¾ 2009-2019
//All rights reserved									  
//////////////////// //////////////////////////////////////////////////////////////
	 
void Sys_Standby(void)
{  
	RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_PWR, ENABLE);//Ê¹ÄÜPWRÍâÉèÊ±ÖÓ
//	PWR_WakeUpPinCmd(ENABLE);  //Ê¹ÄÜ»½ÐÑ¹Ü½Å¹¦ÄÜ
	PWR_EnterStandbyState();	//½øÈë´ýÃü£¨STANDBY£©Ä£Ê½ 	 
}
//ÏµÍ³½øÈë´ý»úÄ£Ê½
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
// PA0 WKUP»½ÐÑ³õÊ¼»¯
void WKUP_Init(void)
{	
//   GPIO_InitTypeDef  GPIO_InitStructure;  		  
	NVIC_InitType 	NVIC_InitStructure;
//	EXTI_InitTypeDef EXTI_InitStructure;

//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);//Ê¹ÄÜGPIOAºÍ¸´ÓÃ¹¦ÄÜÊ±ÖÓ


////------------EXTI17 ?? -------------------   
//		EXTI_InitStructure.EXTI_Line = EXTI_Line17;
//		EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
//		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
//		EXTI_InitStructure.EXTI_LineCmd = ENABLE;
//		EXTI_Init(&EXTI_InitStructure);
		//------------?? ??------------------- 
	    NVIC_InitStructure.NVIC_IRQChannel            = RTCAlarm_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelCmd         = ENABLE;
		NVIC_Init(&NVIC_InitStructure);  
}
















