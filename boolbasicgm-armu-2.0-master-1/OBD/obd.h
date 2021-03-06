#ifndef __OBD_H
#define __OBD_H	 
#include "sys.h"


#define FAULT_CODE_MAX_SIZE 25

#define PROTOCOL_NUM_CNT 5



#define  CAN_LED_ON        	    (PBout(0)=0) 
#define  CAN_LED_OFF            (PBout(0)=1) 
#define READ_CAN_LED   PBin(0)    

typedef enum
{
	Guide_Idle =0,
	Guide_Idle_YesF101=1,
	Guide_Idle_NoF101=2,
	AuxCan19_250_YesF101=3,
	AuxCan19_250_NoF101=4,
	AuxCan19_500_YesF101=5,
	AuxCan19_500_NoF101=6,
 AuxCan311_250_YesF101=7,
 AuxCan311_250_NoF101=8,
 AuxCan311_500_YesF101=9,
 AuxCan311_500_NoF101=10,
}GuideNum;

typedef enum
{
	ProtocolIdle=0,
	J1939_29_250=1,
	ISO15765_11_500=2,
	ISO15765_29_500=3,
	ISO15765_11_250=4,
	ISO15765_29_250=5
}ProtocolNum_t;

//typedef enum
//{
//	ISO15765_11_250=0,
//	ISO15765_29_250=1,
//	ISO15765_11_500=2,
//	ISO15765_29_500=3
//}ISO15765ProtocolNum_t;


typedef struct
{
	uint16_t sptClst:1;               //催化转化器监控
	uint16_t sptHeatedclst:1;         //加热催化转化器监控
	uint16_t sptEprtsys:1;            //蒸发系统监控
	uint16_t sptScdrairss:1;          //二次空气系统监控
	uint16_t sptAcsysrfgr:1;          //A/C系统制冷剂监控
	uint16_t sptEsgassens:1;          //排气传感器监控
	uint16_t sptEsgassenshter:1;      //排气传感器加热器监控
	uint16_t sptEgrvvtsys:1;          //EGR系统和VVT监控
	uint16_t sptColdstartaid:1;       //冷启动辅助系统监控
	uint16_t sptBoostpresctrlsys:1;   //增压压力控制系统
	uint16_t sptDpt:1;                //DPF监控
	uint16_t sptScrnox:1;             //选择性催化还原系统(SCR)或NOX吸附器
	uint16_t sptNmhccvtnclst:1;       //NMHC氧化催化器监控
	uint16_t sptMisfire:1;            //失火监控
	uint16_t sptFuelsys:1;            //燃油系统监控
	uint16_t sptCphscpnt:1;           //综合成分监控
}OBD_dntSpt_t;

//诊断就绪状态 每一位的含义:0=测试完成或者不支持;1=测试未完成
typedef struct
{
	uint16_t rdyClst:1;                //催化转化器监控
	uint16_t rdyHeatedclst:1;          //加热催化转化器监控
	uint16_t rdyEprtsys:1;             //蒸发系统监控
	uint16_t rdyScdrairss:1;           //二次空气系统监控
	uint16_t rdyAcsysrfgr:1;           //A/C系统制冷剂监控
	uint16_t rdyEsgassens:1;           //排气传感器监控
	uint16_t rdyEsgassenshter:1;       //排气传感器加热器监控
	uint16_t rdyEgrvvtsys:1;           //EGR系统和VVT监控
	uint16_t rdyColdstartaid:1;        //冷启动辅助系统监控
	uint16_t rdyBoostpresctrlsys:1;    //增压压力控制系统
	uint16_t rdyDpt:1;                 //DPF监控
	uint16_t rdyScrnox:1;              //选择性催化还原系统(SCR)或NOX吸附器
	uint16_t rdyNmhccvtnclst:1;        //NMHC氧化催化器监控
	uint16_t rdyMisfire:1;             //失火监控
	uint16_t rdyFuelsys:1;             //燃油系统监控
	uint16_t rdyCphscpnt:1;            //综合成分监控
}OBD_dntReady_t;

//IUPR值针对压燃式发动机
typedef struct
{
	uint16_t iuprOBDcond ;           //OBD监测条件运行计数
	uint16_t iuprLgncntr;            //点火循环计数器
	uint16_t iuprHccatcomp;          //NMHC催化剂监测完成计数
	uint16_t iuprHccatcond;          //NMHC催化剂监测条件运行计数
	uint16_t iuprNcatcomp;           //NOX/SCR催化剂监测完成条件计数
	uint16_t iuprNcatcond;           //NOx/SCR催化剂监测条件运行计数
	uint16_t iuprNadscomp;           //NOx吸附器监测器完成条件计数
	uint16_t iuprNadscond;           //NOx吸附器监测条件运行计数
	uint16_t iuprPmcomp;             //PM过滤器监视器完成条件计数
	uint16_t iuprPmcond;             //PM过滤器监控条件运行计数
	uint16_t iuprEgscomp;            //废气传感器监控完成监视计数
	uint16_t iuprEgscond;            //废气传感器监控运行条件运行计数
	uint16_t iuprEgrcomp;            //EGR和/或VVT监测完成条件计数
	uint16_t iuprEgrcond;            //EGR和/或VVT监测条件运行计数
	uint16_t iuprBpcomp;             //提升压力监测完成条件计数
	uint16_t iuprBpcond;             //增压压力监测条件运行计数
	uint16_t iuprFuelcomp;           //燃料监测完成条件计数
	uint16_t iuprFuelcond;           //燃料监测条件运行计数
}OBD_IUPR_t;


typedef  struct
{
	uint32_t mileage;                           //累计里程
	uint16_t speed;                             //车速
	uint8_t liquidLevel;                        //油箱液位
  uint8_t softCbrtNum[18];                    //软件标定识别号
  uint8_t CVN[18];                            //标定验证码
  uint8_t VIN[17];                            //车辆识别码
}OBD_Vehicle_Infor_t;


typedef  struct
{
	 uint8_t obdDntPtc;                          //obd诊断协议
	 uint8_t MIL_state;                          //MIL状态
	 OBD_dntSpt_t dntSptStatus;                  //诊断支持状态
	 OBD_dntReady_t dntReadyStatus;              //诊断就绪状态
   OBD_IUPR_t iupr;                            //IUPR值
   uint8_t faultCodeSum;                       //故障码总数
   uint8_t faultCode[25][4];  // 故障码信息列表
}OBD_Diagnosis_Infor_t;


typedef  struct
{
  uint16_t engineRev;                        //发动机转速
  uint16_t enginefuelFlow;                   //发动机燃料流量 ----
	uint16_t airInflow;                        //进气量----
	uint8_t  atmoPres;                         //大气压力
  uint8_t  engineTorq;                       //发动机扭矩----
  uint8_t  fricTorq;                         //摩擦扭矩
	uint8_t coolantTemp;                       //发动机冷却液温度
  uint8_t  calcLoad;                         //计算负荷
	uint8_t		model_Torq;												//扭矩模式--上海
	uint8_t		position_pedal;												//油门踏板	--上海
	uint32_t		totalfuel;												//总油耗--上海
	
}OBD_Engine_Infor_t;

typedef  struct
{
	uint16_t inLetTemp;                        //SCR入口温度
	uint16_t outLetTemp;                       //SCR出口温度
  uint16_t upstSensValue;                    //SCR上游NOX传窯器输出值
  uint16_t dnstSensValue;                    //SCR下游NOx传窯器输出值
	uint16_t diffPres;                         //DPF压差
  uint16_t DiffTemp;                         //DPF后温
  uint8_t  NOxInvertEff;                     //NOx转化G?
	uint8_t  elecCDISatus;                     //电加热点籫状态
	uint8_t  fuleInjectSatus;                  //燃烧器喷油状态
	uint16_t fuelrate;                 			 	 //燃烧器喷油量
  uint8_t  reagAllowance;                    //反应剂余量
  uint8_t  CatalystTemp;                    //尿素箱温度
  uint32_t	DosingReag;												//尿素当前喷射量--上海
  uint32_t	TotalReagCom;												//尿素总消耗量--上海
	uint16_t  PMdata;																		//颗粒物浓度 --郑州
	uint16_t PM_lightabs;												//光吸收系数
	uint16_t PM_opaticy;														//不透光度
}OBD_Aftertreatment_Infor_t;
    
typedef struct
{
	u8 total_frame_no;				//多帧的总包数 
	u8 valid_byte_no;					//有效字节长度 
	u8 remaining_space;				//剩余的字节大小
	u8 success_no;						//是否接收完整
	u8 items_no;							//此多包的在09模式下是项目数  在01模式下是A字节
	//以下是03和07模式使用的
	u8 DTC_NO;						//此多包的在03或者07模式下是故障数

} ReMessage_stateCanTxMsg_t;

void Can_clear(void);

void  get_OBD_Vehicle_Infor_t(  OBD_Vehicle_Infor_t  *p );
void  get_OBD_Diagnosis_Infor_t(  OBD_Diagnosis_Infor_t  *p );
void  get_OBD_Engine_Infor_t(  OBD_Engine_Infor_t *p );
void  get_OBD_Aftertreatment_Infor_t(  OBD_Aftertreatment_Infor_t  *p );
void Obd_init(void);	

u32 Count_FUN(u16 *p,u32 count);

extern  OBD_Engine_Infor_t 	OBD_engine_infor;
extern  OBD_Vehicle_Infor_t OBD_vehicle_infor;
extern  OBD_Diagnosis_Infor_t  OBD_diagnosis_infor;
extern  OBD_Aftertreatment_Infor_t OBD_aftertreatment_infor;
//extern u8 temp_obd_group[150];

extern void can_collection(void);
extern uint8_t iso_15765_canAdd_Blacklist_judge(uint32_t canid);
extern u8  judge_is_ASCII(char * p,u16 P_NO);			//P 指针  P_NO为字节数  //返回0 为不全是asckII  1 表示全部是ascII

uint8_t GetProtocolNum(void);
#endif




















