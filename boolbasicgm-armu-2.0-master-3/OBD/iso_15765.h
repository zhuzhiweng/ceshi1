#ifndef __ISO_15765_H
#define __ISO_15765_H	 
#include "sys.h"
#include "obd.h"
#include "CanDataAnalysis.h"

typedef struct
{
	uint8_t Req_inf[8];
	uint8_t Cntflag;
	uint8_t SendCnt;
}ISO15765PID_t;


//ReMessage_stateCanTxMsg_t do_15765_Multi_frame(  u8 *p , u8 targer_total_byte);
//void iso_15765_vehicle_infor(void);
//void iso_15765_data_flow(void);

uint8_t iso_15765_canAdd_Blacklist_judge(uint32_t canid);
//void ISO15765Process(ProtocolNum_t Procotol);
//void ISO15765Process(ProtocolNum_t Procotol, uint8_t SendEnFlag);
void ISO15765Init(void);

void ISO15765CheckPid(CanData_t *pCanMsg, uint8_t FrameCnt);
void ISO15765SendMsg(uint8_t Procotol, uint8_t SendEnFlag);
void ISO15765DisPidSend(uint16_t ISO15765Pid);
void ISO15765EnPidSend(uint16_t ISO15765Pid);

void Dis15765_09Sever(void);
#endif



















