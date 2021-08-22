#include "can.h"
#include "delay.h"
#include "obd.h"
#include "j_1939.h"
#include "iso_15765.h"

#include "GB17691_CAN_GPS.h"
#include "define.h"
#include "n32g45x_can.h"
#include "cmsis_os.h" 
#include "UART.h"
#include "bsp.h"
#include "spi_flash.h"
#include "CanDataAnalysis.h"
#include "n32g45x_bkp.h"

#include "UDS.h"

extern uint8_t PenaltyZoneFlag;
//static ProtocolNum_t pro_sn = ProtocolIdle;		//³µÔØÐ­ÒéµÄ±êÊ¶ 0-ÎÞÐ­Òé  1,1930  2,11/500  3,29/500  4,11/250  5,29/250  
ProtocolNum_t ISO5765ProNum;
static uint8_t J1939ProState;
static uint16_t ObdCanBaud;
uint8_t can_state = 0xFF; //0£ºÐÝÃß 0xFF£ºÆ¥ÅäÐ­ÒéÖÐ 1£ºCAN¹¤×÷Õý³£

uint8_t guide = 0;
GuideNum guide_Num ;

OBD_Diagnosis_Infor_t  OBD_diagnosis_infor;
OBD_Vehicle_Infor_t OBD_vehicle_infor;
OBD_Engine_Infor_t 	OBD_engine_infor;
OBD_Aftertreatment_Infor_t OBD_aftertreatment_infor;

//static uint8_t CheckProNum;  //¸ß4Î»Ê±¿ÉÄÜµÄÐ­ÒéºÅ£¬µÍ4Î»ÊÇ¼ì²âµÄÐ­ÒéºÅ
static uint8_t CheckProState; 
static uint8_t CheckProDelay;
static uint8_t CheckProDelayCnt;
static uint8_t CheckProSucCnt;

static uint16_t CheckEpmDelay;
static uint16_t OldEngineSpeed;

static uint8_t AuxiliaryCanState = 0; /*0:Æ¥Åä£¬1:Æ¥Åä³É¹¦£¬2:Æ¥ÅäÊ§°Ü*/
static uint8_t AuxiliaryCanPro = 0;
static uint8_t AuxiliaryCanMatchStep = 0;
static uint8_t AuxiliaryCanMatchCnt = 0; /*Æ¥ÅäÖÜÆÚÊý*/
static uint16_t AuxiliaryCanMatchDealy = 0;

uint8_t flag_UDSSEND=0;	

static uint32_t ObdLoopCnt;
static uint8_t flag_only1939=0;

u32 iwdg_obd=0;
extern JingLi_Sensor jingli_sensor;
extern XinLi_Sensor xinli_sensor;
extern TianDiCheRen_Sensor tiandicheren_sensor;
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//PÎªbufferÆðÊ¼µØÖ·
//P_NOÎªbuffer³¤¶È
//ÇÒ»á½«ÆäÖÐµÄ·ÇÊý×ÖºÍ×ÖÄ¸²¿·ÖÌæ»»³É *
//·µ»ØÖµÎª1ÎªÈ«²¿ÎªÊý×ÖºÍ×ÖÄ¸
//				0Ê±ËµÃ÷ÆäÖÐÓÐÊý×ÖºÍ×ÖÄ¸Ö®ÍâµÄ×Ö½Ú
u8  judge_is_ASCII(char * p,u16 P_NO)			//P Ö¸Õë  P_NOÎª×Ö½ÚÊý  //·µ»Ø0 Îª²»È«ÊÇasckII  1 ±íÊ¾È«²¿ÊÇascII

{
		u8 judge_res=1;

		for(u16 i=0; i<P_NO;i++){
				if( (*(p+i) <0x30) 	||
						(*(p+i) > 0x7A)  ||
						((*(p+i) > 0x39) && (*(p+i) < 0x41) ) ||
						((*(p+i) > 0x5A) && (*(p+i) < 0x61)  )   )
					{
						*(p+i)=0x2A;				//ÆäÖÐÓÐ²»ÊÇÊý×Ö»ò×ÖÄ¸µÄ Çé¿öÖ±½Ó°ÑÕâÒ»¸ö×Ö½ÚÖÃ³É'*'		
						judge_res=0;				//Ð¡ÓÚ13ËµÃ÷ÖÐÍ¾ÍË³öÁË ÅÐ¶¨ ²»È«ÊÇascII
					}
		
		}
		return judge_res;							
}

extern u8 flag_f101;
static void Get_guide(void)
{
uint8_t Result_Sensor = Get_Sensor_flag();

		if(AuxiliaryCanState == 0) //ÕýÔÚÆ¥Åä
		{
			guide =0;
		}

		if(AuxiliaryCanMatchStep == 2) //Ã»ÓÐÆ¥ÅäÉÏ
		{
//			if(Result_Sensor == 1)
//					guide_Num = Guide_Idle_YesF101;
			if(Result_Sensor == 0)
					guide_Num = Guide_Idle_NoF101;	
		}

		if(AuxiliaryCanState == 1) //Æ¥ÅäÉÏÁË
		{
			if(AuxiliaryCanPro == 0)
			{
//				if(Result_Sensor == 1)
//						guide_Num = AuxCan19_250_YesF101;
				if(Result_Sensor == 0)
						guide_Num = AuxCan19_250_NoF101;	
			}
			if(AuxiliaryCanPro == 1)
			{
//				if(Result_Sensor == 1)
//						guide_Num = AuxCan311_250_YesF101;
				if(Result_Sensor == 0)
						guide_Num = AuxCan311_250_NoF101;	
			}
			if(AuxiliaryCanPro == 2)
			{
//				if(Result_Sensor == 1)
//						guide_Num = AuxCan311_500_YesF101;
				if(Result_Sensor == 0)
						guide_Num = AuxCan311_500_NoF101;	
			}
			if(AuxiliaryCanPro == 3)
			{
//				if(Result_Sensor == 1)
//						guide_Num = AuxCan19_500_YesF101;
				if(Result_Sensor == 0)
						guide_Num = AuxCan19_500_NoF101;	
			}
		}
		

		static uint16_t AuxCanGuide_cnt = 0;	
			
		if(AuxCanGuide_cnt > 3000 )
		{
			if(Result_Sensor ==1)
			{
					guide =1; //Æ¥Åä³É¹¦
			}
			else
			{
				if(guide_Num == AuxCan19_250_NoF101)	
				{
					guide = 2;//²¢Èë1/9
				}
				if(guide_Num == AuxCan311_250_NoF101)
				{
					guide = 3;//²¢Èë3/11
				}
				if((guide_Num == AuxCan19_500_NoF101) || (guide_Num == AuxCan311_500_NoF101) || (guide_Num == Guide_Idle_NoF101))
				{
					guide = 4;//¶Ï¿ª1/9ºÍ3/11½ÓÈë19
				}
			}
		}
		else
		{
			AuxCanGuide_cnt++;
		}


}

static void GetObdPara(void)
{	
		u8 udsflag=0;	
		u8 udsmode=1;
		u8 udsmatchnum=0;
		u32 Evades;
		u8 evad1,evad2,evad8, evad7; 	// 
		/*
		evad1,ï¿½ï¿½ï¿½09ï¿½ï¿½ï¿½ï¿½Ø±Õ±ï¿½Ö¾ï¿½ï¿½ 1=ï¿½Ø±ï¿½   0=ï¿½ï¿½ï¿½ï¿½09ï¿½ï¿½ï¿½ï¿½
		evad2, // Ö»Ö§³Ö1939Ð­Òé£¬Æ¥ÅäÊ±²»·¢ËÍ£¬²¨ÌØÂÊÒ»Ö±250£¬·ÀÖ¹³öÏÖ³µÁ¾Æô¶¯ÎÊÌâ¡£
		evad3,
		evad4,
		evad5,
		evad6,
		evad7,
		evad8, UDSï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Ö¾ï¿½ï¿½  1=ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Ãµï¿½UDSÐ­ï¿½é£¬ï¿½ï¿½ï¿½Ú½ï¿½ï¿½ï¿½Æ¥ï¿½ä£»   0=ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½UDSÐ­ï¿½é£¬Ö»ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Ê¼Æ¥ï¿½ï¿½ï¿½UDSÐ­ï¿½é¡£
		*/	
		//----- UDSï¿½ï¿½ï¿½ï¿½ï¿½ï¿½È¡
		udsflag=GetUdsPara(_NEED_COMPARE);//
		SetUdsEnFlag(udsflag);		 																//UDSÊ¹ï¿½Ü¶ï¿½È¡
		udsmode=GetUdsPara(_MODE);
		SetUdsMode(udsmode); 																			//UDSÄ£Ê½ï¿½ï¿½È¡	
		udsmatchnum=GetUdsPara(_DCUID);														//ï¿½ï¿½È¡UDSï¿½ï¿½Ê¼Æ¥ï¿½ï¿½ï¿½ï¿½
		UdsMatchTypeNum(udsmatchnum);	
		//-----  ï¿½ï¿½Ü²ï¿½ï¿½ï¿½ï¿½ï¿½È¡
		Evades=GetUdsPara(_EVADES);
		// ï¿½ï¿½ï¿½ï¿½09ï¿½ï¿½ï¿½ï¿½Ø±ï¿½
		evad1=(Evades>>28)&0x0f;
		evad2=(Evades>>24)&0x0f;		
		if(evad1==1)
			Dis15765_09Sever(); 
		//UDSï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Ö¾ï¿½Ð¶ï¿½
//		if(evad2==0)
//		if(evad2!=1)
//		{
//			SetUdsPara(_EVADES,0x01000000);
//			flag_only1939=1;		
//		}
		
		if(evad2==1)
			flag_only1939=1;
		//------------
		evad8=Evades&0x0f;
		if(evad8==1)
		{
			SetUdsProtocol(udsmatchnum);
			flag_UDSSEND=1;
		}
		evad7=(Evades>>4)&0x0f;
		if(evad7==1)
		{
			PenaltyZoneFlag = 1;
		}
		
}
void OBD_LED_INIT(void)
{
	GPIO_InitType	GPIO_InitStructure;
	
	RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOB, ENABLE);
	
	GPIO_InitStructure.Pin        = GPIO_PIN_0;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitPeripheral(GPIOB, &GPIO_InitStructure);
	
	CAN_LED_OFF;
}
void Obd_init(void)
{

	memset( &OBD_vehicle_infor ,0xFF,sizeof(OBD_vehicle_infor));
	memset( &OBD_diagnosis_infor ,0xFF,sizeof(OBD_diagnosis_infor));
	memset( &OBD_diagnosis_infor.iupr ,0x00,sizeof(OBD_diagnosis_infor.iupr));
	memset( &OBD_engine_infor ,0xFF,sizeof(OBD_engine_infor));
	memset( &OBD_aftertreatment_infor ,0xFF,sizeof(OBD_aftertreatment_infor));

	memset(&(OBD_diagnosis_infor.dntReadyStatus), 0 ,2);
	memset(&(OBD_diagnosis_infor.dntSptStatus), 0 ,2);
	
	GetObdPara();
	ISO15765Init();
	J1939Init();

}


void  get_OBD_Vehicle_Infor_t(  OBD_Vehicle_Infor_t  *p )
{
  memcpy(p,&OBD_vehicle_infor,sizeof(OBD_vehicle_infor));
}

void  get_OBD_Diagnosis_Infor_t(  OBD_Diagnosis_Infor_t  *p )
{
  memcpy(p,&OBD_diagnosis_infor,sizeof(OBD_diagnosis_infor));
	OBD_diagnosis_infor.faultCodeSum = 0xFF;
	memset( OBD_diagnosis_infor.faultCode,0xFF,sizeof(OBD_diagnosis_infor.faultCode));
}
void  get_OBD_Engine_Infor_t(  OBD_Engine_Infor_t *p )
{
  memcpy(p,&OBD_engine_infor,sizeof(OBD_engine_infor));  
}
void  get_OBD_Aftertreatment_Infor_t(  OBD_Aftertreatment_Infor_t  *p )
{
  memcpy(p,&OBD_aftertreatment_infor,sizeof(OBD_aftertreatment_infor));
}

extern u8 flag_f101;
static void AuxiliaryCanInit(void)
{
	switch(AuxiliaryCanPro)
	{
		case 0:
		{
			/*¼ÌµçÆ÷Ê§µç£¬CAN250²¨ÌØÂÊ*/
			GPIO_ResetBits(GPIOB, GPIO_PIN_15);
			//CAN1_Mode_Init(250,0,0,1);
			CanSetBaud(250, CAN1);
		}break;
		case 1:
		{
 			/*¼ÌµçÆ÷µÃµç£¬CAN250²¨ÌØÂÊ*/
			GPIO_SetBits(GPIOB, GPIO_PIN_15);
			//CAN1_Mode_Init(250,0,0,1);
			CanSetBaud(250, CAN1);
		}break;
		case 2:
		{
			/*¼ÌµçÆ÷µÃµç£¬CAN500²¨ÌØÂÊ*/
			GPIO_SetBits(GPIOB, GPIO_PIN_15);
			//CAN1_Mode_Init(500,0,0,1);
			CanSetBaud(500, CAN1);

		}break;
		case 3:
		{
			/*¼ÌµçÆ÷Ê§µç£¬CAN500²¨ÌØÂÊ*/
			GPIO_ResetBits(GPIOB, GPIO_PIN_15);
			//CAN1_Mode_Init(500,0,0,1);
			CanSetBaud(500, CAN1);
		}break;
		default:
		{
			/*¼ÌµçÆ÷µÃµç£¬CAN250²¨ÌØÂÊ*/
			GPIO_SetBits(GPIOB, GPIO_PIN_15);
			//CAN1_Mode_Init(250,0,0,1);
			CanSetBaud(250, CAN1);
		}break;
	}
}

 static void AuxiliaryCanPinMatch(void)
{
	switch(AuxiliaryCanMatchStep)
	{
		case 0: /*³õÊ¼»¯*/
		{
      AuxiliaryCanInit();
			AuxiliaryCanMatchDealy = 0;
			AuxiliaryCanMatchStep = 1;
		}break;
		
		case 1:/*µÈ´ý1939±¨ÎÄ*/
		{
      if(!AuxiliaryCanState)
			{
				AuxiliaryCanMatchDealy += 1;
				if(AuxiliaryCanMatchDealy>1000) /*3ÃëÖÓ*/
				{
					AuxiliaryCanPro += 1;
					if(AuxiliaryCanPro>3) /*×Ü¹²4ÖÖÇé¿ö£¬¼ûAuxiliaryCanInit */
					{
						AuxiliaryCanPro = 0;
						AuxiliaryCanMatchCnt += 1;
						if(AuxiliaryCanMatchCnt>2) /*Æ¥Åä3¸öÖÜÆÚ*/
						{
							/*¼ÌµçÆ÷Ê§µç£¬CAN250²¨ÌØÂÊ*/
							GPIO_ResetBits(GPIOB, GPIO_PIN_15);
							//CAN1_Mode_Init(250,0,0,1);
							CanSetBaud(250, CAN1);
							AuxiliaryCanMatchStep = 2;
						}
						else				
						AuxiliaryCanMatchStep = 0;
						
					}
					else
					AuxiliaryCanMatchStep = 0;
										
				}
			}
		}break;
		
		case 2:  //Ö£ÖÝ µÈ´ýF101 ¹Ø±ÕCAN1
		{
      if(!AuxiliaryCanState)
			{
				if(flag_f101)
				{
					GPIO_SetBits(GPIOA, GPIO_PIN_8); /*¹Ø±ÕCAN1*/
					AuxiliaryCanState = 2; //Òý½ÅÆ¥ÅäÊ§°Ü			
				}
			}
		}break;		
		
		
	}

}

static void ObdCanBaudSet(uint16_t baud)
{
  CanSetBaud(baud, CAN2);
	//ObdCanBaud = baud;
}

static void ProtocolMatch(void)
{
	switch(CheckProState)
	{
		case 0: //³õÊ¼»¯
		{
			if(flag_only1939==1)
				ObdCanBaud=250;
			ObdCanBaudSet(ObdCanBaud);
			CheckProDelayCnt = 0;
			CheckProSucCnt = 0;
		if(ISO5765ProNum==ISO15765_11_250)
			CheckProState = 3;
		else
			CheckProState = 1;
		}break;
		
		case 1: //·¢ËÍ±¨ÎÄ
		{
			CanTxMessage TxMsg;
			memset(&TxMsg,0,sizeof(CanTxMessage));
			TxMsg.Data[0] = 0x02;
			TxMsg.Data[1] = 0x01;
			TxMsg.DLC = 8;
			TxMsg.ExtId = 0x18DB33F1;
			TxMsg.StdId = 0x7DF;
			TxMsg.RTR = 0;
			if((ISO5765ProNum==ISO15765_11_500)||(ISO5765ProNum==ISO15765_11_250))
			{
				TxMsg.IDE = CAN_ID_STD;
			}
			else
			{
				TxMsg.IDE = CAN_ID_EXT;
			}
			if(flag_only1939!=1)
			SendCanMsg(&TxMsg);
			CheckProState = 2;
			CheckProDelay = 0;
		}break;
		
		case 2: //µÈ´ý±¨ÎÄ
		{
			CanRxMsg_t *pMsg;
			do{
				pMsg = GetCanRxMsg();
				if(pMsg==NULL)
				{
					break;
				}
				if(pMsg->CanNum==CAN_NUM2)
				{
					if(ObdCanBaud==250)
					{
						//J1939Ð­ÒéÅÐ¶ÏPGN F004
						if(((uint16_t)(pMsg->CanId>>8) == 0xF004)&&(pMsg->IDE==CAN_Extended_Id))
						{
							J1939ProState = 1;
						}
					}
					//ISO15765Ð­ÒéÅÐ¶Ï0100ÊÇ·ñÓÐ»Ø¸´
					if((pMsg->CanData[1]==0x41)&&(pMsg->CanData[2]==0x00))
					{
						if(pMsg->IDE == CAN_Extended_Id)
						{
							if(iso_15765_canAdd_Blacklist_judge(pMsg->CanId) == 0)  //²»ÔÚºÚÃûµ¥ÖÐ
							{
								CheckProDelay=0;
								CheckProSucCnt += 1;
							}
						}
						else//±ê×¼Ö¡
						{
							CheckProDelay=0;
							CheckProSucCnt += 1;
						}
					}
				}
				pMsg->Empty = 0;

				if(CheckProSucCnt != 0) //Æ¥Åäµ½Ò»´Î¾Í¿ÉÒÔÌáÇ°ÍË³ö
        {
          break;
        }
			}while(1);
			
			if(CheckProSucCnt>=1) //¼ì²âµ½15765ÁË			
			{
				if(ObdCanBaud==250) /*²¨ÌØÂÊ250Ê±£¬µÈ´ýJ1939Ð­Òé*/
				{
					if(J1939ProState)
					{
						can_state = 1;
					}
					else
					{
						CheckProDelay += 1;
						if(CheckProDelay>40) /*µÈ´ý200ms*/
						{
							can_state = 1;
						}
					}
				}
				else
				{
          can_state = 1;
				}
			}
			else
			{
				if(CheckProDelay>10) //³¬Ê± 5*10 50msÃ»ÊÕµ½£¬
				{
					CheckProDelayCnt += 1;
					if(CheckProDelayCnt>=3) //Æ¥Åä³¬Ê±3´Î
					{
						if(ISO5765ProNum==ISO15765_11_250)
						{
							ISO5765ProNum = ISO15765_29_250;
							ObdCanBaud = 250;
							CheckProState = 0;
						}
						else if(ISO5765ProNum == ISO15765_29_250)
						{
							if(J1939ProState==1)
							{
								can_state = 1;
								ISO5765ProNum = ProtocolIdle;
							}
							else
							{
								ISO5765ProNum = ISO15765_11_500;
								ObdCanBaud = 500;
								CheckProState = 0;
							}
						}
						else if(ISO5765ProNum == ISO15765_11_500)
						{
							ISO5765ProNum = ISO15765_29_500;
							ObdCanBaud = 500;
							CheckProState = 0;
						}
						else
						{
							can_state = 0;
						}
					}
					else
					{
						CheckProState = 1;
					}
				}
				else
				{
					CheckProDelay += 1;
				}
			}
		}break;

		case 3: //µÈ´ý1939±¨ÎÄ
		{
			CanRxMsg_t *pMsg;
			do{
				pMsg = GetCanRxMsg();
				if(pMsg==NULL)
				{
					break;
				}
				if(pMsg->CanNum==CAN_NUM2)
				{
					if(ObdCanBaud==250)
					{
						//J1939Ð­ÒéÅÐ¶ÏPGN F004
						if(((uint16_t)(pMsg->CanId>>8) == 0xF004)&&(pMsg->IDE==CAN_Extended_Id))
						{
							J1939ProState = 1;
							can_state = 1;
							ISO5765ProNum = ProtocolIdle;
							flag_only1939=1;
							break;
						}
					}
					//ISO15765Ð­ÒéÅÐ¶Ï0100ÊÇ·ñÓÐ»Ø¸´
				}
				pMsg->Empty = 0;

				}while(1);
			
				CheckProDelay += 1;
				

				if(CheckProDelay>30) //³¬Ê± 5*10 50msÃ»ÊÕµ½£¬
				{
					CheckProDelayCnt = 0;
					CheckProSucCnt = 0;
					CheckProState = 1;	
//					flag_only1939=0;
				}

			
		}break;
		
		default:
		{
			ObdCanBaud = 250;
			ISO5765ProNum = ISO15765_11_250;
			CheckProState = 0;
		}break;
	}
}

static u8 Flag_can_off=0;
//¼ì²é³µÁ¾ÊÇ·ñÓÐ×ªËÙ
static void CheckEPM(void)
{
  if(OBD_engine_infor.engineRev != OldEngineSpeed)
	{
		//·¢¶¯»ú×ªËÙÓÐ±ä»¯£¬
		OldEngineSpeed = OBD_engine_infor.engineRev;
		CheckEpmDelay = 0;
		Flag_can_off=0;
		if(READ_CAN_LED)				 
			CAN_LED_ON;
	}
	else
	{
		if(CheckEpmDelay>7000) //·¢¶¯»ú×ªËÙ²»±ä£¬³¬Ê±
		{
			can_state = 0; //ÐÝÃß
			
			if((CheckEpmDelay%333==332)&&Flag_can_off)
			{
				if(READ_CAN_LED)				
				CAN_LED_ON;
				else
					CAN_LED_OFF;
			}			
	
		}
		else
		{
			CheckEpmDelay += 1;

			if(CheckEpmDelay>=6000)
			{
					Flag_can_off=1;
			}		
				
			if((CheckEpmDelay%333==332)&&Flag_can_off)
			{
				if(READ_CAN_LED)				
				CAN_LED_ON;
				else
					CAN_LED_OFF;
			}
			
		}
	}
}

void NVIC_Config_can(void)
{
	NVIC_InitType 	NVIC_InitStructure;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);
	
	NVIC_InitStructure.NVIC_IRQChannel            = CAN2_RX0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd         = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel            = USB_LP_CAN1_RX0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd         = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

uint8_t GetProtocolNum(void)
{
	if(J1939ProState)
		return 1;
	else	
	return ISO5765ProNum;
}

static void GetObdData(void)
{
	CanData_t *pCanMsg;
	uint8_t FrameCnt;
	uint8_t DoCnt = 0;
	do
	{
		pCanMsg = GetCanData(&FrameCnt);
		if(pCanMsg==NULL)
		{
			break;
		}
		if(pCanMsg->CanNum==CAN_NUM1) /*ÅÐ¶ÏÓÐÃ»ÓÐÊÕµ½µÚ¶þÂ·CANµÄÊý¾Ý*/
		{
			AuxiliaryCanState = 1;
		}
		
		if(pCanMsg->Procotol==J1939)
		{
		  J1939CheckPgn(pCanMsg,FrameCnt);
		}
		else
		{
			if((pCanMsg->Procotol==ISO15765)&&(pCanMsg->CanNum==CAN_NUM2))
			{
				ISO15765CheckPid(pCanMsg,FrameCnt);
				UdsCheckPid(pCanMsg,FrameCnt);
			}
		}
		ReleaseCanDataBuf(pCanMsg); /*ÊÍ·Åbuf*/
		DoCnt += 1;
	}while(DoCnt<50); /*×î¶à´¦Àí50°üÊý¾Ý*/
}

extern u8 flag_set_evads;
void obd_task(void const *argument)
{
	NVIC_Config_can();
	CanInit();
	OBD_LED_INIT();
	Obd_init();
	
//	pro_sn = ISO15765_29_250;
  can_state = 0xFF;
	
  ISO5765ProNum = ISO15765_11_250;
  J1939ProState = 0;
  ObdCanBaud = 250;

	CheckProState = 0;
	CheckProDelay = 0;
	CheckProSucCnt = 0;

	CheckEpmDelay = 1;
	OldEngineSpeed = 0xFFFF;
	
	ObdLoopCnt = 0;
	jingli_sensor.state = 0;
	jingli_sensor.CanNum = CAN_DEFAULT;
	
	xinli_sensor.state = 0;
	xinli_sensor.CanNum = CAN_DEFAULT;
	
	tiandicheren_sensor.state = 0;
	tiandicheren_sensor.CanNum = CAN_DEFAULT;	
	while(1)
	{
		iwdg_obd++;
		if(can_state==1) //Ð­ÒéÆ¥ÅäOK
		{
			CanDataProcess();
			
			if( ((CheckEpmDelay == 0)|| ((CheckEpmDelay >1)&&(CheckEpmDelay <1100))) && (OBD_engine_infor.engineRev !=0xFFFF)&&(OBD_engine_infor.engineRev != 0))
			{
				if(!AuxiliaryCanState)
				{
					AuxiliaryCanPinMatch();
				}
				Get_guide();
			}	
			
			if((ISO5765ProNum==ISO15765_11_500)||(ISO5765ProNum==ISO15765_29_500)||(ISO5765ProNum==ISO15765_11_250)||(ISO5765ProNum==ISO15765_29_250))
			{
				ISO15765SendMsg(ISO5765ProNum,ObdLoopCnt%100);						
			}
			else
			{
				/*OBD CANÉÏ¼ì²âµ½ÁË15765¾Í¹Ø±Õ1939µÄ·¢ËÍ*/
				if(J1939ProState)
				{
				  J1939SendMsg(ObdLoopCnt%100);
				}
				else
				{
					can_state = 0;
				}
			}
			
			F101_Sensor_Send(ObdLoopCnt%100-25);						//Ö»·¢ËÍÒ»´Î
//			Xinli_0000_Sensor_Send(ObdLoopCnt%400-25);					//Ò»Ãë·¢Ò»´Î×ªËÙ
			TianDiCheRen_Sensor_Send(ObdLoopCnt%1000-25);	  //ÈýÃë½øÒ»´Î ¿ªÆôµç¼ÓÈÈ ºÍ ¸ßÑ¹ Ã¿¸ö·¢ËÍ3´Î
			
			//if(ObdLoopCnt>20000)

			if(flag_UDSSEND)
			{
				UdsReqServer(ISO5765ProNum, ObdLoopCnt%100-50);
			}
			
			if(flag_set_evads)
			{
				u32 Evades2;
				Evades2=GetUdsPara(_EVADES);
				if(flag_only1939)
				{
					Evades2&=0xF0FFFFFF;
					Evades2|=0x01000000;
				}
					else
					Evades2&=0xF0FFFFFF;
				
				SetUdsPara(_EVADES,Evades2);
				flag_set_evads=0;
			}
			
			
			GetObdData();
			CheckEPM();
			ObdLoopCnt += 1;
			osDelay(3);
		}
		else
		{
			if(can_state==0xFF) //Ð­ÒéÆ¥ÅäÖÐ
			{
				ProtocolMatch(); /*Ð­ÒéÆ¥Åä³É¹¦Ê±,can_state=1*/
				if(can_state==1)
				{
						CAN_LED_ON;
				
					if(J1939ProState)
					{
						OBD_diagnosis_infor.obdDntPtc = J1939_29_250;
					}
					else
					{
						if((ISO5765ProNum==ISO15765_11_500)||(ISO5765ProNum==ISO15765_29_500)||(ISO5765ProNum==ISO15765_11_250)||(ISO5765ProNum==ISO15765_29_250))
						{
							OBD_diagnosis_infor.obdDntPtc = ISO5765ProNum;
						}
						else
						{
							can_state = 0;
						}
					}
			  }
				osDelay(5);
			}
			else //Ð­ÒéÆ¥ÅäÊ§°Ü,can_state==0
			{
				iwdg_obd++;
//				osDelay(10000);
				osDelay(1000);
				iwdg_obd++;
				osDelay(1000);
				iwdg_obd++;
				osDelay(1000);
				iwdg_obd++;				
				can_state=0xff;
				ISO5765ProNum = ISO15765_11_250;
				J1939ProState = 0;
				ObdCanBaud = 250;
				CheckProState = 0;
				CheckProDelay = 0;
				CheckProSucCnt = 0;
				Obd_init();
			}
		}
	}
}



//-----²É¼¯Ïß³Ì¶¨Òå---------
//void can_collection(void const *argument);
osThreadId tid_can_collection;
osThreadDef(obd_task, osPriorityNormal, 1, 1000);

void can_collection(void)
{
	tid_can_collection = osThreadCreate (osThread(obd_task), NULL);
}





