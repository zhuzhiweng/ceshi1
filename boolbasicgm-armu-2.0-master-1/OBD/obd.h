#ifndef __OBD_H
#define __OBD_H	 
#include "sys.h"


#define FAULT_CODE_MAX_SIZE 25

#define PROTOCOL_NUM_CNT 5



#define  CAN_LED_ON        	    (PBout(0)=0) 
#define  CAN_LED_OFF            (PBout(0)=1) 
#define READ_CAN_LED   PBin(0)    

typedef enum
{
	Guide_Idle =0,
	Guide_Idle_YesF101=1,
	Guide_Idle_NoF101=2,
	AuxCan19_250_YesF101=3,
	AuxCan19_250_NoF101=4,
	AuxCan19_500_YesF101=5,
	AuxCan19_500_NoF101=6,
 AuxCan311_250_YesF101=7,
 AuxCan311_250_NoF101=8,
 AuxCan311_500_YesF101=9,
 AuxCan311_500_NoF101=10,
}GuideNum;

typedef enum
{
	ProtocolIdle=0,
	J1939_29_250=1,
	ISO15765_11_500=2,
	ISO15765_29_500=3,
	ISO15765_11_250=4,
	ISO15765_29_250=5
}ProtocolNum_t;

//typedef enum
//{
//	ISO15765_11_250=0,
//	ISO15765_29_250=1,
//	ISO15765_11_500=2,
//	ISO15765_29_500=3
//}ISO15765ProtocolNum_t;


typedef struct
{
	uint16_t sptClst:1;               //´ß»¯×ª»¯Æ÷¼à¿Ø
	uint16_t sptHeatedclst:1;         //¼ÓÈÈ´ß»¯×ª»¯Æ÷¼à¿Ø
	uint16_t sptEprtsys:1;            //Õô·¢ÏµÍ³¼à¿Ø
	uint16_t sptScdrairss:1;          //¶ş´Î¿ÕÆøÏµÍ³¼à¿Ø
	uint16_t sptAcsysrfgr:1;          //A/CÏµÍ³ÖÆÀä¼Á¼à¿Ø
	uint16_t sptEsgassens:1;          //ÅÅÆø´«¸ĞÆ÷¼à¿Ø
	uint16_t sptEsgassenshter:1;      //ÅÅÆø´«¸ĞÆ÷¼ÓÈÈÆ÷¼à¿Ø
	uint16_t sptEgrvvtsys:1;          //EGRÏµÍ³ºÍVVT¼à¿Ø
	uint16_t sptColdstartaid:1;       //ÀäÆô¶¯¸¨ÖúÏµÍ³¼à¿Ø
	uint16_t sptBoostpresctrlsys:1;   //ÔöÑ¹Ñ¹Á¦¿ØÖÆÏµÍ³
	uint16_t sptDpt:1;                //DPF¼à¿Ø
	uint16_t sptScrnox:1;             //Ñ¡ÔñĞÔ´ß»¯»¹Ô­ÏµÍ³(SCR)»òNOXÎü¸½Æ÷
	uint16_t sptNmhccvtnclst:1;       //NMHCÑõ»¯´ß»¯Æ÷¼à¿Ø
	uint16_t sptMisfire:1;            //Ê§»ğ¼à¿Ø
	uint16_t sptFuelsys:1;            //È¼ÓÍÏµÍ³¼à¿Ø
	uint16_t sptCphscpnt:1;           //×ÛºÏ³É·Ö¼à¿Ø
}OBD_dntSpt_t;

//Õï¶Ï¾ÍĞ÷×´Ì¬ Ã¿Ò»Î»µÄº¬Òå:0=²âÊÔÍê³É»òÕß²»Ö§³Ö;1=²âÊÔÎ´Íê³É
typedef struct
{
	uint16_t rdyClst:1;                //´ß»¯×ª»¯Æ÷¼à¿Ø
	uint16_t rdyHeatedclst:1;          //¼ÓÈÈ´ß»¯×ª»¯Æ÷¼à¿Ø
	uint16_t rdyEprtsys:1;             //Õô·¢ÏµÍ³¼à¿Ø
	uint16_t rdyScdrairss:1;           //¶ş´Î¿ÕÆøÏµÍ³¼à¿Ø
	uint16_t rdyAcsysrfgr:1;           //A/CÏµÍ³ÖÆÀä¼Á¼à¿Ø
	uint16_t rdyEsgassens:1;           //ÅÅÆø´«¸ĞÆ÷¼à¿Ø
	uint16_t rdyEsgassenshter:1;       //ÅÅÆø´«¸ĞÆ÷¼ÓÈÈÆ÷¼à¿Ø
	uint16_t rdyEgrvvtsys:1;           //EGRÏµÍ³ºÍVVT¼à¿Ø
	uint16_t rdyColdstartaid:1;        //ÀäÆô¶¯¸¨ÖúÏµÍ³¼à¿Ø
	uint16_t rdyBoostpresctrlsys:1;    //ÔöÑ¹Ñ¹Á¦¿ØÖÆÏµÍ³
	uint16_t rdyDpt:1;                 //DPF¼à¿Ø
	uint16_t rdyScrnox:1;              //Ñ¡ÔñĞÔ´ß»¯»¹Ô­ÏµÍ³(SCR)»òNOXÎü¸½Æ÷
	uint16_t rdyNmhccvtnclst:1;        //NMHCÑõ»¯´ß»¯Æ÷¼à¿Ø
	uint16_t rdyMisfire:1;             //Ê§»ğ¼à¿Ø
	uint16_t rdyFuelsys:1;             //È¼ÓÍÏµÍ³¼à¿Ø
	uint16_t rdyCphscpnt:1;            //×ÛºÏ³É·Ö¼à¿Ø
}OBD_dntReady_t;

//IUPRÖµÕë¶ÔÑ¹È¼Ê½·¢¶¯»ú
typedef struct
{
	uint16_t iuprOBDcond ;           //OBD¼à²âÌõ¼şÔËĞĞ¼ÆÊı
	uint16_t iuprLgncntr;            //µã»ğÑ­»·¼ÆÊıÆ÷
	uint16_t iuprHccatcomp;          //NMHC´ß»¯¼Á¼à²âÍê³É¼ÆÊı
	uint16_t iuprHccatcond;          //NMHC´ß»¯¼Á¼à²âÌõ¼şÔËĞĞ¼ÆÊı
	uint16_t iuprNcatcomp;           //NOX/SCR´ß»¯¼Á¼à²âÍê³ÉÌõ¼ş¼ÆÊı
	uint16_t iuprNcatcond;           //NOx/SCR´ß»¯¼Á¼à²âÌõ¼şÔËĞĞ¼ÆÊı
	uint16_t iuprNadscomp;           //NOxÎü¸½Æ÷¼à²âÆ÷Íê³ÉÌõ¼ş¼ÆÊı
	uint16_t iuprNadscond;           //NOxÎü¸½Æ÷¼à²âÌõ¼şÔËĞĞ¼ÆÊı
	uint16_t iuprPmcomp;             //PM¹ıÂËÆ÷¼àÊÓÆ÷Íê³ÉÌõ¼ş¼ÆÊı
	uint16_t iuprPmcond;             //PM¹ıÂËÆ÷¼à¿ØÌõ¼şÔËĞĞ¼ÆÊı
	uint16_t iuprEgscomp;            //·ÏÆø´«¸ĞÆ÷¼à¿ØÍê³É¼àÊÓ¼ÆÊı
	uint16_t iuprEgscond;            //·ÏÆø´«¸ĞÆ÷¼à¿ØÔËĞĞÌõ¼şÔËĞĞ¼ÆÊı
	uint16_t iuprEgrcomp;            //EGRºÍ/»òVVT¼à²âÍê³ÉÌõ¼ş¼ÆÊı
	uint16_t iuprEgrcond;            //EGRºÍ/»òVVT¼à²âÌõ¼şÔËĞĞ¼ÆÊı
	uint16_t iuprBpcomp;             //ÌáÉıÑ¹Á¦¼à²âÍê³ÉÌõ¼ş¼ÆÊı
	uint16_t iuprBpcond;             //ÔöÑ¹Ñ¹Á¦¼à²âÌõ¼şÔËĞĞ¼ÆÊı
	uint16_t iuprFuelcomp;           //È¼ÁÏ¼à²âÍê³ÉÌõ¼ş¼ÆÊı
	uint16_t iuprFuelcond;           //È¼ÁÏ¼à²âÌõ¼şÔËĞĞ¼ÆÊı
}OBD_IUPR_t;


typedef  struct
{
	uint32_t mileage;                           //ÀÛ¼ÆÀï³Ì
	uint16_t speed;                             //³µËÙ
	uint8_t liquidLevel;                        //ÓÍÏäÒºÎ»
  uint8_t softCbrtNum[18];                    //Èí¼ş±ê¶¨Ê¶±ğºÅ
  uint8_t CVN[18];                            //±ê¶¨ÑéÖ¤Âë
  uint8_t VIN[17];                            //³µÁ¾Ê¶±ğÂë
}OBD_Vehicle_Infor_t;


typedef  struct
{
	 uint8_t obdDntPtc;                          //obdÕï¶ÏĞ­Òé
	 uint8_t MIL_state;                          //MIL×´Ì¬
	 OBD_dntSpt_t dntSptStatus;                  //Õï¶ÏÖ§³Ö×´Ì¬
	 OBD_dntReady_t dntReadyStatus;              //Õï¶Ï¾ÍĞ÷×´Ì¬
   OBD_IUPR_t iupr;                            //IUPRÖµ
   uint8_t faultCodeSum;                       //¹ÊÕÏÂë×ÜÊı
   uint8_t faultCode[25][4];  // ¹ÊÕÏÂëĞÅÏ¢ÁĞ±í
}OBD_Diagnosis_Infor_t;


typedef  struct
{
  uint16_t engineRev;                        //·¢¶¯»ú×ªËÙ
  uint16_t enginefuelFlow;                   //·¢¶¯»úÈ¼ÁÏÁ÷Á¿ ----
	uint16_t airInflow;                        //½øÆøÁ¿----
	uint8_t  atmoPres;                         //´óÆøÑ¹Á¦
  uint8_t  engineTorq;                       //·¢¶¯»úÅ¤¾Ø----
  uint8_t  fricTorq;                         //Ä¦²ÁÅ¤¾Ø
	uint8_t coolantTemp;                       //·¢¶¯»úÀäÈ´ÒºÎÂ¶È
  uint8_t  calcLoad;                         //¼ÆËã¸ººÉ
	uint8_t		model_Torq;												//Å¤¾ØÄ£Ê½--ÉÏº£
	uint8_t		position_pedal;												//ÓÍÃÅÌ¤°å	--ÉÏº£
	uint32_t		totalfuel;												//×ÜÓÍºÄ--ÉÏº£
	
}OBD_Engine_Infor_t;

typedef  struct
{
	uint16_t inLetTemp;                        //SCRÈë¿ÚÎÂ¶È
	uint16_t outLetTemp;                       //SCR³ö¿ÚÎÂ¶È
  uint16_t upstSensValue;                    //SCRÉÏÓÎNOX´«¸GÆ÷Êä³öÖµ
  uint16_t dnstSensValue;                    //SCRÏÂÓÎNOx´«¸GÆ÷Êä³öÖµ
	uint16_t diffPres;                         //DPFÑ¹²î
  uint16_t DiffTemp;                         //DPFºóÎÂ
  uint8_t  NOxInvertEff;                     //NOx×ª»¯G§ÂÊ
	uint8_t  elecCDISatus;                     //µç¼ÓÈÈµã»g×´Ì¬
	uint8_t  fuleInjectSatus;                  //È¼ÉÕÆ÷ÅçÓÍ×´Ì¬
	uint16_t fuelrate;                 			 	 //È¼ÉÕÆ÷ÅçÓÍÁ¿
  uint8_t  reagAllowance;                    //·´Ó¦¼ÁÓàÁ¿
  uint8_t  CatalystTemp;                    //ÄòËØÏäÎÂ¶È
  uint32_t	DosingReag;												//ÄòËØµ±Ç°ÅçÉäÁ¿--ÉÏº£
  uint32_t	TotalReagCom;												//ÄòËØ×ÜÏûºÄÁ¿--ÉÏº£
	uint16_t  PMdata;																		//¿ÅÁ£ÎïÅ¨¶È --Ö£Öİ
	uint16_t PM_lightabs;												//¹âÎüÊÕÏµÊı
	uint16_t PM_opaticy;														//²»Í¸¹â¶È
}OBD_Aftertreatment_Infor_t;
    
typedef struct
{
	u8 total_frame_no;				//¶àÖ¡µÄ×Ü°üÊı 
	u8 valid_byte_no;					//ÓĞĞ§×Ö½Ú³¤¶È 
	u8 remaining_space;				//Ê£ÓàµÄ×Ö½Ú´óĞ¡
	u8 success_no;						//ÊÇ·ñ½ÓÊÕÍêÕû
	u8 items_no;							//´Ë¶à°üµÄÔÚ09Ä£Ê½ÏÂÊÇÏîÄ¿Êı  ÔÚ01Ä£Ê½ÏÂÊÇA×Ö½Ú
	//ÒÔÏÂÊÇ03ºÍ07Ä£Ê½Ê¹ÓÃµÄ
	u8 DTC_NO;						//´Ë¶à°üµÄÔÚ03»òÕß07Ä£Ê½ÏÂÊÇ¹ÊÕÏÊı

} ReMessage_stateCanTxMsg_t;

void Can_clear(void);

void  get_OBD_Vehicle_Infor_t(  OBD_Vehicle_Infor_t  *p );
void  get_OBD_Diagnosis_Infor_t(  OBD_Diagnosis_Infor_t  *p );
void  get_OBD_Engine_Infor_t(  OBD_Engine_Infor_t *p );
void  get_OBD_Aftertreatment_Infor_t(  OBD_Aftertreatment_Infor_t  *p );
void Obd_init(void);	

u32 Count_FUN(u16 *p,u32 count);

extern  OBD_Engine_Infor_t 	OBD_engine_infor;
extern  OBD_Vehicle_Infor_t OBD_vehicle_infor;
extern  OBD_Diagnosis_Infor_t  OBD_diagnosis_infor;
extern  OBD_Aftertreatment_Infor_t OBD_aftertreatment_infor;
//extern u8 temp_obd_group[150];

extern void can_collection(void);
extern uint8_t iso_15765_canAdd_Blacklist_judge(uint32_t canid);
extern u8  judge_is_ASCII(char * p,u16 P_NO);			//P Ö¸Õë  P_NOÎª×Ö½ÚÊı  //·µ»Ø0 Îª²»È«ÊÇasckII  1 ±íÊ¾È«²¿ÊÇascII

uint8_t GetProtocolNum(void);
#endif




















