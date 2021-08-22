#include "timer.h"
#include "iwdg.h"
#include "UART.h"
//////////////////////////////////////////////////////////////////////////////////	 
//±¾³ÌGòÖ»¹©Ñ§Ï°Ê¹ÓÃ£¬Î´¾­×÷ÕßGí¿É£¬²»µÃÓÃÓÚÆäËüÈÎºÎÓÃÍ¾
//Mini STM32¿ª·¢°å
//Í¨ÓÃ¶¨Ê±Æ÷ Çi¶¯´úÂë			   
//ÕiµãÔ­×Ó@ALIENTEK
//¼¼ÊõÂÛÌ³:www.openedv.com
//GS¸ÄÈÕÆÚ:2010/12/03
//°æ±¾£ºV1.0
//°æÈ¨ËùÓG£¬µÁ°æ±Ø¾¿¡£
//Copyright(C) ÕiµãÔ­×Ó 2009-2019
//All rights reserved
////////////////////////////////////////////////////////////////////////////////// 	  
 

//Í¨ÓÃ¶¨Ê±Æ÷ÖG¶Ï³õÊ¼»¯
//ÕâÀïÊ±ÖÓÑ¡ÔñÎªAPB1µÄ2±¶£¬¶øAPB1Îª36M
//arr£º×Ô¶¯ÖØ×°Öµ¡£
//psc£ºÊ±ÖÓÔ¤·ÖÆµÊi
//ÕâÀïÊ¹ÓÃµÄÊÇ¶¨Ê±Æ÷3!


extern u8 iwdg_net;
extern u8 iwdg_main;
extern u8 iwdg_gps;

void TIM3_Int_Init(u16 arr,u16 psc)
{
    TIM_TimeBaseInitType TIM_TimeBaseStructure;
	
	RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_TIM3, ENABLE);
	
	TIM_TimeBaseStructure.Period    = arr;
    TIM_TimeBaseStructure.Prescaler = psc;
    TIM_TimeBaseStructure.ClkDiv    = 0;
    TIM_TimeBaseStructure.CntMode   = TIM_CNT_MODE_UP;

    TIM_InitTimeBase(TIM3, &TIM_TimeBaseStructure);
	
	TIM_ConfigInt(TIM3, TIM_INT_UPDATE, ENABLE);
	TIM_Enable(TIM3, ENABLE);							 
}

extern uint8_t timing_state;
extern u32 TIME_time;


void TIM3_IRQHandler(void)   //TIM3ÖG¶Ï
{
//	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) //¼ì²éÖ¸¶¨µÄTIMÖG¶Ï·¢ÉúÓë·ñ:TIM ÖG¶ÏÔ´ 
//		{
//		if((iwdg_net>0)&&(iwdg_main>0)&&(iwdg_gps>0))
//			{
//				IWDG_Feed();	
//			}
//		else
//		{
//			#ifdef printf
//			if(iwdg_gps==0)
//				USARTSendOut(USART1,"iwdg_gps is error\r\n",19);
//			if(iwdg_main==0)
//				USARTSendOut(USART1,"iwdg_main is error\r\n",20);			
//			if(iwdg_net==0)
//				USARTSendOut(USART1,"iwdg_net is error\r\n",19);
//			#endif	
//		}
//		iwdg_net=0;
//		iwdg_main=0;
//		iwdg_gps=0;
//		}
	if(timing_state == 1)
	{
//		uprintf("TIMER_time:%d\r\n",TIME_time);
		TIME_time++;
	}
	TIM_ClrIntPendingBit(TIM3,TIM_INT_UPDATE);
}












