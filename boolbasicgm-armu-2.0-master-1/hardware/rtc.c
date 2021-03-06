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
//	NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;					//RTC全局諫断
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;		//先占优先级1位,从优先级3位
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;				//先占优先级0位,从优先级4位
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;					//使能该通道諫断
//	NVIC_Init(&NVIC_InitStructure);									//根綢NVIC_InitStruct諫指定的参蔵初始化外设NVIC寄存器
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
	if(year%4==0) //必G肽鼙?4整砳
	{ 
		if(year%100==0) 
		{ 
			if(year%400==0)return 1;//如果以00结尾,还要能被400整砳 	   
			else return 0;   
		}else return 1;   
	}else return 0;	
}	



//设置时钟
//把输入的时钟转换为秒钟
//以1970年1月1日为基准
//1970~2099年为合法年稩
//返回值:0,成功;其他:错误代码.
//月稩蔵綢表											 
u8 const table_week[12]={0,3,3,6,1,4,6,2,5,0,3,5}; //月GS読蔵綢表	  
//平年的月稩日期表
const u8 mon_table[12]={31,28,31,30,31,30,31,31,30,31,30,31};
u32 TIME_time =0xFFFFFFFF;


ErrorStatus RTC_Set(u8 year,u8 smon,u8 sday,u8 hour,u8 min,u8 sec)
{
	u16 t,syear;
	u32 seccount=0;
	syear=2000+year;
	if(syear<1970||syear>2099) return ERROR;	   
	for(t=1970;t<syear;t++)	//把所覩年稩的秒钟相加
	{
		if(Is_Leap_Year(t))seccount+=31622400;//闰年的秒钟蔵
		else seccount+=31536000;			  //平年的秒钟蔵
	}
	smon-=1;
	for(t=0;t<smon;t++)	   //把前面月稩的秒钟蔵相加
	{
		seccount+=(u32)mon_table[t]*86400;//月稩秒钟蔵相加
		if(Is_Leap_Year(syear)&&t==1)seccount+=86400;//闰年2月稩增加一天的秒钟蔵	   
	}
	seccount+=(u32)(sday-1)*86400;//把前面日期的秒钟蔵相加 
	seccount+=(u32)hour*3600;//G∈泵胫邮i
  seccount+=(u32)min*60;	 //分钟秒钟蔵
	seccount+=sec;//最后的秒钟加上去
	TIME_time = seccount + 2 ; 
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);	//使能PWR和BKP外设时钟  
//	PWR_BackupAccessCmd(ENABLE);	//使能RTC和后备寄存器访问 
//	RTC_SetCounter(seccount);	//设置RTC计蔵器的值

//	RTC_WaitForLastTask();	//等磇最近一次对RTC寄存器的G床僮魍瓿?  	
	RTC_Get();
	return SUCCESS;	    
}
//获得现在是G瞧诩?
//功能描述:输入公历日期得到G瞧?(只允G?1901-2099年)
//输入参蔵：公历年月日 
//返回值：G瞧诤?	
u8 RTC_Get_Week(u16 year,u8 month,u8 day)
{	
	u16 temp2;
	u8 yearH,yearL;
	
	yearH=year/100;	yearL=year%100; 
	// 如果为21世纪,年稩蔵加100  
	if (yearH>19)yearL+=100;
	// 所筰闰年蔵只算1900年之后的  
	temp2=yearL+yearL/4;
	temp2=temp2%7; 
	temp2=temp2+day+table_week[month-1];
	if (yearL%4==0&&month<3)temp2--;
	return(temp2%7);
}			  



//得到当前的时间
//返回值:0,成功;其他:错误代码.
u8 RTC_Get(void)
{
	static u16 daycnt=0;
	u32 timecount=0; 
	u32 temp=0;
	u16 temp1=0;	  
    timecount=TIME_time;	 
 	temp=timecount/86400;   //得到天蔵(秒钟蔵对应的)
	if(daycnt!=temp)//超筰一天了
	{	  
		daycnt=temp;
		temp1=1970;	//从1970年开始
		while(temp>=365)
		{				 
			if(Is_Leap_Year(temp1))//是闰年
			{
				if(temp>=366)temp-=366;//闰年的秒钟蔵
				else {temp1++;break;}  
			}
			else temp-=365;	  //平年 
			temp1++;  
		}   
		calendar.year=temp1 - 2000;//得到年稩
		temp1=0;
		while(temp>=28)//超筰了一个月
		{
			if(Is_Leap_Year(calendar.year)&&temp1==1)//当年是不是闰年/2月稩
			{
				if(temp>=29)temp-=29;//闰年的秒钟蔵
				else break; 
			}
			else 
			{
				if(temp>=mon_table[temp1])temp-=mon_table[temp1];//平年
				else break;
			}
			temp1++;  
		}
		calendar.month=temp1+1;	//得到月稩
		calendar.date=temp+1;  	//得到日期 
	}
	temp=timecount%86400;     		//得到秒钟蔵   	   
	calendar.hour=temp/3600;     	//G∈?
	calendar.minute=(temp%3600)/60; 	//分钟	
	calendar.second=(temp%3600)%60; 	//秒钟
	return 0;
}	


//RTC初始化
//返回值:0,初始化成功;
//       1,LSE开启失败;
//       2,进入初始化模式失败;
u8 My_RTC_Init(void)
{
	RTC_DateType RTC_DateStructure;
	RTC_TimeType RTC_TimeStructure;
	RTC_InitType RTC_InitStructure;
	RTC_AlarmType RTC_AlarmStructure;
	
	
	//检查是不是第一次配置时钟
//	u8 temp=0;	
	RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_PWR | RCC_APB1_PERIPH_BKP, ENABLE);	//使能PWR和BKP外设时钟
	RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_AFIO, ENABLE);	
	PWR_BackupAccessEnable(ENABLE);//使能后备寄存器访问 	
	
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
	
	
//	if(BKP_ReadBackupRegister(BKP_DR1) != 0x5050)		//从指定的后备寄存器諫读出蔵綢:读出了与G慈氲闹付ㄊi綢不相乎
//	{ 			
//		BKP_DeInit();	//复位备稩区域 	
//		RCC_LSEConfig(RCC_LSE_ON);	//设置外部低速晶振(LSE),使用外设低速晶振
//		while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET);	//检查指定的RCC标志位设置与否,等磇低速晶振就G?
//   
//		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);		//设置RTC时钟(RTCCLK),选择LSE作为RTC时钟    
//		RCC_RTCCLKCmd(ENABLE);	//使能RTC时钟  
//		RTC_WaitForLastTask();	//等磇最近一次对RTC寄存器的G床僮魍瓿?
//		RTC_WaitForSynchro();		//等磇RTC寄存器同步  
//		RTC_ITConfig(RTC_IT_SEC, ENABLE);		//使能RTC秒諫断
//		RTC_WaitForLastTask();	//等磇最近一次对RTC寄存器的G床僮魍瓿?
//		RTC_EnterConfigMode();/// 允G砼渲?	
//		RTC_SetPrescaler(32767); //设置RTC预分频的值
//		RTC_WaitForLastTask();	//等磇最近一次对RTC寄存器的G床僮魍瓿?
//		RTC_Set(19,2,1,10,0,55);  //设置时间				
//		RTC_ExitConfigMode(); //退出配置模式  
//		BKP_WriteBackupRegister(BKP_DR1, 0X5050);	//向指定的后备寄存器諫G慈胗没С蘂蚴i綢
//	}
//	else//系统继G剖?
//	{
//		RTC_WaitForSynchro();	//等磇最近一次对RTC寄存器的G床僮魍瓿?
//		RTC_ITConfig(RTC_IT_SEC, ENABLE);	//使能RTC秒諫断
//		RTC_WaitForLastTask();	//等磇最近一次对RTC寄存器的G床僮魍瓿?
//	}
//	RTC_NVIC_Config();//RCT諫断分组设置		    				     
//	RTC_Get();//更G率奔?	
////	time_cnt = RTC_GetCounter();		
//	return 0; //ok
	return 0;
}

////RTC时钟諫断
////每秒触发一次  
////extern u16 tcnt; 
//void RTC_IRQHandler(void)
//{		 
//	if(RTC_GetITStatus(RTC_IT_SEC) != RESET)//秒钟諫断
//	{		
////		time_cnt++;
//		RTC_Get();//更G率奔?   
// 	}
//	
//	
//	if(RTC_GetITStatus(RTC_IT_ALR)!= RESET)//闹钟諫断
//	{
//		RTC_ClearITPendingBit(RTC_IT_ALR);		//清闹钟諫断	  	   
//  	}

//		
//	RTC_ClearITPendingBit(RTC_IT_SEC|RTC_IT_OW);		//清闹钟諫断
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


// ------------------------------------------设置当前时间
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

//// -----------------------------------------------------获取当前时间
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



