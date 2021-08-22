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

#define ACTBLU_START_STRING   0x2424                 //������Э����ʼ��ACTBLU

#define START_STRING       0x2323                    //��ʼ��
#define VEHICLE_LOGIN_CMD  0x01                      //������������
#define VEHICLE_UPLOAD_CMD 0x02                      //����ʵʱ�ϱ�����
#define VEHICLE_RELOAD_CMD 0x03                      //��������G�Ϣ�ϱ�����
#define VEHICLE_LOGOUT_CMD 0x04                      //�����ǳ�����
#define VEHICLE_TIMING_CMD 0x05                      //����G�ʱ����
//#define VEHICLE_HEART_CMD  0x07                      //����G������

#define VEHICLE_CONTROL_CMD 0x0C                      //ƽ̨���������·�
#define VEHICLE_ACKCON_CMD 0x0D                      //�ն˿�������ظ��

#define ZHENGZHOU_RECORD_CMD  0x07                      // ֣�I��������
#define ZHENGZHOU_TIMING_CMD  0xC0                      // ֣�IG�ʱ����
#define ZHENGZHOU_LOGIN_CMD  0xC2                      //  ֣�I��¼����

#define ZHENGZHOU_RECORD_ACK_CMD  0x08                      // ֣�I����Ӧ�g
#define ZHENGZHOU_TIMING_ACK_CMD  0xC1                      // ֣�IG�ʱӦ�g
#define ZHENGZHOU_LOGIN_ACK_CMD  0xC3                      //  ֣�I��¼Ӧ�g
#define ZHENGZHOU_UPLOAD_ACK_CMD  0xF0                      //  ֣�I�ϱ�Ӧ�g

#define VEHICLE_LOGIN_SUCCESS_CMD 0x81               //��������ɹ�Ӧ�g
#define VEHICLE_LOGIN_ERROR_CMD 0x91                 //��������ʧ��Ӧ�g
#define VEHICLE_LOGOUT_SUCCESS_CMD 0x84              //�����ǳ��ɹ�Ӧ�g
#define VEHICLE_LOGOUT_ERROR_CMD 0x94                //�����ǳ�ʧ��Ӧ�g
#define VEHICLE_TIMING_SUCCESS_CMD 0x85              //����G�ʱ�ɹ�Ӧ�g 
#define VEHICLE_TIMING_ERROR_CMD 0x95                //����G�ʱʧ��Ӧ�g
#define VEHICLE_HEART_SUCCESS_CMD 0x87               //����G����ɹ�Ӧ�g
#define VEHICLE_HEART_ERROR_CMD 0x97                 //����G���ʧ��Ӧ�g

//#define VEHICLE_SET_VIN_CMD 0xBF                      //����VIN����
//#define TERMINAL_SOFT_VER 01                           //01��ʼ�I��  ���256



#define NO_ENCRYPT  0x01                              //�i�I������
#define SM2_ENCRYPT  0x03                              //�i�ISM2����

#define ID_BOOL0 'D'
#define ID_BOOL1 'V'
#define ID_BOOL2 'U'

//G�Ϣ��Gͱ�־����
#define GB17691_INFO_OBD_DATA          0x01    //OBDG�Ϣ���־
#define GB17691_INFO_DATASTREAM_DATA   0x02    //�i�I���i�I��־
#define GB17691_INFO_CUSTOM_DATA       0x80    //�Զ����i�I���
#define GB17691_INFO_IMEI_DATA         0x81    //IMEI�i�I

#define GB17691_THREAD_DEALY  10               //�߳���ʱ
#define GB17691_CMD_CHECK_DEALY  (1000/GB17691_THREAD_DEALY) //��������ʱ

#define REALTIME_UPLOAD_FLAG 0xffff   //�i��ʵʱ���͵��i�I
#define RESEND_DATA_FLAG  0xfff0      //G�Ҫ�������i�I
#define SEND_DATA_FLAG    0xff00      //���ͳɹ����i�I

#define REAL_TIME_UPLOAD 0x01         //ʵʱ�ϱ�
#define SAVE_RESEND_DATA 0x02         //���油���i�I
//#define SAVE_ALARM_DATA  0x04

#define TERMINAL_FREE   0x00          //�ն��ͷ�ģʽ
#define TERMINAL_NET    0x01          //�ն�����ģʽ
//#define TERMINAL_USB    0x02

#define LOGIN_MAX_CNT    5            //5->3��¼�����i
#define LOGIN_INTERVAL   5           //��¼���60S

#define TIME_SEND_CNT    5             //5->3G�ʱ���������i
#define TIME_SEND_INTERVAL 5          //G�ʱ���ͼ��60S

#define RECORD_SEND_CNT    10             //5->3G�ʱ���������i
#define RECORD_SEND_INTERVAL 20          //G�ʱ���ͼ��60S

#define RESEND_WRITE_MAX_SIZE (0x200000)   //����G�����G�2M

#define HEART_INTERVAL 50              //G������50S


#define FRAME_CNT_PER_PACKAGE 1        //ÿ��֡���i
#if FRAME_CNT_PER_PACKAGE == 1
#define RESND_MSG_MAX_CNT  60         //���������i
#elif FRAME_CNT_PER_PACKAGE == 2
#define RESND_MSG_MAX_CNT  80
#elif FRAME_CNT_PER_PACKAGE == 3
#define RESND_MSG_MAX_CNT  90
#else
#define RESND_MSG_MAX_CNT  100
#endif
//#define RESND_MSG_MAX_CNT ((600/MSG_SEND_INTERVAL)*FRAME_CNT_PER_PACKAGE)


#define TIME_PRESET_VALUE    0        //ʱ��Ԥ��ֵ6��
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
} GB17691_terminal_state_t;          //�ն�״̬

#pragma pack(1)  //���ֽڶ���
typedef struct
{
    uint32_t timestamp;
    uint8_t  package_cnt;
    uint8_t  package_num;
    uint16_t resend_flag;
} save_resend_data_t;
#pragma pack()   //ȡ�����ֽڶ��� �ָ�ʡȱ����

////#pragma pack(1)  //���ֽڶ���
////���֧��״̬ ÿһλ�ĺ���:0=��֧��;1=֧��
//typedef struct
//{
//	uint16_t sptClst:1;               //�߻�ת�������
//	uint16_t sptHeatedclst:1;         //���ȴ߻�ת�������
//	uint16_t sptEprtsys:1;            //����ϵͳ���
//	uint16_t sptScdrairss:1;          //�s�ο���ϵͳ���
//	uint16_t sptAcsysrfgr:1;          //A/Cϵͳ��������
//	uint16_t sptEsgassens:1;          //�������G�����
//	uint16_t sptEsgassenshter:1;      //�������G�����������
//	uint16_t sptEgrvvtsys:1;          //EGRϵͳ��VVT���
//	uint16_t sptColdstartaid:1;       //����������ϵͳ���
//	uint16_t sptBoostpresctrlsys:1;   //��ѹѹ������ϵͳ
//	uint16_t sptDpt:1;                //DPF���
//	uint16_t sptScrnox:1;             //ѡ��GԴ߻���ԭϵͳ(SCR)��NOX������
//	uint16_t sptNmhccvtnclst:1;       //NMHC�����߻������
//	uint16_t sptMisfire:1;            //ʧ�g���
//	uint16_t sptFuelsys:1;            //ȼ��ϵͳ���
//	uint16_t sptCphscpnt:1;           //�ۺϳɷּ��
//}GB17691_OBD_dntSpt_t;

////��Ͼ�G�״̬ ÿһλ�ĺ���:0=������ɻ��߲�֧��;1=����δ���
//typedef struct
//{
//	uint16_t rdyClst:1;                //�߻�ת�������
//	uint16_t rdyHeatedclst:1;          //���ȴ߻�ת�������
//	uint16_t rdyEprtsys:1;             //����ϵͳ���
//	uint16_t rdyScdrairss:1;           //�s�ο���ϵͳ���
//	uint16_t rdyAcsysrfgr:1;           //A/Cϵͳ��������
//	uint16_t rdyEsgassens:1;           //�������G�����
//	uint16_t rdyEsgassenshter:1;       //�������G�����������
//	uint16_t rdyEgrvvtsys:1;           //EGRϵͳ��VVT���
//	uint16_t rdyColdstartaid:1;        //����������ϵͳ���
//	uint16_t rdyBoostpresctrlsys:1;    //��ѹѹ������ϵͳ
//	uint16_t rdyDpt:1;                 //DPF���
//	uint16_t rdyScrnox:1;              //ѡ��GԴ߻���ԭϵͳ(SCR)��NOX������
//	uint16_t rdyNmhccvtnclst:1;        //NMHC�����߻������
//	uint16_t rdyMisfire:1;             //ʧ�g���
//	uint16_t rdyFuelsys:1;             //ȼ��ϵͳ���
//	uint16_t rdyCphscpnt:1;            //�ۺϳɷּ��
//}GB17691_OBD_dntReady_t;

////IUPRֵ���ѹȼʽ������
//typedef struct
//{
//	uint16_t iuprOBDcond ;           //OBD������s��GG���i
//	uint16_t iuprLgncntr;            //��gѭ�����i��
//	uint16_t iuprHccatcomp;          //NMHC�߻��������ɼ��i
//	uint16_t iuprHccatcond;          //NMHC�߻���������s��GG���i
//	uint16_t iuprNcatcomp;           //NOX/SCR�߻������������s���i
//	uint16_t iuprNcatcond;           //NOx/SCR�߻���������s��GG���i
//	uint16_t iuprNadscomp;           //NOx�����������������s���i
//	uint16_t iuprNadscond;           //NOx������������s��GG���i
//	uint16_t iuprPmcomp;             //PM�i����������������s���i
//	uint16_t iuprPmcond;             //PM�i����������s��GG���i
//	uint16_t iuprEgscomp;            //�������G�������ɼ��Ӽ��i
//	uint16_t iuprEgscond;            //�������G�������GG���s��GG���i
//	uint16_t iuprEgrcomp;            //EGR��/��VVT���������s���i
//	uint16_t iuprEgrcond;            //EGR��/��VVT������s��GG���i
//	uint16_t iuprBpcomp;             //���iѹ�����������s���i
//	uint16_t iuprBpcond;             //��ѹѹ��������s��GG���i
//	uint16_t iuprFuelcomp;           //ȼ�ϼ��������s���i
//	uint16_t iuprFuelcond;           //ȼ�ϼ�����s��GG���i

//}GB17691_OBD_IUPR_t;

//typedef struct
//{
//	uint32_t faultCode1;            //���ϴ���1
//	uint32_t faultCode2;            //���ϴ���2
//  uint32_t faultCode3;            //���ϴ���3
//	uint32_t faultCode4;            //���ϴ���4
//	uint32_t faultCode5;            //���ϴ���5
//}GB17691_OBD_faultCodelist_t;

//typedef  struct
//{
//  uint8_t obdDntPtc;                          //obd���G���
//  uint8_t MIL_state;                          //MIL״̬
//  GB17691_OBD_dntSpt_t dntSptStatus;          //���֧��״̬
//  GB17691_OBD_dntReady_t dntReadyStatus;      //��Ͼ�G�״̬
//  uint8_t VIN[17];                            //����ʶ�g��
//  uint8_t softCbrtNum[18];                    //��s�궨ʶ�g��
//  uint8_t CVN[18];                            //�궨��֤��
//  GB17691_OBD_IUPR_t iupr;                    //IUPRֵ
//  uint8_t faultCodeSum;                       //���������i
//  GB17691_OBD_faultCodelist_t faultCodelist;  // ������G�Ϣ�G��
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
//  uint16_t speed;                            //����
//  uint8_t  atmoPres;                         //����ѹ��
//  uint8_t  engineTorq;                       //������Ť��
//  uint8_t  fricTorq;                         //Ħ��Ť��
//  uint16_t engineRev;                        //������ת��
//  uint16_t enginefuelFlow;                   //������ȼ������
//  uint16_t upstSensValue;                    //SCR����NOX���G�����ֵ
//  uint16_t dnstSensValue;                    //SCR����NOx���G�����ֵ
//  uint8_t  reagAllowance;                    //��Ӧ������
//	uint16_t airInflow;                        //������
//	uint16_t inLetTemp;                        //SCR����¶�
//	uint16_t outLetTemp;                       //SCR�����¶�
//	uint16_t diffPres;                         //DPFѹ��
//	uint8_t coolantTemp;                       //��������ȴҺ�¶�
//	uint8_t liquidLevel;                       //����Һλ
//	GB17691_locationStatus_t locationStatus;   //��λ״̬
//	uint8_t Ing[4];                            //����
//	uint8_t Lat[4];                            //γ��
//	uint32_t mileage;                          //�ۼ����
//}GB17691_datastream_data_t;

//typedef  struct
//{
//  uint16_t datalen;                          //�Զ���G�Ϣ�峤��
//  uint8_t  calcLoad;                         //���i����
//  uint16_t DiffTemp;                         //DPF����
//  uint8_t  NOxInvertEff;                     //NOxת��G���
//	uint8_t  elecCDISatus;                     //����ȵ�g״̬
//	uint8_t  fuleInjectSatus;                  //ȼ������״̬
//	uint16_t ASL;                              //����
//}GB17691_custom_data_t;

//#pragma pack()   //ȡ�����ֽڶ��� �ָ�ʡȱ����

//�������ȼ�
#define GB17691_TASK_PRIO		4
//�����ջ��G�
#define GB17691_STK_SIZE 	1024

uint8_t EvalBCC_FromBytes(uint8_t* Bytes, uint16_t len);   //���G���
void GB17691_CMD_analysis(uint8_t *pdata, uint16_t len);   //GB17691�������
void login_send(void);                                     //���͵���
void logout_send(void);                                    //���͵ǳ�
void GB17691_CMD_check(void);                              //������
void GB17691_task(void const *pvParameters);               //GB17691����
//void GB17691_task(void);                                 //GB17691����
void GB17691_init(void);                                   //GB17691��ʼ��
void GB17691_task_init(void);
void GB17691CheckCmd(void);
uint8_t get_lkt4305_state(void);
void LKT4305_Key_Init(void);
void Get_SM2id(void);
uint8_t get_big_heart_flag(void);
uint8_t get_login_state(void);
uint8_t* read_msg_from_flash(uint32_t* addr, uint32_t* resend_info_addr,uint8_t flash_area);


u16 get_AIKELAN_datapack(uint8_t  *p_datastream);//���������ݴ��
void GB17691_AIKELAN_CheckCmd(void);//����������У��
#endif


