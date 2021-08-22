#include "CanDataAnalysis.h"
//#include "CAN.h"


CanData_t CanData[CAN_DATA_BUF_SIZE]={0};
//uint16_t ObdCanDataReadPos = 0;
//uint16_t UdsCanDataReadPos = 0;
uint16_t CanDataReadPos = 0;
uint16_t CanDataWritePos = 0;
MultiFrame_t CanMultiFrame[CAN_MULTI_FRAME_BUF_SIZE]={0};
uint8_t CanMultiFramePos = 0;
MultiFrameNode_t MultiFrameNode[CAN_NODE_BUF_SIZE]={0};


static CanData_t* GetCanDataEmptyBuf(void)
{
//	uint16_t i;
	CanData_t* EmtpyPos = NULL;
//	for(i=0;i<CAN_DATA_BUF_SIZE;i++)
//	{
//		if(CanData[i].State==(uint8_t)CAN_DATA_STATE_EMPTY)
//		{
//			break;
//		}
//	}
//	if(i<CAN_DATA_BUF_SIZE)
//	{
//		EmtpyPos = &CanData[i];
//	}
//	return EmtpyPos;
	if(CanDataWritePos>=CAN_DATA_BUF_SIZE)
	{
		CanDataWritePos = 0;
	}
	if(CanData[CanDataWritePos].State==(uint8_t)CAN_DATA_STATE_EMPTY)
	{
		EmtpyPos = &CanData[CanDataWritePos];
		CanDataWritePos += 1;
	}
	return EmtpyPos;
}
static uint16_t GetCanDataEmptyBufCnt(void)
{
	uint16_t EmtpyCnt = 0;
//	for(uint16_t i=0;i<CAN_DATA_BUF_SIZE;i++)
//	{
//		if(CanData[i].State==(uint8_t)CAN_DATA_STATE_EMPTY)
//		{
//			EmtpyCnt += 1;
//		}
//	}
//	return EmtpyCnt;
	uint16_t WritePosTemp = CanDataWritePos;
	do{
		if(WritePosTemp>=CAN_DATA_BUF_SIZE)
		{
			WritePosTemp = 0;
		}
		if(CanData[WritePosTemp].State==(uint8_t)CAN_DATA_STATE_EMPTY)
		{
			EmtpyCnt += 1;
			WritePosTemp += 1;
		}
		else
		{
			break;
		}
	}while(EmtpyCnt<CAN_DATA_BUF_SIZE);
	
	return EmtpyCnt;
}
static uint8_t GetCanDataFrameCnt(CanData_t *pHead)
{
	uint8_t FrameCnt = 0;
	CanData_t *pNodeTemp = pHead;
	do
	{
		if(pNodeTemp==NULL)
		{
			break;
		}
		FrameCnt += 1;
		pNodeTemp = pNodeTemp->Next;
	}while(FrameCnt<255);
	return FrameCnt;
}

static MultiFrameNode_t* GetMultiFrameNodeEmptyBuf(void)
{
	uint16_t i;
	MultiFrameNode_t* EmtpyPos = NULL;
	for(i=0;i<CAN_NODE_BUF_SIZE;i++)
	{
		if(MultiFrameNode[i].Empty==0)
		{
			break;
		}
	}
	if(i<CAN_NODE_BUF_SIZE)
	{
		EmtpyPos = &MultiFrameNode[i];
	}
	return EmtpyPos;
}


static void ReleaseMultiFrameBuf(MultiFrame_t *pMultiFrame)
{
	MultiFrameNode_t *pNodeHeadTemp,*pNodeTemp;
	if(pMultiFrame!=NULL)
	{
		pMultiFrame->Empty = 0;
		pMultiFrame->CanNum = CAN_DEFAULT;
		pMultiFrame->FrameCnt = 0;
		pMultiFrame->FrameSoureAdd = 0;
		pNodeHeadTemp = pMultiFrame->pHead;
		pMultiFrame->pHead = NULL;
		do
		{
			if(pNodeHeadTemp==NULL)
			{
				break;
			}
			pNodeHeadTemp->Empty = 0;
			pNodeHeadTemp->NodeNum = 0;
			if(pNodeHeadTemp->pMsg!=NULL)
			{
				ReleaseCanRxBuf(pNodeHeadTemp->pMsg);
			}
			pNodeHeadTemp->pMsg = NULL;
			pNodeTemp = pNodeHeadTemp->Next;
			pNodeHeadTemp->Next = NULL;
			pNodeHeadTemp = pNodeTemp;
		}while(1);
	}
}

static MultiFrame_t* GetMultiFrameEmptyBuf(CanRxMsg_t* pRxMsg)
{
	uint8_t i;
	MultiFrame_t* EmtpyPos = NULL;
	/*遍历没有收全的多帧*/
	for(i=0;i<CAN_MULTI_FRAME_BUF_SIZE;i++)
	{
		if((CanMultiFrame[i].Empty==1)&&(CanMultiFrame[i].pHead!=NULL)&&(CanMultiFrame[i].pHead->pMsg->CanId==pRxMsg->CanId)&& \
			 (CanMultiFrame[i].CanNum==pRxMsg->CanNum))
		{
			EmtpyPos = &CanMultiFrame[i];
			break;
		}
	}
	if(EmtpyPos == NULL)
	{
		CanMultiFramePos += 1;
		if(CanMultiFramePos>=CAN_MULTI_FRAME_BUF_SIZE)
		{
			CanMultiFramePos = 0;
		}
		EmtpyPos = &CanMultiFrame[CanMultiFramePos];
  }
	ReleaseMultiFrameBuf(EmtpyPos);
	return EmtpyPos;
}


static MultiFrameNode_t *GetMultiFrameNodeEnd(MultiFrame_t *pMultiFrame)
{
	MultiFrameNode_t *NodeEnd = NULL;
	if(pMultiFrame!=NULL)
	{
		MultiFrameNode_t *pNodeTemp = pMultiFrame->pHead;
		do
		{
			if(pNodeTemp==NULL)
			{
				break;
			}
			NodeEnd = pNodeTemp;
			pNodeTemp = pNodeTemp->Next;
		}while(1);
	}
	return NodeEnd;
}

void ReleaseCanDataBuf(CanData_t *pHead)
{
	CanData_t *pNodeNext;
  CanData_t *pNodeTemp = pHead;
	do
	{
		if(pNodeTemp==NULL)
		{
		 break;
		}
		pNodeTemp->State = (uint8_t)CAN_DATA_STATE_EMPTY;
		pNodeTemp->Head = 0;
		pNodeTemp->DataLen = 0;
		pNodeTemp->CanId = 0;
		pNodeTemp->Procotol = DEFAULT;
		pNodeTemp->CanNum = CAN_DEFAULT;
		pNodeNext = pNodeTemp->Next;
		pNodeTemp->Next = NULL;
		pNodeTemp = pNodeNext;
	}while(1);
}

static void J1939MultiFrameProcess(CanRxMsg_t* pRxMsg, MultiFrame_t *pMultiFrame)
{
	MultiFrameNode_t *pNodeEnd,*pNodeNext;
	pNodeEnd = GetMultiFrameNodeEnd(pMultiFrame); //得到帧尾
	pNodeNext = GetMultiFrameNodeEmptyBuf();
	if((pNodeEnd != NULL)&&(pNodeEnd->pMsg != NULL)&&(pNodeNext!=NULL))
	{
		if((pRxMsg->CanData[0] <= pMultiFrame->FrameCnt)&&(pNodeEnd->pMsg->CanNum==pRxMsg->CanNum)&& \
			 (((pMultiFrame->pHead==pNodeEnd)&&(pRxMsg->CanData[0]==0x01))||(pRxMsg->CanData[0] == pNodeEnd->pMsg->CanData[0]+1))) //是否时多帧的下一帧
		{
			//在帧尾插入帧
			pNodeEnd->Next = pNodeNext;
			pNodeNext->NodeNum = pNodeEnd->NodeNum+1;
			pNodeNext->pMsg = pRxMsg;
			pNodeNext->Next = NULL;
			pNodeNext->Empty = 1;
		 //置位底层BUF已处理标志
			SetCanRxBufDoneFlag(pRxMsg);
		}
		else
		{
			ReleaseCanRxBuf(pRxMsg);
			//ReleaseMultiFrameBuf(pMultiFrame); //可能丢帧了，收到的也全部丢弃。屏蔽 20190620,hxy 
		}
		//if(pRxMsg->CanData[0] == pMultiFrame->FrameCnt) //收到完整的多帧
	  if(pNodeNext->NodeNum == pMultiFrame->FrameCnt) //收到完整的多帧，FrameCnt不包含首帧
		{
			CanData_t* pCanData;
			CanData_t* pCanDataNode = NULL;
			CanData_t* pNodeHead = NULL;
			pNodeNext = pMultiFrame->pHead;
      if(GetCanDataEmptyBufCnt()>=(pMultiFrame->FrameCnt+1)) //加上首帧
			{
				uint16_t i;
				for(i=0;i<(pMultiFrame->FrameCnt+1);i++)
				{
					pCanData = GetCanDataEmptyBuf();
					if((pCanData==NULL)||(pNodeNext==NULL)||(pNodeNext->pMsg==NULL))
					{
						break;
					}
					if(pCanDataNode==NULL)
					{
						pCanDataNode = pCanData;
						pNodeHead = pCanData;
						pCanData->Head = 1;
					}
					else
					{
						pCanDataNode->Next = pCanData;
						pCanData->Head = 0;
					}
					pCanData->CanId = pNodeNext->pMsg->CanId;
					pCanData->DataLen = pNodeNext->pMsg->DataLen;
					for(uint8_t j=0;j<pCanData->DataLen;j++)
					{
						pCanData->Data[j] = pNodeNext->pMsg->CanData[j];
					}
					pCanData->State = (uint8_t)CAN_DATA_STATE_FULL;
					pCanData->Procotol = J1939;
					pCanData->CanNum = pRxMsg->CanNum;
					pCanData->Next = NULL;
					pNodeNext = pNodeNext->Next;
					pCanDataNode = pCanData;
				}
				if(i<(pMultiFrame->FrameCnt+1))
				{
					ReleaseCanDataBuf(pNodeHead);
				}
			}
			ReleaseMultiFrameBuf(pMultiFrame);
		}
	}
}

static void J1939ProtocolProcess(CanRxMsg_t* pRxMsg)
{
	if((((pRxMsg->CanId>>8)&0xFFFF)==0xECFF) && (pRxMsg->CanData[0] == 0x20)) //多帧的首帧
	{
		MultiFrame_t* pMultiFrame = GetMultiFrameEmptyBuf(pRxMsg);
		MultiFrameNode_t* pMultiFrameNode = GetMultiFrameNodeEmptyBuf();
		if((pMultiFrame!=NULL) && (pMultiFrameNode!=NULL))
		{
			pMultiFrame->pHead = pMultiFrameNode;
			pMultiFrame->FrameCnt = pRxMsg->CanData[3]; //不包含首帧
			pMultiFrame->FrameSoureAdd = (uint8_t)(pRxMsg->CanId&0x000000ff);
			pMultiFrame->Priority = (uint8_t)((pRxMsg->CanId>>24)&0xff);
			pMultiFrame->CanNum = pRxMsg->CanNum;
			pMultiFrame->Empty = 1;
			pMultiFrameNode->pMsg = pRxMsg;
			pMultiFrameNode->NodeNum = 0;
			pMultiFrameNode->Empty = 1;
			pMultiFrameNode->Next = NULL;
			//底层帧已处理
			SetCanRxBufDoneFlag(pRxMsg);
		}
		else
		{
			//释放底层CAN接收的BUF
			ReleaseCanRxBuf(pRxMsg);
		}
	}
	else
	{
		if(((pRxMsg->CanId>>8)&0xFFFF)==0xEBFF)  //多帧的非首帧
		{
			uint8_t i;
			for(i=0;i<CAN_MULTI_FRAME_BUF_SIZE;i++)
			{
				if((CanMultiFrame[i].Empty !=0)&&(CanMultiFrame[i].FrameSoureAdd == (uint8_t)(pRxMsg->CanId&0x000000ff))&& \
					 (CanMultiFrame[i].CanNum==pRxMsg->CanNum)&&(CanMultiFrame[i].Priority == (uint8_t)((pRxMsg->CanId>>24)&0xff)))
				{
					break;
				}
			}
			if(i<CAN_MULTI_FRAME_BUF_SIZE)
			{
				J1939MultiFrameProcess(pRxMsg,&CanMultiFrame[i]);
			}
			else
			{
				//释放底层CAN接收的BUF
				ReleaseCanRxBuf(pRxMsg);
			}
		}
		else //单帧
		{
			CanData_t* pCanData = GetCanDataEmptyBuf();
			if(pCanData!=NULL)
			{
				pCanData->CanId = pRxMsg->CanId;
				pCanData->DataLen = pRxMsg->DataLen;
				for(uint8_t i=0;i<pCanData->DataLen;i++)
				{
					pCanData->Data[i] = pRxMsg->CanData[i];
				}
				pCanData->State = (uint8_t)CAN_DATA_STATE_FULL;
				pCanData->Procotol = J1939;
				pCanData->CanNum = pRxMsg->CanNum;
				pCanData->Head = 1;
				pCanData->Next = NULL;
			}
			//释放底层CAN接收的BUF
			ReleaseCanRxBuf(pRxMsg);
		}
	}
}

static void ISO15765MultiFrameProcess(CanRxMsg_t* pRxMsg, MultiFrame_t *pMultiFrame)
{
	MultiFrameNode_t *pNodeEnd,*pNodeNext;
	pNodeEnd = GetMultiFrameNodeEnd(pMultiFrame); //得到帧尾
	pNodeNext = GetMultiFrameNodeEmptyBuf();
	if((pNodeEnd != NULL)&&(pNodeEnd->pMsg != NULL)&&(pNodeNext!=NULL))
	{
		if(((pRxMsg->CanData[0]-0x20) < pMultiFrame->FrameCnt)&&(pNodeEnd->pMsg->CanNum==pRxMsg->CanNum)&& \
			((pRxMsg->CanData[0]==(pNodeEnd->pMsg->CanData[0]+1))||((pMultiFrame->pHead==pNodeEnd)&&(pRxMsg->CanData[0]==0x21))||((pNodeEnd->pMsg->CanData[0]==0x2F)&&(pRxMsg->CanData[0]==0x20)))) //是否时多帧的下一帧
		{
			//在帧尾插入帧
			pNodeEnd->Next = pNodeNext;
			pNodeNext->NodeNum = pNodeEnd->NodeNum+1;
			pNodeNext->pMsg = pRxMsg;
			pNodeNext->Next = NULL;
			pNodeNext->Empty = 1;
			//置位底层BUF已处理标志
			SetCanRxBufDoneFlag(pRxMsg);
		}
		else
		{
			ReleaseCanRxBuf(pRxMsg);
			//ReleaseMultiFrameBuf(pMultiFrame); //可能丢帧了，收到的也全部丢弃。屏蔽 20190620 hxy
		}
		if((pNodeNext->NodeNum +1) == pMultiFrame->FrameCnt) //收到完整的多帧,FrameCnt包含首帧
		{
			CanData_t* pCanData;
			CanData_t* pCanDataNode = NULL;
			CanData_t* pNodeHead = NULL;
			pNodeNext = pMultiFrame->pHead;
      if(GetCanDataEmptyBufCnt()>=pMultiFrame->FrameCnt)
			{
				uint16_t i;
				for(i=0;i<(pMultiFrame->FrameCnt);i++)
				{
					pCanData = GetCanDataEmptyBuf();
					if((pCanData==NULL)||(pNodeNext==NULL)||(pNodeNext->pMsg==NULL))
					{
						break;
					}
					if(pCanDataNode==NULL)
					{
						pCanDataNode = pCanData;
						pNodeHead = pCanData;
						pCanData->Head = 1;
					}
					else
					{
						pCanDataNode->Next = pCanData;
						pCanData->Head = 0;
					}
					pCanData->CanId = pNodeNext->pMsg->CanId;
					pCanData->DataLen = pNodeNext->pMsg->DataLen;
					for(uint8_t j=0;j<pCanData->DataLen;j++)
					{
						pCanData->Data[j] = pNodeNext->pMsg->CanData[j];
					}
					pCanData->State = (uint8_t)CAN_DATA_STATE_FULL;
					pCanData->CanNum = pRxMsg->CanNum;
					pCanData->Procotol = ISO15765;
					pCanData->Next = NULL;
					pNodeNext = pNodeNext->Next;
					pCanDataNode = pCanData;
				}
				if(i<pMultiFrame->FrameCnt)
				{
					ReleaseCanDataBuf(pNodeHead);
				}
			}
			ReleaseMultiFrameBuf(pMultiFrame);
		}
	}
}

static void ISO15765SendFrameFlowCmd(CanRxMsg_t* pRxMsg)
{
	//发送多帧请求报文
	CanTxMessage TxMsg;
	memset(&TxMsg,0,sizeof(CanTxMessage));
	if(pRxMsg->IDE == CAN_Standard_Id)
	{
		if((pRxMsg->CanId>=0x7DF)&&(pRxMsg->CanId<=0x7EF))
		{
			TxMsg.StdId = pRxMsg->CanId & 0x000007F7;
			TxMsg.IDE = CAN_Standard_Id;
			TxMsg.DLC = 8;
			TxMsg.Data[0] = 0x30;
			TxMsg.RTR =0;
			//TxMsg.Data[2] = 0x0a;
			//CAN2SendMsg(&TxMsg);
		}
	}
	else
	{
		TxMsg.ExtId = (pRxMsg->CanId&0xFFFF0000)|((pRxMsg->CanId&0x000000FF)<<8)|((pRxMsg->CanId&0x0000FF00)>>8);
		TxMsg.IDE = CAN_Extended_Id;
		TxMsg.DLC = 8;
		TxMsg.Data[0] = 0x30;
		TxMsg.RTR =0;
		//TxMsg.Data[2] = 0x0a;
		//CAN2SendMsg(&TxMsg);
	}
	
	if(pRxMsg->CanNum==CAN_NUM1)
	{
		CAN1SendMsg(&TxMsg);
	}
	else
	{
		if(pRxMsg->CanNum==CAN_NUM2)
		{
			CAN2SendMsg(&TxMsg);
		}
	}
}

static void ISO15765ProtocolProcess(CanRxMsg_t* pRxMsg)
{
	if(pRxMsg->CanData[0] == 0x10) //多帧的首帧
	{
		MultiFrame_t* pMultiFrame = GetMultiFrameEmptyBuf(pRxMsg);
		MultiFrameNode_t* pMultiFrameNode = GetMultiFrameNodeEmptyBuf();
		if((pMultiFrame!=NULL) && (pMultiFrameNode!=NULL))
		{
			if(((pRxMsg->CanData[1]+1)%7 > 0))
			{
			  pMultiFrame->FrameCnt = (pRxMsg->CanData[1]+1)/7+1;  //FrameCnt包含首帧
			}
			else
			{
				pMultiFrame->FrameCnt = (pRxMsg->CanData[1]+1)/7;
			}
			pMultiFrame->pHead = pMultiFrameNode;
			pMultiFrame->CanNum = pRxMsg->CanNum;
			pMultiFrame->Empty = 1;
			pMultiFrameNode->pMsg = pRxMsg;
			pMultiFrameNode->NodeNum = 0;
			pMultiFrameNode->Empty = 1;
			pMultiFrameNode->Next = NULL;
			//底层帧已处理
			SetCanRxBufDoneFlag(pRxMsg);
			ISO15765SendFrameFlowCmd(pRxMsg);
		}
		else
		{
			//释放底层CAN接收的BUF
			ReleaseCanRxBuf(pRxMsg);
		}
	}
	else
	{
		if((pRxMsg->CanData[0]&0xF0) == 0x20)  //多帧的非首帧
		{
			uint8_t i;
			for(i=0;i<CAN_MULTI_FRAME_BUF_SIZE;i++)
			{
				if((CanMultiFrame[i].Empty !=0)&&(CanMultiFrame[i].pHead != NULL)&&(CanMultiFrame[i].pHead->pMsg->CanId == pRxMsg->CanId)&&(CanMultiFrame[i].CanNum==pRxMsg->CanNum))
				{
					break;
				}
			}
			if(i<CAN_MULTI_FRAME_BUF_SIZE)
			{
				ISO15765MultiFrameProcess(pRxMsg,&CanMultiFrame[i]);
			}
			else
			{
				//释放底层CAN接收的BUF
				ReleaseCanRxBuf(pRxMsg);
			}
		}
		else //单帧
		{
			CanData_t* pCanData = GetCanDataEmptyBuf();
			if(pCanData!=NULL)
			{
				pCanData->CanId = pRxMsg->CanId;
				pCanData->DataLen = pRxMsg->DataLen;
				for(uint8_t i=0;i<pCanData->DataLen;i++)
				{
					pCanData->Data[i] = pRxMsg->CanData[i];
				}
				pCanData->State = (uint8_t)CAN_DATA_STATE_FULL;
				pCanData->Procotol = ISO15765;
				pCanData->CanNum = pRxMsg->CanNum;
				pCanData->Head = 1;
				pCanData->Next = NULL;
			}
			//释放底层CAN接收的BUF
			ReleaseCanRxBuf(pRxMsg);
		}
	}
}

//static void CheckCanMsg(ProtocolNum_t Protocol)
static void CheckCanMsg(void)
{
	CanRxMsg_t* pRxMsg;
	uint8_t DoCnt = 0;
	do
	{
		pRxMsg = GetCanRxMsg();
		if(pRxMsg==NULL)
		{
			break; //没有报文
		}
    
		if((pRxMsg->IDE==CAN_Extended_Id)&&(((pRxMsg->CanId>>16)&0xFFFF) != 0x18DA)&&(((pRxMsg->CanId>>16)&0xFFFF) != 0x18DB))
		{
			J1939ProtocolProcess(pRxMsg);		
		}
		else if( (pRxMsg->IDE==CAN_Standard_Id)&&(pRxMsg->CanId==0x110 )	)
		{
			pRxMsg->IDE=CAN_Extended_Id;
			pRxMsg->CanId=0x18999805;
			J1939ProtocolProcess(pRxMsg);		
		}
		else
		{
			ISO15765ProtocolProcess(pRxMsg);
		}
	  DoCnt += 1;
  }while(DoCnt < 30); //最多处理30包数据
}

////static void CheckCanDataUsed(ProtocolNum_t Protocol)
//static void CheckCanDataUsed(void)
//{
////	for(uint16_t i=0;i<CAN_DATA_BUF_SIZE;i++)
////	{
////		if((CanData[i].State & (uint8_t)CAN_DATA_STATE_FULL)&& (CanData[i].Head!=0)&& \
////			(CanData[i].State & (uint8_t)CAN_DATA_STATE_OBD_READ)&& \
////		  ((Protocol==J1939_29_250)||(CanData[i].State & (uint8_t)CAN_DATA_STATE_UDS_READ)))
////		{
////			ReleaseCanDataBuf(&CanData[i]);
////		}
////	}
//	
//	for(uint16_t i=0;i<CAN_DATA_BUF_SIZE;i++)
//	{
//		if((CanData[i].State & (uint8_t)CAN_DATA_STATE_FULL)&& (CanData[i].Head!=0)&& \
//			(CanData[i].State & (uint8_t)CAN_DATA_STATE_OBD_READ))
//		{
//			ReleaseCanDataBuf(&CanData[i]);
//		}
//	}
//}

uint16_t J1939MultiFrameToData(CanData_t *pCanMsg,uint8_t FrameCnt, uint8_t *pData, uint16_t DataLen)
{
	CanData_t *pFrame;
	uint8_t GetSize;
	uint16_t Len = 0;
	uint16_t ValidLen = (uint16_t)pCanMsg->Data[1] + (uint16_t)(pCanMsg->Data[2]<<8);
	if((ValidLen<=DataLen)&&(pCanMsg!=NULL)&&(FrameCnt>1))
	{
		for(uint8_t i=1;i<FrameCnt;i++)
		{
			pFrame = GetCanSpecifyFrame(pCanMsg, i);
			if(pFrame==NULL)
			{
				break;
			}
			if((ValidLen - Len) >= 7) //每一帧最多7个字节数据
			{
				GetSize = 7;
			}
			else
			{
				GetSize = ValidLen - Len;
			}
			for(uint8_t j=0;j<GetSize;j++)
			{
				pData[Len] = pFrame->Data[1+j];
				Len += 1;
			}
		}
  }
	return Len;
}

uint8_t J1939SingleFrameToData(CanData_t *pCanMsg, uint8_t FrameCnt, uint8_t *pData, uint16_t DataLen)
{
	uint8_t Len = 0;
	if((pCanMsg->DataLen<=DataLen)&&(pCanMsg!=NULL))
	{
		for(uint8_t i=0;i<pCanMsg->DataLen;i++)
		{
			pData[i] = pCanMsg->Data[i];
			Len += 1;
		}
	}
	return Len;
}

uint8_t ISO15765MultiFrameToData(CanData_t *pCanMsg, uint8_t FrameCnt, uint8_t *pData)
{
	CanData_t *pFrame;
	uint8_t GetSize;
	uint8_t CountFrameCnt = 0;
	uint8_t DataLen = 0;
	if((pCanMsg->Data[0]==0x10)&&(pCanMsg->Data[1]>6)&&(FrameCnt>1)) //是否是多帧
	{
		if((pCanMsg->Data[1]+1)%7 > 0)
		{
			CountFrameCnt = (pCanMsg->Data[1]+1)/7+1;
		}
		else
		{
			CountFrameCnt = (pCanMsg->Data[1]+1)/7;
		}
		if(CountFrameCnt==FrameCnt)
		{
			pData[DataLen] = pCanMsg->Data[2];
			DataLen += 1;
			pData[DataLen] = pCanMsg->Data[3];
			DataLen += 1;
			pData[DataLen] = pCanMsg->Data[4];
			DataLen += 1;
			pData[DataLen] = pCanMsg->Data[5];
			DataLen += 1;
			pData[DataLen] = pCanMsg->Data[6];
			DataLen += 1;
			pData[DataLen] = pCanMsg->Data[7];
			DataLen += 1;
			
			for(uint8_t i=1;i<FrameCnt;i++)
			{
				pFrame = GetCanSpecifyFrame(pCanMsg, i);
				if(pFrame==NULL)
				{
					break;
				}
				if((pCanMsg->Data[1] - DataLen) >= 7) //每一帧最多7个字节数据
				{
					GetSize = 7;
				}
				else
				{
					GetSize = pCanMsg->Data[1] - DataLen;
				}
				for(uint8_t j=0;j<GetSize;j++)
				{
					pData[DataLen] = pFrame->Data[1+j];
					DataLen += 1;
				}
			}
	  }
	}
	return DataLen;
}

uint8_t ISO15765SingleFrameToData(CanData_t *pCanMsg, uint8_t FrameCnt, uint8_t *pData)
{
	uint8_t DataLen = 0;
	if(pCanMsg->Data[0]<8)
	{
		for(uint8_t i=0;i<pCanMsg->Data[0];i++)
		{
			pData[i] = pCanMsg->Data[1+i];
			DataLen += 1;
		}
  }
	return DataLen;
}	


//void ReleaseCanDataBufNew(CanData_t *pHead, uint8_t FrameCnt)
//{
//	CanData_t *pNodeNext;
//  CanData_t *pNodeTemp = pHead;
//	uint8_t DoCnt = FrameCnt;
//	do
//	{
//		if((pNodeTemp==NULL)||(DoCnt==0))
//		{
//		 break;
//		}
//		pNodeTemp->State = (uint8_t)CAN_DATA_STATE_EMPTY;
//		pNodeTemp->Head = 0;
//		pNodeTemp->DataLen = 0;
//		pNodeTemp->CanId = 0;
//		pNodeTemp->Procotol = DEFAULT;
//		pNodeTemp->CanNum = CAN_DEFAULT;
//		pNodeNext = pNodeTemp->Next;
//		pNodeTemp->Next = NULL;
//		pNodeTemp = pNodeNext;
//		DoCnt -= 1;
//	}while(1);
//}

//void CanDataProcess(ProtocolNum_t Protocol)
void CanDataProcess(void)
{
	//CheckCanDataUsed();
	CheckCanMsg();
}

//CanData_t* GetCanData(Reader_t Reader, uint8_t *pFrameCnt)
//{
//	uint16_t *pReadPos;
//	CanData_t* pCanData = NULL;
//	uint16_t DoCnt = 0;
//	uint8_t state = (uint8_t)CAN_DATA_STATE_FULL;
//	
//	if(Reader==OBD)
//	{
//		pReadPos = &ObdCanDataReadPos;
//		state |= (uint8_t)CAN_DATA_STATE_OBD_READ;
//	}
//	else
//	{
//		if(Reader==UDS)
//		{
//			pReadPos = &UdsCanDataReadPos;
//			state |= (uint8_t)CAN_DATA_STATE_UDS_READ;
//		}
//		else
//		{
//			return pCanData;
//		}
//	}
//	
//	do
//	{
//		if((*pReadPos)>=CAN_DATA_BUF_SIZE)
//		{
//			*pReadPos = 0;
//		}
//		if(((CanData[*pReadPos].State & state) == (uint8_t)CAN_DATA_STATE_FULL)&&(CanData[*pReadPos].Head!=0))
//		{
//			pCanData = &CanData[*pReadPos];
//			CanData[*pReadPos].State |= state;
//		}
//		(*pReadPos) += 1;
//		if(pCanData!=NULL)
//		{
//			if(pFrameCnt!=NULL)
//			{
//				*pFrameCnt = GetCanDataFrameCnt(pCanData);
//			}
//			break;
//		}
//		DoCnt += 1;
//	}while(DoCnt<CAN_DATA_BUF_SIZE);
//	return pCanData;
//}

CanData_t* GetCanData(uint8_t *pFrameCnt)
{
	CanData_t* pCanData = NULL;
	uint16_t DoCnt = 0;

	do
	{
		if(CanDataReadPos>=CAN_DATA_BUF_SIZE)
		{
			CanDataReadPos = 0;
		}
		if((CanData[CanDataReadPos].State & ((uint8_t)CAN_DATA_STATE_FULL))&&(CanData[CanDataReadPos].Head!=0))
		{
			pCanData = &CanData[CanDataReadPos];
		}
		CanDataReadPos += 1;
		if(pCanData!=NULL)
		{
			if(pFrameCnt!=NULL)
			{
				*pFrameCnt = GetCanDataFrameCnt(pCanData);
			}
			break;
		}
		DoCnt += 1;
	}while(DoCnt<CAN_DATA_BUF_SIZE);
	return pCanData;
}

CanData_t* GetCanSpecifyFrame(CanData_t* pHead, uint8_t FrameNum)
{
	CanData_t *pCanData = NULL;
	CanData_t *pNodeTemp = pHead;
	uint8_t FrameCnt = 0;
	do
	{
    if(pNodeTemp==NULL)
		{
			break;
		}
		if(FrameCnt==FrameNum)
		{
			pCanData = pNodeTemp;
			break;
		}
		FrameCnt += 1;
		pNodeTemp = pNodeTemp->Next;
	}while(1);
	return pCanData;
}

uint8_t SendCanMsg(CanTxMessage *pTxMsg)
{
	uint8_t res=1;
	if(pTxMsg!=NULL)
	{
		res = CAN2SendMsg(pTxMsg);
	}
	return res;
}




