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
//uint8_t vin_info[17] = {0x4C, 0x50, 0x48, 0x46, 0x41, 0x33, 0x42, 0x44, 0x30, 0x48, 0x59, 0x31, 0x37, 0x30, 0x32, 0x33, 0x35};    //vin��Ϣ
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
//} CONTROL_ID ;          //��������

typedef enum
{
   NO_USE,
   SETUPDATA_FIRMWARE,//�����������http��������һ���ֽ�Ϊ����ڶ����ֽڿ�ʼΪURL��
   SETDO,//��һ���ֽڣ�SHUTDOWN1:1, RESTART:2,    RESET_DATA:3,    OFFLINE,:4,  Relog:5��

   SETVIN,//�����������VIN��
   SETUDS,//�������������һ���ֽ�1Ϊ����0Ϊ�أ��ڶ����ֽ�ΪUDS���룬���������������ġ�
   SETSEND,//�������ֽڣ�1����ʹ�ܣ�0Ϊʧ�ܣ�OFF1939SEND��ON1939SEND��OFF15765SENG09��ON15765SENG09��OFF15765SEND01��ON15765SEND01����

GETVER,//��*�û���*Ӧ�𣬣����ֽڰ汾��-�豸����-�豸��-�汾�ţ���
GETVIN,//��Ӧ�����������VIN��
GETUDS,//��Ӧ�����������UDS��Ϣ��

GETSENDSTATE,//��Ӧ������������ο�SETSEND��
GETTEMSTATE,//��Ӧ�����������״̬��Ϣ��

GETOBDDATA,//��*�û���*Ӧ�����������candata��
GETCANDATA,//��Ӧ�����������candata��
GETCANSTATE,//��Ӧ�������������һ���ֽ�6.14��Э�����ݣ��ڶ����ֽ�6.14��Э�����ݣ�
         //�������ֽ�19��Э�����ݣ����ĸ��ֽ�19��Э�����ݣ�
        //������ֽ�311��Э�����ݣ��������ֽ�311��Э�����ݣ�
        //0����û��Э�飬1����15765��2����1939��
SETCLEARDTC,//��Ӧ�������������һ���ֽ�6.14��1939������ϣ��ڶ����ֽ�6.14��15765������ϣ�
        //�������ֽ�19��1939������ϣ����ĸ��ֽ�19��15765������ϣ�
       //������ֽ�311��1939������ϣ��������ֽ�311��15765��������ݣ���
} CONTROL_ID ;          //��������

typedef enum
{
    CON_SUCCESS=1,
    CON_FAILED,
} CONTROL_STATE	;          //��������


extern uint8_t comupdate_mode_flag;
extern uint8_t vin_info[17];
//static uint8_t read_vin_state = 0;            //��VIN״̬  0:����VIN״̬  1:��VIN״̬
static uint8_t login_state = 0;               //����״̬   0:������״̬   1:����״̬
uint8_t timing_state = 0;              //Уʱ״̬   0:��Уʱ״̬   1:Уʱ״̬
static uint8_t record_state=0;					// ����״̬�� 0δ����		 1�ѱ���
static uint8_t lkt4305_state=0;					// ����״̬�� 0������		 1����
//static  uint8_t flag_nosm2=0;				//			0 ǩ���ϱ�   1����ǩ���ϱ�
//static uint8_t alarm_state = 0;              //����״̬
static uint16_t lognum = 0;                   //�������
static uint8_t send_data_cnt = 0;             //��������
uint8_t GB17691_CMD_data[256];             //�����256
static uint16_t gb17691_rd_pos = 0;           //��λ��
static uint16_t cmd_data_len;                 //�������ݳ���
static uint8_t check_cmd_info_flag = 0;       //���������Ϣ��־
static uint8_t CMD_check_dealy;               //��������ʱ

//static uint32_t	pre_resend_timestamp;
//static uint8_t pre_resend_flag = 0;
static uint8_t GB17691_send_enable = 0;       //GB17691����ʹ�� 0:��ʹ�� 1 :ʹ��

//static uint32_t resend_record_addr = RESEND_RECORD_START_ADDR; //���油����ַ
//static uint32_t resend_wrtie_size = 0;                          //����д��С��ʼΪ0

static uint32_t realtime_upload_flag_addr[HEART_INTERVAL];   //ʵʱ�ϱ���Ǵ洢����
static uint8_t realtime_load_cnt = 0;                        //ʵʱ�ϱ�����
static uint32_t resend_msg_flag_addr[RESND_MSG_MAX_CNT];     //������Ǵ洢����
static uint8_t resend_msg_cnt = 0;                           //��������

//static uint8_t usb_send_enable = 0;
static uint8_t terminal_mode = TERMINAL_FREE;  //0:�����ӣ�1�����磬2��USB
static uint32_t old_time_stamp;                //��ʱ���
//static uint32_t alarm_data_stamp;              //�������ݴ�
static uint32_t realtime_data_stamp;           //ʵʱ���ݴ�
//static uint32_t heart_stamp;                   //������
//static uint32_t alarm_resend_stamp;            //����������
static uint8_t send_heart_flag = 0;            //���������
static uint8_t resend_flash_addr_end_flag = 0;  //���Ҳ���FLASH��������ַ,����1 , �ڽ��յ�����ʱ��0,
//static uint8_t key_state = 0;                  //Կ��״̬
//static uint8_t charge_state = 0;               //���״̬

//static uint32_t current_alarm_flag = 0;        //��ǰ������־
//static uint8_t current_max_alarm_level = 0;    //��ǰ��󱨾�����
//static uint8_t alarm_real_time_send_cnt;
//static uint8_t alarm_resend_cnt;                //������������
//static uint8_t alarm_resend_over;               //������������

static uint32_t realtime_data_wr_flash_addr = RESEND_DATA_START_ADDR; //ʵʱ���ݴ洢��ַ, ��ʼΪ�������ݿ�ʼ��ַ
static uint32_t resend_data_rd_flash_addr = RESEND_DATA_START_ADDR;   //�������ݶ�FLASH��ַ, ��ʼΪ�������ݿ�ʼ��ַ
//static uint32_t alarm_data_wr_flash_addr = ALARM_DATA_START_ADDR;     //�������ݴ洢��ַ, ��ʼΪ�������ݿ�ʼ��ַ
//static uint32_t alarm_data_rd_flash_addr = ALARM_DATA_START_ADDR;     //�������ݶ�FLASH��ַ,��ʼΪ�������ݿ�ʼ��ַ
static uint8_t ckeck_flash_flag = 0;              //���FLASH���

uint8_t GB17691_data_package[1024];               //GB17691���ݰ����� ,��С512

uint8_t data_sign[64];       		//ǩ����Ϣ      
uint8_t SM2ID[16];						//SM2ID��Ϣ
uint8_t platformPublicKeyData[64];  //ƽ̨��Կ


uint8_t big_heart_flag=0;                            //1:���������״̬, 0:�����������    �������ģ������
uint32_t big_heart_time_stamp;                       //������ʱ���
//static uint32_t big_heart_temp_time_stamp;           //��������ʱʱ���

static uint8_t TERMINAL_SOFT_VER=0;
uint16_t INFO_STREAM_NO=1;                         //��Ϣ��ˮ��


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

extern u8 flag_set_vin;   	//����VIN��־
extern u8 flag_UDSSEND;
u8  Actblue_State=1;//ָ֡��

//static uint32_t resend_data_stamp;
//
////#pragma pack(1)   //���ֽڶ���
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
////#pragma pack()  //ȡ��ָ�����룬�ָ�ȱʡ����

//******����������״̬**************//
u8 ACTBLU_date_state=0;//����������״̬

#define upload_successful 1//�ϴ��ɹ�
#define unregistered      2//δע��
#define Check_failure     3//����У��ʧ��
#define unknown_failure   4//δ֪ʧ��
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
  * @����:   hex תascii
  * @����:   None
  * @����ֵ: None
  **********************************************************************/
u16 hex2ascii( u8 *mubiao ,u8 *yuan,u16 BytNo )								//�� stm32 �� can�ڽ��յ�8���ֽ�ת����16���ֽڵ�ascII ��ʽ
{
	u16 j=0; u8 temp_data=0; 
	for(u16 i=0;i<BytNo;i++)
	{
		temp_data=(*(yuan+i))>>4;
		if(temp_data<=9)            //���ascii����
		{
			*(mubiao+j) = temp_data + 0x30;
		}
		else 																					//���ascii��д��ĸ
		{
			*(mubiao+j)= temp_data +	0x37;
		}
		j++;

		temp_data=(*(yuan+i))&0x0f;	
		if(temp_data<=9)            //���ascii����
		{
			*(mubiao+j) = temp_data + 0x30;
		}
		else 																					//���ascii��д��ĸ
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
        res = sim_uart_tx(pdata, data_len);  //��Ҫ�жϴ���BUF�Ƿ�����
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
					send_heart_flag=0;    // ����������0  ������������
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
				if(pdata[33]==1)	//Http����
				{
					u16	url_len = 0 ;
					char url_head[] ="\r\nAT+HTTPPARA=\"URL\",\"";
					url_len = ((uint16_t)pdata[33] << 8) + pdata[34] ;		//����ط���Ҫ��������˵��
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
				else if(pdata[33]==2)//FTP����
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
								memcpy((void *)flag_answer_net_time,pdata+24,6);  //ʱ��ֻ�ܱ���һ������������CtlIDNo����-210304-msc
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
											//��Ҫдflash��������
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
											//��Ҫдflash��������
										}
										else if(pdata[33]==5)//Relog
										{
											flag_relog_net=1;
										}
								}
								else if(pdata[32]==SETVIN) //����VIN
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
										//��Ҫдflash��������
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
        if((new_time_stamp - old_time_stamp) <= 5)  //5->30 for�������ֵ���
        {
            rtc_time_t server_time;
            rtc_time_t set_time;
            server_time.year = pdata[24];
            server_time.month = pdata[25];
            server_time.date = pdata[26];
            server_time.hour = pdata[27];
            server_time.minute = pdata[28];
            server_time.second= pdata[29];
            set_time_preset((new_time_stamp - old_time_stamp + TIME_PRESET_VALUE),server_time,&set_time);  //�������ʱ��set_time
            if(set_current_time(set_time) == SUCCESS)
            {
                timing_state = 1;
				#ifdef debug						
										USARTSendOut(USART1, "\r\n-------��Уʱ�ɹ���-------\r\n", strlen("\r\n-------��Уʱ�ɹ���-------\r\n"));
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
		case 0x82:	//VMSӦ��ɹ�
		case 0x92:  //VMSӦ��ʧ��
    {
        uint32_t new_time_stamp = get_time_stamp();
				send_heart_flag =0;
        break;
    }
		case 0x81: //vms ����ɹ�
		case 0x91: //vms ����ɹ�   
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
	 if(GB17691_CMD_data[0] == (uint8_t)(START_STRING >> 8))//�ж���ʼ�� �� VIN
                {
                    if(GB17691_CMD_data[1] == (uint8_t)(START_STRING))
                    {
												cmd_data_len = (uint16_t)(GB17691_CMD_data[22] << 8) + (uint16_t)GB17691_CMD_data[23];
                        cmd_data_len += 25;
                        //���ݵ�Ԫ���ȣ�����24�ֽڵ���Ϣ��1���ֽڵ�У�飬
                        if(cmd_data_len <= sizeof(GB17691_CMD_data))  //�ж������Ƿ񳬳���Χ, gb17691_rd_pos++ ,�ٽ���ѭ��
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
#define upload_successful 1//�ϴ��ɹ�
#define unregistered      2//δע��
#define Check_failure     3//����У��ʧ��
#define unknown_failure   4//δ֪ʧ��
*/
void GB17691_AIKELAN_CheckCmd(void)//���հ�����
{
 //  get_sim_data_multiple_bytes(GB17691_CMD_data, gb17691_rd_pos, len);
	   get_sim_data_multiple_bytes(GB17691_CMD_data, gb17691_rd_pos, 35);
	      if(GB17691_CMD_data[0] == (uint8_t)(ACTBLU_START_STRING >> 8))//�ж���ʼ�� ��У����Ϣ
                {
                    if(GB17691_CMD_data[1] == (uint8_t)(ACTBLU_START_STRING))
                    {
												if(GB17691_CMD_data[2] ==0)//ע��֡
												{
																cmd_data_len=gb17691_rd_pos;	//���ݵ�Ԫ����															
															if(cmd_data_len <= sizeof(GB17691_CMD_data))  //�ж������Ƿ񳬳���Χ, gb17691_rd_pos++ ,�ٽ���ѭ��
															{
																if(EvalBCC_FromBytes(&GB17691_CMD_data[2], cmd_data_len-4) == GB17691_CMD_data[cmd_data_len-2])
																{
																		gb17691_rd_pos += cmd_data_len;
																	  if (GB17691_CMD_data[3]==1)//�ϴ��ɹ�
																		{
																		 Actblue_State=1;
																		 ACTBLU_date_state=	upload_successful;																			
																		}	
																		else if(GB17691_CMD_data[3]==3)//У��ʧ��
																		{
																			Actblue_State=0;
																			ACTBLU_date_state=Check_failure;																																	
																		}
																		else //δ֪����
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
												else if(GB17691_CMD_data[2] ==1)//ָ��֡
												{
														  cmd_data_len=gb17691_rd_pos;	//���ݵ�Ԫ����
															if(cmd_data_len <= sizeof(GB17691_CMD_data))  //�ж������Ƿ񳬳���Χ, gb17691_rd_pos++ ,�ٽ���ѭ��
															{
																if(EvalBCC_FromBytes(&GB17691_CMD_data[2], cmd_data_len-4) == GB17691_CMD_data[cmd_data_len-2])
																{
																		gb17691_rd_pos += cmd_data_len;																								
																		if (GB17691_CMD_data[3]==1)//�ϴ��ɹ�
																		{
																			 Actblue_State=1;
																			 ACTBLU_date_state=	upload_successful;															
																		}	
																		else if(GB17691_CMD_data[3]==3)//У��ʧ��
																		{
																			Actblue_State=0;
																			ACTBLU_date_state=Check_failure;																																	
																		}
																		else //δ֪����
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
							//--------------------------------------֣���¼ӿ�ʼ------------------
					  if(get_sim_rx_data_size(gb17691_rd_pos) > 90)  //���ٽ��յ�7���ֽڣ��ſ�ʼ����                ֣�����  ʵʱ���ݰ��ظ�  ��7���ֽ�
            {
                get_sim_data_multiple_bytes(GB17691_CMD_data, gb17691_rd_pos, 8);
                if(GB17691_CMD_data[0] == (uint8_t)(START_STRING >> 8))//�ж���ʼ�� �� VIN
                {
                    if(GB17691_CMD_data[1] == (uint8_t)(START_STRING))
                    {																				
											if((GB17691_CMD_data[6]=='Q')&&(GB17691_CMD_data[7]=='N')	)					//����
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
						//--------------------------------------֣���¼ӽ���------------------
											
											
											
					
            if(get_sim_rx_data_size(gb17691_rd_pos) > 23)  //���ٽ��յ�24���ֽڣ��ſ�ʼ����
            {
                get_sim_data_multiple_bytes(GB17691_CMD_data, gb17691_rd_pos, 24);
                if(GB17691_CMD_data[0] == (uint8_t)(START_STRING >> 8))//�ж���ʼ�� �� VIN
                {
                    if(GB17691_CMD_data[1] == (uint8_t)(START_STRING))
                    {																						
                        cmd_data_len = (uint16_t)(GB17691_CMD_data[22] << 8) + (uint16_t)GB17691_CMD_data[23];
                        cmd_data_len += 25;
                        //���ݵ�Ԫ���ȣ�����24�ֽڵ���Ϣ��1���ֽڵ�У�飬
                        if(cmd_data_len <= sizeof(GB17691_CMD_data))  //�ж������Ƿ񳬳���Χ, gb17691_rd_pos++ ,�ٽ���ѭ��
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
                if(CMD_check_dealy < GB17691_CMD_CHECK_DEALY) //��ʱ1s
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


void bool_login_send_new(void)   //����
{
  
    rtc_time_t cur_time;
    uint8_t send_data[120];
    uint8_t send_data_size;

    send_data[0] = (uint8_t)(START_STRING >> 8);  //��ʼ��
    send_data[1] = (uint8_t)START_STRING;
    send_data[2] = VEHICLE_LOGIN_CMD;             //��¼����

    GB17691_get_VIN_Info(&send_data[3]);
    //send_data[20]= GB17691_get_softver_Info();
    //memcpy(&send_data[3],vin_info,sizeof(vin_info));  //VIN

    send_data[20] = TERMINAL_SOFT_VER;        //����汾��
    send_data[21] = NO_ENCRYPT;               //���ݼ��ܷ�ʽ, ������

    get_current_time(&cur_time);              //���ݲɼ�ʱ��
    send_data[24] = cur_time.year;
    send_data[25] = cur_time.month;
    send_data[26] = cur_time.date;
    send_data[27] = cur_time.hour;
    send_data[28] = cur_time.minute;
    send_data[29] = cur_time.second;


//    lognum = get_lognum(cur_time);           //��¼��ˮ��
//    lognum++;
    send_data[30] =0x81;
    send_data[31] = 0;		
		send_data [32]=0x0f;

get_imei(&send_data[33]);                //SIMģ��IMEI
	
	lognum = get_lognum(cur_time);           //��¼��ˮ��
	lognum++;
	send_data[48] = (uint8_t)(lognum >> 8);
	send_data[49] = (uint8_t)lognum;
	
	get_iccid(&send_data[50]);              //SIM��ICCID�� 
		
	send_data[22] = 0;                      //ʱ��6��������ˮ��2��ICCID��20��IMEI��Ϣ��1  ���� 2+ imei 15  �� 46�ֽ�
	send_data[23] = 0x2e;
		
	send_data_size = 71;                   //���������ܸ���:22+2+46+1 ;
	send_data[send_data_size-1] = EvalBCC_FromBytes(&send_data[2], send_data_size-3);  //У��λ

		int res=SocketWrite(1,send_data_size);
						if(res==0)//�˿ڴ򿪳ɹ�
						{
									GB17691_send_data(send_data,send_data_size);                         //ʵʱ�ϱ�									
#ifdef debug
	gb17691_print_data(send_data,send_data_size);		
#endif
		
					}
						else//�˿ڴ�ʧ�ܡ����´�socket
						{
//							int re=OpenSocket(1);
						}
}


void timing_send(void)  //Уʱ
{
	rtc_time_t cur_time;
	uint8_t send_data[50];
	uint8_t send_data_size;
	
	send_data[0] = (uint8_t)(START_STRING >> 8);         //��ʼ��
	send_data[1] = (uint8_t)START_STRING;          
	send_data[2] = VEHICLE_TIMING_CMD;                   //Уʱ����
	
	GB17691_get_VIN_Info(&send_data[3]);
	
			if(getTermianlState()==1) 						//����״̬�������VIN   //20200323 del
		{		
			for(u8 i=0;i<17;i++)
			{
				send_data[3+i]='L';
			}
		}
	
	//send_data[20]= GB17691_get_softver_Info();
//	memcpy(&send_data[3],vin_info,sizeof(vin_info));     //VIN
	send_data[20] = TERMINAL_SOFT_VER;                   //����汾��
	send_data[21] = NO_ENCRYPT;                          //���ݼ��ܷ�ʽ, ������
	
	get_current_time(&cur_time);                         //���ݲɼ�ʱ��
	send_data[24] = cur_time.year;
	send_data[25] = cur_time.month;
	send_data[26] = cur_time.date;
	send_data[27] = cur_time.hour;
	send_data[28] = cur_time.minute;
	send_data[29] = cur_time.second;
	
	send_data [30]=0x81;                                 //IMEI��Ϣ������
	send_data [31]=0;
	send_data [32]=0x0f;
	get_imei(&send_data[33]);                            //SIMģ��IMEI
	
		if(getTermianlState()==1) 						//����״̬������� IMEI
		{		
			for(u8 i=0;i<15;i++)
			{
				send_data[33+i]='8';
			}
		}		
		
	send_data[22] = 0;                                   // 6+1+2+15=24
	send_data[23] = 0x18;
	
	send_data_size = 49;                                 //���������ܸ���:22+2+16+1 ;
	send_data[48] = EvalBCC_FromBytes(&send_data[2], 46);


int res=SocketWrite(1,send_data_size);
						if(res==0)//�˿ڴ򿪳ɹ�
						{
									GB17691_send_data(send_data,send_data_size);                         //ʵʱ�ϱ�									
						}
						else//�˿ڴ�ʧ�ܡ����´�socket
						{
//							int re=OpenSocket(1);
						}

    old_time_stamp = get_time_stamp();
}

u16 alarm_num=0;
void disman_alarm_send(void)  //�������
{
    rtc_time_t cur_time;
    uint8_t send_data[50];
    uint8_t send_data_size;

    send_data[0] = (uint8_t)(START_STRING >> 8);         //��ʼ��
    send_data[1] = (uint8_t)START_STRING;
    send_data[2] = 06;                   //�������

    GB17691_get_VIN_Info(&send_data[3]);
	
    send_data[20] = TERMINAL_SOFT_VER;                   //����汾��
    send_data[21] = NO_ENCRYPT;                          //���ݼ��ܷ�ʽ, ������

    send_data[22] = 0;                                   // 17
    send_data[23] = 17;
	
    get_current_time(&cur_time);              //���ݲɼ�ʱ��
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
    memcpy(&send_data[33] ,&location.longitude,4);                                            //����
		memcpy(&send_data[37],&location.latitude,4);                                             //γ��
		
    send_data_size = 42;                                 //���������ܸ���:22+2+16+1 ;
    send_data[41] = EvalBCC_FromBytes(&send_data[2], 39);


int res=SocketWrite(1,send_data_size);
						if(res==0)//�˿ڴ򿪳ɹ�
						{
									GB17691_send_data(send_data,send_data_size);                         //ʵʱ�ϱ�									
						}
						else//�˿ڴ�ʧ�ܡ����´�socket
						{
//							int re=OpenSocket(1);
						}

    old_time_stamp = get_time_stamp();
}


void login_send_new(void)   //����
{
    rtc_time_t cur_time;
    uint8_t send_data[120];
    uint8_t send_data_size;

    send_data[0] = (uint8_t)(START_STRING >> 8);  //��ʼ��
    send_data[1] = (uint8_t)START_STRING;
    send_data[2] = ZHENGZHOU_LOGIN_CMD;             //��¼����

    GB17691_get_VIN_Info(&send_data[3]);
    //send_data[20]= GB17691_get_softver_Info();
    //memcpy(&send_data[3],vin_info,sizeof(vin_info));  //VIN

    send_data[20] = TERMINAL_SOFT_VER;        //����汾��
    send_data[21] = NO_ENCRYPT;               //���ݼ��ܷ�ʽ, ������

    get_current_time(&cur_time);              //���ݲɼ�ʱ��
    send_data[24] = cur_time.year;
    send_data[25] = cur_time.month;
    send_data[26] = cur_time.date;
    send_data[27] = cur_time.hour;
    send_data[28] = cur_time.minute;
    send_data[29] = cur_time.second;


    lognum = get_lognum(cur_time);           //��¼��ˮ��
    lognum++;
    send_data[30] = (uint8_t)(lognum >> 8);
    send_data[31] = (uint8_t)lognum;		
		set_lognum(cur_time, lognum);
		
    get_iccid(&send_data[32]);              //SIM��ICCID��	
    GB17691_get_VIN_Info(&send_data[52]);		
  	get_imei(&send_data[69]);                //SIMģ��IMEI
		
		send_data[84]=0;
		send_data[85]=0;
		
		send_data[86]=500>>8;
		send_data[87]=500;

		send_data[88]=0x01;
		send_data[89]=0x00;
		send_data[90]=0x00;

    send_data[22] = 0;                      //ʱ��6��������ˮ��2��ICCID��20��IMEI��Ϣ��1  ���� 2+ imei 15  �� 46�ֽ�
    send_data[23] = 67;

    send_data_size = 92;                   //���������ܸ���:22+2+46+1 ;
    send_data[send_data_size-1] = EvalBCC_FromBytes(&send_data[2], send_data_size-3);  //У��λ

		int res=SocketWrite(1,send_data_size);
						if(res==0)//�˿ڴ򿪳ɹ�
						{
								GB17691_send_data(send_data,send_data_size);                         //ʵʱ�ϱ�		
#ifdef debug
	gb17691_print_data(send_data,send_data_size);			
#endif	
							
						}
						else//�˿ڴ�ʧ�ܡ����´�socket
						{
//							int re=OpenSocket(1);
						}

}

void logout_send(void)  //�ǳ�
{
    rtc_time_t cur_time;
    uint8_t send_data[60];

    send_data[0] = (uint8_t)(START_STRING >> 8);
    send_data[1] = (uint8_t)START_STRING;
    send_data[2] = VEHICLE_LOGOUT_CMD;

    GB17691_get_VIN_Info(&send_data[3]);
    //send_data[20]= GB17691_get_softver_Info();

    //memcpy(&send_data[3],vin_info,sizeof(vin_info));

    send_data[20] = TERMINAL_SOFT_VER;        //����汾��
    send_data[21] = NO_ENCRYPT;               //���ݼ��ܷ�ʽ, ������

    get_current_time(&cur_time);
    send_data[24] = cur_time.year;
    send_data[25] = cur_time.month;
    send_data[26] = cur_time.date;
    send_data[27] = cur_time.hour;
    send_data[28] = cur_time.minute;
    send_data[29] = cur_time.second;

    send_data [30]=0x81;                     //IMEI��Ϣ������
    send_data [31]=0;
    send_data [32]=0x0f;
    get_imei(&send_data[33]);                            //SIMģ��IMEI

    send_data[48] = (uint8_t)(lognum >> 8);  //�ǳ���ˮ��
    send_data[49] = (uint8_t)lognum;

    send_data[22] = 0x00;                    //�ǳ��������ݵ�Ԫ��ʱ�䣬�ǳ���ˮ�� IMEI     6+2+1+2+15 =26�ֽ�
    send_data[23] = 0x1a;

    send_data[50] = EvalBCC_FromBytes(&send_data[2], 48);

    //sim_uart_tx(send_data,33);
//    GB17691_send_data(send_data,51);
		int res=SocketWrite(1,51);
						if(res==0)//�˿ڴ򿪳ɹ�
						{
									GB17691_send_data(send_data,51);                         //ʵʱ�ϱ�									
						}
						else//�˿ڴ�ʧ�ܡ����´�socket
						{
//							int re=OpenSocket(1);
						}

}

void BOOL_data_put_buf(uint8_t handle)   //��� �洢  �ϱ�
{
	uint16_t data_len = 0;
//	uint16_t temp_len=0;
//	uint8_t temp_buf[36]={0};
	save_resend_data_t save_data_status;
	
//	if(is_set_time())
	{
		GB17691_data_package[0] = (uint8_t)(START_STRING >> 8);                             // ��ʼ�� 0x2323
		GB17691_data_package[1] = (uint8_t)(START_STRING);
	
		//memcpy(&GB17691_data_package[3],vin_info,sizeof(vin_info));                         //VIN
		GB17691_get_VIN_Info(&GB17691_data_package[3]);
		
		//GB17691_data_package[20]= GB17691_get_softver_Info();
		

	  GB17691_data_package[20] = TERMINAL_SOFT_VER;                                    //����汾��
	//	GB17691_data_package[20] =temp_soft_ver;                                           //����汾��
		GB17691_data_package[21] = NO_ENCRYPT;                                             //���ݼ��ܷ�ʽ 0x01
	
		rtc_time_t cur_time;
		get_current_time(&cur_time);                                                        // ��ȡRTC ��ǰʱ��
		save_data_status.timestamp = get_time_stamp();                                      // ʱ�����
		
		memcpy(&GB17691_data_package[24+data_len],&cur_time,sizeof(rtc_time_t));            //���ϵ�ǰʱ��
		data_len += sizeof(rtc_time_t);
		
		GB17691_data_package[24+data_len]= (uint8_t)(INFO_STREAM_NO>>8);                    //��Ϣ��ˮ��
		GB17691_data_package[25+data_len]= (uint8_t)(INFO_STREAM_NO);
		data_len +=2;
		
		GB17691_data_package[24+data_len] = GB17691_INFO_IMEI_DATA;                      // ��Ϣ���ͱ�־:IMEI��Ϣ��  ,0X81
		data_len += 1;
		GB17691_data_package[24+data_len]=0;                                            //IMEI���� 2�ֽڱ�ʾ 
		GB17691_data_package[25+data_len]=0x0f;
		data_len +=2;
		get_imei(&GB17691_data_package[24+data_len]);                          
		data_len +=15;
		
		GB17691_data_package[24+data_len] = GB17691_INFO_OBD_DATA;                         // ��Ϣ���ͱ�־:OBD��Ϣ ,0X01
		data_len += 1;

    data_len += GB17691_get_OBD_Info(&GB17691_data_package[24+data_len]);           //OBD��Ϣ
		
		GB17691_data_package[24+data_len] = GB17691_INFO_DATASTREAM_DATA;                               //��Ϣ���ͱ�־:��������Ϣ ,0X02
		data_len += 1;
		
		data_len += GB17691_get_BOOLdatastream_Info(&GB17691_data_package[24+data_len]);                                                  //��ȡ��������Ϣ

		GB17691_data_package[24+data_len] = GB17691_INFO_CUSTOM_DATA;                         // ��Ϣ���ͱ�־:�Զ�����Ϣ  ,0X80
		data_len += 1;
		data_len += GB17691_get_BOOLcustom_Info(&GB17691_data_package[24+data_len]);                                                  //��ȡ�Զ�����Ϣ��
			
		GB17691_data_package[22] = (uint8_t)(data_len >> 8);                   // 6+ 2 + 1+2+15 + 1+96 +1+37 +1+2+8
		GB17691_data_package[23] = (uint8_t)data_len;
			
			if(handle & REAL_TIME_UPLOAD)                                                         //ʵʱ�ϱ�
			{
				GB17691_data_package[2] = VEHICLE_UPLOAD_CMD;
				GB17691_data_package[24+data_len] = EvalBCC_FromBytes(&GB17691_data_package[2], data_len+22);//���У����
				
				int res=SocketWrite(1,24+data_len+1);
						if(res==0)
						{
							GB17691_send_data(GB17691_data_package, 24+data_len+1);   
	
						}
						else
						{
//							int re=OpenSocket(1);
						}
						
//				GB17691_send_data(GB17691_data_package, 24+data_len+1);                           //ʵʱ�ϱ�
				INFO_STREAM_NO++;                                                                 //���Ż�:�м��豸�ϵ�,�������洢����, ����Ϊ��λ,��1��ʼ�ۼ�  
				if(INFO_STREAM_NO>=65531){
					INFO_STREAM_NO=0;
				}
			}

			GB17691_data_package[2] = VEHICLE_RELOAD_CMD;
			GB17691_data_package[24+data_len] = EvalBCC_FromBytes(&GB17691_data_package[2], data_len+22);  //���У����
			data_len += 1;
			
			if(handle & SAVE_RESEND_DATA)    //�洢����
			{
				if((handle & REAL_TIME_UPLOAD) && (terminal_mode == TERMINAL_NET))
				{
					save_data_status.resend_flag = SEND_DATA_FLAG;
					realtime_upload_flag_addr[realtime_load_cnt] = realtime_data_wr_flash_addr + 24 + data_len + (sizeof(save_resend_data_t) - 2); //��������ֽ��Ǳ�־λ
					if(realtime_upload_flag_addr[realtime_load_cnt] >= RESEND_DATA_END_ADDR)
					{
						realtime_upload_flag_addr[realtime_load_cnt] -= RESEND_DATA_END_ADDR;
						realtime_upload_flag_addr[realtime_load_cnt] += RESEND_DATA_START_ADDR;
					}
					realtime_load_cnt++;
					if(realtime_load_cnt >= (sizeof(realtime_upload_flag_addr) / sizeof(uint32_t))) //��ֹ���
					{
						realtime_load_cnt = 0;
					}
				}
				else
				{
					save_data_status.resend_flag = RESEND_DATA_FLAG;
				}
							
				memcpy(&GB17691_data_package[24+data_len],&save_data_status,sizeof(save_resend_data_t));   // ʵʱ�ϱ����� + ʱ���+ ������+ ������+ ��״̬��� 
			  write_data_to_flash(GB17691_data_package,realtime_data_wr_flash_addr,24+data_len+sizeof(save_resend_data_t),RESEND_DATA_FLASH_AREA);    // ����д��FLASH
				
				realtime_data_wr_flash_addr += (24+data_len+sizeof(save_resend_data_t));
				if(realtime_data_wr_flash_addr >= RESEND_DATA_END_ADDR)
				{
					realtime_data_wr_flash_addr -= RESEND_DATA_END_ADDR;
					realtime_data_wr_flash_addr += RESEND_DATA_START_ADDR;
				}
//				
//				/***�洢ÿ����2M��¼һ��ʵʱ���ݶ�дFLASH��ַ ****************************/
////				resend_wrtie_size += (24+data_len+sizeof(save_resend_data_t));
////				if(resend_wrtie_size >= RESEND_WRITE_MAX_SIZE) //2M
////				{
////					//save_resend_record_addr(&resend_record_addr, realtime_data_wr_flash_addr);  //���ó�OBD��Ŀ���洢�������ݵ�ַ
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



// �ն�Ӧ�� ��������ظ�
void GB17691_ack_send( u8 con_ID ,u16 ctlidNo , u8 con_state , u8 ack_info)  //�ǳ�
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

    send_data[20] = TERMINAL_SOFT_VER;        //����汾��
    send_data[21] = NO_ENCRYPT;               //���ݼ��ܷ�ʽ, ������

//    get_current_time(&cur_time);
//    send_data[24] = cur_time.year;
//    send_data[25] = cur_time.month;
//    send_data[26] = cur_time.date;
//    send_data[27] = cur_time.hour;
//    send_data[28] = cur_time.minute;
//    send_data[29] = cur_time.second;

//				memcpy(send_data+24,(void *)flag_restart_net_time,6);
			memcpy(send_data+24,(void *)flag_answer_net_time,6);
//    send_data [30]=0x81;                     //IMEI��Ϣ������
//    send_data [31]=0;
//    send_data [32]=0x0f;
//    get_imei(&send_data[33]);                            //SIMģ��IMEI

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
		send_data[24+data_len] = GB17691_INFO_OBD_DATA;                         // ��Ϣ���ͱ�־:OBD��Ϣ ,0X01
		data_len += 1;

    data_len += GB17691_get_OBD_Info(&send_data[24+data_len]);           //OBD��Ϣ
		
		send_data[24+data_len] = GB17691_INFO_DATASTREAM_DATA;                               //��Ϣ���ͱ�־:��������Ϣ ,0X02
		data_len += 1;
		
		data_len += GB17691_get_BOOLdatastream_Info(&send_data[24+data_len]);                                                  //��ȡ��������Ϣ

		send_data[24+data_len] = GB17691_INFO_CUSTOM_DATA;                         // ��Ϣ���ͱ�־:�Զ�����Ϣ  ,0X80
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
    send_data[22] = 0x00;                    //�ǳ��������ݵ�Ԫ��ʱ�䣬�ǳ���ˮ�� IMEI     6+2+1+2+15 =26�ֽ�
    send_data[23] = 11;
}
	
    send_data[24+data_len] = EvalBCC_FromBytes(&send_data[2], 24+data_len-2);

		int res=SocketWrite(1,24+data_len+1);
						if(res==0)//�˿ڴ򿪳ɹ�
						{
									GB17691_send_data(send_data,24+data_len+1);                         //ʵʱ�ϱ�	
#ifdef debug
							     gb17691_print_data(send_data,24+data_len+1);		
#endif							
						}
						else//�˿ڴ�ʧ�ܡ����´�socket
						{
//							int re=OpenSocket(1);
						}

}
/************************************************************************
  * @����:   ���հ�����G4OBD���ݴ�� �洢  �ϱ�
  * @����:   None
  * @����ֵ: None
  **********************************************************************/
void GB17691_data_put_buf(uint8_t handle)   //��� �洢  �ϱ�
{
	      int cont=0;
        uint16_t data_len = 0;
		    unsigned int crcre=0xffff;

        save_resend_data_t save_data_status;
				GB17691_data_package[0] = '$';
				GB17691_data_package[1] = '$';
	    if (Actblue_State!=1)//ע��֡
			 {
					GB17691_data_package[2] = '0';			 
					data_len=get_AIKELAN_register(&GB17691_data_package[3]);//���ݳ���	
				 
					crcre=CRC16_Checkout(&GB17691_data_package[3],data_len);			 
					get_AIKELAN_crcre(crcre,&GB17691_data_package[3]+data_len);//����У��4�ֽ�
				 
					GB17691_data_package[5+data_len]='#';//2+CRC2-1+β2
					GB17691_data_package[6+data_len]='#';
				 	GB17691_data_package[7+data_len]='\r';//2+CRC2-1+β2
					GB17691_data_package[8+data_len]='\n';
			 }
			 else  //����֡
			{
					GB17691_data_package[2] = '1';
					data_len=get_AIKELAN_datapack(&GB17691_data_package[3]);
				
					crcre=CRC16_Checkout(&GB17691_data_package[3],data_len);
					get_AIKELAN_crcre(crcre,&GB17691_data_package[3+data_len]);	//У�鰴��4�ֽڼ���	
					
					GB17691_data_package[5+data_len]='#';//
					GB17691_data_package[6+data_len]='#';//2+2+2
					GB17691_data_package[7+data_len]='\r';//2+CRC2-1+β2
					GB17691_data_package[8+data_len]='\n';
			}

			GB17691_send_data(GB17691_data_package, 9+data_len); 
			
				//----------------------------------------------------------------------
				
				
        if(handle & REAL_TIME_UPLOAD)                                                         //ʵʱ�ϱ�
        {
						int res=SocketWrite(0,7+data_len);//����ע�������֡
						if(res==0)//�˿ڴ򿪳ɹ�
						{
									if(Actblue_State!=1)//ע��֡
									{
												do
												{
													GB17691_send_data(GB17691_data_package, 6+data_len);
													cont ++;
												}
												 while(cont<4);
													
									}
									else//����֡
									{
										GB17691_send_data(GB17691_data_package, 6+data_len);                           //ʵʱ�ϱ�
										
									}								
						}
						else//�˿ڴ�ʧ�ܡ����´�socket
						{
//							int re=OpenSocket(0);
						}
						
            INFO_STREAM_NO++;                                                                 //���Ż�:�м��豸�ϵ�,�������洢����, ����Ϊ��λ,��1��ʼ�ۼ�
            if(INFO_STREAM_NO>=65531)
            {
                INFO_STREAM_NO=0;
            }

        }

//        GB17691_data_package[2] = VEHICLE_RELOAD_CMD;
//        GB17691_data_package[24+data_len] = EvalBCC_FromBytes(&GB17691_data_package[2], data_len+22);  //���У����
//        data_len += 1;

        if(handle & SAVE_RESEND_DATA)    //�洢����
        {
            if((handle & REAL_TIME_UPLOAD) && (terminal_mode == TERMINAL_NET))
            {
                save_data_status.resend_flag = REALTIME_UPLOAD_FLAG;
                realtime_upload_flag_addr[realtime_load_cnt] = realtime_data_wr_flash_addr + 24 + data_len + (sizeof(save_resend_data_t) - 2); //��������ֽ��Ǳ�־λ
                if(realtime_upload_flag_addr[realtime_load_cnt] >= RESEND_DATA_END_ADDR)
                {
                    realtime_upload_flag_addr[realtime_load_cnt] -= RESEND_DATA_END_ADDR;
                    realtime_upload_flag_addr[realtime_load_cnt] += RESEND_DATA_START_ADDR;
                }
                realtime_load_cnt++;							
                if(realtime_load_cnt >= (sizeof(realtime_upload_flag_addr) / sizeof(uint32_t))) //��ֹ���
                {
                    realtime_load_cnt = 0;
                }
            }
            else
            {
                save_data_status.resend_flag = RESEND_DATA_FLAG;
            }

            memcpy(&GB17691_data_package[24+data_len],&save_data_status,sizeof(save_resend_data_t));   // ʵʱ�ϱ����� + ʱ���+ ������+ ������+ ��״̬���
            write_data_to_flash(GB17691_data_package,realtime_data_wr_flash_addr,24+data_len+sizeof(save_resend_data_t),RESEND_DATA_FLASH_AREA);    // ����д��FLASH

            realtime_data_wr_flash_addr += (24+data_len+sizeof(save_resend_data_t));
            if(realtime_data_wr_flash_addr >= RESEND_DATA_END_ADDR)
            {
                realtime_data_wr_flash_addr -= RESEND_DATA_END_ADDR;
                realtime_data_wr_flash_addr += RESEND_DATA_START_ADDR;
            }

            /***�洢ÿ����2M��¼һ��ʵʱ���ݶ�дFLASH��ַ ****************************/
//				resend_wrtie_size += (24+data_len+sizeof(save_resend_data_t));
//				if(resend_wrtie_size >= RESEND_WRITE_MAX_SIZE) //2M
//				{
//					//save_resend_record_addr(&resend_record_addr, realtime_data_wr_flash_addr);  //���ó�OBD��Ŀ���洢�������ݵ�ַ
//					resend_wrtie_size = 0;
//				}
				
					} //**  

//    }
}//**

/************************************************************************
  * @����:   ����G4OBD���ݴ�� �洢  �ϱ�
  * @����:   None
  * @����ֵ: None
  **********************************************************************/
/*
void GB17691_data_put_buf(uint8_t handle)   //��� �洢  �ϱ�
{
    uint16_t data_len = 0;
//		u8 *ps=NULL;
		unsigned int crcre=0xffff;
//	uint16_t temp_len=0;
//	uint8_t temp_buf[36]={0};
    save_resend_data_t save_data_status;

    // if(is_set_time())
    {
        GB17691_data_package[0] = '#';                             // ��ʼ�� 0x2323
        GB17691_data_package[1] = '#';

			data_len=get_xinxiang_datapack(&GB17691_data_package[6]);
		  get_xinxiang_datalen(data_len,&GB17691_data_package[2]);	
			
		  crcre=CRC16_Checkout(&GB17691_data_package[6],data_len);
			get_xinxiang_crcre(crcre,&GB17691_data_package[6+data_len]);
			
			GB17691_data_package[10+data_len]=0x0D;
			GB17691_data_package[11+data_len]=0x0A;			
			GB17691_send_data(GB17691_data_package, 12+data_len); 
			
			
			
			
				//----------------------------------------------------------------------
				
				
        if(handle & REAL_TIME_UPLOAD)                                                         //ʵʱ�ϱ�
        {
						int res=SocketWrite(0,12+data_len);
						if(res==0)//�˿ڴ򿪳ɹ�
						{
									GB17691_send_data(GB17691_data_package, 12+data_len);                           //ʵʱ�ϱ�									
						}
						else//�˿ڴ�ʧ�ܡ����´�socket
						{
//							int re=OpenSocket(0);
						}
						
            INFO_STREAM_NO++;                                                                 //���Ż�:�м��豸�ϵ�,�������洢����, ����Ϊ��λ,��1��ʼ�ۼ�
            if(INFO_STREAM_NO>=65531)
            {
                INFO_STREAM_NO=0;
            }

        }



        GB17691_data_package[2] = VEHICLE_RELOAD_CMD;
        GB17691_data_package[24+data_len] = EvalBCC_FromBytes(&GB17691_data_package[2], data_len+22);  //���У����
        data_len += 1;

        if(handle & SAVE_RESEND_DATA)    //�洢����
        {
            if((handle & REAL_TIME_UPLOAD) && (terminal_mode == TERMINAL_NET))
            {
                save_data_status.resend_flag = REALTIME_UPLOAD_FLAG;
                realtime_upload_flag_addr[realtime_load_cnt] = realtime_data_wr_flash_addr + 24 + data_len + (sizeof(save_resend_data_t) - 2); //��������ֽ��Ǳ�־λ
                if(realtime_upload_flag_addr[realtime_load_cnt] >= RESEND_DATA_END_ADDR)
                {
                    realtime_upload_flag_addr[realtime_load_cnt] -= RESEND_DATA_END_ADDR;
                    realtime_upload_flag_addr[realtime_load_cnt] += RESEND_DATA_START_ADDR;
                }
                realtime_load_cnt++;							
                if(realtime_load_cnt >= (sizeof(realtime_upload_flag_addr) / sizeof(uint32_t))) //��ֹ���
                {
                    realtime_load_cnt = 0;
                }
            }
            else
            {
                save_data_status.resend_flag = RESEND_DATA_FLAG;
            }

            memcpy(&GB17691_data_package[24+data_len],&save_data_status,sizeof(save_resend_data_t));   // ʵʱ�ϱ����� + ʱ���+ ������+ ������+ ��״̬���
            write_data_to_flash(GB17691_data_package,realtime_data_wr_flash_addr,24+data_len+sizeof(save_resend_data_t),RESEND_DATA_FLASH_AREA);    // ����д��FLASH

            realtime_data_wr_flash_addr += (24+data_len+sizeof(save_resend_data_t));
            if(realtime_data_wr_flash_addr >= RESEND_DATA_END_ADDR)
            {
                realtime_data_wr_flash_addr -= RESEND_DATA_END_ADDR;
                realtime_data_wr_flash_addr += RESEND_DATA_START_ADDR;
            }

            /***�洢ÿ����2M��¼һ��ʵʱ���ݶ�дFLASH��ַ ****************************/
//				resend_wrtie_size += (24+data_len+sizeof(save_resend_data_t));
//				if(resend_wrtie_size >= RESEND_WRITE_MAX_SIZE) //2M
//				{
//					//save_resend_record_addr(&resend_record_addr, realtime_data_wr_flash_addr);  //���ó�OBD��Ŀ���洢�������ݵ�ַ
//					resend_wrtie_size = 0;
//				}
				
//					}   

//    }
//}

/******************************************************************************************
���ܣ������ȫоƬID
���ߣ��ź��� 19.09.26
������void
����ֵ��void  
*******************************************************************************************/



/******************************************************************************************
���ܣ���֣��ƽ̨���ͱ�����Ϣ
���ߣ��ź��� 19.09.26
������void
����ֵ��void  
*******************************************************************************************/
//static void record_send(void)
//{
//		u8 *ps=NULL;
//	
//    rtc_time_t cur_time;
//    uint8_t send_data[280];
//    uint16_t send_data_size;

//    send_data[0] = (uint8_t)(START_STRING >> 8);  //��ʼ��
//    send_data[1] = (uint8_t)START_STRING;
//    send_data[2] = ZHENGZHOU_RECORD_CMD;             //��������

//    GB17691_get_VIN_Info(&send_data[3]);

//    send_data[20] = TERMINAL_SOFT_VER;        //����汾��
//    send_data[21] = NO_ENCRYPT;               //���ݼ��ܷ�ʽ, ������

////	  send_data[22] = 0;                      //ʱ��6  + оƬID16 + ��Կ64 + VIN17 + ǩ��130=233
////    send_data[23] = 0xE9;

//		send_data_size=24;
//	
//    get_current_time(&cur_time);              //���ݲɼ�ʱ��
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
//		ps=ReadPublickey();														//��Կ
//		if(ps==NULL)
//			ps=ReadPublickey();														//��Կ	
//		
//		if(ps!=NULL)
//			lkt4305_state=1;
//		
//		memcpy(&send_data[46],ps,64);
//		send_data_size=send_data_size+64;

//		GB17691_get_VIN_Info(&send_data[110]);	
//		send_data_size=send_data_size+17;		// VIN
//		
//		ps=Signature(&send_data[30],97);					//ǩ��
//		memcpy(&data_sign,ps,64);

//		send_data[send_data_size++] =32;   // ǩ��Rֵ
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


//		send_data[send_data_size++] =32;   // ǩ��Sֵ
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
//	  send_data[22] = (send_data_size-24)>>8;                      //ʱ��6  + оƬID16 + ��Կ64 + VIN17 + ǩ��66=169
//    send_data[23] = (send_data_size-24);
//		
//		send_data[send_data_size] = EvalBCC_FromBytes(&send_data[2], send_data_size-2);  //У��λ
//		send_data_size++;


//		int res=SocketWrite(0,send_data_size);
//						if(res==0)//�˿ڴ򿪳ɹ�
//						{
//									GB17691_send_data(send_data,send_data_size);                         //ʵʱ�ϱ�									
//						}
//						else//�˿ڴ�ʧ�ܡ����´�socket
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
            mode = TERMINAL_NET;//  ��������ģʽ
        }
        else
        {
            mode = TERMINAL_FREE;//����ģʽ,
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
    if(terminal_mode != current_mode)     // ģʽ�иı�, �������������״̬��Ϊ����״̬,�� ��ʵʱ�ϱ����ݸ�Ϊ��Ҫ�����ϱ�������
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
			
			
			
						GB17691_data_put_buf(GB17691_data_handle);//�ϱ�֣��ƽ̨=---��������
						
			
			if(cnt>101){cnt=0;}
      if(terminal_mode == TERMINAL_NET)
      {
				GB17691_send_enable=1;   //֣�ݷ���ʹ��  Сʱ
				if(record_state==1)		// �ѱ��� ���ڲ���״̬
				{
						if(timing_state == 0)			//δУʱ
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
												USARTSendOut(USART1, "\r\n����Уʱ\r\n", strlen("\r\n����Уʱ\r\n"));
						#endif	
								}
							}
						}
					else   // ��Уʱ
					{
						if(login_state == 0)  //δ��¼
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
//							USARTSendOut(USART1, "\r\n���͵�¼\r\n", strlen("\r\n����Уʱ\r\n"));
//			#endif
//								}
//							}
								login_state=1;
						}
						else  	// ��Уʱ �ѵ�¼
						{
								GB17691_send_enable=1;   //֣�ݷ���ʹ��  Сʱ
							  GB17691_data_put_buf(GB17691_data_handle);//�ϱ�֣��ƽ̨-----��������
								if(send_heart_flag>3)
								{

									if(get_vin(vin_info))   	//��ȡVIN
									{
										flag_bool_login = 0 ;  //VMSƽ̨ÿ���������綼��Ҫ ��0.�ط���½��
										reset_sim_net();						//120��ƽ̨û�лظ� ��������
									}	
									send_heart_flag=0;
//									record_state=0;			// ����ʧ��
//									set_recordstate(0);									//���汸����Ϣ
									
								}		
		//					GB17691_send_enable=1;   //֣�ݷ���ʹ��  Сʱ
						}
					}
				}
				else		//  δ����
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
//												USARTSendOut(USART1, "\r\n���ͱ���\r\n", strlen("\r\n����Уʱ\r\n"));
//						#endif	
//								}
//							}
						record_state=1;
					
				}
				
		}
		else  // ��������״̬
		{
			if(big_heart_flag==1)						//������
			{
//			    if((new_time_stamp - big_heart_temp_time_stamp) >= 300) //���������ɹ��Ƿ�ʱ5����,�˴�����������,����,�������·� 
			    if((new_time_stamp - big_heart_time_stamp) >= 300) //���������ɹ��Ƿ�ʱ5����,�˴�����������,����,�������·�  
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
		//--------------------------������
			if(big_heart_flag==1)      
			{	  
			    if(login_state == 1)
					{
					  get_GB17691_gps_info(&cur_location);
						if(cur_location.status.is_valid==0)              // 0 : ��ʾ��Ч 1��ʾ��Ч
						{  
							   //GB17691_data_handle |= SAVE_RESEND_DATA;     // ��ʵʱ�ϱ����ݸ�ʽ����,��һ��GPS��Ϣ, �����洢
//					      if(GB17691_send_enable)
//				        {
						      GB17691_data_handle |= REAL_TIME_UPLOAD;
//					      }
							
							  if(GB17691_data_handle)
							  {
									if(GB17691_send_enable)
	//								GB17691_data_put_buf(GB17691_data_handle);//�ϱ�֣��ƽ̨
									
							  	BOOL_data_put_buf(GB17691_data_handle); //�������ϱ�������ƽ̨
							  }                                
							  big_heart_flag=0;
							  big_heart_time_stamp=new_time_stamp;
							  update_big_heart_time(big_heart_time_stamp); 
				    }

		//				else if((new_time_stamp - big_heart_temp_time_stamp) >= 300)						// û�ж�λ  ��ʱ����
				else if((new_time_stamp - big_heart_time_stamp) >= 300)						// û�ж�λ  ��ʱ����
							{   //20190312 add  for test 300->180
	
						      GB17691_data_handle |= REAL_TIME_UPLOAD;
							  if(GB17691_data_handle)
							  {
									if(GB17691_send_enable)
//									GB17691_data_put_buf(GB17691_data_handle);//�ϱ�֣��ƽ̨
									
							  	BOOL_data_put_buf(GB17691_data_handle); //�������ϱ�������ƽ̨
							  }  
								
								big_heart_flag=0;
								big_heart_time_stamp=new_time_stamp;
								update_big_heart_time(big_heart_time_stamp); 
						}
						
					}
					else
					{
		//				if((new_time_stamp - big_heart_temp_time_stamp) >= 300) 	//��ʱ
						if((new_time_stamp - big_heart_time_stamp) >= 300) 	//��ʱ
						 {
							 
									BOOL_data_put_buf(GB17691_data_handle); //�������ϱ�������ƽ�
									big_heart_flag=0;
#ifdef debug						
					        USARTSendOut(USART1, "send big heart over time1!\r\n", 28);
#endif
									big_heart_time_stamp=new_time_stamp;
								  update_big_heart_time(big_heart_time_stamp); 
							}
					}
			}
			//--------------------------------��ʱ�ϱ�---
//        else
			if (can_state==1)
        {
            if((new_time_stamp - realtime_data_stamp) >= MSG_SEND_INTERVAL)
            {
                if(can_state==1) //��ֹ��������־Ϊ0ʱ,�����ݽ��д洢
                {
                    realtime_data_stamp = new_time_stamp;
                    GB17691_data_handle |= SAVE_RESEND_DATA;
                    if(GB17691_send_enable)
                    {
                        GB17691_data_handle |= REAL_TIME_UPLOAD;
                    }

                    if(GB17691_data_handle)//2  ֻ�洢
                    {	
											if(realup_cnt%10==4)
											{
//												GB17691_data_put_buf(GB17691_data_handle);//��� �洢 �ϱ�	
//												send_heart_flag++;  //  ʵʱ=֣������ ���ͼ���
											}
																					
											if(terminal_mode == TERMINAL_NET)
												GB17691_data_handle |= REAL_TIME_UPLOAD;  // �ڶ���·�������ϱ�
									
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
											
                        big_heart_time_stamp=new_time_stamp;      //���´�����ʱ���
                        update_big_heart_time(big_heart_time_stamp);
                    }
                }
								realup_cnt++;
            }
						
						
						
						
						
						
        }
		}
//---------------------------�ǳ�------------------					
    else
    {
        if(((terminal_mode == TERMINAL_NET)) && login_state)
        {
            logout_send();
            login_state = 0;
            GB17691_send_enable = 0;

            for(uint8_t i=0; i<realtime_load_cnt; i++)  //���ѷ����ݱ��Ϊ���ͳɹ�
            {
                write_resend_data_flag(realtime_upload_flag_addr[i], SEND_DATA_FLAG);
            }
            realtime_load_cnt = 0;

            for(uint8_t i=0; i<resend_msg_cnt; i++)    //�����������ݱ��Ϊ���ͳɹ�
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

//					if(BKP_ReadBkpData(BKP_DR5) == 0x5a5a)     //��һ���ϵ� ������
//					{
//							big_heart_flag=1;
//							big_heart_temp_time_stamp = new_time_stamp;
//							BKP_WriteBackupRegister(BKP_DR5, 0xa5a5);
//					}

//					if(BKP_ReadBkpData(BKP_DR2) == 0xa5a5)
//					{
//							if((new_time_stamp - big_heart_time_stamp)>86400)  //ʱ����24Сʱ
//									//	if((new_time_stamp - big_heart_time_stamp)>600)  //ʱ����10����
//							{
//									big_heart_flag=1;
//									big_heart_temp_time_stamp = new_time_stamp;
//							}
//					}
        }
    }
}
/*
����:������ʼ�� VIN  У����, ������λ��, ����������Ϣ��ַ, �Լ�������ʼ��ַ����
����: uint32_t* addr ������ʼ��ַ  �����л����
      uint32_t* resend_info_addr  ����������Ϣ��ַ  �����л����
����: ���ݵ�ַ

*/
uint8_t* read_msg_from_flash(uint32_t* addr, uint32_t* resend_info_addr,uint8_t flash_area)
{
    uint16_t data_len = 0;
    uint16_t i = 0;
    uint32_t res_addr = *addr;
    uint8_t* data_pos = NULL;

    read_data_from_flash(GB17691_data_package,*addr,sizeof(GB17691_data_package),flash_area); //��addr�����ݷ�GB17691_data_package
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
                    if((cur_stamp - save_data_satus->timestamp) > (HEART_INTERVAL << 1))   //����100��ǰ����
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

                    //�ط���־ƫ��6���ֽ� ,����һ�����ݲ�����ʼ��ַ
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
																			//��Ǳ���״̬Ϊ���ͳɹ�
										resend_flash_addr_end_flag = 0;
										for(uint8_t i=0; i<realtime_load_cnt; i++)  //�յ���������,���ѷ����ݱ��Ϊ���ͳɹ�
										{
												write_resend_data_flag(realtime_upload_flag_addr[i], SEND_DATA_FLAG);
										}
										realtime_load_cnt = 0;

										for(uint8_t i=0; i<resend_msg_cnt; i++)    //�յ���������,�򽫲��������ݱ��Ϊ���ͳɹ�
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
//        ��ò�����ʼд��ַ
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
    �Ӵ洢�Ĳ����������ҳ�ʱ��������ǰ����ݵ�ַ, �Լ�ʵʱ���ݶ�д��ַ
    ******************************************************/
    uint32_t check_flash_size  = RESEND_DATA_START_ADDR;

    while(check_flash_size < RESEND_DATA_END_ADDR)
    {
        addr_temp = addr;
        msg_pos = read_msg_from_flash(&addr,&resend_flag_addr,RESEND_DATA_FLASH_AREA); // ��FLASH���� ����λ��, ������ʼ��ַ, ����������Ϣ��ַ,

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
    	�ж�����õ���ʵʱ���ݶ�дFLASH��ַ֮���FLSAH�Ƿ����д,
    	��������д,��������һ��secter, ������д�����д.
    	ע��:	FLASHд֮ǰ�����,��FF�ſ���д, дһ���µ�secterʱ���Ȳ�����������д!
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
                    realtime_data_wr_flash_addr = ((realtime_data_wr_flash_addr / FLASH_SECTOR_SIZE) + 1)*FLASH_SECTOR_SIZE;  //����ǿգ���ʼ����һ��secter��ʼ��
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
					GB17691_ack_send(CtlID[i],CtlIDNo[i],CON_SUCCESS,0);  //��λ�ɹ�
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
                BKP_WriteBkpData(BKP_DAT6, 0xa5a5);  //��һ��Уʱ�ɹ�
            }
            else
            {
                BKP_WriteBkpData(BKP_DAT6, 0x5a5a);  //��һ��Уʱ�ɹ�
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
		
		ps=ReadPublickey();														//��Կ
		if(ps==NULL)
			ps=ReadPublickey();														//��Կ	
		
		if(ps!=NULL)
			lkt4305_state=1;
	
}
static void check_big_heart(void)
{		
		uint32_t new_time_stamp;
    new_time_stamp = get_time_stamp();
	
		if(BKP_ReadBkpData(BKP_DAT5) == 0x5a5a)     //��һ���ϵ� ������
		{
//				big_heart_flag=1;
				BKP_WriteBkpData(BKP_DAT5, 0xa5a5);
		}
				if(BKP_ReadBkpData(BKP_DAT2) == 0xa5a5)
					{
//							if((new_time_stamp - big_heart_time_stamp)>86400)  //ʱ����24Сʱ
//	//							if((new_time_stamp - big_heart_time_stamp)>600)  //ʱ����10����
//							{
////USARTSendOut(USART1, "\r\n������11����������������\r\n", strlen("\r\n������11����������������\r\n"));
//									big_heart_flag=1;
//						//			big_heart_temp_time_stamp = new_time_stamp;
//									big_heart_time_stamp = new_time_stamp;
//							}
					}
}
void Get_SM2id(void)
{
		u8 IMEI_data[15];
		if(get_idstate(SM2ID)==1)				// ID�Ѵ���flash
		{
//			while(Set4305_Sm2id()!=1)			//����SM2 ID
//			{
//				if(j%5==4)
//					LKT4305Init();									//����5��ʧ�� ��λ��ȫоƬ
//				j++;
//				osDelay(30);
//			}
		}
		else						//ID δ����FLASH  �ȴ�IMEI��ȡ�ɹ�������ID����flash
		{
			while(!get_imei(&IMEI_data[0]))					//��ȡ��IMEI
			{	
				iwdg_17691++;	
				osDelay(100);
			}
			SM2ID[0]=ID_BOOL0;
			SM2ID[1]=ID_BOOL1;
			SM2ID[2]=ID_BOOL2;
			memcpy(&SM2ID[3],&IMEI_data[2],13);
			set_idstate(SM2ID);								// ��ID����FLASH				
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
					const char DF_KaiPuTe_VIN[8]  ="LGDCH91G";  //����-������
					const char JN_50ling_VIN[8]   ="LWLDAA5G";  //����-50��

					uint8_t  res , res_1, res_2,res_3,res_4,res_5;
					res   = memcmp(vin_info,RESET_VIN  ,sizeof(vin_info));
					res_1 = memcmp(vin_info,RESET_VIN_1,sizeof(vin_info));
					if((!res)||(!res_1))              //�ظ���������
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
					else   //����VIN �жϳ���
					{
						res   = memcmp(vin_info,DATONG_VIN_1,3);
						res_1  = memcmp(vin_info,DATONG_VIN_2,3);
            if((!res)||(!res_1))	 //�ж��ǲ��Ǵ�ͨ
						{							
							SetUdsProtocol(DaTong_9);	
							SetUdsPara(_DCUID,9);
							Set_evads(8,1);
							flag_UDSSEND=1;
						}
						else
						{							
							res   = memcmp(vin_info,WUSHILING_VIN_1,4);
							if(!res)  // �ж��ǲ�����ʮ��
								{
									SetUdsProtocol(wushiling_17);	
									SetUdsPara(_DCUID,17);
									Set_evads(8,1);
									flag_UDSSEND=1;
								}
								else
								{
									res   = memcmp(vin_info,BENCHI_VIN_1,4);
									if(!res)  // �ж��ǲ��Ǳ���
										{
											SetUdsProtocol(benchiact_18);	
											SetUdsPara(_DCUID,18);
											Set_evads(8,1);
											flag_UDSSEND=1;
										}	
									else
										{
											res   = memcmp(vin_info,DFH4250_VIN,8);
											if(!res)  // �ж��ǲ��Ƕ���
												{
													SetUdsProtocol(DongFeng_2);	
													SetUdsPara(_DCUID,2);
													Set_evads(8,1);
													flag_UDSSEND=1;
												}	
											else
											{
											res   = memcmp(vin_info,JRLH_VIN,8);
											if(!res)  // �ж��ǲ��Ǽ�������
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
													if((!res)||(!res_1)||(!res_2)||(!res_3)||(!res_4)||(!res_5)) // �ж��ǲ��ǽ���
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
u32 randnum=0;        //α����� ����  �����ŷ�����ʹ��
u16 GPS_LED_cnt=0;			//  LED��ʾ V1.8��
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
		
		
		record_state=get_recordstate();	// ��ȡ����״̬
//		record_state=0;									
		//��ȡ��ȫоƬID
		Get_SM2id();
		
    vin_state=get_vin(vin_info);   	//��ȡVIN
	
		if(!vin_state)			//���û������VIN�������ͱ�����Ϣ
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
            if(gps_set_time() && (BKP_ReadBkpData(BKP_DAT6) == 0x5a5a))   //��һ��GPSУʱ�ɹ�
            {
								big_heart_time_stamp=get_time_stamp();
                update_big_heart_time(get_time_stamp());
            }

					if((randnum%500==88)&&(alarm_state_flag !=0 ))		//5s��һ��  alarm_state_flag !=0 ->ûдvin���ն˶������Ͳ������
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
								USARTSendOut(USART1, "\r\n���Ͳ������\r\n", strlen("\r\n���Ͳ������\r\n"));
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
    TERMINAL_SOFT_VER=GB17691_get_softver_Info();   // ��ȡ�豸���� ����汾��
    if(BKP_ReadBkpData(BKP_DAT2) == 0xa5a5)
    {
        big_heart_time_stamp= ((uint32_t)BKP_ReadBkpData(BKP_DAT3) <<16 )|((uint32_t)BKP_ReadBkpData(BKP_DAT4));
        test_flag=0x00;
    }
    else
    {
        big_heart_time_stamp=get_time_stamp();        //��һ���ϵ�
        update_big_heart_time(get_time_stamp());
        BKP_WriteBkpData(BKP_DAT5, 0x5a5a);
        test_flag=0x01;
    }
    tid_GB17691_task = osThreadCreate (osThread(GB17691_task), NULL);
}









