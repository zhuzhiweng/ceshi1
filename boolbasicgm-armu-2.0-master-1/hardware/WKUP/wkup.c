#include "wkup.h"
#include "delay.h"
#include "n32g45x_exti.h"
#include "n32g45x_pwr.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEKս��STM32������
//�������� ����	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/7
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
//////////////////// //////////////////////////////////////////////////////////////
	 
void Sys_Standby(void)
{  
	RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_PWR, ENABLE);//ʹ��PWR����ʱ��
//	PWR_WakeUpPinCmd(ENABLE);  //ʹ�ܻ��ѹܽŹ���
	PWR_EnterStandbyState();	//���������STANDBY��ģʽ 	 
}
//ϵͳ�������ģʽ
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
// PA0 WKUP���ѳ�ʼ��
void WKUP_Init(void)
{	
//   GPIO_InitTypeDef  GPIO_InitStructure;  		  
	NVIC_InitType 	NVIC_InitStructure;
//	EXTI_InitTypeDef EXTI_InitStructure;

//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);//ʹ��GPIOA�͸��ù���ʱ��


////------------EXTI17 ?? -------------------���
//		EXTI_InitStructure.EXTI_Line = EXTI_Line17;
//		EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
//		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
//		EXTI_InitStructure.EXTI_LineCmd = ENABLE;
//		EXTI_Init(&EXTI_InitStructure);
		//------------?? ??-------------------�
	    NVIC_InitStructure.NVIC_IRQChannel            = RTCAlarm_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelCmd         = ENABLE;
		NVIC_Init(&NVIC_InitStructure);  
}
















