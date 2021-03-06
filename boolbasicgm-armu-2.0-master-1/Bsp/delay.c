
#include "delay.h"
#include "utils.h"
////////////////////////////////////////////////////////////////////////////////// 	 
//如果使用OS,则包括下面的头文約（以ucos为纈）即可.

//////////////////////////////////////////////////////////////////////////////////  
//本程G蛑还┭笆褂茫淳髡逩砜桑坏糜糜谄渌魏斡猛?
//ALIENTEK STM32F407开发板
//使用SysTick的普通计蔵模式对延迟进GG管理(支持OS)
//包括delay_us,delay_ms
//読点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2014/5/2
//版本：V1.3
//版权所覩，盗版必究。
//Copyright(C) 广諭蔊G且淼缱涌萍加G蟂公司 2014-2024
//All rights reserved
//********************************************************************************	   
			   
//初始化延迟函蔵
//当使用OS的时候,此函蔵会初始化OS的时钟节拍
//SYSTICK的时钟固定为AHB时钟的1/8
//SYSCLK:系统时钟频率
//void delay_init(u8 SYSCLK)
//{
// 	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8); 
//	fac_us=SYSCLK/8;						//不论是否使用OS,fac_us都G枰褂?
//	fac_ms=(u16)fac_us*1000;				//非OS下,代表每个msG枰膕ystick时钟蔵   
//}								    


//延时nus
//nus为要延时的us蔵.	
//注意:nus的值,不要大于798915us(最大值即2^24/fac_us@fac_us=21)
void delay_us(u32 nus)
{		
	int i = 168;
	while(i-->0);
//	u32 temp;	    	 
//	SysTick->LOAD=nus*fac_us; 				//时间加载	  		 
//	SysTick->VAL=0x00;        				//清空计蔵器
//	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ; //开始倒蔵 	 
//	do
//	{
//	temp=SysTick->CTRL;
//	}while((temp&0x01)&&!(temp&(1<<16)));	//等磇时间到达   
//	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk; //关闭计蔵器
//	SysTick->VAL =0X00;       				//清空计蔵器 
}
//延时nms
//注意nms的范围
//SysTick->LOAD为24位寄存器,所以,最大延时为:
//nms<=0xffffff*8*1000/SYSCLK
//SYSCLK单位为Hz,nms单位为ms
//对168M条約下,nms<=798ms 
void delay_xms(u16 nms)
{	 	
	rtos_sleep_ms(nms);	
//	u32 temp;		   
//	SysTick->LOAD=(u32)nms*fac_ms;			//时间加载(SysTick->LOAD为24bit)
//	SysTick->VAL =0x00;           			//清空计蔵器
//	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;          //开始倒蔵 
//	do
//	{                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             		temp=SysTick->CTRL;
//	}while((temp&0x01)&&!(temp&(1<<16)));	//等磇时间到达   
//	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;       //关闭计蔵器
//	SysTick->VAL =0X00;     		  		//清空计蔵器	  	    
} 

//延时nms //nms:0~65535
void delay_ms(u16 nms)
{	 	 
	rtos_sleep_ms(nms);
//	u8 repeat=nms/540;						//这里用540,是考虑到某G┛突Э赡艹凳褂?,
//											//比如超频到248M的时候,delay_xms最大只能延时541ms左右了
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






































