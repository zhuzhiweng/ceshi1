#ifndef _GB17691A_H_
#define _GB17691A_H_

#include "sys.h"
#include "stdio.h"
#include <string.h>
//#include "FreeRTOS.h"
//#include "task.h"
#include "includes.h"

#include "delay.h"
#include "RTC.h"
#include "GPS_calculation.h"
#include "sim_uart.h"
#include "sim_net.h"
#include "spi_flash.h"
#include "gps.h"
//#include "key_and_charge.h"
//#include "usb_interface.h"
#include "GB17691_CAN_GPS.h"
#include "user_config.h"

#define ACTBLU_START_STRING   0x2424                 //°¬¿ÉÀ¶Ğ­ÒéÆğÊ¼·ûACTBLU

#define START_STRING       0x2323                    //ÆğÊ¼·û
#define VEHICLE_LOGIN_CMD  0x01                      //³µÁ¾µÇÈëÃüÁî
#define VEHICLE_UPLOAD_CMD 0x02                      //³µÁ¾ÊµÊ±ÉÏ±¨ÃüÁî
#define VEHICLE_RELOAD_CMD 0x03                      //³µÁ¾²¹·¢GÅÏ¢ÉÏ±¨ÃüÁî
#define VEHICLE_LOGOUT_CMD 0x04                      //³µÁ¾µÇ³öÃüÁî
#define VEHICLE_TIMING_CMD 0x05                      //³µÁ¾G£Ê±ÃüÁî
//#define VEHICLE_HEART_CMD  0x07                      //³µÁ¾GÄÌøÃüÁ

#define VEHICLE_CONTROL_CMD 0x0C                      //Æ½Ì¨¿ØÖÆÃüÁîÏÂ·¢
#define VEHICLE_ACKCON_CMD 0x0D                      //ÖÕ¶Ë¿ØÖÆÃüÁî»Ø¸´î

#define ZHENGZHOU_RECORD_CMD  0x07                      // Ö£ÖI±¸°¸ÃüÁî
#define ZHENGZHOU_TIMING_CMD  0xC0                      // Ö£ÖIG£Ê±ÃüÁî
#define ZHENGZHOU_LOGIN_CMD  0xC2                      //  Ö£ÖIµÇÂ¼ÃüÁî

#define ZHENGZHOU_RECORD_ACK_CMD  0x08                      // Ö£ÖI±¸°¸Ó¦´g
#define ZHENGZHOU_TIMING_ACK_CMD  0xC1                      // Ö£ÖIG£Ê±Ó¦´g
#define ZHENGZHOU_LOGIN_ACK_CMD  0xC3                      //  Ö£ÖIµÇÂ¼Ó¦´g
#define ZHENGZHOU_UPLOAD_ACK_CMD  0xF0                      //  Ö£ÖIÉÏ±¨Ó¦´g

#define VEHICLE_LOGIN_SUCCESS_CMD 0x81               //³µÁ¾µÇÈë³É¹¦Ó¦´g
#define VEHICLE_LOGIN_ERROR_CMD 0x91                 //³µÁ¾µÇÈëÊ§°ÜÓ¦´g
#define VEHICLE_LOGOUT_SUCCESS_CMD 0x84              //³µÁ¾µÇ³ö³É¹¦Ó¦´g
#define VEHICLE_LOGOUT_ERROR_CMD 0x94                //³µÁ¾µÇ³öÊ§°ÜÓ¦´g
#define VEHICLE_TIMING_SUCCESS_CMD 0x85              //³µÁ¾G£Ê±³É¹¦Ó¦´g 
#define VEHICLE_TIMING_ERROR_CMD 0x95                //³µÁ¾G£Ê±Ê§°ÜÓ¦´g
#define VEHICLE_HEART_SUCCESS_CMD 0x87               //³µÁ¾GÄÌø³É¹¦Ó¦´g
#define VEHICLE_HEART_ERROR_CMD 0x97                 //³µÁ¾GÄÌøÊ§°ÜÓ¦´g

//#define VEHICLE_SET_VIN_CMD 0xBF                      //ÉèÖÃVINÃüÁî
//#define TERMINAL_SOFT_VER 01                           //01¿ªÊ¼µIÔö  ×î´ó256



#define NO_ENCRYPT  0x01                              //Êi¾I²»¼ÓÃÜ
#define SM2_ENCRYPT  0x03                              //Êi¾ISM2¼ÓÃÜ

#define ID_BOOL0 'D'
#define ID_BOOL1 'V'
#define ID_BOOL2 'U'

//GÅÏ¢ÀàGÍ±êÖ¾¶¨Òå
#define GB17691_INFO_OBD_DATA          0x01    //OBDGÅÏ¢Ìå±êÖ¾
#define GB17691_INFO_DATASTREAM_DATA   0x02    //Êi¾IÁ÷Êi¾I±êÖ¾
#define GB17691_INFO_CUSTOM_DATA       0x80    //×Ô¶¨ÒåÊi¾I±êÖ
#define GB17691_INFO_IMEI_DATA         0x81    //IMEIÊi¾I

#define GB17691_THREAD_DEALY  10               //Ïß³ÌÑÓÊ±
#define GB17691_CMD_CHECK_DEALY  (1000/GB17691_THREAD_DEALY) //ÃüÁî¼ì²éÑÓÊ±

#define REALTIME_UPLOAD_FLAG 0xffff   //ÕiÔÚÊµÊ±·¢ËÍµÄÊi¾I
#define RESEND_DATA_FLAG  0xfff0      //GèÒª²¹·¢µÄÊi¾I
#define SEND_DATA_FLAG    0xff00      //·¢ËÍ³É¹¦µÄÊi¾I

#define REAL_TIME_UPLOAD 0x01         //ÊµÊ±ÉÏ±¨
#define SAVE_RESEND_DATA 0x02         //±£´æ²¹·¢Êi¾I
//#define SAVE_ALARM_DATA  0x04

#define TERMINAL_FREE   0x00          //ÖÕ¶ËÊÍ·ÅÄ£Ê½
#define TERMINAL_NET    0x01          //ÖÕ¶ËÍøÂçÄ£Ê½
//#define TERMINAL_USB    0x02

#define LOGIN_MAX_CNT    5            //5->3µÇÂ¼×î´ó¼ÆÊi
#define LOGIN_INTERVAL   5           //µÇÂ¼¼ä¸ô60S

#define TIME_SEND_CNT    5             //5->3G£Ê±·¢ËÍ×î´ó¼ÆÊi
#define TIME_SEND_INTERVAL 5          //G£Ê±·¢ËÍ¼ä¸ô60S

#define RECORD_SEND_CNT    10             //5->3G£Ê±·¢ËÍ×î´ó¼ÆÊi
#define RECORD_SEND_INTERVAL 20          //G£Ê±·¢ËÍ¼ä¸ô60S

#define RESEND_WRITE_MAX_SIZE (0x200000)   //²¹·¢G´×î´ó´óG¡2M

#define HEART_INTERVAL 50              //GÄÌø¼ä¸ô50S


#define FRAME_CNT_PER_PACKAGE 1        //Ã¿°üÖ¡¼ÆÊi
#if FRAME_CNT_PER_PACKAGE == 1
#define RESND_MSG_MAX_CNT  60         //²¹·¢×î´ó¼ÆÊi
#elif FRAME_CNT_PER_PACKAGE == 2
#define RESND_MSG_MAX_CNT  80
#elif FRAME_CNT_PER_PACKAGE == 3
#define RESND_MSG_MAX_CNT  90
#else
#define RESND_MSG_MAX_CNT  100
#endif
//#define RESND_MSG_MAX_CNT ((600/MSG_SEND_INTERVAL)*FRAME_CNT_PER_PACKAGE)


#define TIME_PRESET_VALUE    0        //Ê±¼äÔ¤ÉèÖµ6Ãë
#define MSG_SEND_INTERVAL  30

#define  GPS_LED_ON        	    (PAout(3)=1) 
#define  GPS_LED_OFF            (PAout(3)=0) 
#define READ_GPS_LED   PAin(3)    



typedef enum
{
    GB17691_IDLE,
    GB17691_LOGIN,
    GB17691_TIMING,
    GB17691_HEART,
    GB17691_LOGOUT
} GB17691_terminal_state_t;          //ÖÕ¶Ë×´Ì¬

#pragma pack(1)  //°´×Ö½Ú¶ÔÆë
typedef struct
{
    uint32_t timestamp;
    uint8_t  package_cnt;
    uint8_t  package_num;
    uint16_t resend_flag;
} save_resend_data_t;
#pragma pack()   //È¡Ïû°´×Ö½Ú¶ÔÆë »Ö¸´Ê¡È±¶ÔÆë

////#pragma pack(1)  //°´×Ö½Ú¶ÔÆë
////Õï¶ÏÖ§³Ö×´Ì¬ Ã¿Ò»Î»µÄº¬Òå:0=²»Ö§³Ö;1=Ö§³Ö
//typedef struct
//{
//	uint16_t sptClst:1;               //´ß»¯×ª»¯Æ÷¼à¿Ø
//	uint16_t sptHeatedclst:1;         //¼ÓÈÈ´ß»¯×ª»¯Æ÷¼à¿Ø
//	uint16_t sptEprtsys:1;            //Õô·¢ÏµÍ³¼à¿Ø
//	uint16_t sptScdrairss:1;          //¶s´Î¿ÕÆøÏµÍ³¼à¿Ø
//	uint16_t sptAcsysrfgr:1;          //A/CÏµÍ³ÖÆÀä¼Á¼à¿Ø
//	uint16_t sptEsgassens:1;          //ÅÅÆø´«¸GÆ÷¼à¿Ø
//	uint16_t sptEsgassenshter:1;      //ÅÅÆø´«¸GÆ÷¼ÓÈÈÆ÷¼à¿Ø
//	uint16_t sptEgrvvtsys:1;          //EGRÏµÍ³ºÍVVT¼à¿Ø
//	uint16_t sptColdstartaid:1;       //ÀäÆô¶¯¸¨ÖúÏµÍ³¼à¿Ø
//	uint16_t sptBoostpresctrlsys:1;   //ÔöÑ¹Ñ¹Á¦¿ØÖÆÏµÍ³
//	uint16_t sptDpt:1;                //DPF¼à¿Ø
//	uint16_t sptScrnox:1;             //Ñ¡ÔñGÔ´ß»¯»¹Ô­ÏµÍ³(SCR)»òNOXÎü¸½Æ÷
//	uint16_t sptNmhccvtnclst:1;       //NMHCÑõ»¯´ß»¯Æ÷¼à¿Ø
//	uint16_t sptMisfire:1;            //Ê§»g¼à¿Ø
//	uint16_t sptFuelsys:1;            //È¼ÓÍÏµÍ³¼à¿Ø
//	uint16_t sptCphscpnt:1;           //×ÛºÏ³É·Ö¼à¿Ø
//}GB17691_OBD_dntSpt_t;

////Õï¶Ï¾ÍG÷×´Ì¬ Ã¿Ò»Î»µÄº¬Òå:0=²âÊÔÍê³É»òÕß²»Ö§³Ö;1=²âÊÔÎ´Íê³É
//typedef struct
//{
//	uint16_t rdyClst:1;                //´ß»¯×ª»¯Æ÷¼à¿Ø
//	uint16_t rdyHeatedclst:1;          //¼ÓÈÈ´ß»¯×ª»¯Æ÷¼à¿Ø
//	uint16_t rdyEprtsys:1;             //Õô·¢ÏµÍ³¼à¿Ø
//	uint16_t rdyScdrairss:1;           //¶s´Î¿ÕÆøÏµÍ³¼à¿Ø
//	uint16_t rdyAcsysrfgr:1;           //A/CÏµÍ³ÖÆÀä¼Á¼à¿Ø
//	uint16_t rdyEsgassens:1;           //ÅÅÆø´«¸GÆ÷¼à¿Ø
//	uint16_t rdyEsgassenshter:1;       //ÅÅÆø´«¸GÆ÷¼ÓÈÈÆ÷¼à¿Ø
//	uint16_t rdyEgrvvtsys:1;           //EGRÏµÍ³ºÍVVT¼à¿Ø
//	uint16_t rdyColdstartaid:1;        //ÀäÆô¶¯¸¨ÖúÏµÍ³¼à¿Ø
//	uint16_t rdyBoostpresctrlsys:1;    //ÔöÑ¹Ñ¹Á¦¿ØÖÆÏµÍ³
//	uint16_t rdyDpt:1;                 //DPF¼à¿Ø
//	uint16_t rdyScrnox:1;              //Ñ¡ÔñGÔ´ß»¯»¹Ô­ÏµÍ³(SCR)»òNOXÎü¸½Æ÷
//	uint16_t rdyNmhccvtnclst:1;        //NMHCÑõ»¯´ß»¯Æ÷¼à¿Ø
//	uint16_t rdyMisfire:1;             //Ê§»g¼à¿Ø
//	uint16_t rdyFuelsys:1;             //È¼ÓÍÏµÍ³¼à¿Ø
//	uint16_t rdyCphscpnt:1;            //×ÛºÏ³É·Ö¼à¿Ø
//}GB17691_OBD_dntReady_t;

////IUPRÖµÕë¶ÔÑ¹È¼Ê½·¢¶¯»ú
//typedef struct
//{
//	uint16_t iuprOBDcond ;           //OBD¼à²âÌõ¼sÔËGG¼ÆÊi
//	uint16_t iuprLgncntr;            //µã»gÑ­»·¼ÆÊiÆ÷
//	uint16_t iuprHccatcomp;          //NMHC´ß»¯¼Á¼à²âÍê³É¼ÆÊi
//	uint16_t iuprHccatcond;          //NMHC´ß»¯¼Á¼à²âÌõ¼sÔËGG¼ÆÊi
//	uint16_t iuprNcatcomp;           //NOX/SCR´ß»¯¼Á¼à²âÍê³ÉÌõ¼s¼ÆÊi
//	uint16_t iuprNcatcond;           //NOx/SCR´ß»¯¼Á¼à²âÌõ¼sÔËGG¼ÆÊi
//	uint16_t iuprNadscomp;           //NOxÎü¸½Æ÷¼à²âÆ÷Íê³ÉÌõ¼s¼ÆÊi
//	uint16_t iuprNadscond;           //NOxÎü¸½Æ÷¼à²âÌõ¼sÔËGG¼ÆÊi
//	uint16_t iuprPmcomp;             //PM¹iÂËÆ÷¼àÊÓÆ÷Íê³ÉÌõ¼s¼ÆÊi
//	uint16_t iuprPmcond;             //PM¹iÂËÆ÷¼à¿ØÌõ¼sÔËGG¼ÆÊi
//	uint16_t iuprEgscomp;            //·ÏÆø´«¸GÆ÷¼à¿ØÍê³É¼àÊÓ¼ÆÊi
//	uint16_t iuprEgscond;            //·ÏÆø´«¸GÆ÷¼à¿ØÔËGGÌõ¼sÔËGG¼ÆÊi
//	uint16_t iuprEgrcomp;            //EGRºÍ/»òVVT¼à²âÍê³ÉÌõ¼s¼ÆÊi
//	uint16_t iuprEgrcond;            //EGRºÍ/»òVVT¼à²âÌõ¼sÔËGG¼ÆÊi
//	uint16_t iuprBpcomp;             //ÌáÉiÑ¹Á¦¼à²âÍê³ÉÌõ¼s¼ÆÊi
//	uint16_t iuprBpcond;             //ÔöÑ¹Ñ¹Á¦¼à²âÌõ¼sÔËGG¼ÆÊi
//	uint16_t iuprFuelcomp;           //È¼ÁÏ¼à²âÍê³ÉÌõ¼s¼ÆÊi
//	uint16_t iuprFuelcond;           //È¼ÁÏ¼à²âÌõ¼sÔËGG¼ÆÊi

//}GB17691_OBD_IUPR_t;

//typedef struct
//{
//	uint32_t faultCode1;            //¹ÊÕÏ´úÂë1
//	uint32_t faultCode2;            //¹ÊÕÏ´úÂë2
//  uint32_t faultCode3;            //¹ÊÕÏ´úÂë3
//	uint32_t faultCode4;            //¹ÊÕÏ´úÂë4
//	uint32_t faultCode5;            //¹ÊÕÏ´úÂë5
//}GB17691_OBD_faultCodelist_t;

//typedef  struct
//{
//  uint8_t obdDntPtc;                          //obdÕï¶ÏG­Òé
//  uint8_t MIL_state;                          //MIL×´Ì¬
//  GB17691_OBD_dntSpt_t dntSptStatus;          //Õï¶ÏÖ§³Ö×´Ì¬
//  GB17691_OBD_dntReady_t dntReadyStatus;      //Õï¶Ï¾ÍG÷×´Ì¬
//  uint8_t VIN[17];                            //³µÁ¾Ê¶±gÂë
//  uint8_t softCbrtNum[18];                    //Èí¼s±ê¶¨Ê¶±gºÅ
//  uint8_t CVN[18];                            //±ê¶¨ÑéÖ¤Âë
//  GB17691_OBD_IUPR_t iupr;                    //IUPRÖµ
//  uint8_t faultCodeSum;                       //¹ÊÕÏÂë×ÜÊi
//  GB17691_OBD_faultCodelist_t faultCodelist;  // ¹ÊÕÏÂëGÅÏ¢ÁG±í
//}GB17691_obd_data_t;

//typedef struct
//{
//	  uint8_t is_valid:1;
//		uint8_t NorS:1;
//		uint8_t EorW:1;
//		uint8_t reserve:5;
//}GB17691_locationStatus_t;

//typedef  struct
//{
//  uint16_t speed;                            //³µËÙ
//  uint8_t  atmoPres;                         //´óÆøÑ¹Á¦
//  uint8_t  engineTorq;                       //·¢¶¯»úÅ¤¾Ø
//  uint8_t  fricTorq;                         //Ä¦²ÁÅ¤¾Ø
//  uint16_t engineRev;                        //·¢¶¯»ú×ªËÙ
//  uint16_t enginefuelFlow;                   //·¢¶¯»úÈ¼ÁÏÁ÷Á¿
//  uint16_t upstSensValue;                    //SCRÉÏÓÎNOX´«¸GÆ÷Êä³öÖµ
//  uint16_t dnstSensValue;                    //SCRÏÂÓÎNOx´«¸GÆ÷Êä³öÖµ
//  uint8_t  reagAllowance;                    //·´Ó¦¼ÁÓàÁ¿
//	uint16_t airInflow;                        //½øÆøÁ¿
//	uint16_t inLetTemp;                        //SCRÈë¿ÚÎÂ¶È
//	uint16_t outLetTemp;                       //SCR³ö¿ÚÎÂ¶È
//	uint16_t diffPres;                         //DPFÑ¹²î
//	uint8_t coolantTemp;                       //·¢¶¯»úÀäÈ´ÒºÎÂ¶È
//	uint8_t liquidLevel;                       //ÓÍÏäÒºÎ»
//	GB17691_locationStatus_t locationStatus;   //¶¨Î»×´Ì¬
//	uint8_t Ing[4];                            //¾«¶È
//	uint8_t Lat[4];                            //Î³¶È
//	uint32_t mileage;                          //ÀÛ¼ÆÀï³Ì
//}GB17691_datastream_data_t;

//typedef  struct
//{
//  uint16_t datalen;                          //×Ô¶¨ÒåGÅÏ¢Ìå³¤¶È
//  uint8_t  calcLoad;                         //¼ÆÊi¸ººÉ
//  uint16_t DiffTemp;                         //DPFºóÎÂ
//  uint8_t  NOxInvertEff;                     //NOx×ª»¯G§ÂÊ
//	uint8_t  elecCDISatus;                     //µç¼ÓÈÈµã»g×´Ì¬
//	uint8_t  fuleInjectSatus;                  //È¼ÓÍÅçÉä×´Ì¬
//	uint16_t ASL;                              //º£²¦
//}GB17691_custom_data_t;

//#pragma pack()   //È¡Ïû°´×Ö½Ú¶ÔÆë »Ö¸´Ê¡È±¶ÔÆë

//ÈÎÎñÓÅÏÈ¼¶
#define GB17691_TASK_PRIO		4
//ÈÎÎñ¶ÑÕ»´óG¡
#define GB17691_STK_SIZE 	1024

uint8_t EvalBCC_FromBytes(uint8_t* Bytes, uint16_t len);   //Òì»òG£Ñé
void GB17691_CMD_analysis(uint8_t *pdata, uint16_t len);   //GB17691ÃüÁî½âÎö
void login_send(void);                                     //·¢ËÍµÇÈë
void logout_send(void);                                    //·¢ËÍµÇ³ö
void GB17691_CMD_check(void);                              //ÃüÁî¼ì²é
void GB17691_task(void const *pvParameters);               //GB17691ÈÎÎñ
//void GB17691_task(void);                                 //GB17691ÈÎÎñ
void GB17691_init(void);                                   //GB17691³õÊ¼»¯
void GB17691_task_init(void);
void GB17691CheckCmd(void);
uint8_t get_lkt4305_state(void);
void LKT4305_Key_Init(void);
void Get_SM2id(void);
uint8_t get_big_heart_flag(void);
uint8_t get_login_state(void);
uint8_t* read_msg_from_flash(uint32_t* addr, uint32_t* resend_info_addr,uint8_t flash_area);


u16 get_AIKELAN_datapack(uint8_t  *p_datastream);//°¬¿ÉÀ¶Êı¾İ´ò°ü
void GB17691_AIKELAN_CheckCmd(void);//°¬¿ÉÀ¶Êı¾İĞ£Ñé
#endif


