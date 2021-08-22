#ifndef __J_1939_H
#define __J_1939_H	 
#include "sys.h"
#include "obd.h"
#include "CanDataAnalysis.h"


typedef struct
{
	uint16_t Pgn;
	uint8_t Sendflag;
	uint8_t SendCnt;
	uint8_t Can_Add;			//保存解析PGN时的canID
	uint8_t H_L_Can_Add_priority;		//1时canid 大的优先级高 ，0时canid小的优先级高
}J1939Pgn_t;

typedef struct 
{
	uint8_t state;
//	uint8_t Can_No;
	CanNum_t CanNum;
}	
JingLi_Sensor;	

typedef struct 
{
	uint8_t state;
//	uint8_t Can_No;
	CanNum_t CanNum;
}	
XinLi_Sensor;	

typedef struct 
{
	uint8_t state;
//	uint8_t Can_No;
	CanNum_t CanNum;
}	
TianDiCheRen_Sensor;

//ReMessage_stateCanTxMsg_t do_1939_Multi_frame(  u8 *p , u8 targer_total_byte);
//void j_1939_vehicle_infor(void);
//void j_1939_data_flow(void)	;


//void J1939Process(void);

//void J1939Process(uint8_t SendEnFlag);

void J1939Init(void);
void J1939CheckPgn(CanData_t *pCanMsg,uint8_t FrameCnt);
void J1939SendMsg(uint8_t SendEnFlag);
void F101_Sensor_Send(uint32_t SendEnFlag);
void Xinli_0000_Sensor_Send(uint32_t SendEnFlag);
void TianDiCheRen_Sensor_Send(uint32_t SendEnFlag);
uint8_t Get_Sensor_flag(void);

#endif





