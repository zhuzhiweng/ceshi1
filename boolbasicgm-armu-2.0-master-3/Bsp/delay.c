
#include "delay.h"
#include "utils.h"
////////////////////////////////////////////////////////////////////////////////// 	 
//Èç¹ûÊ¹ÓÃOS,Ôò°üÀ¨ÏÂÃæµÄÍ·ÎÄ¼s£¨ÒÔucosÎªÀi£©¼´¿É.

//////////////////////////////////////////////////////////////////////////////////  
//±¾³ÌGòÖ»¹©Ñ§Ï°Ê¹ÓÃ£¬Î´¾­×÷ÕßGí¿É£¬²»µÃÓÃÓÚÆäËüÈÎºÎÓÃÍ¾
//ALIENTEK STM32F407¿ª·¢°å
//Ê¹ÓÃSysTickµÄÆÕÍ¨¼ÆÊiÄ£Ê½¶ÔÑÓ³Ù½øGG¹ÜÀí(Ö§³ÖOS)
//°üÀ¨delay_us,delay_ms
//ÕiµãÔ­×Ó@ALIENTEK
//¼¼ÊõÂÛÌ³:www.openedv.com
//´´½¨ÈÕÆÚ:2014/5/2
//°æ±¾£ºV1.3
//°æÈ¨ËùÓG£¬µÁ°æ±Ø¾¿¡£
//Copyright(C) ¹ãÖIÊGGÇÒíµç×Ó¿Æ¼¼ÓGÏS¹«Ë¾ 2014-2024
//All rights reserved
//********************************************************************************	   
			   
//³õÊ¼»¯ÑÓ³Ùº¯Êi
//µ±Ê¹ÓÃOSµÄÊ±ºò,´Ëº¯Êi»á³õÊ¼»¯OSµÄÊ±ÖÓ½ÚÅÄ
//SYSTICKµÄÊ±ÖÓ¹Ì¶¨ÎªAHBÊ±ÖÓµÄ1/8
//SYSCLK:ÏµÍ³Ê±ÖÓÆµÂÊ
//void delay_init(u8 SYSCLK)
//{
// 	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8); 
//	fac_us=SYSCLK/8;						//²»ÂÛÊÇ·ñÊ¹ÓÃOS,fac_us¶¼GèÒªÊ¹ÓÃ
//	fac_ms=(u16)fac_us*1000;				//·ÇOSÏÂ,´ú±íÃ¿¸ömsGèÒªµÄsystickÊ±ÖÓÊi   
//}								    


//ÑÓÊ±nus
//nusÎªÒªÑÓÊ±µÄusÊi.	
//×¢Òâ:nusµÄÖµ,²»Òª´óÓÚ798915us(×î´óÖµ¼´2^24/fac_us@fac_us=21)
void delay_us(u32 nus)
{		
	int i = 168;
	while(i-->0);
//	u32 temp;	    	 
//	SysTick->LOAD=nus*fac_us; 				//Ê±¼ä¼ÓÔØ	  		 
//	SysTick->VAL=0x00;        				//Çå¿Õ¼ÆÊiÆ÷
//	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ; //¿ªÊ¼µ¹Êi 	 
//	do
//	{
//	temp=SysTick->CTRL;
//	}while((temp&0x01)&&!(temp&(1<<16)));	//µÈ´iÊ±¼äµ½´ï   
//	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk; //¹Ø±Õ¼ÆÊiÆ÷
//	SysTick->VAL =0X00;       				//Çå¿Õ¼ÆÊiÆ÷ 
}
//ÑÓÊ±nms
//×¢ÒânmsµÄ·¶Î§
//SysTick->LOADÎª24Î»¼Ä´æÆ÷,ËùÒÔ,×î´óÑÓÊ±Îª:
//nms<=0xffffff*8*1000/SYSCLK
//SYSCLKµ¥Î»ÎªHz,nmsµ¥Î»Îªms
//¶Ô168MÌõ¼sÏÂ,nms<=798ms 
void delay_xms(u16 nms)
{	 	
	rtos_sleep_ms(nms);	
//	u32 temp;		   
//	SysTick->LOAD=(u32)nms*fac_ms;			//Ê±¼ä¼ÓÔØ(SysTick->LOADÎª24bit)
//	SysTick->VAL =0x00;           			//Çå¿Õ¼ÆÊiÆ÷
//	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;          //¿ªÊ¼µ¹Êi 
//	do
//	{                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             		temp=SysTick->CTRL;
//	}while((temp&0x01)&&!(temp&(1<<16)));	//µÈ´iÊ±¼äµ½´ï   
//	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;       //¹Ø±Õ¼ÆÊiÆ÷
//	SysTick->VAL =0X00;     		  		//Çå¿Õ¼ÆÊiÆ÷	  	    
} 

//ÑÓÊ±nms //nms:0~65535
void delay_ms(u16 nms)
{	 	 
	rtos_sleep_ms(nms);
//	u8 repeat=nms/540;						//ÕâÀïÓÃ540,ÊÇ¿¼ÂÇµ½Ä³G©¿Í»§¿ÉÄÜ³¬ÆµÊ¹ÓÃ,
//											//±ÈÈç³¬Æµµ½248MµÄÊ±ºò,delay_xms×î´óÖ»ÄÜÑÓÊ±541ms×óÓÒÁË
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






































