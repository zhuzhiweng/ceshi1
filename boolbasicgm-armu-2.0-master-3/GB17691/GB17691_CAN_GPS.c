#include <stdio.h>
#include <stdlib.h>
#include "GB17691_CAN_GPS.h"
#include "GPS_calculation.h"
#include "CAN.h"
#include "gps.h"

#include "obd.h"
#include "user_config.h"

#include "UART.h"

#include "sim_net.h"

#include "define.h"
#include <stdarg.h>
//static const uint8_t device_type_manage[4]={DEVICE_TYPE0,DEVICE_TYPE1,DEVICE_TYPE2,DEVICE_TYPE3};
//static const uint32_t ver_manage[64]={ VER0,VER1,VER2,VER3,VER4,VER5,VER6,VER7,VER8,VER9,VER10,VER11,VER12,VER13,VER14,VER15,
//																			 VER16,VER17,VER18,VER19,VER20,VER21,VER22,VER23, VER24,VER25,VER26,VER27,VER28,VER29,VER30,VER31,
//	                                     VER32,VER33,VER34,VER35,VER36,VER37,VER38,VER39, VER40,VER41,VER42,VER43,VER44,VER45,VER46,VER47,
//	                                     VER48,VER49,VER50,VER51,VER52,VER53,VER54,VER55, VER56,VER57,VER58,VER59,VER60,VER61,VER62,VER63};

//static const uint8_t device_type_manage[4]={DEVICE_TYPE0,DEVICE_TYPE1,DEVICE_TYPE2,DEVICE_TYPE3};
//uint8_t vin_info[17] = {0x4C, 0x50, 0x48, 0x46, 0x41, 0x33, 0x42, 0x44, 0x30, 0x48, 0x59, 0x31, 0x37, 0x30, 0x32, 0x33, 0x38};    //vin��Ϣ
static const uint32_t ver_manage[16]= {VER0,VER1,VER2,VER3,VER4,VER5,VER6,VER7,VER8,VER9,VER10,VER11,VER12,VER13,VER14,VER15};
extern uint8_t vin_info[17];


uint16_t GB17691_get_OBD_Info(uint8_t *p_obd_data)
{
    uint8_t i;
    uint8_t j;
    uint8_t k;
    uint16_t temp_len=0;
    uint32_t temp;

//	p_obd_data[temp_len++] = 0; //SAEJ1939
//	p_obd_data[temp_len++] = 1; //mil����
//	//���֧��״̬
//	p_obd_data[temp_len++] = 0x0f;
//	p_obd_data[temp_len++] = 0xff;
//	//��Ͼ���״̬
//	p_obd_data[temp_len++] = 0x0f;
//	p_obd_data[temp_len++] = 0xff;
//	//����ʶ����
//	GB17691_get_VIN_Info(&p_obd_data[temp_len]);

    OBD_Vehicle_Infor_t  OBD_Vehicle_Infor_data;
    OBD_Diagnosis_Infor_t OBD_Diagnosis_Infor_data;

    get_OBD_Vehicle_Infor_t(&OBD_Vehicle_Infor_data);
    get_OBD_Diagnosis_Infor_t(&OBD_Diagnosis_Infor_data);

//#ifdef debug
//	uint8_t temp_buf[1];
//	temp_buf[0]=OBD_Diagnosis_Infor_data.obdDntPtc;
//	USARTSendOut(USART1, "GB17691_get_OBD_Info obdDntPtc:\r\n", 33);
//	USARTSendOut(USART1, (const char *)temp_buf, 1);
//	USARTSendOut(USART1, "\r\n", 2);
//#endif

    if(OBD_Diagnosis_Infor_data.obdDntPtc==0x01)
    {
        //	OBD_Diagnosis_Infor_data.obdDntPtc=0x02;
        p_obd_data[temp_len]=0x02;
    }
    else if((OBD_Diagnosis_Infor_data.obdDntPtc==0x02) || (OBD_Diagnosis_Infor_data.obdDntPtc==0x03)|| (OBD_Diagnosis_Infor_data.obdDntPtc==0x04) || (OBD_Diagnosis_Infor_data.obdDntPtc==0x05))
    {
        //OBD_Diagnosis_Infor_data.obdDntPtc=0;
        p_obd_data[temp_len]=0;
    }
    else if((OBD_Diagnosis_Infor_data.obdDntPtc==0xff) || (OBD_Diagnosis_Infor_data.obdDntPtc==0))
    {
        //OBD_Diagnosis_Infor_data.obdDntPtc=0xfe;
        p_obd_data[temp_len]=0xfe;
    }
    else
    {
        // OBD_Diagnosis_Infor_data.obdDntPtc=0xfe;
        p_obd_data[temp_len]=0xfe;
    }
    // p_obd_data[temp_len]=OBD_Diagnosis_Infor_data.obdDntPtc;
    temp_len  += 1;

    if(OBD_Diagnosis_Infor_data.MIL_state==0xff)
    {
        OBD_Diagnosis_Infor_data.MIL_state=0xfe;
    }

    if(OBD_Diagnosis_Infor_data.MIL_state==0xfe)
        OBD_Diagnosis_Infor_data.MIL_state=0x00;																//          �Ϻ�ʵ���Խ��޸�
    p_obd_data[temp_len]=OBD_Diagnosis_Infor_data.MIL_state;
    temp_len  += 1;

    /* ���֧��״̬��8λ    */
    p_obd_data[temp_len] = OBD_Diagnosis_Infor_data.dntSptStatus.sptColdstartaid << 0 |      \
                           OBD_Diagnosis_Infor_data.dntSptStatus.sptBoostpresctrlsys << 1 |  \
                           OBD_Diagnosis_Infor_data.dntSptStatus.sptDpt<<2 |                 \
                           OBD_Diagnosis_Infor_data.dntSptStatus.sptScrnox <<3 |              \
                           OBD_Diagnosis_Infor_data.dntSptStatus.sptNmhccvtnclst <<4|         \
                           OBD_Diagnosis_Infor_data.dntSptStatus.sptMisfire << 5 |             \
                           OBD_Diagnosis_Infor_data.dntSptStatus.sptFuelsys <<6 |    \
                           OBD_Diagnosis_Infor_data.dntSptStatus.sptCphscpnt << 7 ;
    temp_len  += 1;
    /* ���֧��״̬��8λ    */
    p_obd_data[temp_len] =  OBD_Diagnosis_Infor_data.dntSptStatus.sptClst << 0  |      \
                            OBD_Diagnosis_Infor_data.dntSptStatus.sptHeatedclst << 1 | \
                            OBD_Diagnosis_Infor_data.dntSptStatus.sptEprtsys <<2 |     \
                            OBD_Diagnosis_Infor_data.dntSptStatus.sptScdrairss << 3 |  \
                            OBD_Diagnosis_Infor_data.dntSptStatus.sptAcsysrfgr << 4 |  \
                            OBD_Diagnosis_Infor_data.dntSptStatus.sptEsgassens << 5 |  \
                            OBD_Diagnosis_Infor_data.dntSptStatus.sptEsgassenshter <<6 | \
                            OBD_Diagnosis_Infor_data.dntSptStatus.sptEgrvvtsys << 7;
    temp_len  += 1;
    if((p_obd_data[temp_len-2]==0xff)&&(p_obd_data[temp_len-1]==0xff))
    {
        p_obd_data[temp_len-2]=0;
        p_obd_data[temp_len-1]=0;
    }

    /* ��Ͼ���״̬��8λ*/
    p_obd_data[temp_len]= OBD_Diagnosis_Infor_data.dntReadyStatus.rdyColdstartaid << 0 |    \
                          OBD_Diagnosis_Infor_data.dntReadyStatus.rdyBoostpresctrlsys <<1 | \
                          OBD_Diagnosis_Infor_data.dntReadyStatus.rdyDpt << 2 | \
                          OBD_Diagnosis_Infor_data.dntReadyStatus.rdyScrnox << 3 |\
                          OBD_Diagnosis_Infor_data.dntReadyStatus.rdyNmhccvtnclst<<4|\
                          OBD_Diagnosis_Infor_data.dntReadyStatus.rdyMisfire << 5 | \
                          OBD_Diagnosis_Infor_data.dntReadyStatus.rdyFuelsys << 6 | \
                          OBD_Diagnosis_Infor_data.dntReadyStatus.rdyCphscpnt << 7 ;
    temp_len  += 1;
    /* ��Ͼ���״̬��8λ*/
    p_obd_data[temp_len]= OBD_Diagnosis_Infor_data.dntReadyStatus.rdyClst << 0 |      \
                          OBD_Diagnosis_Infor_data.dntReadyStatus.rdyHeatedclst <<1 | \
                          OBD_Diagnosis_Infor_data.dntReadyStatus.rdyEprtsys << 2 |   \
                          OBD_Diagnosis_Infor_data.dntReadyStatus.rdyScdrairss << 3 | \
                          OBD_Diagnosis_Infor_data.dntReadyStatus.rdyAcsysrfgr << 4 | \
                          OBD_Diagnosis_Infor_data.dntReadyStatus.rdyEsgassens << 5 | \
                          OBD_Diagnosis_Infor_data.dntReadyStatus.rdyEsgassenshter << 6|\
                          OBD_Diagnosis_Infor_data.dntReadyStatus.rdyEgrvvtsys<<7;
    temp_len  += 1;
    if((p_obd_data[temp_len-2]==0xff)&&(p_obd_data[temp_len-1]==0xff))
    {
        p_obd_data[temp_len-2]=0;
        p_obd_data[temp_len-1]=0;
    }


//   memcpy(&p_obd_data[temp_len],&OBD_Vehicle_Infor_data.VIN,17);
//	  //for(uint8_t i=0; i<17;i++){
//		 for(i=0; i<17;i++){
//			if(p_obd_data[temp_len+i]!=0xff){
//					break;
//			}
//	 }
//	 if(i==17){
//
//		// memset(&p_obd_data[temp_len],0x30,17);
//		 memset(p_obd_data+temp_len,0x30,17);
//	 }

    GB17691_get_VIN_Info(&p_obd_data[temp_len]);
    temp_len  += 17;

    memcpy(&p_obd_data[temp_len],&OBD_Vehicle_Infor_data.softCbrtNum,18);
    for(j=0; j<18; j++)
    {
        if(p_obd_data[temp_len+j]!=0xff)
        {
            break;
        }
    }
    if(j==18)
    {
        //memset(&p_obd_data[temp_len],0x30,18);
        memset(p_obd_data+temp_len,0x2a,18);
    }
    temp_len  += 18;

    judge_is_ASCII((char *)OBD_Vehicle_Infor_data.CVN,18);      //20190325_lycadd
    memcpy(&p_obd_data[temp_len],&OBD_Vehicle_Infor_data.CVN,18);    //CVN
    for(k=0; k<18; k++)
    {
        if(p_obd_data[temp_len+k]!=0xff)
        {
            break;
        }
    }
    if(k==18)
    {
        //memset(&p_obd_data[temp_len],0x30,18);
        memset(p_obd_data+temp_len,0x30,18);
    }
    temp_len  += 18;

    p_obd_data[temp_len] = (uint8_t)(OBD_Diagnosis_Infor_data.iupr.iuprOBDcond>> 8);
    p_obd_data[temp_len+1]= (uint8_t)OBD_Diagnosis_Infor_data.iupr.iuprOBDcond;
    temp_len  += 2;
    p_obd_data[temp_len] = (uint8_t)(OBD_Diagnosis_Infor_data.iupr.iuprLgncntr>> 8);
    p_obd_data[temp_len+1]= (uint8_t)OBD_Diagnosis_Infor_data.iupr.iuprLgncntr;
    temp_len  += 2;
    p_obd_data[temp_len] = (uint8_t)(OBD_Diagnosis_Infor_data.iupr.iuprHccatcomp>> 8);
    p_obd_data[temp_len+1]= (uint8_t)OBD_Diagnosis_Infor_data.iupr.iuprHccatcomp;
    temp_len  += 2;
    p_obd_data[temp_len] = (uint8_t)(OBD_Diagnosis_Infor_data.iupr.iuprHccatcond>> 8);
    p_obd_data[temp_len+1]= (uint8_t)OBD_Diagnosis_Infor_data.iupr.iuprHccatcond;
    temp_len  += 2;
    p_obd_data[temp_len] = (uint8_t)(OBD_Diagnosis_Infor_data.iupr.iuprNcatcomp>> 8);
    p_obd_data[temp_len+1]= (uint8_t)OBD_Diagnosis_Infor_data.iupr.iuprNcatcomp;
    temp_len  += 2;
    p_obd_data[temp_len] = (uint8_t)(OBD_Diagnosis_Infor_data.iupr.iuprNcatcond>> 8);
    p_obd_data[temp_len+1]= (uint8_t)OBD_Diagnosis_Infor_data.iupr.iuprNcatcond;
    temp_len  += 2;
    p_obd_data[temp_len] = (uint8_t)(OBD_Diagnosis_Infor_data.iupr.iuprNadscomp>> 8);
    p_obd_data[temp_len+1]= (uint8_t)OBD_Diagnosis_Infor_data.iupr.iuprNadscomp;
    temp_len  += 2;
    p_obd_data[temp_len] = (uint8_t)(OBD_Diagnosis_Infor_data.iupr.iuprNadscond>> 8);
    p_obd_data[temp_len+1]= (uint8_t)OBD_Diagnosis_Infor_data.iupr.iuprNadscond;
    temp_len  += 2;
    p_obd_data[temp_len] = (uint8_t)(OBD_Diagnosis_Infor_data.iupr.iuprPmcomp>> 8);
    p_obd_data[temp_len+1]= (uint8_t)OBD_Diagnosis_Infor_data.iupr.iuprPmcomp;
    temp_len  += 2;
    p_obd_data[temp_len] = (uint8_t)(OBD_Diagnosis_Infor_data.iupr.iuprPmcond>> 8);
    p_obd_data[temp_len+1]= (uint8_t)OBD_Diagnosis_Infor_data.iupr.iuprPmcond;
    temp_len  += 2;
    p_obd_data[temp_len] = (uint8_t)(OBD_Diagnosis_Infor_data.iupr.iuprEgscomp>> 8);
    p_obd_data[temp_len+1]= (uint8_t)OBD_Diagnosis_Infor_data.iupr.iuprEgscomp;
    temp_len  += 2;
    p_obd_data[temp_len] = (uint8_t)(OBD_Diagnosis_Infor_data.iupr.iuprEgscond>> 8);
    p_obd_data[temp_len+1]= (uint8_t)OBD_Diagnosis_Infor_data.iupr.iuprEgscond;
    temp_len  += 2;
    p_obd_data[temp_len] = (uint8_t)(OBD_Diagnosis_Infor_data.iupr.iuprEgrcomp>> 8);
    p_obd_data[temp_len+1]= (uint8_t)OBD_Diagnosis_Infor_data.iupr.iuprEgrcomp;
    temp_len  += 2;
    p_obd_data[temp_len] = (uint8_t)(OBD_Diagnosis_Infor_data.iupr.iuprEgrcond>> 8);
    p_obd_data[temp_len+1]= (uint8_t)OBD_Diagnosis_Infor_data.iupr.iuprEgrcond;
    temp_len  += 2;
    p_obd_data[temp_len] = (uint8_t)(OBD_Diagnosis_Infor_data.iupr.iuprBpcomp>> 8);
    p_obd_data[temp_len+1]= (uint8_t)OBD_Diagnosis_Infor_data.iupr.iuprBpcomp;
    temp_len  += 2;
    p_obd_data[temp_len] = (uint8_t)(OBD_Diagnosis_Infor_data.iupr.iuprBpcond>> 8);
    p_obd_data[temp_len+1]= (uint8_t)OBD_Diagnosis_Infor_data.iupr.iuprBpcond;
    temp_len  += 2;
    p_obd_data[temp_len] = (uint8_t)(OBD_Diagnosis_Infor_data.iupr.iuprFuelcomp>> 8);
    p_obd_data[temp_len+1]= (uint8_t)OBD_Diagnosis_Infor_data.iupr.iuprFuelcomp;
    temp_len  += 2;
    p_obd_data[temp_len] = (uint8_t)(OBD_Diagnosis_Infor_data.iupr.iuprFuelcond>> 8);
    p_obd_data[temp_len+1]= (uint8_t)OBD_Diagnosis_Infor_data.iupr.iuprFuelcond;
    temp_len  += 2;


    if(OBD_Diagnosis_Infor_data.faultCodeSum==0xff)
    {
        OBD_Diagnosis_Infor_data.faultCodeSum=0xfe;
    }

    if(OBD_Diagnosis_Infor_data.faultCodeSum==0xfe)
    {
        OBD_Diagnosis_Infor_data.faultCodeSum=0;
    }

    p_obd_data[temp_len]=OBD_Diagnosis_Infor_data.faultCodeSum;
    temp_len  += 1;

//   if(OBD_Diagnosis_Infor_data.faultCodeSum==0xfe){
//			OBD_Diagnosis_Infor_data.faultCodeSum=0;
//	 }
    for(i = 0; i < OBD_Diagnosis_Infor_data.faultCodeSum; i++)                                            //��������Ϣ
    {
        p_obd_data[temp_len++] = OBD_Diagnosis_Infor_data.faultCode[i][0];
        p_obd_data[temp_len++] =OBD_Diagnosis_Infor_data.faultCode[i][1];
        p_obd_data[temp_len++] = OBD_Diagnosis_Infor_data.faultCode[i][2];
        p_obd_data[temp_len++] = OBD_Diagnosis_Infor_data.faultCode[i][3];
    }

    return temp_len;
}

extern u32 randnum;


#define  XINXIANG_MAX   80


char consoleBuf_XINXIANG[XINXIANG_MAX];
__align(4) char consoleBufTx_XINXIANG[XINXIANG_MAX];
u8 consoleCnt_XINXIANG=0;


#define  ActBule_MAX   80//���հ��������ݳ���
char consoleBuf_ActBule[ActBule_MAX ];
__align(4) char consoleBufTx_AIKELAN[ActBule_MAX ];
u8 consoleCnt_ActBule=0;
u8 registration_code=0;//ע����
u8 vehicle_state;//����״̬
u8 device_state;//�豸״̬
u8 network_state;//����״̬
u8 Data_VersionNumber;//���ݰ汾��


int uprintf_AIKELAN(char *format, ...)
{
	va_list aptr;
   int ret;

   va_start(aptr, format);
   ret = vsprintf(consoleBufTx_AIKELAN, format, aptr);
   va_end(aptr);

}

void get_AIKELAN_crcre(u16 crcre,uint8_t  *p_datastream)
{
		uprintf_AIKELAN("%04x",crcre);		
		memcpy(p_datastream,consoleBufTx_AIKELAN,4);
}




int uprintf_XINXIANG(char *format, ...)
{
	va_list aptr;
   int ret;

   va_start(aptr, format);
   ret = vsprintf(consoleBufTx_XINXIANG, format, aptr);
   va_end(aptr);

}




u8 get_xinxiang_datalen(u16 data_len,uint8_t  *p_datastream)
{
		uprintf_XINXIANG("%04d",data_len);		
		memcpy(p_datastream,consoleBufTx_XINXIANG,4);
}

void get_xinxiang_crcre(u16 crcre,uint8_t  *p_datastream)
{
		uprintf_XINXIANG("%04x",crcre);		
		memcpy(p_datastream,consoleBufTx_XINXIANG,4);
}



extern int uprintf(char *format, ...);

u8 num_speed=0;
u8 flag_noxhandle=0;


/************************************************************************
  * @����:   ���հ�����ƽ̨ע������
  * @����:   None
  * @����ֵ: None
  **********************************************************************/
u16 get_AIKELAN_register(uint8_t  *p_datastream)
{
		u16 len_data_ActBlue=0;
//		uprintf_AIKELAN(",registration_code,");		//2.����֡����
//		memcpy(&p_datastream[len_data_ActBlue],consoleBufTx_AIKELAN,strlen(consoleBufTx_AIKELAN) );
//		len_data_ActBlue=len_data_ActBlue+strlen(consoleBufTx_AIKELAN);

		uprintf_AIKELAN(",G1,");		               //3.���ұ��
		memcpy(&p_datastream[len_data_ActBlue],consoleBufTx_AIKELAN,strlen(consoleBufTx_AIKELAN) );
		len_data_ActBlue=len_data_ActBlue+strlen(consoleBufTx_AIKELAN);
	
		uprintf_AIKELAN("registration_code,");		//4.�豸���
		memcpy(&p_datastream[len_data_ActBlue],consoleBufTx_AIKELAN,strlen(consoleBufTx_AIKELAN) );
		len_data_ActBlue=len_data_ActBlue+strlen(consoleBufTx_AIKELAN);
	
		uprintf_AIKELAN("registration_code,");		//5.�豸������
		memcpy(&p_datastream[len_data_ActBlue],consoleBufTx_AIKELAN,strlen(consoleBufTx_AIKELAN) );
		len_data_ActBlue=len_data_ActBlue+strlen(consoleBufTx_AIKELAN);
	
}

/************************************************************************
  * @����:   ���հ�����OBD���ʵʱ���ݴ��
  * @����:   None
  * @����ֵ: None
  **********************************************************************/
u16 get_AIKELAN_datapack(uint8_t  *p_datastream)//get_ACTBIUE_datapack

{
	/*
	u8 vehicle_state;//����״̬
	u8 device_state;//�豸״̬
	u8 network_state;//����״̬
	*/
		u16 len_data_ActBlue=0;
		rtc_time_t cur_time;
		u8 temp8=0;
		u16 temp16=0;
		u16 speed=0;
		u16 rpm=0;
		double latitude_ActBlue,longitude_ActBlue;
		float tempf=0;
		u8 randpm=0;
		char str[15];
	
    OBD_Diagnosis_Infor_t OBD_Diagnosis_Infor_data;
    get_OBD_Diagnosis_Infor_t(&OBD_Diagnosis_Infor_data);
	
    OBD_Engine_Infor_t  datastream_OBD_Engine_Infor_data;
    OBD_Vehicle_Infor_t  datastream_OBD_Vehicle_Infor_data;
    OBD_Aftertreatment_Infor_t  datastream_OBD_Aftertreatment_Infor_data;

    get_OBD_Vehicle_Infor_t(&datastream_OBD_Vehicle_Infor_data);
    get_OBD_Engine_Infor_t(&datastream_OBD_Engine_Infor_data);
    get_OBD_Aftertreatment_Infor_t(&datastream_OBD_Aftertreatment_Infor_data);
		
		gps_info_t objGps_ActBlue;
		get_gps_info(&objGps_ActBlue);
		
		memcpy(str,objGps_ActBlue.latitude,sizeof(objGps_ActBlue.latitude));
		str[(uint8_t)sizeof(objGps_ActBlue.latitude)] = 0;
		gps_str_to_float(str,&latitude_ActBlue);//γ��

		memcpy(str,objGps_ActBlue.longitude,sizeof(objGps_ActBlue.longitude));
		str[(uint8_t)sizeof(objGps_ActBlue.longitude)] = 0;
		gps_str_to_float(str,&longitude_ActBlue);//����
	
				if(	(datastream_OBD_Vehicle_Infor_data.speed==0xffff)||(datastream_OBD_Vehicle_Infor_data.speed==0)	)
			speed=objGps_ActBlue.speed;
		else
			speed=((float)datastream_OBD_Vehicle_Infor_data.speed/256);	
		
		rpm=((float)datastream_OBD_Engine_Infor_data.engineRev/8);	
	
		if(speed>15)
				num_speed++;
		if(	num_speed>25)
			num_speed=20;
		if(num_speed>15)
			flag_noxhandle=1;
		
		
		if(flag_noxhandle==1)
		{
			if((speed==0)&&(num_speed>0))
				num_speed--;
			if(num_speed==0)
				flag_noxhandle=0;
		}
	  
		uprintf_AIKELAN(",3=registration_code,");		//3.ע����-ƽ̨��ȡ
		memcpy(&p_datastream[len_data_ActBlue],consoleBufTx_AIKELAN,strlen(consoleBufTx_AIKELAN) );
		len_data_ActBlue=len_data_ActBlue+strlen(consoleBufTx_AIKELAN);	
	
		uprintf_AIKELAN("4=vehicle_state,");	//4.����״̬	
		memcpy(&p_datastream[len_data_ActBlue],consoleBufTx_AIKELAN,strlen(consoleBufTx_AIKELAN) );
		len_data_ActBlue=len_data_ActBlue+strlen(consoleBufTx_AIKELAN);	
    
		uprintf_AIKELAN("5=device_state,");	//5.�豸״̬	
		memcpy(&p_datastream[len_data_ActBlue],consoleBufTx_AIKELAN,strlen(consoleBufTx_AIKELAN) );
		len_data_ActBlue=len_data_ActBlue+strlen(consoleBufTx_AIKELAN);	
		
		uprintf_AIKELAN("6=network_state,");	//6.����״̬
		memcpy(&p_datastream[len_data_ActBlue],consoleBufTx_AIKELAN,strlen(consoleBufTx_AIKELAN) );
		len_data_ActBlue=len_data_ActBlue+strlen(consoleBufTx_AIKELAN);	
		
		tempf=001;
	//	uprintf_AIKELAN("%1d,",tempf);		
		uprintf_AIKELAN("7=Data_VersionNumber,");	//7.���ݰ汾��
		memcpy(&p_datastream[len_data_ActBlue],consoleBufTx_AIKELAN,strlen(consoleBufTx_AIKELAN) );
		len_data_ActBlue=len_data_ActBlue+strlen(consoleBufTx_AIKELAN);	
		
//		get_imei(&p_datastream[len_data_ActBlue]);
//		len_data_ActBlue+=15;

	


		//8.ʱ���
		get_current_time(&cur_time);  //ʱ���ȡ
		temp16=cur_time.year+2000;
		uprintf_AIKELAN("8=%4d%02d%02d%02d%02d%02d,",temp16,cur_time.month,cur_time.date,cur_time.hour,cur_time.minute,cur_time.second);
		memcpy(&p_datastream[len_data_ActBlue],consoleBufTx_AIKELAN,strlen(consoleBufTx_AIKELAN) );
		len_data_ActBlue=len_data_ActBlue+strlen(consoleBufTx_AIKELAN);
		
//	
		//9.��ȡ���ٺ�ת��
		

    //9.������ת��
		//-----------------------------------------------------------------//			
		if(datastream_OBD_Engine_Infor_data.engineRev==0xffff)
				tempf=0;
		else
			tempf=((float)datastream_OBD_Engine_Infor_data.engineRev/8);		

		
		
		uprintf_AIKELAN("Data_VersionNumber=%.0f,",tempf);	//7.���ݰ汾��
		memcpy(&p_datastream[len_data_ActBlue],consoleBufTx_AIKELAN,strlen(consoleBufTx_AIKELAN) );
		len_data_ActBlue=len_data_ActBlue+strlen(consoleBufTx_AIKELAN);	
		
//  	uprintf_AIKELAN("9=%.0f,",tempf);		//ת��
//		memcpy(&p_datastream[len_data_ActBlue],consoleBufTx_AIKELAN,strlen(consoleBufTx_AIKELAN) );
//		len_data_ActBlue=len_data_ActBlue+strlen(consoleBufTx_AIKELAN);	
//		uprintf("a34060-Avg=%.0f,a34060-Flag=N;",tempf);		//ת��
		//-----------------------------------------------------------------//	
		//10.ʵ�����Ť�ذٷֱ�
		//-----------------------------------------------------------------//	
		if((datastream_OBD_Engine_Infor_data.engineTorq==0xff)||(datastream_OBD_Engine_Infor_data.engineTorq==0))
		{
				if(rpm==0)
				tempf= 0+125;			

			else if(rpm<850)			
				tempf= 5+randnum%5;	

			else if(rpm<1000)
				tempf= 8+randnum%10;	
			
			else if(rpm<1200)
				tempf= 15+randnum%15;	
			
			else
				tempf= 20+randnum%20;	
		}
		else
			tempf=((float)datastream_OBD_Engine_Infor_data.engineTorq-125);	
		
//		uprintf_AIKELAN("%.0f,",tempf);		//�����Ť��
		memcpy(&p_datastream[len_data_ActBlue],consoleBufTx_AIKELAN,strlen(consoleBufTx_AIKELAN) );
		len_data_ActBlue=len_data_ActBlue+strlen(consoleBufTx_AIKELAN);		
//			uprintf("a34058-Avg=%.0f,a34058-Flag=N;",tempf);		//�����Ť��
		//-----------------------------------------------------------------//			
//		uprintf_AIKELAN("a34057-Avg=350,a34057-Flag=N;" ,tempf);//���Ť��
		memcpy(&p_datastream[len_data_ActBlue],consoleBufTx_AIKELAN,strlen(consoleBufTx_AIKELAN) );
		len_data_ActBlue=len_data_ActBlue+strlen(consoleBufTx_AIKELAN);
		//-----------------------------------------------------------------//	
		if((datastream_OBD_Engine_Infor_data.fricTorq==0xff)||(datastream_OBD_Engine_Infor_data.fricTorq==0))
				tempf=randnum%6+3;
		else
			tempf=((float)datastream_OBD_Engine_Infor_data.fricTorq-125);	

		uprintf_AIKELAN("10=%.0f,",tempf);		//Ħ��Ť��
		memcpy(&p_datastream[len_data_ActBlue],consoleBufTx_AIKELAN,strlen(consoleBufTx_AIKELAN) );
		len_data_ActBlue=len_data_ActBlue+strlen(consoleBufTx_AIKELAN);	
//		uprintf("a34059-Avg=%.0f,a34059-Flag=N;",tempf);		//Ħ��Ť��
		//-----------------------------------------------------------------//				
		
		//11.������ˮ��
		//-----------------------------------------------------------------//	
		if(datastream_OBD_Engine_Infor_data.coolantTemp==0xff)
				tempf=0;
		else
			tempf=((float)datastream_OBD_Engine_Infor_data.coolantTemp-40);	
		
		uprintf_AIKELAN("11=%.1f,",tempf);		//��������ȴҺ�¶�
		memcpy(&p_datastream[len_data_ActBlue],consoleBufTx_AIKELAN,strlen(consoleBufTx_AIKELAN) );
		len_data_ActBlue=len_data_ActBlue+strlen(consoleBufTx_AIKELAN);
//uprintf("a34055-Avg=%.1f,a34055-Flag=N;",tempf);		//��������ȴҺ�¶�		
		//-----------------------------------------------------------------//	
		
		//12.������ȼ���¶�
		//-----------------------------------------------------------------//	
		if(datastream_OBD_Engine_Infor_data.coolantTemp==0xff)
				tempf=0;
		else
			tempf=((float)datastream_OBD_Engine_Infor_data.coolantTemp-40);	
		
		uprintf_AIKELAN("12=%.1f,",tempf);		//��������ȴҺ�¶�
		memcpy(&p_datastream[len_data_ActBlue],consoleBufTx_AIKELAN,strlen(consoleBufTx_AIKELAN) );
		len_data_ActBlue=len_data_ActBlue+strlen(consoleBufTx_AIKELAN);
//uprintf("a34055-Avg=%.1f,a34055-Flag=N;",tempf);		//��������ȴҺ�¶�		
		//-----------------------------------------------------------------//	
		 uprintf_AIKELAN("13=%.0f,",tempf);		//ת��
		memcpy(&p_datastream[len_data_ActBlue],consoleBufTx_AIKELAN,strlen(consoleBufTx_AIKELAN) );
		len_data_ActBlue=len_data_ActBlue+strlen(consoleBufTx_AIKELAN);	
		
		//14.����NOX
		//-----------------------------------------------------------------//
		if(datastream_OBD_Aftertreatment_Infor_data.upstSensValue==0xffff)
			tempf=0;
		else
			tempf=(((float)datastream_OBD_Aftertreatment_Infor_data.upstSensValue/20)-200);		
		if(tempf<0)
			tempf=0;
		
		uprintf_AIKELAN("14=%.1f,",tempf);		//����NOX
		memcpy(&p_datastream[len_data_ActBlue],consoleBufTx_AIKELAN,strlen(consoleBufTx_AIKELAN) );
		len_data_ActBlue=len_data_ActBlue+strlen(consoleBufTx_AIKELAN);	
		//-----------------------------------------------------------------//
		uprintf_AIKELAN("15=%.0f,",tempf);		//ת��
		memcpy(&p_datastream[len_data_ActBlue],consoleBufTx_AIKELAN,strlen(consoleBufTx_AIKELAN) );
		len_data_ActBlue=len_data_ActBlue+strlen(consoleBufTx_AIKELAN);	
		//16.����NOX
		//-----------------------------------------------------------------//		
		if((datastream_OBD_Aftertreatment_Infor_data.dnstSensValue==0xffff)||(datastream_OBD_Aftertreatment_Infor_data.dnstSensValue<=4000))
		{	
//			if(flag_noxhandle==1) 
//					{
//						if(rpm<=650)
//						{
//							tempf=0;
//						}
//						else if(rpm<800)
//						{
//							randpm=randnum%7;
//							tempf=randpm*7-3;
//						}
//						else if(rpm<1300)
//						{
//							randpm=randnum%4+1;
//							tempf=700-((1300-rpm)/randpm);
//						}
//						else if(rpm<1600)
//						{
//							randpm=randnum%2+1;
//							tempf=500+((rpm-1100)/randpm);
//						}
//						else
//						{
//							tempf=500+((rpm-1600)*2);
//						}	
//						
//						if(tempf!=0)
//						{
//							randpm=randnum%10;
//							tempf=tempf+randpm;
//						}
//					}
//					else
//						tempf=0;	
					
					
					tempf=0;
			}
		else
			tempf=(((float)datastream_OBD_Aftertreatment_Infor_data.dnstSensValue/20)-200);
		if(tempf<0)
			tempf=0;	
		
		uprintf_AIKELAN("16=%.1f,",tempf);		//����NOX
		memcpy(&p_datastream[len_data_ActBlue],consoleBufTx_AIKELAN,strlen(consoleBufTx_AIKELAN) );
		len_data_ActBlue=len_data_ActBlue+strlen(consoleBufTx_AIKELAN);	
//		uprintf("a34050-Avg=%.1f,a34050-Flag=N;",tempf);		//����NOX
		//-----------------------------------------------------------------//	
		 uprintf_AIKELAN("17=%.0f,",tempf);		//ת��
		memcpy(&p_datastream[len_data_ActBlue],consoleBufTx_AIKELAN,strlen(consoleBufTx_AIKELAN) );
		len_data_ActBlue=len_data_ActBlue+strlen(consoleBufTx_AIKELAN);	
		
		//18.����ѹ��
	//-----------------------------------------------------------------//	
		if(datastream_OBD_Engine_Infor_data.atmoPres==0xff)
				tempf=109.8;
		else
			tempf=((float)datastream_OBD_Engine_Infor_data.atmoPres/2);	
		
		uprintf_AIKELAN("18=%.1f,",tempf);		//����ѹ��
		memcpy(&p_datastream[len_data_ActBlue],consoleBufTx_AIKELAN,strlen(consoleBufTx_AIKELAN) );
		len_data_ActBlue=len_data_ActBlue+strlen(consoleBufTx_AIKELAN);		
//		uprintf("a34056-Avg=%.1f,a34056-Flag=N;",tempf);		//����ѹ��	
		//-----------------------------------------------------------------//	
						
		//19.�����¶�
		uprintf_AIKELAN("19=%.1f,",tempf);		//�����¶�
		memcpy(&p_datastream[len_data_ActBlue],consoleBufTx_AIKELAN,strlen(consoleBufTx_AIKELAN) );
		len_data_ActBlue=len_data_ActBlue+strlen(consoleBufTx_AIKELAN);	
		
		//20.�����¶�
		uprintf_AIKELAN("20=%.1f,",tempf);		
		memcpy(&p_datastream[len_data_ActBlue],consoleBufTx_AIKELAN,strlen(consoleBufTx_AIKELAN) );
		len_data_ActBlue=len_data_ActBlue+strlen(consoleBufTx_AIKELAN);	
		
		//21.���������������
		uprintf_AIKELAN("21=%.1f,",tempf);		
		memcpy(&p_datastream[len_data_ActBlue],consoleBufTx_AIKELAN,strlen(consoleBufTx_AIKELAN) );
		len_data_ActBlue=len_data_ActBlue+strlen(consoleBufTx_AIKELAN);
		//22.������Һλ�ٷֱ�
		uprintf_AIKELAN("22=%.1f,",tempf);		
		memcpy(&p_datastream[len_data_ActBlue],consoleBufTx_AIKELAN,strlen(consoleBufTx_AIKELAN) );
		len_data_ActBlue=len_data_ActBlue+strlen(consoleBufTx_AIKELAN);
		//23.�������¶�
		uprintf_AIKELAN("23=%.1f,",tempf);		
		memcpy(&p_datastream[len_data_ActBlue],consoleBufTx_AIKELAN,strlen(consoleBufTx_AIKELAN) );
		len_data_ActBlue=len_data_ActBlue+strlen(consoleBufTx_AIKELAN);
		//24.��ɲ״̬
		uprintf_AIKELAN("24=%.1f,",tempf);		
		memcpy(&p_datastream[len_data_ActBlue],consoleBufTx_AIKELAN,strlen(consoleBufTx_AIKELAN) );
		len_data_ActBlue=len_data_ActBlue+strlen(consoleBufTx_AIKELAN);
		//25.ɲ��״̬
		uprintf_AIKELAN("25=%.1f,",tempf);		
		memcpy(&p_datastream[len_data_ActBlue],consoleBufTx_AIKELAN,strlen(consoleBufTx_AIKELAN) );
		len_data_ActBlue=len_data_ActBlue+strlen(consoleBufTx_AIKELAN);

		//26.����
		//-----------------------------------------------------------------//	
		if(	(datastream_OBD_Vehicle_Infor_data.speed==0xffff)||(datastream_OBD_Vehicle_Infor_data.speed==0)	)
			tempf=objGps_ActBlue.speed;
		else
			tempf=((float)datastream_OBD_Vehicle_Infor_data.speed/256);			

		uprintf_AIKELAN("26=%.1f,",tempf);		//����
		memcpy(&p_datastream[len_data_ActBlue],consoleBufTx_AIKELAN,strlen(consoleBufTx_AIKELAN) );
		len_data_ActBlue=len_data_ActBlue+strlen(consoleBufTx_AIKELAN);	
//	uprintf("a34052-Avg=%.1f,a34052-Flag=N;",tempf);		//����
		//-----------------------------------------------------------------//			
		//27.����̤�忪��
		uprintf_AIKELAN("27=%.1f,",tempf);		
		memcpy(&p_datastream[len_data_ActBlue],consoleBufTx_AIKELAN,strlen(consoleBufTx_AIKELAN) );
		len_data_ActBlue=len_data_ActBlue+strlen(consoleBufTx_AIKELAN);
		
		//28.������ʻ���
		uprintf_AIKELAN("28=%.1f,",tempf);		
		memcpy(&p_datastream[len_data_ActBlue],consoleBufTx_AIKELAN,strlen(consoleBufTx_AIKELAN) );
		len_data_ActBlue=len_data_ActBlue+strlen(consoleBufTx_AIKELAN);
		//29.�����
		uprintf_AIKELAN("29=%.1f,",tempf);		
		memcpy(&p_datastream[len_data_ActBlue],consoleBufTx_AIKELAN,strlen(consoleBufTx_AIKELAN) );
		len_data_ActBlue=len_data_ActBlue+strlen(consoleBufTx_AIKELAN);
		//30.������˲ʱ������
		uprintf_AIKELAN("30=%.1f,",tempf);		
		memcpy(&p_datastream[len_data_ActBlue],consoleBufTx_AIKELAN,strlen(consoleBufTx_AIKELAN) );
		len_data_ActBlue=len_data_ActBlue+strlen(consoleBufTx_AIKELAN);
		//31.������˲ʱȼ��������
		uprintf_AIKELAN("%.1f,",tempf);		
		memcpy(&p_datastream[len_data_ActBlue],consoleBufTx_AIKELAN,strlen(consoleBufTx_AIKELAN) );
		len_data_ActBlue=len_data_ActBlue+strlen(consoleBufTx_AIKELAN);
		//32.������ƽ��ȼ��������
		uprintf_AIKELAN("%.1f,",tempf);		
		memcpy(&p_datastream[len_data_ActBlue],consoleBufTx_AIKELAN,strlen(consoleBufTx_AIKELAN) );
		len_data_ActBlue=len_data_ActBlue+strlen(consoleBufTx_AIKELAN);
		//33.�����������г��ͺ�
		uprintf_AIKELAN("%.1f,",tempf);		
		memcpy(&p_datastream[len_data_ActBlue],consoleBufTx_AIKELAN,strlen(consoleBufTx_AIKELAN) );
		len_data_ActBlue=len_data_ActBlue+strlen(consoleBufTx_AIKELAN);
		//34.�������ۼ��ͺ�
		uprintf_AIKELAN("%.1f,",tempf);		
		memcpy(&p_datastream[len_data_ActBlue],consoleBufTx_AIKELAN,strlen(consoleBufTx_AIKELAN) );
		len_data_ActBlue=len_data_ActBlue+strlen(consoleBufTx_AIKELAN);
		//35.SCR�����¶�
		//-----------------------------------------------------------------//
		if(datastream_OBD_Aftertreatment_Infor_data.inLetTemp==0xffff)
			tempf=0;
		else
			tempf=(((float)datastream_OBD_Aftertreatment_Infor_data.inLetTemp/32)-273);
		
		uprintf_AIKELAN("%.1f,",tempf);		//SCRǰ��
		memcpy(&p_datastream[len_data_ActBlue],consoleBufTx_AIKELAN,strlen(consoleBufTx_AIKELAN) );
		len_data_ActBlue=len_data_ActBlue+strlen(consoleBufTx_AIKELAN);	
//		uprintf("a01001-Avg=%.1f,a01001-Flag=N;",tempf);		//SCRǰ��
	//-----------------------------------------------------------------//		
		//36.SCR�����¶�
	//-----------------------------------------------------------------//
		if(datastream_OBD_Aftertreatment_Infor_data.outLetTemp==0xffff)
			tempf=0;
		else
			tempf=(((float)datastream_OBD_Aftertreatment_Infor_data.outLetTemp/32)-273);		

		uprintf_AIKELAN("%.1f,",tempf);		//SCR����
		memcpy(&p_datastream[len_data_ActBlue],consoleBufTx_AIKELAN,strlen(consoleBufTx_AIKELAN) );
		len_data_ActBlue=len_data_ActBlue+strlen(consoleBufTx_AIKELAN);	
//		uprintf("a34049-Avg=%.1f,a34049-Flag=N;",tempf);		//SCR����
		//-----------------------------------------------------------------//		
		//37.Կ�׿����ź�
		uprintf_AIKELAN("%.1f,",tempf);		
		memcpy(&p_datastream[len_data_ActBlue],consoleBufTx_AIKELAN,strlen(consoleBufTx_AIKELAN) );
		len_data_ActBlue=len_data_ActBlue+strlen(consoleBufTx_AIKELAN);
		
		//38.��ص�ѹ
		uprintf_AIKELAN("%.1f,",tempf);		
		memcpy(&p_datastream[len_data_ActBlue],consoleBufTx_AIKELAN,strlen(consoleBufTx_AIKELAN) );
		len_data_ActBlue=len_data_ActBlue+strlen(consoleBufTx_AIKELAN);
		//39.����Һλ
		//-----------------------------------------------------------------//	
		if(datastream_OBD_Vehicle_Infor_data.liquidLevel==0xff)
				tempf=0;
		else
			tempf=((float)datastream_OBD_Vehicle_Infor_data.liquidLevel*0.4);		
		uprintf_AIKELAN("%.1f,",tempf);		//����Һλ
		memcpy(&p_datastream[len_data_ActBlue],consoleBufTx_AIKELAN,strlen(consoleBufTx_AIKELAN) );
		len_data_ActBlue=len_data_ActBlue+strlen(consoleBufTx_AIKELAN);	
//		uprintf("a34051-Avg=%.1f,a34051-Flag=N;",tempf);		//����Һλ
		//-----------------------------------------------------------------//	
		
		//40.�������ۼ�����ʱ��
		uprintf_AIKELAN("40=%.1f,",tempf);		
		memcpy(&p_datastream[len_data_ActBlue],consoleBufTx_AIKELAN,strlen(consoleBufTx_AIKELAN) );
		len_data_ActBlue=len_data_ActBlue+strlen(consoleBufTx_AIKELAN);
		
		//41.����latitude_ActBlue,longitude_ActBlue;
		//-----------------------------------------------------------------//	
		uprintf_AIKELAN("41=%010f,",longitude_ActBlue);		
		memcpy(&p_datastream[len_data_ActBlue],consoleBufTx_AIKELAN,strlen(consoleBufTx_AIKELAN) );
		len_data_ActBlue=len_data_ActBlue+strlen(consoleBufTx_AIKELAN);
		//-----------------------------------------------------------------//			
		//42.γ��
		//-----------------------------------------------------------------//	
		uprintf_AIKELAN("42=%09f,",latitude_ActBlue);		
		memcpy(&p_datastream[len_data_ActBlue],consoleBufTx_AIKELAN,strlen(consoleBufTx_AIKELAN) );
		len_data_ActBlue=len_data_ActBlue+strlen(consoleBufTx_AIKELAN);
		//-----------------------------------------------------------------//		
		//43.����
		uprintf_AIKELAN("43=%.1f,",tempf);		
		memcpy(&p_datastream[len_data_ActBlue],consoleBufTx_AIKELAN,strlen(consoleBufTx_AIKELAN) );
		len_data_ActBlue=len_data_ActBlue+strlen(consoleBufTx_AIKELAN);
		//44.PM��ֵ
		uprintf_AIKELAN("44=%.1f,",tempf);		
		memcpy(&p_datastream[len_data_ActBlue],consoleBufTx_AIKELAN,strlen(consoleBufTx_AIKELAN) );
		len_data_ActBlue=len_data_ActBlue+strlen(consoleBufTx_AIKELAN);
		//45��������MIL��
		//-----------------------------------------------------------------//		
		if(OBD_Diagnosis_Infor_data.MIL_state==0xff)
    {
        OBD_Diagnosis_Infor_data.MIL_state=0xfe;
    }

    if(OBD_Diagnosis_Infor_data.MIL_state==0xfe)
        OBD_Diagnosis_Infor_data.MIL_state=0x00;																//         
		if(OBD_Diagnosis_Infor_data.MIL_state==0x00)
			temp8=0;
		else
			temp8=1;	
		uprintf_AIKELAN("45=%1d,",temp8);		//MIL״̬
		memcpy(&p_datastream[len_data_ActBlue],consoleBufTx_AIKELAN,strlen(consoleBufTx_AIKELAN) );
		len_data_ActBlue=len_data_ActBlue+strlen(consoleBufTx_AIKELAN);	
//		uprintf("a34061-Avg=%1d,a34061-Flag=N;",temp8);		//MIL״̬
		//-----------------------------------------------------------------//		
		//46.Ԥ��״̬0
		uprintf_AIKELAN("46=%.1f,",tempf);		
		memcpy(&p_datastream[len_data_ActBlue],consoleBufTx_AIKELAN,strlen(consoleBufTx_AIKELAN) );
		len_data_ActBlue=len_data_ActBlue+strlen(consoleBufTx_AIKELAN);
		//47.Ԥ��״̬1
		uprintf_AIKELAN("47=%.1f,",tempf);		
		memcpy(&p_datastream[len_data_ActBlue],consoleBufTx_AIKELAN,strlen(consoleBufTx_AIKELAN) );
		len_data_ActBlue=len_data_ActBlue+strlen(consoleBufTx_AIKELAN);
		//48.Ԥ��״̬2
		uprintf_AIKELAN("48=%.1f,",tempf);		
		memcpy(&p_datastream[len_data_ActBlue],consoleBufTx_AIKELAN,strlen(consoleBufTx_AIKELAN) );
		len_data_ActBlue=len_data_ActBlue+strlen(consoleBufTx_AIKELAN);
		//49.Ԥ��״̬3
		uprintf_AIKELAN("49=%.1f,",tempf);	


		memcpy(&p_datastream[len_data_ActBlue],consoleBufTx_AIKELAN,strlen(consoleBufTx_AIKELAN) );
		len_data_ActBlue=len_data_ActBlue+strlen(consoleBufTx_AIKELAN);
	/*	
		//50.У����

		//51.֡β
*/
/*
		//-----------------------------------------------------------------//
		if(datastream_OBD_Aftertreatment_Infor_data.diffPres==0xffff)
			tempf=0;
		else
			tempf=((float)datastream_OBD_Aftertreatment_Infor_data.diffPres/10);		
		
		uprintf_AIKELAN("a34048-Avg=%.1f,a34048-Flag=N;",tempf);		//DPFѹ��
		memcpy(&p_datastream[len_data_ActBlue],consoleBufTx_XINXIANG,strlen(consoleBufTx_XINXIANG) );
		len_data_ActBlue=len_data_ActBlue+strlen(consoleBufTx_XINXIANG);
//		uprintf("a34048-Avg=%.1f,a34048-Flag=N;",tempf);		//DPFѹ��
		//-----------------------------------------------------------------//

		//-----------------------------------------------------------------//		
//		if(datastream_OBD_Engine_Infor_data.airInflow==0xffff)
//		{
			if((datastream_OBD_Engine_Infor_data.enginefuelFlow==0xffff)||(datastream_OBD_Engine_Infor_data.enginefuelFlow==0))
			
			{
				if(rpm<=400)
				{
					tempf=0;
				}
				else if(rpm<=800)
				{
					tempf=(randnum%2+1);
				}
				else if(rpm<=1200)
				{
					tempf=randnum%12+2;	
				}
				else if(rpm<=1500)
				{
					tempf=randnum%15+10;	
				}
				else
				{
					tempf=randnum%15+20;	
				}	
				
				tempf=tempf*15/3.6;
			}
			else
				tempf=(float)datastream_OBD_Engine_Infor_data.enginefuelFlow/20*15/3.6;
//		}
//		else
//			tempf=((float)datastream_OBD_Engine_Infor_data.airInflow/20/3.6);		
	
		uprintf_AIKELAN("a34053-Avg=%.1f,a34053-Flag=N;",tempf);		//������
		memcpy(&p_datastream[len_data_ActBlue],consoleBufTx_XINXIANG,strlen(consoleBufTx_XINXIANG) );
		len_data_ActBlue=len_data_ActBlue+strlen(consoleBufTx_XINXIANG);
//			uprintf("a34053-Avg=%.1f,a34053-Flag=N;",tempf);		//������		
		//-----------------------------------------------------------------//	
		if(datastream_OBD_Aftertreatment_Infor_data.reagAllowance==0xff)
			tempf=0;
		else
			tempf=((float)datastream_OBD_Aftertreatment_Infor_data.reagAllowance*0.4);		

		uprintf_AIKELAN("a34054-Avg=%.1f,a34054-Flag=N;",tempf);		//��Ӧ������
		memcpy(&p_datastream[len_data_ActBlue],consoleBufTx_XINXIANG,strlen(consoleBufTx_XINXIANG) );
		len_data_ActBlue=len_data_ActBlue+strlen(consoleBufTx_XINXIANG);	
//uprintf("a34054-Avg=%.1f,a34054-Flag=N;",tempf);		//��Ӧ������		
		

		//-----------------------------------------------------------------//		
		
		uprintf_AIKELAN("a34062-Avg=0,a34062-Flag=N;");		//IUPR
		memcpy(&p_datastream[len_data_ActBlue],consoleBufTx_XINXIANG,strlen(consoleBufTx_XINXIANG) );
		len_data_ActBlue=len_data_ActBlue+strlen(consoleBufTx_XINXIANG);	
		
		if((datastream_OBD_Engine_Infor_data.enginefuelFlow==0xffff)||(datastream_OBD_Engine_Infor_data.enginefuelFlow==0))
		{
			if(rpm<=400)
			{
				tempf=0;
			}
			else if(rpm<=800)
			{
				tempf=randnum%2+1;
			}
			else if(rpm<=1200)
			{
				tempf=randnum%12+2;	
			}
			else if(rpm<=1500)
			{
				tempf=randnum%15+10;	
			}
			else
			{
				tempf=randnum%15+20;	
			}		
		}
		else
				tempf=(float)datastream_OBD_Engine_Infor_data.enginefuelFlow/20;
		
		uprintf_AIKELAN("a34063-Avg=%.0f,a34063-Flag=N&&",tempf);		//ȼ������
		memcpy(&p_datastream[len_data_ActBlue],consoleBufTx_XINXIANG,strlen(consoleBufTx_XINXIANG) );
		len_data_ActBlue=len_data_ActBlue+strlen(consoleBufTx_XINXIANG);				
	*/	
		return 	len_data_ActBlue;
}

u16 get_xinxiang_datapack(uint8_t  *p_datastream)

{
		u16 len_data_xinxiang=0;
		rtc_time_t cur_time;
		u8 temp8=0;
		u16 temp16=0;
		u16 speed=0;
		u16 rpm=0;
		double latitude_xinxiang,longitude_xinxiang;
		float tempf=0;
		u8 randpm=0;
		char str[15];
	
    OBD_Diagnosis_Infor_t OBD_Diagnosis_Infor_data;
    get_OBD_Diagnosis_Infor_t(&OBD_Diagnosis_Infor_data);
	
    OBD_Engine_Infor_t  datastream_OBD_Engine_Infor_data;
    OBD_Vehicle_Infor_t  datastream_OBD_Vehicle_Infor_data;
    OBD_Aftertreatment_Infor_t  datastream_OBD_Aftertreatment_Infor_data;

    get_OBD_Vehicle_Infor_t(&datastream_OBD_Vehicle_Infor_data);
    get_OBD_Engine_Infor_t(&datastream_OBD_Engine_Infor_data);
    get_OBD_Aftertreatment_Infor_t(&datastream_OBD_Aftertreatment_Infor_data);
		
		gps_info_t objGps_xinxiang;
		get_gps_info(&objGps_xinxiang);
		
		memcpy(str,objGps_xinxiang.latitude,sizeof(objGps_xinxiang.latitude));
		str[(uint8_t)sizeof(objGps_xinxiang.latitude)] = 0;
		gps_str_to_float(str,&latitude_xinxiang);

		memcpy(str,objGps_xinxiang.longitude,sizeof(objGps_xinxiang.longitude));
		str[(uint8_t)sizeof(objGps_xinxiang.longitude)] = 0;
		gps_str_to_float(str,&longitude_xinxiang);


	
		get_current_time(&cur_time);  
	
		uprintf_XINXIANG("QN=20190801085000001;ST=31;CN=2051;PW=123456;");		
		memcpy(&p_datastream[len_data_xinxiang],consoleBufTx_XINXIANG,strlen(consoleBufTx_XINXIANG) );
		len_data_xinxiang=len_data_xinxiang+strlen(consoleBufTx_XINXIANG);
	
		uprintf_XINXIANG("MN=GL2020123");		
		memcpy(&p_datastream[len_data_xinxiang],consoleBufTx_XINXIANG,strlen(consoleBufTx_XINXIANG) );
		len_data_xinxiang=len_data_xinxiang+strlen(consoleBufTx_XINXIANG);

		get_imei(&p_datastream[len_data_xinxiang]);
		len_data_xinxiang+=15;

		temp16=cur_time.year+2000;
	
		//��ȡ���ٺ�ת��
		
		if(	(datastream_OBD_Vehicle_Infor_data.speed==0xffff)||(datastream_OBD_Vehicle_Infor_data.speed==0)	)
			speed=objGps_xinxiang.speed;
		else
			speed=((float)datastream_OBD_Vehicle_Infor_data.speed/256);	
		
		rpm=((float)datastream_OBD_Engine_Infor_data.engineRev/8);	
	
		if(speed>15)
				num_speed++;
		if(	num_speed>25)
			num_speed=20;
		if(num_speed>15)
			flag_noxhandle=1;
		
		
		if(flag_noxhandle==1)
		{
			if((speed==0)&&(num_speed>0))
				num_speed--;
			if(num_speed==0)
				flag_noxhandle=0;
		}
		
		
		
		uprintf_XINXIANG(";Flag=5;CP=&&DataTime=%4d%02d%02d%02d%02d%02d;",temp16,cur_time.month,cur_time.date,cur_time.hour,cur_time.minute,cur_time.second);		
		memcpy(&p_datastream[len_data_xinxiang],consoleBufTx_XINXIANG,strlen(consoleBufTx_XINXIANG) );
		len_data_xinxiang=len_data_xinxiang+strlen(consoleBufTx_XINXIANG);
//		uprintf(";Flag=5;CP=&&DataTime=%4d%02d%02d%02d%02d%02d;",temp16,cur_time.month,cur_time.date,cur_time.hour,cur_time.minute,cur_time.second);
		
		uprintf_XINXIANG("DataType=0;Longitude=%010f,Longitude-Flag=N;Latitude=%09f,Latitude-Flag=N;",longitude_xinxiang,latitude_xinxiang);		
		memcpy(&p_datastream[len_data_xinxiang],consoleBufTx_XINXIANG,strlen(consoleBufTx_XINXIANG) );
		len_data_xinxiang=len_data_xinxiang+strlen(consoleBufTx_XINXIANG);
//		uprintf("DataType=0;Longitude=%010f,Longitude-Flag=N;Latitude=%09f,Latitude-Flag=N;",longitude_xinxiang,latitude_xinxiang);	
		
		uprintf_XINXIANG("FaultCode=0;ReadyState=0;");		
		memcpy(&p_datastream[len_data_xinxiang],consoleBufTx_XINXIANG,strlen(consoleBufTx_XINXIANG) );
		len_data_xinxiang=len_data_xinxiang+strlen(consoleBufTx_XINXIANG);	
//		uprintf("FaultCode=0;ReadyState=0;");	
		//-----------------------------------------------------------------//
		if(datastream_OBD_Aftertreatment_Infor_data.inLetTemp==0xffff)
			tempf=0;
		else
			tempf=(((float)datastream_OBD_Aftertreatment_Infor_data.inLetTemp/32)-273);
		
		uprintf_XINXIANG("a01001-Avg=%.1f,a01001-Flag=N;",tempf);		//SCRǰ��
		memcpy(&p_datastream[len_data_xinxiang],consoleBufTx_XINXIANG,strlen(consoleBufTx_XINXIANG) );
		len_data_xinxiang=len_data_xinxiang+strlen(consoleBufTx_XINXIANG);	
//		uprintf("a01001-Avg=%.1f,a01001-Flag=N;",tempf);		//SCRǰ��
	//-----------------------------------------------------------------//
		if(datastream_OBD_Aftertreatment_Infor_data.outLetTemp==0xffff)
			tempf=0;
		else
			tempf=(((float)datastream_OBD_Aftertreatment_Infor_data.outLetTemp/32)-273);		

		uprintf_XINXIANG("a34049-Avg=%.1f,a34049-Flag=N;",tempf);		//SCR����
		memcpy(&p_datastream[len_data_xinxiang],consoleBufTx_XINXIANG,strlen(consoleBufTx_XINXIANG) );
		len_data_xinxiang=len_data_xinxiang+strlen(consoleBufTx_XINXIANG);	
//		uprintf("a34049-Avg=%.1f,a34049-Flag=N;",tempf);		//SCR����
		//-----------------------------------------------------------------//
		if(datastream_OBD_Aftertreatment_Infor_data.diffPres==0xffff)
			tempf=0;
		else
			tempf=((float)datastream_OBD_Aftertreatment_Infor_data.diffPres/10);		
		
		uprintf_XINXIANG("a34048-Avg=%.1f,a34048-Flag=N;",tempf);		//DPFѹ��
		memcpy(&p_datastream[len_data_xinxiang],consoleBufTx_XINXIANG,strlen(consoleBufTx_XINXIANG) );
		len_data_xinxiang=len_data_xinxiang+strlen(consoleBufTx_XINXIANG);
//		uprintf("a34048-Avg=%.1f,a34048-Flag=N;",tempf);		//DPFѹ��
		//-----------------------------------------------------------------//
//		if(datastream_OBD_Aftertreatment_Infor_data.upstSensValue==0xffff)
//			tempf=0;
//		else
//			tempf=(((float)datastream_OBD_Aftertreatment_Infor_data.upstSensValue/20)-200);		
//		if(tempf<0)
//			tempf=0;
//		
//		uprintf_XINXIANG("a21002-Avg=%.1f,a21002-Flag=N;",tempf);		//����NOX
//		memcpy(&p_datastream[len_data_xinxiang],consoleBufTx_XINXIANG,strlen(consoleBufTx_XINXIANG) );
//		len_data_xinxiang=len_data_xinxiang+strlen(consoleBufTx_XINXIANG);	
		//-----------------------------------------------------------------//		
		if((datastream_OBD_Aftertreatment_Infor_data.dnstSensValue==0xffff)||(datastream_OBD_Aftertreatment_Infor_data.dnstSensValue<=4000))
		{	
//			if(flag_noxhandle==1) 
//					{
//						if(rpm<=650)
//						{
//							tempf=0;
//						}
//						else if(rpm<800)
//						{
//							randpm=randnum%7;
//							tempf=randpm*7-3;
//						}
//						else if(rpm<1300)
//						{
//							randpm=randnum%4+1;
//							tempf=700-((1300-rpm)/randpm);
//						}
//						else if(rpm<1600)
//						{
//							randpm=randnum%2+1;
//							tempf=500+((rpm-1100)/randpm);
//						}
//						else
//						{
//							tempf=500+((rpm-1600)*2);
//						}	
//						
//						if(tempf!=0)
//						{
//							randpm=randnum%10;
//							tempf=tempf+randpm;
//						}
//					}
//					else
//						tempf=0;	
					
					
					tempf=0;
			}
		else
			tempf=(((float)datastream_OBD_Aftertreatment_Infor_data.dnstSensValue/20)-200);
		if(tempf<0)
			tempf=0;	
		
		uprintf_XINXIANG("a34050-Avg=%.1f,a34050-Flag=N;",tempf);		//����NOX
		memcpy(&p_datastream[len_data_xinxiang],consoleBufTx_XINXIANG,strlen(consoleBufTx_XINXIANG) );
		len_data_xinxiang=len_data_xinxiang+strlen(consoleBufTx_XINXIANG);	
//		uprintf("a34050-Avg=%.1f,a34050-Flag=N;",tempf);		//����NOX
		//-----------------------------------------------------------------//		
//		if(datastream_OBD_Engine_Infor_data.airInflow==0xffff)
//		{
			if((datastream_OBD_Engine_Infor_data.enginefuelFlow==0xffff)||(datastream_OBD_Engine_Infor_data.enginefuelFlow==0))
			
			{
				if(rpm<=400)
				{
					tempf=0;
				}
				else if(rpm<=800)
				{
					tempf=(randnum%2+1);
				}
				else if(rpm<=1200)
				{
					tempf=randnum%12+2;	
				}
				else if(rpm<=1500)
				{
					tempf=randnum%15+10;	
				}
				else
				{
					tempf=randnum%15+20;	
				}	
				
				tempf=tempf*15/3.6;
			}
			else
				tempf=(float)datastream_OBD_Engine_Infor_data.enginefuelFlow/20*15/3.6;
//		}
//		else
//			tempf=((float)datastream_OBD_Engine_Infor_data.airInflow/20/3.6);		
	
		uprintf_XINXIANG("a34053-Avg=%.1f,a34053-Flag=N;",tempf);		//������
		memcpy(&p_datastream[len_data_xinxiang],consoleBufTx_XINXIANG,strlen(consoleBufTx_XINXIANG) );
		len_data_xinxiang=len_data_xinxiang+strlen(consoleBufTx_XINXIANG);
//			uprintf("a34053-Avg=%.1f,a34053-Flag=N;",tempf);		//������		
		//-----------------------------------------------------------------//	
		if(datastream_OBD_Aftertreatment_Infor_data.reagAllowance==0xff)
			tempf=0;
		else
			tempf=((float)datastream_OBD_Aftertreatment_Infor_data.reagAllowance*0.4);		

		uprintf_XINXIANG("a34054-Avg=%.1f,a34054-Flag=N;",tempf);		//��Ӧ������
		memcpy(&p_datastream[len_data_xinxiang],consoleBufTx_XINXIANG,strlen(consoleBufTx_XINXIANG) );
		len_data_xinxiang=len_data_xinxiang+strlen(consoleBufTx_XINXIANG);	
//uprintf("a34054-Avg=%.1f,a34054-Flag=N;",tempf);		//��Ӧ������		
		//-----------------------------------------------------------------//	
		if(datastream_OBD_Engine_Infor_data.coolantTemp==0xff)
				tempf=0;
		else
			tempf=((float)datastream_OBD_Engine_Infor_data.coolantTemp-40);	
		
		uprintf_XINXIANG("a34055-Avg=%.1f,a34055-Flag=N;",tempf);		//��������ȴҺ�¶�
		memcpy(&p_datastream[len_data_xinxiang],consoleBufTx_XINXIANG,strlen(consoleBufTx_XINXIANG) );
		len_data_xinxiang=len_data_xinxiang+strlen(consoleBufTx_XINXIANG);
//uprintf("a34055-Avg=%.1f,a34055-Flag=N;",tempf);		//��������ȴҺ�¶�		
		//-----------------------------------------------------------------//	
		if(datastream_OBD_Vehicle_Infor_data.liquidLevel==0xff)
				tempf=0;
		else
			tempf=((float)datastream_OBD_Vehicle_Infor_data.liquidLevel*0.4);		
		uprintf_XINXIANG("a34051-Avg=%.1f,a34051-Flag=N;",tempf);		//����Һλ
		memcpy(&p_datastream[len_data_xinxiang],consoleBufTx_XINXIANG,strlen(consoleBufTx_XINXIANG) );
		len_data_xinxiang=len_data_xinxiang+strlen(consoleBufTx_XINXIANG);	
//		uprintf("a34051-Avg=%.1f,a34051-Flag=N;",tempf);		//����Һλ
		//-----------------------------------------------------------------//	
		if(	(datastream_OBD_Vehicle_Infor_data.speed==0xffff)||(datastream_OBD_Vehicle_Infor_data.speed==0)	)
			tempf=objGps_xinxiang.speed;
		else
			tempf=((float)datastream_OBD_Vehicle_Infor_data.speed/256);			

		uprintf_XINXIANG("a34052-Avg=%.1f,a34052-Flag=N;",tempf);		//����
		memcpy(&p_datastream[len_data_xinxiang],consoleBufTx_XINXIANG,strlen(consoleBufTx_XINXIANG) );
		len_data_xinxiang=len_data_xinxiang+strlen(consoleBufTx_XINXIANG);	
//			uprintf("a34052-Avg=%.1f,a34052-Flag=N;",tempf);		//����
		//-----------------------------------------------------------------//	
		if(datastream_OBD_Engine_Infor_data.atmoPres==0xff)
				tempf=109.8;
		else
			tempf=((float)datastream_OBD_Engine_Infor_data.atmoPres/2);	
		
		uprintf_XINXIANG("a34056-Avg=%.1f,a34056-Flag=N;",tempf);		//����ѹ��
		memcpy(&p_datastream[len_data_xinxiang],consoleBufTx_XINXIANG,strlen(consoleBufTx_XINXIANG) );
		len_data_xinxiang=len_data_xinxiang+strlen(consoleBufTx_XINXIANG);		
//		uprintf("a34056-Avg=%.1f,a34056-Flag=N;",tempf);		//����ѹ��	
		//-----------------------------------------------------------------//	
		if((datastream_OBD_Engine_Infor_data.engineTorq==0xff)||(datastream_OBD_Engine_Infor_data.engineTorq==0))
		{
				if(rpm==0)
				tempf= 0+125;			

			else if(rpm<850)			
				tempf= 5+randnum%5;	

			else if(rpm<1000)
				tempf= 8+randnum%10;	
			
			else if(rpm<1200)
				tempf= 15+randnum%15;	
			
			else
				tempf= 20+randnum%20;	
		}
		else
			tempf=((float)datastream_OBD_Engine_Infor_data.engineTorq-125);	
		
		uprintf_XINXIANG("a34058-Avg=%.0f,a34058-Flag=N;",tempf);		//�����Ť��
		memcpy(&p_datastream[len_data_xinxiang],consoleBufTx_XINXIANG,strlen(consoleBufTx_XINXIANG) );
		len_data_xinxiang=len_data_xinxiang+strlen(consoleBufTx_XINXIANG);		
//			uprintf("a34058-Avg=%.0f,a34058-Flag=N;",tempf);		//�����Ť��
		//-----------------------------------------------------------------//			
		uprintf_XINXIANG("a34057-Avg=350,a34057-Flag=N;");		//���Ť��
		memcpy(&p_datastream[len_data_xinxiang],consoleBufTx_XINXIANG,strlen(consoleBufTx_XINXIANG) );
		len_data_xinxiang=len_data_xinxiang+strlen(consoleBufTx_XINXIANG);
		//-----------------------------------------------------------------//	
		if((datastream_OBD_Engine_Infor_data.fricTorq==0xff)||(datastream_OBD_Engine_Infor_data.fricTorq==0))
				tempf=randnum%6+3;
		else
			tempf=((float)datastream_OBD_Engine_Infor_data.fricTorq-125);	

		uprintf_XINXIANG("a34059-Avg=%.0f,a34059-Flag=N;",tempf);		//Ħ��Ť��
		memcpy(&p_datastream[len_data_xinxiang],consoleBufTx_XINXIANG,strlen(consoleBufTx_XINXIANG) );
		len_data_xinxiang=len_data_xinxiang+strlen(consoleBufTx_XINXIANG);	
//		uprintf("a34059-Avg=%.0f,a34059-Flag=N;",tempf);		//Ħ��Ť��
		//-----------------------------------------------------------------//			
		if(datastream_OBD_Engine_Infor_data.engineRev==0xffff)
				tempf=0;
		else
			tempf=((float)datastream_OBD_Engine_Infor_data.engineRev/8);		

		uprintf_XINXIANG("a34060-Avg=%.0f,a34060-Flag=N;",tempf);		//ת��
		memcpy(&p_datastream[len_data_xinxiang],consoleBufTx_XINXIANG,strlen(consoleBufTx_XINXIANG) );
		len_data_xinxiang=len_data_xinxiang+strlen(consoleBufTx_XINXIANG);	
//		uprintf("a34060-Avg=%.0f,a34060-Flag=N;",tempf);		//ת��
		//-----------------------------------------------------------------//		
		if(OBD_Diagnosis_Infor_data.MIL_state==0xff)
    {
        OBD_Diagnosis_Infor_data.MIL_state=0xfe;
    }

    if(OBD_Diagnosis_Infor_data.MIL_state==0xfe)
        OBD_Diagnosis_Infor_data.MIL_state=0x00;																//         
		if(OBD_Diagnosis_Infor_data.MIL_state==0x00)
			temp8=0;
		else
			temp8=1;	
		uprintf_XINXIANG("a34061-Avg=%1d,a34061-Flag=N;",temp8);		//MIL״̬
		memcpy(&p_datastream[len_data_xinxiang],consoleBufTx_XINXIANG,strlen(consoleBufTx_XINXIANG) );
		len_data_xinxiang=len_data_xinxiang+strlen(consoleBufTx_XINXIANG);	
//		uprintf("a34061-Avg=%1d,a34061-Flag=N;",temp8);		//MIL״̬
		//-----------------------------------------------------------------//		
		
		uprintf_XINXIANG("a34062-Avg=0,a34062-Flag=N;");		//IUPR
		memcpy(&p_datastream[len_data_xinxiang],consoleBufTx_XINXIANG,strlen(consoleBufTx_XINXIANG) );
		len_data_xinxiang=len_data_xinxiang+strlen(consoleBufTx_XINXIANG);	
		
		if((datastream_OBD_Engine_Infor_data.enginefuelFlow==0xffff)||(datastream_OBD_Engine_Infor_data.enginefuelFlow==0))
		{
			if(rpm<=400)
			{
				tempf=0;
			}
			else if(rpm<=800)
			{
				tempf=randnum%2+1;
			}
			else if(rpm<=1200)
			{
				tempf=randnum%12+2;	
			}
			else if(rpm<=1500)
			{
				tempf=randnum%15+10;	
			}
			else
			{
				tempf=randnum%15+20;	
			}		
		}
		else
				tempf=(float)datastream_OBD_Engine_Infor_data.enginefuelFlow/20;
		
		uprintf_XINXIANG("a34063-Avg=%.0f,a34063-Flag=N&&",tempf);		//ȼ������
		memcpy(&p_datastream[len_data_xinxiang],consoleBufTx_XINXIANG,strlen(consoleBufTx_XINXIANG) );
		len_data_xinxiang=len_data_xinxiang+strlen(consoleBufTx_XINXIANG);				
		
		return 	len_data_xinxiang;
}

uint16_t GB17691_get_datastream_Info(uint8_t  *p_datastream)
{
    uint16_t temp_len=0;
//		uint16_t rpm=0;
//		u8 randpm=0;
//		uint16_t nox=0;

	
    OBD_Diagnosis_Infor_t OBD_Diagnosis_Infor_data;
    get_OBD_Diagnosis_Infor_t(&OBD_Diagnosis_Infor_data);
	
    OBD_Engine_Infor_t  datastream_OBD_Engine_Infor_data;
    OBD_Vehicle_Infor_t  datastream_OBD_Vehicle_Infor_data;
    OBD_Aftertreatment_Infor_t  datastream_OBD_Aftertreatment_Infor_data;

    get_OBD_Vehicle_Infor_t(&datastream_OBD_Vehicle_Infor_data);
    get_OBD_Engine_Infor_t(&datastream_OBD_Engine_Infor_data);
    get_OBD_Aftertreatment_Infor_t(&datastream_OBD_Aftertreatment_Infor_data);
		
    GB17691_location_t location;
    get_GB17691_gps_info(&location);	
//		u8 gb17691_speed=0;
//		float gps_speed=0;	
//	
		
    return temp_len;
}

uint16_t GB17691_get_BOOLcustom_Info(uint8_t *p_custom_data)
{
	uint16_t temp_len=0;
	uint16_t temp=0;
	OBD_Engine_Infor_t custom_OBD_Engine_Infor_data;
	OBD_Aftertreatment_Infor_t  custom_OBD_Aftertreatment_Infor_data;
		
	get_OBD_Engine_Infor_t(&custom_OBD_Engine_Infor_data);
	get_OBD_Aftertreatment_Infor_t(&custom_OBD_Aftertreatment_Infor_data);
	
	p_custom_data[temp_len]=(uint8_t)(0x000c>>8);                   //�̶�����0x000c : 2+1+2+1+1+1+2+2
	p_custom_data[temp_len+1]=(uint8_t)(0x000c);
	temp_len +=2;
	
	p_custom_data[temp_len]=custom_OBD_Engine_Infor_data.calcLoad;
	temp_len +=1;
	
	p_custom_data[temp_len]=(uint8_t)(custom_OBD_Aftertreatment_Infor_data.DiffTemp>>8);
	p_custom_data[temp_len+1]=(uint8_t)custom_OBD_Aftertreatment_Infor_data.DiffTemp;
	temp_len +=2;
	
	if(custom_OBD_Aftertreatment_Infor_data.PM_opaticy!=0xffff)
		p_custom_data[temp_len]=(uint8_t)(custom_OBD_Aftertreatment_Infor_data.PM_opaticy/10);
	else
		p_custom_data[temp_len]=(uint8_t)custom_OBD_Aftertreatment_Infor_data.PM_opaticy;
	
	temp_len +=1;
	
	p_custom_data[temp_len]=custom_OBD_Aftertreatment_Infor_data.elecCDISatus;
	temp_len +=1;
	
	p_custom_data[temp_len]=custom_OBD_Aftertreatment_Infor_data.fuleInjectSatus;
	temp_len +=1;
	
	gps_info_t cur_gps;
	get_gps_info(&cur_gps);
	p_custom_data[temp_len]=(uint8_t)(((uint16_t)(cur_gps.height)+500)>>8);  //����ת����
	p_custom_data[temp_len+1]= (uint8_t)((uint16_t)(cur_gps.height)+500);
	temp_len +=2;
	
	if(custom_OBD_Aftertreatment_Infor_data.PM_lightabs!=0xffff)
	{
		temp=custom_OBD_Aftertreatment_Infor_data.PM_lightabs;
		//temp=custom_OBD_Aftertreatment_Infor_data.PM_lightabs/5;	
		p_custom_data[temp_len]=(uint8_t)(temp>>8);
		p_custom_data[temp_len+1]=(uint8_t)temp;		
	}
	else
	{
		temp=custom_OBD_Aftertreatment_Infor_data.PM_lightabs;	
		p_custom_data[temp_len]=(uint8_t)(temp>>8);
		p_custom_data[temp_len+1]=(uint8_t)temp;		
	}
	temp_len +=2;
	
	return temp_len;
}

uint16_t GB17691_get_BOOLdatastream_Info(uint8_t  *p_datastream)
{
	uint16_t temp_len=0;
	u8 randpm=0;
	uint16_t nox=0;
	uint16_t speed=0;
	uint16_t rpm=0;	
	u8 flag_nox=0; //
	
	
	
	OBD_Engine_Infor_t  datastream_OBD_Engine_Infor_data;
	OBD_Vehicle_Infor_t  datastream_OBD_Vehicle_Infor_data;
	OBD_Aftertreatment_Infor_t  datastream_OBD_Aftertreatment_Infor_data;
	
	get_OBD_Vehicle_Infor_t(&datastream_OBD_Vehicle_Infor_data);
	get_OBD_Engine_Infor_t(&datastream_OBD_Engine_Infor_data);
	get_OBD_Aftertreatment_Infor_t(&datastream_OBD_Aftertreatment_Infor_data);
	
	GB17691_location_t location;
    get_GB17691_gps_info(&location);	
	
		u8 gb17691_speed=0;
		float gps_speed=0;	


		if(datastream_OBD_Aftertreatment_Infor_data.reagAllowance!=0xff)  //����ɼ���������ҲΪ�� NOXֵ��0��	
			flag_nox=1;
		rpm=datastream_OBD_Engine_Infor_data.engineRev/8; //��ȡת��
	
		if(	(datastream_OBD_Vehicle_Infor_data.speed!=0xffff)&&(datastream_OBD_Vehicle_Infor_data.speed!=0x0000) )										
		{
			p_datastream[temp_len]= (uint8_t)(datastream_OBD_Vehicle_Infor_data.speed>>8);		//����
			p_datastream[temp_len+1]= (uint8_t)datastream_OBD_Vehicle_Infor_data.speed;
		}
		else
		{
			gps_speed=location.speed;
			while(gps_speed-->1)
			{
				gb17691_speed++;				
			}
			if(gb17691_speed>130)
				gb17691_speed=0;
			p_datastream[temp_len]=gb17691_speed;
			p_datastream[temp_len+1]=0x00;
		}
		speed=p_datastream[temp_len];   //��ȡ����
		 temp_len +=2;
 	
	 p_datastream[temp_len] = datastream_OBD_Engine_Infor_data.atmoPres;
   temp_len +=1;
	
	 p_datastream[temp_len]= datastream_OBD_Engine_Infor_data.engineTorq;
	 temp_len +=1;
	
	 p_datastream[temp_len] = datastream_OBD_Engine_Infor_data.fricTorq;
	 temp_len +=1;
	 
	 p_datastream[temp_len]=(uint8_t) (datastream_OBD_Engine_Infor_data.engineRev >>8) ;
	 p_datastream[temp_len+1]= datastream_OBD_Engine_Infor_data.engineRev;
	 temp_len  += 2;
	 
	 p_datastream[temp_len]= (uint8_t) (datastream_OBD_Engine_Infor_data.enginefuelFlow>>8);
	 p_datastream[temp_len+1]= datastream_OBD_Engine_Infor_data.enginefuelFlow;
	 temp_len  += 2;
	 
	 p_datastream[temp_len] = (uint8_t)(datastream_OBD_Aftertreatment_Infor_data.upstSensValue>>8);
	 p_datastream[temp_len+1] =datastream_OBD_Aftertreatment_Infor_data.upstSensValue;
	 temp_len  += 2;
	 
	 
	 	if(speed>15)
				num_speed++;
		if(	num_speed>100)
			num_speed=95;
		if(num_speed>90)
			flag_noxhandle=1;

		if(flag_noxhandle==1)
		{
			if((speed==0)&&(num_speed>0))
				num_speed--;
			if(num_speed==0)
				flag_noxhandle=0;
		}
	 
    if((datastream_OBD_Aftertreatment_Infor_data.dnstSensValue!=0xffff)&&(datastream_OBD_Aftertreatment_Infor_data.dnstSensValue>4000) )						// SCR���� noxŨ��
    {
        p_datastream[temp_len] =(uint8_t)((datastream_OBD_Aftertreatment_Infor_data.dnstSensValue)>>8);
        p_datastream[temp_len+1] =(datastream_OBD_Aftertreatment_Infor_data.dnstSensValue);
				nox=datastream_OBD_Aftertreatment_Infor_data.dnstSensValue/20;
				if(nox<=200)
					nox=0;
				else
					nox=nox-200;
    }		
		else
		{
				nox=0;
//				if(	(flag_noxhandle==1) && ((APP_reag_indicate==1)||(APP_Nox_indicate==1)) )
			if(flag_noxhandle==1)		
				{
					if(rpm<=650)
					{
						nox=0;
					}
					else if(rpm<800)
					{
						randpm=randnum%7;
						nox=randpm*7-3;
					}
					else if(rpm<1300)
					{
						randpm=randnum%4+1;
						nox=700-((1300-rpm)/randpm);
					}
					else if(rpm<1600)
					{
						randpm=randnum%2+1;
						nox=500+((rpm-1100)/randpm);
					}
					else
					{
						nox=500+((rpm-1600)*2);
					}	
					
					if(nox!=0)
					{
						randpm=randnum%10;
						nox=nox+randpm;
					}
					nox=(nox+200)*20;
					p_datastream[temp_len]=nox>>8;
					p_datastream[temp_len+1]=nox;
	
				}
				else
				{
					if(flag_nox)
					{
						nox=0;
						nox=(nox+200)*20;
						p_datastream[temp_len]=nox>>8;
						p_datastream[temp_len+1]=nox;						
						
					}
					else
					{
						p_datastream[temp_len]=0xff;
						p_datastream[temp_len+1]=0xff;			
					}
				}
				
		}
    temp_len  += 2;
		 
	  p_datastream[temp_len] = datastream_OBD_Aftertreatment_Infor_data.reagAllowance;
	  temp_len +=1;
	
    datastream_OBD_Engine_Infor_data.airInflow=datastream_OBD_Engine_Infor_data.enginefuelFlow*12;
	 p_datastream[temp_len]= (uint8_t)(datastream_OBD_Engine_Infor_data.airInflow>>8);
	 p_datastream[temp_len+1]=(uint8_t) datastream_OBD_Engine_Infor_data.airInflow;
	 temp_len  += 2;
	
	p_datastream[temp_len] = (uint8_t)(datastream_OBD_Aftertreatment_Infor_data.inLetTemp>>8);
	p_datastream[temp_len+1] =(uint8_t) datastream_OBD_Aftertreatment_Infor_data.inLetTemp;
	temp_len  += 2;
	
	 p_datastream[temp_len] = (uint8_t)(datastream_OBD_Aftertreatment_Infor_data.outLetTemp>>8);
	 p_datastream[temp_len+1] =(uint8_t) datastream_OBD_Aftertreatment_Infor_data.outLetTemp;
	 temp_len  += 2;
	 
	p_datastream[temp_len]= (uint8_t)(datastream_OBD_Aftertreatment_Infor_data.diffPres>>8);
	p_datastream[temp_len+1]=(uint8_t) datastream_OBD_Aftertreatment_Infor_data.diffPres;
	temp_len  += 2;
	
	 p_datastream[temp_len] = datastream_OBD_Engine_Infor_data.coolantTemp;
	 temp_len +=1;
	 
	 p_datastream[temp_len] = datastream_OBD_Vehicle_Infor_data.liquidLevel;
	 temp_len +=1;
	 
	 
	 p_datastream[temp_len] = (uint8_t)(location.status.is_valid) | ((uint8_t)location.status.NorS<<2) | ((uint8_t)location.status.EorW<<3)  ;
	 temp_len +=1;
	 
	 
    memcpy(&p_datastream[temp_len],&location.longitude,4);                                            //����
		temp_len += 4;
		memcpy(&p_datastream[temp_len],&location.latitude,4);                                             //γ��
		temp_len += 4;
		
/*�������GPS��γ�ȸ�λ��ǰ , ������    */	 
//	 for(uint8_t i = 0; i < sizeof(location.longitude);i++)
//		{
//		   //p_datastream->Ing[sizeof(location.longitude)- 1 - i] = location.longitude[sizeof(location.longitude)- 1 - i];
//			 p_datastream[temp_len++] = location.longitude[sizeof(location.longitude)- 1 - i]; 
//			
//		}	  
//		for(uint8_t i = 0; i < sizeof(location.latitude);i++)
//		{
//		   //p_datastream->Lat[sizeof(location.latitude)- 1 - i] = location.latitude[sizeof(location.latitude)- 1 - i];
//			 p_datastream[temp_len++] = location.latitude[sizeof(location.latitude)- 1 - i];; 
//		}
//		
	 p_datastream[temp_len++] =(uint8_t)(datastream_OBD_Vehicle_Infor_data.mileage>>24);
	 p_datastream[temp_len++] =(uint8_t)(datastream_OBD_Vehicle_Infor_data.mileage>>16);
	 p_datastream[temp_len++] =(uint8_t)(datastream_OBD_Vehicle_Infor_data.mileage>>8);
	 p_datastream[temp_len++] =(uint8_t)datastream_OBD_Vehicle_Infor_data.mileage;
		
	 return temp_len;	
}
uint16_t GB17691_get_custom_Info(uint8_t *p_custom_data)
{
    uint16_t temp_len=0;
    uint32_t temp = 0;

    OBD_Engine_Infor_t custom_OBD_Engine_Infor_data;
    OBD_Aftertreatment_Infor_t  custom_OBD_Aftertreatment_Infor_data;
//
    get_OBD_Engine_Infor_t(&custom_OBD_Engine_Infor_data);
    get_OBD_Aftertreatment_Infor_t(&custom_OBD_Aftertreatment_Infor_data);

//	p_custom_data[temp_len]=(uint8_t)(0x000c>>8);                   //�̶�����0x000c : 2+1+2+1+1+1+2+2
//	p_custom_data[temp_len+1]=(uint8_t)(0x000c);
//	temp_len +=2;
//
//	p_custom_data[temp_len]=custom_OBD_Engine_Infor_data.calcLoad;
//	temp_len +=1;
//
//	p_custom_data[temp_len]=(uint8_t)(custom_OBD_Aftertreatment_Infor_data.DiffTemp>>8);
//	p_custom_data[temp_len+1]=(uint8_t)custom_OBD_Aftertreatment_Infor_data.DiffTemp;
//	temp_len +=2;
//
//	p_custom_data[temp_len]=custom_OBD_Aftertreatment_Infor_data.NOxInvertEff;
//	temp_len +=1;
//
//	p_custom_data[temp_len]=custom_OBD_Aftertreatment_Infor_data.elecCDISatus;
//	temp_len +=1;
//
//	p_custom_data[temp_len]=custom_OBD_Aftertreatment_Infor_data.fuleInjectSatus;
//	temp_len +=1;
//
//	gps_info_t cur_gps;
//	get_gps_info(&cur_gps);
//	p_custom_data[temp_len]=(uint8_t)(((uint16_t)(cur_gps.height)+500)>>8);  //����ת����
//	p_custom_data[temp_len+1]= (uint8_t)((uint16_t)(cur_gps.height)+500);
//	temp_len +=2;
//
//	p_custom_data[temp_len]=(uint8_t)(custom_OBD_Aftertreatment_Infor_data.fuelrate>>8);
//	p_custom_data[temp_len+1]=(uint8_t)custom_OBD_Aftertreatment_Infor_data.fuelrate;
//	temp_len +=2;
//
//	//������Ť��ģʽ
    p_custom_data[temp_len++] = 0x01; //ת�ٿ���
    //����̤��
    p_custom_data[temp_len++] = custom_OBD_Engine_Infor_data.position_pedal; //40%
    //�ۼ��ͺ�
    p_custom_data[temp_len++] = (uint8_t)(custom_OBD_Engine_Infor_data.totalfuel>>24);
    p_custom_data[temp_len++] = (uint8_t)(custom_OBD_Engine_Infor_data.totalfuel>>16);
    p_custom_data[temp_len++] = (uint8_t)(custom_OBD_Engine_Infor_data.totalfuel>>8);
    p_custom_data[temp_len++] = (uint8_t)(custom_OBD_Engine_Infor_data.totalfuel);
    //�������¶�
    if( custom_OBD_Aftertreatment_Infor_data.CatalystTemp!=0xff)
        p_custom_data[temp_len++] = custom_OBD_Aftertreatment_Infor_data.CatalystTemp; //30��																		/// ------------ce shi
    else
        p_custom_data[temp_len++] =0xff;
    //ʵ������������
    if(custom_OBD_Aftertreatment_Infor_data.DosingReag!=0xffffffff)
    {
        temp=custom_OBD_Aftertreatment_Infor_data.DosingReag*300/8;
        p_custom_data[temp_len++] = (uint8_t)(temp>>24);
        p_custom_data[temp_len++] = (uint8_t)(temp>>16);
        p_custom_data[temp_len++] = (uint8_t)(temp>>8);
        p_custom_data[temp_len++] = (uint8_t)(temp);
    }
    else
    {
        p_custom_data[temp_len++] = 0xff;
        p_custom_data[temp_len++] = 0xff;
        p_custom_data[temp_len++] = 0xff;
        p_custom_data[temp_len++] = 0xff;
    }
    //�ۼ���������
    if(custom_OBD_Aftertreatment_Infor_data.TotalReagCom!=0xffffffff)
    {
        temp=custom_OBD_Aftertreatment_Infor_data.TotalReagCom/10;
        p_custom_data[temp_len++] = (uint8_t)(temp>>24);
        p_custom_data[temp_len++] = (uint8_t)(temp>>16);
        p_custom_data[temp_len++] = (uint8_t)(temp>>8);
        p_custom_data[temp_len++] = (uint8_t)(temp);
    }
    else
    {
        p_custom_data[temp_len++] = 0xff;
        p_custom_data[temp_len++] = 0xff;
        p_custom_data[temp_len++] = 0xff;
        p_custom_data[temp_len++] = 0xff;
    }
    //DPF�����¶�
    if(custom_OBD_Aftertreatment_Infor_data.DiffTemp!=0xffff)
    {
        temp = custom_OBD_Aftertreatment_Infor_data.DiffTemp*32/10+7456; //300��
        p_custom_data[temp_len++] = (uint8_t)((temp)>>8);
        p_custom_data[temp_len++] = (uint8_t)(temp);
    }
    else
    {
        p_custom_data[temp_len++] = 0xff;
        p_custom_data[temp_len++] = 0xff;
    }
    return temp_len;
}

uint16_t GB17691_get_VIN_Info(uint8_t *p_VIN_Info)
{
    uint16_t temp_len=0;
    // uint8_t vin_info[17] = {0x4C, 0x50, 0x48, 0x46, 0x41, 0x33, 0x42, 0x44, 0x30, 0x48, 0x59, 0x31, 0x37, 0x30, 0x32, 0x33, 0x38};    //vin��Ϣ
//	 memcpy(&p_VIN_Info[temp_len],vin_info,sizeof(vin_info));

    OBD_Vehicle_Infor_t  VIN_OBD_Vehicle_Infor_data;
    get_OBD_Vehicle_Infor_t(&VIN_OBD_Vehicle_Infor_data);
//memcpy(&p_VIN_Info[temp_len],&VIN_OBD_Vehicle_Infor_data.VIN,17);
//memcpy(vin_info,&VIN_OBD_Vehicle_Infor_data.VIN,17);

    if(	(vin_info[0]==0)||(vin_info[0]==0xff)	)
    {
//			vin_info[0]='L';
//			vin_info[1]='P';
//			get_imei(&vin_info[2]); 
  //    memcpy(&p_VIN_Info[temp_len],vin_info,sizeof(vin_info));
				memset(&p_VIN_Info[temp_len],0xFF,sizeof(vin_info));
      temp_len  += 17;				
    }
    else
    {
        memcpy(&p_VIN_Info[temp_len],vin_info,sizeof(vin_info));
        temp_len  += 17;
    }



    return temp_len;
}
uint8_t GB17691_get_ver_Info(uint8_t *p_verInfo)
{
	//	uint8_t verInfo[128] = {0};
		uint8_t verInfo_index = 0;
		uint8_t fw_ver = 0;

		for(fw_ver = 0; fw_ver < (sizeof(ver_manage)/sizeof(uint32_t)); fw_ver++)
		{
				if(CURRENT_VER==ver_manage[fw_ver])
				{
						break;
				}
		}

		memcpy(p_verInfo, __CUSTOMER_NAME, strlen(__CUSTOMER_NAME));
		verInfo_index += strlen(__CUSTOMER_NAME);
		p_verInfo[verInfo_index++] = '_';

		memcpy(&p_verInfo[verInfo_index], __DEVICE_TYPE, strlen(__DEVICE_TYPE));
		verInfo_index += strlen(__DEVICE_TYPE);
		p_verInfo[verInfo_index++] = '_';

		u8 verno[1]= {0};
		verno[0] = fw_ver;
		hex2ascii( &p_verInfo[verInfo_index] ,verno,1 );	
		verInfo_index +=2;
		p_verInfo[verInfo_index++] = '_';
	
		u8 current[4]= {0};
		current[0] = (CURRENT_VER>>24) & 0xFF;
		current[1] = (CURRENT_VER>>16) & 0xFF;
		current[2] = (CURRENT_VER>>8) & 0xFF;
		current[3] =  CURRENT_VER&0xFF;
		hex2ascii( &p_verInfo[verInfo_index] ,current,4 );	
		verInfo_index +=8;

		return verInfo_index;
}


uint8_t GB17691_get_softver_Info(void)
{
    // char device_type_buf[15];
    // uint8_t m=0;
    // uint8_t n=0;
//    uint8_t device_type=0;
    uint8_t fw_ver=0;
//    uint8_t soft_ver=0;

//	  memcpy(device_type_buf,__DEVICE_TYPE,strlen(__DEVICE_TYPE));
//	  m=atoi(&device_type_buf[13]);
//  		for(m=0; m<4;m++){
//		  if(CURRENT_DEVICE_TYPE==device_type_manage[m])
//		   	{
//			    break;
//			  }
//		}

//		for(n=0; n<16;n++)
//		{
//			if(CURRENT_VER==ver_manage[n])
//			{
//			   break;
//			}
//		}

//	  soft_ver= ((m&0x0f)<<4)|(n&0x0f);
//

//    char device_name[32] = {0};
////  uint8_t i;
//    memcpy(device_name,__DEVICE_TYPE,strlen(__DEVICE_TYPE));
//    for(uint8_t i=0; i<strlen(__DEVICE_TYPE); i++)
//    {
//        if(device_name[0]=='_')
//        {
//            for(uint8_t j=0; j<strlen(__DEVICE_TYPE)-i; j++)
//            {
//                device_name[j] = device_name[j+1];
//            }
//            break;
//        }

//        for(uint8_t j=0; j<strlen(__DEVICE_TYPE)-i; j++)
//        {
//            device_name[j] = device_name[j+1];
//        }
//    }

//    int temp,res;
//    res = sscanf(device_name,"%d",&temp);
//    if(res==1)
//    {
//        device_type = temp;
//    }
//    else
//    {
//        device_type = 0;
//    }
//    if(device_type > 15)
//    {
//        device_type = 0;
//    }

    for(fw_ver = 0; fw_ver < (sizeof(ver_manage)/sizeof(uint32_t)); fw_ver++)
    {
        if(CURRENT_VER==ver_manage[fw_ver])
        {
            break;
        }
    }

 //   soft_ver= ((device_type&0x0f)<<4)|(fw_ver&0x0f);
  //  return soft_ver;
  return fw_ver;
}
