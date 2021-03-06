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
//static ProtocolNum_t pro_sn = ProtocolIdle;		//车载协议的标识 0-无协议  1,1930  2,11/500  3,29/500  4,11/250  5,29/250  
ProtocolNum_t ISO5765ProNum;
static uint8_t J1939ProState;
static uint16_t ObdCanBaud;
uint8_t can_state = 0xFF; //0：休眠 0xFF：匹配协议中 1：CAN工作正常

uint8_t guide = 0;
GuideNum guide_Num ;

OBD_Diagnosis_Infor_t  OBD_diagnosis_infor;
OBD_Vehicle_Infor_t OBD_vehicle_infor;
OBD_Engine_Infor_t 	OBD_engine_infor;
OBD_Aftertreatment_Infor_t OBD_aftertreatment_infor;

//static uint8_t CheckProNum;  //高4位时可能的协议号，低4位是检测的协议号
static uint8_t CheckProState; 
static uint8_t CheckProDelay;
static uint8_t CheckProDelayCnt;
static uint8_t CheckProSucCnt;

static uint16_t CheckEpmDelay;
static uint16_t OldEngineSpeed;

static uint8_t AuxiliaryCanState = 0; /*0:匹配，1:匹配成功，2:匹配失败*/
static uint8_t AuxiliaryCanPro = 0;
static uint8_t AuxiliaryCanMatchStep = 0;
static uint8_t AuxiliaryCanMatchCnt = 0; /*匹配周期数*/
static uint16_t AuxiliaryCanMatchDealy = 0;

uint8_t flag_UDSSEND=0;	

static uint32_t ObdLoopCnt;
static uint8_t flag_only1939=0;

u32 iwdg_obd=0;
extern JingLi_Sensor jingli_sensor;
extern XinLi_Sensor xinli_sensor;
extern TianDiCheRen_Sensor tiandicheren_sensor;
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//P为buffer起始地址
//P_NO为buffer长度
//且会将其中的非数字和字母部分替换成 *
//返回值为1为全部为数字和字母
//				0时说明其中有数字和字母之外的字节
u8  judge_is_ASCII(char * p,u16 P_NO)			//P 指针  P_NO为字节数  //返回0 为不全是asckII  1 表示全部是ascII

{
		u8 judge_res=1;

		for(u16 i=0; i<P_NO;i++){
				if( (*(p+i) <0x30) 	||
						(*(p+i) > 0x7A)  ||
						((*(p+i) > 0x39) && (*(p+i) < 0x41) ) ||
						((*(p+i) > 0x5A) && (*(p+i) < 0x61)  )   )
					{
						*(p+i)=0x2A;				//其中有不是数字或字母的 情况直接把这一个字节置成'*'		
						judge_res=0;				//小于13说明中途退出了 判定 不全是ascII
					}
		
		}
		return judge_res;							
}

extern u8 flag_f101;
static void Get_guide(void)
{
uint8_t Result_Sensor = Get_Sensor_flag();

		if(AuxiliaryCanState == 0) //正在匹配
		{
			guide =0;
		}

		if(AuxiliaryCanMatchStep == 2) //没有匹配上
		{
//			if(Result_Sensor == 1)
//					guide_Num = Guide_Idle_YesF101;
			if(Result_Sensor == 0)
					guide_Num = Guide_Idle_NoF101;	
		}

		if(AuxiliaryCanState == 1) //匹配上了
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
					guide =1; //匹配成功
			}
			else
			{
				if(guide_Num == AuxCan19_250_NoF101)	
				{
					guide = 2;//并入1/9
				}
				if(guide_Num == AuxCan311_250_NoF101)
				{
					guide = 3;//并入3/11
				}
				if((guide_Num == AuxCan19_500_NoF101) || (guide_Num == AuxCan311_500_NoF101) || (guide_Num == Guide_Idle_NoF101))
				{
					guide = 4;//断开1/9和3/11接入19
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
		evad1,锟斤拷锟?09锟斤拷锟斤拷乇毡锟街撅拷锟? 1=锟截憋拷   0=锟斤拷锟斤拷09锟斤拷锟斤拷
		evad2, // 只支持1939协议，匹配时不发送，波特率一直250，防止出现车辆启动问题。
		evad3,
		evad4,
		evad5,
		evad6,
		evad7,
		evad8, UDS锟斤拷锟斤拷锟斤拷志锟斤拷  1=锟斤拷锟斤拷锟斤拷锟矫碉拷UDS协锟介，锟斤拷锟节斤拷锟斤拷匹锟戒；   0=锟斤拷锟斤拷锟斤拷UDS协锟介，只锟斤拷锟斤拷锟斤拷锟斤拷始匹锟斤拷锟経DS协锟介。
		*/	
		//----- UDS锟斤拷锟斤拷锟斤拷取
		udsflag=GetUdsPara(_NEED_COMPARE);//
		SetUdsEnFlag(udsflag);		 																//UDS使锟杰讹拷取
		udsmode=GetUdsPara(_MODE);
		SetUdsMode(udsmode); 																			//UDS模式锟斤拷取	
		udsmatchnum=GetUdsPara(_DCUID);														//锟斤拷取UDS锟斤拷始匹锟斤拷锟斤拷
		UdsMatchTypeNum(udsmatchnum);	
		//-----  锟斤拷懿锟斤拷锟斤拷锟饺?
		Evades=GetUdsPara(_EVADES);
		// 锟斤拷锟斤拷09锟斤拷锟斤拷乇锟?
		evad1=(Evades>>28)&0x0f;
		evad2=(Evades>>24)&0x0f;		
		if(evad1==1)
			Dis15765_09Sever(); 
		//UDS锟斤拷锟斤拷锟斤拷志锟叫讹拷
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
			/*继电器失电，CAN250波特率*/
			GPIO_ResetBits(GPIOB, GPIO_PIN_15);
			//CAN1_Mode_Init(250,0,0,1);
			CanSetBaud(250, CAN1);
		}break;
		case 1:
		{
 			/*继电器得电，CAN250波特率*/
			GPIO_SetBits(GPIOB, GPIO_PIN_15);
			//CAN1_Mode_Init(250,0,0,1);
			CanSetBaud(250, CAN1);
		}break;
		case 2:
		{
			/*继电器得电，CAN500波特率*/
			GPIO_SetBits(GPIOB, GPIO_PIN_15);
			//CAN1_Mode_Init(500,0,0,1);
			CanSetBaud(500, CAN1);

		}break;
		case 3:
		{
			/*继电器失电，CAN500波特率*/
			GPIO_ResetBits(GPIOB, GPIO_PIN_15);
			//CAN1_Mode_Init(500,0,0,1);
			CanSetBaud(500, CAN1);
		}break;
		default:
		{
			/*继电器得电，CAN250波特率*/
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
		case 0: /*初始化*/
		{
      AuxiliaryCanInit();
			AuxiliaryCanMatchDealy = 0;
			AuxiliaryCanMatchStep = 1;
		}break;
		
		case 1:/*等待1939报文*/
		{
      if(!AuxiliaryCanState)
			{
				AuxiliaryCanMatchDealy += 1;
				if(AuxiliaryCanMatchDealy>1000) /*3秒钟*/
				{
					AuxiliaryCanPro += 1;
					if(AuxiliaryCanPro>3) /*总共4种情况，见AuxiliaryCanInit */
					{
						AuxiliaryCanPro = 0;
						AuxiliaryCanMatchCnt += 1;
						if(AuxiliaryCanMatchCnt>2) /*匹配3个周期*/
						{
							/*继电器失电，CAN250波特率*/
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
		
		case 2:  //郑州 等待F101 关闭CAN1
		{
      if(!AuxiliaryCanState)
			{
				if(flag_f101)
				{
					GPIO_SetBits(GPIOA, GPIO_PIN_8); /*关闭CAN1*/
					AuxiliaryCanState = 2; //引脚匹配失败			
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
		case 0: //初始化
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
		
		case 1: //发送报文
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
		
		case 2: //等待报文
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
						//J1939协议判断PGN F004
						if(((uint16_t)(pMsg->CanId>>8) == 0xF004)&&(pMsg->IDE==CAN_Extended_Id))
						{
							J1939ProState = 1;
						}
					}
					//ISO15765协议判断0100是否有回复
					if((pMsg->CanData[1]==0x41)&&(pMsg->CanData[2]==0x00))
					{
						if(pMsg->IDE == CAN_Extended_Id)
						{
							if(iso_15765_canAdd_Blacklist_judge(pMsg->CanId) == 0)  //不在黑名单中
							{
								CheckProDelay=0;
								CheckProSucCnt += 1;
							}
						}
						else//标准帧
						{
							CheckProDelay=0;
							CheckProSucCnt += 1;
						}
					}
				}
				pMsg->Empty = 0;

				if(CheckProSucCnt != 0) //匹配到一次就可以提前退出
        {
          break;
        }
			}while(1);
			
			if(CheckProSucCnt>=1) //检测到15765了			
			{
				if(ObdCanBaud==250) /*波特率250时，等待J1939协议*/
				{
					if(J1939ProState)
					{
						can_state = 1;
					}
					else
					{
						CheckProDelay += 1;
						if(CheckProDelay>40) /*等待200ms*/
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
				if(CheckProDelay>10) //超时 5*10 50ms没收到，
				{
					CheckProDelayCnt += 1;
					if(CheckProDelayCnt>=3) //匹配超时3次
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

		case 3: //等待1939报文
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
						//J1939协议判断PGN F004
						if(((uint16_t)(pMsg->CanId>>8) == 0xF004)&&(pMsg->IDE==CAN_Extended_Id))
						{
							J1939ProState = 1;
							can_state = 1;
							ISO5765ProNum = ProtocolIdle;
							flag_only1939=1;
							break;
						}
					}
					//ISO15765协议判断0100是否有回复
				}
				pMsg->Empty = 0;

				}while(1);
			
				CheckProDelay += 1;
				

				if(CheckProDelay>30) //超时 5*10 50ms没收到，
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
//检查车辆是否有转速
static void CheckEPM(void)
{
  if(OBD_engine_infor.engineRev != OldEngineSpeed)
	{
		//发动机转速有变化，
		OldEngineSpeed = OBD_engine_infor.engineRev;
		CheckEpmDelay = 0;
		Flag_can_off=0;
		if(READ_CAN_LED)				 
			CAN_LED_ON;
	}
	else
	{
		if(CheckEpmDelay>7000) //发动机转速不变，超时
		{
			can_state = 0; //休眠
			
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
		if(pCanMsg->CanNum==CAN_NUM1) /*判断有没有收到第二路CAN的数据*/
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
		ReleaseCanDataBuf(pCanMsg); /*释放buf*/
		DoCnt += 1;
	}while(DoCnt<50); /*最多处理50包数据*/
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
		if(can_state==1) //协议匹配OK
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
				/*OBD CAN上检测到了15765就关闭1939的发送*/
				if(J1939ProState)
				{
				  J1939SendMsg(ObdLoopCnt%100);
				}
				else
				{
					can_state = 0;
				}
			}
			
			F101_Sensor_Send(ObdLoopCnt%100-25);						//只发送一次
//			Xinli_0000_Sensor_Send(ObdLoopCnt%400-25);					//一秒发一次转速
			TianDiCheRen_Sensor_Send(ObdLoopCnt%1000-25);	  //三秒进一次 开启电加热 和 高压 每个发送3次
			
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
			if(can_state==0xFF) //协议匹配中
			{
				ProtocolMatch(); /*协议匹配成功时,can_state=1*/
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
			else //协议匹配失败,can_state==0
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



//-----采集线程定义---------
//void can_collection(void const *argument);
osThreadId tid_can_collection;
osThreadDef(obd_task, osPriorityNormal, 1, 1000);

void can_collection(void)
{
	tid_can_collection = osThreadCreate (osThread(obd_task), NULL);
}





