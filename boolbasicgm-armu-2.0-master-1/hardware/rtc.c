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
//	NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;					//RTCȫ���G��
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;		//��ռ���ȼ�1λ,�����ȼ�3λ
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;				//��ռ���ȼ�0λ,�����ȼ�4λ
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;					//ʹ�ܸ�ͨ���G��
//	NVIC_Init(&NVIC_InitStructure);									//���INVIC_InitStruct�Gָ���Ĳ��i��ʼ������NVIC�Ĵ���
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
	if(year%4==0) //��G��ܱ�4���i
	{ 
		if(year%100==0) 
		{ 
			if(year%400==0)return 1;//�����00��β,��Ҫ�ܱ�400���i 	   
			else return 0;   
		}else return 1;   
	}else return 0;	
}	



//����ʱ��
//�������ʱ��ת��Ϊ����
//��1970��1��1��Ϊ��׼
//1970~2099��Ϊ�Ϸ���I
//����ֵ:0,�ɹ�;����:�������.
//�·I�i�I��											 
u8 const table_week[12]={0,3,3,6,1,4,6,2,5,0,3,5}; //��GS�i�i�I��	  
//ƽ����·I���ڱ�
const u8 mon_table[12]={31,28,31,30,31,30,31,31,30,31,30,31};
u32 TIME_time =0xFFFFFFFF;


ErrorStatus RTC_Set(u8 year,u8 smon,u8 sday,u8 hour,u8 min,u8 sec)
{
	u16 t,syear;
	u32 seccount=0;
	syear=2000+year;
	if(syear<1970||syear>2099) return ERROR;	   
	for(t=1970;t<syear;t++)	//�����G��I���������
	{
		if(Is_Leap_Year(t))seccount+=31622400;//����������i
		else seccount+=31536000;			  //ƽ��������i
	}
	smon-=1;
	for(t=0;t<smon;t++)	   //��ǰ���·I�������i���
	{
		seccount+=(u32)mon_table[t]*86400;//�·I�����i���
		if(Is_Leap_Year(syear)&&t==1)seccount+=86400;//����2�·I����һ��������i	   
	}
	seccount+=(u32)(sday-1)*86400;//��ǰ�����ڵ������i��� 
	seccount+=(u32)hour*3600;//G�ʱ�����i
  seccount+=(u32)min*60;	 //���������i
	seccount+=sec;//�������Ӽ���ȥ
	TIME_time = seccount + 2 ; 
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);	//ʹ��PWR��BKP����ʱ��  
//	PWR_BackupAccessCmd(ENABLE);	//ʹ��RTC�ͺ󱸼Ĵ������� 
//	RTC_SetCounter(seccount);	//����RTC���i����ֵ

//	RTC_WaitForLastTask();	//�ȴi���һ�ζ�RTC�Ĵ�����G��������  	
	RTC_Get();
	return SUCCESS;	    
}
//���������G��ڼ�
//��������:���빫�����ڵõ�G���(ֻ��G�1901-2099��)
//������i������������ 
//����ֵ��G��ں�	
u8 RTC_Get_Week(u16 year,u8 month,u8 day)
{	
	u16 temp2;
	u8 yearH,yearL;
	
	yearH=year/100;	yearL=year%100; 
	// ���Ϊ21����,��I�i��100  
	if (yearH>19)yearL+=100;
	// ���i�����iֻ��1900��֮���  
	temp2=yearL+yearL/4;
	temp2=temp2%7; 
	temp2=temp2+day+table_week[month-1];
	if (yearL%4==0&&month<3)temp2--;
	return(temp2%7);
}			  



//�õ���ǰ��ʱ��
//����ֵ:0,�ɹ�;����:�������.
u8 RTC_Get(void)
{
	static u16 daycnt=0;
	u32 timecount=0; 
	u32 temp=0;
	u16 temp1=0;	  
    timecount=TIME_time;	 
 	temp=timecount/86400;   //�õ����i(�����i��Ӧ��)
	if(daycnt!=temp)//���iһ����
	{	  
		daycnt=temp;
		temp1=1970;	//��1970�꿪ʼ
		while(temp>=365)
		{				 
			if(Is_Leap_Year(temp1))//������
			{
				if(temp>=366)temp-=366;//����������i
				else {temp1++;break;}  
			}
			else temp-=365;	  //ƽ�� 
			temp1++;  
		}   
		calendar.year=temp1 - 2000;//�õ���I
		temp1=0;
		while(temp>=28)//���i��һ����
		{
			if(Is_Leap_Year(calendar.year)&&temp1==1)//�����ǲ�������/2�·I
			{
				if(temp>=29)temp-=29;//����������i
				else break; 
			}
			else 
			{
				if(temp>=mon_table[temp1])temp-=mon_table[temp1];//ƽ��
				else break;
			}
			temp1++;  
		}
		calendar.month=temp1+1;	//�õ��·I
		calendar.date=temp+1;  	//�õ����� 
	}
	temp=timecount%86400;     		//�õ������i   	   
	calendar.hour=temp/3600;     	//G�ʱ
	calendar.minute=(temp%3600)/60; 	//����	
	calendar.second=(temp%3600)%60; 	//����
	return 0;
}	


//RTC��ʼ��
//����ֵ:0,��ʼ���ɹ�;
//       1,LSE����ʧ��;
//       2,�����ʼ��ģʽʧ��;
u8 My_RTC_Init(void)
{
	RTC_DateType RTC_DateStructure;
	RTC_TimeType RTC_TimeStructure;
	RTC_InitType RTC_InitStructure;
	RTC_AlarmType RTC_AlarmStructure;
	
	
	//����ǲ��ǵ�һ������ʱ��
//	u8 temp=0;	
	RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_PWR | RCC_APB1_PERIPH_BKP, ENABLE);	//ʹ��PWR��BKP����ʱ��
	RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_AFIO, ENABLE);	
	PWR_BackupAccessEnable(ENABLE);//ʹ�ܺ󱸼Ĵ������� 	
	
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
	
	
//	if(BKP_ReadBackupRegister(BKP_DR1) != 0x5050)		//��ָ���ĺ󱸼Ĵ����G�����i�I:��������G����ָ���i�I�����
//	{ 			
//		BKP_DeInit();	//��λ���I���� 	
//		RCC_LSEConfig(RCC_LSE_ON);	//�����ⲿ���پ���(LSE),ʹ��������پ���
//		while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET);	//���ָ����RCC��־λ�������,�ȴi���پ����G�
//   
//		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);		//����RTCʱ��(RTCCLK),ѡ��LSE��ΪRTCʱ��    
//		RCC_RTCCLKCmd(ENABLE);	//ʹ��RTCʱ��  
//		RTC_WaitForLastTask();	//�ȴi���һ�ζ�RTC�Ĵ�����G��������
//		RTC_WaitForSynchro();		//�ȴiRTC�Ĵ���ͬ��  
//		RTC_ITConfig(RTC_IT_SEC, ENABLE);		//ʹ��RTC���G��
//		RTC_WaitForLastTask();	//�ȴi���һ�ζ�RTC�Ĵ�����G��������
//		RTC_EnterConfigMode();/// ��G�����	
//		RTC_SetPrescaler(32767); //����RTCԤ��Ƶ��ֵ
//		RTC_WaitForLastTask();	//�ȴi���һ�ζ�RTC�Ĵ�����G��������
//		RTC_Set(19,2,1,10,0,55);  //����ʱ��				
//		RTC_ExitConfigMode(); //�˳�����ģʽ  
//		BKP_WriteBackupRegister(BKP_DR1, 0X5050);	//��ָ���ĺ󱸼Ĵ����GG����û���G��i�I
//	}
//	else//ϵͳ��G���ʱ
//	{
//		RTC_WaitForSynchro();	//�ȴi���һ�ζ�RTC�Ĵ�����G��������
//		RTC_ITConfig(RTC_IT_SEC, ENABLE);	//ʹ��RTC���G��
//		RTC_WaitForLastTask();	//�ȴi���һ�ζ�RTC�Ĵ�����G��������
//	}
//	RTC_NVIC_Config();//RCT�G�Ϸ�������		    				     
//	RTC_Get();//��G�ʱ��	
////	time_cnt = RTC_GetCounter();		
//	return 0; //ok
	return 0;
}

////RTCʱ���G��
////ÿ�봥��һ��  
////extern u16 tcnt; 
//void RTC_IRQHandler(void)
//{		 
//	if(RTC_GetITStatus(RTC_IT_SEC) != RESET)//�����G��
//	{		
////		time_cnt++;
//		RTC_Get();//��G�ʱ��   
// 	}
//	
//	
//	if(RTC_GetITStatus(RTC_IT_ALR)!= RESET)//�����G��
//	{
//		RTC_ClearITPendingBit(RTC_IT_ALR);		//�������G��	  	   
//  	}

//		
//	RTC_ClearITPendingBit(RTC_IT_SEC|RTC_IT_OW);		//�������G��
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


// ------------------------------------------���õ�ǰʱ��
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

//// -----------------------------------------------------��ȡ��ǰʱ��
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



