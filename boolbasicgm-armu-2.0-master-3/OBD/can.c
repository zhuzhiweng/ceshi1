#include "CAN.h"
#include "TerminalTest.h"


static CanRxMsg_t CanRxMsgBuf[CAN_RX_BUF_SIZE]={0}; 
static uint16_t CanRxBufRdPos = 0;
static uint16_t CanRxBufWrPos = 0;

//static CanRxMsg_t Can1RxMsgBuf[CAN1_RX_BUF_SIZE]; 
//static uint16_t Can1RxBufRdPos = 0;
//static uint16_t Can1RxBufWrPos = 0;

/************************************************************************
  * @描述:   CAN所用IO引脚配置
  * @参数:   None
  * @返回值: None
  **********************************************************************/
static void CAN_GPIOConfig(void)
{
	GPIO_InitType	GPIO_InitStructure;
	
	RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOB|RCC_APB2_PERIPH_GPIOC|RCC_APB2_PERIPH_AFIO, ENABLE);
	RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_CAN2, ENABLE);
	
	GPIO_InitStructure.Pin        = GPIO_PIN_13;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitPeripheral(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.Pin        = GPIO_PIN_12;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;
	GPIO_InitPeripheral(GPIOB, &GPIO_InitStructure);	
	
	GPIO_InitStructure.Pin        = GPIO_PIN_9;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitPeripheral(GPIOC, &GPIO_InitStructure);	
	
	RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOB, ENABLE);
	RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOA, ENABLE);	
	RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOC, ENABLE);	
	
	GPIO_ResetBits(GPIOC, GPIO_PIN_9);
	
	RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_CAN1, ENABLE);
	
	GPIO_InitStructure.Pin        = GPIO_PIN_12;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitPeripheral(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.Pin        = GPIO_PIN_11;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;
	GPIO_InitPeripheral(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.Pin        = GPIO_PIN_8;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitPeripheral(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.Pin        = GPIO_PIN_15;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitPeripheral(GPIOB, &GPIO_InitStructure);
	
	GPIO_ResetBits(GPIOB, GPIO_PIN_15);
	GPIO_ResetBits(GPIOA, GPIO_PIN_8);
}

/************************************************************************
  * @描述:  CAN速率配置
  * @参数:  velocity 速率分频值
  * @返回值: None
  **********************************************************************/
void CAN_Config(u16 velocity,CAN_Module* CANx)
{
	u8 br;
	
	CAN_InitType CAN_InitStructure;
	br=2000/velocity;

    CAN_InitStructure.TTCM          = DISABLE;
    CAN_InitStructure.ABOM          = DISABLE;
    CAN_InitStructure.AWKUM         = DISABLE;
    CAN_InitStructure.NART          = DISABLE;
    CAN_InitStructure.RFLM          = DISABLE;
    CAN_InitStructure.TXFP          = ENABLE;
    CAN_InitStructure.OperatingMode = CAN_Normal_Mode;
    CAN_InitStructure.RSJW          = CAN_RSJW_1tq;
    CAN_InitStructure.TBS1          = CAN_TBS1_14tq;
    CAN_InitStructure.TBS2          = CAN_TBS2_3tq;
	CAN_InitStructure.BaudRatePrescaler =  br;

	CAN_Init(CANx, &CAN_InitStructure);
}
///************************************************************************
//  * @描述:  CAN扩展帧滤波器设置
//  * @参数:  id1,id2 效验码   mid1,mid2 屏蔽码
//  * @返回值: None
//  **********************************************************************/

//void CAN2_32bitfilter_Init( u32 accepId, u32 maskId,u8 mask)
//{
//	
//	CAN_FilterInitTypeDef  CAN_FilterInitStructure;
//	
//	CAN_FilterInitStructure.CAN_FilterNumber=14;	  //过滤器0
//  CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask; 
//  CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit; //32位 

//	if(mask == 1){																																									//扩展帧 掩码部分 当mask = 1;
//			CAN_FilterInitStructure.CAN_FilterIdHigh=((accepId <<3)&0xFFFF0000)>>16;//32位ID     	
//			CAN_FilterInitStructure.CAN_FilterIdLow=((accepId<<3)|CAN_ID_EXT|CAN_RTR_DATA)&0xFFFF ;
//			CAN_FilterInitStructure.CAN_FilterMaskIdHigh=((maskId<<3)&0xFFFF0000)>>16;//32位MASK
//			CAN_FilterInitStructure.CAN_FilterMaskIdLow=(maskId<<3)&0xFFFF;	
//	}else{																																															//标准帧 掩码部分 当mask = 0; 
//			CAN_FilterInitStructure.CAN_FilterIdHigh=((accepId <<21)&0xFFFF0000)>>16;//32位ID
//			CAN_FilterInitStructure.CAN_FilterIdLow=((accepId<<21)|CAN_ID_STD|CAN_RTR_DATA)&0xFFFF;
//			CAN_FilterInitStructure.CAN_FilterMaskIdHigh=((maskId<<21)&0xFFFF0000)>>16;//32位MASK
//  	  CAN_FilterInitStructure.CAN_FilterMaskIdLow=(maskId<<21)&0xFFFF;		
//	}
//  
//	CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_Filter_FIFO0;//过滤器0关联到FIFO0
//	CAN_FilterInitStructure.CAN_FilterActivation=ENABLE; //激活过滤器0
//	CAN_FilterInit(&CAN_FilterInitStructure);//滤波器初始化
//	
//	CAN_ITConfig(CAN2,CAN_IT_FMP0,ENABLE);//FIFO0消息挂号中断允许.
//}

//void CAN1_32bitfilter_Init( u32 accepId, u32 maskId,u8 mask)
//{
//	
//	CAN_FilterInitTypeDef  CAN_FilterInitStructure;
//	
//	CAN_FilterInitStructure.CAN_FilterNumber=0;	  //过滤器0
//  CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask; 
//  CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit; //32位 

//	if(mask == 1){																																									//扩展帧 掩码部分 当mask = 1;
//			CAN_FilterInitStructure.CAN_FilterIdHigh=((accepId <<3)&0xFFFF0000)>>16;//32位ID     	
//			CAN_FilterInitStructure.CAN_FilterIdLow=((accepId<<3)|CAN_ID_EXT|CAN_RTR_DATA)&0xFFFF ;
//			CAN_FilterInitStructure.CAN_FilterMaskIdHigh=((maskId<<3)&0xFFFF0000)>>16;//32位MASK
//			CAN_FilterInitStructure.CAN_FilterMaskIdLow=(maskId<<3)&0xFFFF;	
//	}else{																																															//标准帧 掩码部分 当mask = 0; 
//			CAN_FilterInitStructure.CAN_FilterIdHigh=((accepId <<21)&0xFFFF0000)>>16;//32位ID
//			CAN_FilterInitStructure.CAN_FilterIdLow=((accepId<<21)|CAN_ID_STD|CAN_RTR_DATA)&0xFFFF;
//			CAN_FilterInitStructure.CAN_FilterMaskIdHigh=((maskId<<21)&0xFFFF0000)>>16;//32位MASK
//  	  CAN_FilterInitStructure.CAN_FilterMaskIdLow=(maskId<<21)&0xFFFF;		
//	}
//  
//	CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_Filter_FIFO0;//过滤器0关联到FIFO0
//	CAN_FilterInitStructure.CAN_FilterActivation=ENABLE; //激活过滤器0
//	CAN_FilterInit(&CAN_FilterInitStructure);//滤波器初始化
//	
//	CAN_ITConfig(CAN1,CAN_IT_FMP0,ENABLE);//FIFO0消息挂号中断允许.
//}

//void CAN2_Mode_Init(u16 brp, u32 accepId, u32 maskId,u8 mask)
//{
//	//CAN_GPIOConfig();
//	CAN_Config(brp,CAN2);
//	CAN2_32bitfilter_Init(accepId,maskId,mask);

//}
//void CAN1_Mode_Init(u16 brp, u32 accepId, u32 maskId,u8 mask)
//{
////	CAN_GPIOConfig();
//	CAN_Config(brp,CAN1);
//	CAN1_32bitfilter_Init(accepId,maskId,mask);

//}

static void Can1ModeInit(void)
{
	CAN_DeInit(CAN1);

	CAN_FilterInitType CAN_FilterInitStructure;
	
	CAN_FilterInitStructure.Filter_Num            = 0;
    CAN_FilterInitStructure.Filter_Mode           = CAN_Filter_IdMaskMode;
    CAN_FilterInitStructure.Filter_Scale          = CAN_Filter_32bitScale;
    CAN_FilterInitStructure.Filter_HighId         = 0;
    CAN_FilterInitStructure.Filter_LowId          = 0;
    CAN_FilterInitStructure.FilterMask_HighId     = 0;
    CAN_FilterInitStructure.FilterMask_LowId      = 0;
    CAN_FilterInitStructure.Filter_FIFOAssignment = CAN_FIFO0;
    CAN_FilterInitStructure.Filter_Act            = ENABLE;
    CAN1_InitFilter(&CAN_FilterInitStructure);

	CAN_INTConfig(CAN1, CAN_INT_FMP0, ENABLE);
}

static void Can2ModeInit(void)
{
	CAN_DeInit(CAN2);
	
	CAN_FilterInitType CAN_FilterInitStructure;
		
	CAN_FilterInitStructure.Filter_Num            = 0;
    CAN_FilterInitStructure.Filter_Mode           = CAN_Filter_IdMaskMode;
    CAN_FilterInitStructure.Filter_Scale          = CAN_Filter_32bitScale;
    CAN_FilterInitStructure.Filter_HighId         = 0;
    CAN_FilterInitStructure.Filter_LowId          = 0;
    CAN_FilterInitStructure.FilterMask_HighId     = 0;
    CAN_FilterInitStructure.FilterMask_LowId      = 0;
    CAN_FilterInitStructure.Filter_FIFOAssignment = CAN_FIFO0;
    CAN_FilterInitStructure.Filter_Act            = ENABLE;
    CAN2_InitFilter(&CAN_FilterInitStructure);

	CAN_INTConfig(CAN2, CAN_INT_FMP0, ENABLE);		
}

void CanSetBaud(uint16_t baud, CAN_Module* CANx)
{
	uint8_t br;
	CAN_InitType CAN_InitStructure;		
	br = 2000/baud;
	CAN_InitStructure.TTCM          = DISABLE;
    CAN_InitStructure.ABOM          = DISABLE;
    CAN_InitStructure.AWKUM         = DISABLE;
    CAN_InitStructure.NART          = DISABLE;
    CAN_InitStructure.RFLM          = DISABLE;
    CAN_InitStructure.TXFP          = ENABLE;
    CAN_InitStructure.OperatingMode = CAN_Normal_Mode;
    CAN_InitStructure.RSJW          = CAN_RSJW_1tq;
    CAN_InitStructure.TBS1          = CAN_TBS1_14tq;
    CAN_InitStructure.TBS2          = CAN_TBS2_3tq;
	CAN_InitStructure.BaudRatePrescaler =  br;
	
	CAN_Init(CANx, &CAN_InitStructure);	
}

void CanInit(void)
{
	CAN_GPIOConfig();
	Can1ModeInit();
	Can2ModeInit();
}

static CanRxMsg_t* getEmptyBuf(void)
{
	CanRxMsg_t* EmptyBuf = NULL;

	uint16_t checkCnt = 0;
	do
	{
		CanRxBufWrPos += 1;
		if(CanRxBufWrPos>=CAN_RX_BUF_SIZE)
		{
			CanRxBufWrPos = 0;
		}
		if(CanRxMsgBuf[CanRxBufWrPos].Empty == 0)
		{
			EmptyBuf = &CanRxMsgBuf[CanRxBufWrPos];
			break;
		}
		checkCnt += 1;
	}while(checkCnt<CAN_RX_BUF_SIZE);
	return EmptyBuf;
}

void CAN2_RX0_IRQHandler(void)
{
	if(CAN_PendingMessage(CAN2, CAN_FIFO0)!=0)
	{
		CanRxMessage RxData;
		CAN_ReceiveMessage(CAN2, CAN_FIFO0, &RxData);
		CanRxMsg_t* pBuf = getEmptyBuf();
		if(pBuf!=NULL)
		{
			pBuf->DataLen = RxData.DLC;
			if(RxData.IDE == CAN_Extended_Id)
			{
				pBuf->CanId = RxData.ExtId;
				pBuf->IDE = CAN_Extended_Id;
			}
			else
			{
				pBuf->CanId = RxData.StdId;
				pBuf->IDE = CAN_Standard_Id;
			}
			for(uint8_t i=0;i<pBuf->DataLen;i++)
			{
				pBuf->CanData[i] = RxData.Data[i];
			}
			pBuf->CanNum = CAN_NUM2;
			pBuf->DoneIt = 0;
			pBuf->Empty = 1;
		}
	}
}
void USB_LP_CAN1_RX0_IRQHandler(void)
{
  CanRxMessage RxData;
  if(CAN_PendingMessage(CAN1, CAN_FIFO0)!=0)
  {
    CAN_ReceiveMessage(CAN1, CAN_FIFO0, &RxData);
		
    if(getTermianlState()==1)
    {     
		CAN1test(&RxData);
    }
		
		CanRxMsg_t* pBuf = getEmptyBuf();
		if(pBuf!=NULL)
		{
			pBuf->DataLen = RxData.DLC;
			if(RxData.IDE == CAN_Extended_Id)
			{
				pBuf->CanId = RxData.ExtId;
				pBuf->IDE = CAN_Extended_Id;
			}
			else
			{
				pBuf->CanId = RxData.StdId;
				pBuf->IDE = CAN_Standard_Id;
			}
			for(uint8_t i=0;i<pBuf->DataLen;i++)
			{
				pBuf->CanData[i] = RxData.Data[i];
			}
			pBuf->CanNum = CAN_NUM1;
			pBuf->DoneIt = 0;
			pBuf->Empty = 1;
		}
  }
}


uint8_t CAN1SendMsg(CanTxMessage *TxMessage)
{
	u8 mbox;
	u16 i=0;
	mbox = CAN_TransmitMessage(CAN1, TxMessage);

	while((CAN_TransmitSTS(CAN1, mbox)!=CANTXSTSOK)&&(i<0XFFF))
	{
	  i++;	//等待发送结束
	}
	if(i>=0XFFF)
	{
		return 1;  //发送失败
	}
	return 0; //发送成功		
}

uint8_t CAN2SendMsg(CanTxMessage *TxMessage)
{
	u8 mbox;
  u16 i=0;
	mbox = CAN_TransmitMessage(CAN2, TxMessage);

	while((CAN_TransmitSTS(CAN2, mbox)!=CANTXSTSOK)&&(i<0XFFF))
	{
	  i++;	//等待发送结束
	}
	if(i>=0XFFF)
	{
		CAN_CancelTransmitMessage(CAN2, mbox);
		return 1;  //发送失败
	}
	return 0; //发送成功		
}

uint8_t CANSendMsg(CAN_Module* CANx,CanTxMessage *TxMessage)
{
	u8 mbox;
  u16 i=0;
	mbox = CAN_TransmitMessage(CANx, TxMessage);

	while((CAN_TransmitSTS(CANx, mbox)!=CANTXSTSOK)&&(i<0XFFF))
	{
	  i++;	//等待发送结束
	}
	if(i>=0XFFF)
	{
		return 1;  //发送失败
	}
	return 0; //发送成功		
}

CanRxMsg_t* GetCanRxMsg(void)
{
	CanRxMsg_t* pRxMsg = NULL;
	uint16_t MsgCnt = 0;
	do
	{
		CanRxBufRdPos += 1;
		if(CanRxBufRdPos>=CAN_RX_BUF_SIZE)
		{
			CanRxBufRdPos = 0;
		}
		if((CanRxMsgBuf[CanRxBufRdPos].Empty!=0)&&(CanRxMsgBuf[CanRxBufRdPos].DoneIt==0))
		{
			pRxMsg = &CanRxMsgBuf[CanRxBufRdPos];
			break;
		}
		MsgCnt += 1;
	}while(MsgCnt < CAN_RX_BUF_SIZE);
	return pRxMsg;
}

void ReleaseCanRxBuf(CanRxMsg_t *pRxBuf)
{
	pRxBuf->Empty = 0;
	pRxBuf->DoneIt = 0;
	pRxBuf->CanNum = CAN_DEFAULT;
	pRxBuf->CanId = 0;
}

void SetCanRxBufDoneFlag(CanRxMsg_t *pRxBuf)
{
	pRxBuf->DoneIt = 1;
}

CanRxMsg_t* GetCan2SpecifyMsg(uint16_t MsgPos)
{
	CanRxMsg_t* pRxMsg = NULL;
	if(MsgPos < CAN_RX_BUF_SIZE)
	{
		pRxMsg = &CanRxMsgBuf[MsgPos];
	}
	return pRxMsg;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////









