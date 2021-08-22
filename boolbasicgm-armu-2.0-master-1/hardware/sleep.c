#include "sleep.h" 
#include "RTC.h"


static void Sys_Standby(void)
{
	
	RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_PWR, ENABLE);//使能PWR外设时钟
	PWR_EnterStandbyState();	//进入待命（STANDBY）模式 	
}

//系统进入待机模式
static void Sys_Enter_Standby(void)
{
	RCC_EnableAPB2PeriphReset(0X01FC, DISABLE);
	Sys_Standby();
}

void sleep_s(uint8_t time_s)
{
	Set_rtcalarm(time_s);
	RTC_EnableAlarm(RTC_A_ALARM, ENABLE);	
	Sys_Enter_Standby();
}

