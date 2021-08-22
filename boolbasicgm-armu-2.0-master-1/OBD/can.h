#ifndef __CAN_H
#define __CAN_H
#include "sys.h" 
#include "stdio.h"
#include <string.h>
#include "n32g45x_gpio.h"


//#define CAN2_RX_BUF_SIZE 150
//#define CAN1_RX_BUF_SIZE 150

#define CAN_RX_BUF_SIZE 300


typedef enum
{
	CAN_DEFAULT = (uint8_t)0,
  CAN_NUM1 = (uint8_t)1,
	CAN_NUM2 = (uint8_t)2
}CanNum_t;


typedef struct
{
	uint8_t Empty:4;
	uint8_t DoneIt:4;
	CanNum_t CanNum:4;
	uint8_t DataLen;
	uint8_t IDE;
	uint32_t CanId;
	uint8_t CanData[8];
}CanRxMsg_t;

//void CAN2Init(uint16_t baud, uint32_t accepId, uint32_t maskId, uint8_t mask);
void CAN1_32bitfilter_Init( u32 accepId, u32 maskId,u8 mask);
CanRxMsg_t* GetCanRxMsg(void);
CanRxMsg_t* GetCan2SpecifyMsg(uint16_t MsgPos);
uint8_t CAN2SendMsg(CanTxMessage *TxMessage);
uint8_t CAN1SendMsg(CanTxMessage *TxMessage);
uint8_t CANSendMsg(CAN_Module* CANx,CanTxMessage *TxMessage);
void ReleaseCanRxBuf(CanRxMsg_t *pRxBuf);
void SetCanRxBufDoneFlag(CanRxMsg_t *pRxBuf);
//u8 CAN1SEND(void);

//void CAN2_Mode_Init(u16 brp, u32 accepId, u32 maskId,u8 mask);
//void CAN1_Mode_Init(u16 brp, u32 accepId, u32 maskId,u8 mask);
//u8 CAN2_Send_Msg(CanTxMsg TxMessage);
void CanSetBaud(uint16_t baud, CAN_Module* CANx);
void CanInit(void);
#endif

















