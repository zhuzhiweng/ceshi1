#include "gps.h"


 uint8_t gps_rx_buf[GSP_RX_BUF_SIZE];
 uint8_t gps_frame_buf[GPS_FRAME_MAX_SIZE];
static uint16_t gps_rx_buf_wr_pos = 0;
static uint16_t gps_rx_buf_rd_pos = 0;
static gps_frame_decode_t frame_decode_step = SEEK_FRAME_HEAD;
//static uint8_t head_flag = 0;
static uint16_t head_pos = 0;
//static uint8_t check_end_flag = 0;
//static uint16_t check_end_pos = 0;
//static uint8_t check_code_flag = 0;
static uint8_t check_code = 0;
static uint8_t char_len = 0;

static gps_gpgga_t gps_gpgga;
static gps_gprmc_t gps_gprmc;
static gps_info_t gps_info;

//static GB32960_location_t location;



void UART4_IRQHandler(void)
{
	if(USART_GetIntStatus(UART4, USART_INT_RXDNE) != RESET)  //接收中断
	{
		gps_rx_buf[gps_rx_buf_wr_pos] = USART_ReceiveData(UART4);//(UART4->DR);	//读取接收到的数据
		
		gps_rx_buf_wr_pos++;
		if(gps_rx_buf_wr_pos >= GSP_RX_BUF_SIZE)
		{
			gps_rx_buf_wr_pos = 0;
		}
	//	USART_ClearITPendingBit(USART2, USART_IT_RXNE);
	}	
	if(USART_GetIntStatus(UART4, USART_INT_TXDE) != RESET)
	{
		USART_ConfigInt(UART4, USART_INT_TXDE, DISABLE);
	}	
}

static uint16_t get_frame_size(uint16_t frame_head, uint16_t frame_end)
{
	uint16_t frame_size = 0;
	if(frame_end > frame_head)
	{
		frame_size = frame_end - frame_head + 1;
	}
	else
	{
		if(frame_end < frame_head)
		{
			frame_size = GSP_RX_BUF_SIZE - (frame_head - frame_end) + 1;
		}
		else
		{
			frame_size = 0;
		}
	}
	return frame_size;
}

static int asc2int(char asc)
{
	if(asc<'0')
		return 0;
	else if(asc>='0' && asc<='9')
		return asc-'0';
	else if(asc>='A' && asc<='F')
		return asc-'A'+10;
	else if(asc>='a' && asc<='f')
		return asc-'a'+10;
	else
		return 15;
}

static char IsLeapYear(unsigned int uiYear)
{
	return (((uiYear%4)==0)&&((uiYear%100)!=0))||((uiYear%400)==0);
}

static void UTCTime2LocalTime(int tzi,rtc_time_t st,rtc_time_t *local_time)
{
	//gps_time_t st_l;
	//memset(&st_l,0,sizeof(gps_time_t));
	memcpy(local_time,&st,sizeof(rtc_time_t));

	local_time->hour += tzi;
	int iHourInterval= local_time->hour/24;
	local_time->hour %= 24;
	if(iHourInterval>0)
	{
		local_time->date += 1;
		int iDays= 0;
		switch(local_time->month)
		{
			case 1:
			case 3:
			case 5:
			case 7:
			case 8:
			case 10:
			case 12:
			{
				iDays = 31;
			}break;
			case 2:
			{
				iDays = IsLeapYear(st.year)?29:28;
			}break;
			case 4:
			case 6:
			case 9:
			case 11:
			{
				iDays = 30;
			}break;
		}

		int iInterval= local_time->date-iDays;
		if(iInterval>0)
		{
			local_time->date = iInterval;
			local_time->month += 1;
			iInterval = local_time->month/12;
			local_time->month %= 12;
			if(iInterval>0)
			{
				local_time->year += 1;
			}
		}
	}
}

static void gps_process_gngga(void)
{
	int res;
	if(memcmp((char*)gps_frame_buf,"$GNGGA",6) == 0)
	{
		memset(&gps_gpgga,0,sizeof(gps_gpgga_t));
		res = sscanf((char*)gps_frame_buf,"$GNGGA,%10s,%[^,],%c,%[^,],%c,%u,%u,%f,%f",
						gps_gpgga.utc_time,
						gps_gpgga.latitude,
						&gps_gpgga.ns,
						gps_gpgga.longitude,
						&gps_gpgga.ew,
						&gps_gpgga.Pos_qua,
						&gps_gpgga.Satellite_num,
		        &gps_gpgga.horizontal_precision,
	        	&gps_gpgga.height
						);
		if(res == 9)
		{
      gps_info.height = gps_gpgga.height;
			gps_info.Satellite_num = (uint8_t)gps_gpgga.Satellite_num;
		}
	}
}

static void gps_process_gprmc(void)
{
	int res;
	if(memcmp((char*)gps_frame_buf,"$GNRMC",6) == 0)
	{
		memset(&gps_gprmc,0,sizeof(gps_gprmc_t));
		//res = sscanf((char*)gps_frame_buf,"$GNRMC,%10s,%c,%9s,%c,%10s,%c,%f,%f,%6s,,,%c*%02hhX",
		res = sscanf((char*)gps_frame_buf,"$GNRMC,%10s,%c,%[^,],%c,%[^,],%c,%f,%f,%6s,,,%c",
		gps_gprmc.utc_time,
		&gps_gprmc.status,
		gps_gprmc.latitude,
		&gps_gprmc.ns,
		gps_gprmc.longitude,
		&gps_gprmc.ew,
		&gps_gprmc.speed,
		&gps_gprmc.course,
		gps_gprmc.utc_date,
		&gps_gprmc.mode_indicator
		);
		if((res == 10)&&(gps_gprmc.status == 'A'))
		{
      rtc_time_t utc_time;
			rtc_time_t local_time;
		  utc_time.year = asc2int(gps_gprmc.utc_date[4])*10 + asc2int(gps_gprmc.utc_date[5]);
			utc_time.month = asc2int(gps_gprmc.utc_date[2])*10 + asc2int(gps_gprmc.utc_date[3]);
			utc_time.date = asc2int(gps_gprmc.utc_date[0])*10 + asc2int(gps_gprmc.utc_date[1]);
			utc_time.hour = asc2int(gps_gprmc.utc_time[0])*10 + asc2int(gps_gprmc.utc_time[1]);
			utc_time.minute = asc2int(gps_gprmc.utc_time[2])*10 + asc2int(gps_gprmc.utc_time[3]);
			utc_time.second = asc2int(gps_gprmc.utc_time[4])*10 + asc2int(gps_gprmc.utc_time[5]);
			UTCTime2LocalTime(8,utc_time,&local_time);
			memcpy(&gps_info.time,&local_time,sizeof(rtc_time_t));
			memcpy(&gps_info.latitude,&gps_gprmc.latitude,sizeof(gps_info.latitude));
			memcpy(&gps_info.longitude,&gps_gprmc.longitude,sizeof(gps_info.longitude));
			gps_info.ew = gps_gprmc.ew;
			gps_info.ns = gps_gprmc.ns;
			gps_info.speed = gps_gprmc.speed;
			gps_info.status = gps_gprmc.status;
		}
		else
		{
			gps_info.status = 'V';  //GPS无效
		}
	}
}

void gps_process_frame(void)
{
	while(gps_rx_buf_wr_pos != gps_rx_buf_rd_pos)
	{
		switch(frame_decode_step)
		{
			case SEEK_FRAME_HEAD:
			{
				if(gps_rx_buf[gps_rx_buf_rd_pos] == '$')
				{
					head_pos = gps_rx_buf_rd_pos;
					char_len = 0;
					frame_decode_step = SEEK_ASTERISK_POS;
				}
			}break;
			case SEEK_ASTERISK_POS:
			{
				if(gps_rx_buf[gps_rx_buf_rd_pos] == '*')
				{
					//check_end_pos = gps_rx_buf_rd_pos;
					frame_decode_step = SEEK_CHECK_HIGHT;
				}
				else
				{
					if(gps_rx_buf[gps_rx_buf_rd_pos] == '$')
					{
						head_pos = gps_rx_buf_rd_pos;
						char_len = 0;
					}
					else
					{
						char_len++;
						if(char_len >= GPS_FRAME_MAX_SIZE)//最多找100个字节，100个字节没找到放弃该帧数据
						{
							frame_decode_step = SEEK_FRAME_HEAD;
						}
				  }
				}
			}break;
			case SEEK_CHECK_HIGHT:
			{
				check_code = 0;
				check_code |= ((asc2int(gps_rx_buf[gps_rx_buf_rd_pos]) << 4) & 0xF0);
				frame_decode_step = SEEK_CHECK_LOW;
			}break;
			case SEEK_CHECK_LOW:
			{
				check_code |= (asc2int(gps_rx_buf[gps_rx_buf_rd_pos]) & 0x0F);
				char_len = 0;
				frame_decode_step = SEEK_END_CHAR;
			}break;
			case SEEK_END_CHAR:
			{
				if(gps_rx_buf[gps_rx_buf_rd_pos] == '\n')
				{
					uint8_t frame_len;
					uint8_t check_res;
					frame_len = get_frame_size(head_pos, gps_rx_buf_rd_pos);
					if(frame_len <= GPS_FRAME_MAX_SIZE)
					{
						for(uint8_t i = 0; i < frame_len; i++)
						{
							gps_frame_buf[i] = gps_rx_buf[head_pos];
							head_pos++;
							if(head_pos>=GSP_RX_BUF_SIZE)
							{
								head_pos=0;
							}
						}
						
						check_res = gps_frame_buf[1];
						for(uint8_t i = 2; gps_frame_buf[i] != '*'; i++)
						{
							check_res ^= gps_frame_buf[i];
						}
						
						if(check_res == check_code)
						{
							gps_process_gngga();
							gps_process_gprmc();
						}
				  }

					frame_decode_step = SEEK_FRAME_HEAD;
				}
				else
				{
					char_len++;
					if(char_len>2)//最多找两个字节，两个字节没找到放弃该帧数据
					{
						frame_decode_step = SEEK_FRAME_HEAD;
					}
				}
			}break;
			
			default:
			{
				frame_decode_step = SEEK_FRAME_HEAD;
			}break;
		}
		
		gps_rx_buf_rd_pos++;
		if(gps_rx_buf_rd_pos >= GSP_RX_BUF_SIZE)
		{
			gps_rx_buf_rd_pos = 0;
		}
	}
}

void get_gps_info(gps_info_t *gps)
{
	memcpy(gps,&gps_info,sizeof(gps_info_t));
}

uint8_t get_gps_time(rtc_time_t *time)
{
  uint8_t res=0;
	if(gps_info.status=='A')
	{
	  memcpy(time,&gps_info.time,sizeof(rtc_time_t));
		res=1;
	}
	return res;
}


void gps_pwr_turn_off(void)
{
	GPIO_ResetBits(GPIOB, GPIO_PIN_9);
}
void gps_pwr_turn_on(void)
{
	GPIO_SetBits(GPIOB, GPIO_PIN_9);
}

//初始化IO 串口2 
//bound:波特率
void gps_uart_init(u32 bound)
{
	GPIO_InitType	GPIO_InitStructure;
	NVIC_InitType 	NVIC_InitStructure;
	USART_InitType 	USART_InitStructure;
	
	RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_UART4, ENABLE);
	RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOC, ENABLE);
	
	GPIO_InitStructure.Pin        = GPIO_PIN_10;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitPeripheral(GPIOC, &GPIO_InitStructure);
	
	GPIO_InitStructure.Pin        = GPIO_PIN_11;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitPeripheral(GPIOC, &GPIO_InitStructure);
	
    USART_InitStructure.BaudRate            = bound;
    USART_InitStructure.WordLength          = USART_WL_8B;
    USART_InitStructure.StopBits            = USART_STPB_1;
    USART_InitStructure.Parity              = USART_PE_NO;
    USART_InitStructure.HardwareFlowControl = USART_HFCTRL_NONE;
    USART_InitStructure.Mode                = USART_MODE_RX | USART_MODE_TX;
	
	USART_Init(UART4, &USART_InitStructure);

    /* Enable the USARTy Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel            = UART4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelCmd         = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
		
    USART_ConfigInt(UART4, USART_INT_RXDNE, ENABLE);
//    USART_ConfigInt(UART4, USART_INT_TXDE, ENABLE);
	
	USART_Enable(UART4, ENABLE);
	
//	GPIO_InitTypeDef GPIO_InitStructure;
//	NVIC_InitTypeDef NVIC_InitStructure;
//	USART_InitTypeDef USART_InitStructure;
//	
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4,ENABLE);
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
//	
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_Init(GPIOC,&GPIO_InitStructure);

//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//GPIO_Mode_IN_FLOATING;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_Init(GPIOC,&GPIO_InitStructure);

//	USART_InitStructure.USART_BaudRate = bound;
//	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
//	USART_InitStructure.USART_StopBits = USART_StopBits_1;
//	USART_InitStructure.USART_Parity = USART_Parity_No;
//	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None ;
//	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
//	USART_Init(UART4,&USART_InitStructure);
	
//	USART_Cmd(UART4,ENABLE);

//	NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
//	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
//	
//	USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);
//	USART_ITConfig(UART4, USART_IT_TXE, DISABLE);
}

void gps_pwr_init(void)
{
	GPIO_InitType	GPIO_InitStructure;
	RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOB, ENABLE);
	GPIO_InitStructure.Pin        = GPIO_PIN_9;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitPeripheral(GPIOB, &GPIO_InitStructure);
	
	gps_pwr_turn_on();
	
	
//	GPIO_InitTypeDef  GPIO_InitStructure;
//	
//  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE );//PORTB时钟使能 

//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//输出
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//  GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化
	
//	gps_pwr_turn_on();
}



