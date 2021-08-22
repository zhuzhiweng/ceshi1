#include "timer.h"
#include "iwdg.h"
#include "UART.h"
//////////////////////////////////////////////////////////////////////////////////	 
//����G�ֻ��ѧϰʹ�ã�δ������G�ɣ��������������κ���;
//Mini STM32������
//ͨ�ö�ʱ�� �i������			   
//�i��ԭ��@ALIENTEK
//������̳:www.openedv.com
//GS������:2010/12/03
//�汾��V1.0
//��Ȩ���G������ؾ���
//Copyright(C) �i��ԭ�� 2009-2019
//All rights reserved
////////////////////////////////////////////////////////////////////////////////// 	  
 

//ͨ�ö�ʱ���G�ϳ�ʼ��
//����ʱ��ѡ��ΪAPB1��2������APB1Ϊ36M
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ�i
//����ʹ�õ��Ƕ�ʱ��3!


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


void TIM3_IRQHandler(void)   //TIM3�G��
{
//	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) //���ָ����TIM�G�Ϸ������:TIM �G��Դ 
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












