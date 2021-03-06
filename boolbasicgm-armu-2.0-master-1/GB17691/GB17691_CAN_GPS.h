#ifndef _GB17691_CAN_GPS_H
#define _GB17691_CAN_GPS_H
#include "sys.h"
#include "stdio.h"
#include <string.h>
#include "CAN.h"


#define _SWAP16(x)   ((x<<8)&0xff00)|((x>>8)&0x00ff)  //16bit字高低字节交换
#define _SWAP32(x)     (x>>24)|((x>>8)&0x0000ff00)|((x<<8)&0x00ff0000)|((x<<24)&0xff000000) //32bit双字

//#pragma pack(1)  //按字节对齐
//////诊断支持状态 每一位的含义:0=不支持;1=支持
////typedef struct
////{
////	uint16_t sptClst:1;               //催化转化器监控
////	uint16_t sptHeatedclst:1;         //加热催化转化器监控
////	uint16_t sptEprtsys:1;            //蒸发系统监控
////	uint16_t sptScdrairss:1;          //二次空气系统监控
////	uint16_t sptAcsysrfgr:1;          //A/C系统制冷剂监控
////	uint16_t sptEsgassens:1;          //排气传感器监控
////	uint16_t sptEsgassenshter:1;      //排气传感器加热器监控
////	uint16_t sptEgrvvtsys:1;          //EGR系统和VVT监控
////	uint16_t sptColdstartaid:1;       //冷启动辅助系统监控
////	uint16_t sptBoostpresctrlsys:1;   //增压压力控制系统
////	uint16_t sptDpt:1;                //DPF监控
////	uint16_t sptScrnox:1;             //选择性催化还原系统(SCR)或NOX吸附器
////	uint16_t sptNmhccvtnclst:1;       //NMHC氧化催化器监控
////	uint16_t sptMisfire:1;            //失火监控
////	uint16_t sptFuelsys:1;            //燃油系统监控
////	uint16_t sptCphscpnt:1;           //综合成分监控
////}OBD_dntSpt_t;

//////诊断就绪状态 每一位的含义:0=测试完成或者不支持;1=测试未完成
////typedef struct
////{
////	uint16_t rdyClst:1;                //催化转化器监控
////	uint16_t rdyHeatedclst:1;          //加热催化转化器监控
////	uint16_t rdyEprtsys:1;             //蒸发系统监控
////	uint16_t rdyScdrairss:1;           //二次空气系统监控
////	uint16_t rdyAcsysrfgr:1;           //A/C系统制冷剂监控
////	uint16_t rdyEsgassens:1;           //排气传感器监控
////	uint16_t rdyEsgassenshter:1;       //排气传感器加热器监控
////	uint16_t rdyEgrvvtsys:1;           //EGR系统和VVT监控
////	uint16_t rdyColdstartaid:1;        //冷启动辅助系统监控
////	uint16_t rdyBoostpresctrlsys:1;    //增压压力控制系统
////	uint16_t rdyDpt:1;                 //DPF监控
////	uint16_t rdyScrnox:1;              //选择性催化还原系统(SCR)或NOX吸附器
////	uint16_t rdyNmhccvtnclst:1;        //NMHC氧化催化器监控
////	uint16_t rdyMisfire:1;             //失火监控
////	uint16_t rdyFuelsys:1;             //燃油系统监控
////	uint16_t rdyCphscpnt:1;            //综合成分监控
////}OBD_dntReady_t;

//////IUPR值针对压燃式发动机
////typedef struct
////{
////	uint16_t iuprOBDcond ;           //OBD监测条件运行计数
////	uint16_t iuprLgncntr;            //点火循环计数器
////	uint16_t iuprHccatcomp;          //NMHC催化剂监测完成计数
////	uint16_t iuprHccatcond;          //NMHC催化剂监测条件运行计数
////	uint16_t iuprNcatcomp;           //NOX/SCR催化剂监测完成条件计数
////	uint16_t iuprNcatcond;           //NOx/SCR催化剂监测条件运行计数
////	uint16_t iuprNadscomp;           //NOx吸附器监测器完成条件计数
////	uint16_t iuprNadscond;           //NOx吸附器监测条件运行计数
////	uint16_t iuprPmcomp;             //PM过滤器监视器完成条件计数
////	uint16_t iuprPmcond;             //PM过滤器监控条件运行计数
////	uint16_t iuprEgscomp;            //废气传感器监控完成监视计数
////	uint16_t iuprEgscond;            //废气传感器监控运行条件运行计数
////	uint16_t iuprEgrcomp;            //EGR和/或VVT监测完成条件计数
////	uint16_t iuprEgrcond;            //EGR和/或VVT监测条件运行计数
////	uint16_t iuprBpcomp;             //提升压力监测完成条件计数
////	uint16_t iuprBpcond;             //增压压力监测条件运行计数
////	uint16_t iuprFuelcomp;           //燃料监测完成条件计数
////	uint16_t iuprFuelcond;           //燃料监测条件运行计数
////}OBD_IUPR_t;

//////typedef struct
//////{
//////	uint32_t faultCode1;            //故障代码1
//////	uint32_t faultCode2;            //故障代码2
//////  uint32_t faultCode3;            //故障代码3
//////	uint32_t faultCode4;            //故障代码4
//////	uint32_t faultCode5;            //故障代码5
//////}OBD_faultCodelist_t;

////typedef struct
////{
////	uint32_t faultCode[50];            //故障代码1
////}OBD_faultCodelist_t;

////typedef  struct
////{
////  uint8_t obdDntPtc;                          //obd诊断协议
////  uint8_t MIL_state;                          //MIL状态
////  OBD_dntSpt_t dntSptStatus;                  //诊断支持状态
////  OBD_dntReady_t dntReadyStatus;              //诊断就绪状态
////  uint8_t VIN[17];                            //车辆识别码
////  uint8_t softCbrtNum[18];                    //软件标定识别号
////  uint8_t CVN[18];                            //标定验证码
////  OBD_IUPR_t iupr;                            //IUPR值
////  uint8_t faultCodeSum;                       //故障码总数
////  OBD_faultCodelist_t faultCodelist;           // 故障码信息列表
////}obd_data_t;

////typedef struct
////{
////	  uint8_t is_valid:1;
////		uint8_t NorS:1;
////		uint8_t EorW:1;
////		uint8_t reserve:5;
////}locationStatus_t;

////typedef  struct
////{
////  uint16_t speed;                            //车速
////  uint8_t  atmoPres;                         //大气压力
////  uint8_t  engineTorq;                       //发动机扭矩
////  uint8_t  fricTorq;                         //摩擦扭矩
////  uint16_t engineRev;                        //发动机转速
////  uint16_t enginefuelFlow;                   //发动机燃料流量
////  uint16_t upstSensValue;                    //SCR上游NOX传感器输出值
////  uint16_t dnstSensValue;                    //SCR下游NOx传感器输出值
////  uint8_t  reagAllowance;                    //反应剂余量
////	uint16_t airInflow;                        //进气量
////	uint16_t inLetTemp;                        //SCR入口温度
////	uint16_t outLetTemp;                       //SCR出口温度
////	uint16_t diffPres;                         //DPF压差
////	uint8_t coolantTemp;                       //发动机冷却液温度
////	uint8_t liquidLevel;                       //油箱液位
//////	GB17691_locationStatus_t locationStatus;   //定位状态
//////	uint8_t Ing[4];                            //精度
//////	uint8_t Lat[4];                            //纬度
////	uint32_t mileage;                          //累计里程
////}datastream_data_t;

////typedef  struct
////{
////  uint16_t datalen;                          //自定义信息体长度
////  uint8_t  calcLoad;                         //计数负荷
////  uint16_t DiffTemp;                         //DPF后温
////  uint8_t  NOxInvertEff;                     //NOx转化效率
////	uint8_t  elecCDISatus;                     //电加热点火状态
////	uint8_t  fuleInjectSatus;                  //燃油喷射状态
////	uint16_t ASL;                              //海拨
////}custom_data_t;
//////#pragma pack()   //取消按字节对齐 恢复省缺对齐


////#pragma pack(1)  //按字节对齐
////诊断支持状态 每一位的含义:0=不支持;1=支持
//typedef struct
//{
//	uint16_t sptClst:1;               //催化转化器监控
//	uint16_t sptHeatedclst:1;         //加热催化转化器监控
//	uint16_t sptEprtsys:1;            //蒸发系统监控
//	uint16_t sptScdrairss:1;          //二次空气系统监控
//	uint16_t sptAcsysrfgr:1;          //A/C系统制冷剂监控
//	uint16_t sptEsgassens:1;          //排气传感器监控
//	uint16_t sptEsgassenshter:1;      //排气传感器加热器监控
//	uint16_t sptEgrvvtsys:1;          //EGR系统和VVT监控
//	uint16_t sptColdstartaid:1;       //冷启动辅助系统监控
//	uint16_t sptBoostpresctrlsys:1;   //增压压力控制系统
//	uint16_t sptDpt:1;                //DPF监控
//	uint16_t sptScrnox:1;             //选择性催化还原系统(SCR)或NOX吸附器
//	uint16_t sptNmhccvtnclst:1;       //NMHC氧化催化器监控
//	uint16_t sptMisfire:1;            //失火监控
//	uint16_t sptFuelsys:1;            //燃油系统监控
//	uint16_t sptCphscpnt:1;           //综合成分监控
//}GB17691_OBD_dntSpt_t;

////诊断就绪状态 每一位的含义:0=测试完成或者不支持;1=测试未完成
//typedef struct
//{
//	uint16_t rdyClst:1;                //催化转化器监控
//	uint16_t rdyHeatedclst:1;          //加热催化转化器监控
//	uint16_t rdyEprtsys:1;             //蒸发系统监控
//	uint16_t rdyScdrairss:1;           //二次空气系统监控
//	uint16_t rdyAcsysrfgr:1;           //A/C系统制冷剂监控
//	uint16_t rdyEsgassens:1;           //排气传感器监控
//	uint16_t rdyEsgassenshter:1;       //排气传感器加热器监控
//	uint16_t rdyEgrvvtsys:1;           //EGR系统和VVT监控
//	uint16_t rdyColdstartaid:1;        //冷启动辅助系统监控
//	uint16_t rdyBoostpresctrlsys:1;    //增压压力控制系统
//	uint16_t rdyDpt:1;                 //DPF监控
//	uint16_t rdyScrnox:1;              //选择性催化还原系统(SCR)或NOX吸附器
//	uint16_t rdyNmhccvtnclst:1;        //NMHC氧化催化器监控
//	uint16_t rdyMisfire:1;             //失火监控
//	uint16_t rdyFuelsys:1;             //燃油系统监控
//	uint16_t rdyCphscpnt:1;            //综合成分监控
//}GB17691_OBD_dntReady_t;

////IUPR值针对压燃式发动机
//typedef struct
//{
//	uint16_t iuprOBDcond ;           //OBD监测条件运行计数
//	uint16_t iuprLgncntr;            //点火循环计数器
//	uint16_t iuprHccatcomp;          //NMHC催化剂监测完成计数
//	uint16_t iuprHccatcond;          //NMHC催化剂监测条件运行计数
//	uint16_t iuprNcatcomp;           //NOX/SCR催化剂监测完成条件计数
//	uint16_t iuprNcatcond;           //NOx/SCR催化剂监测条件运行计数
//	uint16_t iuprNadscomp;           //NOx吸附器监测器完成条件计数
//	uint16_t iuprNadscond;           //NOx吸附器监测条件运行计数
//	uint16_t iuprPmcomp;             //PM过滤器监视器完成条件计数
//	uint16_t iuprPmcond;             //PM过滤器监控条件运行计数
//	uint16_t iuprEgscomp;            //废气传感器监控完成监视计数
//	uint16_t iuprEgscond;            //废气传感器监控运行条件运行计数
//	uint16_t iuprEgrcomp;            //EGR和/或VVT监测完成条件计数
//	uint16_t iuprEgrcond;            //EGR和/或VVT监测条件运行计数
//	uint16_t iuprBpcomp;             //提升压力监测完成条件计数
//	uint16_t iuprBpcond;             //增压压力监测条件运行计数
//	uint16_t iuprFuelcomp;           //燃料监测完成条件计数
//	uint16_t iuprFuelcond;           //燃料监测条件运行计数
//}GB17691_OBD_IUPR_t;

////typedef struct
////{
////	uint32_t faultCode1;            //故障代码1
////	uint32_t faultCode2;            //故障代码2
////  uint32_t faultCode3;            //故障代码3
////	uint32_t faultCode4;            //故障代码4
////	uint32_t faultCode5;            //故障代码5
////}GB17691_OBD_faultCodelist_t;

//typedef struct
//{
//	uint32_t faultCode[50];            //故障代码最大50个
//}GB17691_OBD_faultCodelist_t;

//typedef  struct
//{
//  uint8_t obdDntPtc;                          //obd诊断协议
//  uint8_t MIL_state;                          //MIL状态
//  GB17691_OBD_dntSpt_t dntSptStatus;          //诊断支持状态
//  GB17691_OBD_dntReady_t dntReadyStatus;      //诊断就绪状态
//  uint8_t VIN[17];                            //车辆识别码
//  uint8_t softCbrtNum[18];                    //软件标定识别号
//  uint8_t CVN[18];                            //标定验证码
//  GB17691_OBD_IUPR_t iupr;                    //IUPR值
//  uint8_t faultCodeSum;                       //故障码总数
//  //GB17691_OBD_faultCodelist_t faultCodelist;  // 故障码信息列表
//	 uint8_t faultCode[25][4];            // 故障码信息列表
//}GB17691_obd_data_t;

//typedef struct
//{
//	  uint8_t is_valid:1;
//		uint8_t NorS:1;
//		uint8_t EorW:1;
//		uint8_t reserve:5;
//}GB17691_locationStatus_t;

//typedef  struct
//{
//  uint16_t speed;                            //车速
//  uint8_t  atmoPres;                         //大气压力
//  uint8_t  engineTorq;                       //发动机扭矩
//  uint8_t  fricTorq;                         //摩擦扭矩
//  uint16_t engineRev;                        //发动机转速
//  uint16_t enginefuelFlow;                   //发动机燃料流量
//  uint16_t upstSensValue;                    //SCR上游NOX传感器输出值
//  uint16_t dnstSensValue;                    //SCR下游NOx传感器输出值
//  uint8_t  reagAllowance;                    //反应剂余量
//	uint16_t airInflow;                        //进气量
//	uint16_t inLetTemp;                        //SCR入口温度
//	uint16_t outLetTemp;                       //SCR出口温度
//	uint16_t diffPres;                         //DPF压差
//	uint8_t coolantTemp;                       //发动机冷却液温度
//	uint8_t liquidLevel;                       //油箱液位
//	GB17691_locationStatus_t locationStatus;   //定位状态
//	uint8_t Ing[4];                            //精度
//	uint8_t Lat[4];                            //纬度
//  //uint32_t Ing;                              //精度
//	//uint32_t Lat;                              //纬度
//	uint32_t mileage;                          //累计里程
//}GB17691_datastream_data_t;

//typedef  struct
//{
//  uint16_t datalen;                          //自定义信息体长度
//  uint8_t  calcLoad;                         //计数负荷
//  uint16_t DiffTemp;                         //DPF后温
//  uint8_t  NOxInvertEff;                     //NOx转化效率
//	uint8_t  elecCDISatus;                     //电加热点火状态
//	uint8_t  fuleInjectSatus;                  //燃油喷射状态
//	uint16_t ASL;                              //海拨
//	uint16_t fuelrate;                         //燃烧器喷油量  20190318_add
//}GB17691_custom_data_t;
//#pragma pack()   //取消按字节对齐 恢复省缺对齐

//void GB17691_get_OBD_Info(GB17691_obd_data_t *p_obd_data);
//void GB17691_get_datastream_Info(GB17691_datastream_data_t *p_datastream);
//void GB17691_get_custom_Info(GB17691_custom_data_t *p_custom_data);

uint16_t GB17691_get_OBD_Info(uint8_t *p_obd_data);
uint16_t GB17691_get_datastream_Info(uint8_t  *p_datastream);
uint16_t GB17691_get_custom_Info(uint8_t *p_custom_data);
uint16_t GB17691_get_BOOLcustom_Info(uint8_t *p_custom_data);
uint16_t GB17691_get_VIN_Info(uint8_t *p_VIN_Info);
uint8_t  GB17691_get_softver_Info(void);
uint16_t GB17691_get_BOOLdatastream_Info(uint8_t  *p_datastream);
uint8_t GB17691_get_ver_Info(uint8_t *p_verInfo);


//u16 get_xinxiang_datapack(uint8_t  *p_datastream);
//u8 get_xinxiang_datalen(u16 data_len,uint8_t  *p_datastream);
//void get_xinxiang_crcre(u16 crcre,uint8_t  *p_datastream);

u16 get_AIKELAN_register(uint8_t  *p_datastream);
void get_AIKELAN_crcre(u16 crcre,uint8_t  *p_datastream);

#endif


