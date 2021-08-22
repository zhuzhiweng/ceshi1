#ifndef __RTC_H
#define __RTC_H	 
#include "sys.h" 
#include "n32g45x_rtc.h"
#include "n32g45x_bkp.h"
#include "n32g45x_pwr.h"
#include "n32g45x_exti.h"

typedef struct
{
	uint8_t year;
	uint8_t month;
	uint8_t date;
	uint8_t hour;
	uint8_t minute;
	uint8_t second;
}rtc_time_t;


#define EPOCH_YEAR  (1970U)
#define DAYS_IN_AN_YEAR  (365U)
#define SECONDS_IN_A_DAY  (24U*60U*60U)
#define SECONDS_IN_AN_HOUR  (60U*60U)
#define SECONDS_IN_A_MINUTE  (60U)


extern u8 const mon_table[12];											//�·��������ݱ�
void Disp_Time(u8 x,u8 y,u8 size);										//���ƶ�λ�ÿ�ʼ��ʾʱ��
void Disp_Week(u8 x,u8 y,u8 size,u8 lang);								//��ָ��λ����ʾ����
u8 My_RTC_Init(void);        											//��ʼ��RTC,����0,ʧ��;1,�ɹ�;
u8 Is_Leap_Year(u16 year);												//ƽ��,�����ж�
u8 RTC_Get(void);         												//����ʱ��   
u8 RTC_Get_Week(u16 year,u8 month,u8 day);
ErrorStatus RTC_Set(u8 year,u8 smon,u8 sday,u8 hour,u8 min,u8 sec);		//����ʱ��

ErrorStatus set_current_time(rtc_time_t time);
void get_current_time(rtc_time_t* time);
uint32_t get_time_stamp(void);
void set_time_stamp(uint32_t stamp);
uint8_t is_set_time(void);

void Set_rtcalarm(uint8_t time_s);

#endif

















