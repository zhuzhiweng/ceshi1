#include "j_1939.h"
#include "CanDataAnalysis.h"
#include "iso_15765.h"
#include "UDS.h"
#include "utils.h"
#include "obd.h"
#include	"math.h"

extern uint16_t APP_Nox_indicate;
extern uint16_t APP_reag_indicate;
static uint8_t J1939SendPgnNum = 0;
//static uint16_t J1939SendDelay = 0;
static uint64_t J1939PgnSendState = 0; //0���� 1��ֹ
static uint64_t J1939PgnDoneState = 0; //0���� 1��ֹ
//PGN
static const J1939Pgn_t J1939PgnList[] = {    
																					{0xFECE,1,10,0xFF,0},{0xFEEC,1,10,0xFF,0},{0xD300,1,10,0xFF,0},{0xC200,1,10,0xFF,0},{0xF004,0,0,0xFF,0},{0xF003,0,0,0xFF,0},{0xFEF2,0,0,0xFF,0},{0xF00A,0,0,0xFF,0},
                                          {0xFD8B,0,0,0xFF,0}, {0xFD8C,0,0,0xFF,0}, {0xFDAF,0,0,0xFF,0}, {0xFDB2,0,0,0xFF,0}, {0xFDB0,0,0,0xFF,0},{0xFD3E,0,0,0xFF,0},{0xF00E,0,0,0xFF,0},{0xF010,0,0,0xFF,0},
																					{0xF00F,0,0,0xFF,0}, {0xF011,0,0,0xFF,0}, {0xFDA1,0,0,0xFF,0}, {0xFDA0,0,0,0xFF,0}, {0xFD40,0,0,0xFF,0},{0xFD41,0,0,0xFF,0},{0xFE56,0,0,0xFF,0},{0xFEF5,0,0,0xFF,0},
																					{0xFEDF,0,0,0xFF,0}, {0xFEF1,0,0,0xFF,0}, {0xFEEE,0,0,0xFF,0}, {0xFEFC,0,0,0xFF,0}, {0xFEE0,0,0,0xFF,0},{0xFECA,0,0,0xFF,0},{0xFEC1,0,0,0,1},
																					{0xFDB3,0,0,0xFF,0}, {0xFD38,0,0,0xFF,0}, {0xF023,0,0,0xFF,0}, {0xFEE9,0,0,0xFF,0}, {0xF101,0,0,0xFF,0},{0x0000,0,0,0xFF,0},{0x9998,0,0,0xFF,0},{0x0702,0,0,0xFF,0},
                                      };
//J1939PgnList ĩβ�����ֽڵ�����
//0xFF��0��������Ĭ��canaddԽС���ȼ�Խ�ߣ�Ĭ�Ͻ���can��ַ<=0xFF
//0x0��1��������Ĭ��canaddԽ�����ȼ�Խ�ߣ�Ĭ�Ͻ���can��ַ>=0x0

static uint8_t J1939PgnSendCnt[sizeof(J1939PgnList)/sizeof(J1939Pgn_t)];
static uint8_t J1939Pgn_Canadd[sizeof(J1939PgnList)/sizeof(J1939Pgn_t)];

JingLi_Sensor jingli_sensor;	
XinLi_Sensor xinli_sensor ;
TianDiCheRen_Sensor tiandicheren_sensor ;
																			
/************************************************************************
  * @����:   CAN����IO��������
  * @����:   None
  * @����ֵ: None
18ECFF00	8	20	26	00	06	FF	CA	FE	00	1ms				6��	0x26�ֽ� 38�ֽ�   	5*7 +3 = 38���ֽ�
18EBFF00	8	01	15	CC	4C	0A	0C	01	5B	1ms				
18EBFF00	8	02	00	04	01	1D	00	04	01	1ms				�ֽ��������������� 01  02  03 
18EBFF00	8	03	32	03	07	09	9D	00	03	1ms				
18EBFF00	8	04	01	69	00	03	01	6E	00	1ms				
18EBFF00	8	05	03	01	66	00	03	01	8B	1ms				
18EBFF00	8	06	06	05	01	FF	FF	FF	FF	1ms		
  **********************************************************************/

//static uint8_t IsAscii(uint8_t ascii)
//{
//	uint8_t res = 1;
//	if(((ascii>='0')&&(ascii <= '9')) || ((ascii>='A')&&(ascii <= 'Z')) || ((ascii>='a')&&(ascii <= 'z')))
//	{
//		res = 0;
//	}
//	return res;
//}

uint8_t Get_Sensor_flag(void)
{
	uint8_t res = 0;
	if((uint8_t)jingli_sensor.state&1)
	{
		res = 1;
	}
	if((uint8_t)xinli_sensor.state&1)
	{
		res = 1;
	}
	if((uint8_t)tiandicheren_sensor.state&1)
	{
		res = 1;
	}
	return res;
}

static void J1939PgnDisSend(uint16_t Pgn)
{
	for(uint8_t i=0;i<sizeof(J1939PgnList)/sizeof(J1939Pgn_t);i++)
	{
		if(Pgn==J1939PgnList[i].Pgn)
		{
			J1939PgnSendState |= (((uint64_t)1)<<i);
			break;
		}
	}
}

static uint8_t canid_priority_judge(uint16_t PGN,uint8_t CANADD)		//����ֵ=1��֡���ݲ����д��� �� ����ֵ=0������������
{
	uint16_t Pgn  =  PGN;
	uint8_t can_add = CANADD;
	uint8_t res = 0;

	uint8_t i=0;
	for(i=0;i<sizeof(J1939PgnList)/sizeof(J1939Pgn_t);i++)
	{
		if(Pgn==J1939PgnList[i].Pgn)
		{
			break;
		}
	}

	if(i<sizeof(J1939PgnList)/sizeof(J1939Pgn_t))
	{
		if(J1939PgnList[i].H_L_Can_Add_priority)		//1ʱcanid ������ȼ��� 
		{
			if(can_add < J1939Pgn_Canadd[i] )
			{
				res =1;
			}
			else
			{
				J1939Pgn_Canadd[i] = can_add ;
			}
		}
		else
		{
			if(can_add > J1939Pgn_Canadd[i] )
			{
			 res =1;
			}
			else
			{
				J1939Pgn_Canadd[i] = can_add ;
			}
		}
	}
	else
	{
		res  = 1; 
	}

	return res;
		
}



static void J1939PgnDisDone(uint16_t Pgn)
{
	for(uint8_t i=0;i<sizeof(J1939PgnList)/sizeof(J1939Pgn_t);i++)
	{
		if(Pgn==J1939PgnList[i].Pgn)
		{
			J1939PgnDoneState |= (((uint64_t)1)<<i);
			break;
		}
	}
}

static uint8_t IsJ1939PgnDoneEn(uint16_t Pgn)
{
	uint8_t res = 0x01;
	for(uint8_t i=0;i<sizeof(J1939PgnList)/sizeof(J1939Pgn_t);i++)
	{
		if(Pgn == J1939PgnList[i].Pgn)
		{
		if(!(	((uint8_t)(J1939PgnDoneState>>i)) & 0x01) )
			{
				res = 0;
			}
			break;
		}
	}
	return res;
}

//static uint16_t J1939MultiFrameToData(CanData_t *pCanMsg,uint8_t FrameCnt, uint8_t *pData, uint16_t DataLen)
//{
//	CanData_t *pFrame;
//	uint8_t GetSize;
//	uint16_t Len = 0;
//	uint16_t ValidLen = (uint16_t)pCanMsg->Data[1] + (uint16_t)(pCanMsg->Data[2]<<8);
//	if((ValidLen<=DataLen)&&(pCanMsg!=NULL)&&(FrameCnt>1))
//	{
//		for(uint8_t i=1;i<FrameCnt;i++)
//		{
//			pFrame = GetCanSpecifyFrame(pCanMsg, i);
//			if(pFrame==NULL)
//			{
//				break;
//			}
//			if((ValidLen - Len) >= 7) //ÿһ֡���7���ֽ�����
//			{
//				GetSize = 7;
//			}
//			else
//			{
//				GetSize = ValidLen - Len;
//			}
//			for(uint8_t j=0;j<GetSize;j++)
//			{
//				pData[Len] = pFrame->Data[1+j];
//				Len += 1;
//			}
//		}
//  }
//	return Len;
//}

//static uint8_t J1939SingleFrameToData(CanData_t *pCanMsg, uint8_t FrameCnt, uint8_t *pData, uint16_t DataLen)
//{
//	uint8_t Len = 0;
//	if((pCanMsg->DataLen<=DataLen)&&(pCanMsg!=NULL))
//	{
//		for(uint8_t i=0;i<pCanMsg->DataLen;i++)
//		{
//			pData[i] = pCanMsg->Data[i];
//			Len += 1;
//		}
//	}
//	return Len;
//}

static uint8_t IuprProcess (uint32_t iupr_SPN, uint8_t *pData, uint8_t iupr_Pos)
{
		uint8_t iupr_res = 0;
		uint16_t   iupt_Numerator= (uint16_t)(pData[iupr_Pos+4]<<8)+(uint16_t)pData[iupr_Pos+3];  //fenzi
		uint16_t  iupt_Denominator  = (uint16_t)(pData[iupr_Pos+6]<<8)+(uint16_t)pData[iupr_Pos+5];  //fenmu

		if(iupr_SPN == 3055)  //ok
		{
			OBD_diagnosis_infor.iupr.iuprFuelcond =  iupt_Denominator;
			OBD_diagnosis_infor.iupr.iuprFuelcomp = iupt_Numerator;
			iupr_res = 1;
		}
		else if(iupr_SPN ==5322)   //ok
		{
			OBD_diagnosis_infor.iupr.iuprHccatcond= iupt_Denominator;
			OBD_diagnosis_infor.iupr.iuprHccatcomp = iupt_Numerator;
			iupr_res = 1;
		}
		else if(iupr_SPN ==5308)  //ok
		{
			OBD_diagnosis_infor.iupr.iuprNadscond=  iupt_Denominator;
			OBD_diagnosis_infor.iupr.iuprNadscomp = iupt_Numerator;
			iupr_res = 1;
		}
		else if(iupr_SPN ==4792)   //0k
		{
			OBD_diagnosis_infor.iupr.iuprNcatcond = iupt_Denominator;
			OBD_diagnosis_infor.iupr.iuprNcatcomp = iupt_Numerator;
			iupr_res = 1;
		}
		else if(iupr_SPN ==3064)   //ok
		{
			OBD_diagnosis_infor.iupr.iuprPmcond = iupt_Denominator;
			OBD_diagnosis_infor.iupr.iuprPmcomp = iupt_Numerator;
			iupr_res = 1;
		}
		else if(iupr_SPN ==5318 )      //-ok
		{
			OBD_diagnosis_infor.iupr.iuprEgscond = iupt_Denominator;
			OBD_diagnosis_infor.iupr.iuprEgscomp = iupt_Numerator;
			iupr_res = 1;
		}
		else if(iupr_SPN ==3058) //-ok
		{
			OBD_diagnosis_infor.iupr.iuprEgrcond = iupt_Denominator;
			OBD_diagnosis_infor.iupr.iuprEgrcomp = iupt_Numerator;
			iupr_res = 1;
		}
		else if(iupr_SPN ==3306) //-ok
		{
			OBD_diagnosis_infor.iupr.iuprEgrcond = iupt_Denominator;
			OBD_diagnosis_infor.iupr.iuprEgrcomp = iupt_Numerator;
			iupr_res = 1;
		}
		else if(iupr_SPN ==5321)   //OK
		{
			OBD_diagnosis_infor.iupr.iuprBpcond = iupt_Denominator;
			OBD_diagnosis_infor.iupr.iuprBpcomp = iupt_Numerator;
			iupr_res = 1;
		}
		return  iupr_res ;
}



static void J1939IuprProcess(uint8_t *pData, uint8_t DataLen)
{
	uint8_t Pos = 0;
	do
	{
		if((DataLen-Pos)<2)
		{
			break;
		}
		OBD_diagnosis_infor.iupr.iuprLgncntr = (uint16_t)pData[Pos]+(uint16_t)(pData[Pos+1] <<8);
		Pos += 2;
		
		if((DataLen-Pos)<2)
		{
			break;
		}
		OBD_diagnosis_infor.iupr.iuprOBDcond = (uint16_t)pData[Pos]+(uint16_t)(pData[Pos+1] <<8);
		Pos += 2;
		
		if((DataLen-Pos)<7)
		{
			break;
		}

		for(uint8_t i = 0; i< ((sizeof(OBD_diagnosis_infor.iupr) -4)>>2); )
		{
			uint8_t temp_iupr =0;	
			temp_iupr = IuprProcess(((uint32_t)pData[Pos]+(uint32_t)(pData[Pos+1]<<8)+(uint32_t)(pData[Pos+2]<<16)), pData, Pos);
			
			if(temp_iupr ==1)
			{
				i++;
			}
		
			Pos += 7;

			if((DataLen-Pos)<7)
			{
				break;
			}

		}
	}while(0);
}

u8 flag_f101=0;
u8 CAN_flag_f101=0;
//typedef enum
//{
//	CAN_DEFAULT = (uint8_t)0,
//  CAN_NUM1 = (uint8_t)1,
//	CAN_NUM2 = (uint8_t)2
//}CanNum_t;

static uint8_t J1939_can1_2 = 0;
u8 GetIsHave1939Pro(void)
{
	return J1939_can1_2;
}
void J1939CheckPgn(CanData_t *pCanMsg,uint8_t FrameCnt)
{
	uint8_t ValidData[255];
	uint16_t ValidDataLen;
	uint16_t J1939Pgn= 0xFFFF;
	uint8_t CanAdd;

	if((((pCanMsg->CanId >>8)&0xFFFF)==0xECFF)&&(pCanMsg->Data[0]==0x20)&&(pCanMsg->Data[3]+1==FrameCnt))
	{
		ValidDataLen = J1939MultiFrameToData(pCanMsg, FrameCnt, ValidData, sizeof(ValidData));
		if(ValidDataLen==(uint16_t)pCanMsg->Data[1]+(uint16_t)(pCanMsg->Data[2]<<8))
		{
		  J1939Pgn = (uint16_t)(pCanMsg->Data[6] << 8)+(uint16_t)pCanMsg->Data[5];
		}
	}
	else
	{
		ValidDataLen = J1939SingleFrameToData(pCanMsg, FrameCnt, ValidData, sizeof(ValidData));
		if(ValidDataLen==pCanMsg->DataLen)
		{
		  J1939Pgn = (uint16_t)((pCanMsg->CanId >> 8) & 0xFFFF);

		}
	}

	if((J1939Pgn !=0xFECA )&&(J1939Pgn !=0xF101)&&(J1939Pgn !=0x0000)&&(J1939Pgn !=0x9998))			//0xFECA��case�д���
	{
		CanAdd  =(uint8_t) ( pCanMsg->CanId) & 0xFF;
		if(canid_priority_judge(J1939Pgn , CanAdd))		//can add�����Ϲ��� ���ε�canid ֡������
		{
			J1939Pgn = 0xFFFF;	
		}
	}

	if(J1939Pgn == 0xF101)
	{
		if(pCanMsg->CanId != 0x18F10155)
		{
			J1939Pgn = 0xFFFF;		
		}
		else
		{
			flag_f101=1;
			jingli_sensor.state |= 0x01;
			jingli_sensor.CanNum = pCanMsg->CanNum;
		}
	}
	
	if(J1939Pgn == 0x0702)
	{
		if(pCanMsg->CanId != 0x1807023D)
		{
			J1939Pgn = 0xFFFF;		
		}
	}	
	
	if(J1939Pgn == 0x0000)
	{
		if(pCanMsg->CanId != 0x18000002)
		{
			J1939Pgn = 0xFFFF;		
		}
		else
		{
			xinli_sensor.state = 0x01;
			xinli_sensor.CanNum = pCanMsg->CanNum;
		}
	}
	
	if(J1939Pgn == 0x9998)
	{
		if(pCanMsg->CanId != 0x18999805)
		{
			J1939Pgn = 0xFFFF;		
		}
		else
		{
			tiandicheren_sensor.state |= 0x01;
			tiandicheren_sensor.CanNum = pCanMsg->CanNum;
			tiandicheren_sensor.state |= pCanMsg->Data[0] & 0xC0;
			
//			if((pCanMsg->Data[0]>>7)&1)
//			{
//				tiandicheren_sensor.state |= 0x80;
//			}
//			else
//			{
//				tiandicheren_sensor.state &= 0x7F;
//			}
//			
//			if((pCanMsg->Data[0]>>6)&1)	
//			{
//				tiandicheren_sensor.state |= 0x40;
//			}
//			else
//			{
//				tiandicheren_sensor.state &= 0xBF;
//			}
		}
	}
	
	
	switch(J1939Pgn)
	{
		case 0xFECE: //OBD�����Ϣ
		{

			OBD_diagnosis_infor.dntSptStatus.sptClst						=(ValidData[4]   )&1; //
			OBD_diagnosis_infor.dntSptStatus.sptHeatedclst			=(ValidData[4]>>1)&1;//
			OBD_diagnosis_infor.dntSptStatus.sptEprtsys					=(ValidData[4]>>2)&1;//
			OBD_diagnosis_infor.dntSptStatus.sptScdrairss				=(ValidData[4]>>3)&1;//
			OBD_diagnosis_infor.dntSptStatus.sptAcsysrfgr				=(ValidData[4]>>4)&1;	//
			OBD_diagnosis_infor.dntSptStatus.sptEsgassens		  	=(ValidData[4]>>5)&1;
			OBD_diagnosis_infor.dntSptStatus.sptEsgassenshter		=(ValidData[4]>>6)&1;
			OBD_diagnosis_infor.dntSptStatus.sptEgrvvtsys				=(ValidData[4]>>7)&1;  //
			OBD_diagnosis_infor.dntSptStatus.sptColdstartaid	  =(ValidData[5]   )&1;  //
			OBD_diagnosis_infor.dntSptStatus.sptBoostpresctrlsys=(ValidData[5]>>1)&1;
			OBD_diagnosis_infor.dntSptStatus.sptDpt							=(ValidData[5]>>2)&1;
			OBD_diagnosis_infor.dntSptStatus.sptScrnox					=(ValidData[5]>>3)&1;
			OBD_diagnosis_infor.dntSptStatus.sptNmhccvtnclst		=(ValidData[5]>>4)&1;
			OBD_diagnosis_infor.dntSptStatus.sptMisfire					=(ValidData[3]	 )&1;  //
			OBD_diagnosis_infor.dntSptStatus.sptFuelsys					=(ValidData[3]>>1)&1;	//
			OBD_diagnosis_infor.dntSptStatus.sptCphscpnt				=(ValidData[3]>>2)&1;	//

			OBD_diagnosis_infor.dntReadyStatus.rdyClst						=(ValidData[6]	 )&1;
			OBD_diagnosis_infor.dntReadyStatus.rdyHeatedclst			=(ValidData[6]>>1)&1;
			OBD_diagnosis_infor.dntReadyStatus.rdyEprtsys					=(ValidData[6]>>2)&1;
			OBD_diagnosis_infor.dntReadyStatus.rdyScdrairss				=(ValidData[6]>>3)&1;
			OBD_diagnosis_infor.dntReadyStatus.rdyAcsysrfgr				=(ValidData[6]>>4)&1;
			OBD_diagnosis_infor.dntReadyStatus.rdyEsgassens				=(ValidData[6]>>5)&1;
			OBD_diagnosis_infor.dntReadyStatus.rdyEsgassenshter		=(ValidData[6]>>6)&1;
			OBD_diagnosis_infor.dntReadyStatus.rdyEgrvvtsys				=(ValidData[6]>>7)&1;
			OBD_diagnosis_infor.dntReadyStatus.rdyColdstartaid		=(ValidData[7]   )&1;
			OBD_diagnosis_infor.dntReadyStatus.rdyBoostpresctrlsys=(ValidData[7]>>1)&1;
			OBD_diagnosis_infor.dntReadyStatus.rdyDpt							=(ValidData[7]>>2)&1;
			OBD_diagnosis_infor.dntReadyStatus.rdyScrnox					=(ValidData[7]>>3)&1;
			OBD_diagnosis_infor.dntReadyStatus.rdyNmhccvtnclst		=(ValidData[7]>>4)&1;
			OBD_diagnosis_infor.dntReadyStatus.rdyMisfire					=(ValidData[3]>>4)&1; //
			OBD_diagnosis_infor.dntReadyStatus.rdyFuelsys					=(ValidData[3]>>5)&1; //
			OBD_diagnosis_infor.dntReadyStatus.rdyCphscpnt				=(ValidData[3]>>6)&1; //
			
			J1939PgnDisSend(J1939Pgn);
			ISO15765DisPidSend(0x0101); /*��ֹ15765 OBD��Ϣ����*/
			
		}break;
		
		case 0xFEEC:  //VIN
		{
			uint8_t Asc;
      for(uint16_t i=0;i<sizeof(OBD_vehicle_infor.VIN);i++)
			{
				if(i<ValidDataLen)
				{
					if(!IsAscii(ValidData[i]))
					{
						Asc = ValidData[i];
					}
					else
					{
						Asc = '*';
					}
				}
				else
				{
					Asc = '0';
				}
				OBD_vehicle_infor.VIN[i]=Asc;
			}
			J1939PgnDisSend(J1939Pgn);
		}break;
		
		case 0xD300:  //����궨ʶ���
		{
      if(ValidDataLen > 4)
			{
				uint8_t temp;
				uint8_t temp_data;
				uint8_t j = 0;
				for(uint8_t i=0;i<4;i++)		//cvn��4��hexת����8��ascII�󣬱��浽buffer��
				{
						temp = ValidData[i];
						temp_data = (temp>>4)&0x0F;
								if(temp_data<=9)            //���ascii����
										OBD_vehicle_infor.CVN[j] = temp_data + 0x30;
								else 																					//���ascii��д��ĸ
										OBD_vehicle_infor.CVN[j] = temp_data +	0x37;
						j++;
						temp_data=temp&0x0F;
								if(temp_data<=9)            //���ascii����
										OBD_vehicle_infor.CVN[j] = temp_data + 0x30;
								else 																					//���ascii��д��ĸ
										OBD_vehicle_infor.CVN[j] = temp_data +	0x37;
						j++;
				}
				for(uint8_t i=8;i<sizeof(OBD_vehicle_infor.CVN);i++)	//cvnʣ�ಿ�ֲ���0��
				{
						OBD_vehicle_infor.CVN[i]='0';
				}
			if(ValidDataLen >= 20)
			{
				for(uint8_t i=4;i<sizeof(OBD_vehicle_infor.softCbrtNum)+4;i++)		//�����Ǵ�5���ֽڿ�ʼ��cal.ID
				{
	//				if(i<ValidDataLen)
				if(i<20)
					{
//						temp = ValidData[i];
						if(!IsAscii(ValidData[i]))
						{
							temp = ValidData[i];
						}
						else
						{
							temp = '*';
						}
					}
					else
					{
						temp = '0';
					}
					OBD_vehicle_infor.softCbrtNum[i-4]=temp;
				}
			}
				J1939PgnDisSend(J1939Pgn);
			}
		}break;
		
		case 0xC200: //IUPR
		{
			J1939IuprProcess(ValidData,ValidDataLen);
			J1939PgnDisSend(J1939Pgn);
		}break;
		
		case 0xF004:
		{
			if(pCanMsg->CanId==0x0CF00400)
			{
				if(ValidDataLen>4)
				{
					uint16_t temp;
					temp = (uint16_t)(ValidData[4]<<8)+(uint16_t)ValidData[3];
					if(temp <= 0xFAFF)
					{
						OBD_engine_infor.engineRev = temp; //������ת��
		    		ISO15765DisPidSend(0x010C); /*��ֹ15031����ת��*/
						J1939_can1_2=1;
					}
					else
					{
						OBD_engine_infor.engineRev = 0xFFFF;
					}
					if(ValidData[2] <= 0xFF)
					{
						OBD_engine_infor.engineTorq = ValidData[2]; //������Ť��
						ISO15765DisPidSend(0x0162);/*��ֹ15031���󷢶���Ť��*/
					}
					else
					{
  					OBD_engine_infor.engineTorq = 0xFF; //������Ť��
					}
			  }
			}
		}break;
		
		case 0xF003:
		{
			if(ValidDataLen>2)
			{
				if(ValidData[2]<=0xFA)
				{
					OBD_engine_infor.calcLoad = ValidData[2];  //���㸺��
					ISO15765DisPidSend(0x0104); /*��ֹ15031������㸺��*/
				}
				else
				{
					OBD_engine_infor.calcLoad = 0xFF;  //���㸺��
				}
				if(ValidData[1]<=0xFA)
				{
					OBD_engine_infor.position_pedal=ValidData[1]; // ����̤��
				}
				else
				{
					OBD_engine_infor.position_pedal=0xFF; // ����̤��
				}
			}
		}break;
		
		case 0xFEF2:
		{
			if(ValidDataLen>1)
			{
				uint16_t temp;
				temp = ((uint16_t)ValidData[1]<<8)+(uint16_t)ValidData[0];   //����8-msc
				if(temp<=0xFAFF)
				{
					OBD_engine_infor.enginefuelFlow = temp; //������ȼ������
					ISO15765DisPidSend(0x015E); /*��ֹ15031���󷢶���ȼ������*/
				}
				else
				{
					OBD_engine_infor.enginefuelFlow = 0xFFFF; //������ȼ������
				}
			}
		}break;
		
		case 0xFEE9:
		{
			if(ValidDataLen>3)
			{
				uint32_t temp;
				temp = (uint32_t)ValidData[4]+((uint32_t)ValidData[5]<<8)+((uint32_t)ValidData[6]<<16)+((uint32_t)ValidData[7]<<24);
				if(temp<=0xFAFFFFFF)
				{
					OBD_engine_infor.totalfuel= temp; //������ȼ��������
				}
				else
				{
					OBD_engine_infor.totalfuel= 0xFFFFFFFF ;//������ȼ��������
				}
			}
		}break;		
		
		case 0xF101:
		{
			if(ValidDataLen>3)
			{
				uint16_t temp;

				temp = ((uint16_t)ValidData[2]<<8)+((uint16_t)ValidData[1]);
				if(temp!=0xffff)
					OBD_aftertreatment_infor.PMdata = temp/100 ;                 // 16  ԭ��01  PMŨ��Э����ʱ���� ��λת�� û����
				else
					OBD_aftertreatment_infor.PMdata = temp;
				
				temp = ((uint16_t)ValidData[4]<<8)+((uint16_t)ValidData[3]);
				if(temp!=0xffff)
				OBD_aftertreatment_infor.PM_lightabs  = temp/10;   		      //�����ֽ�	 //16   k ������ ������ϵ��0.001 m-1   ƽ̨ 0.01
				else
					OBD_aftertreatment_infor.PM_lightabs  = temp;
				
				temp = ((uint16_t)ValidData[6]<<8)+((uint16_t)ValidData[5]);  //  �����ֽ� ��͸��� ������ϵ��0.1%    ƽ̨��0.1
				OBD_aftertreatment_infor.PM_opaticy = temp ; //  8	
			}
		}break;		
		
		case 0x9998:  //֣��ƽ̨ PM ��λ0.1mg/m3  ��  K����ϵ�� 0.01
		{
			if(ValidDataLen>3)
			{
				uint32_t temp;
				temp = ((uint32_t)ValidData[1]<<24)+((uint32_t)ValidData[2]<<16)+((uint32_t)ValidData[3]<<8)+((uint32_t)ValidData[4]);
				if(temp!=0xffffffff)
				{
//					OBD_aftertreatment_infor.PMdata = (uint16_t)(temp/170);     //������������ /1000 /4  ��λΪ0.1m-1  ת����λΪ0.1  �� *10
//					if(OBD_aftertreatment_infor.PMdata >=50)				
//						OBD_aftertreatment_infor.PM_lightabs = (uint16_t)(OBD_aftertreatment_infor.PMdata*1.9);  //������������ /1000 /1000  ��λΪ1m-1  ת����λΪ0.01 ��*100				
//					else
//						OBD_aftertreatment_infor.PM_lightabs = (uint16_t)(OBD_aftertreatment_infor.PMdata*4);  //������������ /1000 /1000  ��λΪ1m-1  ת����λΪ0.01 ��*100
//					
//					OBD_aftertreatment_infor.PM_opaticy =(uint16_t)(OBD_aftertreatment_infor.PM_lightabs*4);

OBD_aftertreatment_infor.PMdata = (uint16_t)(temp/400 + 15);  //��temp/1000/4 +1.5��->mg/m3   ->ʵ��ֵ*10

float SensorI = ((float)temp)/1000;
float SensorK = 0.03*sqrt(SensorI)+0.015;

OBD_aftertreatment_infor.PM_lightabs =(uint16_t)(SensorK*100);

double SensorN = 100*(1-pow(2.72, -(SensorK *0.43)));

					OBD_aftertreatment_infor.PM_opaticy =(uint16_t)(SensorN*10);
					
					}
				else
				{
					OBD_aftertreatment_infor.PMdata=0xffff;
					OBD_aftertreatment_infor.PM_lightabs=0xffff;	
				}
			}
		}break;	
		
		
		case 0x0000:
		{
			if(ValidDataLen>3)
			{
				uint16_t temp;
				temp = ((uint16_t)ValidData[2]<<8)+((uint16_t)ValidData[3]);
				OBD_aftertreatment_infor.PMdata = temp;                 // 16  ԭ��01  PMŨ��Э����ʱ���� ��λת�� û����

				temp = ((uint16_t)ValidData[4]<<8)+((uint16_t)ValidData[5]);
				if(temp!=0xffff)
						OBD_aftertreatment_infor.PM_lightabs  = temp/10;   		      //�����ֽ�	 //16   k ������ ������ϵ��0.01 m-1   ƽ̨ 0.01
					else
						OBD_aftertreatment_infor.PM_lightabs  = temp; 
				
				temp = ((uint16_t)ValidData[0]<<8)+((uint16_t)ValidData[1]);  //  �����ֽ� ��͸��� ������ϵ��0.1%    ƽ̨��0.1
				OBD_aftertreatment_infor.PM_opaticy = temp ; //  8	
			}
		}break;	
		
		case 0xF00A:
		{
			if(ValidDataLen>1)
			{
				uint16_t temp;
				temp = ((uint16_t)ValidData[3]<<8)+(uint16_t)ValidData[2];  //msc
				if(temp<=0xFAFF)
				{
					OBD_engine_infor.airInflow = temp; //MAF������������
					ISO15765DisPidSend(0x0110);  /*��ֹ15031���������������*/
					ISO15765DisPidSend(0x0150);  /*��ֹ15031�����������ϵ��*/
				}
				else
				{
					OBD_engine_infor.airInflow = 0xFFFF; //MAF������������
				}
			}
		}break;
		
		case 0xFD8B: //Aftertreatment 2 Gas Parameters
		case 0xFD8C: //Aftertreatment 1 Gas Parameters
		{
			if(ValidDataLen>3)
			{
				uint16_t InPressure = (uint16_t)ValidData[0] + (uint16_t)(ValidData[1]<<8);
				uint16_t OutPressure = (uint16_t)ValidData[2] + (uint16_t)(ValidData[3]<<8);
				if((InPressure<=0xFAFF)&&(OutPressure<=0xFAFF))
				{		
					if(OutPressure>InPressure)
					{
						OBD_aftertreatment_infor.diffPres = InPressure - OutPressure;
						ISO15765DisPidSend(0x017A); /*��ֹ15031����DPFѹ��*/
						ISO15765DisPidSend(0x017B); /*��ֹ15031����DPFѹ��*/
					}
					if(J1939Pgn == 0xFD8C)
					{
						J1939PgnDisDone(0xFD8B); //Aftertreatment 2
					}
				}
				else
				{
					OBD_aftertreatment_infor.diffPres = 0xFFFF;
				}
			}
		}break;
		
		case 0xFDAF: //Aftertreatment 2 Intermediate Gas
		case 0xFDB2: //�ɵ��� //Aftertreatment 1 Intermediate Gas
		{
			if(ValidDataLen>5)
			{
				uint16_t DiffPres = (uint16_t)ValidData[4] + (uint16_t)(ValidData[5]<<8);
				if(DiffPres<=0xFAFF)
				{
					OBD_aftertreatment_infor.diffPres = DiffPres;
					ISO15765DisPidSend(0x017A); /*��ֹ15031����DPFѹ��*/
					ISO15765DisPidSend(0x017B); /*��ֹ15031����DPFѹ��*/
			  	if(J1939Pgn == 0xFDB2)
			  	{
			  		J1939PgnDisDone(0xFDAF); //Aftertreatment 2
			  	}
				}
				else
				{
				OBD_aftertreatment_infor.diffPres = 0xFFFF;
				}
		  }
		}break;
		
		case 0xFDB0: //DPF���� Aftertreatment 2 Outlet Gas 2/
		case 0xFDB3: //Aftertreatment 1 Outlet Gas 2
		{
			if(ValidDataLen>3)
			{
				uint16_t temp = (uint16_t)ValidData[2] + (uint16_t)(ValidData[3]<<8);
				if(temp<0xD9F0)
				{
					if(temp > 7456 )
					{
					OBD_aftertreatment_infor.DiffTemp = temp*10/32-2330;
					ISO15765DisPidSend(0x017C); /*��ֹ15031����DPF����*/
					if(J1939Pgn == 0xFDB3)
					{
						J1939PgnDisDone(0xFDB0); //Aftertreatment 2
					}
					}
				}
			}
		}break;
		case 0xFD38:	 //	Aftertreatment 2 SCR Exhaust Gas Temperature
		case 0xFD3E:  //SCRǰ����  //Aftertreatment 1 SCR Exhaust Gas Temperature
		{
			if(ValidDataLen>4)
			{
				uint16_t temp;
				temp = (uint16_t)ValidData[0] + (uint16_t)(ValidData[1]<<8);
				if(temp<=0xFAFF)
				{
					OBD_aftertreatment_infor.inLetTemp = temp;
					ISO15765DisPidSend(0x013C);  /*��ֹ15031����SCRǰ��*/
					ISO15765DisPidSend(0x013D);
					UdsCmdDisSend(UDS_SCR_FRONT_TEMP); /*��ֹUDS����SCRǰ��*/
				}
				else
				{
				OBD_aftertreatment_infor.inLetTemp = 0xFFFF;
				}
				temp = (uint16_t)ValidData[3] + (uint16_t)(ValidData[4]<<8);
				if(temp<=0xFAFF)
				{
					OBD_aftertreatment_infor.outLetTemp = temp;
					ISO15765DisPidSend(0x013E); /*��ֹ15031����SCR����*/
					ISO15765DisPidSend(0x013F);
					UdsCmdDisSend(UDS_SCR_BACK_TEMP);  /*��ֹUDS����SCR����*/
					if(J1939Pgn == 0xFD3E) //Aftertreatment 1
					{
						J1939PgnDisDone(0xFD38); //Aftertreatment 2
					}
				}
				else
				{
				OBD_aftertreatment_infor.outLetTemp = 0xFFFF;
				}

			}
		}break;

		case 0xF00E:  //ǰ��nox //Aftertreatment 1 Intake Gas 1
		case 0xF010: //Aftertreatment 2 Intake Gas 1
//		{
//			if(ValidDataLen>1)
//			{
//				uint16_t temp = (uint16_t)ValidData[0] + (uint16_t)(ValidData[1]<<8);
//				if(temp<=0xFAFF)
//				{
//					OBD_aftertreatment_infor.upstSensValue = temp;
//					ISO15765DisPidSend(0x0183); /*��ֹ15031����NOXŨ��*/
//			  	if(J1939Pgn == 0xF00E) //Aftertreatment 1
//			  	{
//			  		J1939PgnDisDone(0xF010); //Aftertreatment 2
//		  		}
//				}
//				else
//				{
//				OBD_aftertreatment_infor.upstSensValue = 0xFFFF;
//				}
//			}
//		}break;
		
		case 0xF00F:  //���nox //Aftertreatment 1 Outlet Gas 1
		case 0xF011:  //Aftertreatment 2 Outlet Gas 1
		{
			if(ValidDataLen>1)
			{
				APP_Nox_indicate = 1;
				uint16_t temp = (uint16_t)ValidData[0] + (uint16_t)(ValidData[1]<<8);
				if(temp<=0xFAFF)
				{
					OBD_aftertreatment_infor.dnstSensValue = temp;
					ISO15765DisPidSend(0x0183); /*��ֹ15031����NOXŨ��*/
					UdsCmdDisSend(UDS_NOX_CONTEST); /*��ֹUDS����NOXŨ��*/
					if(J1939Pgn == 0xF00F) //Aftertreatment 1
					{
						J1939PgnDisDone(0xF011); //Aftertreatment 2
						J1939PgnDisDone(0xF00E); 
					}
				}
				else
				{
				OBD_aftertreatment_infor.dnstSensValue = 0xFFFF;
				}
			}
		}break;
		
		case 0xFDA1:  //DPFȼ������״̬ //Aftertreatment 1 Fuel Control 1
		case 0xFDA0:	//Aftertreatment 2 Fuel Control 1
		{
			if(ValidDataLen>6)
			{
				uint8_t acvite = (ValidData[6]>>2)&0x03;
				if(acvite==1)
				{
					OBD_aftertreatment_infor.fuleInjectSatus=1;
				}
				else
				{
					if(acvite==0)
					{
						OBD_aftertreatment_infor.fuleInjectSatus=0;
					}
				}
				
				uint16_t temp = (uint16_t)ValidData[2] + (uint16_t)(ValidData[3]<<8);
				if(temp<=0xFAFF)
				{
					OBD_aftertreatment_infor.fuelrate = temp;
					if(J1939Pgn == 0xFDA1) //Aftertreatment 1
					{
						J1939PgnDisDone(0xFDA0); //Aftertreatment 2
					}
				}
				else
				{
					OBD_aftertreatment_infor.fuelrate = 0xFFFF;
				}
			}
		}break;
		
		case 0xFD40: //Ҫ�����ȵ��״̬
		{
			if(ValidDataLen>1)
			{
				uint8_t active;
				for(uint8_t i=0;i<4;i++)
				{
					active = (ValidData[1]>>(i<<1))&0x03;
					if(active==1)
					{
						OBD_aftertreatment_infor.elecCDISatus=1;
						break;
					}
					else
					{
						if(active==0)
						{
							OBD_aftertreatment_infor.elecCDISatus=0;
							break;
						}
					}
				}
			}
		}break;
		
		case 0xFD41: //����ȵ��״̬
		{
			uint8_t active;
			for(uint8_t i=3;i<8;i++)
			{
				if(ValidDataLen<(i+1))
				{
					break;
				}
				active = ValidData[i]&0x03;
				if(active==1)
				{
					OBD_aftertreatment_infor.elecCDISatus=1;
					ISO15765DisPidSend(0x0141); /*��ֹ15031�������ȵ��״̬*/
					break;
				}
				else
				{
					if(active==0)
					{
						OBD_aftertreatment_infor.elecCDISatus=0;
						ISO15765DisPidSend(0x0141); /*��ֹ15031�������ȵ��״̬*/
						break;
					}
				}
			}
		}break;
		case 0x0702:
		case 0xFE56: //����Ũ�� Һλ
		{
			if(ValidDataLen>0)
			{
				APP_reag_indicate=1;
				if(ValidData[0]<=0xFA)
				{
					OBD_aftertreatment_infor.reagAllowance = ValidData[0];
					ISO15765DisPidSend(0x0185);  /*��ֹ15031��������Һλ*/
					UdsCmdDisSend(UDS_UREA_LIQUID_LEVEL);  /*��ֹUDS��������Һλ*/
					
					if(J1939Pgn == 0xFE56) //Aftertreatment 1
					{
						J1939PgnDisDone(0x0702); //Aftertreatment 2
					}
				}
				else
				{
					OBD_aftertreatment_infor.reagAllowance= 0xFF;
				}
				if(J1939Pgn == 0xFE56)
				{
					if(ValidData[1]<=0xFA)
					{
						OBD_aftertreatment_infor.CatalystTemp= ValidData[1]; // �������¶� �Ϻ�
					}
					else
					{
						OBD_aftertreatment_infor.CatalystTemp= 0xFF; // �������¶� �Ϻ�
					}
				}
			}
		}break;
		
		case 0xF023: //����������
		{
			if(ValidDataLen>3)
			{
				uint32_t temp=(uint32_t)ValidData[0]+(uint32_t)(ValidData[1]<<8);
							
				if(temp<=0xFAFF)
				{
					OBD_aftertreatment_infor.DosingReag =temp;// ��λ0.3g/H  ʵʱ����������
				}
				else
				{
					OBD_aftertreatment_infor.DosingReag=0xffffffff;
				}
				temp=(uint32_t)ValidData[3]+(uint32_t)(ValidData[4]<<8);
				
				if(temp <=0xFAFF)
				{
					OBD_aftertreatment_infor.TotalReagCom= temp; // ���������� �Ϻ�  ��λ
				}	
				else
				{
					OBD_aftertreatment_infor.TotalReagCom=0xffffffff;
				}								
			}
		}break;		
	
		
		
		case 0xFEF5:  //����ѹ��
		{
			if(ValidDataLen>0)
			{
				if(ValidData[0]<=0xFA)
				{
					OBD_engine_infor.atmoPres = ValidData[0];
					ISO15765DisPidSend(0x0133); /*��ֹ15031�������ѹ��*/
				}
				else
				{
				OBD_engine_infor.atmoPres = 0xFF;
				}
			}
		}break;
		
		case 0xFEDF:  //Ħ��Ť��
		{
			if(ValidDataLen>0)
			{
				if(ValidData[0]<=0xFA)
				{
				  OBD_engine_infor.fricTorq = ValidData[0];
					ISO15765DisPidSend(0x018E); /*��ֹ15031����Ħ��Ť��*/
				}
				else
				{
					OBD_engine_infor.fricTorq = 0xFF;
				}
			}
		}break;
		
		case 0xFEF1:  //����
		{
			if(ValidDataLen>2)
			{
				uint16_t temp = (uint16_t)ValidData[1] + (uint16_t)(ValidData[2]<<8);
				if(temp<=0xFAFF)
				{
					OBD_vehicle_infor.speed = temp;
					ISO15765DisPidSend(0x010D); /*��ֹ15031������*/
				}
				else
				{
					OBD_vehicle_infor.speed = 0xFFFF;
				}
			}
		}break;
		
		case 0xFEEE:  //��ȴҺ�¶�
		{
			if(ValidDataLen>0)
			{
				if(ValidData[0]<=0xFA)
				{
					OBD_engine_infor.coolantTemp = ValidData[0];
					ISO15765DisPidSend(0x0105); /*��ֹ15031������ȴҺ�¶�*/
					UdsCmdDisSend(UDS_COOLTEMP); //��ֹUDS������ȴҺ
				}
				else
				{
					OBD_engine_infor.coolantTemp = 0xFF;
				}
			}
		}break;
		
		case 0xFEFC:  //����Һλ
		{
			if(ValidDataLen>1)
			{
				if(ValidData[1]<=0xFA)
				{
					OBD_vehicle_infor.liquidLevel = ValidData[1];
					ISO15765DisPidSend(0x012F);  /*��ֹ15031��������Һλ*/
				}
				else
				{
					OBD_vehicle_infor.liquidLevel = 0xFF;
				}
			}
		}break;
		
		case 0xFEE0: //�ۼ����
		{
			if(!IsJ1939PgnDoneEn(J1939Pgn))
			{
				if(ValidDataLen>7)
				{
					uint32_t temp = 0;
					temp += (uint32_t)(ValidData[7]<<24);
					temp += (uint32_t)(ValidData[6]<<16);
					temp += (uint32_t)(ValidData[5]<<8);
					temp += (uint32_t)ValidData[4];
					if(temp<=0xFAFFFFFF)
					{
						OBD_vehicle_infor.mileage = temp*5/4;
	//					J1939PgnDisDone(0xFEC1); //��ֹ����߷ֱ������   //�����ȼ���PGN���ܽ�ֹ�����ȼ���PGN
						ISO15765DisPidSend(0x0917);  /*��ֹ15031�����ۼ����*/
					}
					else
					{
						OBD_vehicle_infor.mileage =0xFFFFFFFF;
					}
				}
		  }
		}break;
		
		case 0xFEC1:  //�߷ֱ������
		{
			if(!IsJ1939PgnDoneEn(J1939Pgn))
			{
				if(ValidDataLen>3)
				{
					uint32_t temp = 0;
					temp += (uint32_t)(ValidData[3]<<24);
					temp += (uint32_t)(ValidData[2]<<16);
					temp += (uint32_t)(ValidData[1]<<8);
					temp += (uint32_t)ValidData[0];
					if(temp<=0xFAFFFFF0)
					{
						OBD_vehicle_infor.mileage = temp/20;
						J1939PgnDisDone(0xFEE0); //��ֹ�������
						ISO15765DisPidSend(0x0917);  /*��ֹ15031�����ۼ����*/
					}
					else
					{
						OBD_vehicle_infor.mileage =0xFFFFFFFF;
					}
				}
		  }
		}break;
		
		case 0xFECA: //����
		{
			if(ValidDataLen>1) //2���ֽڵƵ�״̬
			{
				if(canid_priority_judge(J1939Pgn , (pCanMsg->CanId) & 0xFF)==0)		//can add�����Ϲ��� ���ε�canid ֡������
				{
					if((ValidData[0]>>6)&0x03)
					{
						OBD_diagnosis_infor.MIL_state=1;
					}
					else
					{
						OBD_diagnosis_infor.MIL_state=0;
					}
				}
				for(uint8_t i=0;;i+=4)
				{
					if((ValidDataLen-2-i)<4) //����4���ֽ�
					{
						break;
					}
					if((ValidData[2+i]!=0)||(ValidData[3+i]!=0)||(ValidData[4+i]!=0))
					{
						uint8_t j ;
						for(  j = 0;j < FAULT_CODE_MAX_SIZE ;j++)
						{
							if(	(	OBD_diagnosis_infor.faultCode[j][0]==ValidData[2+i] )&&( OBD_diagnosis_infor.faultCode[j][1]==ValidData[3+i] ) &&
									( OBD_diagnosis_infor.faultCode[j][2]==ValidData[4+i] )&&(	OBD_diagnosis_infor.faultCode[j][3]==ValidData[5+i])
								)
							{
								break;
							}
						}
						if( j == FAULT_CODE_MAX_SIZE )
						{
							if(OBD_diagnosis_infor.faultCodeSum == 0xFF)
							{
								OBD_diagnosis_infor.faultCodeSum =0;
							}
							OBD_diagnosis_infor.faultCode[OBD_diagnosis_infor.faultCodeSum][0]=ValidData[2+i];
							OBD_diagnosis_infor.faultCode[OBD_diagnosis_infor.faultCodeSum][1]=ValidData[3+i];
							OBD_diagnosis_infor.faultCode[OBD_diagnosis_infor.faultCodeSum][2]=ValidData[4+i];
							OBD_diagnosis_infor.faultCode[OBD_diagnosis_infor.faultCodeSum][3]=ValidData[5+i];
							OBD_diagnosis_infor.faultCodeSum += 1;
							if(OBD_diagnosis_infor.faultCodeSum>=(sizeof(OBD_diagnosis_infor.faultCode)/sizeof(OBD_diagnosis_infor.faultCode[0])))
							{
								break;
							}
						}
					}
				} 
				ISO15765DisPidSend(0x0300);  /*��ֹ15031�������*/
				ISO15765DisPidSend(0x0700);  /*��ֹ15031�������*/
			}
		}break;
		
		default:break;
	}
}
void F101_Sensor_Send(uint32_t SendEnFlag)
{
	if(!SendEnFlag)
	{
		if((jingli_sensor.state== 1)&&(jingli_sensor.CanNum != CAN_DEFAULT)) //F101c��ʼ������
		{
			CanTxMessage TxMsg;
			TxMsg.Data[0] = 0xA5;
			TxMsg.Data[1] = 0x5A;
			TxMsg.Data[2] = 0x03;
			TxMsg.Data[3] = 0;
			TxMsg.Data[4] = 0;
			TxMsg.Data[5] = 0;
			TxMsg.Data[6] = 0;
			TxMsg.Data[7] = 0x50;
			TxMsg.DLC = 8;
			TxMsg.IDE = CAN_ID_EXT;
			TxMsg.ExtId = 0x18F1005D;
			TxMsg.RTR = 0;
			uint8_t res=1;
			uint8_t retry = 0;
			do
			{
				if(jingli_sensor.CanNum == CAN_NUM2)
				{
					res = SendCanMsg(&TxMsg);
				}
				else if(jingli_sensor.CanNum == CAN_NUM1) // CAN_NUM1�ǵڶ�· can
				{
					res = CAN1SendMsg(&TxMsg);
				}
				if(res==0)
				{
					break;
				}
				retry += 1;
			}while(retry<3);
			
			jingli_sensor.state |=0x80;
		}
	}
}

void Xinli_0000_Sensor_Send(uint32_t SendEnFlag)
{
	if(!SendEnFlag)
	{
		if((xinli_sensor.state== 1)&&(xinli_sensor.CanNum == CAN_NUM1)) //0000��ʼ������
		{
			CanTxMessage TxMsg;
			
		//						temp = (uint16_t)(ValidData[4]<<8)+(uint16_t)ValidData[3];

				uint16_t temp =OBD_engine_infor.engineRev;
			TxMsg.Data[0] = 0;
			TxMsg.Data[1] = 0;
			TxMsg.Data[2] = 0;
			TxMsg.Data[3] = (uint8_t )(temp&0xFF);
			TxMsg.Data[4] = (uint8_t )((temp>>8)&0xFF);
			TxMsg.Data[5] = 0;
			TxMsg.Data[6] = 0;
			TxMsg.Data[7] = 0;
			TxMsg.DLC = 8;
			TxMsg.IDE = CAN_ID_EXT;
			TxMsg.ExtId = 0x0CF00400;
			TxMsg.RTR = 0;
			uint8_t res=1;
			uint8_t retry = 0;
			do
			{
//				if(xinli_sensor.CanNum == CAN_NUM2)
//				{
//					res = SendCanMsg(&TxMsg);
//				}
//				else 
				if(xinli_sensor.CanNum == CAN_NUM1) // CAN_NUM1�ǵڶ�· can
				{
					res = CAN1SendMsg(&TxMsg);
				}
				if(res==0)
				{
					break;
				}
				retry += 1;
			}while(retry<3);
		}
	}
}

void TianDiCheRen_Sensor_Send(uint32_t SendEnFlag)
{
	if(!SendEnFlag)
	{
		static uint8_t DianJiaRen_sent_cnt =0;
		if(((tiandicheren_sensor.state&1)== 1)&&(DianJiaRen_sent_cnt<6))
		{
			if((((tiandicheren_sensor.state>>6)&1)== 0)&&(tiandicheren_sensor.CanNum != CAN_DEFAULT)&&(DianJiaRen_sent_cnt<3)) //0000��ʼ������
			{
				CanTxMessage TxMsg;
				TxMsg.Data[0] = 0x11;
				TxMsg.Data[1] = 0x01;
				TxMsg.Data[2] = 0;
				TxMsg.Data[3] = 0;
				TxMsg.Data[4] = 0;
				TxMsg.Data[5] = 0;
				TxMsg.Data[6] = 0;
				TxMsg.Data[7] = 0xED;	
			
				TxMsg.DLC = 8;
//				TxMsg.IDE = CAN_ID_EXT;
				TxMsg.IDE = CAN_ID_STD;
//				TxMsg.ExtId = 0x18999705;
				TxMsg.StdId=0x100;
				TxMsg.RTR = 0;
				uint8_t res=1;
				uint8_t retry = 0;
				do
				{
					if(tiandicheren_sensor.CanNum == CAN_NUM2)
					{
						res = SendCanMsg(&TxMsg);
					}
					else if(tiandicheren_sensor.CanNum == CAN_NUM1) // CAN_NUM1�ǵڶ�· can
					{
						res = CAN1SendMsg(&TxMsg);
					}
					if(res==0)
					{
						break;
					}
					retry += 1;
				}while(retry<3);
				DianJiaRen_sent_cnt++;
			}
			else if((((tiandicheren_sensor.state>>7)&1)== 0)&&(tiandicheren_sensor.CanNum != CAN_DEFAULT)&&(DianJiaRen_sent_cnt<6))
			{
				CanTxMessage TxMsg;
				TxMsg.Data[0] = 0x10;
				TxMsg.Data[1] = 0x01;
				TxMsg.Data[2] = 0;
				TxMsg.Data[3] = 0;
				TxMsg.Data[4] = 0;
				TxMsg.Data[5] = 0;
				TxMsg.Data[6] = 0;
				TxMsg.Data[7] = 0xEE;
				TxMsg.DLC = 8;
//				TxMsg.IDE = CAN_ID_EXT;
				TxMsg.IDE = CAN_ID_STD;
//				TxMsg.ExtId = 0x18999705;
				TxMsg.StdId=0x100;
				TxMsg.RTR = 0;
				uint8_t res=1;
				uint8_t retry = 0;
				do
				{
					if(tiandicheren_sensor.CanNum == CAN_NUM2)
					{
						res = SendCanMsg(&TxMsg);
					}
					else if(tiandicheren_sensor.CanNum == CAN_NUM1) // CAN_NUM1�ǵڶ�· can
					{
						res = CAN1SendMsg(&TxMsg);
					}
					if(res==0)
					{
						break;
					}
					retry += 1;
				}while(retry<3);
				DianJiaRen_sent_cnt++;
			}	
			
		}
	}
}

void J1939SendMsg(uint8_t SendEnFlag)
{
	//if(J1939SendDelay>10) //��ʱ���߳���ʱ�й�
	if(!SendEnFlag)
	{
		uint32_t PgnNumCnt = 0;
		do
		{
			J1939SendPgnNum += 1;
			if(J1939SendPgnNum >= (sizeof(J1939PgnList)/sizeof(J1939Pgn_t)))
			{
				J1939SendPgnNum = 0;
			}
			if(J1939PgnList[J1939SendPgnNum].Sendflag)
			{
				if((!(J1939PgnSendState & (((uint64_t)1)<<J1939SendPgnNum))) && (J1939PgnSendCnt[J1939SendPgnNum] < J1939PgnList[J1939SendPgnNum].SendCnt))
				{
					break;
				}
		  }
			PgnNumCnt += 1;
	  }while(PgnNumCnt<(sizeof(J1939PgnList)/sizeof(J1939Pgn_t)));
		
		if(PgnNumCnt<(sizeof(J1939PgnList)/sizeof(J1939Pgn_t)))
		{
			CanTxMessage TxMsg;
			TxMsg.Data[0] = (uint8_t)J1939PgnList[J1939SendPgnNum].Pgn;
			TxMsg.Data[1] = (uint8_t)(J1939PgnList[J1939SendPgnNum].Pgn >> 8);
			TxMsg.Data[2] = 0;
			TxMsg.DLC = 3;
			TxMsg.IDE = CAN_ID_EXT;
			TxMsg.ExtId = 0x18EAFFF9;
			TxMsg.RTR = 0;
			uint8_t res;
			uint8_t retry = 0;
			do
			{
				res = SendCanMsg(&TxMsg);
				if(res==0)
				{
					J1939PgnSendCnt[J1939SendPgnNum] += 1;
					break;
				}
				retry += 1;
			}while(retry<3);
	  }
  }
}

//static void J1939CheckMsg(void)
//{
//	CanData_t *pCanMsg;
//	uint8_t FrameCnt;
//	do
//	{
//		pCanMsg = GetCanData(OBD, &FrameCnt);
//		if(pCanMsg==NULL)
//		{
//			break;
//		}
//		J1939CheckPgn(pCanMsg,FrameCnt);
//	}while(1);
//}

//void J1939Process(uint8_t SendEnFlag)
//{
//	J1939SendMsg(SendEnFlag);
//	J1939CheckMsg();
//}

void J1939Init(void)
{
	J1939SendPgnNum = 0;
  J1939PgnSendState = 0;
  J1939PgnDoneState = 0;

	for(uint8_t i=0; i<  sizeof(J1939PgnList)/sizeof(J1939Pgn_t)  ;i++)
	{
		J1939PgnSendCnt[i] = 0;
		J1939Pgn_Canadd[i] = J1939PgnList[i].Can_Add;
	}


}







