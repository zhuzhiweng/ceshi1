
#include "delay.h"
#include "utils.h"
////////////////////////////////////////////////////////////////////////////////// 	 
//���ʹ��OS,����������ͷ�ļs����ucosΪ�i������.

//////////////////////////////////////////////////////////////////////////////////  
//����G�ֻ��ѧϰʹ�ã�δ������G�ɣ��������������κ���;
//ALIENTEK STM32F407������
//ʹ��SysTick����ͨ���iģʽ���ӳٽ�GG����(֧��OS)
//����delay_us,delay_ms
//�i��ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2014/5/2
//�汾��V1.3
//��Ȩ���G������ؾ���
//Copyright(C) ���I�GG�����ӿƼ��G�S��˾ 2014-2024
//All rights reserved
//********************************************************************************	   
			   
//��ʼ���ӳٺ��i
//��ʹ��OS��ʱ��,�˺��i���ʼ��OS��ʱ�ӽ���
//SYSTICK��ʱ�ӹ̶�ΪAHBʱ�ӵ�1/8
//SYSCLK:ϵͳʱ��Ƶ��
//void delay_init(u8 SYSCLK)
//{
// 	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8); 
//	fac_us=SYSCLK/8;						//�����Ƿ�ʹ��OS,fac_us��G�Ҫʹ��
//	fac_ms=(u16)fac_us*1000;				//��OS��,����ÿ��msG�Ҫ��systickʱ���i   
//}								    


//��ʱnus
//nusΪҪ��ʱ��us�i.	
//ע��:nus��ֵ,��Ҫ����798915us(���ֵ��2^24/fac_us@fac_us=21)
void delay_us(u32 nus)
{		
	int i = 168;
	while(i-->0);
//	u32 temp;	    	 
//	SysTick->LOAD=nus*fac_us; 				//ʱ�����	  		 
//	SysTick->VAL=0x00;        				//��ռ��i��
//	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ; //��ʼ���i 	 
//	do
//	{
//	temp=SysTick->CTRL;
//	}while((temp&0x01)&&!(temp&(1<<16)));	//�ȴiʱ�䵽��   
//	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk; //�رռ��i��
//	SysTick->VAL =0X00;       				//��ռ��i�� 
}
//��ʱnms
//ע��nms�ķ�Χ
//SysTick->LOADΪ24λ�Ĵ���,����,�����ʱΪ:
//nms<=0xffffff*8*1000/SYSCLK
//SYSCLK��λΪHz,nms��λΪms
//��168M���s��,nms<=798ms 
void delay_xms(u16 nms)
{	 	
	rtos_sleep_ms(nms);	
//	u32 temp;		   
//	SysTick->LOAD=(u32)nms*fac_ms;			//ʱ�����(SysTick->LOADΪ24bit)
//	SysTick->VAL =0x00;           			//��ռ��i��
//	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;          //��ʼ���i 
//	do
//	{                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             		temp=SysTick->CTRL;
//	}while((temp&0x01)&&!(temp&(1<<16)));	//�ȴiʱ�䵽��   
//	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;       //�رռ��i��
//	SysTick->VAL =0X00;     		  		//��ռ��i��	  	    
} 

//��ʱnms //nms:0~65535
void delay_ms(u16 nms)
{	 	 
	rtos_sleep_ms(nms);
//	u8 repeat=nms/540;						//������540,�ǿ��ǵ�ĳG��ͻ����ܳ�Ƶʹ��,
//											//���糬Ƶ��248M��ʱ��,delay_xms���ֻ����ʱ541ms������
//	u16 remain=nms%540;
//	while(repeat)
//	{
//		delay_xms(540);
//		repeat--;
//	}
//	if(remain)delay_xms(remain);
} 

void delay_s(u16 ns)
{
	rtos_sleep_ms(1000*ns);
//    u16 i;
//    for(i = 0; i < ns; i++)
//    {
//        delay_ms(1000);
//    }
}






































