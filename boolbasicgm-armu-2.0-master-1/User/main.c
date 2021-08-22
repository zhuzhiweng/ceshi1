

#include "cmsis_os.h"                   // ARM::CMSIS:RTOS:Keil RTX
#include "includes.h"
#include "utils.h"
#include "wkup.h"

#include "iwdg.h"

#include "sim_net.h"

#include "GB17691A.h"
//#include "GB17691_CAN_GPS.h"
#include "gps.h"

#include "spi_flash.h"
#include "sleep.h"
#include "power_ctl.h"
#include "obd.h"

//#include "udsservice.h"
#include "UDS.h"

#include "bsp.h"

#include "TerminalTest.h"
#include "LKT4305_IIC.h"
#include "timer.h"
//#define debug 1

	volatile uint32_t mainCnt=0;
	volatile uint32_t mainCnt1=0;
//------------------------------
  u8 DMATX_len=0;			// DMA·¢ËÍ³¤¶È¡
	
//  SIM868 ½ÓÊÕBUFF	
//u8 SIM868_RXBUF[SIM868RXBUF_SIZE]={0};
//u16 SIM868_RX=0;




//SIM868 ÁªÍø²ÎÊý
//u8 air800_count=0;
u8 log_state=0;
u8 net_state=0;		
//-------------»¥³âËø¶¨Òå
osMutexDef(spiflash_mtx);
osMutexId  spiflash_mtx_id;
osStatus   spi_state;

osMutexDef(USART1_MUX);
osMutexId USART1_MUX_mutex_id;

extern uint32_t time_cnt;



extern uint32_t time_cnt;
extern void ShowDataRun(void);

extern u32 iwdg_17691;
extern u32 iwdg_net;  
extern u32 iwdg_obd;

int main(void)
{
//	uint32_t mainCnt=0;
//	uint32_t mainCnt1=0;
	u8 flag_test=0;
	for(uint16_t i=0xffff;i>0;i--); //ÑÓÊ±µÈ´ýÍâ²¿ÔªÆ÷¼þ×¼±¸¾ÍÐ÷¡£ 
  
	power_ctl_gpio_init();
	IWDG_Init(6,2500);//¿´ÃÅ¹·³õÊ¼»¯,  Òç³öÊ±¼ä4S    £¨4*2^6£©*625/40
	
	gps_uart_init(9600);
	ADC_Configuration();
	
	RS232_USART_Config(115200);   //20190326_lycadd
	USART1_MUX_mutex_id = osMutexCreate (osMutex(USART1_MUX));
#ifdef debug
	USARTSendOut(RS232_USART,"DEBUG USART CONFIG!\r\n",strlen("DEBUG USART CONFIG!\r\n"));
#endif
	TIM3_Int_Init((10000-1),7200-1);	//¶¨Ê±Æ÷Ê±ÖÓ84M£¬·ÖÆµÏµÊý8400£¬ËùÒÔ84M/8400=10KhzµÄ¼ÆÊýÆµÂÊ£¬¼ÆÊý5000´ÎÎª500ms
	My_RTC_Init();
	W25QXX_Init();
	
	//clear_firmware_info();
 // W25QXX_Erase_Chip();
	
//	SCB->VTOR = FLASH_BASE | 0x8000; // Vector Table Relocation in Internal FLASH.  
	osKernelInitialize();
//	uart_gps_rx_queue_init();
	//rtos_task_create((void *)main_task,0,"m",0,0);
	can_collection();
	sim_net_task_init();
	GB17691_task_init();
//	ShowDataRun();
	osKernelStart();
    while(1)
    {
					
		osDelay(100);  //ÑÓÊ±100mS	
			COM_CMD_check();
       mainCnt1++;
		if(mainCnt1%10==0)
		{	
			RTC_Get();

		}

			if(mainCnt1%20==0){
			   mainCnt1=0;
				 if(!get_comupdate_mode()){   //´®¿ÚÉý¼¶Ä£Ê½ÏÂ£¬²»×öÖÕ¶Ë²âÊÔ´¦Àí
						TerminalTestProcess();
					  mainCnt++;
						if(mainCnt%5==0)
							{ 
							  mainCnt=0;
								if(	(getTermianlState()==1)||(getTermianlState()==2)	)
									{
					          flag_test=1;
									}
								if(!flag_test)
									{
//					          ShowData2App(); 
									}
							}
					}
				
			if(	(iwdg_17691!=0)&&(iwdg_net!=0)&&(iwdg_obd!=0)	)
			{
				IWDG_Feed();	
			}
			else
			{
			#ifdef debug
			if(iwdg_17691==0)
				USARTSendOut(RS232_USART,"iwdg_17691 is error\r\n",21);
			if(iwdg_net==0)
				USARTSendOut(RS232_USART,"iwdg_net is error\r\n",19);			
			if(iwdg_obd==0)
				USARTSendOut(RS232_USART,"iwdg_obd is error\r\n",19);
			#endif	
			}
				iwdg_17691=0;
				iwdg_net=0;
				iwdg_obd=0;
				
			}
			is_sleep();
				
			/*
			osDelay(2000);		
			TerminalTestProcess();
			if(	(iwdg_17691!=0)&&(iwdg_net!=0)&&(iwdg_obd!=0)	)
			{
				IWDG_Feed();	
			}
			else
			{
			#ifdef debug
			if(iwdg_17691==0)
				USARTSendOut(RS232_USART,"iwdg_17691 is error\r\n",21);
			if(iwdg_net==0)
				USARTSendOut(RS232_USART,"iwdg_net is error\r\n",19);			
			if(iwdg_obd==0)
				USARTSendOut(RS232_USART,"iwdg_obd is error\r\n",19);
			#endif	
			}
				iwdg_17691=0;
				iwdg_net=0;
				iwdg_obd=0;
			
			
			is_sleep();
			mainCnt++;
			if(mainCnt%5==0)
			{
				  if(	(getTermianlState()==1)||(getTermianlState()==2)	)
				{
					flag_test=1;
				}
				if(!flag_test)
				{
					ShowData2App(); 
				}

			}
     */
		 
    }
}
