#ifndef __CAN_DATA_ANALYSIS_H
#define __CAN_DATA_ANALYSIS_H
#include "sys.h" 
#include "stdio.h"
#include <string.h>
#include "CAN.h"
#include "obd.h"
#define CAN_DATA_BUF_SIZE 300
#define CAN_MULTI_FRAME_BUF_SIZE 10
#define CAN_NODE_BUF_SIZE 100

#define CAN_DATA_STATE_EMPTY    (0x00)
#define CAN_DATA_STATE_FULL     (0x01)
#define CAN_DATA_STATE_OBD_READ (0x02)
#define CAN_DATA_STATE_UDS_READ (0x04)

//typedef struct CanNode
//{
//	uint16_t MsgPos;
//	uint8_t Empty;
//	struct CanNode* Next;
//}CanNode_t;

typedef enum
{
  OBD,
	UDS
}Reader_t;

typedef enum
{
	DEFAULT = (uint8_t)0,
  J1939 = (uint8_t)1,
	ISO15765 = (uint8_t)2
}Procotol_t;

//typedef enum
//{
//	CAN_DEFAULT = (uint8_t)0,
//  CAN_NUM1 = (uint8_t)1,
//	CAN_NUM2 = (uint8_t)2
//}CanNum_t;

//typedef enum
//{
//	Empty = 0x00,
//  Full = 0x01
//	ObdRd = 0x02,
//	UdsRd = 0x04
//}CanDataState_t;

typedef struct CanDataNode
{
	uint8_t Head;
	uint8_t State;
	Procotol_t Procotol:4; //1:J1939 2:15765
	CanNum_t CanNum:4;
	uint8_t DataLen;
	uint8_t Data[8];
	uint32_t CanId;
	struct CanDataNode* Next;
}CanData_t;

typedef struct CanMultiFrameNode
{
	uint8_t Empty;
	uint8_t NodeNum;
	CanRxMsg_t* pMsg;
	struct CanMultiFrameNode* Next;
}MultiFrameNode_t;

typedef struct
{
	uint8_t Empty:4;
	CanNum_t CanNum:4;
	uint8_t Priority;
	uint8_t FrameSoureAdd;
	uint8_t FrameCnt;
	MultiFrameNode_t* pHead;
}MultiFrame_t;

//void ReleaseCanDataBuf(CanData_t *pHead);
uint8_t SendCanMsg(CanTxMessage *pTxMsg);
//CanData_t* GetCanData(Reader_t Reader, uint8_t *pFrameCnt);
CanData_t* GetCanData(uint8_t *pFrameCnt);
CanData_t* GetCanSpecifyFrame(CanData_t* pHead, uint8_t FrameNum);
void CanDataProcess(void);
void ReleaseCanDataBuf(CanData_t *pHead);
//void CanDataProcess(ProtocolNum_t Protocol);
uint8_t ISO15765MultiFrameToData(CanData_t *pCanMsg, uint8_t FrameCnt, uint8_t *pData);
uint8_t ISO15765SingleFrameToData(CanData_t *pCanMsg, uint8_t FrameCnt, uint8_t *pData);
uint16_t J1939MultiFrameToData(CanData_t *pCanMsg,uint8_t FrameCnt, uint8_t *pData, uint16_t DataLen);
uint8_t J1939SingleFrameToData(CanData_t *pCanMsg, uint8_t FrameCnt, uint8_t *pData, uint16_t DataLen);
#endif


