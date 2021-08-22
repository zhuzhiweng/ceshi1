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
//uint8_t vin_info[17] = {0x4C, 0x50, 0x48, 0x46, 0x41, 0x33, 0x42, 0x44, 0x30, 0x48, 0x59, 0x31, 0x37, 0x30, 0x32, 0x33, 0x35};    //vinĞÅÏ¢
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
//} CONTROL_ID ;          //¿ØÖÆÃüÁî

typedef enum
{
   NO_USE,
   SETUPDATA_FIRMWARE,//¡¾ÃüÁî²ÎÊı£ºhttpÉı¼¶£¬µÚÒ»¸ö×Ö½ÚÎªÃüÁî£¬µÚ¶ş¸ö×Ö½Ú¿ªÊ¼ÎªURL¡¿
   SETDO,//¡¾Ò»¸ö×Ö½Ú£¬SHUTDOWN1:1, RESTART:2,    RESET_DATA:3,    OFFLINE,:4,  Relog:5¡¿

   SETVIN,//¡¾ÃüÁî²ÎÊı£ºVIN¡¿
   SETUDS,//¡¾ÃüÁî²ÎÊı£ºµÚÒ»¸ö×Ö½Ú1Îª¿ª¡£0Îª¹Ø£¬µÚ¶ş¸ö×Ö½ÚÎªUDSºÅÂë£¬µÚÈı¸ö£¬Èı¸öµÚËÄ¡¿
   SETSEND,//¡¾Èı¸ö×Ö½Ú£¬1´ú±íÊ¹ÄÜ£¬0ÎªÊ§ÄÜ£¬OFF1939SEND£¬ON1939SEND£¬OFF15765SENG09£¬ON15765SENG09£¬OFF15765SEND01£¬ON15765SEND01£¬¡¿

GETVER,//¡¾*ÓÃ»§¿´*Ó¦´ğ£¬£¨µ¥×Ö½Ú°æ±¾ºÅ-Éè±¸ÀàĞÍ-Éè±¸Ãû-°æ±¾ºÅ£©¡¿
GETVIN,//¡¾Ó¦´ğÃüÁî²ÎÊı£ºVIN¡¿
GETUDS,//¡¾Ó¦´ğÃüÁî²ÎÊı£ºUDSĞÅÏ¢¡¿

GETSENDSTATE,//¡¾Ó¦´ğÃüÁî²ÎÊı£º²Î¿¼SETSEND¡¿
GETTEMSTATE,//¡¾Ó¦´ğÃüÁî²ÎÊı£º×´Ì¬ĞÅÏ¢¡¿

GETOBDDATA,//¡¾*ÓÃ»§¿´*Ó¦´ğÃüÁî²ÎÊı£ºcandata¡¿
GETCANDATA,//¡¾Ó¦´ğÃüÁî²ÎÊı£ºcandata¡¿
GETCANSTATE,//¡¾Ó¦´ğÃüÁî²ÎÊı£ºµÚÒ»¸ö×Ö½Ú6.14µÄĞ­ÒéÄÚÈİ£¬µÚ¶ş¸ö×Ö½Ú6.14µÄĞ­ÒéÄÚÈİ£¬
         //µÚÈı¸ö×Ö½Ú19µÄĞ­ÒéÄÚÈİ£¬µÚËÄ¸ö×Ö½Ú19µÄĞ­ÒéÄÚÈİ£¬
        //µÚÎå¸ö×Ö½Ú311µÄĞ­ÒéÄÚÈİ£¬µÚÁù¸ö×Ö½Ú311µÄĞ­ÒéÄÚÈİ£¬
        //0´ú±íÃ»ÓĞĞ­Òé£¬1´ú±í15765£¬2´ú±í1939¡¿
SETCLEARDTC,//¡¾Ó¦´ğÃüÁî²ÎÊı£ºµÚÒ»¸ö×Ö½Ú6.14µÄ1939Çå³ı¹ÊÕÏ£¬µÚ¶ş¸ö×Ö½Ú6.14ÉÏ15765Çå³ı¹ÊÕÏ£¬
        //µÚÈı¸ö×Ö½Ú19µÄ1939Çå³ı¹ÊÕÏ£¬µÚËÄ¸ö×Ö½Ú19µÄ15765Çå³ı¹ÊÕÏ£¬
       //µÚÎå¸ö×Ö½Ú311µÄ1939Çå³ı¹ÊÕÏ£¬µÚÁù¸ö×Ö½Ú311µÄ15765Çå³ı¹ÊÕÏÈİ£¬¡¿
} CONTROL_ID ;          //¿ØÖÆÃüÁî

typedef enum
{
    CON_SUCCESS=1,
    CON_FAILED,
} CONTROL_STATE	;          //¿ØÖÆÃüÁî


extern uint8_t comupdate_mode_flag;
extern uint8_t vin_info[17];
//static uint8_t read_vin_state = 0;            //¶ÁVIN×´Ì¬  0:²»¶ÁVIN×´Ì¬  1:¶ÁVIN×´Ì¬
static uint8_t login_state = 0;               //µÇÈë×´Ì¬   0:²»µÇÈë×´Ì¬   1:µÇÈë×´Ì¬
uint8_t timing_state = 0;              //Ğ£Ê±×´Ì¬   0:·ÇĞ£Ê±×´Ì¬   1:Ğ£Ê±×´Ì¬
static uint8_t record_state=0;					// ±¸°¸×´Ì¬£¬ 0Î´±¸°¸		 1ÒÑ±¸°¸
static uint8_t lkt4305_state=0;					// ±¸°¸×´Ì¬£¬ 0²»Õı³£		 1Õı³£
//static  uint8_t flag_nosm2=0;				//			0 Ç©ÃûÉÏ±¨   1²»´øÇ©ÃûÉÏ±¨
//static uint8_t alarm_state = 0;              //±¨¾¯×´Ì¬
static uint16_t lognum = 0;                   //µÇÈë´ÎÊı
static uint8_t send_data_cnt = 0;             //·¢Êı¼ÆÊı
uint8_t GB17691_CMD_data[256];             //ÃüÁî»º³å256
static uint16_t gb17691_rd_pos = 0;           //¶ÁÎ»ÖÃ
static uint16_t cmd_data_len;                 //ÃüÁîÊı¾İ³¤¶È
static uint8_t check_cmd_info_flag = 0;       //¼ì²éÃüÁîĞÅÏ¢±êÖ¾
static uint8_t CMD_check_dealy;               //ÃüÁî¼ì²éÑÓÊ±

//static uint32_t	pre_resend_timestamp;
//static uint8_t pre_resend_flag = 0;
static uint8_t GB17691_send_enable = 0;       //GB17691·¢ËÍÊ¹ÄÜ 0:²»Ê¹ÄÜ 1 :Ê¹ÄÜ

//static uint32_t resend_record_addr = RESEND_RECORD_START_ADDR; //±£´æ²¹·¢µØÖ·
//static uint32_t resend_wrtie_size = 0;                          //²¹·¢Ğ´´óĞ¡³õÊ¼Îª0

static uint32_t realtime_upload_flag_addr[HEART_INTERVAL];   //ÊµÊ±ÉÏ±¨±ê¼Ç´æ´¢»º³å
static uint8_t realtime_load_cnt = 0;                        //ÊµÊ±ÉÏ±¨¼ÆÊı
static uint32_t resend_msg_flag_addr[RESND_MSG_MAX_CNT];     //²¹·¢±ê¼Ç´æ´¢»º³å
static uint8_t resend_msg_cnt = 0;                           //²¹·¢¼ÆÊı

//static uint8_t usb_send_enable = 0;
static uint8_t terminal_mode = TERMINAL_FREE;  //0:ÎŞÁ¬½Ó£¬1£ºÍøÂç£¬2£ºUSB
static uint32_t old_time_stamp;                //¾ÉÊ±¼ä´Á
//static uint32_t alarm_data_stamp;              //±¨¾¯Êı¾İ´Á
static uint32_t realtime_data_stamp;           //ÊµÊ±Êı¾İ´Á
//static uint32_t heart_stamp;                   //ĞÄÌø´Á
//static uint32_t alarm_resend_stamp;            //±¨¾¯²¹·¢´Á
static uint8_t send_heart_flag = 0;            //ËÍĞÄÌø±ê¼Ç
static uint8_t resend_flash_addr_end_flag = 0;  //²éÕÒ²¹·¢FLASHµ½½áÊøµØÖ·,ÔòÖÃ1 , ÔÚ½ÓÊÕµ½ĞÄÌøÊ±ÖÃ0,
//static uint8_t key_state = 0;                  //Ô¿³××´Ì¬
//static uint8_t charge_state = 0;               //³äµç×´Ì¬

//static uint32_t current_alarm_flag = 0;        //µ±Ç°±¨¾¯±êÖ¾
//static uint8_t current_max_alarm_level = 0;    //µ±Ç°×î´ó±¨¾¯¼¶±ğ
//static uint8_t alarm_real_time_send_cnt;
//static uint8_t alarm_resend_cnt;                //±¨¾¯²¹·¢¼ÆÊı
//static uint8_t alarm_resend_over;               //±¨¾¯²¹·¢½áÊø

static uint32_t realtime_data_wr_flash_addr = RESEND_DATA_START_ADDR; //ÊµÊ±Êı¾İ´æ´¢µØÖ·, ³õÊ¼Îª²¹·¢Êı¾İ¿ªÊ¼µØÖ·
static uint32_t resend_data_rd_flash_addr = RESEND_DATA_START_ADDR;   //²¹·¢Êı¾İ¶ÁFLASHµØÖ·, ³õÊ¼Îª²¹·¢Êı¾İ¿ªÊ¼µØÖ·
//static uint32_t alarm_data_wr_flash_addr = ALARM_DATA_START_ADDR;     //±¨¾¯Êı¾İ´æ´¢µØÖ·, ³õÊ¼Îª±¨¾¯Êı¾İ¿ªÊ¼µØÖ·
//static uint32_t alarm_data_rd_flash_addr = ALARM_DATA_START_ADDR;     //±¨¾¯Êı¾İ¶ÁFLASHµØÖ·,³õÊ¼Îª±¨¾¯Êı¾İ¿ªÊ¼µØÖ·
static uint8_t ckeck_flash_flag = 0;              //¼ì²éFLASH±ê¼Ç

uint8_t GB17691_data_package[1024];               //GB17691Êı¾İ°ü»º´æ ,´óĞ¡512

uint8_t data_sign[64];       		//Ç©ÃûĞÅÏ¢      
uint8_t SM2ID[16];						//SM2IDĞÅÏ¢
uint8_t platformPublicKeyData[64];  //Æ½Ì¨¹«Ô¿


uint8_t big_heart_flag=0;                            //1:½øÈë´óĞÄÌø×´Ì¬, 0:²»½øÈë´óĞÄÌø    ÍøÂç¹ÜÀíÄ£¿é»áµ÷ÓÃ
uint32_t big_heart_time_stamp;                       //´óĞÄÌøÊ±¼ä´Á
//static uint32_t big_heart_temp_time_stamp;           //´óĞÄÌøÁÙÊ±Ê±¼ä´Á

static uint8_t TERMINAL_SOFT_VER=0;
uint16_t INFO_STREAM_NO=1;                         //ĞÅÏ¢Á÷Ë®ºÅ


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

extern u8 flag_set_vin;   	//ÉèÖÃVIN±êÖ¾
extern u8 flag_UDSSEND;
u8  Actblue_State=1;//Ö¡Ö¸Áî

//static uint32_t resend_data_stamp;
//
////#pragma pack(1)   //°´×Ö½Ú¶ÔÆë
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
////#pragma pack()  //È¡ÏûÖ¸¶¨¶ÔÆë£¬»Ö¸´È±Ê¡¶ÔÆë

//******°¬¿ÉÀ¶Êı¾İ×´Ì¬**************//
u8 ACTBLU_date_state=0;//°¬¿ÉÀ¶Êı¾İ×´Ì¬

#define upload_successful 1//ÉÏ´«³É¹¦
#define unregistered      2//Î´×¢²á
#define Check_failure     3//Êı¾İĞ£ÑéÊ§°Ü
#define unknown_failure   4//Î´ÖªÊ§°Ü
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
  * @ÃèÊö:   hex ×ªascii
  * @²ÎÊı:   None
  * @·µ»ØÖµ: None
  **********************************************************************/
u16 hex2ascii( u8 *mubiao ,u8 *yuan,u16 BytNo )								//°Ñ stm32 µÄ can¿Ú½ÓÊÕµÄ8¸ö×Ö½Ú×ª»»³É16¸ö×Ö½ÚµÄascII ĞÎÊ½
{
	u16 j=0; u8 temp_data=0; 
	for(u16 i=0;i<BytNo;i++)
	{
		temp_data=(*(yuan+i))>>4;
		if(temp_data<=9)            //±ä³ÉasciiÊı×Ö
		{
			*(mubiao+j) = temp_data + 0x30;
		}
		else 																					//±ä³Éascii´óĞ´×ÖÄ¸
		{
			*(mubiao+j)= temp_data +	0x37;
		}
		j++;

		temp_data=(*(yuan+i))&0x0f;	
		if(temp_data<=9)            //±ä³ÉasciiÊı×Ö
		{
			*(mubiao+j) = temp_data + 0x30;
		}
		else 																					//±ä³Éascii´óĞ´×ÖÄ¸
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
        res = sim_uart_tx(pdata, data_len);  //ĞèÒªÅĞ¶Ï´®¿ÚBUFÊÇ·ñÂú£¿
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
					send_heart_flag=0;    // ĞÄÌø¼ÆÊıÇå0  ÍøÂçÖØÆô¼ÆÊı
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
				if(pdata[33]==1)	//HttpÉı¼¶
				{
					u16	url_len = 0 ;
					char url_head[] ="\r\nAT+HTTPPARA=\"URL\",\"";
					url_len = ((uint16_t)pdata[33] << 8) + pdata[34] ;		//Õâ¸öµØ·½ĞèÒª¸ú¼°ÏşÁÖËµÃ÷
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
				else if(pdata[33]==2)//FTPÉı¼¶
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
								memcpy((void *)flag_answer_net_time,pdata+24,6);  //Ê±¼äÖ»ÄÜ±£´æÒ»¸ö£¬·şÎñÆ÷ÓÃCtlIDNoÇø·Ö-210304-msc
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
											//ĞèÒªĞ´flash£¬´ı¿ª·¢
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
											//ĞèÒªĞ´flash£¬´ı¿ª·¢
										}
										else if(pdata[33]==5)//Relog
										{
											flag_relog_net=1;
										}
								}
								else if(pdata[32]==SETVIN) //ÉèÖÃVIN
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
										//ĞèÒªĞ´flash£¬´ı¿ª·¢
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
        if((new_time_stamp - old_time_stamp) <= 5)  //5->30 forÍøÂçÖúÊÖµ÷ÊÔ
        {
            rtc_time_t server_time;
            rtc_time_t set_time;
            server_time.year = pdata[24];
            server_time.month = pdata[25];
            server_time.date = pdata[26];
            server_time.hour = pdata[27];
            server_time.minute = pdata[28];
            server_time.second= pdata[29];
            set_time_preset((new_time_stamp - old_time_stamp + TIME_PRESET_VALUE),server_time,&set_time);  //»ñµÃÉèÖÃÊ±¼äset_time
            if(set_current_time(set_time) == SUCCESS)
            {
                timing_state = 1;
				#ifdef debug						
										USARTSendOut(USART1, "\r\n-------¡·Ğ£Ê±³É¹¦¡¶-------\r\n", strlen("\r\n-------¡·Ğ£Ê±³É¹¦¡¶-------\r\n"));
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
		case 0x82:	//VMSÓ¦´ğ³É¹¦
		case 0x92:  //VMSÓ¦´ğÊ§°Ü
    {
        uint32_t new_time_stamp = get_time_stamp();
				send_heart_flag =0;
        break;
    }
		case 0x81: //vms µÇÈë³É¹¦
		case 0x91: //vms µÇÈë³É¹¦   
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
	 if(GB17691_CMD_data[0] == (uint8_t)(START_STRING >> 8))//ÅĞ¶ÏÆğÊ¼·û ºÍ VIN
                {
                    if(GB17691_CMD_data[1] == (uint8_t)(START_STRING))
                    {
												cmd_data_len = (uint16_t)(GB17691_CMD_data[22] << 8) + (uint16_t)GB17691_CMD_data[23];
                        cmd_data_len += 25;
                        //Êı¾İµ¥Ôª³¤¶È£¬¼ÓÉÏ24×Ö½ÚµÄĞÅÏ¢£¬1¸ö×Ö½ÚµÄĞ£Ñé£¬
                        if(cmd_data_len <= sizeof(GB17691_CMD_data))  //ÅĞ¶ÏÃüÁîÊÇ·ñ³¬³ö·¶Î§, gb17691_rd_pos++ ,ÔÙ½øÈëÑ­»·
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
#define upload_successful 1//ÉÏ´«³É¹¦
#define unregistered      2//Î´×¢²á
#define Check_failure     3//Êı¾İĞ£ÑéÊ§°Ü
#define unknown_failure   4//Î´ÖªÊ§°Ü
*/
void GB17691_AIKELAN_CheckCmd(void)//°²»Õ°¬¿ÉÀ¶
{
 //  get_sim_data_multiple_bytes(GB17691_CMD_data, gb17691_rd_pos, len);
	   get_sim_data_multiple_bytes(GB17691_CMD_data, gb17691_rd_pos, 35);
	      if(GB17691_CMD_data[0] == (uint8_t)(ACTBLU_START_STRING >> 8))//ÅĞ¶ÏÆğÊ¼·û ºÍĞ£ÑéĞÅÏ¢
                {
                    if(GB17691_CMD_data[1] == (uint8_t)(ACTBLU_START_STRING))
                    {
												if(GB17691_CMD_data[2] ==0)//×¢²áÖ¡
												{
																cmd_data_len=gb17691_rd_pos;	//Êı¾İµ¥Ôª³¤¶È															
															if(cmd_data_len <= sizeof(GB17691_CMD_data))  //ÅĞ¶ÏÃüÁîÊÇ·ñ³¬³ö·¶Î§, gb17691_rd_pos++ ,ÔÙ½øÈëÑ­»·
															{
																if(EvalBCC_FromBytes(&GB17691_CMD_data[2], cmd_data_len-4) == GB17691_CMD_data[cmd_data_len-2])
																{
																		gb17691_rd_pos += cmd_data_len;
																	  if (GB17691_CMD_data[3]==1)//ÉÏ´«³É¹¦
																		{
																		 Actblue_State=1;
																		 ACTBLU_date_state=	upload_successful;																			
																		}	
																		else if(GB17691_CMD_data[3]==3)//Ğ£ÑéÊ§°Ü
																		{
																			Actblue_State=0;
																			ACTBLU_date_state=Check_failure;																																	
																		}
																		else //Î´Öª´íÎó
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
												else if(GB17691_CMD_data[2] ==1)//Ö¸ÁîÖ¡
												{
														  cmd_data_len=gb17691_rd_pos;	//Êı¾İµ¥Ôª³¤¶È
															if(cmd_data_len <= sizeof(GB17691_CMD_data))  //ÅĞ¶ÏÃüÁîÊÇ·ñ³¬³ö·¶Î§, gb17691_rd_pos++ ,ÔÙ½øÈëÑ­»·
															{
																if(EvalBCC_FromBytes(&GB17691_CMD_data[2], cmd_data_len-4) == GB17691_CMD_data[cmd_data_len-2])
																{
																		gb17691_rd_pos += cmd_data_len;																								
																		if (GB17691_CMD_data[3]==1)//ÉÏ´«³É¹¦
																		{
																			 Actblue_State=1;
																			 ACTBLU_date_state=	upload_successful;															
																		}	
																		else if(GB17691_CMD_data[3]==3)//Ğ£ÑéÊ§°Ü
																		{
																			Actblue_State=0;
																			ACTBLU_date_state=Check_failure;																																	
																		}
																		else //Î´Öª´íÎó
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
							//--------------------------------------Ö£ÖİĞÂ¼Ó¿ªÊ¼------------------
					  if(get_sim_rx_data_size(gb17691_rd_pos) > 90)  //ÖÁÉÙ½ÓÊÕµ½7¸ö×Ö½Ú£¬²Å¿ªÊ¼½âÎö                Ö£ÖİÌí¼Ó  ÊµÊ±Êı¾İ°ü»Ø¸´  ¹²7¸ö×Ö½Ú
            {
                get_sim_data_multiple_bytes(GB17691_CMD_data, gb17691_rd_pos, 8);
                if(GB17691_CMD_data[0] == (uint8_t)(START_STRING >> 8))//ÅĞ¶ÏÆğÊ¼·û ºÍ VIN
                {
                    if(GB17691_CMD_data[1] == (uint8_t)(START_STRING))
                    {																				
											if((GB17691_CMD_data[6]=='Q')&&(GB17691_CMD_data[7]=='N')	)					//ĞÂÏç
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
						//--------------------------------------Ö£ÖİĞÂ¼Ó½áÊø------------------
											
											
											
					
            if(get_sim_rx_data_size(gb17691_rd_pos) > 23)  //ÖÁÉÙ½ÓÊÕµ½24¸ö×Ö½Ú£¬²Å¿ªÊ¼½âÎö
            {
                get_sim_data_multiple_bytes(GB17691_CMD_data, gb17691_rd_pos, 24);
                if(GB17691_CMD_data[0] == (uint8_t)(START_STRING >> 8))//ÅĞ¶ÏÆğÊ¼·û ºÍ VIN
                {
                    if(GB17691_CMD_data[1] == (uint8_t)(START_STRING))
                    {																						
                        cmd_data_len = (uint16_t)(GB17691_CMD_data[22] << 8) + (uint16_t)GB17691_CMD_data[23];
                        cmd_data_len += 25;
                        //Êı¾İµ¥Ôª³¤¶È£¬¼ÓÉÏ24×Ö½ÚµÄĞÅÏ¢£¬1¸ö×Ö½ÚµÄĞ£Ñé£¬
                        if(cmd_data_len <= sizeof(GB17691_CMD_data))  //ÅĞ¶ÏÃüÁîÊÇ·ñ³¬³ö·¶Î§, gb17691_rd_pos++ ,ÔÙ½øÈëÑ­»·
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
                if(CMD_check_dealy < GB17691_CMD_CHECK_DEALY) //ÑÓÊ±1s
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


void bool_login_send_new(void)   //µÇÈë
{
  
    rtc_time_t cur_time;
    uint8_t send_data[120];
    uint8_t send_data_size;

    send_data[0] = (uint8_t)(START_STRING >> 8);  //ÆğÊ¼·û
    send_data[1] = (uint8_t)START_STRING;
    send_data[2] = VEHICLE_LOGIN_CMD;             //µÇÂ¼ÃüÁî

    GB17691_get_VIN_Info(&send_data[3]);
    //send_data[20]= GB17691_get_softver_Info();
    //memcpy(&send_data[3],vin_info,sizeof(vin_info));  //VIN

    send_data[20] = TERMINAL_SOFT_VER;        //Èí¼ş°æ±¾ºÅ
    send_data[21] = NO_ENCRYPT;               //Êı¾İ¼ÓÃÜ·½Ê½, ²»¼ÓÃÜ

    get_current_time(&cur_time);              //Êı¾İ²É¼¯Ê±¼ä
    send_data[24] = cur_time.year;
    send_data[25] = cur_time.month;
    send_data[26] = cur_time.date;
    send_data[27] = cur_time.hour;
    send_data[28] = cur_time.minute;
    send_data[29] = cur_time.second;


//    lognum = get_lognum(cur_time);           //µÇÂ¼Á÷Ë®ºÅ
//    lognum++;
    send_data[30] =0x81;
    send_data[31] = 0;		
		send_data [32]=0x0f;

get_imei(&send_data[33]);                //SIMÄ£¿éIMEI
	
	lognum = get_lognum(cur_time);           //µÇÂ¼Á÷Ë®ºÅ
	lognum++;
	send_data[48] = (uint8_t)(lognum >> 8);
	send_data[49] = (uint8_t)lognum;
	
	get_iccid(&send_data[50]);              //SIM¿¨ICCIDºÅ 
		
	send_data[22] = 0;                      //Ê±¼ä6£¬µÇÈëÁ÷Ë®ºÅ2£¬ICCIDÂë20£¬IMEIĞÅÏ¢Ìå1  ³¤¶È 2+ imei 15  ¹² 46×Ö½Ú
	send_data[23] = 0x2e;
		
	send_data_size = 71;                   //·¢ËÍÊı¾İ×Ü¸öÊı:22+2+46+1 ;
	send_data[send_data_size-1] = EvalBCC_FromBytes(&send_data[2], send_data_size-3);  //Ğ£ÑéÎ»

		int res=SocketWrite(1,send_data_size);
						if(res==0)//¶Ë¿Ú´ò¿ª³É¹¦
						{
									GB17691_send_data(send_data,send_data_size);                         //ÊµÊ±ÉÏ±¨									
#ifdef debug
	gb17691_print_data(send_data,send_data_size);		
#endif
		
					}
						else//¶Ë¿Ú´ò¿ªÊ§°Ü¡£ÖØĞÂ´ò¿ªsocket
						{
//							int re=OpenSocket(1);
						}
}


void timing_send(void)  //Ğ£Ê±
{
	rtc_time_t cur_time;
	uint8_t send_data[50];
	uint8_t send_data_size;
	
	send_data[0] = (uint8_t)(START_STRING >> 8);         //ÆğÊ¼·û
	send_data[1] = (uint8_t)START_STRING;          
	send_data[2] = VEHICLE_TIMING_CMD;                   //Ğ£Ê±ÃüÁî
	
	GB17691_get_VIN_Info(&send_data[3]);
	
			if(getTermianlState()==1) 						//²âÊÔ×´Ì¬²ÉÓÃĞé¼ÙVIN   //20200323 del
		{		
			for(u8 i=0;i<17;i++)
			{
				send_data[3+i]='L';
			}
		}
	
	//send_data[20]= GB17691_get_softver_Info();
//	memcpy(&send_data[3],vin_info,sizeof(vin_info));     //VIN
	send_data[20] = TERMINAL_SOFT_VER;                   //Èí¼ş°æ±¾ºÅ
	send_data[21] = NO_ENCRYPT;                          //Êı¾İ¼ÓÃÜ·½Ê½, ²»¼ÓÃÜ
	
	get_current_time(&cur_time);                         //Êı¾İ²É¼¯Ê±¼ä
	send_data[24] = cur_time.year;
	send_data[25] = cur_time.month;
	send_data[26] = cur_time.date;
	send_data[27] = cur_time.hour;
	send_data[28] = cur_time.minute;
	send_data[29] = cur_time.second;
	
	send_data [30]=0x81;                                 //IMEIĞÅÏ¢ÌåÀàĞÍ
	send_data [31]=0;
	send_data [32]=0x0f;
	get_imei(&send_data[33]);                            //SIMÄ£¿éIMEI
	
		if(getTermianlState()==1) 						//²âÊÔ×´Ì¬²ÉÓÃĞé¼Ù IMEI
		{		
			for(u8 i=0;i<15;i++)
			{
				send_data[33+i]='8';
			}
		}		
		
	send_data[22] = 0;                                   // 6+1+2+15=24
	send_data[23] = 0x18;
	
	send_data_size = 49;                                 //·¢ËÍÊı¾İ×Ü¸öÊı:22+2+16+1 ;
	send_data[48] = EvalBCC_FromBytes(&send_data[2], 46);


int res=SocketWrite(1,send_data_size);
						if(res==0)//¶Ë¿Ú´ò¿ª³É¹¦
						{
									GB17691_send_data(send_data,send_data_size);                         //ÊµÊ±ÉÏ±¨									
						}
						else//¶Ë¿Ú´ò¿ªÊ§°Ü¡£ÖØĞÂ´ò¿ªsocket
						{
//							int re=OpenSocket(1);
						}

    old_time_stamp = get_time_stamp();
}

u16 alarm_num=0;
void disman_alarm_send(void)  //²ğ³ı±¨¾¯
{
    rtc_time_t cur_time;
    uint8_t send_data[50];
    uint8_t send_data_size;

    send_data[0] = (uint8_t)(START_STRING >> 8);         //ÆğÊ¼·û
    send_data[1] = (uint8_t)START_STRING;
    send_data[2] = 06;                   //²ğ³ı±¨¾¯

    GB17691_get_VIN_Info(&send_data[3]);
	
    send_data[20] = TERMINAL_SOFT_VER;                   //Èí¼ş°æ±¾ºÅ
    send_data[21] = NO_ENCRYPT;                          //Êı¾İ¼ÓÃÜ·½Ê½, ²»¼ÓÃÜ

    send_data[22] = 0;                                   // 17
    send_data[23] = 17;
	
    get_current_time(&cur_time);              //Êı¾İ²É¼¯Ê±¼ä
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
    memcpy(&send_data[33] ,&location.longitude,4);                                            //¾­¶È
		memcpy(&send_data[37],&location.latitude,4);                                             //Î³¶È
		
    send_data_size = 42;                                 //·¢ËÍÊı¾İ×Ü¸öÊı:22+2+16+1 ;
    send_data[41] = EvalBCC_FromBytes(&send_data[2], 39);


int res=SocketWrite(1,send_data_size);
						if(res==0)//¶Ë¿Ú´ò¿ª³É¹¦
						{
									GB17691_send_data(send_data,send_data_size);                         //ÊµÊ±ÉÏ±¨									
						}
						else//¶Ë¿Ú´ò¿ªÊ§°Ü¡£ÖØĞÂ´ò¿ªsocket
						{
//							int re=OpenSocket(1);
						}

    old_time_stamp = get_time_stamp();
}


void login_send_new(void)   //µÇÈë
{
    rtc_time_t cur_time;
    uint8_t send_data[120];
    uint8_t send_data_size;

    send_data[0] = (uint8_t)(START_STRING >> 8);  //ÆğÊ¼·û
    send_data[1] = (uint8_t)START_STRING;
    send_data[2] = ZHENGZHOU_LOGIN_CMD;             //µÇÂ¼ÃüÁî

    GB17691_get_VIN_Info(&send_data[3]);
    //send_data[20]= GB17691_get_softver_Info();
    //memcpy(&send_data[3],vin_info,sizeof(vin_info));  //VIN

    send_data[20] = TERMINAL_SOFT_VER;        //Èí¼ş°æ±¾ºÅ
    send_data[21] = NO_ENCRYPT;               //Êı¾İ¼ÓÃÜ·½Ê½, ²»¼ÓÃÜ

    get_current_time(&cur_time);              //Êı¾İ²É¼¯Ê±¼ä
    send_data[24] = cur_time.year;
    send_data[25] = cur_time.month;
    send_data[26] = cur_time.date;
    send_data[27] = cur_time.hour;
    send_data[28] = cur_time.minute;
    send_data[29] = cur_time.second;


    lognum = get_lognum(cur_time);           //µÇÂ¼Á÷Ë®ºÅ
    lognum++;
    send_data[30] = (uint8_t)(lognum >> 8);
    send_data[31] = (uint8_t)lognum;		
		set_lognum(cur_time, lognum);
		
    get_iccid(&send_data[32]);              //SIM¿¨ICCIDºÅ	
    GB17691_get_VIN_Info(&send_data[52]);		
  	get_imei(&send_data[69]);                //SIMÄ£¿éIMEI
		
		send_data[84]=0;
		send_data[85]=0;
		
		send_data[86]=500>>8;
		send_data[87]=500;

		send_data[88]=0x01;
		send_data[89]=0x00;
		send_data[90]=0x00;

    send_data[22] = 0;                      //Ê±¼ä6£¬µÇÈëÁ÷Ë®ºÅ2£¬ICCIDÂë20£¬IMEIĞÅÏ¢Ìå1  ³¤¶È 2+ imei 15  ¹² 46×Ö½Ú
    send_data[23] = 67;

    send_data_size = 92;                   //·¢ËÍÊı¾İ×Ü¸öÊı:22+2+46+1 ;
    send_data[send_data_size-1] = EvalBCC_FromBytes(&send_data[2], send_data_size-3);  //Ğ£ÑéÎ»

		int res=SocketWrite(1,send_data_size);
						if(res==0)//¶Ë¿Ú´ò¿ª³É¹¦
						{
								GB17691_send_data(send_data,send_data_size);                         //ÊµÊ±ÉÏ±¨		
#ifdef debug
	gb17691_print_data(send_data,send_data_size);			
#endif	
							
						}
						else//¶Ë¿Ú´ò¿ªÊ§°Ü¡£ÖØĞÂ´ò¿ªsocket
						{
//							int re=OpenSocket(1);
						}

}

void logout_send(void)  //µÇ³ö
{
    rtc_time_t cur_time;
    uint8_t send_data[60];

    send_data[0] = (uint8_t)(START_STRING >> 8);
    send_data[1] = (uint8_t)START_STRING;
    send_data[2] = VEHICLE_LOGOUT_CMD;

    GB17691_get_VIN_Info(&send_data[3]);
    //send_data[20]= GB17691_get_softver_Info();

    //memcpy(&send_data[3],vin_info,sizeof(vin_info));

    send_data[20] = TERMINAL_SOFT_VER;        //Èí¼ş°æ±¾ºÅ
    send_data[21] = NO_ENCRYPT;               //Êı¾İ¼ÓÃÜ·½Ê½, ²»¼ÓÃÜ

    get_current_time(&cur_time);
    send_data[24] = cur_time.year;
    send_data[25] = cur_time.month;
    send_data[26] = cur_time.date;
    send_data[27] = cur_time.hour;
    send_data[28] = cur_time.minute;
    send_data[29] = cur_time.second;

    send_data [30]=0x81;                     //IMEIĞÅÏ¢ÌåÀàĞÍ
    send_data [31]=0;
    send_data [32]=0x0f;
    get_imei(&send_data[33]);                            //SIMÄ£¿éIMEI

    send_data[48] = (uint8_t)(lognum >> 8);  //µÇ³öÁ÷Ë®ºÅ
    send_data[49] = (uint8_t)lognum;

    send_data[22] = 0x00;                    //µÇ³ö±¨ÎÄÊı¾İµ¥Ôª£¬Ê±¼ä£¬µÇ³öÁ÷Ë®ºÅ IMEI     6+2+1+2+15 =26×Ö½Ú
    send_data[23] = 0x1a;

    send_data[50] = EvalBCC_FromBytes(&send_data[2], 48);

    //sim_uart_tx(send_data,33);
//    GB17691_send_data(send_data,51);
		int res=SocketWrite(1,51);
						if(res==0)//¶Ë¿Ú´ò¿ª³É¹¦
						{
									GB17691_send_data(send_data,51);                         //ÊµÊ±ÉÏ±¨									
						}
						else//¶Ë¿Ú´ò¿ªÊ§°Ü¡£ÖØĞÂ´ò¿ªsocket
						{
//							int re=OpenSocket(1);
						}

}

void BOOL_data_put_buf(uint8_t handle)   //´ò°ü ´æ´¢  ÉÏ±¨
{
	uint16_t data_len = 0;
//	uint16_t temp_len=0;
//	uint8_t temp_buf[36]={0};
	save_resend_data_t save_data_status;
	
//	if(is_set_time())
	{
		GB17691_data_package[0] = (uint8_t)(START_STRING >> 8);                             // ÆğÊ¼·û 0x2323
		GB17691_data_package[1] = (uint8_t)(START_STRING);
	
		//memcpy(&GB17691_data_package[3],vin_info,sizeof(vin_info));                         //VIN
		GB17691_get_VIN_Info(&GB17691_data_package[3]);
		
		//GB17691_data_package[20]= GB17691_get_softver_Info();
		

	  GB17691_data_package[20] = TERMINAL_SOFT_VER;                                    //Èí¼ş°æ±¾ºÅ
	//	GB17691_data_package[20] =temp_soft_ver;                                           //Èí¼ş°æ±¾ºÅ
		GB17691_data_package[21] = NO_ENCRYPT;                                             //Êı¾İ¼ÓÃÜ·½Ê½ 0x01
	
		rtc_time_t cur_time;
		get_current_time(&cur_time);                                                        // »ñÈ¡RTC µ±Ç°Ê±¼ä
		save_data_status.timestamp = get_time_stamp();                                      // Ê±¼ä¼ÆÊı
		
		memcpy(&GB17691_data_package[24+data_len],&cur_time,sizeof(rtc_time_t));            //¼ÓÉÏµ±Ç°Ê±¼ä
		data_len += sizeof(rtc_time_t);
		
		GB17691_data_package[24+data_len]= (uint8_t)(INFO_STREAM_NO>>8);                    //ĞÅÏ¢Á÷Ë®ºÅ
		GB17691_data_package[25+data_len]= (uint8_t)(INFO_STREAM_NO);
		data_len +=2;
		
		GB17691_data_package[24+data_len] = GB17691_INFO_IMEI_DATA;                      // ĞÅÏ¢ÀàĞÍ±êÖ¾:IMEIĞÅÏ¢Ìå  ,0X81
		data_len += 1;
		GB17691_data_package[24+data_len]=0;                                            //IMEI³¤¶È 2×Ö½Ú±íÊ¾ 
		GB17691_data_package[25+data_len]=0x0f;
		data_len +=2;
		get_imei(&GB17691_data_package[24+data_len]);                          
		data_len +=15;
		
		GB17691_data_package[24+data_len] = GB17691_INFO_OBD_DATA;                         // ĞÅÏ¢ÀàĞÍ±êÖ¾:OBDĞÅÏ¢ ,0X01
		data_len += 1;

    data_len += GB17691_get_OBD_Info(&GB17691_data_package[24+data_len]);           //OBDĞÅÏ¢
		
		GB17691_data_package[24+data_len] = GB17691_INFO_DATASTREAM_DATA;                               //ĞÅÏ¢ÀàĞÍ±êÖ¾:Êı¾İÁ÷ĞÅÏ¢ ,0X02
		data_len += 1;
		
		data_len += GB17691_get_BOOLdatastream_Info(&GB17691_data_package[24+data_len]);                                                  //»ñÈ¡Êı¾İÁ÷ĞÅÏ¢

		GB17691_data_package[24+data_len] = GB17691_INFO_CUSTOM_DATA;                         // ĞÅÏ¢ÀàĞÍ±êÖ¾:×Ô¶¨ÒåĞÅÏ¢  ,0X80
		data_len += 1;
		data_len += GB17691_get_BOOLcustom_Info(&GB17691_data_package[24+data_len]);                                                  //»ñÈ¡×Ô¶¨ÒåĞÅÏ¢Ìå
			
		GB17691_data_package[22] = (uint8_t)(data_len >> 8);                   // 6+ 2 + 1+2+15 + 1+96 +1+37 +1+2+8
		GB17691_data_package[23] = (uint8_t)data_len;
			
			if(handle & REAL_TIME_UPLOAD)                                                         //ÊµÊ±ÉÏ±¨
			{
				GB17691_data_package[2] = VEHICLE_UPLOAD_CMD;
				GB17691_data_package[24+data_len] = EvalBCC_FromBytes(&GB17691_data_package[2], data_len+22);//Òì»òĞ£Ñé×Ö
				
				int res=SocketWrite(1,24+data_len+1);
						if(res==0)
						{
							GB17691_send_data(GB17691_data_package, 24+data_len+1);   
	
						}
						else
						{
//							int re=OpenSocket(1);
						}
						
//				GB17691_send_data(GB17691_data_package, 24+data_len+1);                           //ÊµÊ±ÉÏ±¨
				INFO_STREAM_NO++;                                                                 //ĞèÓÅ»¯:ÖĞ¼äÉè±¸¶Ïµç,ÔòĞè×ö´æ´¢´¦Àí, ÒÔÌìÎªµ¥Î»,´Ó1¿ªÊ¼ÀÛ¼Ó  
				if(INFO_STREAM_NO>=65531){
					INFO_STREAM_NO=0;
				}
			}

			GB17691_data_package[2] = VEHICLE_RELOAD_CMD;
			GB17691_data_package[24+data_len] = EvalBCC_FromBytes(&GB17691_data_package[2], data_len+22);  //Òì»òĞ£Ñé×Ö
			data_len += 1;
			
			if(handle & SAVE_RESEND_DATA)    //´æ´¢Êı¾İ
			{
				if((handle & REAL_TIME_UPLOAD) && (terminal_mode == TERMINAL_NET))
				{
					save_data_status.resend_flag = SEND_DATA_FLAG;
					realtime_upload_flag_addr[realtime_load_cnt] = realtime_data_wr_flash_addr + 24 + data_len + (sizeof(save_resend_data_t) - 2); //×îºóÁ½¸ö×Ö½ÚÊÇ±êÖ¾Î»
					if(realtime_upload_flag_addr[realtime_load_cnt] >= RESEND_DATA_END_ADDR)
					{
						realtime_upload_flag_addr[realtime_load_cnt] -= RESEND_DATA_END_ADDR;
						realtime_upload_flag_addr[realtime_load_cnt] += RESEND_DATA_START_ADDR;
					}
					realtime_load_cnt++;
					if(realtime_load_cnt >= (sizeof(realtime_upload_flag_addr) / sizeof(uint32_t))) //·ÀÖ¹Òç³ö
					{
						realtime_load_cnt = 0;
					}
				}
				else
				{
					save_data_status.resend_flag = RESEND_DATA_FLAG;
				}
							
				memcpy(&GB17691_data_package[24+data_len],&save_data_status,sizeof(save_resend_data_t));   // ÊµÊ±ÉÏ±¨Êı¾İ + Ê±¼ä´Á+ °ü¼ÆÊı+ °üÊıÁ¿+ °ü×´Ì¬±ê¼Ç 
			  write_data_to_flash(GB17691_data_package,realtime_data_wr_flash_addr,24+data_len+sizeof(save_resend_data_t),RESEND_DATA_FLASH_AREA);    // Êı¾İĞ´ÈëFLASH
				
				realtime_data_wr_flash_addr += (24+data_len+sizeof(save_resend_data_t));
				if(realtime_data_wr_flash_addr >= RESEND_DATA_END_ADDR)
				{
					realtime_data_wr_flash_addr -= RESEND_DATA_END_ADDR;
					realtime_data_wr_flash_addr += RESEND_DATA_START_ADDR;
				}
//				
//				/***´æ´¢Ã¿³¬¹ı2M¼ÇÂ¼Ò»ÏÂÊµÊ±Êı¾İ¶ÁĞ´FLASHµØÖ· ****************************/
////				resend_wrtie_size += (24+data_len+sizeof(save_resend_data_t));
////				if(resend_wrtie_size >= RESEND_WRITE_MAX_SIZE) //2M
////				{
////					//save_resend_record_addr(&resend_record_addr, realtime_data_wr_flash_addr);  //ÔÚÓÃ³µOBDÏîÄ¿²»´æ´¢²¹·¢Êı¾İµØÖ·
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



// ÖÕ¶ËÓ¦´ğ ¿ØÖÆÃüÁî»Ø¸´
void GB17691_ack_send( u8 con_ID ,u16 ctlidNo , u8 con_state , u8 ack_info)  //µÇ³ö
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

    send_data[20] = TERMINAL_SOFT_VER;        //Èí¼ş°æ±¾ºÅ
    send_data[21] = NO_ENCRYPT;               //Êı¾İ¼ÓÃÜ·½Ê½, ²»¼ÓÃÜ

//    get_current_time(&cur_time);
//    send_data[24] = cur_time.year;
//    send_data[25] = cur_time.month;
//    send_data[26] = cur_time.date;
//    send_data[27] = cur_time.hour;
//    send_data[28] = cur_time.minute;
//    send_data[29] = cur_time.second;

//				memcpy(send_data+24,(void *)flag_restart_net_time,6);
			memcpy(send_data+24,(void *)flag_answer_net_time,6);
//    send_data [30]=0x81;                     //IMEIĞÅÏ¢ÌåÀàĞÍ
//    send_data [31]=0;
//    send_data [32]=0x0f;
//    get_imei(&send_data[33]);                            //SIMÄ£¿éIMEI

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
		send_data[24+data_len] = GB17691_INFO_OBD_DATA;                         // ĞÅÏ¢ÀàĞÍ±êÖ¾:OBDĞÅÏ¢ ,0X01
		data_len += 1;

    data_len += GB17691_get_OBD_Info(&send_data[24+data_len]);           //OBDĞÅÏ¢
		
		send_data[24+data_len] = GB17691_INFO_DATASTREAM_DATA;                               //ĞÅÏ¢ÀàĞÍ±êÖ¾:Êı¾İÁ÷ĞÅÏ¢ ,0X02
		data_len += 1;
		
		data_len += GB17691_get_BOOLdatastream_Info(&send_data[24+data_len]);                                                  //»ñÈ¡Êı¾İÁ÷ĞÅÏ¢

		send_data[24+data_len] = GB17691_INFO_CUSTOM_DATA;                         // ĞÅÏ¢ÀàĞÍ±êÖ¾:×Ô¶¨ÒåĞÅÏ¢  ,0X80
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
    send_data[22] = 0x00;                    //µÇ³ö±¨ÎÄÊı¾İµ¥Ôª£¬Ê±¼ä£¬µÇ³öÁ÷Ë®ºÅ IMEI     6+2+1+2+15 =26×Ö½Ú
    send_data[23] = 11;
}
	
    send_data[24+data_len] = EvalBCC_FromBytes(&send_data[2], 24+data_len-2);

		int res=SocketWrite(1,24+data_len+1);
						if(res==0)//¶Ë¿Ú´ò¿ª³É¹¦
						{
									GB17691_send_data(send_data,24+data_len+1);                         //ÊµÊ±ÉÏ±¨	
#ifdef debug
							     gb17691_print_data(send_data,24+data_len+1);		
#endif							
						}
						else//¶Ë¿Ú´ò¿ªÊ§°Ü¡£ÖØĞÂ´ò¿ªsocket
						{
//							int re=OpenSocket(1);
						}

}
/************************************************************************
  * @ÃèÊö:   °²»Õ°¬¿ÉÀ¶G4OBDÊı¾İ´ò°ü ´æ´¢  ÉÏ±¨
  * @²ÎÊı:   None
  * @·µ»ØÖµ: None
  **********************************************************************/
void GB17691_data_put_buf(uint8_t handle)   //´ò°ü ´æ´¢  ÉÏ±¨
{
	      int cont=0;
        uint16_t data_len = 0;
		    unsigned int crcre=0xffff;

        save_resend_data_t save_data_status;
				GB17691_data_package[0] = '$';
				GB17691_data_package[1] = '$';
	    if (Actblue_State!=1)//×¢²áÖ¡
			 {
					GB17691_data_package[2] = '0';			 
					data_len=get_AIKELAN_register(&GB17691_data_package[3]);//Êı¾İ³¤¶È	
				 
					crcre=CRC16_Checkout(&GB17691_data_package[3],data_len);			 
					get_AIKELAN_crcre(crcre,&GB17691_data_package[3]+data_len);//±£´æĞ£Ñé4×Ö½Ú
				 
					GB17691_data_package[5+data_len]='#';//2+CRC2-1+Î²2
					GB17691_data_package[6+data_len]='#';
				 	GB17691_data_package[7+data_len]='\r';//2+CRC2-1+Î²2
					GB17691_data_package[8+data_len]='\n';
			 }
			 else  //Êı¾İÖ¡
			{
					GB17691_data_package[2] = '1';
					data_len=get_AIKELAN_datapack(&GB17691_data_package[3]);
				
					crcre=CRC16_Checkout(&GB17691_data_package[3],data_len);
					get_AIKELAN_crcre(crcre,&GB17691_data_package[3+data_len]);	//Ğ£Ñé°´ÕÕ4×Ö½Ú¼ÆËã	
					
					GB17691_data_package[5+data_len]='#';//
					GB17691_data_package[6+data_len]='#';//2+2+2
					GB17691_data_package[7+data_len]='\r';//2+CRC2-1+Î²2
					GB17691_data_package[8+data_len]='\n';
			}

			GB17691_send_data(GB17691_data_package, 9+data_len); 
			
				//----------------------------------------------------------------------
				
				
        if(handle & REAL_TIME_UPLOAD)                                                         //ÊµÊ±ÉÏ±¨
        {
						int res=SocketWrite(0,7+data_len);//Çø·Ö×¢²áºÍÊı¾İÖ¡
						if(res==0)//¶Ë¿Ú´ò¿ª³É¹¦
						{
									if(Actblue_State!=1)//×¢²áÖ¡
									{
												do
												{
													GB17691_send_data(GB17691_data_package, 6+data_len);
													cont ++;
												}
												 while(cont<4);
													
									}
									else//Êı¾İÖ¡
									{
										GB17691_send_data(GB17691_data_package, 6+data_len);                           //ÊµÊ±ÉÏ±¨
										
									}								
						}
						else//¶Ë¿Ú´ò¿ªÊ§°Ü¡£ÖØĞÂ´ò¿ªsocket
						{
//							int re=OpenSocket(0);
						}
						
            INFO_STREAM_NO++;                                                                 //ĞèÓÅ»¯:ÖĞ¼äÉè±¸¶Ïµç,ÔòĞè×ö´æ´¢´¦Àí, ÒÔÌìÎªµ¥Î»,´Ó1¿ªÊ¼ÀÛ¼Ó
            if(INFO_STREAM_NO>=65531)
            {
                INFO_STREAM_NO=0;
            }

        }

//        GB17691_data_package[2] = VEHICLE_RELOAD_CMD;
//        GB17691_data_package[24+data_len] = EvalBCC_FromBytes(&GB17691_data_package[2], data_len+22);  //Òì»òĞ£Ñé×Ö
//        data_len += 1;

        if(handle & SAVE_RESEND_DATA)    //´æ´¢Êı¾İ
        {
            if((handle & REAL_TIME_UPLOAD) && (terminal_mode == TERMINAL_NET))
            {
                save_data_status.resend_flag = REALTIME_UPLOAD_FLAG;
                realtime_upload_flag_addr[realtime_load_cnt] = realtime_data_wr_flash_addr + 24 + data_len + (sizeof(save_resend_data_t) - 2); //×îºóÁ½¸ö×Ö½ÚÊÇ±êÖ¾Î»
                if(realtime_upload_flag_addr[realtime_load_cnt] >= RESEND_DATA_END_ADDR)
                {
                    realtime_upload_flag_addr[realtime_load_cnt] -= RESEND_DATA_END_ADDR;
                    realtime_upload_flag_addr[realtime_load_cnt] += RESEND_DATA_START_ADDR;
                }
                realtime_load_cnt++;							
                if(realtime_load_cnt >= (sizeof(realtime_upload_flag_addr) / sizeof(uint32_t))) //·ÀÖ¹Òç³ö
                {
                    realtime_load_cnt = 0;
                }
            }
            else
            {
                save_data_status.resend_flag = RESEND_DATA_FLAG;
            }

            memcpy(&GB17691_data_package[24+data_len],&save_data_status,sizeof(save_resend_data_t));   // ÊµÊ±ÉÏ±¨Êı¾İ + Ê±¼ä´Á+ °ü¼ÆÊı+ °üÊıÁ¿+ °ü×´Ì¬±ê¼Ç
            write_data_to_flash(GB17691_data_package,realtime_data_wr_flash_addr,24+data_len+sizeof(save_resend_data_t),RESEND_DATA_FLASH_AREA);    // Êı¾İĞ´ÈëFLASH

            realtime_data_wr_flash_addr += (24+data_len+sizeof(save_resend_data_t));
            if(realtime_data_wr_flash_addr >= RESEND_DATA_END_ADDR)
            {
                realtime_data_wr_flash_addr -= RESEND_DATA_END_ADDR;
                realtime_data_wr_flash_addr += RESEND_DATA_START_ADDR;
            }

            /***´æ´¢Ã¿³¬¹ı2M¼ÇÂ¼Ò»ÏÂÊµÊ±Êı¾İ¶ÁĞ´FLASHµØÖ· ****************************/
//				resend_wrtie_size += (24+data_len+sizeof(save_resend_data_t));
//				if(resend_wrtie_size >= RESEND_WRITE_MAX_SIZE) //2M
//				{
//					//save_resend_record_addr(&resend_record_addr, realtime_data_wr_flash_addr);  //ÔÚÓÃ³µOBDÏîÄ¿²»´æ´¢²¹·¢Êı¾İµØÖ·
//					resend_wrtie_size = 0;
//				}
				
					} //**  

//    }
}//**

/************************************************************************
  * @ÃèÊö:   ĞÂÏçG4OBDÊı¾İ´ò°ü ´æ´¢  ÉÏ±¨
  * @²ÎÊı:   None
  * @·µ»ØÖµ: None
  **********************************************************************/
/*
void GB17691_data_put_buf(uint8_t handle)   //´ò°ü ´æ´¢  ÉÏ±¨
{
    uint16_t data_len = 0;
//		u8 *ps=NULL;
		unsigned int crcre=0xffff;
//	uint16_t temp_len=0;
//	uint8_t temp_buf[36]={0};
    save_resend_data_t save_data_status;

    // if(is_set_time())
    {
        GB17691_data_package[0] = '#';                             // ÆğÊ¼·û 0x2323
        GB17691_data_package[1] = '#';

			data_len=get_xinxiang_datapack(&GB17691_data_package[6]);
		  get_xinxiang_datalen(data_len,&GB17691_data_package[2]);	
			
		  crcre=CRC16_Checkout(&GB17691_data_package[6],data_len);
			get_xinxiang_crcre(crcre,&GB17691_data_package[6+data_len]);
			
			GB17691_data_package[10+data_len]=0x0D;
			GB17691_data_package[11+data_len]=0x0A;			
			GB17691_send_data(GB17691_data_package, 12+data_len); 
			
			
			
			
				//----------------------------------------------------------------------
				
				
        if(handle & REAL_TIME_UPLOAD)                                                         //ÊµÊ±ÉÏ±¨
        {
						int res=SocketWrite(0,12+data_len);
						if(res==0)//¶Ë¿Ú´ò¿ª³É¹¦
						{
									GB17691_send_data(GB17691_data_package, 12+data_len);                           //ÊµÊ±ÉÏ±¨									
						}
						else//¶Ë¿Ú´ò¿ªÊ§°Ü¡£ÖØĞÂ´ò¿ªsocket
						{
//							int re=OpenSocket(0);
						}
						
            INFO_STREAM_NO++;                                                                 //ĞèÓÅ»¯:ÖĞ¼äÉè±¸¶Ïµç,ÔòĞè×ö´æ´¢´¦Àí, ÒÔÌìÎªµ¥Î»,´Ó1¿ªÊ¼ÀÛ¼Ó
            if(INFO_STREAM_NO>=65531)
            {
                INFO_STREAM_NO=0;
            }

        }



        GB17691_data_package[2] = VEHICLE_RELOAD_CMD;
        GB17691_data_package[24+data_len] = EvalBCC_FromBytes(&GB17691_data_package[2], data_len+22);  //Òì»òĞ£Ñé×Ö
        data_len += 1;

        if(handle & SAVE_RESEND_DATA)    //´æ´¢Êı¾İ
        {
            if((handle & REAL_TIME_UPLOAD) && (terminal_mode == TERMINAL_NET))
            {
                save_data_status.resend_flag = REALTIME_UPLOAD_FLAG;
                realtime_upload_flag_addr[realtime_load_cnt] = realtime_data_wr_flash_addr + 24 + data_len + (sizeof(save_resend_data_t) - 2); //×îºóÁ½¸ö×Ö½ÚÊÇ±êÖ¾Î»
                if(realtime_upload_flag_addr[realtime_load_cnt] >= RESEND_DATA_END_ADDR)
                {
                    realtime_upload_flag_addr[realtime_load_cnt] -= RESEND_DATA_END_ADDR;
                    realtime_upload_flag_addr[realtime_load_cnt] += RESEND_DATA_START_ADDR;
                }
                realtime_load_cnt++;							
                if(realtime_load_cnt >= (sizeof(realtime_upload_flag_addr) / sizeof(uint32_t))) //·ÀÖ¹Òç³ö
                {
                    realtime_load_cnt = 0;
                }
            }
            else
            {
                save_data_status.resend_flag = RESEND_DATA_FLAG;
            }

            memcpy(&GB17691_data_package[24+data_len],&save_data_status,sizeof(save_resend_data_t));   // ÊµÊ±ÉÏ±¨Êı¾İ + Ê±¼ä´Á+ °ü¼ÆÊı+ °üÊıÁ¿+ °ü×´Ì¬±ê¼Ç
            write_data_to_flash(GB17691_data_package,realtime_data_wr_flash_addr,24+data_len+sizeof(save_resend_data_t),RESEND_DATA_FLASH_AREA);    // Êı¾İĞ´ÈëFLASH

            realtime_data_wr_flash_addr += (24+data_len+sizeof(save_resend_data_t));
            if(realtime_data_wr_flash_addr >= RESEND_DATA_END_ADDR)
            {
                realtime_data_wr_flash_addr -= RESEND_DATA_END_ADDR;
                realtime_data_wr_flash_addr += RESEND_DATA_START_ADDR;
            }

            /***´æ´¢Ã¿³¬¹ı2M¼ÇÂ¼Ò»ÏÂÊµÊ±Êı¾İ¶ÁĞ´FLASHµØÖ· ****************************/
//				resend_wrtie_size += (24+data_len+sizeof(save_resend_data_t));
//				if(resend_wrtie_size >= RESEND_WRITE_MAX_SIZE) //2M
//				{
//					//save_resend_record_addr(&resend_record_addr, realtime_data_wr_flash_addr);  //ÔÚÓÃ³µOBDÏîÄ¿²»´æ´¢²¹·¢Êı¾İµØÖ·
//					resend_wrtie_size = 0;
//				}
				
//					}   

//    }
//}

/******************************************************************************************
¹¦ÄÜ£º´ò°ü°²È«Ğ¾Æ¬ID
×÷Õß£º¶ÅººÓî 19.09.26
²ÎÊı£ºvoid
·µ»ØÖµ£ºvoid  
*******************************************************************************************/



/******************************************************************************************
¹¦ÄÜ£ºÏòÖ£ÖİÆ½Ì¨·¢ËÍ±¸°¸ĞÅÏ¢
×÷Õß£º¶ÅººÓî 19.09.26
²ÎÊı£ºvoid
·µ»ØÖµ£ºvoid  
*******************************************************************************************/
//static void record_send(void)
//{
//		u8 *ps=NULL;
//	
//    rtc_time_t cur_time;
//    uint8_t send_data[280];
//    uint16_t send_data_size;

//    send_data[0] = (uint8_t)(START_STRING >> 8);  //ÆğÊ¼·û
//    send_data[1] = (uint8_t)START_STRING;
//    send_data[2] = ZHENGZHOU_RECORD_CMD;             //±¸°¸ÃüÁî

//    GB17691_get_VIN_Info(&send_data[3]);

//    send_data[20] = TERMINAL_SOFT_VER;        //Èí¼ş°æ±¾ºÅ
//    send_data[21] = NO_ENCRYPT;               //Êı¾İ¼ÓÃÜ·½Ê½, ²»¼ÓÃÜ

////	  send_data[22] = 0;                      //Ê±¼ä6  + Ğ¾Æ¬ID16 + ¹«Ô¿64 + VIN17 + Ç©Ãû130=233
////    send_data[23] = 0xE9;

//		send_data_size=24;
//	
//    get_current_time(&cur_time);              //Êı¾İ²É¼¯Ê±¼ä
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
//		ps=ReadPublickey();														//¹«Ô¿
//		if(ps==NULL)
//			ps=ReadPublickey();														//¹«Ô¿	
//		
//		if(ps!=NULL)
//			lkt4305_state=1;
//		
//		memcpy(&send_data[46],ps,64);
//		send_data_size=send_data_size+64;

//		GB17691_get_VIN_Info(&send_data[110]);	
//		send_data_size=send_data_size+17;		// VIN
//		
//		ps=Signature(&send_data[30],97);					//Ç©Ãû
//		memcpy(&data_sign,ps,64);

//		send_data[send_data_size++] =32;   // Ç©ÃûRÖµ
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


//		send_data[send_data_size++] =32;   // Ç©ÃûSÖµ
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
//	  send_data[22] = (send_data_size-24)>>8;                      //Ê±¼ä6  + Ğ¾Æ¬ID16 + ¹«Ô¿64 + VIN17 + Ç©Ãû66=169
//    send_data[23] = (send_data_size-24);
//		
//		send_data[send_data_size] = EvalBCC_FromBytes(&send_data[2], send_data_size-2);  //Ğ£ÑéÎ»
//		send_data_size++;


//		int res=SocketWrite(0,send_data_size);
//						if(res==0)//¶Ë¿Ú´ò¿ª³É¹¦
//						{
//									GB17691_send_data(send_data,send_data_size);                         //ÊµÊ±ÉÏ±¨									
//						}
//						else//¶Ë¿Ú´ò¿ªÊ§°Ü¡£ÖØĞÂ´ò¿ªsocket
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
            mode = TERMINAL_NET;//  ÍøÂçÁ¬½ÓÄ£Ê½
        }
        else
        {
            mode = TERMINAL_FREE;//×ÔÓÉÄ£Ê½,
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
    if(terminal_mode != current_mode)     // Ä£Ê½ÓĞ¸Ä±ä, Èç¹ûÊÇÍøÂçÁ¬½Ó×´Ì¬±äÎªÆäËû×´Ì¬,Ôò ½«ÊµÊ±ÉÏ±¨Êı¾İ¸ÄÎªĞèÒª²¹·¢ÉÏ±¨µÄÊı¾İ
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
			
			
			
						GB17691_data_put_buf(GB17691_data_handle);//ÉÏ±¨Ö£ÖİÆ½Ì¨=---²âÊÔÊı¾İ
						
			
			if(cnt>101){cnt=0;}
      if(terminal_mode == TERMINAL_NET)
      {
				GB17691_send_enable=1;   //Ö£Öİ·¢ËÍÊ¹ÄÜ  Ğ¡Ê±
				if(record_state==1)		// ÒÑ±¸°¸ »òÔÚ²âÊÔ×´Ì¬
				{
						if(timing_state == 0)			//Î´Ğ£Ê±
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
												USARTSendOut(USART1, "\r\n·¢ËÍĞ£Ê±\r\n", strlen("\r\n·¢ËÍĞ£Ê±\r\n"));
						#endif	
								}
							}
						}
					else   // ÒÑĞ£Ê±
					{
						if(login_state == 0)  //Î´µÇÂ¼
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
//							USARTSendOut(USART1, "\r\n·¢ËÍµÇÂ¼\r\n", strlen("\r\n·¢ËÍĞ£Ê±\r\n"));
//			#endif
//								}
//							}
								login_state=1;
						}
						else  	// ÒÑĞ£Ê± ÒÑµÇÂ¼
						{
								GB17691_send_enable=1;   //Ö£Öİ·¢ËÍÊ¹ÄÜ  Ğ¡Ê±
							  GB17691_data_put_buf(GB17691_data_handle);//ÉÏ±¨Ö£ÖİÆ½Ì¨-----²âÊÔÊı¾İ
								if(send_heart_flag>3)
								{

									if(get_vin(vin_info))   	//»ñÈ¡VIN
									{
										flag_bool_login = 0 ;  //VMSÆ½Ì¨Ã¿´ÎÖØÆôÍøÂç¶¼ĞèÒª ÖÃ0.ÖØ·¢µÇÂ½£»
										reset_sim_net();						//120ÃëÆ½Ì¨Ã»ÓĞ»Ø¸´ ÖØÆôÍøÂç
									}	
									send_heart_flag=0;
//									record_state=0;			// ±¸°¸Ê§°Ü
//									set_recordstate(0);									//±£´æ±¸°¸ĞÅÏ¢
									
								}		
		//					GB17691_send_enable=1;   //Ö£Öİ·¢ËÍÊ¹ÄÜ  Ğ¡Ê±
						}
					}
				}
				else		//  Î´±¸°¸
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
//												USARTSendOut(USART1, "\r\n·¢ËÍ±¸°¸\r\n", strlen("\r\n·¢ËÍĞ£Ê±\r\n"));
//						#endif	
//								}
//							}
						record_state=1;
					
				}
				
		}
		else  // ²»ÊÇÁªÍø×´Ì¬
		{
			if(big_heart_flag==1)						//´óĞÄÌø
			{
//			    if((new_time_stamp - big_heart_temp_time_stamp) >= 300) //·¢´óĞÄÌø³É¹¦ÊÇ·ñ³¬Ê±5·ÖÖÓ,´Ë´ÎĞÄÌø²»·¢ÁË,Ìø³ö,·ñÔòÖØĞÂ·¢ 
			    if((new_time_stamp - big_heart_time_stamp) >= 300) //·¢´óĞÄÌø³É¹¦ÊÇ·ñ³¬Ê±5·ÖÖÓ,´Ë´ÎĞÄÌø²»·¢ÁË,Ìø³ö,·ñÔòÖØĞÂ·¢  
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
		//--------------------------´óĞÄÌø
			if(big_heart_flag==1)      
			{	  
			    if(login_state == 1)
					{
					  get_GB17691_gps_info(&cur_location);
						if(cur_location.status.is_valid==0)              // 0 : ±íÊ¾ÓĞĞ§ 1±íÊ¾ÎŞĞ§
						{  
							   //GB17691_data_handle |= SAVE_RESEND_DATA;     // °´ÊµÊ±ÉÏ±¨Êı¾İ¸ñÊ½´¦Àí,´«Ò»ÏÂGPSĞÅÏ¢, µ«²»´æ´¢
//					      if(GB17691_send_enable)
//				        {
						      GB17691_data_handle |= REAL_TIME_UPLOAD;
//					      }
							
							  if(GB17691_data_handle)
							  {
									if(GB17691_send_enable)
	//								GB17691_data_put_buf(GB17691_data_handle);//ÉÏ±¨Ö£ÖİÆ½Ì¨
									
							  	BOOL_data_put_buf(GB17691_data_handle); //´óĞÄÌøÉÏ±¨ÖÁ²¼¶ûÆ½Ì¨
							  }                                
							  big_heart_flag=0;
							  big_heart_time_stamp=new_time_stamp;
							  update_big_heart_time(big_heart_time_stamp); 
				    }

		//				else if((new_time_stamp - big_heart_temp_time_stamp) >= 300)						// Ã»ÓĞ¶¨Î»  ³¬Ê±·¢ËÍ
				else if((new_time_stamp - big_heart_time_stamp) >= 300)						// Ã»ÓĞ¶¨Î»  ³¬Ê±·¢ËÍ
							{   //20190312 add  for test 300->180
	
						      GB17691_data_handle |= REAL_TIME_UPLOAD;
							  if(GB17691_data_handle)
							  {
									if(GB17691_send_enable)
//									GB17691_data_put_buf(GB17691_data_handle);//ÉÏ±¨Ö£ÖİÆ½Ì¨
									
							  	BOOL_data_put_buf(GB17691_data_handle); //´óĞÄÌøÉÏ±¨ÖÁ²¼¶ûÆ½Ì¨
							  }  
								
								big_heart_flag=0;
								big_heart_time_stamp=new_time_stamp;
								update_big_heart_time(big_heart_time_stamp); 
						}
						
					}
					else
					{
		//				if((new_time_stamp - big_heart_temp_time_stamp) >= 300) 	//³¬Ê±
						if((new_time_stamp - big_heart_time_stamp) >= 300) 	//³¬Ê±
						 {
							 
									BOOL_data_put_buf(GB17691_data_handle); //´óĞÄÌøÉÏ±¨ÖÁ²¼¶ûÆ½Ì
									big_heart_flag=0;
#ifdef debug						
					        USARTSendOut(USART1, "send big heart over time1!\r\n", 28);
#endif
									big_heart_time_stamp=new_time_stamp;
								  update_big_heart_time(big_heart_time_stamp); 
							}
					}
			}
			//--------------------------------¶¨Ê±ÉÏ±¨---
//        else
			if (can_state==1)
        {
            if((new_time_stamp - realtime_data_stamp) >= MSG_SEND_INTERVAL)
            {
                if(can_state==1) //·ÀÖ¹´óĞÄÌø±êÖ¾Îª0Ê±,¶ÔÊı¾İ½øĞĞ´æ´¢
                {
                    realtime_data_stamp = new_time_stamp;
                    GB17691_data_handle |= SAVE_RESEND_DATA;
                    if(GB17691_send_enable)
                    {
                        GB17691_data_handle |= REAL_TIME_UPLOAD;
                    }

                    if(GB17691_data_handle)//2  Ö»´æ´¢
                    {	
											if(realup_cnt%10==4)
											{
//												GB17691_data_put_buf(GB17691_data_handle);//´ò°ü ´æ´¢ ÉÏ±¨	
//												send_heart_flag++;  //  ÊµÊ±=Ö£ÖİĞÄÌø ·¢ËÍ¼ÆÊı
											}
																					
											if(terminal_mode == TERMINAL_NET)
												GB17691_data_handle |= REAL_TIME_UPLOAD;  // µÚ¶şÁªÂ·ÎŞÌõ¼şÉÏ±¨
									
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
											
                        big_heart_time_stamp=new_time_stamp;      //¸üĞÂ´óĞÄÌøÊ±¼ä´Á
                        update_big_heart_time(big_heart_time_stamp);
                    }
                }
								realup_cnt++;
            }
						
						
						
						
						
						
        }
		}
//---------------------------µÇ³ö------------------					
    else
    {
        if(((terminal_mode == TERMINAL_NET)) && login_state)
        {
            logout_send();
            login_state = 0;
            GB17691_send_enable = 0;

            for(uint8_t i=0; i<realtime_load_cnt; i++)  //½«ÒÑ·¢Êı¾İ±ê¼ÇÎª·¢ËÍ³É¹¦
            {
                write_resend_data_flag(realtime_upload_flag_addr[i], SEND_DATA_FLAG);
            }
            realtime_load_cnt = 0;

            for(uint8_t i=0; i<resend_msg_cnt; i++)    //½«²¹·¢µÄÊı¾İ±ê¼ÇÎª·¢ËÍ³É¹¦
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

//					if(BKP_ReadBkpData(BKP_DR5) == 0x5a5a)     //µÚÒ»´ÎÉÏµç ´óĞÄÌø
//					{
//							big_heart_flag=1;
//							big_heart_temp_time_stamp = new_time_stamp;
//							BKP_WriteBackupRegister(BKP_DR5, 0xa5a5);
//					}

//					if(BKP_ReadBkpData(BKP_DR2) == 0xa5a5)
//					{
//							if((new_time_stamp - big_heart_time_stamp)>86400)  //Ê±¼ä¼ä¸ô24Ğ¡Ê±
//									//	if((new_time_stamp - big_heart_time_stamp)>600)  //Ê±¼ä¼ä¸ô10·ÖÖÓ
//							{
//									big_heart_flag=1;
//									big_heart_temp_time_stamp = new_time_stamp;
//							}
//					}
        }
    }
}
/*
¹¦ÄÜ:¸ù¾İÆğÊ¼·û VIN  Ğ£ÑéÂë, ÕÒÊı¾İÎ»ÖÃ, ²¹·¢Êı¾İĞÅÏ¢µØÖ·, ÒÔ¼°²¹·¢¿ªÊ¼µØÖ·¸üĞÂ
²ÎÊı: uint32_t* addr ²¹·¢¿ªÊ¼µØÖ·  º¯ÊıÖĞ»á¸üĞÂ
      uint32_t* resend_info_addr  ²¹·¢Êı¾İĞÅÏ¢µØÖ·  º¯ÊıÖĞ»á¸üĞÂ
·µ»Ø: Êı¾İµØÖ·

*/
uint8_t* read_msg_from_flash(uint32_t* addr, uint32_t* resend_info_addr,uint8_t flash_area)
{
    uint16_t data_len = 0;
    uint16_t i = 0;
    uint32_t res_addr = *addr;
    uint8_t* data_pos = NULL;

    read_data_from_flash(GB17691_data_package,*addr,sizeof(GB17691_data_package),flash_area); //´Óaddr¶ÁÊı¾İ·ÅGB17691_data_package
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
                    if((cur_stamp - save_data_satus->timestamp) > (HEART_INTERVAL << 1))   //²¹·¢100ÃëÇ°Êı¾İ
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

                    //ÖØ·¢±êÖ¾Æ«ÒÆ6¸ö×Ö½Ú ,¼´ÏÂÒ»°üÊı¾İ²¹·¢¿ªÊ¼µØÖ·
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
																			//±ê¼Ç±¨ÎÄ×´Ì¬Îª·¢ËÍ³É¹¦
										resend_flash_addr_end_flag = 0;
										for(uint8_t i=0; i<realtime_load_cnt; i++)  //ÊÕµ½ĞÄÌøÃüÁî,Ôò½«ÒÑ·¢Êı¾İ±ê¼ÇÎª·¢ËÍ³É¹¦
										{
												write_resend_data_flag(realtime_upload_flag_addr[i], SEND_DATA_FLAG);
										}
										realtime_load_cnt = 0;

										for(uint8_t i=0; i<resend_msg_cnt; i++)    //ÊÕµ½ĞÄÌøÃüÁî,Ôò½«²¹·¢µÄÊı¾İ±ê¼ÇÎª·¢ËÍ³É¹¦
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
//        »ñµÃ²¹·¢¿ªÊ¼Ğ´µØÖ·
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
    ´Ó´æ´¢µÄ²¹·¢Êı¾İÀïÕÒ³öÊ±¼ä´Á×î´óµÄÄÇ°üÊı¾İµØÖ·, ÒÔ¼°ÊµÊ±Êı¾İ¶ÁĞ´µØÖ·
    ******************************************************/
    uint32_t check_flash_size  = RESEND_DATA_START_ADDR;

    while(check_flash_size < RESEND_DATA_END_ADDR)
    {
        addr_temp = addr;
        msg_pos = read_msg_from_flash(&addr,&resend_flag_addr,RESEND_DATA_FLASH_AREA); // ´ÓFLASHÀïÕÒ Êı¾İÎ»ÖÃ, ²¹·¢¿ªÊ¼µØÖ·, ²¹·¢Êı¾İĞÅÏ¢µØÖ·,

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
    	ÅĞ¶ÏÉÏÃæµÃµ½µÄÊµÊ±Êı¾İ¶ÁĞ´FLASHµØÖ·Ö®ºóµÄFLSAHÊÇ·ñ¿ÉÒÔĞ´,
    	Èô²»¿ÉÒÔĞ´,ÔòÌøµ½ÏÂÒ»¸ösecter, Èô¿ÉÒÔĞ´Ôò½Ó×ÅĞ´.
    	×¢Òâ:	FLASHĞ´Ö®Ç°»á²Á³ı,ÊÇFF²Å¿ÉÒÔĞ´, Ğ´Ò»¸öĞÂµÄsecterÊ±»áÏÈ²Á³ı¸ÃÉÈÇøÔÙĞ´!
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
                    realtime_data_wr_flash_addr = ((realtime_data_wr_flash_addr / FLASH_SECTOR_SIZE) + 1)*FLASH_SECTOR_SIZE;  //Èç¹û·Ç¿Õ£¬¿ªÊ¼´ÓÏÂÒ»¸ösecter¿ªÊ¼¡£
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
					GB17691_ack_send(CtlID[i],CtlIDNo[i],CON_SUCCESS,0);  //¸´Î»³É¹¦
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
                BKP_WriteBkpData(BKP_DAT6, 0xa5a5);  //µÚÒ»´ÎĞ£Ê±³É¹¦
            }
            else
            {
                BKP_WriteBkpData(BKP_DAT6, 0x5a5a);  //µÚÒ»´ÎĞ£Ê±³É¹¦
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
		
		ps=ReadPublickey();														//¹«Ô¿
		if(ps==NULL)
			ps=ReadPublickey();														//¹«Ô¿	
		
		if(ps!=NULL)
			lkt4305_state=1;
	
}
static void check_big_heart(void)
{		
		uint32_t new_time_stamp;
    new_time_stamp = get_time_stamp();
	
		if(BKP_ReadBkpData(BKP_DAT5) == 0x5a5a)     //µÚÒ»´ÎÉÏµç ´óĞÄÌø
		{
//				big_heart_flag=1;
				BKP_WriteBkpData(BKP_DAT5, 0xa5a5);
		}
				if(BKP_ReadBkpData(BKP_DAT2) == 0xa5a5)
					{
//							if((new_time_stamp - big_heart_time_stamp)>86400)  //Ê±¼ä¼ä¸ô24Ğ¡Ê±
//	//							if((new_time_stamp - big_heart_time_stamp)>600)  //Ê±¼ä¼ä¸ô10·ÖÖÓ
//							{
////USARTSendOut(USART1, "\r\n£¡£¡£¡11£¡£¡£¡£¡£¡£¡£¡£¡\r\n", strlen("\r\n£¡£¡£¡11£¡£¡£¡£¡£¡£¡£¡£¡\r\n"));
//									big_heart_flag=1;
//						//			big_heart_temp_time_stamp = new_time_stamp;
//									big_heart_time_stamp = new_time_stamp;
//							}
					}
}
void Get_SM2id(void)
{
		u8 IMEI_data[15];
		if(get_idstate(SM2ID)==1)				// IDÒÑ´æÈëflash
		{
//			while(Set4305_Sm2id()!=1)			//ÉèÖÃSM2 ID
//			{
//				if(j%5==4)
//					LKT4305Init();									//Á¬Ğø5´ÎÊ§°Ü ¸´Î»°²È«Ğ¾Æ¬
//				j++;
//				osDelay(30);
//			}
		}
		else						//ID Î´´æÈëFLASH  µÈ´ıIMEI¶ÁÈ¡³É¹¦ºóÉú³ÉID´æÈëflash
		{
			while(!get_imei(&IMEI_data[0]))					//»ñÈ¡µ½IMEI
			{	
				iwdg_17691++;	
				osDelay(100);
			}
			SM2ID[0]=ID_BOOL0;
			SM2ID[1]=ID_BOOL1;
			SM2ID[2]=ID_BOOL2;
			memcpy(&SM2ID[3],&IMEI_data[2],13);
			set_idstate(SM2ID);								// ½«ID´æÈëFLASH				
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
					const char DF_KaiPuTe_VIN[8]  ="LGDCH91G";  //¶«·ç-¿­ÆÕÌØ
					const char JN_50ling_VIN[8]   ="LWLDAA5G";  //¼ÃÄÏ-50Áå

					uint8_t  res , res_1, res_2,res_3,res_4,res_5;
					res   = memcmp(vin_info,RESET_VIN  ,sizeof(vin_info));
					res_1 = memcmp(vin_info,RESET_VIN_1,sizeof(vin_info));
					if((!res)||(!res_1))              //»Ø¸´³ö³§ÉèÖÃ
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
					else   //ÉèÖÃVIN ÅĞ¶Ï³µĞÍ
					{
						res   = memcmp(vin_info,DATONG_VIN_1,3);
						res_1  = memcmp(vin_info,DATONG_VIN_2,3);
            if((!res)||(!res_1))	 //ÅĞ¶ÏÊÇ²»ÊÇ´óÍ¨
						{							
							SetUdsProtocol(DaTong_9);	
							SetUdsPara(_DCUID,9);
							Set_evads(8,1);
							flag_UDSSEND=1;
						}
						else
						{							
							res   = memcmp(vin_info,WUSHILING_VIN_1,4);
							if(!res)  // ÅĞ¶ÏÊÇ²»ÊÇÎåÊ®Áå
								{
									SetUdsProtocol(wushiling_17);	
									SetUdsPara(_DCUID,17);
									Set_evads(8,1);
									flag_UDSSEND=1;
								}
								else
								{
									res   = memcmp(vin_info,BENCHI_VIN_1,4);
									if(!res)  // ÅĞ¶ÏÊÇ²»ÊÇ±¼³Û
										{
											SetUdsProtocol(benchiact_18);	
											SetUdsPara(_DCUID,18);
											Set_evads(8,1);
											flag_UDSSEND=1;
										}	
									else
										{
											res   = memcmp(vin_info,DFH4250_VIN,8);
											if(!res)  // ÅĞ¶ÏÊÇ²»ÊÇ¶«·ç
												{
													SetUdsProtocol(DongFeng_2);	
													SetUdsPara(_DCUID,2);
													Set_evads(8,1);
													flag_UDSSEND=1;
												}	
											else
											{
											res   = memcmp(vin_info,JRLH_VIN,8);
											if(!res)  // ÅĞ¶ÏÊÇ²»ÊÇ¼¯ÈğÁªºÏ
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
													if((!res)||(!res_1)||(!res_2)||(!res_3)||(!res_4)||(!res_5)) // ÅĞ¶ÏÊÇ²»ÊÇ½­Áê
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
u32 randnum=0;        //Î±Ëæ»úÊı ¼ÆÊı  Éú²úÅÅ·ÅÊı¾İÊ¹ÓÃ
u16 GPS_LED_cnt=0;			//  LEDÏÔÊ¾ V1.8°æ
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
		
		
		record_state=get_recordstate();	// »ñÈ¡±¸°¸×´Ì¬
//		record_state=0;									
		//»ñÈ¡°²È«Ğ¾Æ¬ID
		Get_SM2id();
		
    vin_state=get_vin(vin_info);   	//»ñÈ¡VIN
	
		if(!vin_state)			//Èç¹ûÃ»ÓĞÉèÖÃVIN£¬²»·¢ËÍ±¸°¸ĞÅÏ¢
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
            if(gps_set_time() && (BKP_ReadBkpData(BKP_DAT6) == 0x5a5a))   //µÚÒ»´ÎGPSĞ£Ê±³É¹¦
            {
								big_heart_time_stamp=get_time_stamp();
                update_big_heart_time(get_time_stamp());
            }

					if((randnum%500==88)&&(alarm_state_flag !=0 ))		//5s·¢Ò»´Î  alarm_state_flag !=0 ->Ã»Ğ´vinµÃÖÕ¶Ë¶¼²»·¢ËÍ²ğ³ı±¨¾¯
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
								USARTSendOut(USART1, "\r\n·¢ËÍ²ğ³ı±¨¾¯\r\n", strlen("\r\n·¢ËÍ²ğ³ı±¨¾¯\r\n"));
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
    TERMINAL_SOFT_VER=GB17691_get_softver_Info();   // »ñÈ¡Éè±¸ÀàĞÍ Èí¼ş°æ±¾ºÅ
    if(BKP_ReadBkpData(BKP_DAT2) == 0xa5a5)
    {
        big_heart_time_stamp= ((uint32_t)BKP_ReadBkpData(BKP_DAT3) <<16 )|((uint32_t)BKP_ReadBkpData(BKP_DAT4));
        test_flag=0x00;
    }
    else
    {
        big_heart_time_stamp=get_time_stamp();        //µÚÒ»´ÎÉÏµç
        update_big_heart_time(get_time_stamp());
        BKP_WriteBkpData(BKP_DAT5, 0x5a5a);
        test_flag=0x01;
    }
    tid_GB17691_task = osThreadCreate (osThread(GB17691_task), NULL);
}









