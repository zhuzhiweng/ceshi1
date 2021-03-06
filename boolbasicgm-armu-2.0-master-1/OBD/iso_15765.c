#include "iso_15765.h"
#include "CanDataAnalysis.h"
#include "utils.h"
#include "n32g45x_bkp.h"
#include "UDS.h"

static uint8_t ISO15765SendPidNum = 0;
//static uint16_t ISO15765SendDelay =0;
static uint64_t ISO15765PidSendEn = 0; //0禁止 1允许
static uint64_t ISO15765PidDoneEn = 0; //0禁止 1允许

static uint8_t Dis09sever=0;  // 0 锟斤拷09锟斤拷锟斤拷   1锟截憋拷09锟斤拷锟斤拷
static uint8_t MafPra = 0;

static const ISO15765PID_t ISO15765PidList[] = {
																							{ {0x02,0x09,0x00,0x00,0x00,0x00,0x00,0x00},1,5},   //查询0900段支持的PID
	                                            { {0x02,0x01,0x00,0x00,0x00,0x00,0x00,0x00},0,0},  //查询0100段支持的PID
																							{ {0x02,0x01,0x20,0x00,0x00,0x00,0x00,0x00},0,0},   //查询0120段支持的PID
																							{ {0x02,0x01,0x40,0x00,0x00,0x00,0x00,0x00},0,0},   //查询0140段支持的PID

																							{ {0x02,0x01,0x60,0x00,0x00,0x00,0x00,0x00},0,0},   //查询0160段支持的PID
																							{ {0x02,0x01,0x80,0x00,0x00,0x00,0x00,0x00},0,0},   //查询0180段支持的PID
																							{ {0x02,0x01,0x01,0x00,0x00,0x00,0x00,0x00},1,5},		// MIL状态 /诊断支持状态/诊断就绪状态				(单帧双ECU)
																							//{0x02,0x01,0x41,0x00,0x00,0x00,0x00,0x00},		//通上备用				(单帧双ECU)
																							{ {0x02,0x09,0x02,0x00,0x00,0x00,0x00,0x00},1,5},	//	VIN				(多帧单ECU)

																							{ {0x02,0x09,0x04,0x00,0x00,0x00,0x00,0x00},1,5},	//	软件标定识别号				(多帧单ECU)
																							{ {0x02,0x09,0x06,0x00,0x00,0x00,0x00,0x00},1,5},	//	标定验证码CVN				(多帧双ECU)
																						//{0x02,0x09,0x08,0x00,0x00,0x00,0x00,0x00},	//	IUPR				点燃式发动机				(多帧单ECU) 汽油车不用)
																							{ {0x02,0x09,0x0B,0x00,0x00,0x00,0x00,0x00},1,5},	//	IUPR				压燃式发动机				(多帧单ECU) 柴油车
																							{ {0x01,0x03,0x00,0x00,0x00,0x00,0x00,0x00},0,0},	//		请求历史故障	(存储了的故障)				(多帧单ECU)
																						//{0x01,0x07,0x00,0x00,0x00,0x00,0x00,0x00},	//		请求当前故障	(临时故障)				(多帧单ECU)

																							{ {0x02,0x01,0x85,0x00,0x00,0x00,0x00,0x00},0,0},	//		反应剂余量				10个字节
																							{ {0x02,0x01,0x04,0x00,0x00,0x00,0x00,0x00},0,0},	//		计算负荷				一个字节
																							{ {0x02,0x01,0x05,0x00,0x00,0x00,0x00,0x00},0,0},	//		冷却液温度				一个字节
																							{ {0x02,0x01,0x0C,0x00,0x00,0x00,0x00,0x00},0,0},	//		发动机转速				两个字节

																							{ {0x02,0x01,0x0D,0x00,0x00,0x00,0x00,0x00},0,0},	//		车速				一个字节	
																							{ {0x02,0x01,0x50,0x00,0x00,0x00,0x00,0x00},0,0},  //    MAF 修正参数
																							{ {0x02,0x01,0x10,0x00,0x00,0x00,0x00,0x00},0,0},	//		MAF空气质量流速			两个字节
																							{ {0x02,0x01,0x2F,0x00,0x00,0x00,0x00,0x00},0,0},  //		邮箱液位				一个字节

																							{ {0x02,0x01,0x33,0x00,0x00,0x00,0x00,0x00},0,0},	//		大气压力				一个字节
																							//{ {0x02,0x01,0x41,0x00,0x00,0x00,0x00,0x00},0,0},	//		电加热点火状态				 D 1   ??????????
																							{ {0x02,0x01,0x5E,0x00,0x00,0x00,0x00,0x00},0,0},	//		发动机 燃料流量				两个字节
																							{ {0x02,0x01,0x62,0x00,0x00,0x00,0x00,0x00},0,0},	//		发动机净输出扭矩/实际扭矩				一个字节			//实际扭矩    (0161 别用参考)

																							{ {0x02,0x01,0x8E,0x00,0x00,0x00,0x00,0x00},0,0},	//		发动机摩擦扭矩 	(-125    130)		一个字节
																							{ {0x02,0x01,0x83,0x00,0x00,0x00,0x00,0x00},0,0},	//		Nox传感器输出				九个字节				NOx转化效率
																							{ {0x02,0x01,0x7A,0x00,0x00,0x00,0x00,0x00},0,0},	//		DPF压力差				#1				七个字节
																							{ {0x02,0x01,0x7B,0x00,0x00,0x00,0x00,0x00},0,0},	//		DPF压力差备用				#2				七个字节

																							{ {0x02,0x01,0x7C,0x00,0x00,0x00,0x00,0x00},0,0},	//		DPF后温				九个字节
																							//{0x02,0x01,0x1F,0x00,0x00,0x00,0x00,0x00},	//		本次运行时间
																							//{0x02,0x01,0x7F,0x00,0x00,0x00,0x00,0x00},	//		总运行时间
																							//{0x02,0x09,0x16,0x00,0x00,0x00,0x00,0x00},	//		总运行时间    本次运行时间
																							{ {0x02,0x09,0x17,0x00,0x00,0x00,0x00,0x00},1,5}, 	//		总油耗 总历程 本次油耗  本次里程	
 
																							{ {0x02,0x01,0x3C,0x00,0x00,0x00,0x00,0x00},0,0},
																							{ {0x02,0x01,0x3D,0x00,0x00,0x00,0x00,0x00},0,0},
																							{ {0x02,0x01,0x3E,0x00,0x00,0x00,0x00,0x00},0,0},
																							{ {0x02,0x01,0x3F,0x00,0x00,0x00,0x00,0x00},0,0},	

                                            };

static const uint8_t CanAdd_Blacklist[]={	0x3D,0x7D };  //can结点地址黑名单，协议匹配和数据解析，不处理黑名单中的can节点数据

static uint8_t ISO15765PidSendCnt[sizeof(ISO15765PidList)/sizeof(ISO15765PidList[0])];

 uint8_t iso_15765_canAdd_Blacklist_judge(uint32_t canid)	//返回值=1此帧数据不进行处理 ， 返回值=0；则正常处理
{
	uint8_t CanAdd  =  (uint8_t) canid & 0xFF;
	uint8_t res = 0;
	uint8_t i = 0;

	for(i = 0;i<sizeof(CanAdd_Blacklist)/sizeof(uint8_t);i++ )
	{
		if(CanAdd == CanAdd_Blacklist[i])
		{
			break;
		}
	}

	if(i < sizeof(CanAdd_Blacklist)/sizeof(uint8_t) )
	{
		res  = 1;
	}

	return res;
		
}

static void Dis_15765_09_SID(void)	//Dis  09 sid send
{

	for(uint8_t i = 0;i<sizeof(ISO15765PidList)/sizeof(ISO15765PidList[0]);i++ )
	{
		if(ISO15765PidList[i].Req_inf[1] == 0x09)
		{
			ISO15765PidSendEn &= (~(((uint64_t)1)<<i));
		}
	}
}

void ISO15765DisPidSend(uint16_t ISO15765Pid)
{
	for(uint8_t i=0;i<sizeof(ISO15765PidList)/sizeof(ISO15765PidList[0]);i++)
	{
		if(ISO15765Pid == (uint16_t)(ISO15765PidList[i].Req_inf[1]<<8)+ISO15765PidList[i].Req_inf[2])
		{
			//ISO15765PidSendEn |= (((uint64_t)1)<<i);
			ISO15765PidSendEn &= (~(((uint64_t)1)<<i));
			break;
		}
	}
}

void ISO15765EnPidSend(uint16_t ISO15765Pid)
{
	for(uint8_t i=0;i<sizeof(ISO15765PidList)/sizeof(ISO15765PidList[0]);i++)
	{
		if(ISO15765Pid == (uint16_t)(ISO15765PidList[i].Req_inf[1]<<8)+ISO15765PidList[i].Req_inf[2])
		{
			//ISO15765PidSendEn &= (~(((uint64_t)1)<<i));
			ISO15765PidSendEn |= (((uint64_t)1)<<i);
			break;
		}
	}
}

static void ISO15765DisPidDone(uint16_t ISO15765Pid)
{
	for(uint8_t i=0;i<sizeof(ISO15765PidList)/sizeof(ISO15765PidList[0]);i++)
	{
		if(ISO15765Pid == (uint16_t)(ISO15765PidList[i].Req_inf[1]<<8)+ISO15765PidList[i].Req_inf[2])
		{
			ISO15765PidDoneEn &= (~(((uint64_t)1)<<i));
			break;
		}
	}
}
static uint8_t IsISO15765PidDoneEn(uint16_t ISO15765Pid)
{
	uint8_t res = 0x01;
	for(uint8_t i=0;i<sizeof(ISO15765PidList)/sizeof(ISO15765PidList[0]);i++)
	{
		if(ISO15765Pid == (uint16_t)(ISO15765PidList[i].Req_inf[1]<<8)+ISO15765PidList[i].Req_inf[2])
		{
			if(((uint8_t)(ISO15765PidDoneEn>>i)) & 0x01)
			{
				res = 0;
			}
			break;
		}
	}
	return res;
}

//static uint8_t ISO15765MultiFrameToData(CanData_t *pCanMsg, uint8_t FrameCnt, uint8_t *pData)
//{
//	CanData_t *pFrame;
//	uint8_t GetSize;
//	uint8_t DataLen = 0;
//	if((pCanMsg->Data[0]==0x10)&&(pCanMsg->Data[1]>6)&&(FrameCnt>1)) //是否是多帧
//	{
//		pData[DataLen] = pCanMsg->Data[2];
//		DataLen += 1;
//		pData[DataLen] = pCanMsg->Data[3];
//		DataLen += 1;
//		pData[DataLen] = pCanMsg->Data[4];
//		DataLen += 1;
//		pData[DataLen] = pCanMsg->Data[5];
//		DataLen += 1;
//		pData[DataLen] = pCanMsg->Data[6];
//		DataLen += 1;
//		pData[DataLen] = pCanMsg->Data[7];
//		DataLen += 1;
//		
//		for(uint8_t i=1;i<FrameCnt;i++)
//		{
//			pFrame = GetCanSpecifyFrame(pCanMsg, i);
//			if(pFrame==NULL)
//			{
//				break;
//			}
//			if((pCanMsg->Data[1] - DataLen) >= 7) //每一帧最多7个字节数据
//			{
//				GetSize = 7;
//			}
//			else
//			{
//				GetSize = pCanMsg->Data[1] - DataLen;
//			}
//			for(uint8_t j=0;j<GetSize;j++)
//			{
//				pData[DataLen] = pFrame->Data[1+j];
//				DataLen += 1;
//			}
//		}
//	}
//	return DataLen;
//}

//static uint8_t ISO15765SingleFrameToData(CanData_t *pCanMsg, uint8_t FrameCnt, uint8_t *pData)
//{
//	uint8_t DataLen = 0;
//	if(pCanMsg->Data[0]<8)
//	{
//		for(uint8_t i=0;i<pCanMsg->Data[0];i++)
//		{
//			pData[i] = pCanMsg->Data[1+i];
//			DataLen += 1;
//		}
//  }
//	return DataLen;
//}	

static void ISO15765ObdDiagnosis(uint8_t *pData, uint8_t DataLen)
{
	OBD_diagnosis_infor.MIL_state							       		  =(pData[0]>>7)&1;	//A bit7

	OBD_diagnosis_infor.dntSptStatus.sptMisfire						=(pData[1]		)&1;	//B
	OBD_diagnosis_infor.dntSptStatus.sptFuelsys						=(pData[1]>>1)&1;	
	OBD_diagnosis_infor.dntSptStatus.sptCphscpnt		 			=(pData[1]>>2)&1;

	OBD_diagnosis_infor.dntReadyStatus.rdyMisfire					=(pData[1]>>4)&1;
	OBD_diagnosis_infor.dntReadyStatus.rdyFuelsys					=(pData[1]>>5)&1;
	OBD_diagnosis_infor.dntReadyStatus.rdyCphscpnt				=(pData[1]>>6)&1;	

//  if(((pData[1]>>3)&1)==1)  //==1就是压燃  ==0就是点燃

		OBD_diagnosis_infor.dntSptStatus.sptNmhccvtnclst			=(pData[2]   )&1;
		OBD_diagnosis_infor.dntSptStatus.sptScrnox						=(pData[2]>>1)&1;
		OBD_diagnosis_infor.dntSptStatus.sptBoostpresctrlsys	=(pData[2]>>3)&1;
		OBD_diagnosis_infor.dntSptStatus.sptEsgassens					=(pData[2]>>5)&1;
		OBD_diagnosis_infor.dntSptStatus.sptDpt								=(pData[2]>>6)&1;
  	OBD_diagnosis_infor.dntSptStatus.sptEgrvvtsys				  =(pData[2]>>7)&1;

		OBD_diagnosis_infor.dntReadyStatus.rdyNmhccvtnclst		=(pData[3]	)&1;
		OBD_diagnosis_infor.dntReadyStatus.rdyScrnox					=(pData[3]>>1)&1;
		OBD_diagnosis_infor.dntReadyStatus.rdyBoostpresctrlsys=(pData[3]>>3)&1;
		OBD_diagnosis_infor.dntReadyStatus.rdyEsgassens				=(pData[3]>>5)&1;
		OBD_diagnosis_infor.dntReadyStatus.rdyDpt							=(pData[3]>>6)&1;
		OBD_diagnosis_infor.dntReadyStatus.rdyEgrvvtsys		  	=(pData[3]>>7)&1;

}

static void ISO15765ObdFault(uint8_t *pData, uint8_t DataLen)
{
	if(((pData[0]<<1)+1)==DataLen)//数据是否合法
	{
		OBD_diagnosis_infor.faultCodeSum = pData[0];
		if(OBD_diagnosis_infor.faultCodeSum > (sizeof(OBD_diagnosis_infor.faultCode)/sizeof(OBD_diagnosis_infor.faultCode[0])))
		{
			OBD_diagnosis_infor.faultCodeSum = sizeof(OBD_diagnosis_infor.faultCode)/sizeof(OBD_diagnosis_infor.faultCode[0]);
		}
		for(uint8_t i=0;i<OBD_diagnosis_infor.faultCodeSum;i++)
		{
			OBD_diagnosis_infor.faultCode[i][0] = pData[1+(i<<1)];
			OBD_diagnosis_infor.faultCode[i][1] = pData[2+(i<<1)];
		}
  }
}

static void ISO15765VinProcess(uint8_t *pData, uint8_t DataLen)
{
	uint8_t Asc;
	for(uint8_t i=0;i<sizeof(OBD_vehicle_infor.VIN);i++)
	{
		if(i<DataLen)
		{
			if(!IsAscii(pData[i]))
			{
				Asc = pData[i];
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
		OBD_vehicle_infor.VIN[i] = Asc;
	}
}

static void ISO15765SoftCbrtNum(uint8_t *pData, uint8_t DataLen)
{
	uint8_t Asc;
	for(uint8_t i=0;i<sizeof(OBD_vehicle_infor.softCbrtNum);i++)
	{
		if(i<DataLen)
		{
			if(!IsAscii(pData[i]))
			{
				Asc = pData[i];
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
		OBD_vehicle_infor.softCbrtNum[i] = Asc;
	}
}

static void ISO15765CvnProcess(uint8_t *pData, uint8_t DataLen)
{
	uint8_t Asc;
	uint8_t temp_data = 0;
	uint8_t j = 0;
	for(uint8_t i=0;j<sizeof(OBD_vehicle_infor.CVN);i++)
	{
		if(i<DataLen)
		{
			Asc = pData[i];

				temp_data=(Asc>>4)&0x0F;
						if(temp_data<=9)            //变成ascii数字
								OBD_vehicle_infor.CVN[j] = temp_data + 0x30;
						else 																					//变成ascii大写字母
								OBD_vehicle_infor.CVN[j] = temp_data +	0x37;
				j++;
				temp_data=Asc&0x0F;
						if(temp_data<=9)            //变成ascii数字
								OBD_vehicle_infor.CVN[j] = temp_data + 0x30;
						else 																					//变成ascii大写字母
								OBD_vehicle_infor.CVN[j] = temp_data +	0x37;
				j++;
		}
		else
		{
			Asc = '0';
			OBD_vehicle_infor.CVN[j] = Asc;
			j++;
		}
	//	OBD_vehicle_infor.CVN[i] = Asc;
	}


}


static void ISO15765IuprProcess(uint8_t *pData, uint8_t DataLen)
{
	uint8_t Pos = 0;
	do
	{
		if((DataLen-Pos)<2)
		{
			break;
		}
		OBD_diagnosis_infor.iupr.iuprOBDcond = (uint16_t)(pData[Pos] << 8) + (uint16_t)pData[Pos+1];
		Pos += 2;

		if((DataLen-Pos)<2)
		{
			break;
		}
		OBD_diagnosis_infor.iupr.iuprLgncntr = (uint16_t)(pData[Pos] << 8) + (uint16_t)pData[Pos+1];
		Pos += 2;

		if((DataLen-Pos)<2)
		{
			break;
		}
		OBD_diagnosis_infor.iupr.iuprHccatcomp = (uint16_t)(pData[Pos] << 8) + (uint16_t)pData[Pos+1];
		Pos += 2;

		if((DataLen-Pos)<2)
		{
			break;
		}
		OBD_diagnosis_infor.iupr.iuprHccatcond = (uint16_t)(pData[Pos] << 8) + (uint16_t)pData[Pos+1];
		Pos += 2;

		if((DataLen-Pos)<2)
		{
			break;
		}
		OBD_diagnosis_infor.iupr.iuprNcatcomp = (uint16_t)(pData[Pos] << 8) + (uint16_t)pData[Pos+1];
		Pos += 2;

		if((DataLen-Pos)<2)
		{
			break;
		}
		OBD_diagnosis_infor.iupr.iuprNcatcond = (uint16_t)(pData[Pos] << 8) + (uint16_t)pData[Pos+1];
		Pos += 2;
		
		if((DataLen-Pos)<2)
		{
			break;
		}
		OBD_diagnosis_infor.iupr.iuprNadscomp = (uint16_t)(pData[Pos] << 8) + (uint16_t)pData[Pos+1];
		Pos += 2;
		
		if((DataLen-Pos)<2)
		{
			break;
		}
		OBD_diagnosis_infor.iupr.iuprNadscond = (uint16_t)(pData[Pos] << 8) + (uint16_t)pData[Pos+1];
		Pos += 2;

		if((DataLen-Pos)<2)
		{
			break;
		}
		OBD_diagnosis_infor.iupr.iuprPmcomp = (uint16_t)(pData[Pos] << 8) + (uint16_t)pData[Pos+1];
		Pos += 2;
		
		if((DataLen-Pos)<2)
		{
			break;
		}
		OBD_diagnosis_infor.iupr.iuprPmcond = (uint16_t)(pData[Pos] << 8) + (uint16_t)pData[Pos+1];
		Pos += 2;
		
		if((DataLen-Pos)<2)
		{
			break;
		}
		OBD_diagnosis_infor.iupr.iuprEgscomp = (uint16_t)(pData[Pos] << 8) + (uint16_t)pData[Pos+1];
		Pos += 2;
		
		if((DataLen-Pos)<2)
		{
			break;
		}
		OBD_diagnosis_infor.iupr.iuprEgscond = (uint16_t)(pData[Pos] << 8) + (uint16_t)pData[Pos+1];
		Pos += 2;
		
		if((DataLen-Pos)<2)
		{
			break;
		}
		OBD_diagnosis_infor.iupr.iuprEgrcomp = (uint16_t)(pData[Pos] << 8) + (uint16_t)pData[Pos+1];
		Pos += 2;
		
		if((DataLen-Pos)<2)
		{
			break;
		}
		OBD_diagnosis_infor.iupr.iuprEgrcond = (uint16_t)(pData[Pos] << 8) + (uint16_t)pData[Pos+1];
		Pos += 2;
		
		if((DataLen-Pos)<2)
		{
			break;
		}
		OBD_diagnosis_infor.iupr.iuprBpcomp = (uint16_t)(pData[Pos] << 8) + (uint16_t)pData[Pos+1];
		Pos += 2;
		
		if((DataLen-Pos)<2)
		{
			break;
		}
		OBD_diagnosis_infor.iupr.iuprBpcond = (uint16_t)(pData[Pos] << 8) + (uint16_t)pData[Pos+1];
		Pos += 2;
		
		if((DataLen-Pos)<2)
		{
			break;
		}
		OBD_diagnosis_infor.iupr.iuprFuelcomp = (uint16_t)(pData[Pos] << 8) + (uint16_t)pData[Pos+1];
		Pos += 2;
		
		if((DataLen-Pos)<2)
		{
			break;
		}
		OBD_diagnosis_infor.iupr.iuprFuelcond = (uint16_t)(pData[Pos] << 8) + (uint16_t)pData[Pos+1];
		Pos += 2;
	}while(0);
}

static void ISO15765NoxProcess(uint8_t *pData, uint8_t DataLen)
{
	if(DataLen >= 5)
	{
			uint8_t j = 2;
			uint16_t Temp;
//		Temp = (uint16_t)(pData[1]<<8) + (uint16_t)pData[2];
//		if(Temp==0)
//		{
//			OBD_aftertreatment_infor.upstSensValue = 0xFFFF;
//		}
//		else
//		{
//			OBD_aftertreatment_infor.upstSensValue = (Temp+200)*20;
////			OBD_aftertreatment_infor.upstSensValue = (Temp+200)/100;
//		}
//		if((DataLen >= 7))
//		{
//			Temp = (uint16_t)(pData[3]<<8) + (uint16_t)pData[4];
//			if(Temp==0)
//			{
//				OBD_aftertreatment_infor.dnstSensValue = 0xFFFF;
//			}

//			else
//			{
//				OBD_aftertreatment_infor.dnstSensValue = (Temp+200)*20;
//			}
			if(DataLen >= 9)
			{
				j = 4;
			}
			for( uint8_t i =0; i < j;i++)
			{
				if(((pData[0] >> i)&1) == 1)
				{
					Temp  = (uint16_t)(pData[2*i + 1]<<8) + (uint16_t)pData[2*i + 2];
					if(Temp !=0)
					{
						if((i == 0)||(i == 2))
						{
							OBD_aftertreatment_infor.upstSensValue = (Temp+200)*20;
						}
						if((i == 1)||(i == 3))
						{
								if(	(Temp!=0x0000)&&(Temp!=0xffff) )
								{
									OBD_aftertreatment_infor.dnstSensValue = (Temp+200)*20;
									UdsCmdDisSend(UDS_NOX_CONTEST); /*禁止UDS请求NOX浓度*/
								}
						
						}
					}
				}
			}
			
			if((OBD_aftertreatment_infor.upstSensValue!=0xFFFF)&&(OBD_aftertreatment_infor.dnstSensValue!=0xFFFF))
			{
				if(OBD_aftertreatment_infor.upstSensValue > OBD_aftertreatment_infor.dnstSensValue)
				{
					OBD_aftertreatment_infor.NOxInvertEff = (uint8_t)((OBD_aftertreatment_infor.upstSensValue - OBD_aftertreatment_infor.dnstSensValue)*100/OBD_aftertreatment_infor.upstSensValue);
				}
			}
//	}
	}
}
void ISO15765CheckPid(CanData_t *pCanMsg, uint8_t FrameCnt)
{
//	uint8_t CountFrameCnt;
	uint8_t ValidData[255];
	uint8_t ValidDataLen;
	uint16_t ISO15765Pid = 0;

	
//	if((pCanMsg->CanId & 0xFF) == 0x3D)
//	{
//		return;
//	}
		if(((pCanMsg->CanId) & 0xFFFF0000) != 0)	//不等于0 则判断是是扩展帧
		{
			if( iso_15765_canAdd_Blacklist_judge(pCanMsg->CanId) == 1 )  //此canid中的canadd在黑名单中
			{
				return;
			}
		}

	if((pCanMsg->Data[0]==0x10)&&(FrameCnt>1))
	{
//		if((pCanMsg->Data[1]+1)%7 > 0)
//		{
//			CountFrameCnt = (pCanMsg->Data[1]+1)/7+1;
//		}
//		else
//		{
//			CountFrameCnt = (pCanMsg->Data[1]+1)/7;
//		}
//		if(CountFrameCnt==FrameCnt)
		{
			ValidDataLen = ISO15765MultiFrameToData(pCanMsg, FrameCnt, ValidData);
			if(ValidDataLen==pCanMsg->Data[1])
			{
				if((pCanMsg->Data[2]==0x43) || (pCanMsg->Data[2]==0x47))
				{
					ISO15765Pid = (uint16_t)((pCanMsg->Data[2]-0x40)<<8); //为了统一后面的switch，左移8位
				}
				else
				{
				 ISO15765Pid = (uint16_t)((pCanMsg->Data[2]-0x40) << 8)+pCanMsg->Data[3];
				}
		  }
		}
	}
	else
	{

		ValidDataLen = ISO15765SingleFrameToData(pCanMsg, FrameCnt, ValidData);
		if(ValidDataLen == pCanMsg->Data[0])
		{
			if((pCanMsg->Data[1]==0x43) || (pCanMsg->Data[1]==0x47))
			{
				ISO15765Pid = (uint16_t)((pCanMsg->Data[1]-0x40)<<8); //为了统一后面的switch，左移8位
			}
			else
			{
				ISO15765Pid = (uint16_t)((pCanMsg->Data[1]-0x40) << 8)+pCanMsg->Data[2];
			}
	  }
	}

	if( ISO15765Pid == 0x3F09 )			//diaable all  09 SID send
	{
		Dis_15765_09_SID();
		BKP_WriteBkpData(BKP_DAT8,(uint16_t) 1);
		return;
	}

	if( ISO15765Pid==(uint16_t)(ISO15765PidList[ISO15765SendPidNum].Req_inf[1]<<8)+(uint16_t)ISO15765PidList[ISO15765SendPidNum].Req_inf[2])  //当前发送的PID
	{
		switch(ISO15765Pid)
		{
			case 0x0100:
			case 0x0120:
			case 0x0140:
			case 0x0160:
			case 0x0180:
			case 0x0900:
			{
				if(!IsISO15765PidDoneEn(ISO15765Pid))
				{
					if(ValidDataLen>=6)
					{
            uint32_t temp = (uint32_t)(ValidData[2]<<24) + (uint32_t)(ValidData[3]<<16) + (uint32_t)(ValidData[4]<<8) + (uint32_t)ValidData[5];
						for(uint8_t i=31;;i--)
						{
							if(temp&(((uint32_t)1)<<i))
							{
								ISO15765EnPidSend(ISO15765Pid+(32-i));
							}
							if(i==0)
							{
								break;
							}
						}
						ISO15765DisPidSend(ISO15765Pid); 
					}
					ISO15765DisPidDone(ISO15765Pid); 
				}
			}break;
			
			case 0x0101:  //OBD诊断信息
			{
				if(!IsISO15765PidDoneEn(ISO15765Pid))
				{
					if(ValidDataLen>2)
					{
            ISO15765ObdDiagnosis(&ValidData[2], ValidDataLen-2);
						ISO15765DisPidSend(ISO15765Pid); //obd诊断信息获得一次后就可以不用采集了，关闭请求。
					}
					ISO15765DisPidDone(ISO15765Pid); //关闭处理，只处理最先发送该PID的ECU数据
				}
			}break;
			
			case 0x0902: //VIN
			{
				if(!IsISO15765PidDoneEn(ISO15765Pid))
				{
					if(ValidDataLen>3)
					{
						if(( IsAscii(ValidData[3]) == 0) &&( IsAscii(ValidData[4]) == 0) )
						{
							ISO15765VinProcess(&ValidData[3], ValidDataLen-3);
						}
						ISO15765DisPidDone(ISO15765Pid); //关闭处理，只处理最先发送该PID的ECU数据
					}
					ISO15765DisPidSend(ISO15765Pid); //VIN信息获得一次后就可以不用采集了，关闭请求。
				}
			}break;
			
			case 0x0904: //软件标定识别号
			{
				if(!IsISO15765PidDoneEn(ISO15765Pid))
				{
					if(ValidDataLen>3)
					{
						if(( IsAscii(ValidData[3]) == 0) &&( IsAscii(ValidData[4]) == 0) )
						{
							ISO15765SoftCbrtNum(&ValidData[3], ValidDataLen-3);
						}
					  ISO15765DisPidDone(ISO15765Pid); //关闭处理，只处理最先发送该PID的ECU数据
					}
					ISO15765DisPidSend(ISO15765Pid); //软件标定识别号获得一次后就可以不用采集了，关闭请求。
				}
			}break;
			case 0x0906: //CVN
			{
				if(!IsISO15765PidDoneEn(ISO15765Pid))
				{
          if(ValidDataLen>3)
					{
					  ISO15765CvnProcess(&ValidData[3], ValidDataLen-3);
					  ISO15765DisPidDone(ISO15765Pid); //关闭处理，只处理最先发送该PID的ECU数据
					}
					ISO15765DisPidSend(ISO15765Pid); //CVN获得一次后就可以不用采集了，关闭请求。
				}
			}break;
			
			case 0x090B: //IUPR
			{
				if(!IsISO15765PidDoneEn(ISO15765Pid))
				{
					if(ValidDataLen>3)
					{
					  ISO15765IuprProcess(&ValidData[3], ValidDataLen-3);
						ISO15765DisPidDone(ISO15765Pid); //关闭处理，只处理最先发送该PID的ECU数据
					}
					ISO15765DisPidSend(ISO15765Pid); //CVN获得一次后就可以不用采集了，关闭请求。
				}
			}break;
			//故障处理，PID是0x03,0x07为了统一switch左移了8位
			case 0x0300:
			case 0x0700:
			{
				if(!IsISO15765PidDoneEn(ISO15765Pid))
				{
					if(ValidDataLen>1)
					{
            ISO15765ObdFault(&ValidData[1], ValidDataLen-1);
					}
					ISO15765DisPidDone(ISO15765Pid);
				}
			}break;
			
			case 0x010D: //车速
			{
				if(!IsISO15765PidDoneEn(ISO15765Pid))
				{
					if(ValidDataLen>2)
					{
						if(ValidData[2]!=0xFF)
						{
							OBD_vehicle_infor.speed = (ValidData[2]<<8);
						}
					}
					ISO15765DisPidDone(ISO15765Pid);
				}
			}break;
			
			case 0x0133: //大气压力
			{
				if(!IsISO15765PidDoneEn(ISO15765Pid))
				{
					if(ValidDataLen>2)
					{
						if(ValidData[2]!=0xFF)
						{
							OBD_engine_infor.atmoPres = (ValidData[2]<<1);
						}
					}
					ISO15765DisPidDone(ISO15765Pid);
				}
			}break;
			case 0x013C: //  Bank 1, Sensor 1 ，Sensor 1是前温  2是后温
			{
				if(!IsISO15765PidDoneEn(ISO15765Pid))
				{
					if(ValidDataLen>2)
					{
						u16 temp;
						temp=(uint16_t)(ValidData[2]<<8)+(uint16_t)ValidData[3];
					if(	(temp<8000)&&(temp!=0x0000) )
						{
							OBD_aftertreatment_infor.inLetTemp = temp*3.2 + 7456 ; // ((X*0.1-40) + 273 ) *32
							ISO15765DisPidSend(0x013D);
							UdsCmdDisSend(UDS_SCR_FRONT_TEMP); /*禁止UDS请求SCR前温*/
						}
					}
					ISO15765DisPidDone(ISO15765Pid);
				}
			}break;
			case 0x013D: //Bank 2, Sensor 1  ，    Sensor1是前温  2是后温
			{
				if(!IsISO15765PidDoneEn(ISO15765Pid))
				{
					if(ValidDataLen>2)
					{
						u16 temp;
						temp=(uint16_t)(ValidData[2]<<8)+(uint16_t)ValidData[3];
					if(	(temp<8000)&&(temp!=0x0000) )
						{
							OBD_aftertreatment_infor.inLetTemp = temp*3.2 + 7456 ;  // ((X*0.1-40) + 273 ) *32
							UdsCmdDisSend(UDS_SCR_FRONT_TEMP); /*禁止UDS请求SCR前温*/
						}
					}
					ISO15765DisPidDone(ISO15765Pid);
				}
			}break;

			case 0x013E://Bank 1, Sensor 2
			{
				if(!IsISO15765PidDoneEn(ISO15765Pid))
				{
					if(ValidDataLen>2)
					{
						u16 temp;
						temp=(uint16_t)(ValidData[2]<<8)+(uint16_t)ValidData[3];
						if(	(temp<8000)&&(temp!=0x0000) )
						{
							OBD_aftertreatment_infor.outLetTemp = temp*3.2 + 7456 ;  // ((X*0.1-40) + 273 ) *32
							UdsCmdDisSend(UDS_SCR_BACK_TEMP);  /*禁止UDS请求SCR后温*/
							ISO15765DisPidSend(0x013F);
						}
					}
					ISO15765DisPidDone(ISO15765Pid);
				}
			}break;
			case 0x013F: //Bank 2, Sensor 2
			{
				if(!IsISO15765PidDoneEn(ISO15765Pid))
				{
					if(ValidDataLen>2)
					{
						u16 temp;
						temp=(uint16_t)(ValidData[2]<<8)+(uint16_t)ValidData[3];
						if(	(temp<8000)&&(temp!=0x0000) )
						{
							OBD_aftertreatment_infor.outLetTemp =  temp*3.2 + 7456 ;  // ((X*0.1-40) + 273 ) *32
							UdsCmdDisSend(UDS_SCR_BACK_TEMP);  /*禁止UDS请求SCR后温*/
						}
					}
					ISO15765DisPidDone(ISO15765Pid);
				}
			}break;
			
			case 0x0162: //发动机净输出扭矩/实际扭矩
			{
				if(!IsISO15765PidDoneEn(ISO15765Pid))
				{
					if(ValidDataLen>2)
					{
						OBD_engine_infor.engineTorq = ValidData[2];
					}
					ISO15765DisPidDone(ISO15765Pid);
				}
			}break;
			
			case 0x018E: //发动机摩擦扭矩
			{
				if(!IsISO15765PidDoneEn(ISO15765Pid))
				{
					if(ValidDataLen>2)
					{
						OBD_engine_infor.fricTorq = ValidData[2];
					}
					ISO15765DisPidDone(ISO15765Pid);
				}
			}break;
			
			case 0x010C: //发动机转速
			{
				if(!IsISO15765PidDoneEn(ISO15765Pid))
				{
					if(ValidDataLen>3)
					{
						OBD_engine_infor.engineRev = ((uint16_t)(ValidData[2]<<8)+(uint16_t)ValidData[3])<<1;
					}
					ISO15765DisPidDone(ISO15765Pid);
				}
			}break;
			
			case 0x015E: //燃料流量
			{
				if(!IsISO15765PidDoneEn(ISO15765Pid))
				{
					if(ValidDataLen>3)
					{
				//		OBD_engine_infor.enginefuelFlow = ((uint16_t)(ValidData[2]<<8)+(uint16_t)ValidData[3])<<1; msc
							OBD_engine_infor.enginefuelFlow = ((uint16_t)(ValidData[2]<<8)+(uint16_t)ValidData[3]);
					}
					ISO15765DisPidDone(ISO15765Pid);
				}
			}break;
//			
			case 0x0183: //NOX传感器
			{
				if(!IsISO15765PidDoneEn(ISO15765Pid))
				{
					if(ValidDataLen>=7)
					{
						ISO15765NoxProcess(&ValidData[2], ValidDataLen-2);
					}
					ISO15765DisPidDone(ISO15765Pid);
				}
			}break;
			
			case 0x017A: //DPF压差传感器  Diesel Particulate Filter Bank 1
			{
				if(!IsISO15765PidDoneEn(ISO15765Pid))
				{
					if(ValidDataLen>4)
					{
						uint16_t temp;
						temp = (uint16_t)(ValidData[3]<<8)+(uint16_t)ValidData[4];
						if(temp&0x8000)
						{
							OBD_aftertreatment_infor.diffPres = 0;
						}
						else
						{
							OBD_aftertreatment_infor.diffPres = temp/10;
						}
						ISO15765DisPidSend(0x017B);
					}
					ISO15765DisPidDone(ISO15765Pid);
				}
			}break;
			
			case 0x017B: //DPF压差传感器
			{
				if(!IsISO15765PidDoneEn(ISO15765Pid))
				{
					if(ValidDataLen>4)
					{
						uint16_t temp;
						temp = (uint16_t)(ValidData[3]<<8)+(uint16_t)ValidData[4];
						if(temp&0x8000)
						{
							OBD_aftertreatment_infor.diffPres = 0;
						}
						else
						{
							OBD_aftertreatment_infor.diffPres = temp/10;
						}
					}
					ISO15765DisPidDone(ISO15765Pid);
				}
			}break;
			case 0x0150: //MAF空气质量流速 系数 0150
			{
				if(!IsISO15765PidDoneEn(ISO15765Pid))
				{
					if(ValidDataLen>=3)
					{
							MafPra = ValidData[2];
					}
					ISO15765DisPidDone(ISO15765Pid);
					ISO15765DisPidSend(ISO15765Pid); 
				}
			}break;

			case 0x0110: //MAF空气质量流速
			{
				if(!IsISO15765PidDoneEn(ISO15765Pid))
				{
					if(ValidDataLen>3)
					{
						uint16_t temp;
						temp = (uint16_t)(ValidData[2]<<8)+(uint16_t)ValidData[3];
						if(temp!=0xFFFF)
						{
							if(MafPra != 0)
							{
								uint32_t MAF_0150 = (temp*720*MafPra)/65535;
								OBD_engine_infor.airInflow = (uint16_t)MAF_0150 ;			// *3.6*20 *MAF_0150 *10/65535
							}
							else
								OBD_engine_infor.airInflow = (temp*3.6)/5;		//     *3.6*20 /100
						}
					}
					ISO15765DisPidDone(ISO15765Pid);
				}
			}break;
			
//			case 0x0110: //MAF空气质量流速
//			{
//				if(!IsISO15765PidDoneEn(ISO15765Pid))
//				{
//					if(ValidDataLen>3)
//					{
//						uint16_t temp;
//						temp = (uint16_t)(ValidData[2]<<8)+(uint16_t)ValidData[3];
//						if(temp!=0xFFFF)
//						{
//							//OBD_engine_infor.airInflow = temp/18;
//								OBD_engine_infor.airInflow = (temp*3.6)/5;
//						}
//					}
//					ISO15765DisPidDone(ISO15765Pid);
//				}
//			}break;


			
			case 0x0185: //反应剂余量
			{
				if(!IsISO15765PidDoneEn(ISO15765Pid))
				{
					if(ValidDataLen>7)
					{
						if(ValidData[2]&0x04)
						{
							if(ValidData[7]!=0x00)
							{
								//OBD_aftertreatment_infor.reagAllowance = ValidData[7]*255/250;
								OBD_aftertreatment_infor.reagAllowance = ValidData[7]*250/255;							
								UdsCmdDisSend(UDS_UREA_LIQUID_LEVEL);  /*禁止UDS请求尿素液位*/
							}
						}
					}
					ISO15765DisPidDone(ISO15765Pid);
				}
			}break;
			
			case 0x0105: //冷却液温度
			{
				if(!IsISO15765PidDoneEn(ISO15765Pid))
				{
					if(ValidDataLen>2)
					{
            OBD_engine_infor.coolantTemp = ValidData[2];
					}
					if(ValidData[2]!=0xff)
						UdsCmdDisSend(UDS_COOLTEMP); //禁止UDS请求冷却液
					ISO15765DisPidDone(ISO15765Pid);
					
				}
			}break;
			
			case 0x012F: //油箱液位
			{
				if(!IsISO15765PidDoneEn(ISO15765Pid))
				{
					if(ValidDataLen>2)
					{
						if(ValidData[2]!=0xFF)
						{
              OBD_vehicle_infor.liquidLevel = ValidData[2]*250/255;				//0704
						}
					}
					ISO15765DisPidDone(ISO15765Pid);
				}
			}break;
			
			case 0x0104: //计算负荷
			{
				if(!IsISO15765PidDoneEn(ISO15765Pid))
				{
					if(ValidDataLen>2)
					{
						if(ValidData[2]!=0xFF)
						{
              OBD_engine_infor.calcLoad = ValidData[2]*100/255;
						}
					}
					ISO15765DisPidDone(ISO15765Pid);
				}
			}break;
			
			case 0x017C: //DPF后温
			{
				if(!IsISO15765PidDoneEn(ISO15765Pid))
				{
					if(ValidDataLen>4)
					{
            OBD_aftertreatment_infor.DiffTemp = (uint16_t)(ValidData[5]<<8)+(uint16_t)ValidData[6];
					}
					ISO15765DisPidDone(ISO15765Pid);
				}
			}break;
			
			case 0x0917: //累计里程
			{
				if(!IsISO15765PidDoneEn(ISO15765Pid))
				{
					if(ValidDataLen>10)
					{
            OBD_vehicle_infor.mileage = (uint32_t)(ValidData[7]<<24)+(uint32_t)(ValidData[8]<<16)+(uint32_t)(ValidData[9]<<8)+(uint32_t)ValidData[10];
					}
					ISO15765DisPidDone(ISO15765Pid);
				}
			}break;
			
			default:break;
		}
  }
}

void ISO15765SendMsg(uint8_t Procotol, uint8_t SendEnFlag)
{
	if(!SendEnFlag)
	{
		uint32_t PidNumCnt = 0;
		do
		{
			ISO15765SendPidNum += 1;
			if(ISO15765SendPidNum >= (sizeof(ISO15765PidList)/sizeof(ISO15765PidList[0])))
			{
				ISO15765SendPidNum = 0;
			}
			if((ISO15765PidSendEn & (((uint64_t)1)<<ISO15765SendPidNum)) && (ISO15765PidSendCnt[ISO15765SendPidNum] <= ISO15765PidList[ISO15765SendPidNum].SendCnt) )
			{
				break;
			}
			PidNumCnt += 1;
	  }while(PidNumCnt<(sizeof(ISO15765PidList)/sizeof(ISO15765PidList[0])));
		
		if(PidNumCnt<(sizeof(ISO15765PidList)/sizeof(ISO15765PidList[0])))
		{
			CanTxMessage TxMsg;
      memcpy(TxMsg.Data,&ISO15765PidList[ISO15765SendPidNum].Req_inf[0],8);
			TxMsg.DLC = 8;
			TxMsg.RTR = 0;
			if(Procotol==ISO15765_11_500||Procotol==ISO15765_11_250)
			{
				TxMsg.IDE = CAN_ID_STD;
			  TxMsg.StdId = 0x7DF;
			}
			else
			{
  			TxMsg.IDE = CAN_ID_EXT;
			  TxMsg.ExtId = 0x18DB33F1;
			}
			uint8_t res;
			uint8_t retry = 0;
			do
			{
				res = SendCanMsg(&TxMsg);
				if(res==0)
				{
					if(ISO15765PidList[ISO15765SendPidNum].Cntflag)
					{
						ISO15765PidSendCnt[ISO15765SendPidNum] +=1;
					}
					break;
				}
				retry += 1;
			}while(retry<3);
			ISO15765PidDoneEn |= (((uint64_t)1)<<ISO15765SendPidNum); //允许处理该PID
	  }
  }
}

//static void ISO15765CheckMsg(void)
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
//		ISO15765CheckPid(pCanMsg,FrameCnt);
//	}while(1);
//}

//void ISO15765Process(ProtocolNum_t Procotol, uint8_t SendEnFlag)
//{
//	ISO15765SendMsg(Procotol, SendEnFlag);
//	ISO15765CheckMsg();
//}
void ISO15765Init(void)
{
	for(uint8_t i=0; i<  sizeof(ISO15765PidList)/sizeof(ISO15765PidList[0]) ;i++)
	{
		ISO15765PidSendCnt[i] = 0;
	}
	ISO15765SendPidNum = 0;
	ISO15765PidSendEn = 0;
	ISO15765PidDoneEn = 0;
	MafPra = 0;

	uint16_t DR8_vale = BKP_ReadBkpData(BKP_DAT8);

	if( DR8_vale == 0 )
	{
	ISO15765EnPidSend(0x0900);
	}
//	if(Dis09sever)															// 郑州修改，不采集09服务 防止重汽车辆采集异常
	Dis_15765_09_SID();
  ISO15765EnPidSend(0x0100);
	ISO15765EnPidSend(0x0300);
}

void Dis15765_09Sever(void)
{
		Dis09sever=1;
}
	


