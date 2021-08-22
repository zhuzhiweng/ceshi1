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
	uint16_t sptClst:1;               //�߻�ת�������
	uint16_t sptHeatedclst:1;         //���ȴ߻�ת�������
	uint16_t sptEprtsys:1;            //����ϵͳ���
	uint16_t sptScdrairss:1;          //���ο���ϵͳ���
	uint16_t sptAcsysrfgr:1;          //A/Cϵͳ��������
	uint16_t sptEsgassens:1;          //�������������
	uint16_t sptEsgassenshter:1;      //�������������������
	uint16_t sptEgrvvtsys:1;          //EGRϵͳ��VVT���
	uint16_t sptColdstartaid:1;       //����������ϵͳ���
	uint16_t sptBoostpresctrlsys:1;   //��ѹѹ������ϵͳ
	uint16_t sptDpt:1;                //DPF���
	uint16_t sptScrnox:1;             //ѡ���Դ߻���ԭϵͳ(SCR)��NOX������
	uint16_t sptNmhccvtnclst:1;       //NMHC�����߻������
	uint16_t sptMisfire:1;            //ʧ����
	uint16_t sptFuelsys:1;            //ȼ��ϵͳ���
	uint16_t sptCphscpnt:1;           //�ۺϳɷּ��
}OBD_dntSpt_t;

//��Ͼ���״̬ ÿһλ�ĺ���:0=������ɻ��߲�֧��;1=����δ���
typedef struct
{
	uint16_t rdyClst:1;                //�߻�ת�������
	uint16_t rdyHeatedclst:1;          //���ȴ߻�ת�������
	uint16_t rdyEprtsys:1;             //����ϵͳ���
	uint16_t rdyScdrairss:1;           //���ο���ϵͳ���
	uint16_t rdyAcsysrfgr:1;           //A/Cϵͳ��������
	uint16_t rdyEsgassens:1;           //�������������
	uint16_t rdyEsgassenshter:1;       //�������������������
	uint16_t rdyEgrvvtsys:1;           //EGRϵͳ��VVT���
	uint16_t rdyColdstartaid:1;        //����������ϵͳ���
	uint16_t rdyBoostpresctrlsys:1;    //��ѹѹ������ϵͳ
	uint16_t rdyDpt:1;                 //DPF���
	uint16_t rdyScrnox:1;              //ѡ���Դ߻���ԭϵͳ(SCR)��NOX������
	uint16_t rdyNmhccvtnclst:1;        //NMHC�����߻������
	uint16_t rdyMisfire:1;             //ʧ����
	uint16_t rdyFuelsys:1;             //ȼ��ϵͳ���
	uint16_t rdyCphscpnt:1;            //�ۺϳɷּ��
}OBD_dntReady_t;

//IUPRֵ���ѹȼʽ������
typedef struct
{
	uint16_t iuprOBDcond ;           //OBD����������м���
	uint16_t iuprLgncntr;            //���ѭ��������
	uint16_t iuprHccatcomp;          //NMHC�߻��������ɼ���
	uint16_t iuprHccatcond;          //NMHC�߻�������������м���
	uint16_t iuprNcatcomp;           //NOX/SCR�߻�����������������
	uint16_t iuprNcatcond;           //NOx/SCR�߻�������������м���
	uint16_t iuprNadscomp;           //NOx����������������������
	uint16_t iuprNadscond;           //NOx����������������м���
	uint16_t iuprPmcomp;             //PM�����������������������
	uint16_t iuprPmcond;             //PM����������������м���
	uint16_t iuprEgscomp;            //���������������ɼ��Ӽ���
	uint16_t iuprEgscond;            //������������������������м���
	uint16_t iuprEgrcomp;            //EGR��/��VVT��������������
	uint16_t iuprEgrcond;            //EGR��/��VVT����������м���
	uint16_t iuprBpcomp;             //����ѹ����������������
	uint16_t iuprBpcond;             //��ѹѹ������������м���
	uint16_t iuprFuelcomp;           //ȼ�ϼ�������������
	uint16_t iuprFuelcond;           //ȼ�ϼ���������м���
}OBD_IUPR_t;


typedef  struct
{
	uint32_t mileage;                           //�ۼ����
	uint16_t speed;                             //����
	uint8_t liquidLevel;                        //����Һλ
  uint8_t softCbrtNum[18];                    //����궨ʶ���
  uint8_t CVN[18];                            //�궨��֤��
  uint8_t VIN[17];                            //����ʶ����
}OBD_Vehicle_Infor_t;


typedef  struct
{
	 uint8_t obdDntPtc;                          //obd���Э��
	 uint8_t MIL_state;                          //MIL״̬
	 OBD_dntSpt_t dntSptStatus;                  //���֧��״̬
	 OBD_dntReady_t dntReadyStatus;              //��Ͼ���״̬
   OBD_IUPR_t iupr;                            //IUPRֵ
   uint8_t faultCodeSum;                       //����������
   uint8_t faultCode[25][4];  // ��������Ϣ�б�
}OBD_Diagnosis_Infor_t;


typedef  struct
{
  uint16_t engineRev;                        //������ת��
  uint16_t enginefuelFlow;                   //������ȼ������ ----
	uint16_t airInflow;                        //������----
	uint8_t  atmoPres;                         //����ѹ��
  uint8_t  engineTorq;                       //������Ť��----
  uint8_t  fricTorq;                         //Ħ��Ť��
	uint8_t coolantTemp;                       //��������ȴҺ�¶�
  uint8_t  calcLoad;                         //���㸺��
	uint8_t		model_Torq;												//Ť��ģʽ--�Ϻ�
	uint8_t		position_pedal;												//����̤��	--�Ϻ�
	uint32_t		totalfuel;												//���ͺ�--�Ϻ�
	
}OBD_Engine_Infor_t;

typedef  struct
{
	uint16_t inLetTemp;                        //SCR����¶�
	uint16_t outLetTemp;                       //SCR�����¶�
  uint16_t upstSensValue;                    //SCR����NOX���G�����ֵ
  uint16_t dnstSensValue;                    //SCR����NOx���G�����ֵ
	uint16_t diffPres;                         //DPFѹ��
  uint16_t DiffTemp;                         //DPF����
  uint8_t  NOxInvertEff;                     //NOxת��G���
	uint8_t  elecCDISatus;                     //����ȵ�g״̬
	uint8_t  fuleInjectSatus;                  //ȼ��������״̬
	uint16_t fuelrate;                 			 	 //ȼ����������
  uint8_t  reagAllowance;                    //��Ӧ������
  uint8_t  CatalystTemp;                    //�������¶�
  uint32_t	DosingReag;												//���ص�ǰ������--�Ϻ�
  uint32_t	TotalReagCom;												//������������--�Ϻ�
	uint16_t  PMdata;																		//������Ũ�� --֣��
	uint16_t PM_lightabs;												//������ϵ��
	uint16_t PM_opaticy;														//��͸���
}OBD_Aftertreatment_Infor_t;
    
typedef struct
{
	u8 total_frame_no;				//��֡���ܰ��� 
	u8 valid_byte_no;					//��Ч�ֽڳ��� 
	u8 remaining_space;				//ʣ����ֽڴ�С
	u8 success_no;						//�Ƿ��������
	u8 items_no;							//�˶������09ģʽ������Ŀ��  ��01ģʽ����A�ֽ�
	//������03��07ģʽʹ�õ�
	u8 DTC_NO;						//�˶������03����07ģʽ���ǹ�����

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
extern u8  judge_is_ASCII(char * p,u16 P_NO);			//P ָ��  P_NOΪ�ֽ���  //����0 Ϊ��ȫ��asckII  1 ��ʾȫ����ascII

uint8_t GetProtocolNum(void);
#endif




















