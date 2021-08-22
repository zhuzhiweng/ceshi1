#include "power_ctl.h"
#include "sim_net.h"
#include "GB17691A.h"
#include "spi_flash.h"
#include "sleep.h"
#include "Bsp.h"
#include "n32g45x_adc.h"
#include "UART.h"
//#include "stm32f10x_flash.h"

//#include "stm32f10x_bkp.h"

static uint8_t power_state = 0; //0:电源已经断开，或即将断开 1：电源已经打开
extern uint8_t can_state;
extern uint8_t PenaltyZoneFlag;
extern uint32_t big_heart_time_stamp;

uint8_t get_3v3_state(void)
{
	uint8_t res;
	res = GPIO_ReadOutputDataBit(GPIOB, GPIO_PIN_9);
	return res;
}

void turn_off_3v3(void)
{
	GPIO_ResetBits(GPIOB, GPIO_PIN_9);
}
void turn_on_3v3(void)
{
	GPIO_SetBits(GPIOB, GPIO_PIN_9);
}

void turn_on_sim_3v8(void)
{
	if(PenaltyZoneFlag != 1)
	{
		GPIO_SetBits(GPIOB, GPIO_PIN_5);
	}
}

void turn_off_sim_3v8(void)
{
	GPIO_ResetBits(GPIOB, GPIO_PIN_5);
}

void power_ctl_gpio_init(void)
{
	GPIO_InitType GPIO_InitStructure;

	RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOB, ENABLE);
	
	GPIO_InitStructure.Pin        = GPIO_PIN_5;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitPeripheral(GPIOB, &GPIO_InitStructure);
	
	turn_off_sim_3v8();
	
	GPIO_InitStructure.Pin        = GPIO_PIN_9;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitPeripheral(GPIOB, &GPIO_InitStructure);
		
	turn_off_3v3();
}

uint8_t get_power_state(void)
{
	return power_state;
}
extern uint8_t alarm_state_flag;
static uint8_t offset=0;
void is_sleep(void)
{
		uint16_t Temp_Value;  //ADC原始值
		float ADC_Value=0.00; //内部电压值
		u8 vin_state=0;
		uint8_t vin_info[17]={0};

		Temp_Value = ADC_GetDat(ADC1);	         //获取ADC1转换值
		ADC_Value = (3.3/4096)*Temp_Value;
		if( Temp_Value < 0x322 )
		{
//				flag_dismon=1;
			vin_state=get_vin(vin_info);
				if(vin_state)
			{
				if(	alarm_state_flag == 0)
						alarm_state_flag = 1;

				if( get_3v3_state() == 1)		//有电
				{
					vin_state=get_vin(vin_info);   	//获取VIN
					if(!vin_state)			//如果没有设置VIN，不发送备案信息
					{	
						if(	alarm_state_flag == 1)
								alarm_state_flag = 2;
					}
					else
					{
						if(	alarm_state_flag == 1)
								alarm_state_flag = 3;
					}
				}
				else
				{
					turn_on_3v3();
				}
			}
		}
		else
		{
				alarm_state_flag=0;
		}
	

	//uint8_t can_state; //= get_can_status();
	uint8_t big_heart_state = get_big_heart_flag();
	if((can_state==0) && (get_login_state()==0) &&(big_heart_state==0) && (get_net_status()==SIM_NET_TURN_OFF) && ((alarm_state_flag%2) ==0 )) //alarm_state_flag = 0,2,4可休眠
	{
		turn_off_sim_3v8();
		turn_off_3v3();
		
//    /*for save big heart time stamp*/		
//		BKP_WriteBackupRegister(BKP_DR2, 0Xa5a5);
//		BKP_WriteBackupRegister(BKP_DR3, (uint16_t)(big_heart_time_stamp>>16));
//		BKP_WriteBackupRegister(BKP_DR4, (uint16_t)(big_heart_time_stamp));	
		
		power_state = 0;
		sleep_s(20);
	}
	else
	{
		if((can_state==1) || (big_heart_state==1) ||(alarm_state_flag == 3))  //3为有
		{
			turn_on_3v3(); 
			if(power_state)
			{	//	 
					if(offset==29)
					{
							char test_data[50]={0};
							int res = snprintf(&test_data[0],sizeof(test_data)-1,"\r\nThis version is %s-%08X\n",__CUSTOMER_NAME,CURRENT_VER);
							USARTSendOut(RS232_USART,(char*)test_data,strlen((char*)test_data));
							offset=0xff;
					}
					if(offset!=0xff)
						offset++;
			}
			
			power_state = 1;
		}
		else
		{
			if(power_state)
			{	//	 			
					if(offset==29)
					{
							char test_data[50]={0};
							int res = snprintf(&test_data[0],sizeof(test_data)-1,"\r\nThis version is %s-%08X\n",__CUSTOMER_NAME,CURRENT_VER);
							USARTSendOut(RS232_USART,(char*)test_data,strlen((char*)test_data));
							offset=0xff;
					}
					if(offset!=0xff)
						offset++;
			}			
			
			
			
		  power_state = 0;
		}
	}
}
//单片机软件复位
void MCU_RESET(void)
{
__set_FAULTMASK(1); //屏蔽所有中断
NVIC_SystemReset(); //复位
}
