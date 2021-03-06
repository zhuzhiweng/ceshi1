#ifndef _GB17691A_H_
#define _GB17691A_H_

#include "sys.h"
#include "stdio.h"
#include <string.h>
//#include "FreeRTOS.h"
//#include "task.h"
#include "includes.h"

#include "delay.h"
#include "RTC.h"
#include "GPS_calculation.h"
#include "sim_uart.h"
#include "sim_net.h"
#include "spi_flash.h"
#include "gps.h"
//#include "key_and_charge.h"
//#include "usb_interface.h"
#include "GB17691_CAN_GPS.h"
#include "user_config.h"

#define ACTBLU_START_STRING   0x2424                 //艾可蓝协议起始符ACTBLU

#define START_STRING       0x2323                    //起始符
#define VEHICLE_LOGIN_CMD  0x01                      //车辆登入命令
#define VEHICLE_UPLOAD_CMD 0x02                      //车辆实时上报命令
#define VEHICLE_RELOAD_CMD 0x03                      //车辆补发G畔⑸媳?
#define VEHICLE_LOGOUT_CMD 0x04                      //车辆登出命令
#define VEHICLE_TIMING_CMD 0x05                      //车辆GＪ泵?
//#define VEHICLE_HEART_CMD  0x07                      //车辆G奶

#define VEHICLE_CONTROL_CMD 0x0C                      //平台控制命令下发
#define VEHICLE_ACKCON_CMD 0x0D                      //终端控制命令回复?

#define ZHENGZHOU_RECORD_CMD  0x07                      // 郑諭备案命令
#define ZHENGZHOU_TIMING_CMD  0xC0                      // 郑諭GＪ泵?
#define ZHENGZHOU_LOGIN_CMD  0xC2                      //  郑諭登录命令

#define ZHENGZHOU_RECORD_ACK_CMD  0x08                      // 郑諭备案应磄
#define ZHENGZHOU_TIMING_ACK_CMD  0xC1                      // 郑諭GＪ庇Υg
#define ZHENGZHOU_LOGIN_ACK_CMD  0xC3                      //  郑諭登录应磄
#define ZHENGZHOU_UPLOAD_ACK_CMD  0xF0                      //  郑諭上报应磄

#define VEHICLE_LOGIN_SUCCESS_CMD 0x81               //车辆登入成功应磄
#define VEHICLE_LOGIN_ERROR_CMD 0x91                 //车辆登入失败应磄
#define VEHICLE_LOGOUT_SUCCESS_CMD 0x84              //车辆登出成功应磄
#define VEHICLE_LOGOUT_ERROR_CMD 0x94                //车辆登出失败应磄
#define VEHICLE_TIMING_SUCCESS_CMD 0x85              //车辆GＪ背晒τΥg 
#define VEHICLE_TIMING_ERROR_CMD 0x95                //车辆GＪ笔О苡Υg
#define VEHICLE_HEART_SUCCESS_CMD 0x87               //车辆G奶晒τΥg
#define VEHICLE_HEART_ERROR_CMD 0x97                 //车辆G奶О苡Υg

//#define VEHICLE_SET_VIN_CMD 0xBF                      //设置VIN命令
//#define TERMINAL_SOFT_VER 01                           //01开始礗增  最大256



#define NO_ENCRYPT  0x01                              //蔵綢不加密
#define SM2_ENCRYPT  0x03                              //蔵綢SM2加密

#define ID_BOOL0 'D'
#define ID_BOOL1 'V'
#define ID_BOOL2 'U'

//G畔⒗郍捅曛径ㄒ?
#define GB17691_INFO_OBD_DATA          0x01    //OBDG畔⑻灞曛?
#define GB17691_INFO_DATASTREAM_DATA   0x02    //蔵綢流蔵綢标志
#define GB17691_INFO_CUSTOM_DATA       0x80    //自定义蔵綢标?
#define GB17691_INFO_IMEI_DATA         0x81    //IMEI蔵綢

#define GB17691_THREAD_DEALY  10               //线程延时
#define GB17691_CMD_CHECK_DEALY  (1000/GB17691_THREAD_DEALY) //命令检查延时

#define REALTIME_UPLOAD_FLAG 0xffff   //読在实时发送的蔵綢
#define RESEND_DATA_FLAG  0xfff0      //G枰狗⒌氖i綢
#define SEND_DATA_FLAG    0xff00      //发送成功的蔵綢

#define REAL_TIME_UPLOAD 0x01         //实时上报
#define SAVE_RESEND_DATA 0x02         //保存补发蔵綢
//#define SAVE_ALARM_DATA  0x04

#define TERMINAL_FREE   0x00          //终端释放模式
#define TERMINAL_NET    0x01          //终端网络模式
//#define TERMINAL_USB    0x02

#define LOGIN_MAX_CNT    5            //5->3登录最大计蔵
#define LOGIN_INTERVAL   5           //登录间隔60S

#define TIME_SEND_CNT    5             //5->3GＪ狈⑺妥畲蠹剖i
#define TIME_SEND_INTERVAL 5          //GＪ狈⑺图涓?60S

#define RECORD_SEND_CNT    10             //5->3GＪ狈⑺妥畲蠹剖i
#define RECORD_SEND_INTERVAL 20          //GＪ狈⑺图涓?60S

#define RESEND_WRITE_MAX_SIZE (0x200000)   //补发G醋畲蟠驡?2M

#define HEART_INTERVAL 50              //G奶涓?50S


#define FRAME_CNT_PER_PACKAGE 1        //每包帧计蔵
#if FRAME_CNT_PER_PACKAGE == 1
#define RESND_MSG_MAX_CNT  60         //补发最大计蔵
#elif FRAME_CNT_PER_PACKAGE == 2
#define RESND_MSG_MAX_CNT  80
#elif FRAME_CNT_PER_PACKAGE == 3
#define RESND_MSG_MAX_CNT  90
#else
#define RESND_MSG_MAX_CNT  100
#endif
//#define RESND_MSG_MAX_CNT ((600/MSG_SEND_INTERVAL)*FRAME_CNT_PER_PACKAGE)


#define TIME_PRESET_VALUE    0        //时间预设值6秒
#define MSG_SEND_INTERVAL  30

#define  GPS_LED_ON        	    (PAout(3)=1) 
#define  GPS_LED_OFF            (PAout(3)=0) 
#define READ_GPS_LED   PAin(3)    



typedef enum
{
    GB17691_IDLE,
    GB17691_LOGIN,
    GB17691_TIMING,
    GB17691_HEART,
    GB17691_LOGOUT
} GB17691_terminal_state_t;          //终端状态

#pragma pack(1)  //按字节对齐
typedef struct
{
    uint32_t timestamp;
    uint8_t  package_cnt;
    uint8_t  package_num;
    uint16_t resend_flag;
} save_resend_data_t;
#pragma pack()   //取消按字节对齐 恢复省缺对齐

////#pragma pack(1)  //按字节对齐
////诊断支持状态 每一位的含义:0=不支持;1=支持
//typedef struct
//{
//	uint16_t sptClst:1;               //催化转化器监控
//	uint16_t sptHeatedclst:1;         //加热催化转化器监控
//	uint16_t sptEprtsys:1;            //蒸发系统监控
//	uint16_t sptScdrairss:1;          //秙次空气系统监控
//	uint16_t sptAcsysrfgr:1;          //A/C系统制冷剂监控
//	uint16_t sptEsgassens:1;          //排气传窯器监控
//	uint16_t sptEsgassenshter:1;      //排气传窯器加热器监控
//	uint16_t sptEgrvvtsys:1;          //EGR系统和VVT监控
//	uint16_t sptColdstartaid:1;       //冷启动辅助系统监控
//	uint16_t sptBoostpresctrlsys:1;   //增压压力控制系统
//	uint16_t sptDpt:1;                //DPF监控
//	uint16_t sptScrnox:1;             //选择G源呋乖低?(SCR)或NOX吸附器
//	uint16_t sptNmhccvtnclst:1;       //NMHC氧化催化器监控
//	uint16_t sptMisfire:1;            //失籫监控
//	uint16_t sptFuelsys:1;            //燃油系统监控
//	uint16_t sptCphscpnt:1;           //综合成分监控
//}GB17691_OBD_dntSpt_t;

////诊断就G髯刺? 每一位的含义:0=测试完成或者不支持;1=测试未完成
//typedef struct
//{
//	uint16_t rdyClst:1;                //催化转化器监控
//	uint16_t rdyHeatedclst:1;          //加热催化转化器监控
//	uint16_t rdyEprtsys:1;             //蒸发系统监控
//	uint16_t rdyScdrairss:1;           //秙次空气系统监控
//	uint16_t rdyAcsysrfgr:1;           //A/C系统制冷剂监控
//	uint16_t rdyEsgassens:1;           //排气传窯器监控
//	uint16_t rdyEsgassenshter:1;       //排气传窯器加热器监控
//	uint16_t rdyEgrvvtsys:1;           //EGR系统和VVT监控
//	uint16_t rdyColdstartaid:1;        //冷启动辅助系统监控
//	uint16_t rdyBoostpresctrlsys:1;    //增压压力控制系统
//	uint16_t rdyDpt:1;                 //DPF监控
//	uint16_t rdyScrnox:1;              //选择G源呋乖低?(SCR)或NOX吸附器
//	uint16_t rdyNmhccvtnclst:1;        //NMHC氧化催化器监控
//	uint16_t rdyMisfire:1;             //失籫监控
//	uint16_t rdyFuelsys:1;             //燃油系统监控
//	uint16_t rdyCphscpnt:1;            //综合成分监控
//}GB17691_OBD_dntReady_t;

////IUPR值针对压燃式发动机
//typedef struct
//{
//	uint16_t iuprOBDcond ;           //OBD监测条約运GG计蔵
//	uint16_t iuprLgncntr;            //点籫循环计蔵器
//	uint16_t iuprHccatcomp;          //NMHC催化剂监测完成计蔵
//	uint16_t iuprHccatcond;          //NMHC催化剂监测条約运GG计蔵
//	uint16_t iuprNcatcomp;           //NOX/SCR催化剂监测完成条約计蔵
//	uint16_t iuprNcatcond;           //NOx/SCR催化剂监测条約运GG计蔵
//	uint16_t iuprNadscomp;           //NOx吸附器监测器完成条約计蔵
//	uint16_t iuprNadscond;           //NOx吸附器监测条約运GG计蔵
//	uint16_t iuprPmcomp;             //PM筰滤器监视器完成条約计蔵
//	uint16_t iuprPmcond;             //PM筰滤器监控条約运GG计蔵
//	uint16_t iuprEgscomp;            //废气传窯器监控完成监视计蔵
//	uint16_t iuprEgscond;            //废气传窯器监控运GG条約运GG计蔵
//	uint16_t iuprEgrcomp;            //EGR和/或VVT监测完成条約计蔵
//	uint16_t iuprEgrcond;            //EGR和/或VVT监测条約运GG计蔵
//	uint16_t iuprBpcomp;             //提蒳压力监测完成条約计蔵
//	uint16_t iuprBpcond;             //增压压力监测条約运GG计蔵
//	uint16_t iuprFuelcomp;           //燃料监测完成条約计蔵
//	uint16_t iuprFuelcond;           //燃料监测条約运GG计蔵

//}GB17691_OBD_IUPR_t;

//typedef struct
//{
//	uint32_t faultCode1;            //故障代码1
//	uint32_t faultCode2;            //故障代码2
//  uint32_t faultCode3;            //故障代码3
//	uint32_t faultCode4;            //故障代码4
//	uint32_t faultCode5;            //故障代码5
//}GB17691_OBD_faultCodelist_t;

//typedef  struct
//{
//  uint8_t obdDntPtc;                          //obd诊断G?
//  uint8_t MIL_state;                          //MIL状态
//  GB17691_OBD_dntSpt_t dntSptStatus;          //诊断支持状态
//  GB17691_OBD_dntReady_t dntReadyStatus;      //诊断就G髯刺?
//  uint8_t VIN[17];                            //车辆识眊码
//  uint8_t softCbrtNum[18];                    //软約标定识眊号
//  uint8_t CVN[18];                            //标定验证码
//  GB17691_OBD_IUPR_t iupr;                    //IUPR值
//  uint8_t faultCodeSum;                       //故障码总蔵
//  GB17691_OBD_faultCodelist_t faultCodelist;  // 故障码G畔⒘G表
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
//  uint16_t upstSensValue;                    //SCR上游NOX传窯器输出值
//  uint16_t dnstSensValue;                    //SCR下游NOx传窯器输出值
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
//	uint32_t mileage;                          //累计里程
//}GB17691_datastream_data_t;

//typedef  struct
//{
//  uint16_t datalen;                          //自定义G畔⑻宄ざ?
//  uint8_t  calcLoad;                         //计蔵负荷
//  uint16_t DiffTemp;                         //DPF后温
//  uint8_t  NOxInvertEff;                     //NOx转化G?
//	uint8_t  elecCDISatus;                     //电加热点籫状态
//	uint8_t  fuleInjectSatus;                  //燃油喷射状态
//	uint16_t ASL;                              //海拨
//}GB17691_custom_data_t;

//#pragma pack()   //取消按字节对齐 恢复省缺对齐

//任务优先级
#define GB17691_TASK_PRIO		4
//任务堆栈大G?
#define GB17691_STK_SIZE 	1024

uint8_t EvalBCC_FromBytes(uint8_t* Bytes, uint16_t len);   //异或GＱ?
void GB17691_CMD_analysis(uint8_t *pdata, uint16_t len);   //GB17691命令解析
void login_send(void);                                     //发送登入
void logout_send(void);                                    //发送登出
void GB17691_CMD_check(void);                              //命令检查
void GB17691_task(void const *pvParameters);               //GB17691任务
//void GB17691_task(void);                                 //GB17691任务
void GB17691_init(void);                                   //GB17691初始化
void GB17691_task_init(void);
void GB17691CheckCmd(void);
uint8_t get_lkt4305_state(void);
void LKT4305_Key_Init(void);
void Get_SM2id(void);
uint8_t get_big_heart_flag(void);
uint8_t get_login_state(void);
uint8_t* read_msg_from_flash(uint32_t* addr, uint32_t* resend_info_addr,uint8_t flash_area);


u16 get_AIKELAN_datapack(uint8_t  *p_datastream);//艾可蓝数据打包
void GB17691_AIKELAN_CheckCmd(void);//艾可蓝数据校验
#endif


