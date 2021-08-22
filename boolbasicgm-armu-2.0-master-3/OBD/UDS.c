#include "UDS.h"
#include "CAN.h"
#include "CanDataAnalysis.h"
#include "gps.h"
#include "spi_flash.h"
#include "sim_net.h"
uint16_t APP_Nox_indicate = 0;
uint16_t APP_reag_indicate= 0;
u8 flag_50ling_send=1;
extern uint8_t guide;
extern ProtocolNum_t ISO5765ProNum;
extern  u8 GetIsHave1939Pro(void);
static const uint8_t ISO15031ReqSpeed[]={0x02,0x01,0x0C,0x00,0x00,0x00,0x00,0x00};
static const uint32_t UdsCanIdList[] = {0x18DA00F1,0x18DA00FA,0x18DA00FB,0x18DA10F1,0x18DA3DF1,0x18DB3DF1,0x18DA3DF9,0x18DA7FFA,0x18DB33F1,0X7E8};
static const UdsList_t UdsReqList[]={
																					// 0 重汽
																					{
																						.UdsReqCanId = 0x18DA3DF1,
																						.UdsResCanId = 0x18DAF13D,
																						.UdsBaud = 500,
																						.UdsProtocolNum = ZhongQi_0,
																						.ReqList=
																						{
																							{0x02,0x3E,0x00,0x00,0x00,0x00,0x00,0x00}, //心跳
																							{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, //建立连接
																							{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, //安全访问请求seed
																							{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, //安全访问请求解密结果
																							{0x03,0x22,0x10,0x31,0x00,0x00,0x00,0x00}, //请求转速
																							{0x03,0x22,0x10,0x11,0x00,0x00,0x00,0x00}, //请求SCR前温
																							{0x03,0x22,0x10,0x12,0x00,0x00,0x00,0x00}, //请求SCR后温
																							{0x03,0x22,0x10,0x2A,0x00,0x00,0x00,0x00}, //请求NOX浓度
																							{0x03,0x22,0x10,0x14,0x00,0x00,0x00,0x00}, //请求尿素液位
																						},
																						.FunctionPro=
																						{
																							[UDS_HEART]                = HeartResponse,
																							[UDS_LINK]                 = NULL,
																							[UDS_SECURITY_ACCESS_SEED] = NULL,
																							[UDS_SECURITY_ACCESS_RES]  = NULL,
																							[UDS_SPEED]                = GetUdsData_256A_B,
																							[UDS_SCR_FRONT_TEMP]       = GetUdsData_256A_B_sign,
																							[UDS_SCR_BACK_TEMP]        = GetUdsData_256A_B_sign,
																							[UDS_NOX_CONTEST]          = GetUdsData_256A_B,
																							[UDS_UREA_LIQUID_LEVEL]    = GetUdsData_256A_B,
																						},
																					},
																					// 1 东风商用后处理，国五-ACM
																					{
																						.UdsReqCanId = 0x18DA3DF1,
																						.UdsResCanId = 0x18DAF13D,
																						.UdsBaud = 250,
																						.UdsProtocolNum = DongFengACM_1,
																						.ReqList=
																						{
																							{0x02,0x3E,0x00,0x00,0x00,0x00,0x00,0x00}, //心跳
																							{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, //建立连接
																							{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, //安全访问请求seed
																							{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, //安全访问请求解密结果
																							{0x03,0x22,0x04,0x0F,0x00,0x00,0x00,0x00}, //请求转速
																							{0x03,0x22,0x04,0x32,0x00,0x00,0x00,0x00}, //请求SCR前温
																							{0x03,0x22,0x04,0x39,0x00,0x00,0x00,0x00}, //请求SCR后温
																							{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, //请求NOX浓度
																							{0x03,0x22,0x04,0x40,0x00,0x00,0x00,0x00}, //请求尿素液位
																						},
																						.FunctionPro=
																						{
																							[UDS_HEART]                = HeartResponse,
																							[UDS_LINK]                 = NULL,
																							[UDS_SECURITY_ACCESS_SEED] = NULL,
																							[UDS_SECURITY_ACCESS_RES]  = NULL,
																							[UDS_SPEED]                = GetUdsData_256A_B,
																							[UDS_SCR_FRONT_TEMP]       = GetUdsData_16777216A_65536B_256C_D_Sub270,
																							[UDS_SCR_BACK_TEMP]        = GetUdsData_16777216A_65536B_256C_D_Sub270,
																							[UDS_NOX_CONTEST]          = NULL,
																							[UDS_UREA_LIQUID_LEVEL]    = GetUdsData_100D_Div250,
																						},
																					},
																					// 2 东风商用后处理，DINE-ACU,迪奈斯，东风轻卡
																					{
																						.UdsReqCanId = 0x18DA3DF1,
																						.UdsResCanId = 0x18DAF13D,
																						.UdsBaud = 250,
																						.UdsProtocolNum = DongFeng_2,
																						.ReqList=
																						{
																							{0x02,0x3E,0x00,0x00,0x00,0x00,0x00,0x00}, //心跳
																							{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, //建立连接
																							{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, //安全访问请求seed
																							{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, //安全访问请求解密结果
																							{0x03,0x22,0xFD,0x00,0x00,0x00,0x00,0x00}, //请求转速
																							{0x03,0x22,0xFD,0x14,0x00,0x00,0x00,0x00}, //请求SCR前温
																							{0x03,0x22,0xFD,0x15,0x00,0x00,0x00,0x00}, //请求SCR后温
																							{0x03,0x22,0xFD,0x1C,0x00,0x00,0x00,0x00}, //请求NOX浓度
																							{0x03,0x22,0xFD,0x12,0x00,0x00,0x00,0x00}, //请求尿素液位
																						},
																						.FunctionPro=
																						{
																							[UDS_HEART]                = HeartResponse,
																							[UDS_LINK]                 = NULL,
																							[UDS_SECURITY_ACCESS_SEED] = NULL,
																							[UDS_SECURITY_ACCESS_RES]  = NULL,
																							[UDS_SPEED]                = GetUdsData_256A_B_Div4,
																							[UDS_SCR_FRONT_TEMP]       = GetUdsData_256A_B_Mul625_Div10000_Sub40,
																							[UDS_SCR_BACK_TEMP]        = GetUdsData_256A_B_Mul625_Div10000_Sub40,
																							[UDS_NOX_CONTEST]          = GetUdsData_256A_B,
																							[UDS_UREA_LIQUID_LEVEL]    = GetUdsData_256A_B_Div100,
																						},
																					},
																					// 3 凯瑞伟柯斯，朝柴凯瑞柯斯
																					{
																						.UdsReqCanId = 0x18DA3DF1,
																						.UdsResCanId = 0x18DAF13D,
																						.UdsBaud = 250,
																						.UdsProtocolNum = KaiRuiWeiKeSi_3,
																						.ReqList=
																						{
																							{0x02,0x3E,0x00,0x00,0x00,0x00,0x00,0x00}, //心跳
																							{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, //建立连接
																							{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, //安全访问请求seed
																							{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, //安全访问请求解密结果
																							{0x04,0x2F,0x01,0x00,0x04,0x00,0x00,0x00}, //请求转速
																							{0x04,0x2F,0x51,0x00,0x01,0x00,0x00,0x00}, //请求SCR前温
																							{0x04,0x2F,0x51,0x00,0x02,0x00,0x00,0x00}, //请求SCR后温
																							{0x04,0x2F,0x51,0x00,0x05,0x00,0x00,0x00}, //请求NOX浓度
																							{0x04,0x2F,0x51,0x00,0x03,0x00,0x00,0x00}, //请求尿素液位
																						},
																						.FunctionPro=
																						{
																							[UDS_HEART]                = HeartResponse,
																							[UDS_LINK]                 = NULL,
																							[UDS_SECURITY_ACCESS_SEED] = NULL,
																							[UDS_SECURITY_ACCESS_RES]  = NULL,
																							[UDS_SPEED]                = GetUdsSpeed_KaiRuiWeiKeSi,
																							[UDS_SCR_FRONT_TEMP]       = GetScrTemp_KaiRuiWeiKeSi,
																							[UDS_SCR_BACK_TEMP]        = GetScrTemp_KaiRuiWeiKeSi,
																							[UDS_NOX_CONTEST]          = GetNoxContest_KaiRuiWeiKeSi,
																							[UDS_UREA_LIQUID_LEVEL]    = GetUreaLiquidLevel_KaiRuiWeiKeSi,  /*得到的液位单位是mm，后面需要再处理*/
																						},
																					},
																					// 4 ,天纳克，重汽SCR
																					{
																						.UdsReqCanId = 0x18DA3DF1,
																						.UdsResCanId = 0x18DAF13D,
																						.UdsBaud = 500,
																						.UdsProtocolNum = TianNaKe_4,
																						.ReqList=
																						{
																							{0x02,0x3E,0x00,0x00,0x00,0x00,0x00,0x00}, //心跳
																							{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, //建立连接
																							{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, //安全访问请求seed
																							{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, //安全访问请求解密结果
																							{0x02,0x01,0x0C,0x00,0x00,0x00,0x00,0x00}, //请求转速
																							{0x02,0x01,0x3C,0x00,0x00,0x00,0x00,0x00}, //请求SCR前温
																							{0x02,0x01,0x3E,0x00,0x00,0x00,0x00,0x00}, //请求SCR后温
																							{0x02,0x01,0x5D,0x00,0x00,0x00,0x00,0x00}, //请求NOX浓度
																							{0x02,0x01,0x5B,0x00,0x00,0x00,0x00,0x00}, //请求尿素液位
																						},
																						.FunctionPro=
																						{
																							[UDS_HEART]                = HeartResponse,
																							[UDS_LINK]                 = NULL,
																							[UDS_SECURITY_ACCESS_SEED] = NULL,
																							[UDS_SECURITY_ACCESS_RES]  = NULL,
																							[UDS_SPEED]                = GetUdsData_ISO15031_256A_B_Div4,
																							[UDS_SCR_FRONT_TEMP]       = GetUdsData_ISO15031_256A_B_Div10_Sub40,
																							[UDS_SCR_BACK_TEMP]        = GetUdsData_ISO15031_256A_B_Div10_Sub40,
																							[UDS_NOX_CONTEST]          = GetUdsData_ISO15031_256A_B_Div10,
																							[UDS_UREA_LIQUID_LEVEL]    = GetUdsData_ISO15031_100A_Div255,
																						},
																					},
																					/* 5 ZD30后处理器，东风轻卡*/
																					{
																						.UdsReqCanId = 0x18DA10F1,
																						.UdsResCanId = 0x18DAF110,
																						.UdsBaud = 500,
																						.UdsProtocolNum = ZD30_5,
																						.ReqList=
																						{
																							{0x02,0x3E,0x00,0x00,0x00,0x00,0x00,0x00}, //心跳
																							{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, //建立连接
																							{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, //安全访问请求seed
																							{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, //安全访问请求解密结果
																							{0x02,0x01,0x0C,0x00,0x00,0x00,0x00,0x00}, //请求转速
																							{0x02,0x01,0x3D,0x00,0x00,0x00,0x00,0x00}, //请求SCR前温
																							{0x02,0x01,0x3E,0x00,0x00,0x00,0x00,0x00}, //请求SCR后温
																							{0x02,0x01,0xA3,0x00,0x00,0x00,0x00,0x00}, //请求NOX浓度
																							{0x02,0x01,0xA5,0x00,0x00,0x00,0x00,0x00}, //请求尿素液位
																						},
																						.FunctionPro=
																						{
																							[UDS_HEART]                = HeartResponse,
																							[UDS_LINK]                 = NULL,
																							[UDS_SECURITY_ACCESS_SEED] = NULL,
																							[UDS_SECURITY_ACCESS_RES]  = NULL,
																							[UDS_SPEED]                = GetUdsData_ISO15031_256A_B_Div4,
																							[UDS_SCR_FRONT_TEMP]       = GetUdsData_ISO15031_256A_B_Div10_Sub40,
																							[UDS_SCR_BACK_TEMP]        = GetUdsData_ISO15031_256A_B_Div10_Sub40,
																							[UDS_NOX_CONTEST]          = GetUdsData_ISO15031_256B_C,
																							[UDS_UREA_LIQUID_LEVEL]    = GetUdsData_ISO15031_100B_Div255,
																						},
																					},																					
																					/* 6 亿利蓝洁，玉柴*/
																					{
																						.UdsReqCanId = 0x18DA10F1,
																						.UdsResCanId = 0x18DAF110,
																						.UdsBaud = 250,
																						.UdsProtocolNum = YiLiLanJie_6,
																						.ReqList=
																						{
																							{0x02,0x3E,0x00,0x00,0x00,0x00,0x00,0x00}, //心跳
																							{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, //建立连接
																							{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, //安全访问请求seed
																							{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, //安全访问请求解密结果
																							{0x02,0x21,0x21,0x00,0x00,0x00,0x00,0x00}, //请求转速
																							{0x02,0x21,0x20,0x00,0x00,0x00,0x00,0x00}, //请求SCR前温,SCR后温,NOX浓度,尿素液位
																							{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
																							{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
																							{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
																						},
																						.FunctionPro=
																						{
																							[UDS_HEART]                = HeartResponse,
																							[UDS_LINK]                 = NULL,
																							[UDS_SECURITY_ACCESS_SEED] = NULL,
																							[UDS_SECURITY_ACCESS_RES]  = NULL,
																							[UDS_SPEED]                = GetUdsData_YiLiLanJie,
																							[UDS_SCR_FRONT_TEMP]       = GetUdsData_YiLiLanJie,
																							[UDS_SCR_BACK_TEMP]        = NULL,
																							[UDS_NOX_CONTEST]          = NULL,
																							[UDS_UREA_LIQUID_LEVEL]    = NULL,
																						},
																					},																					
																					/* 7 依可卡特*/ //一汽解放四环
																					{
																						.UdsReqCanId = 0x18DA10F1,
																						.UdsResCanId = 0x18DAF110,
																						.UdsBaud = 500,
																						.UdsProtocolNum = YiKeKaTei_7,
																						.ReqList=
																						{
																							{0x02,0x3E,0x00,0x00,0x00,0x00,0x00,0x00}, //心跳
																							{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, //建立连接
																							{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, //安全访问请求seed
																							{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, //安全访问请求解密结果
																							{0x02,0x21,0x21,0x00,0x00,0x00,0x00,0x00}, //请求转速
																							{0x02,0x21,0x20,0x00,0x00,0x00,0x00,0x00}, //请求SCR前温,SCR后温,NOX浓度,尿素液位
																							{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
																							{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
																							{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
																						},
																						.FunctionPro=
																						{
																							[UDS_HEART]                = HeartResponse,
																							[UDS_LINK]                 = NULL,
																							[UDS_SECURITY_ACCESS_SEED] = NULL,
																							[UDS_SECURITY_ACCESS_RES]  = NULL,
																							[UDS_SPEED]                = GetUdsData_YiLiLanJie, /*和亿利蓝洁一样*/
																							[UDS_SCR_FRONT_TEMP]       = GetUdsData_YiLiLanJie,
																							[UDS_SCR_BACK_TEMP]        = NULL,
																							[UDS_NOX_CONTEST]          = NULL,
																							[UDS_UREA_LIQUID_LEVEL]    = NULL,
																						},
																					},																					
//																					
																					// 8 凯德斯后处理
																					{
																						.UdsReqCanId = 0x18DA3DF9,
																						.UdsResCanId = 0x18DAF93D,
																						.UdsBaud = 250,
																						.UdsProtocolNum = KaiDeSi_8,
																						.ReqList=
																						{
																							{0x02,0x3E,0x00,0x00,0x00,0x00,0x00,0x00}, //心跳
																							{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, //建立连接
																							{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, //安全访问请求seed
																							{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, //安全访问请求解密结果
																							{0x05,0x23,0x00,0x28,0x32,0x02,0x00,0x00}, //请求转速
																							{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, //请求SCR前温
																							{0x05,0x23,0x00,0x27,0xB8,0x02,0x00,0x00}, //请求SCR后温
																							{0x05,0x23,0x00,0x27,0x0E,0x02,0x00,0x00}, //请求NOX浓度
																							{0x05,0x23,0x00,0x27,0x80,0x01,0x00,0x00}, //请求尿素液位
																						},
																						.FunctionPro=
																						{
																							[UDS_HEART]                = HeartResponse,
																							[UDS_LINK]                 = NULL,
																							[UDS_SECURITY_ACCESS_SEED] = NULL,
																							[UDS_SECURITY_ACCESS_RES]  = NULL,
																							[UDS_SPEED]                = GetUdsSpeed_KaiDeSi,
																							[UDS_SCR_FRONT_TEMP]       = NULL,
																							[UDS_SCR_BACK_TEMP]        = GetUdsData_256A_B_KaiDeSi,
																							[UDS_NOX_CONTEST]          = GetUdsData_256A_B_KaiDeSi,
																							[UDS_UREA_LIQUID_LEVEL]    = GetUdsUreaLiquidLevel_KaiDeSi,
																						},
																					},																					
																					// 9 大通-----------------------------------------------------------------------
																					{
																						.UdsReqCanId = 0x7E0,
																						.UdsResCanId = 0x7E8,
																						.UdsBaud = 500,
																						.UdsProtocolNum = DaTong_9,
																						.ReqList=
																						{
																							{0x02,0x3E,0x00,0x00,0x00,0x00,0x00,0x00}, //心跳
																							{0x02,0x10,0x03,0x00,0x00,0x00,0x00,0x00}, //建立连接
																							{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, //安全访问请求seed
																							{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, //安全访问请求解密结果
																							{0x05,0x23,0x40,0xC8,0x36,0x02,0x00,0x00}, //请求转速
																							{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, //请求SCR前温
																							{0x03,0x22,0x01,0x1C,0x00,0x00,0x00,0x00}, //请求冷却液温度
																							{0x0E,0x22,0x01,0x8A,0x00,0x00,0x00,0x00}, //请求燃油流量
																							{0x0E,0x22,0x01,0x07,0x00,0x00,0x00,0x00}, //请求扭矩
																						},
																						.FunctionPro=
																						{
																							[UDS_HEART]                = HeartResponse,
																							[UDS_LINK]                 = UpLinkResponse,
																							[UDS_SECURITY_ACCESS_SEED] = NULL,
																							[UDS_SECURITY_ACCESS_RES]  = NULL,
																							[UDS_SPEED]                = GetUdsSpeed_BoShiYuChai,
																							[UDS_SCR_FRONT_TEMP]       = NULL,
																							[UDS_SCR_BACK_TEMP]        = Datong_TEMP_LQY,
																							[UDS_NOX_CONTEST]          = Datong_enginefuelFlow,
																							[UDS_UREA_LIQUID_LEVEL]    = Datong_engineTorq,
																						},
																					},
																					/* A 解放，解放自主气驱*/
																					{
																						.UdsReqCanId = 0x18DA00F1,
																						.UdsResCanId = 0x18DAF100,
																						.UdsBaud = 500,
																						.UdsProtocolNum = JieFang_10,
																						.ReqList=
																						{
																							{0x02,0x3E,0x00,0x00,0x00,0x00,0x00,0x00}, //心跳
																							{0x02,0x10,0x03,0x00,0x00,0x00,0x00,0x00}, //建立连接
																							{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, //安全访问请求seed
																							{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, //安全访问请求解密结果
																							{0x03,0x22,0x01,0x65,0x00,0x00,0x00,0x00}, //请求转速
																							{0x03,0x22,0x01,0x4D,0x00,0x00,0x00,0x00}, //请求SCR前温
																							{0x03,0x22,0x01,0x4F,0x00,0x00,0x00,0x00}, //请求SCR后温
																							{0x03,0x22,0x01,0x50,0x00,0x00,0x00,0x00}, //请求NOX浓度
																							{0x03,0x22,0x01,0x52,0x00,0x00,0x00,0x00}, //请求尿素液位
																						},
																						.FunctionPro=
																						{
																							[UDS_HEART]                = HeartResponse,
																							[UDS_LINK]                 = UpLinkResponse,
																							[UDS_SECURITY_ACCESS_SEED] = NULL,
																							[UDS_SECURITY_ACCESS_RES]  = NULL,
																							[UDS_SPEED]                = GetUdsData_256A_B,
																							[UDS_SCR_FRONT_TEMP]       = GetScrTempJieFang,
																							[UDS_SCR_BACK_TEMP]        = GetScrTempJieFang,
																							[UDS_NOX_CONTEST]          = GetNoxContestJieFang,
																							[UDS_UREA_LIQUID_LEVEL]    = GetUreaLiquidLevelJieFang,
																						},
																					},																			
																					/* B 博世重汽*/   //int
																					{
																						.UdsReqCanId = 0x18DA00F1,
																						.UdsResCanId = 0x18DAF100,
																						.UdsBaud = 500,
																						.UdsProtocolNum = BoShiZhongQi_11,
																						.ReqList=
																						{
																							{0x02,0x3E,0x00,0x00,0x00,0x00,0x00,0x00}, //心跳
																							{0x02,0x10,0x03,0x00,0x00,0x00,0x00,0x00}, //建立连接
																							{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, //安全访问请求seed
																							{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, //安全访问请求解密结果
																							{0x03,0x22,0x01,0x5E,0x00,0x00,0x00,0x00}, //请求转速
																							{0x03,0x22,0x40,0x15,0x00,0x00,0x00,0x00}, //请求SCR前温
																							{0x03,0x22,0x40,0x15,0x00,0x00,0x00,0x00}, //请求SCR后温
																							{0x03,0x22,0x40,0x33,0x00,0x00,0x00,0x00}, //请求NOX浓度
																							{0x03,0x22,0x40,0x33,0x00,0x00,0x00,0x00}, //请求尿素液位
																						},
																						.FunctionPro=
																						{
																							[UDS_HEART]                = HeartResponse,
																							[UDS_LINK]                 = UpLinkResponse,
																							[UDS_SECURITY_ACCESS_SEED] = NULL,
																							[UDS_SECURITY_ACCESS_RES]  = NULL,
																							[UDS_SPEED]                = GetUdsData_256A_B_Div2,
																							[UDS_SCR_FRONT_TEMP]       = GetScrTemp_256A_B_Div10_Sub273,
																							[UDS_SCR_BACK_TEMP]        = NULL,
																							[UDS_NOX_CONTEST]          = GetUdsData_256A_B,
																							[UDS_UREA_LIQUID_LEVEL]    = NULL,
																						},
																					},

																					/* C 博世锡柴1*/
																					{
																						.UdsReqCanId = 0x18DA00FA,
																						.UdsResCanId = 0x18DAFA00,
																						.UdsBaud = 500,
																						.UdsProtocolNum = BoShiXiChai_12,
																						.ReqList=
																						{
																							{0x02,0x3E,0x00,0x00,0x00,0x00,0x00,0x00}, //心跳
																							{0x02,0x10,0x03,0x00,0x00,0x00,0x00,0x00}, //建立连接
																							{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, //安全访问请求seed
																							{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, //安全访问请求解密结果
																							{0x03,0x22,0x01,0x5E,0x00,0x00,0x00,0x00}, //请求转速
																							{0x03,0x22,0x40,0x15,0x00,0x00,0x00,0x00}, //请求SCR前温
																							{0x03,0x22,0x40,0x15,0x00,0x00,0x00,0x00}, //请求SCR后温
																							{0x03,0x22,0x40,0x33,0x00,0x00,0x00,0x00}, //请求NOX浓度
																							{0x03,0x22,0x40,0x33,0x00,0x00,0x00,0x00}, //请求尿素液位
																						},
																						.FunctionPro=
																						{
																							[UDS_HEART]                = HeartResponse,
																							[UDS_LINK]                 = UpLinkResponse,
																							[UDS_SECURITY_ACCESS_SEED] = NULL,
																							[UDS_SECURITY_ACCESS_RES]  = NULL,
																							[UDS_SPEED]                = GetUdsData_256A_B_Div2,
																							[UDS_SCR_FRONT_TEMP]       = GetScrTemp_256A_B_Div10_Sub273,
																							[UDS_SCR_BACK_TEMP]        = NULL,
																							[UDS_NOX_CONTEST]          = GetUdsData_256A_B,
																							[UDS_UREA_LIQUID_LEVEL]    = NULL,
																						},
																					},
																					/* D 博世锡柴2*/
																					{
																						.UdsReqCanId = 0x18DA00FB,
																						.UdsResCanId = 0x18DAFB00,
																						.UdsBaud = 500,
																						.UdsProtocolNum = BoShiXiChai_13,
																						.ReqList=
																						{
																							{0x02,0x3E,0x00,0x00,0x00,0x00,0x00,0x00}, //心跳
																							{0x02,0x10,0x03,0x00,0x00,0x00,0x00,0x00}, //建立连接
																							{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, //安全访问请求seed
																							{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, //安全访问请求解密结果
																							{0x03,0x21,0x00,0x5E,0x00,0x00,0x00,0x00}, //请求转速
																							{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, //请求SCR前温
																							{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, //请求SCR后温
																							{0x03,0x21,0x40,0x33,0x00,0x00,0x00,0x00}, //请求NOX浓度
																							{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, //请求尿素液位
																						},
																						.FunctionPro=
																						{
																							[UDS_HEART]                = HeartResponse,
																							[UDS_LINK]                 = UpLinkResponse,
																							[UDS_SECURITY_ACCESS_SEED] = NULL,
																							[UDS_SECURITY_ACCESS_RES]  = NULL,
																							[UDS_SPEED]                = GetUdsData_256A_B_Div2,
																							[UDS_SCR_FRONT_TEMP]       = NULL,
																							[UDS_SCR_BACK_TEMP]        = NULL,
																							[UDS_NOX_CONTEST]          = GetUdsData_256A_B_Div2,
																							[UDS_UREA_LIQUID_LEVEL]    = NULL,
																						},
																					},
																					// E 凯龙后处理
																					{
																						.UdsReqCanId = 0x18DA7FFA,
																						.UdsResCanId = 0x18DAFA7F,
																						.UdsBaud = 250,
																						.UdsProtocolNum = KaiLong_14,
																						.ReqList=
																						{
																							{0x02,0x3E,0x00,0x00,0x00,0x00,0x00,0x00}, //心跳
																							{0x02,0x10,0x49,0x00,0x00,0x00,0x00,0x00}, //建立连接
																							{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, //安全访问请求seed
																							{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, //安全访问请求解密结果
																							{0x03,0x22,0x51,0xFE,0x00,0x00,0x00,0x00}, //请求转速
																							{0x03,0x22,0xA0,0xF2,0x00,0x00,0x00,0x00}, //请求SCR前温
																							{0x03,0x22,0xA0,0xF1,0x00,0x00,0x00,0x00}, //请求SCR后温
																							{0x03,0x22,0xA0,0xF0,0x00,0x00,0x00,0x00}, //请求NOX浓度
																							{0x03,0x22,0xA0,0xF3,0x00,0x00,0x00,0x00}, //请求尿素液位
																						},
																						.FunctionPro=
																						{
																							[UDS_HEART]                = HeartResponse,
																							[UDS_LINK]                 = UpLinkResponse,
																							[UDS_SECURITY_ACCESS_SEED] = NULL,
																							[UDS_SECURITY_ACCESS_RES]  = NULL,
																							[UDS_SPEED]                = GetUdsSpeed_KaiLong,
																							[UDS_SCR_FRONT_TEMP]       = GetScrTemp_KaiLong,
																							[UDS_SCR_BACK_TEMP]        = GetScrTemp_KaiLong,
																							[UDS_NOX_CONTEST]          = GetNoxContest_KaiLong,
																							[UDS_UREA_LIQUID_LEVEL]    = GetUreaLiquidLevel_KaiLong,
																						},
																					},																				
																					// F 天纳华凌，天纳克(解放)
																					{
																						.UdsReqCanId = 0x18DB3DF1,
																						.UdsResCanId = 0x18DAF13D,
																						.UdsBaud = 500,
																						.UdsProtocolNum = TianNa_15,
																						.ReqList=
																						{
																							{0x02,0x3E,0x00,0x00,0x00,0x00,0x00,0x00}, //心跳
																							{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, //建立连接
																							{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, //安全访问请求seed
																							{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, //安全访问请求解密结果
																							{0x02,0x01,0x0C,0x00,0x00,0x00,0x00,0x00}, //请求转速
																							{0x02,0x01,0x3C,0x00,0x00,0x00,0x00,0x00}, //请求SCR前温
																							{0x02,0x01,0x3E,0x00,0x00,0x00,0x00,0x00}, //请求SCR后温
																							{0x02,0x01,0x5D,0x00,0x00,0x00,0x00,0x00}, //请求NOX浓度
																							{0x02,0x01,0x5B,0x00,0x00,0x00,0x00,0x00}, //请求尿素液位
																						},
																						.FunctionPro=
																						{
																							[UDS_HEART]                = HeartResponse,
																							[UDS_LINK]                 = NULL,
																							[UDS_SECURITY_ACCESS_SEED] = NULL,
																							[UDS_SECURITY_ACCESS_RES]  = NULL,
																							[UDS_SPEED]                = GetUdsData_ISO15031_256A_B_Div4,
																							[UDS_SCR_FRONT_TEMP]       = GetUdsData_ISO15031_256A_B_Mul99_Div1000,
																							[UDS_SCR_BACK_TEMP]        = GetUdsData_ISO15031_256A_B_Mul99_Div1000,
																							[UDS_NOX_CONTEST]          = GetUdsData_ISO15031_256A_B_Div10,
																							[UDS_UREA_LIQUID_LEVEL]    = GetUdsData_ISO15031_100A_Div255,
																						},
																					},
																				 // 10 恒和后处理
																					{
																						.UdsReqCanId = 0x18DB33F1,
																						.UdsResCanId = 0x18DAF13D,
																						.UdsBaud = 250,
																						.UdsProtocolNum = HengHe_16,
																						.ReqList=
																						{
																							{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, //心跳
																							{0x02,0x01,0x00,0x00,0x00,0x00,0x00,0x00}, //建立连接
																							{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, //安全访问请求seed
																							{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, //安全访问请求解密结果
																							{0x02,0x01,0x0C,0x00,0x00,0x00,0x00,0x00}, //请求转速
																							{0x02,0x01,0x3C,0x00,0x00,0x00,0x00,0x00}, //请求SCR前温
																							{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, //请求SCR后温
																							{0x02,0x01,0xA3,0x00,0x00,0x00,0x00,0x00}, //请求NOX浓度
																							{0x02,0x01,0xA5,0x00,0x00,0x00,0x00,0x00}, //请求尿素液位
																						},
																						.FunctionPro=
																						{
																							[UDS_HEART]                = NULL,
																							[UDS_LINK]                 = UpLinkResponse,
																							[UDS_SECURITY_ACCESS_SEED] = NULL,
																							[UDS_SECURITY_ACCESS_RES]  = NULL,
																							[UDS_SPEED]                = GetUdsSpeed_HengHe,
																							[UDS_SCR_FRONT_TEMP]       = GetScrTemp_HengHe,
																							[UDS_SCR_BACK_TEMP]        = NULL,
																							[UDS_NOX_CONTEST]          = GetNoxContest_HengHe,
																							[UDS_UREA_LIQUID_LEVEL]    = GetUreaLiquidLevel_HengHe,
																						},
																					}	,
																				 // 11 50铃后处理
																					{
																						.UdsReqCanId = 0X7E3,
																						.UdsResCanId = 0x7EB,
																						.UdsBaud = 500,
																						.UdsProtocolNum = wushiling_17,
																						.ReqList=
																						{
																							{0x01,0x3E,0x00,0x00,0x00,0x00,0x00,0x00}, //心跳
																							{0x01,0x20,0x00,0x00,0x00,0x00,0x00,0x00}, //建立连接
																							{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, //安全访问请求seed
																							{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, //安全访问请求解密结果
																							{0x02,0x01,0x0C,0x00,0x00,0x00,0x00,0x00}, //请求转速
																							{0x02,0x01,0x3C,0x00,0x00,0x00,0x00,0x00}, //请求SCR前温
																							{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, //请求SCR后温
																							{0x02,0x01,0xA3,0x00,0x00,0x00,0x00,0x00}, //请求NOX浓度
																							{0x04,0xAA,0x03,0xFE,0xFD,0x00,0x00,0x00}, //请求尿素液位
																						},
																						.FunctionPro=
																						{
																							[UDS_HEART]                = NULL,
																							[UDS_LINK]                 = NULL,
																							[UDS_SECURITY_ACCESS_SEED] = NULL,
																							[UDS_SECURITY_ACCESS_RES]  = NULL,
																							[UDS_SPEED]                = NULL,
																							[UDS_SCR_FRONT_TEMP]       = NULL,
																							[UDS_SCR_BACK_TEMP]        = NULL,
																							[UDS_NOX_CONTEST]          = NULL,
																							[UDS_UREA_LIQUID_LEVEL]    = GetUreaLiquidLevel_50Ling,
																						},
																					},
																				 //  12  奔驰ACTORS
																					{
																						.UdsReqCanId = 0X18DA3DF1,
																						.UdsResCanId = 0x18DAF13D,
																						.UdsBaud = 500,
																						.UdsProtocolNum = benchiact_18,
																						.ReqList=
																						{
																							{0x02,0x3E,0x00,0x00,0x00,0x00,0x00,0x00}, //心跳
																							{0x02,0x10,0x03,0x00,0x00,0x00,0x00,0x00}, //建立连接
																							{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, //安全访问请求seed
																							{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, //安全访问请求解密结果
																							{0x02,0x01,0x0C,0x00,0x00,0x00,0x00,0x00}, //请求转速
																							{0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, //请求SCR前温
																							{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, //请求SCR后温
																							{0x03,0x22,0x1D,0x72,0x00,0x00,0x00,0x00}, //请求NOX浓度
																							{0x03,0x22,0x1D,0x19,0x00,0x00,0x00,0x00}, //请求尿素液位
																						},
																						.FunctionPro=
																						{
																							[UDS_HEART]                = HeartResponse,
																							[UDS_LINK]                 = UpLinkResponse,
																							[UDS_SECURITY_ACCESS_SEED] = NULL,
																							[UDS_SECURITY_ACCESS_RES]  = NULL,
																							[UDS_SPEED]                = NULL,
																							[UDS_SCR_FRONT_TEMP]       = NULL,
																							[UDS_SCR_BACK_TEMP]        = NULL,
																							[UDS_NOX_CONTEST]          = GetNOX_benchi,
																							[UDS_UREA_LIQUID_LEVEL]    = GetUreaLiquidLevel_benchi,
																						},
																					},
	// 19 江陵-----------------------------------------------------------------------
																					{
																						.UdsReqCanId = 0x7E0,
																						.UdsResCanId = 0x7E8,
																						.UdsBaud = 500,
																						.UdsProtocolNum = JiangLing_19,
																						.ReqList=
																						{
																							{0x02,0x3E,0x00,0x00,0x00,0x00,0x00,0x00}, //心跳
																							{0x02,0x10,0x03,0x00,0x00,0x00,0x00,0x00}, //建立连接
																							{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, //安全访问请求seed
																							{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, //安全访问请求解密结果
																							{0x05,0x23,0x40,0xC8,0x36,0x02,0x00,0x00}, //请求转速
																							{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, //请求SCR前温
																							{0x03,0x22,0x01,0x1C,0x00,0x00,0x00,0x00}, //请求冷却液温度
																							{0x03,0x22,0x04,0x91,0x00,0x00,0x00,0x00}, //NOX浓度（can）/下游NOX传感器测的NOX浓度
																							{0x03,0x22,0x04,0x81,0x00,0x00,0x00,0x00}, //尿素罐液位物理值
																						},
																						.FunctionPro=
																						{
																							[UDS_HEART]                = HeartResponse,
																							[UDS_LINK]                 = UpLinkResponse,
																							[UDS_SECURITY_ACCESS_SEED] = NULL,
																							[UDS_SECURITY_ACCESS_RES]  = NULL,
																							[UDS_SPEED]                = NULL,
																							[UDS_SCR_FRONT_TEMP]       = NULL,
																							[UDS_SCR_BACK_TEMP]        = NULL,
																							[UDS_NOX_CONTEST]          = Get07_45data,
																							[UDS_UREA_LIQUID_LEVEL]    = Get07_45data,
																						},
																					},
                                     };

static uint8_t UdsProMatchCnt = 0; /*协议匹配次数，匹配一周加一*/
static uint8_t UdsProState = 0;
static uint8_t UdsVehicleTypeNum = 0;
static UdsReqCmd_t UdsCmdIndexNum = UDS_HEART;
static uint32_t UdsCanIdDoneFlag[sizeof(UdsCanIdList)/32+1] = {0}; /*32bit 可以表示32个CANID的处理状态*/
static UdsStep_t UdsSampStep = STEP_INIT;
static UdsResponseState_t UdsCmdResponseFlag = NO_RESPONSE;  //
static uint16_t UdsSendHeartDelay = 0;
static uint8_t UdsConnectFlag = 0;
static uint8_t UdsSendCmdCnt = 0;
static uint16_t UdsReqContinueFailCnt = 0;
static uint16_t UdsEngineSpeed = 0;
static uint16_t ISO15031EngineSpeed = 0;
static uint8_t ObdSpeedContinueFailCnt = 0;
static uint8_t FromS1ToS2ValidCnt = 0;
static uint8_t FromS2ToS3ValidCnt = 0;
static uint8_t FromS3ToS4ValidCnt = 0;
																				
static uint8_t FromS1ToS2TotalCnt = 0;
static uint8_t FromS2ToS3TotalCnt = 0;
static uint8_t FromS3ToS4TotalCnt = 0;
																				
static uint8_t UdsReMatchCnt = 0;

static uint8_t UdsEnFlag = 0; //0,使能；1禁止
static uint8_t Three_Stage_Match=0;
static uint16_t UdsEnDealy = 0;
static uint32_t UdsCmdSendState = 0; /*命令发送状态，0：允许发送，1：禁止发送*/

/************************************************************************************/

static void UdsCanIdDoneFlagClear(void)
{
	for(uint8_t i=0;i<(sizeof(UdsCanIdList)/sizeof(uint32_t));i++)
	{
		UdsCanIdDoneFlag[i] = 0;
	}
}

static void UdsCanIdDoneFlagMark(uint32_t CanId)
{
	for(uint8_t i=0;i<(sizeof(UdsCanIdList)/sizeof(uint32_t));i++)
	{
		if(CanId==UdsCanIdList[i])
		{
			UdsCanIdDoneFlag[i>>5] |= (1<<(i&0x1F));
			break;
		}
	}
}

static uint8_t IsUdsCanIdDoneFlagMark(uint32_t CanId)
{
	uint8_t Res = 1;
	for(uint8_t i=0;i<(sizeof(UdsCanIdList)/sizeof(uint32_t));i++)
	{
		if(CanId==UdsCanIdList[i])
		{
			if(!(UdsCanIdDoneFlag[i>>5] & (1<<(i&0x1F))))
			{
				Res = 0;
			}
			break;
		}
	}
	return Res;
}

static void UdsSendReqCmd(uint8_t VehicleTypeNum, uint8_t CmdIndexNum)
{
	CanTxMessage TxMsg;
	memset(&TxMsg,0,sizeof(CanTxMessage));
	memcpy(TxMsg.Data,UdsReqList[VehicleTypeNum].ReqList[CmdIndexNum],sizeof(UdsReqList[VehicleTypeNum].ReqList[CmdIndexNum]));
	TxMsg.DLC = 8;
		if(	(UdsVehicleTypeNum==DaTong_9)||(UdsVehicleTypeNum==wushiling_17) ||(UdsVehicleTypeNum==JiangLing_19))
		{
			TxMsg.IDE=CAN_ID_STD;
			TxMsg.StdId= UdsReqList[VehicleTypeNum].UdsReqCanId;
		}
		else
		{
		TxMsg.IDE = CAN_ID_EXT;
			TxMsg.ExtId = UdsReqList[VehicleTypeNum].UdsReqCanId;
		}
			
	TxMsg.RTR =0;
	CAN2SendMsg(&TxMsg);
	UdsCmdResponseFlag = NO_RESPONSE;
	UdsSendHeartDelay = 0;
	UdsSendCmdCnt += 1;
}

static void UdsSendHeartWithoutResponse(uint8_t VehicleTypeNum)
{
	CanTxMessage TxMsg;
	memset(&TxMsg,0,sizeof(CanTxMessage));
	TxMsg.Data[0] = 0x02;
	TxMsg.Data[1] = 0x3E;
	TxMsg.Data[2] = 0x80;
	if(UdsVehicleTypeNum==wushiling_17) 
	{
		TxMsg.Data[0] = 0x01;
		TxMsg.Data[1] = 0x3E;
		TxMsg.Data[2] = 0x00;
	}
	
	TxMsg.DLC = 8;
	TxMsg.RTR =0;
	if(	(UdsVehicleTypeNum==DaTong_9)||(UdsVehicleTypeNum==wushiling_17)||(UdsVehicleTypeNum==JiangLing_19) )
	{
		TxMsg.IDE=CAN_ID_STD;
		TxMsg.StdId= UdsReqList[VehicleTypeNum].UdsReqCanId;
	}
	else
	{
	TxMsg.IDE = CAN_ID_EXT;
		TxMsg.ExtId = UdsReqList[VehicleTypeNum].UdsReqCanId;
	}
	CAN2SendMsg(&TxMsg);
}

static void StartMatchNextPro(void)
{
	uint8_t UdsVehicleTypeCnt = 0;
	do{
	  UdsVehicleTypeNum += 1;
		if(UdsVehicleTypeNum>=sizeof(UdsReqList)/sizeof(UdsList_t))
		{
			UdsVehicleTypeNum = 0;
			UdsCanIdDoneFlagClear();
			
			UdsProMatchCnt += 1;
			if(UdsProMatchCnt>5) /*最多匹配6遍协议*/
			{
				UdsEnFlag = 1; /*UDS禁止匹配*/
				break;
			}
		}
		if(!IsUdsCanIdDoneFlagMark(UdsReqList[UdsVehicleTypeNum].UdsReqCanId))
		{
			break;
		}
		UdsVehicleTypeCnt += 1;
	}while(UdsVehicleTypeCnt<(sizeof(UdsReqList)/sizeof(UdsList_t)));
	UdsReMatchCnt = 0;
	UdsSampStep = STEP_INIT;
}

static void GoToNextStep(UdsReqCmd_t CmdNum, UdsStep_t Step)
{
	UdsCmdIndexNum = CmdNum;
	UdsSampStep = Step;
}

static uint8_t CountSpeedMatchRes(uint8_t ValidCnt, uint8_t TotalCnt)
{
	uint8_t MatchRes = 0;
	uint16_t Percentage = 0;
	if(TotalCnt!=0)
	{
	  Percentage = ValidCnt*100/TotalCnt;
	}
	if(Percentage>=80) //80%以上数据
	{
		MatchRes = 1;
	}
	else if(Percentage>=50)
	{
		MatchRes = 2;
	}
	else
	{
		MatchRes = 3;
	}
	return MatchRes;
}

static void UdsSpeedCompare(void)
{
	uint16_t SpeedDifference = 0;
	
	if(ISO15031EngineSpeed>UdsEngineSpeed)
	{
		SpeedDifference = ISO15031EngineSpeed - UdsEngineSpeed;
	}
	else
	{
		SpeedDifference = UdsEngineSpeed - ISO15031EngineSpeed;
	}
	if((ISO15031EngineSpeed>0)&&(ISO15031EngineSpeed<MATCH_SPEED_S2))
	{
		if((Three_Stage_Match)&&(FromS1ToS2TotalCnt<MATCH_SPEED_CNT) )
		{
			FromS1ToS2TotalCnt += 1;		
			if(((SpeedDifference*100/ISO15031EngineSpeed)<10)) /*两个速度误差少于10% */
			{
				FromS1ToS2ValidCnt += 1;
			}
		}
		else if(Three_Stage_Match==0)
		{
			FromS1ToS2TotalCnt += 1;		
			if(((SpeedDifference*100/ISO15031EngineSpeed)<10)) /*两个速度误差少于10% */
			{
				FromS1ToS2ValidCnt += 1;
			}			
		}
	}
	else if((ISO15031EngineSpeed>=MATCH_SPEED_S2)&&(ISO15031EngineSpeed<MATCH_SPEED_S3))
	{
		
		if((Three_Stage_Match)&&(FromS2ToS3TotalCnt<MATCH_SPEED_CNT) )
		{		
			FromS2ToS3TotalCnt += 1;
			if(((SpeedDifference*100/ISO15031EngineSpeed)<10)) /*两个速度误差少于10% */
			{
				FromS2ToS3ValidCnt += 1;
			}
		}
			else if(Three_Stage_Match==0)
			{
				FromS2ToS3TotalCnt += 1;
				if(((SpeedDifference*100/ISO15031EngineSpeed)<10)) /*两个速度误差少于10% */
				{
					FromS2ToS3ValidCnt += 1;
				}				
			}
	
	}
	else if(ISO15031EngineSpeed>=MATCH_SPEED_S3)
	{
		if((Three_Stage_Match)&&(FromS3ToS4TotalCnt<MATCH_SPEED_CNT) )
		{		
			FromS3ToS4TotalCnt += 1;
			if(((SpeedDifference*100/ISO15031EngineSpeed)<10)) /*两个速度误差少于10% */
			{
				FromS3ToS4ValidCnt += 1;
			}		
		}
		else if(Three_Stage_Match==0)
		{
			FromS3ToS4TotalCnt += 1;
			if(((SpeedDifference*100/ISO15031EngineSpeed)<10)) /*两个速度误差少于10% */
			{
				FromS3ToS4ValidCnt += 1;
			}			
		}	
	}
	
	uint16_t MatchValidTotalCnt = FromS1ToS2ValidCnt+FromS2ToS3ValidCnt+FromS3ToS4ValidCnt;
	uint16_t MatchTotalCnt = FromS1ToS2TotalCnt+FromS2ToS3TotalCnt+FromS3ToS4TotalCnt;
	if(MatchTotalCnt>=MatchValidTotalCnt)
	{
	//	if(!((MatchTotalCnt>=10)&&(MatchValidTotalCnt>=5))) /*前期10次需要至少5次成功*/
				if( (MatchTotalCnt>=10)&&(MatchValidTotalCnt<5) ) 
		{
			StartMatchNextPro();
		}
	}
	else
	{
		FromS1ToS2TotalCnt = 0;
		FromS2ToS3TotalCnt = 0;
		FromS3ToS4TotalCnt = 0;
		FromS1ToS2ValidCnt = 0;
		FromS2ToS3ValidCnt = 0;
		FromS3ToS4ValidCnt = 0;
	}
	
	if(Three_Stage_Match==1)
	{
		/*匹配三段速度*/
		if((FromS1ToS2TotalCnt>=MATCH_SPEED_CNT)&&(FromS2ToS3TotalCnt>=MATCH_SPEED_CNT)&&(FromS3ToS4TotalCnt>=MATCH_SPEED_CNT))
		{
			uint8_t FromS1ToS2Compare = CountSpeedMatchRes(FromS1ToS2ValidCnt,FromS1ToS2TotalCnt);
			uint8_t FromS2ToS3Compare = CountSpeedMatchRes(FromS2ToS3ValidCnt,FromS2ToS3TotalCnt);
			uint8_t FromS3ToS4Compare = CountSpeedMatchRes(FromS3ToS4ValidCnt,FromS3ToS4TotalCnt);
			if((FromS1ToS2Compare!=0)&&(FromS2ToS3Compare!=0)&&(FromS3ToS4Compare!=0))
			{
				if(FromS1ToS2Compare+FromS2ToS3Compare+FromS3ToS4Compare==3)
				{
					/*匹配成功*/
					UdsProState = 1;
					UdsSendCmdCnt = 0;
					GoToNextStep(UDS_SCR_FRONT_TEMP, STEP_UDS_AFTERPROCESS_REQUEST);
				}
				else if((FromS1ToS2Compare+FromS2ToS3Compare+FromS3ToS4Compare>3)&&(FromS1ToS2Compare+FromS2ToS3Compare+FromS3ToS4Compare<7))
				{
					/*重新匹配*/
					UdsReMatchCnt += 1;
					if(UdsReMatchCnt<UDS_REPEAT_MATCH_CNT)
					{
						UdsSampStep = STEP_INIT;
					}
					else
					{
						StartMatchNextPro();
					}
				}
				else
				{
					/*匹配失败*/
					StartMatchNextPro();
				}
			}
		}
		else
		{
			UdsSendCmdCnt = 0;
			UdsEnDealy = UDS_MATCH_DEALY;
			GoToNextStep(UDS_SPEED,STEP_UDS_SPEED_REQUEST);
		}
	}
	else
	{
		/*一段匹配*/
		if((FromS1ToS2TotalCnt+FromS2ToS3TotalCnt+FromS3ToS4TotalCnt)>=(MATCH_SPEED_CNT*3))
		{
			uint8_t CompareRes;
			CompareRes = CountSpeedMatchRes(FromS1ToS2ValidCnt+FromS2ToS3ValidCnt+FromS3ToS4ValidCnt,FromS1ToS2TotalCnt+FromS2ToS3TotalCnt+FromS3ToS4TotalCnt);
			if(CompareRes==1)
			{
				/*匹配成功*/
				UdsProState = 1;
				UdsSendCmdCnt = 0;
				GoToNextStep(UDS_SCR_FRONT_TEMP, STEP_UDS_AFTERPROCESS_REQUEST);
			}
			else if(CompareRes==2)
			{
				/*重新匹配*/
				UdsReMatchCnt += 1;
				if(UdsReMatchCnt<UDS_REPEAT_MATCH_CNT)
				{
					UdsSampStep = STEP_INIT;
				}
				else
				{
					StartMatchNextPro();
				}
			}
			else
			{
				/*匹配失败*/
				StartMatchNextPro();
			}
		}
		else
		{
			UdsSendCmdCnt = 0;
			UdsEnDealy = UDS_MATCH_DEALY;
			GoToNextStep(UDS_SPEED,STEP_UDS_SPEED_REQUEST);
		}
	}
}

void UdsReqServer(ProtocolNum_t Procotol, uint8_t DealyFlag)
{
	if((UdsCmdSendState&(1<<((uint8_t)UDS_SCR_FRONT_TEMP)))&&(UdsCmdSendState&(1<<((uint8_t)UDS_SCR_BACK_TEMP))) && \
		(UdsCmdSendState&(1<<((uint8_t)UDS_NOX_CONTEST)))&&(UdsCmdSendState&(1<<((uint8_t)UDS_UREA_LIQUID_LEVEL)))) /*数据都已经采集到了，关闭UDS*/
	{
		UdsSampStep = STEP_INIT;
		UdsEnFlag = 1;
	}
	
	if(UdsVehicleTypeNum==DaTong_9)
		{
			if(UdsCmdSendState&(1<<((uint8_t)UDS_COOLTEMP)))
			{
				UdsSampStep = STEP_INIT;
				UdsEnFlag = 1;				
			}
			else
				UdsEnFlag = 0;	
		}	
		
	if(!UdsEnFlag)
	{
		/*维持心跳*/
		if((DealyFlag==25)&&(UdsConnectFlag==1)) /*DealyFlag的值是0-99之间的值减去50*/
		{
			UdsSendHeartDelay += 1;
			if(UdsSendHeartDelay>=2) /*300ms*2=600ms*/
			{
				UdsSendHeartWithoutResponse(UdsVehicleTypeNum);
				UdsSendHeartDelay = 0;
			}
		}
		
		switch(UdsSampStep)
		{
			case STEP_INIT: /*初始化*/
			{
				if(UdsProState==0)
				{
					 /*匹配协议*/
					FromS1ToS2ValidCnt = 0;
					FromS2ToS3ValidCnt = 0;
					FromS3ToS4ValidCnt = 0;
					FromS1ToS2TotalCnt = 0;
					FromS2ToS3TotalCnt = 0;
					FromS3ToS4TotalCnt = 0;
					UdsSendHeartDelay = 0;
					UdsSendCmdCnt = 0;
					UdsConnectFlag = 0;
					ObdSpeedContinueFailCnt = 0;
					UdsCmdIndexNum = UDS_HEART;
					UdsSampStep = STEP_HEART_REQUEST;
				}
				else
				{
					/*UDS采集*/
					if(UdsEnDealy<=0)
					{
						UdsSendHeartDelay = 0;
						UdsSendCmdCnt = 0;
						UdsConnectFlag = 0;
						ObdSpeedContinueFailCnt = 0;
						UdsCmdIndexNum = UDS_HEART;
						UdsSampStep = STEP_HEART_REQUEST;
					}
					else
					{
						if(DealyFlag==0)
						{
							UdsEnDealy -= 1;
						}
					}
				}
			}break;
			
			case STEP_HEART_REQUEST: /*请求心跳*/
			{
				if(UdsReqList[UdsVehicleTypeNum].FunctionPro[UdsCmdIndexNum]!=NULL)
				{
					if(DealyFlag==0)
					{
						UdsSendReqCmd(UdsVehicleTypeNum, UdsCmdIndexNum);
						UdsSampStep = STEP_HEART_RESPONSE;
					}
				}
				else
				{
					UdsSendCmdCnt = 0;
					GoToNextStep(UDS_LINK,STEP_LINK_REQUEST);
				}
			}break;
			
			case STEP_HEART_RESPONSE: /*等待心跳回复*/
			{
				if(UdsCmdResponseFlag == POSITIVE_RESPONSE)  /*积极回复*/
				{
					UdsSendCmdCnt = 0;
					GoToNextStep(UDS_LINK,STEP_LINK_REQUEST);
				}
				else 
				{
					if((DealyFlag==0)||(UdsCmdResponseFlag == NEGATIVE_RESPONSE)) /*超时没有回复或者消极回复*/
					{
						uint16_t ResendCnt = 0;
						if(UdsProState==0)
						{
							ResendCnt = SEND_CMD_MAX_CNT; //匹配时，超时或消极回复的次数
						}
						else
						{
							ResendCnt = SEND_CMD_MAX_CNT<<5; //采集时，超时或消极的回复次数
						}
						if(UdsSendCmdCnt < ResendCnt)
						{
							GoToNextStep(UDS_HEART,STEP_HEART_REQUEST); //重发
						}
						else
						{
							UdsProState = 0;
							UdsCanIdDoneFlagMark(UdsReqList[UdsVehicleTypeNum].UdsReqCanId);  /*心跳不通过时，剔除该CANID的协议*/
							StartMatchNextPro();
						}
					}
				}
			}break;
			
			case STEP_LINK_REQUEST: /*请求会话*/
			{
				if((UdsReqList[UdsVehicleTypeNum].FunctionPro[UdsCmdIndexNum]!=NULL)&&(UdsConnectFlag==0))
				{
					if(DealyFlag==0)
					{
						UdsSendReqCmd(UdsVehicleTypeNum, UdsCmdIndexNum);
						UdsSampStep = STEP_LINK_RESPONSE;
					}
				}
				else
				{
					UdsSendCmdCnt = 0;
					GoToNextStep(UDS_SECURITY_ACCESS_SEED,STEP_SECURITY_ACCESS_SEED_REQUEST);
				}
			}break;
			
			case STEP_LINK_RESPONSE: /*等待会话回复*/
			{
				if(UdsCmdResponseFlag == POSITIVE_RESPONSE)
				{
					UdsConnectFlag = 1;
					UdsSendCmdCnt = 0;
					GoToNextStep(UDS_SECURITY_ACCESS_SEED,STEP_SECURITY_ACCESS_SEED_REQUEST);
				}
				else
				{
					if((DealyFlag==0)||(UdsCmdResponseFlag == NEGATIVE_RESPONSE)) /*超时没有回复或者消极回复*/
					{
						uint16_t ResendCnt = 0;
						if(UdsProState==0)
						{
							ResendCnt = SEND_CMD_MAX_CNT; //匹配时，超时或消极回复的次数
						}
						else
						{
							ResendCnt = SEND_CMD_MAX_CNT<<5; //采集时，超时或消极的回复次数
						}
						if(UdsSendCmdCnt < ResendCnt)
						{
							GoToNextStep(UDS_LINK,STEP_LINK_REQUEST);
						}
						else
						{
							UdsProState = 0;
							StartMatchNextPro();
						}
					}
				}
			}break;
			
			case STEP_SECURITY_ACCESS_SEED_REQUEST:
			{
				if((UdsReqList[UdsVehicleTypeNum].FunctionPro[UdsCmdIndexNum]!=NULL))
				{
					if(DealyFlag==0)
					{
						UdsSendReqCmd(UdsVehicleTypeNum, UdsCmdIndexNum);
						UdsSampStep = STEP_SECURITY_ACCESS_SEED_RESPONSE;
					}
				}
				else
				{
					if(UdsProState==0)
					{
						/*协议匹配，开始匹配速度*/
						UdsSendCmdCnt = 0;
						UdsEnDealy = 0;
						GoToNextStep(UDS_SPEED,STEP_UDS_SPEED_REQUEST);
					}
					else
					{
						/*采集UDS数据*/
						UdsSendCmdCnt = 0;
						GoToNextStep(UDS_SCR_FRONT_TEMP,STEP_UDS_AFTERPROCESS_REQUEST);
					}
				}
			}break;
			
			case STEP_SECURITY_ACCESS_SEED_RESPONSE:
			{
				if(UdsCmdResponseFlag == POSITIVE_RESPONSE)
				{
					UdsSendCmdCnt = 0;
					GoToNextStep(UDS_SECURITY_ACCESS_RES,STEP_SECURITY_ACCESS_RES_REQUEST);
				}
				else
				{
					if((DealyFlag==0)||(UdsCmdResponseFlag == NEGATIVE_RESPONSE)) /*超时没有回复或者消极回复*/
					{
						uint16_t ResendCnt = 0;
						if(UdsProState==0)
						{
							ResendCnt = SEND_CMD_MAX_CNT; //匹配时，超时或消极回复的次数
						}
						else
						{
							ResendCnt = SEND_CMD_MAX_CNT<<5; //采集时，超时或消极的回复次数
						}
						if(UdsSendCmdCnt < ResendCnt)
						{
							GoToNextStep(UDS_SECURITY_ACCESS_SEED,STEP_SECURITY_ACCESS_SEED_REQUEST);
						}
						else
						{
							UdsProState = 0;
							StartMatchNextPro();
						}
					}
				}
			}break;
			
			case STEP_SECURITY_ACCESS_RES_REQUEST:
			{
				if((UdsReqList[UdsVehicleTypeNum].FunctionPro[UdsCmdIndexNum]!=NULL))
				{
					if(DealyFlag==0)
					{
						UdsSendReqCmd(UdsVehicleTypeNum, UdsCmdIndexNum);
						UdsSampStep = STEP_SECURITY_ACCESS_RES_RESPONSE;
					}
				}
				else
				{
					if(UdsProState==0)
					{
						/*协议匹配，开始匹配速度*/
						UdsSendCmdCnt = 0;
						UdsEnDealy = 0;
						GoToNextStep(UDS_SPEED,STEP_UDS_SPEED_REQUEST);
					}
					else
					{
						/*采集UDS数据*/
						UdsSendCmdCnt = 0;
						GoToNextStep(UDS_SCR_FRONT_TEMP,STEP_UDS_AFTERPROCESS_REQUEST);
					}
				}
			}break;
			
			case STEP_SECURITY_ACCESS_RES_RESPONSE:
			{
				if(UdsCmdResponseFlag == POSITIVE_RESPONSE)
				{
					UdsSendCmdCnt = 0;
					if(UdsProState==0)
					{
						/*协议匹配，开始匹配速度*/
						UdsEnDealy = 0;
						GoToNextStep(UDS_SPEED,STEP_UDS_SPEED_REQUEST);
					}
					else
					{
						/*采集UDS数据*/
						GoToNextStep(UDS_SCR_FRONT_TEMP,STEP_UDS_AFTERPROCESS_REQUEST);
					}
				}
				else
				{
					if((DealyFlag==0)||(UdsCmdResponseFlag == NEGATIVE_RESPONSE)) /*超时没有回复或者消极回复*/
					{
						uint16_t ResendCnt = 0;
						if(UdsProState==0)
						{
							ResendCnt = SEND_CMD_MAX_CNT; //匹配时，超时或消极回复的次数
						}
						else
						{
							ResendCnt = SEND_CMD_MAX_CNT<<5; //采集时，超时或消极的回复次数
						}
						if(UdsSendCmdCnt < ResendCnt)
						{
							GoToNextStep(UDS_SECURITY_ACCESS_RES,STEP_SECURITY_ACCESS_RES_REQUEST);
						}
						else
						{
							UdsProState = 0;
							StartMatchNextPro();
						}
					}
				}
			}break;
			
			case STEP_UDS_SPEED_REQUEST: /*请求UDS转速*/
			{
				if(DealyFlag==0)
				{
					if(UdsEnDealy==0)
					{
						UdsSendReqCmd(UdsVehicleTypeNum, UdsCmdIndexNum);
						UdsSampStep = STEP_UDS_SPEED_RESPONSE;
					}
					else
					{
						UdsEnDealy -= 1;
					}
				}
			}break;
			
			case STEP_UDS_SPEED_RESPONSE: /*等待UDS转速回复*/
			{
				if(UdsCmdResponseFlag == POSITIVE_RESPONSE)
				{
					UdsSendCmdCnt = 0;
					UdsSampStep = STEP_OBD_SPEED_REQUEST;
				}
				else
				{
					if((DealyFlag==0)||(UdsCmdResponseFlag == NEGATIVE_RESPONSE)) /*超时没有回复或者消极回复*/
					{
						if(UdsSendCmdCnt < SEND_CMD_MAX_CNT)
						{
							UdsEnDealy = 0;
							GoToNextStep(UDS_SPEED,STEP_UDS_SPEED_REQUEST);
						}
						else
						{
							StartMatchNextPro();
						}
					}
				}
			}break;
			
			case STEP_OBD_SPEED_REQUEST: /*请求OBD转速*/
			{
				if(DealyFlag==0)
				{
					CanTxMessage TxMsg;
					memset(&TxMsg,0,sizeof(CanTxMessage));
					memcpy(TxMsg.Data,ISO15031ReqSpeed,sizeof(ISO15031ReqSpeed));
					TxMsg.DLC = 8;
					TxMsg.IDE = CAN_ID_EXT;
					TxMsg.ExtId = 0x18DB33F1;
					TxMsg.RTR =0;
					CAN2SendMsg(&TxMsg);
					UdsCmdResponseFlag = NO_RESPONSE;
					UdsSampStep = STEP_OBD_SPEED_RESPONSE;
				}
			}break;
			
			case STEP_OBD_SPEED_RESPONSE: /*等待OBD转速回复*/
			{
				if(UdsCmdResponseFlag == POSITIVE_RESPONSE)
				{
					ObdSpeedContinueFailCnt = 0;
					UdsSpeedCompare();
				}
				else
				{
					if((DealyFlag==0)||(UdsCmdResponseFlag == NEGATIVE_RESPONSE))
					{
						ObdSpeedContinueFailCnt += 1;
						if(ObdSpeedContinueFailCnt<OBD_SPEED_FAIL_CNT)
						{
							UdsSendCmdCnt = 0;
							UdsEnDealy = 0;
							GoToNextStep(UDS_SPEED,STEP_UDS_SPEED_REQUEST);
						}
						else
						{
							StartMatchNextPro();
						}
					}
				}
			}break;
			
			case STEP_UDS_AFTERPROCESS_REQUEST: /*请求后处理数据*/
			{
        if(DealyFlag==0)
				{
					do
					{
						if((UdsReqList[UdsVehicleTypeNum].FunctionPro[UdsCmdIndexNum]!=NULL)&&(!(UdsCmdSendState&(1<<UdsCmdIndexNum))))
						{
							if((flag_50ling_send==1)||(UdsVehicleTypeNum!=wushiling_17))
								UdsSendReqCmd(UdsVehicleTypeNum, UdsCmdIndexNum);
							UdsCmdResponseFlag = NO_RESPONSE;
  						UdsSampStep = STEP_UDS_AFTERPROCESS_RESPONSE;
							break;
						}
						else
						{
							UdsSendCmdCnt = 0;
					    UdsCmdIndexNum += 1;
							if(((uint8_t)UdsCmdIndexNum)>=(sizeof(UdsReqList[0].ReqList)/sizeof(UdsReqList[0].ReqList[0])))
							{
								GoToNextStep(UDS_HEART,STEP_INIT);
								UdsEnDealy = UDS_ENABLE_DEALY;
								break;
							}
						}
					}while(1);
			  }
			}break;
			
			case STEP_UDS_AFTERPROCESS_RESPONSE: /*等待后处理数据回复*/
			{
				if(UdsCmdResponseFlag == POSITIVE_RESPONSE) /*积极回复*/
				{
					UdsReqContinueFailCnt = 0;
					UdsSendCmdCnt = 0;
					UdsCmdIndexNum += 1;
					if(((uint8_t)UdsCmdIndexNum)<(sizeof(UdsReqList[0].ReqList)/sizeof(UdsReqList[0].ReqList[0])))
					{
						GoToNextStep(UdsCmdIndexNum,STEP_UDS_AFTERPROCESS_REQUEST);
					}
					else
					{
						GoToNextStep(UDS_HEART,STEP_INIT);
						UdsEnDealy = UDS_ENABLE_DEALY;
					}
				}
				else
				{
					if((DealyFlag==0)||(UdsCmdResponseFlag == NEGATIVE_RESPONSE))
					{
						UdsReqContinueFailCnt += 1;
						if(UdsReqContinueFailCnt<1000) /*连续50次失败，4个参数重发5遍*/
						{
							if(UdsSendCmdCnt<SEND_CMD_MAX_CNT)
							{
								/*重新请求*/
								GoToNextStep(UdsCmdIndexNum,STEP_UDS_AFTERPROCESS_REQUEST);
							}
							else
							{
								/*请求下一个*/
								UdsSendCmdCnt = 0;
								UdsCmdIndexNum += 1;
								if(((uint8_t)UdsCmdIndexNum)<(sizeof(UdsReqList[0].ReqList)/sizeof(UdsReqList[0].ReqList[0])))
								{
									GoToNextStep(UdsCmdIndexNum,STEP_UDS_AFTERPROCESS_REQUEST);
								}
								else
								{
									GoToNextStep(UDS_HEART,STEP_INIT);
									UdsEnDealy = UDS_ENABLE_DEALY;
								}
							}
						}
						else
						{
							UdsReqContinueFailCnt = 0;
							UdsProState = 0;
							StartMatchNextPro();
						}
					}
				}
			}break;
			
	//		case STEP_UDS_SCR_FRONT_TEMP_REQUEST:
	//		{
	//			if(SendFlag==0)
	//			{
	//				UdsSendReqCmd(UdsVehicleTypeNum, UdsCmdIndexNum);
	//				UdsProtocolMatchStep = STEP_UDS_SCR_FRONT_TEMP_RESPONSE;
	//			}
	//		}break;
	//		
	//		case STEP_UDS_SCR_FRONT_TEMP_RESPONSE:
	//		{
	//			if(UdsCmdResponseFlag == POSITIVE_RESPONSE)
	//			{
	//				UdsSendCmdCnt = 0;
	//				GoToNextStep(4,STEP_UDS_SCR_BACK_TEMP_REQUEST);
	//			}
	//			else
	//			{
	//				if(SendFlag==0)
	//				{
	//					if(UdsSendCmdCnt < SEND_HEART_MAX_CNT)
	//					{
	//						GoToNextStep(3,STEP_UDS_SCR_FRONT_TEMP_REQUEST);
	//					}
	//				}
	//				else
	//				{
	//					UdsSendCmdCnt = 0;
	//					GoToNextStep(4,STEP_UDS_SCR_BACK_TEMP_REQUEST);
	//				}
	//			}
	//		}
			
			default: 
			{
				UdsSampStep = STEP_INIT;
			}break;
		}
	}
}

void UdsCheckPid(CanData_t *pCanMsg, uint8_t FrameCnt)
{
	uint8_t ValidData[255] = {0};
	uint8_t ValidDataLen = 0;
	uint8_t LengthCheck;

	if(!UdsEnFlag)
	{
		if((pCanMsg->Data[0]==0x10)&&(FrameCnt>1))
		{
			ValidDataLen = ISO15765MultiFrameToData(pCanMsg, FrameCnt, ValidData);
			LengthCheck = pCanMsg->Data[1];
		}
		else
		{
			ValidDataLen = ISO15765SingleFrameToData(pCanMsg, FrameCnt, ValidData);
			if((UdsVehicleTypeNum==wushiling_17)&&(pCanMsg->Data[0]==0xFD)&&(pCanMsg->CanId==0x5EB))
				ValidDataLen =2;
			LengthCheck = pCanMsg->Data[0];
		}
		
		if( (ValidDataLen==LengthCheck)||(UdsVehicleTypeNum==wushiling_17) )  //五十铃回复不符合16765格式，不校验第一个字节的数据长度
		{
			if((pCanMsg->CanId==UdsReqList[UdsVehicleTypeNum].UdsResCanId)||((UdsVehicleTypeNum==wushiling_17)&&((pCanMsg->CanId==0x5EB)||(pCanMsg->CanId==0x2B0)) )	)
			{
				switch(UdsSampStep)
				{
					case STEP_HEART_RESPONSE:
					case STEP_LINK_RESPONSE:
					case STEP_SECURITY_ACCESS_SEED_RESPONSE:
					case STEP_SECURITY_ACCESS_RES_RESPONSE:
					case STEP_UDS_SPEED_RESPONSE:
					case STEP_UDS_AFTERPROCESS_RESPONSE:
					{
						if(ValidData[0]==0x7F) /*消极回复*/
						{
							if(ValidData[1]==UdsReqList[UdsVehicleTypeNum].ReqList[UdsCmdIndexNum][1]) /*需不需要把判断范围缩小？hxy20190705*/
							{
								UdsCmdResponseFlag = NEGATIVE_RESPONSE;
							}
						}
						else
						{
							if( ((ValidData[0]-0x40)==UdsReqList[UdsVehicleTypeNum].ReqList[UdsCmdIndexNum][1]) || (UdsVehicleTypeNum==wushiling_17))  /*积极回复*/
							{
								if(UdsReqList[UdsVehicleTypeNum].FunctionPro[UdsCmdIndexNum]!=NULL)
								{
									if((UdsVehicleTypeNum==wushiling_17)&&((pCanMsg->CanId==0x5EB)||(pCanMsg->CanId==0x2B0)))
									{
											ValidData[0]=pCanMsg->Data[0];
											ValidData[2]=pCanMsg->Data[2];
											if(pCanMsg->CanId==0x2B0)
											{
											ValidData[0]=0xFD;
											ValidData[2]=pCanMsg->Data[0];
											}
											flag_50ling_send=0;
									}
									UdsReqList[UdsVehicleTypeNum].FunctionPro[UdsCmdIndexNum](ValidData,ValidDataLen);
								}
							}
						
						}
					}break;
					
					case STEP_OBD_SPEED_RESPONSE: /*UDS的回复CANID和15031的回复CANID一样*/
					{
						if(((ValidData[0]-0x40)==ISO15031ReqSpeed[1])&&(ValidData[1]==ISO15031ReqSpeed[2]))
						{
							UdsCmdResponseFlag = POSITIVE_RESPONSE;
							ISO15031EngineSpeed = ((uint16_t)(ValidData[2]<<8)+(uint16_t)ValidData[3])>>2;
						}
					}break;
					
					default: break;
				}
			}
			else
			{
				if((UdsSampStep==STEP_OBD_SPEED_RESPONSE)&&((ValidData[0]-0x40)==ISO15031ReqSpeed[1])&&(ValidData[1]==ISO15031ReqSpeed[2])) /*15031 发动机转速*/
				{
					UdsCmdResponseFlag = POSITIVE_RESPONSE;
					ISO15031EngineSpeed = ((uint16_t)(ValidData[2]<<8)+(uint16_t)ValidData[3])>>2;
				}
			}
		}
	}
}

void UdsCmdDisSend(UdsReqCmd_t cmd)
{
	UdsCmdSendState |= (1<<((uint8_t)cmd));
}

void UdsCmdEnSend(UdsReqCmd_t cmd)
{
	UdsCmdSendState &= (~(1<<((uint8_t)cmd)));
}

//static void UdsCheckMsg(void)
//{
//	CanData_t *pCanMsg;
//	uint8_t FrameCnt;
//	do
//	{
//		pCanMsg = GetCanData(UDS, &FrameCnt);
//		if(pCanMsg==NULL)
//		{
//			break;
//		}

//		if(((UdsSampStep > STEP_INIT))&&(pCanMsg->Procotol == ISO15765))
//		{
//			UdsCheckPid(pCanMsg,FrameCnt);
//		}
//	}while(1);
//}

uint8_t SetUdsProtocol(UdsProtocol_t ProtocolNum)
{
	uint8_t res = 1;
	for(uint8_t i=0;i<(sizeof(UdsReqList)/sizeof(UdsList_t));i++)
	{
		if(UdsReqList[i].UdsProtocolNum == ProtocolNum)
		{
			UdsVehicleTypeNum = i;
			UdsReqContinueFailCnt = 0;
			UdsProState = 1;
			UdsSendCmdCnt = 0;
			GoToNextStep(UDS_HEART,STEP_INIT);
			res = 0;
			break;
		}
	}
	return res;
}

void SetUdsEnFlag(u8 udsEnFlag)
{
	if(udsEnFlag==1)
		UdsEnFlag=udsEnFlag;			//0 使能  1停止
	else
		UdsEnFlag=0;
}

void SetUdsMode(u8 udsmode)
{
	if(udsmode==0)
		Three_Stage_Match=1;			//1 三段匹配    0  一段匹配
	else
		Three_Stage_Match=0;
}
void UdsMatchTypeNum(u8 udsnum)
{
	if(udsnum>=sizeof(UdsReqList)/sizeof(UdsList_t))
		UdsVehicleTypeNum=0;
	else
	UdsVehicleTypeNum=udsnum;
}

//void UdsProcess(ProtocolNum_t Procotol, uint8_t DealyFlag)
//{
//	UdsCheckMsg();
//	UdsReqServer(Procotol, DealyFlag);
//}

/************************************************************/
/*以下是各个车型的UDS解析函数*/
/************************************************************/
static void UdsUpdateData(uint16_t data)
{
	switch(UdsCmdIndexNum)
	{
		case UDS_SPEED:
		{
			UdsEngineSpeed = data;
		}break;
		case UDS_SCR_FRONT_TEMP:
		{
			OBD_aftertreatment_infor.inLetTemp = (data+273)<<5; /* (data+273)*32 */
		}break;
		case UDS_SCR_BACK_TEMP:
		{
				if(UdsVehicleTypeNum==DaTong_9)
					OBD_engine_infor.coolantTemp=	data+40;
				else
					OBD_aftertreatment_infor.outLetTemp = (data+273)<<5;/* (data+273)*32 */
		
		
		}break;
		case UDS_NOX_CONTEST:
		{
					if(UdsVehicleTypeNum==DaTong_9)
					OBD_engine_infor.enginefuelFlow=	data*(OBD_engine_infor.engineRev/8)*6/80000;
							else
					OBD_aftertreatment_infor.dnstSensValue = ((data+200)<<4)+((data+200)<<2); /* (data+200)*20 */
		}break;
		case UDS_UREA_LIQUID_LEVEL:
		{
						if(UdsVehicleTypeNum==DaTong_9)
								OBD_engine_infor.engineTorq=	data;
						else
						{
								if(UdsVehicleTypeNum==JiangLing_19)
										{
											if(data >250 )
												 data = 100;
											else
												data = (data<<1)/5;//data单位mm ，尿素罐250mm高
										}
									OBD_aftertreatment_infor.reagAllowance = (data<<1)+(data>>1); /* data*2.5 */
						}
//						else			
//					OBD_aftertreatment_infor.reagAllowance = (data<<1)+(data>>1); /* data*2.5 */
		}break;
		default:break;
	}
}

/*心跳回复*/
static void HeartResponse(const uint8_t* pData, uint8_t DataLen)
{
  if(UdsSampStep==STEP_HEART_RESPONSE)
	{
	  UdsCmdResponseFlag = POSITIVE_RESPONSE;
	}
}

/*连接回复*/
static void UpLinkResponse(const uint8_t* pData, uint8_t DataLen)
{
	if(UdsSampStep==STEP_LINK_RESPONSE)
	{
		if(pData[1]==UdsReqList[UdsVehicleTypeNum].ReqList[UdsCmdIndexNum][2])
		{
		  UdsCmdResponseFlag = POSITIVE_RESPONSE;
		}
	}
}

static void GetUdsData_256A_B(const uint8_t* pData, uint8_t DataLen) /*解析方式 A*256+B*/
{
	if(DataLen>=5)
	{
		uint16_t fid = (uint16_t)(pData[1]<<8) + (uint16_t)pData[2];
		if(fid == ((uint16_t)(UdsReqList[UdsVehicleTypeNum].ReqList[UdsCmdIndexNum][2]<<8))+(uint16_t)(UdsReqList[UdsVehicleTypeNum].ReqList[UdsCmdIndexNum][3]))
		{
			UdsCmdResponseFlag = POSITIVE_RESPONSE;
		  uint16_t UdsData = (uint16_t)(pData[3]<<8)+(uint16_t)(pData[4]);
			UdsUpdateData(UdsData);
		}
	}
}
static void GetUdsData_256A_B_sign(const uint8_t* pData, uint8_t DataLen) /*解析方式 A*256+B*/  //最高位为符号位
{
	if(DataLen>=5)
	{
		uint16_t fid = (uint16_t)(pData[1]<<8) + (uint16_t)pData[2];
		if(fid == ((uint16_t)(UdsReqList[UdsVehicleTypeNum].ReqList[UdsCmdIndexNum][2]<<8))+(uint16_t)(UdsReqList[UdsVehicleTypeNum].ReqList[UdsCmdIndexNum][3]))
		{
			UdsCmdResponseFlag = POSITIVE_RESPONSE;
		 if(pData[3] & 0x80)
		 {
			int UdsData = (uint16_t)((pData[3]&0x7F)<<8)+(uint16_t)(pData[4]) - 32768;
			UdsUpdateData(UdsData);
		 }
	   else
		 {
			int UdsData = (uint16_t)(pData[3]<<8)+(uint16_t)(pData[4]);
			UdsUpdateData(UdsData);
		 }
		}
	}
}
static void GetUdsData_256A_B_Div2(const uint8_t* pData, uint8_t DataLen)/*解析方式 (A*256+B)/2*/
{
	if(DataLen>=5)
	{
		uint16_t fid = (uint16_t)(pData[1]<<8) + (uint16_t)pData[2];
		if(fid == ((uint16_t)(UdsReqList[UdsVehicleTypeNum].ReqList[UdsCmdIndexNum][2]<<8))+(uint16_t)(UdsReqList[UdsVehicleTypeNum].ReqList[UdsCmdIndexNum][3]))
		{
			UdsCmdResponseFlag = POSITIVE_RESPONSE;
		  uint16_t UdsData = ((uint16_t)(pData[3]<<8)+(uint16_t)(pData[4]))>>1;
			UdsUpdateData(UdsData);
		}
	}
}

static void GetUdsData_256A_B_Div4(const uint8_t* pData, uint8_t DataLen)/*解析方式 (A*256+B)/4*/
{
	if(DataLen>=5)
	{
		uint16_t fid = (uint16_t)(pData[1]<<8) + (uint16_t)pData[2];
		if(fid == ((uint16_t)(UdsReqList[UdsVehicleTypeNum].ReqList[UdsCmdIndexNum][2]<<8))+(uint16_t)(UdsReqList[UdsVehicleTypeNum].ReqList[UdsCmdIndexNum][3]))
		{
			UdsCmdResponseFlag = POSITIVE_RESPONSE;
		  uint16_t UdsData = ((uint16_t)(pData[3]<<8)+(uint16_t)(pData[4]))>>2;
			UdsUpdateData(UdsData);
		}
	}
}

//static void GetUdsData_256A_B_Div8(const uint8_t* pData, uint8_t DataLen)/*解析方式 (A*256+B)/8*/
//{
//	if(DataLen>=5)
//	{
//		uint16_t fid = (uint16_t)(pData[1]<<8) + (uint16_t)pData[2];
//		if(fid == ((uint16_t)(UdsReqList[UdsVehicleTypeNum].ReqList[UdsCmdIndexNum][2]<<8))+(uint16_t)(UdsReqList[UdsVehicleTypeNum].ReqList[UdsCmdIndexNum][3]))
//		{
//			UdsCmdResponseFlag = POSITIVE_RESPONSE;
//		  uint16_t UdsData = ((uint16_t)(pData[3]<<8)+(uint16_t)(pData[4]))>>3;
//			UdsUpdateData(UdsData);
//		}
//	}
//}

//static void GetUdsData_256A_B_Div10(const uint8_t* pData, uint8_t DataLen)/*解析方式 (A*256+B)/10*/
//{
//	if(DataLen>=5)
//	{
//		uint16_t fid = (uint16_t)(pData[1]<<8) + (uint16_t)pData[2];
//		if(fid == ((uint16_t)(UdsReqList[UdsVehicleTypeNum].ReqList[UdsCmdIndexNum][2]<<8))+(uint16_t)(UdsReqList[UdsVehicleTypeNum].ReqList[UdsCmdIndexNum][3]))
//		{
//			UdsCmdResponseFlag = POSITIVE_RESPONSE;
//		  uint16_t UdsData = (((uint16_t)(pData[3]<<8)+(uint16_t)(pData[4]))>>3)+(((uint16_t)(pData[3]<<8)+(uint16_t)(pData[4]))>>1);
//			UdsUpdateData(UdsData);
//		}
//	}
//}

//static void GetUdsData_256B_A(const uint8_t* pData, uint8_t DataLen)/*解析方式 (B*256+A)*/
//{
//	if(DataLen>=5)
//	{
//		uint16_t fid = (uint16_t)(pData[1]<<8) + (uint16_t)pData[2];
//		if(fid == ((uint16_t)(UdsReqList[UdsVehicleTypeNum].ReqList[UdsCmdIndexNum][2]<<8))+(uint16_t)(UdsReqList[UdsVehicleTypeNum].ReqList[UdsCmdIndexNum][3]))
//		{
//			UdsCmdResponseFlag = POSITIVE_RESPONSE;
//		  uint16_t UdsData = (uint16_t)(pData[4]<<8)+(uint16_t)(pData[3]);
//			UdsUpdateData(UdsData);
//		}
//	}
//}

static void GetScrTempJieFang(const uint8_t* pData, uint8_t DataLen)
{
	if(DataLen>=5)
	{
		uint16_t fid = (uint16_t)(pData[1]<<8) + (uint16_t)pData[2];
		if(fid == ((uint16_t)(UdsReqList[UdsVehicleTypeNum].ReqList[UdsCmdIndexNum][2]<<8))+(uint16_t)(UdsReqList[UdsVehicleTypeNum].ReqList[UdsCmdIndexNum][3]))
		{
			UdsCmdResponseFlag = POSITIVE_RESPONSE;
			int UdsData = 0;
		  if(pData[3]&0x80)
			{
				UdsData = (((((uint16_t)pData[3])<<8)+(uint16_t)pData[4])<<6)/1000-2097;
			}
			else
			{
				UdsData = (((((uint16_t)pData[3])<<8)+(uint16_t)pData[4])<<6)/1000;
			}
			UdsUpdateData(UdsData);
		}
	}
}

static void GetNoxContestJieFang(const uint8_t* pData, uint8_t DataLen)
{
	if(DataLen>=5)
	{
		uint16_t fid = (uint16_t)(pData[1]<<8) + (uint16_t)pData[2];
		if(fid == ((uint16_t)(UdsReqList[UdsVehicleTypeNum].ReqList[UdsCmdIndexNum][2]<<8))+(uint16_t)(UdsReqList[UdsVehicleTypeNum].ReqList[UdsCmdIndexNum][3]))
		{
			UdsCmdResponseFlag = POSITIVE_RESPONSE;
			int UdsData = 0;
		  if(pData[3]&0x80)
			{
				UdsData = ((((uint16_t)pData[3])<<8)+(uint16_t)pData[4])-32768;
			}
			else
			{
				UdsData = (((uint16_t)pData[3])<<8)+(uint16_t)pData[4];
			}
			UdsUpdateData(UdsData);
		}
	}
}

static void GetUreaLiquidLevelJieFang(const uint8_t* pData, uint8_t DataLen)
{
	if(DataLen>=5)
	{
		uint16_t fid = (uint16_t)(pData[1]<<8) + (uint16_t)pData[2];
		if(fid == ((uint16_t)(UdsReqList[UdsVehicleTypeNum].ReqList[UdsCmdIndexNum][2]<<8))+(uint16_t)(UdsReqList[UdsVehicleTypeNum].ReqList[UdsCmdIndexNum][3]))
		{
			UdsCmdResponseFlag = POSITIVE_RESPONSE;
			uint16_t UdsData = 0;
		  if(pData[3]&0x80)
			{
				UdsData = ((((uint16_t)pData[3])<<8)+(uint16_t)pData[4])-819;
			}
			else
			{
				UdsData = ((((uint16_t)pData[3])<<8)+(uint16_t)pData[4])*25/1000;
			}
			UdsUpdateData(UdsData);
		}
	}
}

static void GetScrTemp_256A_B_Div10_Sub273(const uint8_t* pData, uint8_t DataLen) /*解析方式：(256*A+B)/10-273 */
{
	if(DataLen>=5)
	{
		uint16_t fid = (uint16_t)(pData[1]<<8) + (uint16_t)pData[2];
		if(fid == ((uint16_t)(UdsReqList[UdsVehicleTypeNum].ReqList[UdsCmdIndexNum][2]<<8))+(uint16_t)(UdsReqList[UdsVehicleTypeNum].ReqList[UdsCmdIndexNum][3]))
		{
			UdsCmdResponseFlag = POSITIVE_RESPONSE;
			int UdsData = 0;
			UdsData = ((((uint16_t)pData[3])<<8)+(uint16_t)pData[4])/10-273;
			UdsUpdateData(UdsData);
		}
	}
}

static void GetUdsData_YiLiLanJie(const uint8_t* pData, uint8_t DataLen)
{
	if(DataLen>1)
	{
		if(pData[1]==UdsReqList[UdsVehicleTypeNum].ReqList[UdsCmdIndexNum][2])
		{
			if(UdsCmdIndexNum==UDS_SPEED)
			{
				if(DataLen>7)
				{
					UdsCmdResponseFlag = POSITIVE_RESPONSE;
				  UdsEngineSpeed = ((((uint16_t)pData[6])<<8) + (uint16_t)pData[7])>>1;
				}
			}
			else
			{
				if(DataLen>51)
				{
						uint16_t value = 0;
					if((ISO5765ProNum==ISO15765_11_500)||(ISO5765ProNum==ISO15765_29_500))  //如果波特率是500  则是 依柯卡特
					{
						value = (((((uint16_t)pData[16])<<8) + (uint16_t)pData[17])>>5 )-40;
						OBD_aftertreatment_infor.inLetTemp = (value+273)<<5;
						value = (((((uint16_t)pData[18])<<8) + (uint16_t)pData[19])>>5 )- 40;
						OBD_aftertreatment_infor.outLetTemp = (value+273)<<5;
					}
					else
					{
						value = ((((uint16_t)pData[16])<<8) + (uint16_t)pData[17])*225/1000-40;
						OBD_aftertreatment_infor.inLetTemp = (value+273)<<5;
						value = ((((uint16_t)pData[18])<<8) + (uint16_t)pData[19])*225/1000-40;
						OBD_aftertreatment_infor.outLetTemp = (value+273)<<5;
					}

					value = ((((uint16_t)pData[51])<<8) + (uint16_t)pData[52])*5/100-200;
					OBD_aftertreatment_infor.dnstSensValue = ((value+200)<<4) + ((value+200)<<2);
					value = (uint16_t)pData[43];
					OBD_aftertreatment_infor.reagAllowance = (value<<1)+(value>>1);
					UdsCmdResponseFlag = POSITIVE_RESPONSE;
				}
			}
		}
  }
}
//static void GetUdsData_YiLanKaTe(const uint8_t* pData, uint8_t DataLen)
//{
//	if(DataLen>1)
//	{
//		if(pData[1]==UdsReqList[UdsVehicleTypeNum].ReqList[UdsCmdIndexNum][2])
//		{
//			if(UdsCmdIndexNum==UDS_SPEED)
//			{
//				if(DataLen>7)
//				{
//					UdsCmdResponseFlag = POSITIVE_RESPONSE;
//				  UdsEngineSpeed = ((((uint16_t)pData[6])<<8) + (uint16_t)pData[7])>>1;
//				}
//			}
//			else
//			{
//				if(DataLen>51)
//				{
//					uint16_t value = 0;
//					value = (((((uint16_t)pData[16])<<8) + (uint16_t)pData[17])<<5 )-40;
//					OBD_aftertreatment_infor.inLetTemp = (value+273)<<5;
//					value = (((((uint16_t)pData[18])<<8) + (uint16_t)pData[19])<<5 )- 40;
//					OBD_aftertreatment_infor.outLetTemp = (value+273)<<5;

//					value = ((((uint16_t)pData[51])<<8) + (uint16_t)pData[52])*5/100-200;
//					OBD_aftertreatment_infor.dnstSensValue = ((value+200)<<4) + ((value+200)<<2);
//					value = (uint16_t)pData[43];
//					OBD_aftertreatment_infor.reagAllowance = (value<<1)+(value>>1);
//					UdsCmdResponseFlag = POSITIVE_RESPONSE;
//				}
//			}
//		}
//  }
//}

static void GetUdsData_ISO15031_256A_B_Div4(const uint8_t* pData, uint8_t DataLen)/*解析方式 (A*256+B)/4*/
{
	if(DataLen>=4)
	{
		if(pData[1] == UdsReqList[UdsVehicleTypeNum].ReqList[UdsCmdIndexNum][2])
		{
			UdsCmdResponseFlag = POSITIVE_RESPONSE;
		  uint16_t UdsData = (((uint16_t)pData[2]<<8)+(uint16_t)(pData[3]))>>2;
			UdsUpdateData(UdsData);
		}
	}
}

static void Get07_45data(const uint8_t* pData, uint8_t DataLen) /* 0-7数据，取45字节 (256*4+5)  */
{
	if(DataLen>4)
	{
		uint32_t fid = (uint32_t)(pData[1]<<8) + (uint32_t)pData[2];
		if(fid == ((uint32_t)(UdsReqList[UdsVehicleTypeNum].ReqList[UdsCmdIndexNum][2]<<8)+ \
			(uint32_t)(UdsReqList[UdsVehicleTypeNum].ReqList[UdsCmdIndexNum][3])))
		{
			UdsCmdResponseFlag = POSITIVE_RESPONSE;
			uint16_t UdsData = 0;
			UdsData = ((((uint16_t)pData[3])<<8) + (uint16_t)pData[4]);
			UdsUpdateData(UdsData);
		}
	}
}

static void GetUdsData_ISO15031_256A_B_Div10_Sub40(const uint8_t* pData, uint8_t DataLen)/*解析方式 (A*256+B)/10-40*/
{
	if(DataLen>=4)
	{
		if(pData[1] == UdsReqList[UdsVehicleTypeNum].ReqList[UdsCmdIndexNum][2])
		{
			UdsCmdResponseFlag = POSITIVE_RESPONSE;
		  uint16_t UdsData = ((uint16_t)pData[2]<<8)+(uint16_t)(pData[3]);
			UdsData /= 10;
			UdsData -= 40;
			UdsUpdateData(UdsData);
		}
	}
}

static void GetUdsData_ISO15031_256A_B_Mul99_Div1000(const uint8_t* pData, uint8_t DataLen)/*解析方式 (A*256+B)*0.099*/
{
	if(DataLen>=4)
	{
		if(pData[1] == UdsReqList[UdsVehicleTypeNum].ReqList[UdsCmdIndexNum][2])
		{
			UdsCmdResponseFlag = POSITIVE_RESPONSE;
		  uint16_t UdsData = (((uint16_t)pData[2]<<8)+(uint16_t)(pData[3]))*99/1000;
			UdsUpdateData(UdsData);
		}
	}
}

static void GetUdsData_ISO15031_256B_C(const uint8_t* pData, uint8_t DataLen)/*解析方式 (B*256+C)*/
{
	if(DataLen>=5)
	{
		if(pData[1] == UdsReqList[UdsVehicleTypeNum].ReqList[UdsCmdIndexNum][2])
		{
			UdsCmdResponseFlag = POSITIVE_RESPONSE;
		  uint16_t UdsData = (((uint16_t)pData[3])<<8)+(uint16_t)(pData[4]);
			UdsUpdateData(UdsData);
		}
	}
}

static void GetUdsData_ISO15031_100B_Div255(const uint8_t* pData, uint8_t DataLen)/*解析方式 (B*100/255)*/
{
	if(DataLen>=4)
	{
		if(pData[1] == UdsReqList[UdsVehicleTypeNum].ReqList[UdsCmdIndexNum][2])
		{
			UdsCmdResponseFlag = POSITIVE_RESPONSE;
		  uint16_t UdsData = pData[3]*100/255;
			UdsUpdateData(UdsData);
		}
	}
}

static void GetUdsData_ISO15031_256A_B_Div10(const uint8_t* pData, uint8_t DataLen)/*解析方式 (A*256+B)/10*/
{
	if(DataLen>=4)
	{
		if(pData[1] == UdsReqList[UdsVehicleTypeNum].ReqList[UdsCmdIndexNum][2])
		{
			UdsCmdResponseFlag = POSITIVE_RESPONSE;
		  uint16_t UdsData = (((uint16_t)pData[2]<<8)+(uint16_t)(pData[3]))/10;
			UdsUpdateData(UdsData);
		}
	}
}

static void GetUdsData_ISO15031_100A_Div255(const uint8_t* pData, uint8_t DataLen)/*解析方式 (A*100/255)*/
{
	if(DataLen>=4)
	{
		if(pData[1] == UdsReqList[UdsVehicleTypeNum].ReqList[UdsCmdIndexNum][2])
		{
			UdsCmdResponseFlag = POSITIVE_RESPONSE;
		  uint16_t UdsData = (pData[2]*100)>>8;
			UdsUpdateData(UdsData);
		}
	}
}

static void GetUdsData_16777216A_65536B_256C_D_Sub270(const uint8_t* pData, uint8_t DataLen) /*解析方式： (16777216*A+65536*B+256*C+D)-270  */
{
	if(DataLen>6)
	{
		uint16_t fid = (uint16_t)(pData[1]<<8) + (uint16_t)pData[2];
		if(fid == ((uint16_t)(UdsReqList[UdsVehicleTypeNum].ReqList[UdsCmdIndexNum][2]<<8))+(uint16_t)(UdsReqList[UdsVehicleTypeNum].ReqList[UdsCmdIndexNum][3]))
		{
			UdsCmdResponseFlag = POSITIVE_RESPONSE;
			uint32_t UdsData = 0;
			UdsData = (((uint32_t)pData[3])<<24)+(uint32_t)(pData[4]<<16)+(uint32_t)(pData[5]<<8)+(uint32_t)pData[6]-270;
			UdsUpdateData(UdsData);
		}
	}
}

static void GetUdsData_100D_Div250(const uint8_t* pData, uint8_t DataLen) /*解析方式： 100D/250  */
{
	if(DataLen>6)
	{
		uint16_t fid = (uint16_t)(pData[1]<<8) + (uint16_t)pData[2];
		if(fid == ((uint16_t)(UdsReqList[UdsVehicleTypeNum].ReqList[UdsCmdIndexNum][2]<<8))+(uint16_t)(UdsReqList[UdsVehicleTypeNum].ReqList[UdsCmdIndexNum][3]))
		{
			UdsCmdResponseFlag = POSITIVE_RESPONSE;
			uint16_t UdsData = 0;
			UdsData = pData[6]*100/250;
			UdsUpdateData(UdsData);
		}
	}
}

static void GetUdsData_256A_B_Mul625_Div10000_Sub40(const uint8_t* pData, uint8_t DataLen) /*解析方式： (256*A+B)*0.0625-40  */
{
	if(DataLen>4)
	{
		uint16_t fid = (uint16_t)(pData[1]<<8) + (uint16_t)pData[2];
		if(fid == ((uint16_t)(UdsReqList[UdsVehicleTypeNum].ReqList[UdsCmdIndexNum][2]<<8))+(uint16_t)(UdsReqList[UdsVehicleTypeNum].ReqList[UdsCmdIndexNum][3]))
		{
			UdsCmdResponseFlag = POSITIVE_RESPONSE;
			uint16_t UdsData = 0;
			UdsData = ((((uint16_t)pData[3])<<8) + (uint16_t)pData[4])*625/10000-40;
			UdsUpdateData(UdsData);
		}
	}
}

static void GetUdsData_256A_B_Div100(const uint8_t* pData, uint8_t DataLen) /*解析方式： (256*A+B)/100  */
{
	if(DataLen>4)
	{
		uint16_t fid = (uint16_t)(pData[1]<<8) + (uint16_t)pData[2];
		if(fid == ((uint16_t)(UdsReqList[UdsVehicleTypeNum].ReqList[UdsCmdIndexNum][2]<<8))+(uint16_t)(UdsReqList[UdsVehicleTypeNum].ReqList[UdsCmdIndexNum][3]))
		{
			UdsCmdResponseFlag = POSITIVE_RESPONSE;
			uint16_t UdsData = 0;
			UdsData = ((((uint16_t)pData[3])<<8) + (uint16_t)pData[4])/100;
			UdsUpdateData(UdsData);
		}
	}
}

/*凯瑞伟柯斯*/
static void GetUdsSpeed_KaiRuiWeiKeSi(const uint8_t* pData, uint8_t DataLen) /* 凯瑞伟柯斯 解析方式： (256*A+B)/10  */
{
	if(DataLen>5)
	{
		uint32_t fid = (uint32_t)(pData[1]<<16) + (uint32_t)(pData[2]<<8) + (uint32_t)pData[3];
		if(fid == ((uint32_t)(UdsReqList[UdsVehicleTypeNum].ReqList[UdsCmdIndexNum][2]<<16))+(uint32_t)(UdsReqList[UdsVehicleTypeNum].ReqList[UdsCmdIndexNum][3]<<8)+ \
			(uint32_t)(UdsReqList[UdsVehicleTypeNum].ReqList[UdsCmdIndexNum][4]))
		{
			UdsCmdResponseFlag = POSITIVE_RESPONSE;
			uint16_t UdsData = 0;
			UdsData = ((((uint16_t)pData[4])<<8) + (uint16_t)pData[5])/10;
			UdsUpdateData(UdsData);
		}
	}
}

static void GetScrTemp_KaiRuiWeiKeSi(const uint8_t* pData, uint8_t DataLen) /* 凯瑞伟柯斯 解析方式： (256*A+B)-40  */
{
	if(DataLen>5)
	{
		uint32_t fid = (uint32_t)(pData[1]<<16) + (uint32_t)(pData[2]<<8) + (uint32_t)pData[3];
		if(fid == ((uint32_t)(UdsReqList[UdsVehicleTypeNum].ReqList[UdsCmdIndexNum][2]<<16))+(uint32_t)(UdsReqList[UdsVehicleTypeNum].ReqList[UdsCmdIndexNum][3]<<8)+ \
			(uint32_t)(UdsReqList[UdsVehicleTypeNum].ReqList[UdsCmdIndexNum][4]))
		{
			UdsCmdResponseFlag = POSITIVE_RESPONSE;
			uint16_t UdsData = 0;
			UdsData = ((((uint16_t)pData[4])<<8) + (uint16_t)pData[5]) - 40;
			UdsUpdateData(UdsData);
		}
	}
}

static void GetNoxContest_KaiRuiWeiKeSi(const uint8_t* pData, uint8_t DataLen) /* 凯瑞伟柯斯 解析方式： (256*A+B)  */
{
	if(DataLen>5)
	{
		uint32_t fid = (uint32_t)(pData[1]<<16) + (uint32_t)(pData[2]<<8) + (uint32_t)pData[3];
		if(fid == ((uint32_t)(UdsReqList[UdsVehicleTypeNum].ReqList[UdsCmdIndexNum][2]<<16))+(uint32_t)(UdsReqList[UdsVehicleTypeNum].ReqList[UdsCmdIndexNum][3]<<8)+ \
			(uint32_t)(UdsReqList[UdsVehicleTypeNum].ReqList[UdsCmdIndexNum][4]))
		{
			UdsCmdResponseFlag = POSITIVE_RESPONSE;
			uint16_t UdsData = 0;
			UdsData = ((((uint16_t)pData[4])<<8) + (uint16_t)pData[5]);
			UdsUpdateData(UdsData);
		}
	}
}

static void GetUreaLiquidLevel_KaiRuiWeiKeSi(const uint8_t* pData, uint8_t DataLen) /* 凯瑞伟柯斯 解析方式： A(单位mm)  */
{
	if(DataLen>4)
	{
		uint32_t fid = (uint32_t)(pData[1]<<16) + (uint32_t)(pData[2]<<8) + (uint32_t)pData[3];
		if(fid == ((uint32_t)(UdsReqList[UdsVehicleTypeNum].ReqList[UdsCmdIndexNum][2]<<16))+(uint32_t)(UdsReqList[UdsVehicleTypeNum].ReqList[UdsCmdIndexNum][3]<<8)+ \
			(uint32_t)(UdsReqList[UdsVehicleTypeNum].ReqList[UdsCmdIndexNum][4]))
		{
			UdsCmdResponseFlag = POSITIVE_RESPONSE;
			uint16_t UdsData = 0;
			UdsData = pData[4]*100/255;
			UdsUpdateData(UdsData);
		}
	}
}

/*博世后处理，玉柴博世DCU*/
static void GetUdsSpeed_BoShiYuChai(const uint8_t* pData, uint8_t DataLen)  /*博世后处理，玉柴博世DCU  解析方式： (256*B+A)  */
{
	if(DataLen>2)
	{
		UdsCmdResponseFlag = POSITIVE_RESPONSE;
		uint16_t UdsData = 0;
		UdsData = (uint16_t)pData[1]+(uint16_t)(pData[2]<<8);
		UdsUpdateData(UdsData);
	}
}

//static void GetUdsScrBackTemp_BoShiYuChai(const uint8_t* pData, uint8_t DataLen)  /*博世后处理，玉柴博世DCU  解析方式： (256*B+A)*0.0625-273  */
//{
//	if(DataLen>2)
//	{
//		UdsCmdResponseFlag = POSITIVE_RESPONSE;
//		uint16_t UdsData = 0;
//		UdsData = ((uint16_t)pData[1]+(uint16_t)(pData[2]<<8))*625/10000 - 273;
//		UdsUpdateData(UdsData);
//	}
//}

//static void GetUdsNoxContest_BoShiYuChai(const uint8_t* pData, uint8_t DataLen)  /*博世后处理，玉柴博世DCU  解析方式： (256*B+A)  */
//{
//	if(DataLen>2)
//	{
//		UdsCmdResponseFlag = POSITIVE_RESPONSE;
//		uint16_t UdsData = 0;
//		UdsData = ((uint16_t)pData[1]+(uint16_t)(pData[2]<<8));
//		if(pData[2]&0x80)
//		{
//			UdsData -= 32768;
//		}
//		UdsUpdateData(UdsData);
//	}
//}

//static void GetUdsUreaLiquidLevel_BoShiYuChai(const uint8_t* pData, uint8_t DataLen)  /*博世后处理，玉柴博世DCU  解析方式： (256*B+A)/100  */
//{
//	if(DataLen>2)
//	{
//		UdsCmdResponseFlag = POSITIVE_RESPONSE;
//		uint16_t UdsData = 0;
//		UdsData = ((uint16_t)pData[1]+(uint16_t)(pData[2]<<8))/100;
//		UdsUpdateData(UdsData);
//	}
//}

/*凯德斯后处理*/
static void GetUdsSpeed_KaiDeSi(const uint8_t* pData, uint8_t DataLen)  /* 凯德斯 解析方式： (256*A+B)/8  */
{
	if(DataLen>2)
	{
		UdsCmdResponseFlag = POSITIVE_RESPONSE;
		uint16_t UdsData = 0;
		UdsData = ((uint16_t)(pData[1]<<8)+(uint16_t)pData[2])>>3;
		UdsUpdateData(UdsData);
	}
}

static void GetUdsData_256A_B_KaiDeSi(const uint8_t* pData, uint8_t DataLen)   /* 凯德斯 解析方式： (256*A+B)  */
{
	if(DataLen>2)
	{
		UdsCmdResponseFlag = POSITIVE_RESPONSE;
		uint16_t UdsData = 0;
		UdsData = (uint16_t)(pData[1]<<8)+(uint16_t)pData[2];
		UdsUpdateData(UdsData);
	}
}
static void Datong_TEMP_LQY(const uint8_t* pData, uint8_t DataLen)   //大通冷却液温度
{
	if(DataLen>2)
	{
		UdsCmdResponseFlag = POSITIVE_RESPONSE;
		uint16_t UdsData = 0;
		UdsData = ((uint16_t)(pData[3]<<8)+(uint16_t)pData[4])*0.1-273.1;
		UdsUpdateData(UdsData);
	}
}
static void Datong_enginefuelFlow(const uint8_t* pData, uint8_t DataLen)   //大通燃料流量
{
	if(DataLen>2)
	{
		UdsCmdResponseFlag = POSITIVE_RESPONSE;
		uint16_t UdsData = 0;
		UdsData = ((uint16_t)(pData[3]<<8)+(uint16_t)pData[4]);
		UdsUpdateData(UdsData);
	}
}

static void Datong_engineTorq(const uint8_t* pData, uint8_t DataLen)   //大通扭矩
{
	if(DataLen>2)
	{
		UdsCmdResponseFlag = POSITIVE_RESPONSE;
		uint16_t UdsData = 0;
		UdsData = (((uint16_t)(pData[3]<<8)+(uint16_t)pData[4])*0.1)+125;
		UdsUpdateData(UdsData);
	}
}



static void GetUdsUreaLiquidLevel_KaiDeSi(const uint8_t* pData, uint8_t DataLen) /* 凯德斯 解析方式： A  */
{
	if(DataLen>1)
	{
		UdsCmdResponseFlag = POSITIVE_RESPONSE;
		uint16_t UdsData = 0;
		UdsData = (uint16_t)pData[1];
		UdsUpdateData(UdsData);
	}
}

/*凯龙后处理*/
static void GetUdsSpeed_KaiLong(const uint8_t* pData, uint8_t DataLen) /*凯龙解析方式:256*B+A)/8  */
{
	if(DataLen>=5)
	{
		uint16_t fid = (uint16_t)(pData[1]<<8) + (uint16_t)pData[2];
		if(fid == ((uint16_t)(UdsReqList[UdsVehicleTypeNum].ReqList[UdsCmdIndexNum][2]<<8))+(uint16_t)(UdsReqList[UdsVehicleTypeNum].ReqList[UdsCmdIndexNum][3]))
		{
			UdsCmdResponseFlag = POSITIVE_RESPONSE;
			uint16_t UdsData = 0;
			UdsData = ((((uint16_t)pData[4])<<8)+(uint16_t)pData[3])>>3;
			UdsUpdateData(UdsData);
		}
	}
}

static void GetScrTemp_KaiLong(const uint8_t* pData, uint8_t DataLen) /*凯龙解析方式:256*A+B-40  */
{
	if(DataLen>=5)
	{
		uint16_t fid = (uint16_t)(pData[1]<<8) + (uint16_t)pData[2];
		if(fid == ((uint16_t)(UdsReqList[UdsVehicleTypeNum].ReqList[UdsCmdIndexNum][2]<<8))+(uint16_t)(UdsReqList[UdsVehicleTypeNum].ReqList[UdsCmdIndexNum][3]))
		{
			UdsCmdResponseFlag = POSITIVE_RESPONSE;
			uint16_t UdsData = 0;
			UdsData = (((((uint16_t)pData[3])<<8)+(uint16_t)pData[4])) - 40;
			UdsUpdateData(UdsData);
		}
	}
}

static void GetNoxContest_KaiLong(const uint8_t* pData, uint8_t DataLen) /*凯龙解析方式:(256*A+B)*0.05-200  */
{
	if(DataLen>=5)
	{
		uint16_t fid = (uint16_t)(pData[1]<<8) + (uint16_t)pData[2];
		if(fid == ((uint16_t)(UdsReqList[UdsVehicleTypeNum].ReqList[UdsCmdIndexNum][2]<<8))+(uint16_t)(UdsReqList[UdsVehicleTypeNum].ReqList[UdsCmdIndexNum][3]))
		{
			UdsCmdResponseFlag = POSITIVE_RESPONSE;
			uint16_t UdsData = 0;
			UdsData = (((((uint16_t)pData[3])<<8)+(uint16_t)pData[4]))*5/100 - 200;
			UdsUpdateData(UdsData);
		}
	}
}

static void GetUreaLiquidLevel_KaiLong(const uint8_t* pData, uint8_t DataLen) /*凯龙解析方式:A*100/250  */
{
	if(DataLen>=4)
	{
		uint16_t fid = (uint16_t)(pData[1]<<8) + (uint16_t)pData[2];
		if(fid == ((uint16_t)(UdsReqList[UdsVehicleTypeNum].ReqList[UdsCmdIndexNum][2]<<8))+(uint16_t)(UdsReqList[UdsVehicleTypeNum].ReqList[UdsCmdIndexNum][3]))
		{
			UdsCmdResponseFlag = POSITIVE_RESPONSE;
			uint16_t UdsData = 0;
			UdsData = pData[3]*100/250;
			UdsUpdateData(UdsData);
		}
	}
}

/*恒和后处理*/
static void GetUdsSpeed_HengHe(const uint8_t* pData, uint8_t DataLen)/*解析方式 (A*256+B)/4*/
{
	if(DataLen>=4)
	{
		if(pData[1] == UdsReqList[UdsVehicleTypeNum].ReqList[UdsCmdIndexNum][2])
		{
			UdsCmdResponseFlag = POSITIVE_RESPONSE;
		  uint16_t UdsData = (((uint16_t)pData[2]<<8)+(uint16_t)(pData[3]))>>2;
			UdsUpdateData(UdsData);
		}
	}
}

static void GetScrTemp_HengHe(const uint8_t* pData, uint8_t DataLen)/*解析方式 (A*256+B)*0.1-40*/
{
	if(DataLen>=4)
	{
		if(pData[1] == UdsReqList[UdsVehicleTypeNum].ReqList[UdsCmdIndexNum][2])
		{
			UdsCmdResponseFlag = POSITIVE_RESPONSE;
		  uint16_t UdsData = (((uint16_t)pData[2]<<8)+(uint16_t)(pData[3]))/10-40;
			UdsUpdateData(UdsData);
		}
	}
}

static void GetNoxContest_HengHe(const uint8_t* pData, uint8_t DataLen)/*解析方式 */
{
	if(DataLen>=5)
	{
		if(pData[1] == UdsReqList[UdsVehicleTypeNum].ReqList[UdsCmdIndexNum][2])
		{
			if((pData[2]==1)||(pData[2]==3))
			{
			  UdsCmdResponseFlag = POSITIVE_RESPONSE;
		    uint16_t UdsData = (((uint16_t)pData[3]<<8)+(uint16_t)(pData[4]));
			  UdsUpdateData(UdsData);
			}
			else
			{
				if((pData[2]==2)&&(DataLen>=7))
				{
					UdsCmdResponseFlag = POSITIVE_RESPONSE;
		      uint16_t UdsData = (((uint16_t)pData[5]<<8)+(uint16_t)(pData[6]));
			    UdsUpdateData(UdsData);
				}
			}
		}
	}
}

static void GetUreaLiquidLevel_HengHe(const uint8_t* pData, uint8_t DataLen)/*解析方式 B*100/255*/
{
	if(DataLen>=4)
	{
		if(pData[1] == UdsReqList[UdsVehicleTypeNum].ReqList[UdsCmdIndexNum][2])
		{
			UdsCmdResponseFlag = POSITIVE_RESPONSE;
		  uint16_t UdsData = pData[3]*100/255;
			UdsUpdateData(UdsData);
		}
	}
}
static void GetUreaLiquidLevel_50Ling(const uint8_t* pData, uint8_t DataLen)//*解析方式 C*0.4
{
	if(pData[0]==0xFD) 
		{
			UdsCmdResponseFlag = POSITIVE_RESPONSE;
		  uint16_t UdsData = pData[2]/2.5;
			UdsUpdateData(UdsData);
	}
}
static void GetNOX_benchi(const uint8_t* pData, uint8_t DataLen)//*解析方式 C*256+D
{
	if(DataLen>=7)
	{
		uint16_t fid = (uint16_t)(pData[1]<<8) + (uint16_t)pData[2];
		if(fid == ((uint16_t)(UdsReqList[UdsVehicleTypeNum].ReqList[UdsCmdIndexNum][2]<<8))+(uint16_t)(UdsReqList[UdsVehicleTypeNum].ReqList[UdsCmdIndexNum][3]))
		{
			UdsCmdResponseFlag = POSITIVE_RESPONSE;
			int UdsData = 0;
			UdsData = ((((uint16_t)pData[5])<<8)+(uint16_t)pData[6]);
			UdsUpdateData(UdsData);
		}
	}
}
static void GetUreaLiquidLevel_benchi(const uint8_t* pData, uint8_t DataLen)//*解析方蔆*256+D/100
{
	if(DataLen>=7)
	{
		uint16_t fid = (uint16_t)(pData[1]<<8) + (uint16_t)pData[2];
		if(fid == ((uint16_t)(UdsReqList[UdsVehicleTypeNum].ReqList[UdsCmdIndexNum][2]<<8))+(uint16_t)(UdsReqList[UdsVehicleTypeNum].ReqList[UdsCmdIndexNum][3]))
		{
			UdsCmdResponseFlag = POSITIVE_RESPONSE;
			int UdsData = 0;
			UdsData = ((((uint16_t)pData[5])<<8)+(uint16_t)pData[6])/100;
			UdsUpdateData(UdsData);
		}
	}
}

extern int uprintf(char *format, ...);


static u8 source=0;// 数据源  默认为0  非UDS采集到的

gps_info_t objGps;

struct data2App
{
  u16 speed;
  u16 engineSpeed;
  u16 fuelFlow;
  u16 scrNox;
  u8 reactantAllowance;
  u16 scrHeadTemp;
  u16 scrRearTemp;
  u16 dpfDiffPressure;
  u8   liquidLevel;
  uint32_t mileage;
	//add 190925
	int16_t coolantTemp;                       //发动机冷却液温度
	uint8_t  atmoPres;                         //大气压力
	uint8_t MIL_state;
	int8_t  engineTorq;                       //发动机扭矩
	
	int8_t fricTorq;		//摩擦扭矩   %
	uint16_t upstSensValue;		//SCR上游NOX传窯器输出值  ppm
	uint16_t airInflow;		//进气量		kg/h
	uint8_t model_Torq;		//扭矩模式--上海 没有单位
	uint8_t position_pedal;		//油门踏板--上海		%
	uint32_t totalfuel;		//总油耗--上海		L
	int16_t CatalystTemp;		//尿素箱温度--上海		℃    -40~210℃
	uint32_t DosingReag;		//尿素当前喷射量--上海		ml/h
	uint32_t TotalReagCom;		//尿素总消耗量--上海		g
	int16_t DiffTemp;		//DPF后温--上海		℃
	
	uint16_t PM_lightabs;                        //光吸收系数 K值
  uint16_t PM_opaticy;                          //不透光度
	
//	char latitude[10];  //纬度  ddmm.mmmm
//	char ns;    //北纬，南纬
//	char longitude[11]; //经度  dddmm.mmmm
//	char ew;   //东经，西经
	
	uint8_t contain1939;
	
  
}d2app;

void ShowData2App(void)
{
 uint8_t ShowData[12]={0};
	if(OBD_engine_infor.fricTorq==0xFF)
  {
    d2app.fricTorq=0xFF;
		uprintf("fricTorq:%d\r\n",255);
  }
  else
  {
    d2app.fricTorq=OBD_engine_infor.fricTorq-125;
		uprintf("fricTorq:%d\r\n",d2app.fricTorq);
  }


  if(OBD_aftertreatment_infor.upstSensValue==0xFFFF)
  {
		d2app.upstSensValue=0xFFFF;  
  }
  else
  {
    d2app.upstSensValue=OBD_aftertreatment_infor.upstSensValue/20-200;
  }

  if(OBD_engine_infor.enginefuelFlow==0xFFFF)
  {
		d2app.airInflow = OBD_engine_infor.enginefuelFlow*15/20; //MAF空气质量流速 
	}
	else
	{
		d2app.airInflow = OBD_engine_infor.airInflow/20;
	}
	
	
	if(OBD_engine_infor.position_pedal == 0xFF)
	{
		d2app.position_pedal = 0xFF;
	}
	else
	{
		d2app.position_pedal = OBD_engine_infor.position_pedal*2/5 ; // 油门踏板
	}


  if(	OBD_engine_infor.totalfuel== 0xFFFFFFF)
  {
		d2app.totalfuel =  0xFFFFFFFF;// OBD_engine_infor.totalfuel= temp; //发动机燃幼芡流量
	}
	else
	{
		d2app.totalfuel = OBD_engine_infor.totalfuel/2 ;//发动机燃幼芡流量
	}

				if(	OBD_aftertreatment_infor.CatalystTemp ==0xFF)
				{
					d2app.CatalystTemp =0xFFFF;
				}
				else
				{
					d2app.CatalystTemp = (OBD_aftertreatment_infor.CatalystTemp -40); 
				}

				if(OBD_aftertreatment_infor.DosingReag == 0xffffffff)
				{
						d2app.DosingReag = 0xffffffff;
				}
				else
				{
					d2app.DosingReag = OBD_aftertreatment_infor.DosingReag*3/8;
				}

				if(OBD_aftertreatment_infor.TotalReagCom == 0xffffffff)
				{
						d2app.TotalReagCom = 0xffffffff;
				}
				else
				{
					d2app.TotalReagCom = OBD_aftertreatment_infor.TotalReagCom/10;
				}

				if(OBD_aftertreatment_infor.DiffTemp == 0xffff)
				{
						d2app.DiffTemp = 0x8000;
				}
				else
				{
					d2app.DiffTemp = OBD_aftertreatment_infor.DiffTemp/32 - 273;
				}

	d2app.model_Torq = OBD_engine_infor.model_Torq;
//还没有计算
//uprintf("fricTorq:%d\r\n",d2app.fricTorq);
uprintf("upstSensValue:%d\r\n",d2app.upstSensValue);
uprintf("airInflow:%d\r\n",d2app.airInflow);
uprintf("model_Torq:%d\r\n",d2app.model_Torq);  //最后弄
uprintf("position_pedal:%d\r\n",d2app.position_pedal);
uprintf("totalfuel:%d\r\n",d2app.totalfuel);
uprintf("CatalystTemp:%d\r\n",d2app.CatalystTemp);
uprintf("DosingReag:%d\r\n",d2app.DosingReag);
uprintf("TotalReagCom:%d\r\n",d2app.TotalReagCom);
uprintf("DiffTemp:%d\r\n",d2app.DiffTemp);

  if(OBD_vehicle_infor.speed==0xFFFF)
  {
    d2app.speed=0xFFFF;
  }
  else
  {
    d2app.speed=OBD_vehicle_infor.speed>>8;
  }
  uprintf("speed:%d\r\n",d2app.speed);
  
  if(OBD_engine_infor.engineRev==0xFFFF)
  {
    d2app.engineSpeed=0xFFFF;
  }
  else
  {
    d2app.engineSpeed=OBD_engine_infor.engineRev>>3;
  }  
  uprintf("engineSpeed:%d\r\n",d2app.engineSpeed);
  
  if(OBD_engine_infor.enginefuelFlow==0xFFFF)
  {
    d2app.fuelFlow=0xFFFF;
  }
  else
  {
    d2app.fuelFlow=OBD_engine_infor.enginefuelFlow/20;
  }    
  uprintf("fuelFlow:%d\r\n",d2app.fuelFlow);
  
  if(OBD_aftertreatment_infor.dnstSensValue==0xFFFF)
  {
		d2app.scrNox=0xFFFF;  
		if(APP_Nox_indicate == 1)
		{
			d2app.scrNox=0x1A0A;   //为了让安装人员知道采集到了NOx的PGN  显示6666
		}
  }
  else
  {
    d2app.scrNox=OBD_aftertreatment_infor.dnstSensValue/20-200;
  }    
  uprintf("scrNox:%d\r\n",d2app.scrNox);
  
  if(OBD_aftertreatment_infor.reagAllowance==0xFF)
  {
    d2app.reactantAllowance=0xFF;
  }
  else
  {
    d2app.reactantAllowance=OBD_aftertreatment_infor.reagAllowance*2/5;
  }    
  uprintf("reactantAllowance:%d\r\n",d2app.reactantAllowance);
  
    if(OBD_aftertreatment_infor.inLetTemp==0xFFFF)
  {
    d2app.scrHeadTemp=0xFFFF;
  }
  else
  {
    d2app.scrHeadTemp=((OBD_aftertreatment_infor.inLetTemp>>5)-273);
  }  
  uprintf("scrHeadTemp:%d\r\n",d2app.scrHeadTemp);
  
    if(OBD_aftertreatment_infor.outLetTemp==0xFFFF)
  {
    d2app.scrRearTemp=0xFFFF;
  }
  else
  {
    d2app.scrRearTemp=((OBD_aftertreatment_infor.outLetTemp>>5)-273);
  }  
  uprintf("scrRearTemp:%d\r\n",d2app.scrRearTemp);
  
  if(OBD_aftertreatment_infor.diffPres==0xFFFF)
  {
    d2app.dpfDiffPressure=0xFFFF;
  }
  else
  {
    d2app.dpfDiffPressure=OBD_aftertreatment_infor.diffPres/10;
  }  
  uprintf("dpfDiffPressure:%d\r\n",d2app.dpfDiffPressure);
  
  if(OBD_vehicle_infor.liquidLevel==0xFF)
  {
    d2app.liquidLevel=0xFF;
  }
  else
  {
    d2app.liquidLevel=OBD_vehicle_infor.liquidLevel*2/5;
  }
  uprintf("liquidLevel:%d\r\n",d2app.liquidLevel);
  
  if(OBD_vehicle_infor.mileage==0xFFFFFFFF)
  {
    d2app.mileage=0xFFFFFFFF;
  }
  else
  {
    d2app.mileage=OBD_vehicle_infor.mileage/10;
  }
  uprintf("mileage:%u\r\n",d2app.mileage);
//========================================================	
	if(OBD_engine_infor.coolantTemp==0xFF)
  {
    d2app.coolantTemp=0xFFFF;
		uprintf("coolantTemp:%d\r\n",65535);
  }
  else
  {
    d2app.coolantTemp=OBD_engine_infor.coolantTemp-40;
	uprintf("coolantTemp:%d\r\n",d2app.coolantTemp);
  }

	
	if(OBD_engine_infor.atmoPres==0xFF)
  {
    d2app.atmoPres=0xFF;
  }
  else
  {
    d2app.atmoPres=OBD_engine_infor.atmoPres/2;
  }
	uprintf("atmoPres:%d\r\n",d2app.atmoPres);
	
	d2app.MIL_state=OBD_diagnosis_infor.MIL_state;
	uprintf("MIL_state:%d\r\n",d2app.MIL_state);
	
	if(OBD_engine_infor.engineTorq==0xFF)
  {
    d2app.engineTorq=0xFF;
		uprintf("engineTorq:%d\r\n",255);
  }
  else
  {
    d2app.engineTorq=OBD_engine_infor.engineTorq-125;
		uprintf("engineTorq:%d\r\n",d2app.engineTorq);
  }
//	uprintf("engineTorq:%d\r\n",d2app.engineTorq);
	
	
	if(OBD_aftertreatment_infor.PM_lightabs==0xFFFF)
  {
    d2app.PM_lightabs=0xFFFF;
  }
  else
  {
    d2app.PM_lightabs=((OBD_aftertreatment_infor.PM_lightabs));
  }  
  uprintf("PM_lightabs:%d\r\n",d2app.PM_lightabs);//0.01
	
	if(OBD_aftertreatment_infor.PM_opaticy==0xFFFF)
  {
    d2app.PM_opaticy=0xFFFF;
  }
  else
  {
    d2app.PM_opaticy=((OBD_aftertreatment_infor.PM_opaticy));
  }  
  uprintf("PM_opaticy:%d\r\n",d2app.PM_opaticy);//0.1
  
	get_gps_info(&objGps);
	
	memcpy(ShowData,objGps.latitude,10);

//	uprintf("latitude:%9s\r\n",objGps.latitude);
	uprintf("latitude:%10s\r\n",ShowData);
	uprintf("ns:%c",objGps.ns);
	uprintf("\r\n");

	memcpy(ShowData,objGps.longitude,11);
	uprintf("longitude:%11s\r\n",ShowData);
	uprintf("ew:%c",objGps.ew);
	uprintf("\r\n");
	d2app.contain1939=GetIsHave1939Pro();
	uprintf("contain1939:%d\r\n",d2app.contain1939);
	
	
  uprintf("protocol:%d\r\n",GetProtocolNum());
  uprintf("udsId:%d\r\n",UdsVehicleTypeNum);
  uprintf("source:%d\r\n",source);
  char vin[18]={0};
  memcpy(vin,OBD_vehicle_infor.VIN,17);
  uprintf("vin:%s\r\n",vin);
//添加读取flash vin	191206 yangchao	
	char vin2[18]={0};
	u8 res=get_vin((u8*)vin2);
	if(res==0)//没读到
	{
		uprintf("vin2:\r\n");
	}
  else
	{
		uprintf("vin2:%s\r\n",vin2);
	}
  
	  uprintf("guide:%d\r\n",guide);

	u8 IMEI_APP[15]={0};
	if(get_imei(IMEI_APP))
	{
		uprintf("imei:%s\r\n",IMEI_APP);
	}
	else
	{
		uprintf("imei:\r\n");
	}
	
//  u8 test[10]={0x30,0x31,0x30,0x31,0x30,0x31,0x30,0x31,0x30,0x31};
//  uprintf("test:%s\r\n",test);
  
}
