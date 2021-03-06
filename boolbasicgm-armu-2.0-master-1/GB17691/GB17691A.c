#include "GB17691A.h"
//#include "bsp.h"
#include "cmsis_os.h"
#include "spi_flash.h"
#include "power_ctl.h"
#include "user_config.h"
#include "n32g45x_bkp.h"
#include "LKT4305_IIC.h"
#include "TerminalTest.h"
#include <stdarg.h>
#include "obd.h"
#include "UDS.h"
#include "UART.h"
//#include "usb_data_process.h"

//static const uint8_t device_type_manage[4]={DEVICE_TYPE0,DEVICE_TYPE1,DEVICE_TYPE2,DEVICE_TYPE3};
//static const uint32_t ver_manage[64]={ VER0,VER1,VER2,VER3,VER4,VER5,VER6,VER7,VER8,VER9,VER10,VER11,VER12,VER13,VER14,VER15,
//																			 VER16,VER17,VER18,VER19,VER20,VER21,VER22,VER23, VER24,VER25,VER26,VER27,VER28,VER29,VER30,VER31,
//	                                     VER32,VER33,VER34,VER35,VER36,VER37,VER38,VER39, VER40,VER41,VER42,VER43,VER44,VER45,VER46,VER47,
//	                                      VER48,VER49,VER50,VER51,VER52,VER53,VER54,VER55, VER56,VER57,VER58,VER59,VER60,VER61,VER62,VER63};
//uint8_t vin_info[17] = {0x4C, 0x50, 0x48, 0x46, 0x41, 0x33, 0x42, 0x44, 0x30, 0x48, 0x59, 0x31, 0x37, 0x30, 0x32, 0x33, 0x35};    //vin信息
//uint8_t vin_info[17] = {0x4C, 0x44, 0x50, 0x46, 0x41, 0x41, 0x41, 0x42, 0x36, 0x48, 0x46, 0x31, 0x36, 0x31, 0x32, 0x35, 0x37};
//uint8_t vin_info[17] = {0x4C, 0x44, 0x4C, 0x41, 0x34, 0x32, 0x41, 0x44, 0x34, 0x4A, 0x31, 0x30, 0x30, 0x30, 0x30, 0x30, 0x31};


//enum
//{
//    NO_USE,
//    UPDATA_FIRMWARE,
//    SHUTDOWN,
//    RESTART,
//    RESET_DATA,
//		OFFLINE,
//} CONTROL_ID ;          //控制命令

typedef enum
{
   NO_USE,
   SETUPDATA_FIRMWARE,//【命令参数：http升级，第一个字节为命令，第二个字节开始为URL】
   SETDO,//【一个字节，SHUTDOWN1:1, RESTART:2,    RESET_DATA:3,    OFFLINE,:4,  Relog:5】

   SETVIN,//【命令参数：VIN】
   SETUDS,//【命令参数：第一个字节1为开。0为关，第二个字节为UDS号码，第三个，三个第四】
   SETSEND,//【三个字节，1代表使能，0为失能，OFF1939SEND，ON1939SEND，OFF15765SENG09，ON15765SENG09，OFF15765SEND01，ON15765SEND01，】

GETVER,//【*用户看*应答，（单字节版本号-设备类型-设备名-版本号）】
GETVIN,//【应答命令参数：VIN】
GETUDS,//【应答命令参数：UDS信息】

GETSENDSTATE,//【应答命令参数：参考SETSEND】
GETTEMSTATE,//【应答命令参数：状态信息】

GETOBDDATA,//【*用户看*应答命令参数：candata】
GETCANDATA,//【应答命令参数：candata】
GETCANSTATE,//【应答命令参数：第一个字节6.14的协议内容，第二个字节6.14的协议内容，
         //第三个字节19的协议内容，第四个字节19的协议内容，
        //第五个字节311的协议内容，第六个字节311的协议内容，
        //0代表没有协议，1代表15765，2代表1939】
SETCLEARDTC,//【应答命令参数：第一个字节6.14的1939清除故障，第二个字节6.14上15765清除故障，
        //第三个字节19的1939清除故障，第四个字节19的15765清除故障，
       //第五个字节311的1939清除故障，第六个字节311的15765清除故障容，】
} CONTROL_ID ;          //控制命令

typedef enum
{
    CON_SUCCESS=1,
    CON_FAILED,
} CONTROL_STATE	;          //控制命令


extern uint8_t comupdate_mode_flag;
extern uint8_t vin_info[17];
//static uint8_t read_vin_state = 0;            //读VIN状态  0:不读VIN状态  1:读VIN状态
static uint8_t login_state = 0;               //登入状态   0:不登入状态   1:登入状态
uint8_t timing_state = 0;              //校时状态   0:非校时状态   1:校时状态
static uint8_t record_state=0;					// 备案状态， 0未备案		 1已备案
static uint8_t lkt4305_state=0;					// 备案状态， 0不正常		 1正常
//static  uint8_t flag_nosm2=0;				//			0 签名上报   1不带签名上报
//static uint8_t alarm_state = 0;              //报警状态
static uint16_t lognum = 0;                   //登入次数
static uint8_t send_data_cnt = 0;             //发数计数
uint8_t GB17691_CMD_data[256];             //命令缓冲256
static uint16_t gb17691_rd_pos = 0;           //读位置
static uint16_t cmd_data_len;                 //命令数据长度
static uint8_t check_cmd_info_flag = 0;       //检查命令信息标志
static uint8_t CMD_check_dealy;               //命令检查延时

//static uint32_t	pre_resend_timestamp;
//static uint8_t pre_resend_flag = 0;
static uint8_t GB17691_send_enable = 0;       //GB17691发送使能 0:不使能 1 :使能

//static uint32_t resend_record_addr = RESEND_RECORD_START_ADDR; //保存补发地址
//static uint32_t resend_wrtie_size = 0;                          //补发写大小初始为0

static uint32_t realtime_upload_flag_addr[HEART_INTERVAL];   //实时上报标记存储缓冲
static uint8_t realtime_load_cnt = 0;                        //实时上报计数
static uint32_t resend_msg_flag_addr[RESND_MSG_MAX_CNT];     //补发标记存储缓冲
static uint8_t resend_msg_cnt = 0;                           //补发计数

//static uint8_t usb_send_enable = 0;
static uint8_t terminal_mode = TERMINAL_FREE;  //0:无连接，1：网络，2：USB
static uint32_t old_time_stamp;                //旧时间戳
//static uint32_t alarm_data_stamp;              //报警数据戳
static uint32_t realtime_data_stamp;           //实时数据戳
//static uint32_t heart_stamp;                   //心跳戳
//static uint32_t alarm_resend_stamp;            //报警补发戳
static uint8_t send_heart_flag = 0;            //送心跳标记
static uint8_t resend_flash_addr_end_flag = 0;  //查找补发FLASH到结束地址,则置1 , 在接收到心跳时置0,
//static uint8_t key_state = 0;                  //钥匙状态
//static uint8_t charge_state = 0;               //充电状态

//static uint32_t current_alarm_flag = 0;        //当前报警标志
//static uint8_t current_max_alarm_level = 0;    //当前最大报警级别
//static uint8_t alarm_real_time_send_cnt;
//static uint8_t alarm_resend_cnt;                //报警补发计数
//static uint8_t alarm_resend_over;               //报警补发结束

static uint32_t realtime_data_wr_flash_addr = RESEND_DATA_START_ADDR; //实时数据存储地址, 初始为补发数据开始地址
static uint32_t resend_data_rd_flash_addr = RESEND_DATA_START_ADDR;   //补发数据读FLASH地址, 初始为补发数据开始地址
//static uint32_t alarm_data_wr_flash_addr = ALARM_DATA_START_ADDR;     //报警数据存储地址, 初始为报警数据开始地址
//static uint32_t alarm_data_rd_flash_addr = ALARM_DATA_START_ADDR;     //报警数据读FLASH地址,初始为报警数据开始地址
static uint8_t ckeck_flash_flag = 0;              //检查FLASH标记

uint8_t GB17691_data_package[1024];               //GB17691数据包缓存 ,大小512

uint8_t data_sign[64];       		//签名信息      
uint8_t SM2ID[16];						//SM2ID信息
uint8_t platformPublicKeyData[64];  //平台公钥


uint8_t big_heart_flag=0;                            //1:进入大心跳状态, 0:不进入大心跳    网络管理模块会调用
uint32_t big_heart_time_stamp;                       //大心跳时间戳
//static uint32_t big_heart_temp_time_stamp;           //大心跳临时时间戳

static uint8_t TERMINAL_SOFT_VER=0;
uint16_t INFO_STREAM_NO=1;                         //信息流水号


static uint8_t flag_bool_login = 0;
static GB17691_location_t cur_location;

extern u8 net_state;
uint8_t low_power_state;

static u8 flag_restart_net=0;
static u8 flag_relog_net = 0;
static u8 flag_reset_net = 0;
static u8 flag_answer_net= 0;
static u8 flag_setvin_net= 0;
static u8 flag_ver_net = 0;
static u8 flag_vin_net = 0;

//u8 flag_restart_net_time[6]={0};
u8 flag_answer_net_time[6]={0};
extern u8 can_state;


//for test logout
uint32_t temp_test_stamp=0;
uint32_t test_logout=0;


uint8_t test_flag=0xff;
uint8_t test_flag_num=0;

extern u8 flag_set_vin;   	//设置VIN标志
extern u8 flag_UDSSEND;
u8  Actblue_State=1;//帧指令

//static uint32_t resend_data_stamp;
//
////#pragma pack(1)   //按字节对齐
//extern GB32960_vehicle_data_t vehicle_data;
//extern GB32960_motor_data_t motor_data;
//
//#ifdef _FUEL_CELL_DATA_
//extern GB32960_fuel_data_t fuel_cell_data;
//#endif

//#ifdef _ENGINE_DATA_
//extern GB32960_engine_data_t engine_data;
//#endif

//extern GB32960_extreme_t extreme_data;
//extern GB32960_alarm_t alarm_data;
//extern GB32960_charge_device_voltage_t charge_device_voltage_data;
//extern GB32960_charge_device_temp_t charge_device_temp_data;
////#pragma pack()  //取消指定对齐，恢复缺省对齐

//******艾可蓝数据状态**************//
u8 ACTBLU_date_state=0;//艾可蓝数据状态

#define upload_successful 1//上传成功
#define unregistered      2//未注册
#define Check_failure     3//数据校验失败
#define unknown_failure   4//未知失败
//***********************************//
void ResetData(void)
{
						W25QXX_Erase_Chip_osMutex();
						for(uint8_t i=0 ;i< sizeof(vin_info);i++)
						{
							vin_info[i] = 0xFF;
						}
}

/*for save big heart time stamp*/
static void update_big_heart_time(uint32_t time)
{
    BKP_WriteBkpData(BKP_DAT2, 0xa5a5);
    BKP_WriteBkpData(BKP_DAT3, (uint16_t)(time>>16));
    BKP_WriteBkpData(BKP_DAT4, (uint16_t)(time));
}

static char IsLeapYear(unsigned int uiYear)
{
    return (((uiYear%4)==0)&&((uiYear%100)!=0))||((uiYear%400)==0);
}

static void set_time_preset(int tzi,rtc_time_t st,rtc_time_t *set_time)
{
    memcpy(set_time,&st,sizeof(rtc_time_t));

    //set_time->hour += tzi;

    set_time->second += tzi;
    if(set_time->second >= 60)
    {
        set_time->second -= 60;
        set_time->minute++;
    }

    if(set_time->minute >= 60)
    {
        set_time->minute -= 60;
        set_time->hour++;
    }

    int iHourInterval= set_time->hour/24;
    set_time->hour %= 24;
    if(iHourInterval>0)
    {
        set_time->date += 1;
        int iDays= 0;
        switch(set_time->month)
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
        }
        break;
        case 2:
        {
            iDays = IsLeapYear(st.year)?29:28;
        }
        break;
        case 4:
        case 6:
        case 9:
        case 11:
        {
            iDays = 30;
        }
        break;
        }

        int iInterval= set_time->date-iDays;
        if(iInterval>0)
        {
            set_time->date = iInterval;
            set_time->month += 1;
            iInterval = set_time->month/12;
            set_time->month %= 12;
            if(iInterval>0)
            {
                set_time->year += 1;
            }
        }
    }
}
/************************************************************************
  * @描述:   hex 转ascii
  * @参数:   None
  * @返回值: None
  **********************************************************************/
u16 hex2ascii( u8 *mubiao ,u8 *yuan,u16 BytNo )								//把 stm32 的 can口接收的8个字节转换成16个字节的ascII 形式
{
	u16 j=0; u8 temp_data=0; 
	for(u16 i=0;i<BytNo;i++)
	{
		temp_data=(*(yuan+i))>>4;
		if(temp_data<=9)            //变成ascii数字
		{
			*(mubiao+j) = temp_data + 0x30;
		}
		else 																					//变成ascii大写字母
		{
			*(mubiao+j)= temp_data +	0x37;
		}
		j++;

		temp_data=(*(yuan+i))&0x0f;	
		if(temp_data<=9)            //变成ascii数字
		{
			*(mubiao+j) = temp_data + 0x30;
		}
		else 																					//变成ascii大写字母
		{	
			*(mubiao+j) = temp_data +	0x37;
		}
		j++;
		
	}
	return j;
}
u8 printbuf[800]={0};

static void gb17691_print_data(uint8_t* pdata, uint16_t data_len )
{
	u16 len_print=0;
	len_print=hex2ascii(printbuf,pdata,data_len);
	USARTSendOut(USART1, (const char *)printbuf,len_print);	 
}



static void GB17691_send_data(uint8_t* pdata, uint16_t data_len)
{
    uint8_t res;
    do
    {
        res = sim_uart_tx(pdata, data_len);  //需要判断串口BUF是否满？
    }
    while(!res);
	USARTSendOut(USART1, (const char *)pdata,data_len);	 //
#ifdef debug
    //USARTSendOut(USART1, "\r\n", 2);
    USARTSendOut(USART1, (const char *)pdata,data_len);	 //
    //USARTSendOut(USART1, "\r\n", 2);
#endif
}

CONTROL_ID CtlID[8] = 0;
u16 CtlIDNo[8] = 0  ;
extern u8 http_url[256] ;
extern uint8_t flag_http_update;
void GB17691_CMD_analysis(uint8_t *pdata, uint16_t len)
{
    switch(pdata[6])
    {
			
		 case 'Q':
    {
			if(pdata[7]=='N')	
			{
					send_heart_flag=0;    // 心跳计数清0  网络重启计数
					timing_state=1;
			}
        break;
    }

    default:
        break;
    }
		
		
	switch(pdata[2])	
	{
    case VEHICLE_CONTROL_CMD:
    {
//			if(pdata[30]==0x03)
//			{
//				flag_restart_net=1;		
//				memcpy((void *)flag_restart_net_time,pdata+24,6);
//			}
//      break;

			if(pdata[32]==SETUPDATA_FIRMWARE)
			{
				if(pdata[33]==1)	//Http升级
				{
					u16	url_len = 0 ;
					char url_head[] ="\r\nAT+HTTPPARA=\"URL\",\"";
					url_len = ((uint16_t)pdata[33] << 8) + pdata[34] ;		//这个地方需要跟及晓林说明
					if(url_len <= (256-4-strlen(url_head)))
					{
						memcpy(http_url, url_head, strlen(url_head));				
						memcpy(http_url+strlen(url_head), &pdata[35], url_len);
						http_url[strlen(url_head)+url_len]   ='\"';
						http_url[strlen(url_head)+url_len+1] ='\r';
						http_url[strlen(url_head)+url_len+2] ='\n';
						http_url[strlen(url_head)+url_len+3] ='\0';
						flag_http_update = 1;
						reset_sim_net();
					}
				}
				else if(pdata[33]==2)//FTP升级
				{
					flag_restart_net=1;
				}
			}
			else
			{
						uint8_t i;
						for( i=0;i<(sizeof(CtlIDNo)/sizeof(CtlIDNo[0]));i++)
						{
							if(CtlIDNo[i]==0)
							{
								CtlIDNo[i]=(u16)((pdata[30]<<8)|(pdata[31]));
								CtlID[i] = pdata[32];
								memcpy((void *)flag_answer_net_time,pdata+24,6);  //时间只能保存一个，服务器用CtlIDNo区分-210304-msc
								flag_answer_net ++;
								break;
							}
						}
						if(i >= (sizeof(CtlIDNo)/sizeof(CtlIDNo[0])))
						{
							break;
						}
						else
						{
								if(pdata[32]==SETDO)
								{
										if(pdata[33]==1) //SHUTDOWN
										{
											//需要写flash，待开发
										}
										else if(pdata[33]==2)//RESTART
										{
											flag_restart_net=1;
										}
										else if(pdata[33]==3)//RESET_DATA
										{
											flag_reset_net=1;
										}
										else if(pdata[33]==4)//OFFLINE
										{
											//需要写flash，待开发
										}
										else if(pdata[33]==5)//Relog
										{
											flag_relog_net=1;
										}
								}
								else if(pdata[32]==SETVIN) //设置VIN
								{
									memcpy(vin_info,&pdata[33],17);
									flag_set_vin=1;
						//			flag_setvin_net=1;
								}
								else if(pdata[32]==SETUDS)
								{
									flag_UDSSEND = pdata[33];
									if(pdata[33] ==1)
									{
										if((pdata[34]>=0)&&(pdata[34]<=20))
										{
											SetUdsProtocol(pdata[34]);	
											SetUdsPara(_DCUID,pdata[34]);
											Set_evads(8,1);
										}
									}
								}
								else if(pdata[32]==SETSEND)
								{
										//需要写flash，待开发
								}
								else if(pdata[32]==GETVER)
								{
										flag_ver_net=1;
								}
								else if(pdata[32]==GETVIN)
								{
										flag_vin_net=1;
								}
						}
			}
      break;
    }
	 case VEHICLE_TIMING_SUCCESS_CMD:
    {
        uint32_t new_time_stamp = get_time_stamp();
        if((new_time_stamp - old_time_stamp) <= 5)  //5->30 for网络助手调试
        {
            rtc_time_t server_time;
            rtc_time_t set_time;
            server_time.year = pdata[24];
            server_time.month = pdata[25];
            server_time.date = pdata[26];
            server_time.hour = pdata[27];
            server_time.minute = pdata[28];
            server_time.second= pdata[29];
            set_time_preset((new_time_stamp - old_time_stamp + TIME_PRESET_VALUE),server_time,&set_time);  //获得设置时间set_time
            if(set_current_time(set_time) == SUCCESS)
            {
                timing_state = 1;
				#ifdef debug						
										USARTSendOut(USART1, "\r\n-------》校时成功《-------\r\n", strlen("\r\n-------》校时成功《-------\r\n"));
				#endif	
            }
            send_data_cnt = 0;
            GB17691_send_enable = 1;

            if(lognum != get_lognum(server_time))
            {
                set_lognum(set_time,lognum);
            }
        }
        break;
    }
		case 0x82:	//VMS应答成功
		case 0x92:  //VMS应答失败
    {
        uint32_t new_time_stamp = get_time_stamp();
				send_heart_flag =0;
        break;
    }
		case 0x81: //vms 登入成功
		case 0x91: //vms 登入成功   
    {
				send_heart_flag = 0;
				flag_bool_login = 1;
        break;
    }
		    default:
        break;
	
	}
		
		
}
u8 simRxData[256]={0};

void GB17691CheckCmd(void)
{

 //  get_sim_data_multiple_bytes(GB17691_CMD_data, gb17691_rd_pos, len);
	 if(GB17691_CMD_data[0] == (uint8_t)(START_STRING >> 8))//判断起始符 和 VIN
                {
                    if(GB17691_CMD_data[1] == (uint8_t)(START_STRING))
                    {
												cmd_data_len = (uint16_t)(GB17691_CMD_data[22] << 8) + (uint16_t)GB17691_CMD_data[23];
                        cmd_data_len += 25;
                        //数据单元长度，加上24字节的信息，1个字节的校验，
                        if(cmd_data_len <= sizeof(GB17691_CMD_data))  //判断命令是否超出范围, gb17691_rd_pos++ ,再进入循环
                        {
													if(EvalBCC_FromBytes(&GB17691_CMD_data[2], cmd_data_len-3) == GB17691_CMD_data[cmd_data_len-1])
													{
															gb17691_rd_pos += cmd_data_len;
															if(gb17691_rd_pos >= SIM_UART_RX_BUF_SIZE)
															{
																	gb17691_rd_pos -= SIM_UART_RX_BUF_SIZE;
															}
															GB17691_CMD_analysis(GB17691_CMD_data, cmd_data_len);
													}
                        }
										}
								}    
}

/*
#define upload_successful 1//上传成功
#define unregistered      2//未注册
#define Check_failure     3//数据校验失败
#define unknown_failure   4//未知失败
*/
void GB17691_AIKELAN_CheckCmd(void)//安徽艾可蓝
{
 //  get_sim_data_multiple_bytes(GB17691_CMD_data, gb17691_rd_pos, len);
	   get_sim_data_multiple_bytes(GB17691_CMD_data, gb17691_rd_pos, 35);
	      if(GB17691_CMD_data[0] == (uint8_t)(ACTBLU_START_STRING >> 8))//判断起始符 和校验信息
                {
                    if(GB17691_CMD_data[1] == (uint8_t)(ACTBLU_START_STRING))
                    {
												if(GB17691_CMD_data[2] ==0)//注册帧
												{
																cmd_data_len=gb17691_rd_pos;	//数据单元长度															
															if(cmd_data_len <= sizeof(GB17691_CMD_data))  //判断命令是否超出范围, gb17691_rd_pos++ ,再进入循环
															{
																if(EvalBCC_FromBytes(&GB17691_CMD_data[2], cmd_data_len-4) == GB17691_CMD_data[cmd_data_len-2])
																{
																		gb17691_rd_pos += cmd_data_len;
																	  if (GB17691_CMD_data[3]==1)//上传成功
																		{
																		 Actblue_State=1;
																		 ACTBLU_date_state=	upload_successful;																			
																		}	
																		else if(GB17691_CMD_data[3]==3)//校验失败
																		{
																			Actblue_State=0;
																			ACTBLU_date_state=Check_failure;																																	
																		}
																		else //未知错误
																		{
																			Actblue_State=0;
																		 ACTBLU_date_state=unknown_failure;
																		}
																		if(gb17691_rd_pos >= SIM_UART_RX_BUF_SIZE)
																		{
																				gb17691_rd_pos -= SIM_UART_RX_BUF_SIZE;
																		}												
																		
																		GB17691_CMD_analysis(GB17691_CMD_data, cmd_data_len);
																}
															}													
												}
												else if(GB17691_CMD_data[2] ==1)//指令帧
												{
														  cmd_data_len=gb17691_rd_pos;	//数据单元长度
															if(cmd_data_len <= sizeof(GB17691_CMD_data))  //判断命令是否超出范围, gb17691_rd_pos++ ,再进入循环
															{
																if(EvalBCC_FromBytes(&GB17691_CMD_data[2], cmd_data_len-4) == GB17691_CMD_data[cmd_data_len-2])
																{
																		gb17691_rd_pos += cmd_data_len;																								
																		if (GB17691_CMD_data[3]==1)//上传成功
																		{
																			 Actblue_State=1;
																			 ACTBLU_date_state=	upload_successful;															
																		}	
																		else if(GB17691_CMD_data[3]==3)//校验失败
																		{
																			Actblue_State=0;
																			ACTBLU_date_state=Check_failure;																																	
																		}
																		else //未知错误
																		{
																			Actblue_State=0;
																		  ACTBLU_date_state=unknown_failure;
																		}	
																		if(gb17691_rd_pos >= SIM_UART_RX_BUF_SIZE)
																		{
																				gb17691_rd_pos -= SIM_UART_RX_BUF_SIZE;
																		}
																		GB17691_CMD_analysis(GB17691_CMD_data, cmd_data_len);
																}
															}																																			
												}											
										}
								}    
}


void GB17691_CMD_check(void)
{
//	uint8_t i;
    do
    {
        if(check_cmd_info_flag == 0)
        {
							//--------------------------------------郑州新加开始------------------
					  if(get_sim_rx_data_size(gb17691_rd_pos) > 90)  //至少接收到7个字节，才开始解析                郑州添加  实时数据包回复  共7个字节
            {
                get_sim_data_multiple_bytes(GB17691_CMD_data, gb17691_rd_pos, 8);
                if(GB17691_CMD_data[0] == (uint8_t)(START_STRING >> 8))//判断起始符 和 VIN
                {
                    if(GB17691_CMD_data[1] == (uint8_t)(START_STRING))
                    {																				
											if((GB17691_CMD_data[6]=='Q')&&(GB17691_CMD_data[7]=='N')	)					//新乡
											{
													cmd_data_len=8;
		
														gb17691_rd_pos += 8;
														if(gb17691_rd_pos >= SIM_UART_RX_BUF_SIZE)
														{
																gb17691_rd_pos -= SIM_UART_RX_BUF_SIZE;
														}
														GB17691_CMD_analysis(GB17691_CMD_data,cmd_data_len);																					
											}
										}
									}
						}
						//--------------------------------------郑州新加结束------------------
											
											
											
					
            if(get_sim_rx_data_size(gb17691_rd_pos) > 23)  //至少接收到24个字节，才开始解析
            {
                get_sim_data_multiple_bytes(GB17691_CMD_data, gb17691_rd_pos, 24);
                if(GB17691_CMD_data[0] == (uint8_t)(START_STRING >> 8))//判断起始符 和 VIN
                {
                    if(GB17691_CMD_data[1] == (uint8_t)(START_STRING))
                    {																						
                        cmd_data_len = (uint16_t)(GB17691_CMD_data[22] << 8) + (uint16_t)GB17691_CMD_data[23];
                        cmd_data_len += 25;
                        //数据单元长度，加上24字节的信息，1个字节的校验，
                        if(cmd_data_len <= sizeof(GB17691_CMD_data))  //判断命令是否超出范围, gb17691_rd_pos++ ,再进入循环
                        {
                            check_cmd_info_flag = 1;
                            CMD_check_dealy = 0;
                        }
											
                    }
                }

                if(check_cmd_info_flag == 0)
                {
                    gb17691_rd_pos++;
                    if(gb17691_rd_pos >= SIM_UART_RX_BUF_SIZE)
                    {
                        gb17691_rd_pos = 0;
                    }
                }			
            }
            else
            {
                break;
            }
        }
        else
        {
            if(get_sim_rx_data_size(gb17691_rd_pos) >= cmd_data_len)
            {
                check_cmd_info_flag = 0;

                get_sim_data_multiple_bytes(GB17691_CMD_data, gb17691_rd_pos, cmd_data_len);
                if(EvalBCC_FromBytes(&GB17691_CMD_data[2], cmd_data_len-3) == GB17691_CMD_data[cmd_data_len-1])
                {
                    gb17691_rd_pos += cmd_data_len;
                    if(gb17691_rd_pos >= SIM_UART_RX_BUF_SIZE)
                    {
                        gb17691_rd_pos -= SIM_UART_RX_BUF_SIZE;
                    }
                    GB17691_CMD_analysis(GB17691_CMD_data, cmd_data_len);
                }
                else
                {
                    gb17691_rd_pos++;
                    if(gb17691_rd_pos >= SIM_UART_RX_BUF_SIZE)
                    {
                        gb17691_rd_pos = 0;
                    }
                }
            }
            else
            {
                if(CMD_check_dealy < GB17691_CMD_CHECK_DEALY) //延时1s
                {
                    CMD_check_dealy++;
                    break;
                }
                else
                {
                    check_cmd_info_flag = 0;
                    gb17691_rd_pos++;
                    if(gb17691_rd_pos >= SIM_UART_RX_BUF_SIZE)
                    {
                        gb17691_rd_pos = 0;
                    }
                }
            }
        }
    }
    while(1);
}





uint8_t EvalBCC_FromBytes(uint8_t* Bytes, uint16_t len)
{
    u32 i;
    u8 bcc = 0;
    for (i = 0; i < len; i++)
    {
        bcc ^= Bytes[i];
    }
    return bcc;
}


void bool_login_send_new(void)   //登入
{
  
    rtc_time_t cur_time;
    uint8_t send_data[120];
    uint8_t send_data_size;

    send_data[0] = (uint8_t)(START_STRING >> 8);  //起始符
    send_data[1] = (uint8_t)START_STRING;
    send_data[2] = VEHICLE_LOGIN_CMD;             //登录命令

    GB17691_get_VIN_Info(&send_data[3]);
    //send_data[20]= GB17691_get_softver_Info();
    //memcpy(&send_data[3],vin_info,sizeof(vin_info));  //VIN

    send_data[20] = TERMINAL_SOFT_VER;        //软件版本号
    send_data[21] = NO_ENCRYPT;               //数据加密方式, 不加密

    get_current_time(&cur_time);              //数据采集时间
    send_data[24] = cur_time.year;
    send_data[25] = cur_time.month;
    send_data[26] = cur_time.date;
    send_data[27] = cur_time.hour;
    send_data[28] = cur_time.minute;
    send_data[29] = cur_time.second;


//    lognum = get_lognum(cur_time);           //登录流水号
//    lognum++;
    send_data[30] =0x81;
    send_data[31] = 0;		
		send_data [32]=0x0f;

get_imei(&send_data[33]);                //SIM模块IMEI
	
	lognum = get_lognum(cur_time);           //登录流水号
	lognum++;
	send_data[48] = (uint8_t)(lognum >> 8);
	send_data[49] = (uint8_t)lognum;
	
	get_iccid(&send_data[50]);              //SIM卡ICCID号 
		
	send_data[22] = 0;                      //时间6，登入流水号2，ICCID码20，IMEI信息体1  长度 2+ imei 15  共 46字节
	send_data[23] = 0x2e;
		
	send_data_size = 71;                   //发送数据总个数:22+2+46+1 ;
	send_data[send_data_size-1] = EvalBCC_FromBytes(&send_data[2], send_data_size-3);  //校验位

		int res=SocketWrite(1,send_data_size);
						if(res==0)//端口打开成功
						{
									GB17691_send_data(send_data,send_data_size);                         //实时上报									
#ifdef debug
	gb17691_print_data(send_data,send_data_size);		
#endif
		
					}
						else//端口打开失败。重新打开socket
						{
//							int re=OpenSocket(1);
						}
}


void timing_send(void)  //校时
{
	rtc_time_t cur_time;
	uint8_t send_data[50];
	uint8_t send_data_size;
	
	send_data[0] = (uint8_t)(START_STRING >> 8);         //起始符
	send_data[1] = (uint8_t)START_STRING;          
	send_data[2] = VEHICLE_TIMING_CMD;                   //校时命令
	
	GB17691_get_VIN_Info(&send_data[3]);
	
			if(getTermianlState()==1) 						//测试状态采用虚假VIN   //20200323 del
		{		
			for(u8 i=0;i<17;i++)
			{
				send_data[3+i]='L';
			}
		}
	
	//send_data[20]= GB17691_get_softver_Info();
//	memcpy(&send_data[3],vin_info,sizeof(vin_info));     //VIN
	send_data[20] = TERMINAL_SOFT_VER;                   //软件版本号
	send_data[21] = NO_ENCRYPT;                          //数据加密方式, 不加密
	
	get_current_time(&cur_time);                         //数据采集时间
	send_data[24] = cur_time.year;
	send_data[25] = cur_time.month;
	send_data[26] = cur_time.date;
	send_data[27] = cur_time.hour;
	send_data[28] = cur_time.minute;
	send_data[29] = cur_time.second;
	
	send_data [30]=0x81;                                 //IMEI信息体类型
	send_data [31]=0;
	send_data [32]=0x0f;
	get_imei(&send_data[33]);                            //SIM模块IMEI
	
		if(getTermianlState()==1) 						//测试状态采用虚假 IMEI
		{		
			for(u8 i=0;i<15;i++)
			{
				send_data[33+i]='8';
			}
		}		
		
	send_data[22] = 0;                                   // 6+1+2+15=24
	send_data[23] = 0x18;
	
	send_data_size = 49;                                 //发送数据总个数:22+2+16+1 ;
	send_data[48] = EvalBCC_FromBytes(&send_data[2], 46);


int res=SocketWrite(1,send_data_size);
						if(res==0)//端口打开成功
						{
									GB17691_send_data(send_data,send_data_size);                         //实时上报									
						}
						else//端口打开失败。重新打开socket
						{
//							int re=OpenSocket(1);
						}

    old_time_stamp = get_time_stamp();
}

u16 alarm_num=0;
void disman_alarm_send(void)  //拆除报警
{
    rtc_time_t cur_time;
    uint8_t send_data[50];
    uint8_t send_data_size;

    send_data[0] = (uint8_t)(START_STRING >> 8);         //起始符
    send_data[1] = (uint8_t)START_STRING;
    send_data[2] = 06;                   //拆除报警

    GB17691_get_VIN_Info(&send_data[3]);
	
    send_data[20] = TERMINAL_SOFT_VER;                   //软件版本号
    send_data[21] = NO_ENCRYPT;                          //数据加密方式, 不加密

    send_data[22] = 0;                                   // 17
    send_data[23] = 17;
	
    get_current_time(&cur_time);              //数据采集时间
    send_data[24] = cur_time.year;
    send_data[25] = cur_time.month;
    send_data[26] = cur_time.date;
    send_data[27] = cur_time.hour;
    send_data[28] = cur_time.minute;
    send_data[29] = cur_time.second;
	
    send_data[30] = (uint8_t)(alarm_num >> 8);
    send_data[31] = (uint8_t)alarm_num;			
		alarm_num++;
		
		 GB17691_location_t location;
	  get_GB17691_gps_info(&location);

	  send_data[32] = (uint8_t)(location.status.is_valid) | ((uint8_t)location.status.NorS<<2) | ((uint8_t)location.status.EorW<<3)  ;	 
    memcpy(&send_data[33] ,&location.longitude,4);                                            //经度
		memcpy(&send_data[37],&location.latitude,4);                                             //纬度
		
    send_data_size = 42;                                 //发送数据总个数:22+2+16+1 ;
    send_data[41] = EvalBCC_FromBytes(&send_data[2], 39);


int res=SocketWrite(1,send_data_size);
						if(res==0)//端口打开成功
						{
									GB17691_send_data(send_data,send_data_size);                         //实时上报									
						}
						else//端口打开失败。重新打开socket
						{
//							int re=OpenSocket(1);
						}

    old_time_stamp = get_time_stamp();
}


void login_send_new(void)   //登入
{
    rtc_time_t cur_time;
    uint8_t send_data[120];
    uint8_t send_data_size;

    send_data[0] = (uint8_t)(START_STRING >> 8);  //起始符
    send_data[1] = (uint8_t)START_STRING;
    send_data[2] = ZHENGZHOU_LOGIN_CMD;             //登录命令

    GB17691_get_VIN_Info(&send_data[3]);
    //send_data[20]= GB17691_get_softver_Info();
    //memcpy(&send_data[3],vin_info,sizeof(vin_info));  //VIN

    send_data[20] = TERMINAL_SOFT_VER;        //软件版本号
    send_data[21] = NO_ENCRYPT;               //数据加密方式, 不加密

    get_current_time(&cur_time);              //数据采集时间
    send_data[24] = cur_time.year;
    send_data[25] = cur_time.month;
    send_data[26] = cur_time.date;
    send_data[27] = cur_time.hour;
    send_data[28] = cur_time.minute;
    send_data[29] = cur_time.second;


    lognum = get_lognum(cur_time);           //登录流水号
    lognum++;
    send_data[30] = (uint8_t)(lognum >> 8);
    send_data[31] = (uint8_t)lognum;		
		set_lognum(cur_time, lognum);
		
    get_iccid(&send_data[32]);              //SIM卡ICCID号	
    GB17691_get_VIN_Info(&send_data[52]);		
  	get_imei(&send_data[69]);                //SIM模块IMEI
		
		send_data[84]=0;
		send_data[85]=0;
		
		send_data[86]=500>>8;
		send_data[87]=500;

		send_data[88]=0x01;
		send_data[89]=0x00;
		send_data[90]=0x00;

    send_data[22] = 0;                      //时间6，登入流水号2，ICCID码20，IMEI信息体1  长度 2+ imei 15  共 46字节
    send_data[23] = 67;

    send_data_size = 92;                   //发送数据总个数:22+2+46+1 ;
    send_data[send_data_size-1] = EvalBCC_FromBytes(&send_data[2], send_data_size-3);  //校验位

		int res=SocketWrite(1,send_data_size);
						if(res==0)//端口打开成功
						{
								GB17691_send_data(send_data,send_data_size);                         //实时上报		
#ifdef debug
	gb17691_print_data(send_data,send_data_size);			
#endif	
							
						}
						else//端口打开失败。重新打开socket
						{
//							int re=OpenSocket(1);
						}

}

void logout_send(void)  //登出
{
    rtc_time_t cur_time;
    uint8_t send_data[60];

    send_data[0] = (uint8_t)(START_STRING >> 8);
    send_data[1] = (uint8_t)START_STRING;
    send_data[2] = VEHICLE_LOGOUT_CMD;

    GB17691_get_VIN_Info(&send_data[3]);
    //send_data[20]= GB17691_get_softver_Info();

    //memcpy(&send_data[3],vin_info,sizeof(vin_info));

    send_data[20] = TERMINAL_SOFT_VER;        //软件版本号
    send_data[21] = NO_ENCRYPT;               //数据加密方式, 不加密

    get_current_time(&cur_time);
    send_data[24] = cur_time.year;
    send_data[25] = cur_time.month;
    send_data[26] = cur_time.date;
    send_data[27] = cur_time.hour;
    send_data[28] = cur_time.minute;
    send_data[29] = cur_time.second;

    send_data [30]=0x81;                     //IMEI信息体类型
    send_data [31]=0;
    send_data [32]=0x0f;
    get_imei(&send_data[33]);                            //SIM模块IMEI

    send_data[48] = (uint8_t)(lognum >> 8);  //登出流水号
    send_data[49] = (uint8_t)lognum;

    send_data[22] = 0x00;                    //登出报文数据单元，时间，登出流水号 IMEI     6+2+1+2+15 =26字节
    send_data[23] = 0x1a;

    send_data[50] = EvalBCC_FromBytes(&send_data[2], 48);

    //sim_uart_tx(send_data,33);
//    GB17691_send_data(send_data,51);
		int res=SocketWrite(1,51);
						if(res==0)//端口打开成功
						{
									GB17691_send_data(send_data,51);                         //实时上报									
						}
						else//端口打开失败。重新打开socket
						{
//							int re=OpenSocket(1);
						}

}

void BOOL_data_put_buf(uint8_t handle)   //打包 存储  上报
{
	uint16_t data_len = 0;
//	uint16_t temp_len=0;
//	uint8_t temp_buf[36]={0};
	save_resend_data_t save_data_status;
	
//	if(is_set_time())
	{
		GB17691_data_package[0] = (uint8_t)(START_STRING >> 8);                             // 起始符 0x2323
		GB17691_data_package[1] = (uint8_t)(START_STRING);
	
		//memcpy(&GB17691_data_package[3],vin_info,sizeof(vin_info));                         //VIN
		GB17691_get_VIN_Info(&GB17691_data_package[3]);
		
		//GB17691_data_package[20]= GB17691_get_softver_Info();
		

	  GB17691_data_package[20] = TERMINAL_SOFT_VER;                                    //软件版本号
	//	GB17691_data_package[20] =temp_soft_ver;                                           //软件版本号
		GB17691_data_package[21] = NO_ENCRYPT;                                             //数据加密方式 0x01
	
		rtc_time_t cur_time;
		get_current_time(&cur_time);                                                        // 获取RTC 当前时间
		save_data_status.timestamp = get_time_stamp();                                      // 时间计数
		
		memcpy(&GB17691_data_package[24+data_len],&cur_time,sizeof(rtc_time_t));            //加上当前时间
		data_len += sizeof(rtc_time_t);
		
		GB17691_data_package[24+data_len]= (uint8_t)(INFO_STREAM_NO>>8);                    //信息流水号
		GB17691_data_package[25+data_len]= (uint8_t)(INFO_STREAM_NO);
		data_len +=2;
		
		GB17691_data_package[24+data_len] = GB17691_INFO_IMEI_DATA;                      // 信息类型标志:IMEI信息体  ,0X81
		data_len += 1;
		GB17691_data_package[24+data_len]=0;                                            //IMEI长度 2字节表示 
		GB17691_data_package[25+data_len]=0x0f;
		data_len +=2;
		get_imei(&GB17691_data_package[24+data_len]);                          
		data_len +=15;
		
		GB17691_data_package[24+data_len] = GB17691_INFO_OBD_DATA;                         // 信息类型标志:OBD信息 ,0X01
		data_len += 1;

    data_len += GB17691_get_OBD_Info(&GB17691_data_package[24+data_len]);           //OBD信息
		
		GB17691_data_package[24+data_len] = GB17691_INFO_DATASTREAM_DATA;                               //信息类型标志:数据流信息 ,0X02
		data_len += 1;
		
		data_len += GB17691_get_BOOLdatastream_Info(&GB17691_data_package[24+data_len]);                                                  //获取数据流信息

		GB17691_data_package[24+data_len] = GB17691_INFO_CUSTOM_DATA;                         // 信息类型标志:自定义信息  ,0X80
		data_len += 1;
		data_len += GB17691_get_BOOLcustom_Info(&GB17691_data_package[24+data_len]);                                                  //获取自定义信息体
			
		GB17691_data_package[22] = (uint8_t)(data_len >> 8);                   // 6+ 2 + 1+2+15 + 1+96 +1+37 +1+2+8
		GB17691_data_package[23] = (uint8_t)data_len;
			
			if(handle & REAL_TIME_UPLOAD)                                                         //实时上报
			{
				GB17691_data_package[2] = VEHICLE_UPLOAD_CMD;
				GB17691_data_package[24+data_len] = EvalBCC_FromBytes(&GB17691_data_package[2], data_len+22);//异或校验字
				
				int res=SocketWrite(1,24+data_len+1);
						if(res==0)
						{
							GB17691_send_data(GB17691_data_package, 24+data_len+1);   
	
						}
						else
						{
//							int re=OpenSocket(1);
						}
						
//				GB17691_send_data(GB17691_data_package, 24+data_len+1);                           //实时上报
				INFO_STREAM_NO++;                                                                 //需优化:中间设备断电,则需做存储处理, 以天为单位,从1开始累加  
				if(INFO_STREAM_NO>=65531){
					INFO_STREAM_NO=0;
				}
			}

			GB17691_data_package[2] = VEHICLE_RELOAD_CMD;
			GB17691_data_package[24+data_len] = EvalBCC_FromBytes(&GB17691_data_package[2], data_len+22);  //异或校验字
			data_len += 1;
			
			if(handle & SAVE_RESEND_DATA)    //存储数据
			{
				if((handle & REAL_TIME_UPLOAD) && (terminal_mode == TERMINAL_NET))
				{
					save_data_status.resend_flag = SEND_DATA_FLAG;
					realtime_upload_flag_addr[realtime_load_cnt] = realtime_data_wr_flash_addr + 24 + data_len + (sizeof(save_resend_data_t) - 2); //最后两个字节是标志位
					if(realtime_upload_flag_addr[realtime_load_cnt] >= RESEND_DATA_END_ADDR)
					{
						realtime_upload_flag_addr[realtime_load_cnt] -= RESEND_DATA_END_ADDR;
						realtime_upload_flag_addr[realtime_load_cnt] += RESEND_DATA_START_ADDR;
					}
					realtime_load_cnt++;
					if(realtime_load_cnt >= (sizeof(realtime_upload_flag_addr) / sizeof(uint32_t))) //防止溢出
					{
						realtime_load_cnt = 0;
					}
				}
				else
				{
					save_data_status.resend_flag = RESEND_DATA_FLAG;
				}
							
				memcpy(&GB17691_data_package[24+data_len],&save_data_status,sizeof(save_resend_data_t));   // 实时上报数据 + 时间戳+ 包计数+ 包数量+ 包状态标记 
			  write_data_to_flash(GB17691_data_package,realtime_data_wr_flash_addr,24+data_len+sizeof(save_resend_data_t),RESEND_DATA_FLASH_AREA);    // 数据写入FLASH
				
				realtime_data_wr_flash_addr += (24+data_len+sizeof(save_resend_data_t));
				if(realtime_data_wr_flash_addr >= RESEND_DATA_END_ADDR)
				{
					realtime_data_wr_flash_addr -= RESEND_DATA_END_ADDR;
					realtime_data_wr_flash_addr += RESEND_DATA_START_ADDR;
				}
//				
//				/***存储每超过2M记录一下实时数据读写FLASH地址 ****************************/
////				resend_wrtie_size += (24+data_len+sizeof(save_resend_data_t));
////				if(resend_wrtie_size >= RESEND_WRITE_MAX_SIZE) //2M
////				{
////					//save_resend_record_addr(&resend_record_addr, realtime_data_wr_flash_addr);  //在用车OBD项目不存储补发数据地址
////					resend_wrtie_size = 0;
////				}		
//			}
			}
	}
}


// CRC16_Checkout
unsigned int CRC16_Checkout ( unsigned char *puchMsg, unsigned int usDataLen )
{
unsigned int i,j,crc_reg,check;
crc_reg = 0xFFFF;
for(i=0;i<usDataLen;i++)
{
	crc_reg = (crc_reg>>8) ^ puchMsg[i];
	for(j=0;j<8;j++)
	{
		check = crc_reg & 0x0001;
		crc_reg >>= 1;
		if(check==0x0001)
		{
			crc_reg ^= 0xA001;
		}
	}
}
	return crc_reg;
}



// 终端应答 控制命令回复
void GB17691_ack_send( u8 con_ID ,u16 ctlidNo , u8 con_state , u8 ack_info)  //登出
{
 //   rtc_time_t cur_time;
		uint16_t data_len = 0;
    uint8_t send_data[1024];

    send_data[0] = (uint8_t)(START_STRING >> 8);
    send_data[1] = (uint8_t)START_STRING;
    send_data[2] = VEHICLE_ACKCON_CMD;

    GB17691_get_VIN_Info(&send_data[3]);
    //send_data[20]= GB17691_get_softver_Info();

    //memcpy(&send_data[3],vin_info,sizeof(vin_info));

    send_data[20] = TERMINAL_SOFT_VER;        //软件版本号
    send_data[21] = NO_ENCRYPT;               //数据加密方式, 不加密

//    get_current_time(&cur_time);
//    send_data[24] = cur_time.year;
//    send_data[25] = cur_time.month;
//    send_data[26] = cur_time.date;
//    send_data[27] = cur_time.hour;
//    send_data[28] = cur_time.minute;
//    send_data[29] = cur_time.second;

//				memcpy(send_data+24,(void *)flag_restart_net_time,6);
			memcpy(send_data+24,(void *)flag_answer_net_time,6);
//    send_data [30]=0x81;                     //IMEI信息体类型
//    send_data [31]=0;
//    send_data [32]=0x0f;
//    get_imei(&send_data[33]);                            //SIM模块IMEI

		send_data[30]=(ctlidNo>>8)&0xFF;
		send_data[31]=ctlidNo&0xFF;


		send_data[32]=con_ID;
		send_data[33]=con_state;
		send_data[34]=ack_info;
		data_len = 35-24;

if(con_ID == GETVER)
{
	data_len += GB17691_get_ver_Info(&send_data[24+data_len]);
  send_data[22] = (data_len>>8)&0xFF;
  send_data[23] = data_len&0xFF;
}
else if(con_ID == GETVIN)
{
	data_len += GB17691_get_VIN_Info(&send_data[24+data_len]);
  send_data[22] = (data_len>>8)&0xFF;
  send_data[23] = data_len&0xFF;
}
else if(con_ID == GETUDS)
{

}
else if(con_ID == GETSENDSTATE)
{

}
else if(con_ID == GETTEMSTATE)
{

}
else if(con_ID == GETOBDDATA)
{
		send_data[24+data_len] = GB17691_INFO_OBD_DATA;                         // 信息类型标志:OBD信息 ,0X01
		data_len += 1;

    data_len += GB17691_get_OBD_Info(&send_data[24+data_len]);           //OBD信息
		
		send_data[24+data_len] = GB17691_INFO_DATASTREAM_DATA;                               //信息类型标志:数据流信息 ,0X02
		data_len += 1;
		
		data_len += GB17691_get_BOOLdatastream_Info(&send_data[24+data_len]);                                                  //获取数据流信息

		send_data[24+data_len] = GB17691_INFO_CUSTOM_DATA;                         // 信息类型标志:自定义信息  ,0X80
		data_len += 1;
		data_len += GB17691_get_BOOLcustom_Info(&send_data[24+data_len]);

  send_data[22] = (data_len>>8)&0xFF;
  send_data[23] = data_len&0xFF;
}
else if(con_ID == GETCANDATA)
{

}
else if(con_ID == GETCANSTATE)
{

}
else
{
    send_data[22] = 0x00;                    //登出报文数据单元，时间，登出流水号 IMEI     6+2+1+2+15 =26字节
    send_data[23] = 11;
}
	
    send_data[24+data_len] = EvalBCC_FromBytes(&send_data[2], 24+data_len-2);

		int res=SocketWrite(1,24+data_len+1);
						if(res==0)//端口打开成功
						{
									GB17691_send_data(send_data,24+data_len+1);                         //实时上报	
#ifdef debug
							     gb17691_print_data(send_data,24+data_len+1);		
#endif							
						}
						else//端口打开失败。重新打开socket
						{
//							int re=OpenSocket(1);
						}

}
/************************************************************************
  * @描述:   安徽艾可蓝G4OBD数据打包 存储  上报
  * @参数:   None
  * @返回值: None
  **********************************************************************/
void GB17691_data_put_buf(uint8_t handle)   //打包 存储  上报
{
	      int cont=0;
        uint16_t data_len = 0;
		    unsigned int crcre=0xffff;

        save_resend_data_t save_data_status;
				GB17691_data_package[0] = '$';
				GB17691_data_package[1] = '$';
	    if (Actblue_State!=1)//注册帧
			 {
					GB17691_data_package[2] = '0';			 
					data_len=get_AIKELAN_register(&GB17691_data_package[3]);//数据长度	
				 
					crcre=CRC16_Checkout(&GB17691_data_package[3],data_len);			 
					get_AIKELAN_crcre(crcre,&GB17691_data_package[3]+data_len);//保存校验4字节
				 
					GB17691_data_package[5+data_len]='#';//2+CRC2-1+尾2
					GB17691_data_package[6+data_len]='#';
				 	GB17691_data_package[7+data_len]='\r';//2+CRC2-1+尾2
					GB17691_data_package[8+data_len]='\n';
			 }
			 else  //数据帧
			{
					GB17691_data_package[2] = '1';
					data_len=get_AIKELAN_datapack(&GB17691_data_package[3]);
				
					crcre=CRC16_Checkout(&GB17691_data_package[3],data_len);
					get_AIKELAN_crcre(crcre,&GB17691_data_package[3+data_len]);	//校验按照4字节计算	
					
					GB17691_data_package[5+data_len]='#';//
					GB17691_data_package[6+data_len]='#';//2+2+2
					GB17691_data_package[7+data_len]='\r';//2+CRC2-1+尾2
					GB17691_data_package[8+data_len]='\n';
			}

			GB17691_send_data(GB17691_data_package, 9+data_len); 
			
				//----------------------------------------------------------------------
				
				
        if(handle & REAL_TIME_UPLOAD)                                                         //实时上报
        {
						int res=SocketWrite(0,7+data_len);//区分注册和数据帧
						if(res==0)//端口打开成功
						{
									if(Actblue_State!=1)//注册帧
									{
												do
												{
													GB17691_send_data(GB17691_data_package, 6+data_len);
													cont ++;
												}
												 while(cont<4);
													
									}
									else//数据帧
									{
										GB17691_send_data(GB17691_data_package, 6+data_len);                           //实时上报
										
									}								
						}
						else//端口打开失败。重新打开socket
						{
//							int re=OpenSocket(0);
						}
						
            INFO_STREAM_NO++;                                                                 //需优化:中间设备断电,则需做存储处理, 以天为单位,从1开始累加
            if(INFO_STREAM_NO>=65531)
            {
                INFO_STREAM_NO=0;
            }

        }

//        GB17691_data_package[2] = VEHICLE_RELOAD_CMD;
//        GB17691_data_package[24+data_len] = EvalBCC_FromBytes(&GB17691_data_package[2], data_len+22);  //异或校验字
//        data_len += 1;

        if(handle & SAVE_RESEND_DATA)    //存储数据
        {
            if((handle & REAL_TIME_UPLOAD) && (terminal_mode == TERMINAL_NET))
            {
                save_data_status.resend_flag = REALTIME_UPLOAD_FLAG;
                realtime_upload_flag_addr[realtime_load_cnt] = realtime_data_wr_flash_addr + 24 + data_len + (sizeof(save_resend_data_t) - 2); //最后两个字节是标志位
                if(realtime_upload_flag_addr[realtime_load_cnt] >= RESEND_DATA_END_ADDR)
                {
                    realtime_upload_flag_addr[realtime_load_cnt] -= RESEND_DATA_END_ADDR;
                    realtime_upload_flag_addr[realtime_load_cnt] += RESEND_DATA_START_ADDR;
                }
                realtime_load_cnt++;							
                if(realtime_load_cnt >= (sizeof(realtime_upload_flag_addr) / sizeof(uint32_t))) //防止溢出
                {
                    realtime_load_cnt = 0;
                }
            }
            else
            {
                save_data_status.resend_flag = RESEND_DATA_FLAG;
            }

            memcpy(&GB17691_data_package[24+data_len],&save_data_status,sizeof(save_resend_data_t));   // 实时上报数据 + 时间戳+ 包计数+ 包数量+ 包状态标记
            write_data_to_flash(GB17691_data_package,realtime_data_wr_flash_addr,24+data_len+sizeof(save_resend_data_t),RESEND_DATA_FLASH_AREA);    // 数据写入FLASH

            realtime_data_wr_flash_addr += (24+data_len+sizeof(save_resend_data_t));
            if(realtime_data_wr_flash_addr >= RESEND_DATA_END_ADDR)
            {
                realtime_data_wr_flash_addr -= RESEND_DATA_END_ADDR;
                realtime_data_wr_flash_addr += RESEND_DATA_START_ADDR;
            }

            /***存储每超过2M记录一下实时数据读写FLASH地址 ****************************/
//				resend_wrtie_size += (24+data_len+sizeof(save_resend_data_t));
//				if(resend_wrtie_size >= RESEND_WRITE_MAX_SIZE) //2M
//				{
//					//save_resend_record_addr(&resend_record_addr, realtime_data_wr_flash_addr);  //在用车OBD项目不存储补发数据地址
//					resend_wrtie_size = 0;
//				}
				
					} //**  

//    }
}//**

/************************************************************************
  * @描述:   新乡G4OBD数据打包 存储  上报
  * @参数:   None
  * @返回值: None
  **********************************************************************/
/*
void GB17691_data_put_buf(uint8_t handle)   //打包 存储  上报
{
    uint16_t data_len = 0;
//		u8 *ps=NULL;
		unsigned int crcre=0xffff;
//	uint16_t temp_len=0;
//	uint8_t temp_buf[36]={0};
    save_resend_data_t save_data_status;

    // if(is_set_time())
    {
        GB17691_data_package[0] = '#';                             // 起始符 0x2323
        GB17691_data_package[1] = '#';

			data_len=get_xinxiang_datapack(&GB17691_data_package[6]);
		  get_xinxiang_datalen(data_len,&GB17691_data_package[2]);	
			
		  crcre=CRC16_Checkout(&GB17691_data_package[6],data_len);
			get_xinxiang_crcre(crcre,&GB17691_data_package[6+data_len]);
			
			GB17691_data_package[10+data_len]=0x0D;
			GB17691_data_package[11+data_len]=0x0A;			
			GB17691_send_data(GB17691_data_package, 12+data_len); 
			
			
			
			
				//----------------------------------------------------------------------
				
				
        if(handle & REAL_TIME_UPLOAD)                                                         //实时上报
        {
						int res=SocketWrite(0,12+data_len);
						if(res==0)//端口打开成功
						{
									GB17691_send_data(GB17691_data_package, 12+data_len);                           //实时上报									
						}
						else//端口打开失败。重新打开socket
						{
//							int re=OpenSocket(0);
						}
						
            INFO_STREAM_NO++;                                                                 //需优化:中间设备断电,则需做存储处理, 以天为单位,从1开始累加
            if(INFO_STREAM_NO>=65531)
            {
                INFO_STREAM_NO=0;
            }

        }



        GB17691_data_package[2] = VEHICLE_RELOAD_CMD;
        GB17691_data_package[24+data_len] = EvalBCC_FromBytes(&GB17691_data_package[2], data_len+22);  //异或校验字
        data_len += 1;

        if(handle & SAVE_RESEND_DATA)    //存储数据
        {
            if((handle & REAL_TIME_UPLOAD) && (terminal_mode == TERMINAL_NET))
            {
                save_data_status.resend_flag = REALTIME_UPLOAD_FLAG;
                realtime_upload_flag_addr[realtime_load_cnt] = realtime_data_wr_flash_addr + 24 + data_len + (sizeof(save_resend_data_t) - 2); //最后两个字节是标志位
                if(realtime_upload_flag_addr[realtime_load_cnt] >= RESEND_DATA_END_ADDR)
                {
                    realtime_upload_flag_addr[realtime_load_cnt] -= RESEND_DATA_END_ADDR;
                    realtime_upload_flag_addr[realtime_load_cnt] += RESEND_DATA_START_ADDR;
                }
                realtime_load_cnt++;							
                if(realtime_load_cnt >= (sizeof(realtime_upload_flag_addr) / sizeof(uint32_t))) //防止溢出
                {
                    realtime_load_cnt = 0;
                }
            }
            else
            {
                save_data_status.resend_flag = RESEND_DATA_FLAG;
            }

            memcpy(&GB17691_data_package[24+data_len],&save_data_status,sizeof(save_resend_data_t));   // 实时上报数据 + 时间戳+ 包计数+ 包数量+ 包状态标记
            write_data_to_flash(GB17691_data_package,realtime_data_wr_flash_addr,24+data_len+sizeof(save_resend_data_t),RESEND_DATA_FLASH_AREA);    // 数据写入FLASH

            realtime_data_wr_flash_addr += (24+data_len+sizeof(save_resend_data_t));
            if(realtime_data_wr_flash_addr >= RESEND_DATA_END_ADDR)
            {
                realtime_data_wr_flash_addr -= RESEND_DATA_END_ADDR;
                realtime_data_wr_flash_addr += RESEND_DATA_START_ADDR;
            }

            /***存储每超过2M记录一下实时数据读写FLASH地址 ****************************/
//				resend_wrtie_size += (24+data_len+sizeof(save_resend_data_t));
//				if(resend_wrtie_size >= RESEND_WRITE_MAX_SIZE) //2M
//				{
//					//save_resend_record_addr(&resend_record_addr, realtime_data_wr_flash_addr);  //在用车OBD项目不存储补发数据地址
//					resend_wrtie_size = 0;
//				}
				
//					}   

//    }
//}

/******************************************************************************************
功能：打包安全芯片ID
作者：杜汉宇 19.09.26
参数：void
返回值：void  
*******************************************************************************************/



/******************************************************************************************
功能：向郑州平台发送备案信息
作者：杜汉宇 19.09.26
参数：void
返回值：void  
*******************************************************************************************/
//static void record_send(void)
//{
//		u8 *ps=NULL;
//	
//    rtc_time_t cur_time;
//    uint8_t send_data[280];
//    uint16_t send_data_size;

//    send_data[0] = (uint8_t)(START_STRING >> 8);  //起始符
//    send_data[1] = (uint8_t)START_STRING;
//    send_data[2] = ZHENGZHOU_RECORD_CMD;             //备案命令

//    GB17691_get_VIN_Info(&send_data[3]);

//    send_data[20] = TERMINAL_SOFT_VER;        //软件版本号
//    send_data[21] = NO_ENCRYPT;               //数据加密方式, 不加密

////	  send_data[22] = 0;                      //时间6  + 芯片ID16 + 公钥64 + VIN17 + 签名130=233
////    send_data[23] = 0xE9;

//		send_data_size=24;
//	
//    get_current_time(&cur_time);              //数据采集时间
//    send_data[send_data_size++] = cur_time.year;
//    send_data[send_data_size++] = cur_time.month;
//    send_data[send_data_size++] = cur_time.date;
//    send_data[send_data_size++] = cur_time.hour;
//    send_data[send_data_size++] = cur_time.minute;
//    send_data[send_data_size++] = cur_time.second;


//		memcpy(&send_data[30],SM2ID,16);
//		send_data_size+=16;
//		
//		
//	
//			
//		ps=ReadPublickey();														//公钥
//		if(ps==NULL)
//			ps=ReadPublickey();														//公钥	
//		
//		if(ps!=NULL)
//			lkt4305_state=1;
//		
//		memcpy(&send_data[46],ps,64);
//		send_data_size=send_data_size+64;

//		GB17691_get_VIN_Info(&send_data[110]);	
//		send_data_size=send_data_size+17;		// VIN
//		
//		ps=Signature(&send_data[30],97);					//签名
//		memcpy(&data_sign,ps,64);

//		send_data[send_data_size++] =32;   // 签名R值
////		for(uint8_t i=0;i<32;i++)
////		{
////			u8 temp;
////			temp=(data_sign[i]>>4)&0x0F;
////			if(temp<=9)
////				send_data[send_data_size+i*2]=temp+0x30;
////			else
////				send_data[send_data_size+i*2]=temp+0x37;
////			
////			temp=data_sign[i]&0x0F;
////			if(temp<=9)
////				send_data[send_data_size+i*2+1]=temp+0x30;
////			else
////				send_data[send_data_size+i*2+1]=temp+0x37;				
////		}
////		send_data_size=send_data_size+64;

//		for(uint8_t i=0;i<32;i++)
//		{
//			send_data[send_data_size+i]=data_sign[i];
//		}
//		send_data_size=send_data_size+32;
//		


//		send_data[send_data_size++] =32;   // 签名S值
////		for(uint8_t i=32;i<64;i++)
////		{
////			u8 temp;
////			temp=(data_sign[i]>>4)&0x0F;
////			if(temp<=9)
////				send_data[send_data_size+i*2-64]=temp+0x30;
////			else
////				send_data[send_data_size+i*2-64]=temp+0x37;
////			
////			temp=data_sign[i]&0x0F;
////			if(temp<=9)
////				send_data[send_data_size+i*2+1-64]=temp+0x30;
////			else
////				send_data[send_data_size+i*2+1-64]=temp+0x37;				
////		}
////		send_data_size=send_data_size+64;

//		for(uint8_t i=0;i<32;i++)
//		{
//			send_data[send_data_size+i]=data_sign[i+32];
//		}				
//		send_data_size=send_data_size+32;
//		
//	  send_data[22] = (send_data_size-24)>>8;                      //时间6  + 芯片ID16 + 公钥64 + VIN17 + 签名66=169
//    send_data[23] = (send_data_size-24);
//		
//		send_data[send_data_size] = EvalBCC_FromBytes(&send_data[2], send_data_size-2);  //校验位
//		send_data_size++;


//		int res=SocketWrite(0,send_data_size);
//						if(res==0)//端口打开成功
//						{
//									GB17691_send_data(send_data,send_data_size);                         //实时上报									
//						}
//						else//端口打开失败。重新打开socket
//						{
//							int re=OpenSocket(0);
//						}
//}

static uint8_t get_terminal_connect_status(void)
{
    uint8_t mode;
//	if(is_usb_connected())
//	{
//		mode = TERMINAL_USB;
//	}
//	else
    {
        //	if(get_net_status() == 0x01)
        if(get_net_status() == ON_LINE)
        {
            mode = TERMINAL_NET;//  网络连接模式
        }
        else
        {
            mode = TERMINAL_FREE;//自由模式,
        }
    }
    return mode;
}


static u16 realup_cnt=0;
void GB17691_real_time_upload(void)
{
    uint32_t new_time_stamp;
    uint8_t GB17691_data_handle = 0;
    uint8_t current_mode;
		u8 cnt;
    new_time_stamp = get_time_stamp();

    current_mode = get_terminal_connect_status();
    if(terminal_mode != current_mode)     // 模式有改变, 如果是网络连接状态变为其他状态,则 将实时上报数据改为需要补发上报的数据
    {
        if(terminal_mode==TERMINAL_NET)
        {
            for(uint8_t i=0; i<realtime_load_cnt; i++)
            {
                write_resend_data_flag(realtime_upload_flag_addr[i], RESEND_DATA_FLAG);
            }
        }
        terminal_mode = current_mode;
        login_state = 0;
        timing_state = 0;
        GB17691_send_enable = 0;
    		send_data_cnt = 0;
        send_heart_flag = 0;
        realtime_load_cnt = 0;
        resend_msg_cnt = 0;
    }

    if((big_heart_flag==1) || (can_state==1))
    {
			
			
			
						GB17691_data_put_buf(GB17691_data_handle);//上报郑州平台=---测试数据
						
			
			if(cnt>101){cnt=0;}
      if(terminal_mode == TERMINAL_NET)
      {
				GB17691_send_enable=1;   //郑州发送使能  小时
				if(record_state==1)		// 已备案 或在测试状态
				{
						if(timing_state == 0)			//未校时
						{
							if(((new_time_stamp - old_time_stamp) >= TIME_SEND_INTERVAL) || (send_data_cnt == 0))
							{
								if((send_data_cnt >= TIME_SEND_CNT) && (terminal_mode == TERMINAL_NET))
								{
									if(big_heart_flag==1)            
									{
											big_heart_flag=0;
											big_heart_time_stamp=new_time_stamp;
											update_big_heart_time(big_heart_time_stamp);
									}
									else
									{
										if(is_set_time())
											timing_state=1;
										else
										{
											reconnect_sim_net();
											send_data_cnt=0;
										}
									}
								}
								else
								{
									timing_send();
									send_data_cnt++;
									old_time_stamp = new_time_stamp;
						#ifdef debug						
												USARTSendOut(USART1, "\r\n发送校时\r\n", strlen("\r\n发送校时\r\n"));
						#endif	
								}
							}
						}
					else   // 已校时
					{
						if(login_state == 0)  //未登录
						{
//							if(((new_time_stamp - old_time_stamp) >= LOGIN_INTERVAL) || (send_data_cnt == 0))
//							{
//								if((send_data_cnt >= LOGIN_MAX_CNT) && (terminal_mode == TERMINAL_NET))
//								{
//									if(big_heart_flag==1)            
//									{
//											big_heart_flag=0;
//											big_heart_time_stamp=new_time_stamp;
//											update_big_heart_time(big_heart_time_stamp);
//									}else
//									{
//											reconnect_sim_net();
//									}
//								}
//								else
//								{
//									login_send_new();
//									send_data_cnt++;
//									old_time_stamp = new_time_stamp;
//			#ifdef debug 					
//							USARTSendOut(USART1, "\r\n发送登录\r\n", strlen("\r\n发送校时\r\n"));
//			#endif
//								}
//							}
								login_state=1;
						}
						else  	// 已校时 已登录
						{
								GB17691_send_enable=1;   //郑州发送使能  小时
							  GB17691_data_put_buf(GB17691_data_handle);//上报郑州平台-----测试数据
								if(send_heart_flag>3)
								{

									if(get_vin(vin_info))   	//获取VIN
									{
										flag_bool_login = 0 ;  //VMS平台每次重启网络都需要 置0.重发登陆；
										reset_sim_net();						//120秒平台没有回复 重启网络
									}	
									send_heart_flag=0;
//									record_state=0;			// 备案失败
//									set_recordstate(0);									//保存备案信息
									
								}		
		//					GB17691_send_enable=1;   //郑州发送使能  小时
						}
					}
				}
				else		//  未备案
				{
//							if(((new_time_stamp - old_time_stamp) >= RECORD_SEND_INTERVAL) || (send_data_cnt == 0))
//							{
//								if((send_data_cnt >= RECORD_SEND_CNT) && (terminal_mode == TERMINAL_NET))
//								{
//									if(big_heart_flag==1)            
//									{
//											big_heart_flag=0;
//											big_heart_time_stamp=new_time_stamp;
//											update_big_heart_time(big_heart_time_stamp);
//									}
//									else
//									{
//											reconnect_sim_net();
//											record_state=1;	
//											flag_nosm2=1;
//											send_data_cnt=0;
//									}
//								}
//								else
//								{
//									record_send();
//									if(getTermianlState()==1)
//										record_state=1;
//									send_data_cnt++;
//									old_time_stamp = new_time_stamp;
//						#ifdef debug						
//												USARTSendOut(USART1, "\r\n发送备案\r\n", strlen("\r\n发送校时\r\n"));
//						#endif	
//								}
//							}
						record_state=1;
					
				}
				
		}
		else  // 不是联网状态
		{
			if(big_heart_flag==1)						//大心跳
			{
//			    if((new_time_stamp - big_heart_temp_time_stamp) >= 300) //发大心跳成功是否超时5分钟,此次心跳不发了,跳出,否则重新发 
			    if((new_time_stamp - big_heart_time_stamp) >= 300) //发大心跳成功是否超时5分钟,此次心跳不发了,跳出,否则重新发  
					{
						big_heart_flag=0;
#ifdef debug						
					  USARTSendOut(USART1, "send big heart over time!\r\n", 27);
#endif						
						big_heart_time_stamp=new_time_stamp;
						update_big_heart_time(big_heart_time_stamp);
					}
			}
		}
		//--------------------------大心跳
			if(big_heart_flag==1)      
			{	  
			    if(login_state == 1)
					{
					  get_GB17691_gps_info(&cur_location);
						if(cur_location.status.is_valid==0)              // 0 : 表示有效 1表示无效
						{  
							   //GB17691_data_handle |= SAVE_RESEND_DATA;     // 按实时上报数据格式处理,传一下GPS信息, 但不存储
//					      if(GB17691_send_enable)
//				        {
						      GB17691_data_handle |= REAL_TIME_UPLOAD;
//					      }
							
							  if(GB17691_data_handle)
							  {
									if(GB17691_send_enable)
	//								GB17691_data_put_buf(GB17691_data_handle);//上报郑州平台
									
							  	BOOL_data_put_buf(GB17691_data_handle); //大心跳上报至布尔平台
							  }                                
							  big_heart_flag=0;
							  big_heart_time_stamp=new_time_stamp;
							  update_big_heart_time(big_heart_time_stamp); 
				    }

		//				else if((new_time_stamp - big_heart_temp_time_stamp) >= 300)						// 没有定位  超时发送
				else if((new_time_stamp - big_heart_time_stamp) >= 300)						// 没有定位  超时发送
							{   //20190312 add  for test 300->180
	
						      GB17691_data_handle |= REAL_TIME_UPLOAD;
							  if(GB17691_data_handle)
							  {
									if(GB17691_send_enable)
//									GB17691_data_put_buf(GB17691_data_handle);//上报郑州平台
									
							  	BOOL_data_put_buf(GB17691_data_handle); //大心跳上报至布尔平台
							  }  
								
								big_heart_flag=0;
								big_heart_time_stamp=new_time_stamp;
								update_big_heart_time(big_heart_time_stamp); 
						}
						
					}
					else
					{
		//				if((new_time_stamp - big_heart_temp_time_stamp) >= 300) 	//超时
						if((new_time_stamp - big_heart_time_stamp) >= 300) 	//超时
						 {
							 
									BOOL_data_put_buf(GB17691_data_handle); //大心跳上报至布尔平?
									big_heart_flag=0;
#ifdef debug						
					        USARTSendOut(USART1, "send big heart over time1!\r\n", 28);
#endif
									big_heart_time_stamp=new_time_stamp;
								  update_big_heart_time(big_heart_time_stamp); 
							}
					}
			}
			//--------------------------------定时上报---
//        else
			if (can_state==1)
        {
            if((new_time_stamp - realtime_data_stamp) >= MSG_SEND_INTERVAL)
            {
                if(can_state==1) //防止大心跳标志为0时,对数据进行存储
                {
                    realtime_data_stamp = new_time_stamp;
                    GB17691_data_handle |= SAVE_RESEND_DATA;
                    if(GB17691_send_enable)
                    {
                        GB17691_data_handle |= REAL_TIME_UPLOAD;
                    }

                    if(GB17691_data_handle)//2  只存储
                    {	
											if(realup_cnt%10==4)
											{
//												GB17691_data_put_buf(GB17691_data_handle);//打包 存储 上报	
//												send_heart_flag++;  //  实时=郑州心跳 发送计数
											}
																					
											if(terminal_mode == TERMINAL_NET)
												GB17691_data_handle |= REAL_TIME_UPLOAD;  // 第二联路无条件上报
									
											if	(terminal_mode == TERMINAL_NET)
											{
												if(flag_bool_login ==0)
												{		
													bool_login_send_new();
					//								flag_bool_login = 1;
												}
											}
											send_heart_flag ++;
											BOOL_data_put_buf(GB17691_data_handle);
											
                        big_heart_time_stamp=new_time_stamp;      //更新大心跳时间戳
                        update_big_heart_time(big_heart_time_stamp);
                    }
                }
								realup_cnt++;
            }
						
						
						
						
						
						
        }
		}
//---------------------------登出------------------					
    else
    {
        if(((terminal_mode == TERMINAL_NET)) && login_state)
        {
            logout_send();
            login_state = 0;
            GB17691_send_enable = 0;

            for(uint8_t i=0; i<realtime_load_cnt; i++)  //将已发数据标记为发送成功
            {
                write_resend_data_flag(realtime_upload_flag_addr[i], SEND_DATA_FLAG);
            }
            realtime_load_cnt = 0;

            for(uint8_t i=0; i<resend_msg_cnt; i++)    //将补发的数据标记为发送成功
            {
                write_resend_data_flag(resend_msg_flag_addr[i], SEND_DATA_FLAG);
            }
            resend_msg_cnt = 0;
        }
        else
        {
					login_state = 0;
					GB17691_send_enable = 0;
					realtime_load_cnt = 0;
					resend_msg_cnt = 0;

//					if(BKP_ReadBkpData(BKP_DR5) == 0x5a5a)     //第一次上电 大心跳
//					{
//							big_heart_flag=1;
//							big_heart_temp_time_stamp = new_time_stamp;
//							BKP_WriteBackupRegister(BKP_DR5, 0xa5a5);
//					}

//					if(BKP_ReadBkpData(BKP_DR2) == 0xa5a5)
//					{
//							if((new_time_stamp - big_heart_time_stamp)>86400)  //时间间隔24小时
//									//	if((new_time_stamp - big_heart_time_stamp)>600)  //时间间隔10分钟
//							{
//									big_heart_flag=1;
//									big_heart_temp_time_stamp = new_time_stamp;
//							}
//					}
        }
    }
}
/*
功能:根据起始符 VIN  校验码, 找数据位置, 补发数据信息地址, 以及补发开始地址更新
参数: uint32_t* addr 补发开始地址  函数中会更新
      uint32_t* resend_info_addr  补发数据信息地址  函数中会更新
返回: 数据地址

*/
uint8_t* read_msg_from_flash(uint32_t* addr, uint32_t* resend_info_addr,uint8_t flash_area)
{
    uint16_t data_len = 0;
    uint16_t i = 0;
    uint32_t res_addr = *addr;
    uint8_t* data_pos = NULL;

    read_data_from_flash(GB17691_data_package,*addr,sizeof(GB17691_data_package),flash_area); //从addr读数据放GB17691_data_package
    for(i = 0; i < sizeof(GB17691_data_package); i++)
    {
        if(GB17691_data_package[i] == 0x23)
        {
            if((i + 23) < sizeof(GB17691_data_package))
            {
                if(GB17691_data_package[i+1] == 0x23)
                {
//					for(j = 0; j < sizeof(vin_info); j++)
//					{
//						if(GB17691_data_package[i+3+j] != vin_info[j])
//						{
//							break;
//						}
//					}
//					if(j == sizeof(vin_info))
//					{
//						data_len = (GB17691_data_package[i+22] << 8) + GB17691_data_package[i+23];
//						if((i+23+data_len+1+sizeof(save_resend_data_t)) < sizeof(GB17691_data_package))
//						{
//							uint8_t BCC_check = EvalBCC_FromBytes(&GB17691_data_package[i+2], data_len+22);
//							if(BCC_check == GB17691_data_package[i+23+data_len+1])
//							{
//								data_pos = &GB17691_data_package[i];
//								*resend_info_addr = res_addr + i + 24 + data_len + 1;
//								i += (24+data_len+1+sizeof(save_resend_data_t));
//								break;
//							}
//						}
//						else
//						{
//							//i++;
//							break;
//						}
//					}
                    data_len = (GB17691_data_package[i+22] << 8) + GB17691_data_package[i+23];
                    if((i+23+data_len+1+sizeof(save_resend_data_t)) < sizeof(GB17691_data_package))
                    {
                        uint8_t BCC_check = EvalBCC_FromBytes(&GB17691_data_package[i+2], data_len+22);
                        if(BCC_check == GB17691_data_package[i+23+data_len+1])
                        {
                            data_pos = &GB17691_data_package[i];
                            *resend_info_addr = res_addr + i + 24 + data_len + 1;
                            i += (24+data_len+1+sizeof(save_resend_data_t));
                            break;
                        }
                    }
                }
            }
            else
            {
                //i++;
                break;
            }
        }
    }
    *addr = res_addr + i;
    return data_pos;
}

void GB17691_resend_data(void)
{
    uint16_t data_len;
    uint8_t send_flag = 0;
    uint32_t resend_flag_addr;

    save_resend_data_t *save_data_satus;

    uint8_t *msg_pos = NULL;

//	uint32_t test_addr;

    if(login_state)
    {

        if((GB17691_send_enable) && (resend_msg_cnt < RESND_MSG_MAX_CNT) && (!resend_flash_addr_end_flag))
        {

            //			test_addr = resend_data_rd_flash_addr;

            msg_pos = read_msg_from_flash(&resend_data_rd_flash_addr,&resend_flag_addr,RESEND_DATA_FLASH_AREA);
            if(resend_data_rd_flash_addr >= RESEND_DATA_END_ADDR)
            {
                resend_flash_addr_end_flag = 1;

                resend_data_rd_flash_addr -= RESEND_DATA_END_ADDR;
                resend_data_rd_flash_addr += RESEND_DATA_START_ADDR;
            }
        }

        if(msg_pos != NULL)
        {
            data_len = (msg_pos[22] << 8) + msg_pos[23];
            save_data_satus = (save_resend_data_t*)&msg_pos[25+data_len];

            if(save_data_satus->resend_flag == RESEND_DATA_FLAG)
            {
                send_flag = 1;
            }
            else
            {
                if(save_data_satus->resend_flag == REALTIME_UPLOAD_FLAG)
                {
                    uint32_t cur_stamp =  get_time_stamp();
                    //	if((cur_stamp - save_data_satus->timestamp) > (HEART_INTERVAL >> 2))
                    if((cur_stamp - save_data_satus->timestamp) > (HEART_INTERVAL << 1))   //补发100秒前数据
                    {
                        send_flag = 1;
                    }
                }
            }

//				uint8_t test_data[100];
//				int res = snprintf((char*)test_data,100,"this is RESEND data READ addr  %08X, %02X,%02X,%02X,%02X,%02X,%02X\n", \
//				test_addr, \
//				msg_pos[24], \
//				msg_pos[25], \
//				msg_pos[26], \
//				msg_pos[27], \
//				msg_pos[28], \
//				msg_pos[29]);
//				usb_tx_data(test_data, strlen((char*)test_data));


            if(send_flag)
            {
//                GB17691_send_data(msg_pos,25+data_len);       //  22+2 +data_len +1
						int res=SocketWrite(1,24+data_len+1);
						if(res==0)
						{
							GB17691_send_data(msg_pos,25+data_len);       //  22+2 +data_len +1
						}
						else
						{
//							int re=OpenSocket(1);
						}
							
							
							
							
                if(terminal_mode == TERMINAL_NET)
                {
                    if(resend_flag_addr >= RESEND_DATA_END_ADDR)
                    {
                        resend_flag_addr -= RESEND_DATA_END_ADDR;
                        resend_flag_addr += RESEND_DATA_START_ADDR;
                    }

                    //重发标志偏移6个字节 ,即下一包数据补发开始地址
                    resend_flag_addr += 6;
                    if(resend_flag_addr >= RESEND_DATA_END_ADDR)
                    {
                        resend_flag_addr -= RESEND_DATA_END_ADDR;
                        resend_flag_addr += RESEND_DATA_START_ADDR;
                    }

                    resend_msg_flag_addr[resend_msg_cnt] = resend_flag_addr;
                    resend_msg_cnt++;
                    if(resend_msg_cnt > RESND_MSG_MAX_CNT)
                    {
                        resend_msg_cnt = 0;
                    }
																			//标记报文状态为发送成功
										resend_flash_addr_end_flag = 0;
										for(uint8_t i=0; i<realtime_load_cnt; i++)  //收到心跳命令,则将已发数据标记为发送成功
										{
												write_resend_data_flag(realtime_upload_flag_addr[i], SEND_DATA_FLAG);
										}
										realtime_load_cnt = 0;

										for(uint8_t i=0; i<resend_msg_cnt; i++)    //收到心跳命令,则将补发的数据标记为发送成功
										{
												write_resend_data_flag(resend_msg_flag_addr[i], SEND_DATA_FLAG);
										}
										resend_msg_cnt = 0;	
										
										
                }
            }
        }
    }
}

///**********************************************************
//        获得补发开始写地址
//***********************************************************/
void check_flash(void)
{
    uint32_t addr;
    uint32_t addr_temp;
    uint8_t *msg_pos;
    uint32_t resend_flag_addr;
    uint32_t max_time_stamp = 0;
//	rtc_time_t time;

    addr = RESEND_DATA_START_ADDR ;

    /*******************************************************
    从存储的补发数据里找出时间戳最大的那包数据地址, 以及实时数据读写地址
    ******************************************************/
    uint32_t check_flash_size  = RESEND_DATA_START_ADDR;

    while(check_flash_size < RESEND_DATA_END_ADDR)
    {
        addr_temp = addr;
        msg_pos = read_msg_from_flash(&addr,&resend_flag_addr,RESEND_DATA_FLASH_AREA); // 从FLASH里找 数据位置, 补发开始地址, 补发数据信息地址,

        check_flash_size += (addr - addr_temp);

        if(addr>=RESEND_DATA_END_ADDR)
        {
            addr -= RESEND_DATA_END_ADDR;
            addr += RESEND_DATA_START_ADDR;
        }
        if(msg_pos != NULL)
        {
            uint16_t data_len = (msg_pos[22] << 8) + msg_pos[23];
            uint32_t timestamp = (msg_pos[28+data_len] << 24) + (msg_pos[27+data_len] << 16) +(msg_pos[26+data_len] << 8) +(msg_pos[25+data_len]);
            if(timestamp >= max_time_stamp)
            {
                max_time_stamp = timestamp;
                realtime_data_wr_flash_addr = addr;
            }
        }
    }

    /***************************************************************************
    	判断上面得到的实时数据读写FLASH地址之后的FLSAH是否可以写,
    	若不可以写,则跳到下一个secter, 若可以写则接着写.
    	注意:	FLASH写之前会擦除,是FF才可以写, 写一个新的secter时会先擦除该扇区再写!
    	***************************************************************************/

    uint16_t secoff = realtime_data_wr_flash_addr % FLASH_SECTOR_SIZE;
    uint16_t remain = FLASH_SECTOR_SIZE - secoff;
    uint16_t read_size = 0;
    uint16_t i;
    if(secoff > 0)
    {
        do
        {
            if(remain <= sizeof(GB17691_data_package))
            {
                read_size = remain;
            }
            else
            {
                read_size = sizeof(GB17691_data_package);
            }
            read_data_from_flash(GB17691_data_package,realtime_data_wr_flash_addr,read_size,RESEND_DATA_FLASH_AREA);
            for(i=0; i<read_size; i++)
            {
                if(GB17691_data_package[i] != 0xFF)
                {
                    realtime_data_wr_flash_addr = ((realtime_data_wr_flash_addr / FLASH_SECTOR_SIZE) + 1)*FLASH_SECTOR_SIZE;  //如果非空，开始从下一个secter开始。
                    break;
                }
            }
            if(i==read_size)
            {
                remain -= read_size;
            }
            else
            {
                break;
            }
        }
        while(remain > 0);
    }

    ckeck_flash_flag = 1;
}


void GB17691_CONTROL_CHECK(void)
{
	if(flag_answer_net)
	{
			uint8_t  i;
			for( i=0;i<(sizeof(CtlIDNo)/sizeof(CtlIDNo[0]));i++)
			{
				if(CtlIDNo[i]!=0)
				{
					GB17691_ack_send(CtlID[i],CtlIDNo[i],CON_SUCCESS,0);  //复位成功
					osDelay(800);
					CtlID[i] = 0;
					CtlIDNo[i] = 0;
					flag_answer_net --;
					break;
				}
			}
			if(i>=sizeof(CtlIDNo)/sizeof(CtlIDNo[0]))
			{
				flag_answer_net = 0;
				memset(CtlIDNo,sizeof(CtlIDNo)/sizeof(CtlIDNo[0]),0);
				memset(CtlID,sizeof(CtlID)/sizeof(CtlID[0]),0);
				flag_restart_net=0;
				flag_relog_net = 0;
				flag_reset_net = 0;
				flag_answer_net= 0;
				flag_setvin_net= 0;
				flag_ver_net = 0;
				flag_vin_net = 0;
			}
			else
			{
					if(flag_restart_net)
					{
						flag_restart_net = 0;
						MCU_RESET();
					}
					else if(flag_reset_net)
					{
						flag_reset_net = 0 ;
						ResetData();
					}
					else if(flag_setvin_net)
					{
						flag_setvin_net = 0;
						record_state = 0;
						login_state = 0;
					}
					else if(flag_relog_net)//Relog
					{
						flag_relog_net = 0;
						flag_bool_login = 0 ;
					}
					else if(flag_ver_net)
					{
						flag_ver_net = 0;
					}
			}
		}
}
uint8_t gps_set_time(void)
{
    rtc_time_t cur_time;
    if(!is_set_time())
    {
        if(get_gps_time(&cur_time))
        {
            set_current_time(cur_time);
            if(BKP_ReadBkpData(BKP_DAT6) == 0x5a5a)
            {
                BKP_WriteBkpData(BKP_DAT6, 0xa5a5);  //第一次校时成功
            }
            else
            {
                BKP_WriteBkpData(BKP_DAT6, 0x5a5a);  //第一次校时成功
            }
            return 1;
        }
        return 0;
    }
    return 0;
}


uint8_t get_login_state(void)
{
    return login_state;
}

uint8_t get_lkt4305_state(void)
{
    return lkt4305_state;
}


uint8_t get_big_heart_flag(void)
{
    return big_heart_flag;
}
void Gps_Led_Init(void)
{
	GPIO_InitType	GPIO_InitStructure;
	RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOA, ENABLE);
	
	GPIO_InitStructure.Pin        = GPIO_PIN_3;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitPeripheral(GPIOA, &GPIO_InitStructure);
	GPS_LED_OFF;
}


u32 iwdg_17691=0;
void LKT4305_Key_Init(void)
{
	u8 lkt4305_init_cnt=0;
	u8 *ps=NULL;
		while((get_keystate()!=1))
		{	
			LKT4305Test();	
			osDelay(100);
			lkt4305_init_cnt++;
			
			if(lkt4305_init_cnt%5==4)
				LKT4305Init();
			
			if(lkt4305_init_cnt>50)
				break;
			iwdg_17691++;	
		}
		
		ps=ReadPublickey();														//公钥
		if(ps==NULL)
			ps=ReadPublickey();														//公钥	
		
		if(ps!=NULL)
			lkt4305_state=1;
	
}
static void check_big_heart(void)
{		
		uint32_t new_time_stamp;
    new_time_stamp = get_time_stamp();
	
		if(BKP_ReadBkpData(BKP_DAT5) == 0x5a5a)     //第一次上电 大心跳
		{
//				big_heart_flag=1;
				BKP_WriteBkpData(BKP_DAT5, 0xa5a5);
		}
				if(BKP_ReadBkpData(BKP_DAT2) == 0xa5a5)
					{
//							if((new_time_stamp - big_heart_time_stamp)>86400)  //时间间隔24小时
//	//							if((new_time_stamp - big_heart_time_stamp)>600)  //时间间隔10分钟
//							{
////USARTSendOut(USART1, "\r\n！！！11！！！！！！！！\r\n", strlen("\r\n！！！11！！！！！！！！\r\n"));
//									big_heart_flag=1;
//						//			big_heart_temp_time_stamp = new_time_stamp;
//									big_heart_time_stamp = new_time_stamp;
//							}
					}
}
void Get_SM2id(void)
{
		u8 IMEI_data[15];
		if(get_idstate(SM2ID)==1)				// ID已存入flash
		{
//			while(Set4305_Sm2id()!=1)			//设置SM2 ID
//			{
//				if(j%5==4)
//					LKT4305Init();									//连续5次失败 复位安全芯片
//				j++;
//				osDelay(30);
//			}
		}
		else						//ID 未存入FLASH  等待IMEI读取成功后生成ID存入flash
		{
			while(!get_imei(&IMEI_data[0]))					//获取到IMEI
			{	
				iwdg_17691++;	
				osDelay(100);
			}
			SM2ID[0]=ID_BOOL0;
			SM2ID[1]=ID_BOOL1;
			SM2ID[2]=ID_BOOL2;
			memcpy(&SM2ID[3],&IMEI_data[2],13);
			set_idstate(SM2ID);								// 将ID存入FLASH				
		}	
}



void Checkvin_SetUds(void)
{
 
        if(flag_set_vin)
        {
					const char RESET_VIN[] 	 = "RESET000000000000";
					const char RESET_VIN_1[] = "reset000000000000";
					const char RESETSYS_VIN[] 	 = "RESET111111111111";
					
					const char DATONG_VIN_1[] = "LSH";
					const char DATONG_VIN_2[] = "LSK";
					const char WUSHILING_VIN_1[] = "LWLG";
					const char BENCHI_VIN_1[] = "WDAN";
					const char DFH4250_VIN[9]="LGAG4DY3";
					const char JRLH_VIN[9]="L584C4D9";

					const char JiangLing_VIN[6]   ="LEFYED";
					const char JiangLing_VIN_2[6] ="LEFAEC";
					const char JiangLing_VIN_3[6] ="LEFAFC";
					const char JiangLing_VIN_4[8] ="LEFYECC2";
					const char DF_KaiPuTe_VIN[8]  ="LGDCH91G";  //东风-凯普特
					const char JN_50ling_VIN[8]   ="LWLDAA5G";  //济南-50铃

					uint8_t  res , res_1, res_2,res_3,res_4,res_5;
					res   = memcmp(vin_info,RESET_VIN  ,sizeof(vin_info));
					res_1 = memcmp(vin_info,RESET_VIN_1,sizeof(vin_info));
					if((!res)||(!res_1))              //回复出厂设置
					{
						ResetData();
//						W25QXX_Erase_Chip_osMutex();
//						for(uint8_t i=0 ;i< sizeof(vin_info);i++)
//						{
//							vin_info[i] = 0xFF;
//						}
					}
					else 
					{
					res   = memcmp(vin_info,RESETSYS_VIN  ,sizeof(vin_info));
					if(!res)
							MCU_RESET();
					else   //设置VIN 判断车型
					{
						res   = memcmp(vin_info,DATONG_VIN_1,3);
						res_1  = memcmp(vin_info,DATONG_VIN_2,3);
            if((!res)||(!res_1))	 //判断是不是大通
						{							
							SetUdsProtocol(DaTong_9);	
							SetUdsPara(_DCUID,9);
							Set_evads(8,1);
							flag_UDSSEND=1;
						}
						else
						{							
							res   = memcmp(vin_info,WUSHILING_VIN_1,4);
							if(!res)  // 判断是不是五十铃
								{
									SetUdsProtocol(wushiling_17);	
									SetUdsPara(_DCUID,17);
									Set_evads(8,1);
									flag_UDSSEND=1;
								}
								else
								{
									res   = memcmp(vin_info,BENCHI_VIN_1,4);
									if(!res)  // 判断是不是奔驰
										{
											SetUdsProtocol(benchiact_18);	
											SetUdsPara(_DCUID,18);
											Set_evads(8,1);
											flag_UDSSEND=1;
										}	
									else
										{
											res   = memcmp(vin_info,DFH4250_VIN,8);
											if(!res)  // 判断是不是东风
												{
													SetUdsProtocol(DongFeng_2);	
													SetUdsPara(_DCUID,2);
													Set_evads(8,1);
													flag_UDSSEND=1;
												}	
											else
											{
											res   = memcmp(vin_info,JRLH_VIN,8);
											if(!res)  // 判断是不是集瑞联合
												{
													SetUdsProtocol(YiLiLanJie_6);	
													SetUdsPara(_DCUID,6);
													Set_evads(8,1);
													flag_UDSSEND=1;
												}
											else
											{
												res    = memcmp(vin_info,JiangLing_VIN,sizeof(JiangLing_VIN));
												res_1  = memcmp(vin_info,DF_KaiPuTe_VIN,sizeof(DF_KaiPuTe_VIN));
												res_2  = memcmp(vin_info,JN_50ling_VIN,sizeof(JN_50ling_VIN));
												res_3  = memcmp(vin_info,JiangLing_VIN_2,sizeof(JiangLing_VIN_2));
												res_4  = memcmp(vin_info,JiangLing_VIN_3,sizeof(JiangLing_VIN_3));
												res_5  = memcmp(vin_info,JiangLing_VIN_4,sizeof(JiangLing_VIN_4));
													if((!res)||(!res_1)||(!res_2)||(!res_3)||(!res_4)||(!res_5)) // 判断是不是江陵
													{
														SetUdsProtocol(JiangLing_19);	
														SetUdsPara(_DCUID,19);
														Set_evads(8,1);
														flag_UDSSEND=1;
													}
											}
											}		
										}
								}
						}																	
					}
				}
					set_vin(vin_info);
					flag_set_vin=0;
				uprintf("SET OK\r\n");
				flag_setvin_net=1;
        }
}
uint8_t alarm_send_cnt = 0 ;
uint8_t alarm_state_flag=0;
extern uint8_t get_power_state(void);
u32 randnum=0;        //伪随机数 计数  生产排放数据使用
u16 GPS_LED_cnt=0;			//  LED显示 V1.8版
void GB17691_task(void const *pvParameters)
//void GB17691_task(void)
{
    u8 vin_state=0;
    u8 cnt_getvin=0;
	
		check_big_heart();
		while(!get_power_state())
		{
			 osDelay(10);
		}
		Gps_Led_Init();
		 osDelay(10);
		 LKT4305Init();		
		iwdg_17691++;	
		 osDelay(1000);
		iwdg_17691++;	
		
	  LKT4305_Key_Init();
		
		iwdg_17691++;	
		
		
		record_state=get_recordstate();	// 获取备案状态
//		record_state=0;									
		//获取安全芯片ID
		Get_SM2id();
		
    vin_state=get_vin(vin_info);   	//获取VIN
	
		if(!vin_state)			//如果没有设置VIN，不发送备案信息
			record_state=1;
    while(1)
    {
				CheckPenaltyZone();
				iwdg_17691++;	
				randnum++;
				GPS_LED_cnt++;			
        if(!vin_state)
        {
            if(cnt_getvin<10)
            {
                cnt_getvin++;
                if(get_vin(vin_info))
                {
                    vin_state = 1;
                }
						}
        }
				
				Checkvin_SetUds	();
				
				if(GPS_LED_cnt%100==99)
				{
				get_GB17691_gps_info(&cur_location);
				if(cur_location.status.is_valid==0) 
					{
						if(!READ_GPS_LED)
			 				GPS_LED_ON;	
					}
					else
					{
						if(!READ_GPS_LED)
							GPS_LED_ON;	
						else
							GPS_LED_OFF;		
					}
				}

        //	if(ckeck_flash_flag)

        if((ckeck_flash_flag &&(can_state==1)) || (can_state==0) || (can_state==0xff))
        {
            //	CAN_analysis_data();
            gps_process_frame();
            //gps_set_time();
            if(gps_set_time() && (BKP_ReadBkpData(BKP_DAT6) == 0x5a5a))   //第一次GPS校时成功
            {
								big_heart_time_stamp=get_time_stamp();
                update_big_heart_time(get_time_stamp());
            }

					if((randnum%500==88)&&(alarm_state_flag !=0 ))		//5s发一次  alarm_state_flag !=0 ->没写vin得终端都不发送拆除报警
						{
							if(GB17691_send_enable)
							{

								if(alarm_send_cnt > 2)
								{
										alarm_state_flag = 4;
								}
								disman_alarm_send();
								alarm_send_cnt ++;
								#ifdef debug						
								USARTSendOut(USART1, "\r\n发送拆除报警\r\n", strlen("\r\n发送拆除报警\r\n"));
								#endif	
							}
						}
						else
						{

            //if(is_set_time())
            //	{
            GB17691_real_time_upload();
            //		}
						GB17691_resend_data();
            GB17691_CMD_check();
//						GB17691CheckCmd();
//						GB17691_CONTROL_CHECK();
            osDelay(GB17691_THREAD_DEALY);
						}
        }
        else
        {
					if(comupdate_mode_flag!=1)
            check_flash();
            osDelay(100);
        }
    }
}

void GB17691_init(void)
{

    //check_flash();
}

osThreadId tid_GB17691_task;
osThreadDef (GB17691_task, osPriorityNormal, 1, 1600);
void GB17691_task_init(void)
{
    TERMINAL_SOFT_VER=GB17691_get_softver_Info();   // 获取设备类型 软件版本号
    if(BKP_ReadBkpData(BKP_DAT2) == 0xa5a5)
    {
        big_heart_time_stamp= ((uint32_t)BKP_ReadBkpData(BKP_DAT3) <<16 )|((uint32_t)BKP_ReadBkpData(BKP_DAT4));
        test_flag=0x00;
    }
    else
    {
        big_heart_time_stamp=get_time_stamp();        //第一次上电
        update_big_heart_time(get_time_stamp());
        BKP_WriteBkpData(BKP_DAT5, 0x5a5a);
        test_flag=0x01;
    }
    tid_GB17691_task = osThreadCreate (osThread(GB17691_task), NULL);
}









