#include  <stdio.h>
#include  <string.h>
#include "math.h"
#include "stdlib.h"
#include "rtc.h"
#include "delay.h"
#include "cmsis_os.h"

uint8_t set_time_flag = 0;

const uint32_t DAYS_IN_MONTH[13] =
{
   /* Index from 1, hence skip 0*/
    0U,
    /*Jan, Feb, Mar, Apr, May, Jun, Jul, Aug, Sep, Oct, Nov, Dec*/
      31U, 28U, 31U, 30U, 31U, 30U, 31U, 31U, 30U, 31U, 30U, 31U
};


u8 RTC_Get(void);

rtc_time_t calendar;



NVIC_InitType   NVIC_InitStructure;

//static void RTC_NVIC_Config(void)
//{	
//	NVIC_InitType NVIC_InitStructure;
//	NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;					//RTCÈ«¾ÖÖG¶Ï
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;		//ÏÈÕ¼ÓÅÏÈ¼¶1Î»,´ÓÓÅÏÈ¼¶3Î»
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;				//ÏÈÕ¼ÓÅÏÈ¼¶0Î»,´ÓÓÅÏÈ¼¶4Î»
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;					//Ê¹ÄÜ¸ÃÍ¨µÀÖG¶Ï
//	NVIC_Init(&NVIC_InitStructure);									//¸ù¾INVIC_InitStructÖGÖ¸¶¨µÄ²ÎÊi³õÊ¼»¯ÍâÉèNVIC¼Ä´æÆ÷
//}


void Set_rtcalarm(u8 time_s)
{
	RTC_TimeType RTC_TimeStructure;
	RTC_AlarmType RTC_AlarmStructure;
	
	RTC_GetTime(RTC_FORMAT_BIN, &RTC_TimeStructure);
	RTC_GetAlarm(RTC_FORMAT_BIN, RTC_A_ALARM, &RTC_AlarmStructure);

//	if(RTC_TimeStructure.Seconds + time_s >= 60)
//	{
//		if(RTC_TimeStructure.Minutes + 1 >= 60)
//		{
//			RTC_AlarmStructure.AlarmTime.Hours = RTC_TimeStructure.Hours + 1;
//			if(RTC_TimeStructure.Hours >= 24)
//			{	
//				RTC_AlarmStructure.AlarmTime.Hours = 0;
//			}			
//			RTC_AlarmStructure.AlarmTime.Minutes = 0;
//			RTC_AlarmStructure.AlarmTime.Seconds = RTC_TimeStructure.Seconds + time_s - 60;
//		}
//		else
//		{
//			RTC_AlarmStructure.AlarmTime.Hours = RTC_TimeStructure.Hours;
//			RTC_AlarmStructure.AlarmTime.Minutes = RTC_TimeStructure.Minutes + 1;
//			RTC_AlarmStructure.AlarmTime.Seconds = RTC_TimeStructure.Seconds + time_s - 60;
//		}
//	}
//	else
//	{
//		RTC_AlarmStructure.AlarmTime.Seconds = RTC_TimeStructure.Seconds + time_s;
//	}
	if(RTC_TimeStructure.Seconds + time_s >= 60)
	{
		RTC_AlarmStructure.AlarmTime.Seconds = RTC_TimeStructure.Seconds + time_s - 60;
	}
	else
	{
		RTC_AlarmStructure.AlarmTime.Seconds = RTC_TimeStructure.Seconds + time_s;
	}
	RTC_SetAlarm(RTC_FORMAT_BIN, RTC_A_ALARM, &RTC_AlarmStructure);
}





u8 Is_Leap_Year(u16 year)
{			  
	if(year%4==0) //±ØGëÄÜ±»4Õû³i
	{ 
		if(year%100==0) 
		{ 
			if(year%400==0)return 1;//Èç¹ûÒÔ00½áÎ²,»¹ÒªÄÜ±»400Õû³i 	   
			else return 0;   
		}else return 1;   
	}else return 0;	
}	



//ÉèÖÃÊ±ÖÓ
//°ÑÊäÈëµÄÊ±ÖÓ×ª»»ÎªÃëÖÓ
//ÒÔ1970Äê1ÔÂ1ÈÕÎª»ù×¼
//1970~2099ÄêÎªºÏ·¨Äê·I
//·µ»ØÖµ:0,³É¹¦;ÆäËû:´íÎó´úÂë.
//ÔÂ·IÊi¾I±í											 
u8 const table_week[12]={0,3,3,6,1,4,6,2,5,0,3,5}; //ÔÂGSÕiÊi¾I±í	  
//Æ½ÄêµÄÔÂ·IÈÕÆÚ±í
const u8 mon_table[12]={31,28,31,30,31,30,31,31,30,31,30,31};
u32 TIME_time =0xFFFFFFFF;


ErrorStatus RTC_Set(u8 year,u8 smon,u8 sday,u8 hour,u8 min,u8 sec)
{
	u16 t,syear;
	u32 seccount=0;
	syear=2000+year;
	if(syear<1970||syear>2099) return ERROR;	   
	for(t=1970;t<syear;t++)	//°ÑËùÓGÄê·IµÄÃëÖÓÏà¼Ó
	{
		if(Is_Leap_Year(t))seccount+=31622400;//ÈòÄêµÄÃëÖÓÊi
		else seccount+=31536000;			  //Æ½ÄêµÄÃëÖÓÊi
	}
	smon-=1;
	for(t=0;t<smon;t++)	   //°ÑÇ°ÃæÔÂ·IµÄÃëÖÓÊiÏà¼Ó
	{
		seccount+=(u32)mon_table[t]*86400;//ÔÂ·IÃëÖÓÊiÏà¼Ó
		if(Is_Leap_Year(syear)&&t==1)seccount+=86400;//ÈòÄê2ÔÂ·IÔö¼ÓÒ»ÌìµÄÃëÖÓÊi	   
	}
	seccount+=(u32)(sday-1)*86400;//°ÑÇ°ÃæÈÕÆÚµÄÃëÖÓÊiÏà¼Ó 
	seccount+=(u32)hour*3600;//G¡Ê±ÃëÖÓÊi
  seccount+=(u32)min*60;	 //·ÖÖÓÃëÖÓÊi
	seccount+=sec;//×îºóµÄÃëÖÓ¼ÓÉÏÈ¥
	TIME_time = seccount + 2 ; 
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);	//Ê¹ÄÜPWRºÍBKPÍâÉèÊ±ÖÓ  
//	PWR_BackupAccessCmd(ENABLE);	//Ê¹ÄÜRTCºÍºó±¸¼Ä´æÆ÷·ÃÎÊ 
//	RTC_SetCounter(seccount);	//ÉèÖÃRTC¼ÆÊiÆ÷µÄÖµ

//	RTC_WaitForLastTask();	//µÈ´i×î½üÒ»´Î¶ÔRTC¼Ä´æÆ÷µÄG´²Ù×÷Íê³É  	
	RTC_Get();
	return SUCCESS;	    
}
//»ñµÃÏÖÔÚÊÇGÇÆÚ¼¸
//¹¦ÄÜÃèÊö:ÊäÈë¹«ÀúÈÕÆÚµÃµ½GÇÆÚ(Ö»ÔÊGí1901-2099Äê)
//ÊäÈë²ÎÊi£º¹«ÀúÄêÔÂÈÕ 
//·µ»ØÖµ£ºGÇÆÚºÅ	
u8 RTC_Get_Week(u16 year,u8 month,u8 day)
{	
	u16 temp2;
	u8 yearH,yearL;
	
	yearH=year/100;	yearL=year%100; 
	// Èç¹ûÎª21ÊÀ¼Í,Äê·IÊi¼Ó100  
	if (yearH>19)yearL+=100;
	// Ëù¹iÈòÄêÊiÖ»Ëã1900ÄêÖ®ºóµÄ  
	temp2=yearL+yearL/4;
	temp2=temp2%7; 
	temp2=temp2+day+table_week[month-1];
	if (yearL%4==0&&month<3)temp2--;
	return(temp2%7);
}			  



//µÃµ½µ±Ç°µÄÊ±¼ä
//·µ»ØÖµ:0,³É¹¦;ÆäËû:´íÎó´úÂë.
u8 RTC_Get(void)
{
	static u16 daycnt=0;
	u32 timecount=0; 
	u32 temp=0;
	u16 temp1=0;	  
    timecount=TIME_time;	 
 	temp=timecount/86400;   //µÃµ½ÌìÊi(ÃëÖÓÊi¶ÔÓ¦µÄ)
	if(daycnt!=temp)//³¬¹iÒ»ÌìÁË
	{	  
		daycnt=temp;
		temp1=1970;	//´Ó1970Äê¿ªÊ¼
		while(temp>=365)
		{				 
			if(Is_Leap_Year(temp1))//ÊÇÈòÄê
			{
				if(temp>=366)temp-=366;//ÈòÄêµÄÃëÖÓÊi
				else {temp1++;break;}  
			}
			else temp-=365;	  //Æ½Äê 
			temp1++;  
		}   
		calendar.year=temp1 - 2000;//µÃµ½Äê·I
		temp1=0;
		while(temp>=28)//³¬¹iÁËÒ»¸öÔÂ
		{
			if(Is_Leap_Year(calendar.year)&&temp1==1)//µ±ÄêÊÇ²»ÊÇÈòÄê/2ÔÂ·I
			{
				if(temp>=29)temp-=29;//ÈòÄêµÄÃëÖÓÊi
				else break; 
			}
			else 
			{
				if(temp>=mon_table[temp1])temp-=mon_table[temp1];//Æ½Äê
				else break;
			}
			temp1++;  
		}
		calendar.month=temp1+1;	//µÃµ½ÔÂ·I
		calendar.date=temp+1;  	//µÃµ½ÈÕÆÚ 
	}
	temp=timecount%86400;     		//µÃµ½ÃëÖÓÊi   	   
	calendar.hour=temp/3600;     	//G¡Ê±
	calendar.minute=(temp%3600)/60; 	//·ÖÖÓ	
	calendar.second=(temp%3600)%60; 	//ÃëÖÓ
	return 0;
}	


//RTC³õÊ¼»¯
//·µ»ØÖµ:0,³õÊ¼»¯³É¹¦;
//       1,LSE¿ªÆôÊ§°Ü;
//       2,½øÈë³õÊ¼»¯Ä£Ê½Ê§°Ü;
u8 My_RTC_Init(void)
{
	RTC_DateType RTC_DateStructure;
	RTC_TimeType RTC_TimeStructure;
	RTC_InitType RTC_InitStructure;
	RTC_AlarmType RTC_AlarmStructure;
	
	
	//¼ì²éÊÇ²»ÊÇµÚÒ»´ÎÅäÖÃÊ±ÖÓ
//	u8 temp=0;	
	RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_PWR | RCC_APB1_PERIPH_BKP, ENABLE);	//Ê¹ÄÜPWRºÍBKPÍâÉèÊ±ÖÓ
	RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_AFIO, ENABLE);	
	PWR_BackupAccessEnable(ENABLE);//Ê¹ÄÜºó±¸¼Ä´æÆ÷·ÃÎÊ 	
	
//	RCC_EnableLsi(DISABLE);
//	RCC_ConfigHse(RCC_HSE_ENABLE);
//	while (RCC_WaitHseStable() == ERROR);
//	RCC_ConfigRtcClk(RCC_RTCCLK_SRC_HSE_DIV128);
	RCC_EnableLsi(ENABLE);
	while (RCC_GetFlagStatus(RCC_FLAG_LSIRD) == RESET);
	RCC_ConfigRtcClk(RCC_RTCCLK_SRC_LSI);
	
	

	RCC_EnableRtcClk(ENABLE);
    RTC_WaitForSynchro();
//	RTC_InitStructure.RTC_AsynchPrediv = 0x1E8;
//	RTC_InitStructure.RTC_SynchPrediv  = 0x7F;
	RTC_InitStructure.RTC_AsynchPrediv = 0x7F;
    RTC_InitStructure.RTC_SynchPrediv  = 0x136;
	RTC_InitStructure.RTC_HourFormat   = RTC_24HOUR_FORMAT;
	
	RTC_Init(&RTC_InitStructure);
	

	
	RTC_DateStructure.WeekDay = 0x3;
	RTC_DateStructure.Date = 20;
	RTC_DateStructure.Month = 11;
	RTC_DateStructure.Year = 19;
	
	RTC_SetDate(RTC_FORMAT_BIN, &RTC_DateStructure);
	RTC_SetDate(RTC_FORMAT_BIN, &RTC_DateStructure);
	
	RTC_TimeStructure.Hours = 4;
	RTC_TimeStructure.Minutes = 22;
	RTC_TimeStructure.Seconds = 30;
	RTC_ConfigTime(RTC_FORMAT_BIN, &RTC_TimeStructure);
	
	
	
	RTC_EnableAlarm(RTC_A_ALARM, DISABLE);
	RTC_AlarmStructure.AlarmTime.H12 = RTC_AM_H12;
    RTC_TimeStructure.H12            = RTC_AM_H12;
	
	RTC_AlarmStructure.AlarmTime.Hours = 4;
	RTC_AlarmStructure.AlarmTime.Minutes = 22;
	RTC_AlarmStructure.AlarmTime.Seconds = 40;
	RTC_AlarmStructure.DateWeekValue = 0x31;
	RTC_AlarmStructure.DateWeekMode = RTC_ALARM_SEL_WEEKDAY_DATE;
	RTC_AlarmStructure.AlarmMask =  RTC_ALARMMASK_HOURS | RTC_ALARMMASK_MINUTES | RTC_ALARMMASK_WEEKDAY;
//	RTC_AlarmStructure.AlarmMask = RTC_ALARMMASK_HOURS | RTC_ALARMMASK_MINUTES;
	RTC_SetAlarm(RTC_FORMAT_BIN, RTC_A_ALARM, &RTC_AlarmStructure);
	RTC_ConfigInt(RTC_INT_ALRA, ENABLE);
	RTC_EnableAlarm(RTC_A_ALARM, DISABLE);
//	
	    EXTI_InitType EXTI_InitStructure;
    NVIC_InitType NVIC_InitStructure;

    EXTI_ClrITPendBit(EXTI_LINE17);
    EXTI_InitStructure.EXTI_Line = EXTI_LINE17;

    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_InitPeripheral(&EXTI_InitStructure);

//    /* Enable the RTC Alarm Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel                   = RTCAlarm_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
	
//	EXTI17_RTCAlarm_Configuration(ENABLE);
	
	
//	if(BKP_ReadBackupRegister(BKP_DR1) != 0x5050)		//´ÓÖ¸¶¨µÄºó±¸¼Ä´æÆ÷ÖG¶Á³öÊi¾I:¶Á³öÁËÓëG´ÈëµÄÖ¸¶¨Êi¾I²»Ïàºõ
//	{ 			
//		BKP_DeInit();	//¸´Î»±¸·IÇøÓò 	
//		RCC_LSEConfig(RCC_LSE_ON);	//ÉèÖÃÍâ²¿µÍËÙ¾§Õñ(LSE),Ê¹ÓÃÍâÉèµÍËÙ¾§Õñ
//		while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET);	//¼ì²éÖ¸¶¨µÄRCC±êÖ¾Î»ÉèÖÃÓë·ñ,µÈ´iµÍËÙ¾§Õñ¾ÍG÷
//   
//		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);		//ÉèÖÃRTCÊ±ÖÓ(RTCCLK),Ñ¡ÔñLSE×÷ÎªRTCÊ±ÖÓ    
//		RCC_RTCCLKCmd(ENABLE);	//Ê¹ÄÜRTCÊ±ÖÓ  
//		RTC_WaitForLastTask();	//µÈ´i×î½üÒ»´Î¶ÔRTC¼Ä´æÆ÷µÄG´²Ù×÷Íê³É
//		RTC_WaitForSynchro();		//µÈ´iRTC¼Ä´æÆ÷Í¬²½  
//		RTC_ITConfig(RTC_IT_SEC, ENABLE);		//Ê¹ÄÜRTCÃëÖG¶Ï
//		RTC_WaitForLastTask();	//µÈ´i×î½üÒ»´Î¶ÔRTC¼Ä´æÆ÷µÄG´²Ù×÷Íê³É
//		RTC_EnterConfigMode();/// ÔÊGíÅäÖÃ	
//		RTC_SetPrescaler(32767); //ÉèÖÃRTCÔ¤·ÖÆµµÄÖµ
//		RTC_WaitForLastTask();	//µÈ´i×î½üÒ»´Î¶ÔRTC¼Ä´æÆ÷µÄG´²Ù×÷Íê³É
//		RTC_Set(19,2,1,10,0,55);  //ÉèÖÃÊ±¼ä				
//		RTC_ExitConfigMode(); //ÍË³öÅäÖÃÄ£Ê½  
//		BKP_WriteBackupRegister(BKP_DR1, 0X5050);	//ÏòÖ¸¶¨µÄºó±¸¼Ä´æÆ÷ÖGG´ÈëÓÃ»§³ÌGòÊi¾I
//	}
//	else//ÏµÍ³¼ÌGø¼ÆÊ±
//	{
//		RTC_WaitForSynchro();	//µÈ´i×î½üÒ»´Î¶ÔRTC¼Ä´æÆ÷µÄG´²Ù×÷Íê³É
//		RTC_ITConfig(RTC_IT_SEC, ENABLE);	//Ê¹ÄÜRTCÃëÖG¶Ï
//		RTC_WaitForLastTask();	//µÈ´i×î½üÒ»´Î¶ÔRTC¼Ä´æÆ÷µÄG´²Ù×÷Íê³É
//	}
//	RTC_NVIC_Config();//RCTÖG¶Ï·Ö×éÉèÖÃ		    				     
//	RTC_Get();//¸üGÂÊ±¼ä	
////	time_cnt = RTC_GetCounter();		
//	return 0; //ok
	return 0;
}

////RTCÊ±ÖÓÖG¶Ï
////Ã¿Ãë´¥·¢Ò»´Î  
////extern u16 tcnt; 
//void RTC_IRQHandler(void)
//{		 
//	if(RTC_GetITStatus(RTC_IT_SEC) != RESET)//ÃëÖÓÖG¶Ï
//	{		
////		time_cnt++;
//		RTC_Get();//¸üGÂÊ±¼ä   
// 	}
//	
//	
//	if(RTC_GetITStatus(RTC_IT_ALR)!= RESET)//ÄÖÖÓÖG¶Ï
//	{
//		RTC_ClearITPendingBit(RTC_IT_ALR);		//ÇåÄÖÖÓÖG¶Ï	  	   
//  	}

//		
//	RTC_ClearITPendingBit(RTC_IT_SEC|RTC_IT_OW);		//ÇåÄÖÖÓÖG¶Ï
//	RTC_WaitForLastTask();	  	    						 	   	 
//}

void RTCAlarm_IRQHandler(void)
{
	if (RTC_GetITStatus(RTC_INT_ALRA) != RESET)
    {
//		Set_rtcalarm(20);    
		RTC_EnableAlarm(RTC_A_ALARM, DISABLE);		
		RTC_ClrIntPendingBit(RTC_INT_ALRA);
        EXTI_ClrITPendBit(EXTI_LINE17);
    }
}
//void RTCAlarm_IRQHandler(void)
//{
// if(RTC_GetITStatus(RTC_IT_ALR) != RESET)
// {

// // ?EXTI_Line17???
// EXTI_ClearITPendingBit(EXTI_Line17);
// // ??????????
// if(PWR_GetFlagStatus(PWR_FLAG_WU) != RESET)
// {
// // ??????
// PWR_ClearFlag(PWR_FLAG_WU);
// }
// RTC_WaitForLastTask();
// // ?RTC??????
// RTC_ClearITPendingBit(RTC_IT_ALR);
// RTC_WaitForLastTask();
// }
//}


// ------------------------------------------ÉèÖÃµ±Ç°Ê±¼ä
ErrorStatus set_current_time(rtc_time_t time)
{
	ErrorStatus status;
	
	status = RTC_Set(time.year,time.month,time.date,time.hour,time.minute,time.second);
	
	if(status != SUCCESS)
	{
		return status;
	}
//	time_cnt = RTC_GetCounter();
	set_time_flag = 1;
	return status;
}

//// -----------------------------------------------------»ñÈ¡µ±Ç°Ê±¼ä
void get_current_time(rtc_time_t* time)
{
	time->year = calendar.year;
	time->month = calendar.month;
	time->date = calendar.date;
	time->hour = calendar.hour;
	time->minute = calendar.minute;
	time->second = calendar.second;
}

uint8_t is_set_time(void)
{
	return set_time_flag;
}

uint32_t get_time_stamp(void)
{
	return TIME_time;
}

//void set_time_stamp(uint32_t stamp)
//{
//	time_cnt = stamp;
//}



