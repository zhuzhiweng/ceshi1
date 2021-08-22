#ifndef __UDS_H
#define __UDS_H	 
#include "sys.h"
#include "stdio.h"
#include <string.h>
#include "obd.h"
#include "CanDataAnalysis.h"


//#define THREE_STAGE_MATCH

#define SEND_CMD_MAX_CNT   5
#define UDS_REPEAT_MATCH_CNT 3
#define OBD_SPEED_FAIL_CNT 5

#define MATCH_SPEED_S1  600
#define MATCH_SPEED_S2  800
#define MATCH_SPEED_S3  1000
#define MATCH_SPEED_S4  1200

#define MATCH_SPEED_CNT 7

#define UDS_ENABLE_DEALY 10
#define UDS_MATCH_DEALY 10

//	void(*ScrFrontTempPro)(uint8_t* pData, uint8_t Len);
//	void(*ScrBackTempPro)(uint8_t* pData, uint8_t Len);
//	void(*NoxContestPro)(uint8_t* pData, uint8_t Len);
//	void(*UreaLiquidLevelPro)(uint8_t* pData, uint8_t Len);


typedef enum
{
	ZhongQi_0        = (uint8_t)0,
	DongFengACM_1    = (uint8_t)1,		
	DongFeng_2      = (uint8_t)2,
	KaiRuiWeiKeSi_3 = (uint8_t)3,
	TianNaKe_4       = (uint8_t)4,
	ZD30_5           = (uint8_t)5,
	YiLiLanJie_6     = (uint8_t)6,
	YiKeKaTei_7      = (uint8_t)7,	
	KaiDeSi_8       = (uint8_t)8,
	DaTong_9     = (uint8_t)9,	
	JieFang_10        = (uint8_t)10,	
	BoShiZhongQi_11   = (uint8_t)11,	
	BoShiXiChai_12    = (uint8_t)12,	
	BoShiXiChai_13    = (uint8_t)13,	
	KaiLong_14       = (uint8_t)14,	
	TianNa_15        = (uint8_t)15,		
	HengHe_16        = (uint8_t)16,
	wushiling_17     = (uint8_t)17,
	benchiact_18     = (uint8_t)18,
JiangLing_19				= (uint8_t)19,
}UdsProtocol_t;

typedef enum
{
	UDS_HEART = 0,
	UDS_LINK = 1,
	UDS_SECURITY_ACCESS_SEED = 2,
	UDS_SECURITY_ACCESS_RES = 3,
	UDS_SPEED = 4,
	UDS_SCR_FRONT_TEMP = 5,
	UDS_SCR_BACK_TEMP = 6,
	UDS_NOX_CONTEST = 7,
	UDS_UREA_LIQUID_LEVEL = 8,
	UDS_COOLTEMP = 9  //郑州 大通客车
}UdsReqCmd_t;

typedef enum
{
	NO_RESPONSE,
	POSITIVE_RESPONSE,
	NEGATIVE_RESPONSE
}UdsResponseState_t;

typedef enum
{
	STEP_INIT,
	STEP_HEART_REQUEST,
	STEP_HEART_RESPONSE,
	STEP_LINK_REQUEST,
	STEP_LINK_RESPONSE,
	STEP_SECURITY_ACCESS_SEED_REQUEST,
	STEP_SECURITY_ACCESS_SEED_RESPONSE,
	STEP_SECURITY_ACCESS_RES_REQUEST,
	STEP_SECURITY_ACCESS_RES_RESPONSE,
	STEP_UDS_SPEED_REQUEST,
	STEP_UDS_SPEED_RESPONSE,
	STEP_OBD_SPEED_REQUEST,
	STEP_OBD_SPEED_RESPONSE,
	STEP_UDS_AFTERPROCESS_REQUEST,
	STEP_UDS_AFTERPROCESS_RESPONSE
//	STEP_UDS_SCR_FRONT_TEMP_REQUEST,
//	STEP_UDS_SCR_FRONT_TEMP_RESPONSE,
//	STEP_UDS_SCR_BACK_TEMP_REQUEST,
//	STEP_UDS_SCR_BACK_TEMP_RESPONSE,
//	STEP_UDS_NOX_CONTEST_REQUEST,
//	STEP_UDS_NOX_CONTEST_RESPONSE,
//	STEP_UDS_UREA_LIQUID_LEVEL_REQUEST,
//	STEP_UDS_UREA_LIQUID_LEVEL_RESPONSE
}UdsStep_t;

typedef void (*pUdsServer_t)(const uint8_t* pData, uint8_t DataLen);

typedef struct
{
	uint32_t UdsReqCanId;
	uint32_t UdsResCanId;
	uint16_t UdsBaud;
	UdsProtocol_t UdsProtocolNum;
	uint8_t ReqList[9][8];
	pUdsServer_t FunctionPro[9];
}UdsList_t;


static void HeartResponse(const uint8_t* pData, uint8_t DataLen);
static void UpLinkResponse(const uint8_t* pData, uint8_t DataLen);
static void GetUdsData_256A_B(const uint8_t* pData, uint8_t DataLen);
static void GetUdsData_256A_B_Div2(const uint8_t* pData, uint8_t DataLen);
static void GetUdsData_256A_B_Div4(const uint8_t* pData, uint8_t DataLen);
static void GetUdsData_256A_B_Div8(const uint8_t* pData, uint8_t DataLen);
static void GetUdsData_256A_B_Div10(const uint8_t* pData, uint8_t DataLen);
static void GetUdsData_256B_A(const uint8_t* pData, uint8_t DataLen);

static void GetScrTempJieFang(const uint8_t* pData, uint8_t DataLen);
static void GetNoxContestJieFang(const uint8_t* pData, uint8_t DataLen);
static void GetUreaLiquidLevelJieFang(const uint8_t* pData, uint8_t DataLen);

static void GetScrTemp_256A_B_Div10_Sub273(const uint8_t* pData, uint8_t DataLen);

static void GetUdsData_YiLiLanJie(const uint8_t* pData, uint8_t DataLen);

static void GetUdsData_ISO15031_256A_B_Div4(const uint8_t* pData, uint8_t DataLen);/*解析方式 (A*256+B)/4*/
static void GetUdsData_ISO15031_256A_B_Div10(const uint8_t* pData, uint8_t DataLen);/*解析方式 (A*256+B)/10*/
static void GetUdsData_ISO15031_256A_B_Div10_Sub40(const uint8_t* pData, uint8_t DataLen);
static void GetUdsData_ISO15031_256B_C(const uint8_t* pData, uint8_t DataLen);
static void GetUdsData_ISO15031_100B_Div255(const uint8_t* pData, uint8_t DataLen);
static void GetUdsData_ISO15031_100A_Div255(const uint8_t* pData, uint8_t DataLen);/*解析方式 (A*100/256)*/
static void GetUdsData_ISO15031_256A_B_Mul99_Div1000(const uint8_t* pData, uint8_t DataLen);/*解析方式 (A*256+B)*0.099*/

static void GetUdsData_16777216A_65536B_256C_D_Sub270(const uint8_t* pData, uint8_t DataLen); /*解析方式： (16777216*A+65536*B+256*C+D)-270  */
static void GetUdsData_100D_Div250(const uint8_t* pData, uint8_t DataLen); /*解析方式： 100D/250  */
static void GetUdsData_256A_B_Mul625_Div10000_Sub40(const uint8_t* pData, uint8_t DataLen); /*解析方式： (256*A+B)*0.0625-40  */
static void GetUdsData_256A_B_Div100(const uint8_t* pData, uint8_t DataLen); /*解析方式： (256*A+B)/100  */

static void GetUdsSpeed_KaiRuiWeiKeSi(const uint8_t* pData, uint8_t DataLen); /* 凯瑞伟柯斯 解析方式： (256*A+B)/10  */
static void GetScrTemp_KaiRuiWeiKeSi(const uint8_t* pData, uint8_t DataLen); /* 凯瑞伟柯斯 解析方式： (256*A+B)-40  */
static void GetNoxContest_KaiRuiWeiKeSi(const uint8_t* pData, uint8_t DataLen); /* 凯瑞伟柯斯 解析方式： (256*A+B)  */
static void GetUreaLiquidLevel_KaiRuiWeiKeSi(const uint8_t* pData, uint8_t DataLen); /* 凯瑞伟柯斯 解析方式： A(单位mm)  */

static void GetUdsSpeed_BoShiYuChai(const uint8_t* pData, uint8_t DataLen);  /*博世后处理，玉柴博世DCU  解析方式： (256*B+A)  */
static void GetUdsScrBackTemp_BoShiYuChai(const uint8_t* pData, uint8_t DataLen);  /*博世后处理，玉柴博世DCU  解析方式： (256*B+A)*0.0625-273  */
static void GetUdsNoxContest_BoShiYuChai(const uint8_t* pData, uint8_t DataLen);  /*博世后处理，玉柴博世DCU  解析方式： (256*B+A)  */
static void GetUdsUreaLiquidLevel_BoShiYuChai(const uint8_t* pData, uint8_t DataLen);  /*博世后处理，玉柴博世DCU  解析方式： (256*B+A)/100  */

static void GetUdsSpeed_KaiDeSi(const uint8_t* pData, uint8_t DataLen);  /* 凯德斯 解析方式： (256*A+B)/8  */
static void GetUdsData_256A_B_KaiDeSi(const uint8_t* pData, uint8_t DataLen);   /* 凯德斯 解析方式： (256*A+B)  */
static void GetUdsUreaLiquidLevel_KaiDeSi(const uint8_t* pData, uint8_t DataLen); /* 凯德斯 解析方式： A  */

static void GetUdsSpeed_KaiLong(const uint8_t* pData, uint8_t DataLen); /*凯龙解析方式:256*B+A)/8  */
static void GetScrTemp_KaiLong(const uint8_t* pData, uint8_t DataLen); /*凯龙解析方式:256*A+B-40  */
static void GetNoxContest_KaiLong(const uint8_t* pData, uint8_t DataLen); /*凯龙解析方式:(256*A+B)*0.05-200  */
static void GetUreaLiquidLevel_KaiLong(const uint8_t* pData, uint8_t DataLen); /*凯龙解析方式:A*100/250  */

static void GetUdsSpeed_HengHe(const uint8_t* pData, uint8_t DataLen);/*解析方式 (A*256+B)/4*/
static void GetScrTemp_HengHe(const uint8_t* pData, uint8_t DataLen);/*解析方式 (A*256+B)*0.1-40*/
static void GetNoxContest_HengHe(const uint8_t* pData, uint8_t DataLen);/*解析方式 */
static void GetUreaLiquidLevel_HengHe(const uint8_t* pData, uint8_t DataLen);/*解析方式 B*100/255*/
static void GetUdsData_256A_B_sign(const uint8_t* pData, uint8_t DataLen); /*解析方式 A*256+B*/  //最高位为符号位

static void Datong_TEMP_LQY(const uint8_t* pData, uint8_t DataLen) ;
static void Datong_enginefuelFlow(const uint8_t* pData, uint8_t DataLen);
static void Datong_engineTorq(const uint8_t* pData, uint8_t DataLen);

static void GetNOX_benchi(const uint8_t* pData, uint8_t DataLen);//*解析方式 C*256+D
static void GetUreaLiquidLevel_benchi(const uint8_t* pData, uint8_t DataLen);//*解析方蔆*256+D/100
	
static void GetUreaLiquidLevel_50Ling(const uint8_t* pData, uint8_t DataLen);//*解析方式 C*0.4
//void UdsProcess(ProtocolNum_t Procotol, uint8_t DealyFlag);
static void Get07_45data(const uint8_t* pData, uint8_t DataLen);

uint8_t SetUdsProtocol(UdsProtocol_t ProtocolNum);
void SetUdsEnFlag(u8 udsEnFlag);
void SetUdsMode(u8 udsmode);
void UdsMatchTypeNum(u8 udsnum);
void UdsCheckPid(CanData_t *pCanMsg, uint8_t FrameCnt);
void UdsReqServer(ProtocolNum_t Procotol, uint8_t DealyFlag);
void UdsCmdDisSend(UdsReqCmd_t cmd);
void UdsCmdEnSend(UdsReqCmd_t cmd);
void ShowData2App(void);
#endif


