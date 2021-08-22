#ifndef _GB17691_CAN_GPS_H
#define _GB17691_CAN_GPS_H
#include "sys.h"
#include "stdio.h"
#include <string.h>
#include "CAN.h"


#define _SWAP16(x)   ((x<<8)&0xff00)|((x>>8)&0x00ff)  //16bit�ָߵ��ֽڽ���
#define _SWAP32(x)     (x>>24)|((x>>8)&0x0000ff00)|((x<<8)&0x00ff0000)|((x<<24)&0xff000000) //32bit˫��

//#pragma pack(1)  //���ֽڶ���
//////���֧��״̬ ÿһλ�ĺ���:0=��֧��;1=֧��
////typedef struct
////{
////	uint16_t sptClst:1;               //�߻�ת�������
////	uint16_t sptHeatedclst:1;         //���ȴ߻�ת�������
////	uint16_t sptEprtsys:1;            //����ϵͳ���
////	uint16_t sptScdrairss:1;          //���ο���ϵͳ���
////	uint16_t sptAcsysrfgr:1;          //A/Cϵͳ��������
////	uint16_t sptEsgassens:1;          //�������������
////	uint16_t sptEsgassenshter:1;      //�������������������
////	uint16_t sptEgrvvtsys:1;          //EGRϵͳ��VVT���
////	uint16_t sptColdstartaid:1;       //����������ϵͳ���
////	uint16_t sptBoostpresctrlsys:1;   //��ѹѹ������ϵͳ
////	uint16_t sptDpt:1;                //DPF���
////	uint16_t sptScrnox:1;             //ѡ���Դ߻���ԭϵͳ(SCR)��NOX������
////	uint16_t sptNmhccvtnclst:1;       //NMHC�����߻������
////	uint16_t sptMisfire:1;            //ʧ����
////	uint16_t sptFuelsys:1;            //ȼ��ϵͳ���
////	uint16_t sptCphscpnt:1;           //�ۺϳɷּ��
////}OBD_dntSpt_t;

//////��Ͼ���״̬ ÿһλ�ĺ���:0=������ɻ��߲�֧��;1=����δ���
////typedef struct
////{
////	uint16_t rdyClst:1;                //�߻�ת�������
////	uint16_t rdyHeatedclst:1;          //���ȴ߻�ת�������
////	uint16_t rdyEprtsys:1;             //����ϵͳ���
////	uint16_t rdyScdrairss:1;           //���ο���ϵͳ���
////	uint16_t rdyAcsysrfgr:1;           //A/Cϵͳ��������
////	uint16_t rdyEsgassens:1;           //�������������
////	uint16_t rdyEsgassenshter:1;       //�������������������
////	uint16_t rdyEgrvvtsys:1;           //EGRϵͳ��VVT���
////	uint16_t rdyColdstartaid:1;        //����������ϵͳ���
////	uint16_t rdyBoostpresctrlsys:1;    //��ѹѹ������ϵͳ
////	uint16_t rdyDpt:1;                 //DPF���
////	uint16_t rdyScrnox:1;              //ѡ���Դ߻���ԭϵͳ(SCR)��NOX������
////	uint16_t rdyNmhccvtnclst:1;        //NMHC�����߻������
////	uint16_t rdyMisfire:1;             //ʧ����
////	uint16_t rdyFuelsys:1;             //ȼ��ϵͳ���
////	uint16_t rdyCphscpnt:1;            //�ۺϳɷּ��
////}OBD_dntReady_t;

//////IUPRֵ���ѹȼʽ������
////typedef struct
////{
////	uint16_t iuprOBDcond ;           //OBD����������м���
////	uint16_t iuprLgncntr;            //���ѭ��������
////	uint16_t iuprHccatcomp;          //NMHC�߻��������ɼ���
////	uint16_t iuprHccatcond;          //NMHC�߻�������������м���
////	uint16_t iuprNcatcomp;           //NOX/SCR�߻�����������������
////	uint16_t iuprNcatcond;           //NOx/SCR�߻�������������м���
////	uint16_t iuprNadscomp;           //NOx����������������������
////	uint16_t iuprNadscond;           //NOx����������������м���
////	uint16_t iuprPmcomp;             //PM�����������������������
////	uint16_t iuprPmcond;             //PM����������������м���
////	uint16_t iuprEgscomp;            //���������������ɼ��Ӽ���
////	uint16_t iuprEgscond;            //������������������������м���
////	uint16_t iuprEgrcomp;            //EGR��/��VVT��������������
////	uint16_t iuprEgrcond;            //EGR��/��VVT����������м���
////	uint16_t iuprBpcomp;             //����ѹ����������������
////	uint16_t iuprBpcond;             //��ѹѹ������������м���
////	uint16_t iuprFuelcomp;           //ȼ�ϼ�������������
////	uint16_t iuprFuelcond;           //ȼ�ϼ���������м���
////}OBD_IUPR_t;

//////typedef struct
//////{
//////	uint32_t faultCode1;            //���ϴ���1
//////	uint32_t faultCode2;            //���ϴ���2
//////  uint32_t faultCode3;            //���ϴ���3
//////	uint32_t faultCode4;            //���ϴ���4
//////	uint32_t faultCode5;            //���ϴ���5
//////}OBD_faultCodelist_t;

////typedef struct
////{
////	uint32_t faultCode[50];            //���ϴ���1
////}OBD_faultCodelist_t;

////typedef  struct
////{
////  uint8_t obdDntPtc;                          //obd���Э��
////  uint8_t MIL_state;                          //MIL״̬
////  OBD_dntSpt_t dntSptStatus;                  //���֧��״̬
////  OBD_dntReady_t dntReadyStatus;              //��Ͼ���״̬
////  uint8_t VIN[17];                            //����ʶ����
////  uint8_t softCbrtNum[18];                    //�����궨ʶ���
////  uint8_t CVN[18];                            //�궨��֤��
////  OBD_IUPR_t iupr;                            //IUPRֵ
////  uint8_t faultCodeSum;                       //����������
////  OBD_faultCodelist_t faultCodelist;           // ��������Ϣ�б�
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
////  uint16_t speed;                            //����
////  uint8_t  atmoPres;                         //����ѹ��
////  uint8_t  engineTorq;                       //������Ť��
////  uint8_t  fricTorq;                         //Ħ��Ť��
////  uint16_t engineRev;                        //������ת��
////  uint16_t enginefuelFlow;                   //������ȼ������
////  uint16_t upstSensValue;                    //SCR����NOX���������ֵ
////  uint16_t dnstSensValue;                    //SCR����NOx���������ֵ
////  uint8_t  reagAllowance;                    //��Ӧ������
////	uint16_t airInflow;                        //������
////	uint16_t inLetTemp;                        //SCR����¶�
////	uint16_t outLetTemp;                       //SCR�����¶�
////	uint16_t diffPres;                         //DPFѹ��
////	uint8_t coolantTemp;                       //��������ȴҺ�¶�
////	uint8_t liquidLevel;                       //����Һλ
//////	GB17691_locationStatus_t locationStatus;   //��λ״̬
//////	uint8_t Ing[4];                            //����
//////	uint8_t Lat[4];                            //γ��
////	uint32_t mileage;                          //�ۼ����
////}datastream_data_t;

////typedef  struct
////{
////  uint16_t datalen;                          //�Զ�����Ϣ�峤��
////  uint8_t  calcLoad;                         //��������
////  uint16_t DiffTemp;                         //DPF����
////  uint8_t  NOxInvertEff;                     //NOxת��Ч��
////	uint8_t  elecCDISatus;                     //����ȵ��״̬
////	uint8_t  fuleInjectSatus;                  //ȼ������״̬
////	uint16_t ASL;                              //����
////}custom_data_t;
//////#pragma pack()   //ȡ�����ֽڶ��� �ָ�ʡȱ����


////#pragma pack(1)  //���ֽڶ���
////���֧��״̬ ÿһλ�ĺ���:0=��֧��;1=֧��
//typedef struct
//{
//	uint16_t sptClst:1;               //�߻�ת�������
//	uint16_t sptHeatedclst:1;         //���ȴ߻�ת�������
//	uint16_t sptEprtsys:1;            //����ϵͳ���
//	uint16_t sptScdrairss:1;          //���ο���ϵͳ���
//	uint16_t sptAcsysrfgr:1;          //A/Cϵͳ��������
//	uint16_t sptEsgassens:1;          //�������������
//	uint16_t sptEsgassenshter:1;      //�������������������
//	uint16_t sptEgrvvtsys:1;          //EGRϵͳ��VVT���
//	uint16_t sptColdstartaid:1;       //����������ϵͳ���
//	uint16_t sptBoostpresctrlsys:1;   //��ѹѹ������ϵͳ
//	uint16_t sptDpt:1;                //DPF���
//	uint16_t sptScrnox:1;             //ѡ���Դ߻���ԭϵͳ(SCR)��NOX������
//	uint16_t sptNmhccvtnclst:1;       //NMHC�����߻������
//	uint16_t sptMisfire:1;            //ʧ����
//	uint16_t sptFuelsys:1;            //ȼ��ϵͳ���
//	uint16_t sptCphscpnt:1;           //�ۺϳɷּ��
//}GB17691_OBD_dntSpt_t;

////��Ͼ���״̬ ÿһλ�ĺ���:0=������ɻ��߲�֧��;1=����δ���
//typedef struct
//{
//	uint16_t rdyClst:1;                //�߻�ת�������
//	uint16_t rdyHeatedclst:1;          //���ȴ߻�ת�������
//	uint16_t rdyEprtsys:1;             //����ϵͳ���
//	uint16_t rdyScdrairss:1;           //���ο���ϵͳ���
//	uint16_t rdyAcsysrfgr:1;           //A/Cϵͳ��������
//	uint16_t rdyEsgassens:1;           //�������������
//	uint16_t rdyEsgassenshter:1;       //�������������������
//	uint16_t rdyEgrvvtsys:1;           //EGRϵͳ��VVT���
//	uint16_t rdyColdstartaid:1;        //����������ϵͳ���
//	uint16_t rdyBoostpresctrlsys:1;    //��ѹѹ������ϵͳ
//	uint16_t rdyDpt:1;                 //DPF���
//	uint16_t rdyScrnox:1;              //ѡ���Դ߻���ԭϵͳ(SCR)��NOX������
//	uint16_t rdyNmhccvtnclst:1;        //NMHC�����߻������
//	uint16_t rdyMisfire:1;             //ʧ����
//	uint16_t rdyFuelsys:1;             //ȼ��ϵͳ���
//	uint16_t rdyCphscpnt:1;            //�ۺϳɷּ��
//}GB17691_OBD_dntReady_t;

////IUPRֵ���ѹȼʽ������
//typedef struct
//{
//	uint16_t iuprOBDcond ;           //OBD����������м���
//	uint16_t iuprLgncntr;            //���ѭ��������
//	uint16_t iuprHccatcomp;          //NMHC�߻��������ɼ���
//	uint16_t iuprHccatcond;          //NMHC�߻�������������м���
//	uint16_t iuprNcatcomp;           //NOX/SCR�߻�����������������
//	uint16_t iuprNcatcond;           //NOx/SCR�߻�������������м���
//	uint16_t iuprNadscomp;           //NOx����������������������
//	uint16_t iuprNadscond;           //NOx����������������м���
//	uint16_t iuprPmcomp;             //PM�����������������������
//	uint16_t iuprPmcond;             //PM����������������м���
//	uint16_t iuprEgscomp;            //���������������ɼ��Ӽ���
//	uint16_t iuprEgscond;            //������������������������м���
//	uint16_t iuprEgrcomp;            //EGR��/��VVT��������������
//	uint16_t iuprEgrcond;            //EGR��/��VVT����������м���
//	uint16_t iuprBpcomp;             //����ѹ����������������
//	uint16_t iuprBpcond;             //��ѹѹ������������м���
//	uint16_t iuprFuelcomp;           //ȼ�ϼ�������������
//	uint16_t iuprFuelcond;           //ȼ�ϼ���������м���
//}GB17691_OBD_IUPR_t;

////typedef struct
////{
////	uint32_t faultCode1;            //���ϴ���1
////	uint32_t faultCode2;            //���ϴ���2
////  uint32_t faultCode3;            //���ϴ���3
////	uint32_t faultCode4;            //���ϴ���4
////	uint32_t faultCode5;            //���ϴ���5
////}GB17691_OBD_faultCodelist_t;

//typedef struct
//{
//	uint32_t faultCode[50];            //���ϴ������50��
//}GB17691_OBD_faultCodelist_t;

//typedef  struct
//{
//  uint8_t obdDntPtc;                          //obd���Э��
//  uint8_t MIL_state;                          //MIL״̬
//  GB17691_OBD_dntSpt_t dntSptStatus;          //���֧��״̬
//  GB17691_OBD_dntReady_t dntReadyStatus;      //��Ͼ���״̬
//  uint8_t VIN[17];                            //����ʶ����
//  uint8_t softCbrtNum[18];                    //�����궨ʶ���
//  uint8_t CVN[18];                            //�궨��֤��
//  GB17691_OBD_IUPR_t iupr;                    //IUPRֵ
//  uint8_t faultCodeSum;                       //����������
//  //GB17691_OBD_faultCodelist_t faultCodelist;  // ��������Ϣ�б�
//	 uint8_t faultCode[25][4];            // ��������Ϣ�б�
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
//  uint16_t upstSensValue;                    //SCR����NOX���������ֵ
//  uint16_t dnstSensValue;                    //SCR����NOx���������ֵ
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
//  //uint32_t Ing;                              //����
//	//uint32_t Lat;                              //γ��
//	uint32_t mileage;                          //�ۼ����
//}GB17691_datastream_data_t;

//typedef  struct
//{
//  uint16_t datalen;                          //�Զ�����Ϣ�峤��
//  uint8_t  calcLoad;                         //��������
//  uint16_t DiffTemp;                         //DPF����
//  uint8_t  NOxInvertEff;                     //NOxת��Ч��
//	uint8_t  elecCDISatus;                     //����ȵ��״̬
//	uint8_t  fuleInjectSatus;                  //ȼ������״̬
//	uint16_t ASL;                              //����
//	uint16_t fuelrate;                         //ȼ����������  20190318_add
//}GB17691_custom_data_t;
//#pragma pack()   //ȡ�����ֽڶ��� �ָ�ʡȱ����

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

