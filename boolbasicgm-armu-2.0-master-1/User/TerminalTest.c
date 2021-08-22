#include "TerminalTest.h"
#include "obd.h"
#include "Bsp.h"
#include "UART.h"
#include "sim_net.h"
#include "gps.h"
#include "GB17691A.h"
#include "can.h"
#include "GB17691A.h"
#include "power_ctl.h"
#include "sleep.h" 
static uint8_t testState = 0;
static uint16_t testList;
static uint16_t listState;
static uint16_t testDelay;
static uint8_t imeiOffset;
extern uint8_t flag_rs232test;

static const uint8_t testVin[] = {0x30,0x31,0x32,0x33,0x34,0x35,0x2A,0x36,0x37,0x38,0x39,0x2A,0x41,0x42,0x43,0x44,0x45,0x46,0x2A,0x47};
static const char listPrintf[][50] = {
																			{"\r\n1-->Module failure\r\n"}, 
																			{"\r\n2-->Networking failure\r\n"}, 
																			{"\r\n3-->GPS failure\r\n"},
																			{"\r\n4-->Flash failure\r\n"},
																			{"\r\n5-->Relay Normal, CAN1 failure\r\n"}, 
																			{"\r\n6-->Relay power, CAN1 failure\r\n"},
																			{"\r\n7-->CAN2 send failure\r\n"},
																			{"\r\n8-->LKT4305 TEST failure\r\n"},
																			{"\r\n9-->RS232 TEST failure\r\n"}
                                     };

void TerminalTestProcess(void)
{
	if(testState==1)
	{
		if((testDelay==1)&&(BKP_ReadBkpData(BKP_DAT9) == 0xa5a5)	)
		{
					USARTSendOut(RS232_USART,"\r\n---TEST SUCCESS---\r\n",strlen("\r\n---TEST SUCCESS---\r\n"));
					testState = 2;	
		    	BKP_WriteBkpData(BKP_DAT9, 0xffff);	
		}	
		else
		{
			if((!(listState&(1<<0)))&&(testDelay>0))
			{
				USARTSendOut(RS232_USART,"\r\n----------------------\r\n", \
													strlen("\r\n----------------------\r\n"));
				USARTSendOut(RS232_USART,"\r\ngoto test state!\r\n",strlen("\r\ngoto test state!\r\n"));
				listState|=(1<<0);
			}
			if(!(testList&(1<<0))) //模块信息
			{
				char* pFwVer;
				uint8_t imei[IMEI_LEN];
				uint8_t iccid[ICCID_LEN];
				
				pFwVer = get_sim_fw_ver();
				if((pFwVer!=NULL)&&get_iccid(iccid)&&get_imei(imei))
				{
					USARTSendOut(RS232_USART,"\r\nSIM7600CE firmware version: ",strlen("\r\nSIM7600CE firmware version: "));
					USARTSendOut(RS232_USART,pFwVer,strlen(pFwVer));
					USARTSendOut(RS232_USART,"\r\n",strlen("\r\n"));
					USARTSendOut(RS232_USART,"\r\nICCID: ",strlen("\r\nICCID: "));
					USARTSendOut(RS232_USART,(char*)iccid,sizeof(iccid));
					USARTSendOut(RS232_USART,"\r\n",strlen("\r\n"));
					USARTSendOut(RS232_USART,"\r\nIMEI: ",strlen("\r\nIMEI: "));
					USARTSendOut(RS232_USART,(char*)imei,sizeof(imei));
					USARTSendOut(RS232_USART,"\r\n",strlen("\r\n"));
					testList |= (1<<0);
				}
			}
			
			if(!(testList&(1<<1))) //联网状态
			{
				extern uint8_t timing_state;
				if(timing_state == 1)
				{
					testList |= (1<<1);
				}	
			}
			
			if(!(testList&(1<<2))) //GPS
			{
				rtc_time_t time;
				if(get_gps_time(&time))
				{
					testList |= (1<<2);
				}
			}
			
			if(!(testList&(1<<3))) //flash
			{
				uint32_t resend_flag_addr;
				uint8_t* msg_pos;
				uint8_t i;
				uint8_t readSuc = 0;
				uint32_t resend_flash_addr = RESEND_DATA_START_ADDR;
				for(i=0;i<5;i++)
				{
					msg_pos = read_msg_from_flash(&resend_flash_addr,&resend_flag_addr,RESEND_DATA_FLASH_AREA);
					if(msg_pos!=NULL)
					{
						readSuc += 1;
					}
				}
				
				if(readSuc>=2)
				{
					testList |= (1<<3);
				}
				if(get_keystate()==1)
					testList |= (1<<3);
			}
			
			if(testList&(1<<0))
			{
				
				do{
					uint8_t imei[IMEI_LEN];
					get_imei(imei);
					if(!(testList&(1<<4))) //CAN1 send rx
					{
						GPIO_ResetBits(GPIOB, GPIO_PIN_15);
						if(!(listState&(1<<4)))  //
						{
							imeiOffset = 1;
//							CAN1_32bitfilter_Init(0x12345678, 0x1fffffff,1);
							CanTxMessage TxMessage;
							memset(&TxMessage,0,sizeof(CanTxMessage));
							TxMessage.ExtId = 0x12345678;
							TxMessage.IDE = CAN_ID_EXT;
							
							for(uint8_t i=0;i<8;i++)
							{
								TxMessage.Data[i]=imei[i+7]+imeiOffset;
							}
							TxMessage.DLC = 8;
							CAN1SendMsg(&TxMessage);
							listState|=(1<<4);
							break;
						}
						else
						{
							listState&=(~(1<<4));
							break;
						}
					}
					
					if(!(testList&(1<<5))) //CAN1 send rx
					{
						GPIO_SetBits(GPIOB, GPIO_PIN_15);
						osDelay(100);
						if(!(listState&(1<<5)))  //
						{
							imeiOffset = 2;
//							CAN1_32bitfilter_Init(0x12345678, 0x1fffffff,1);
							CanTxMessage TxMessage;
							memset(&TxMessage,0,sizeof(CanTxMessage));
							TxMessage.ExtId = 0x12345678;
							TxMessage.IDE = CAN_ID_EXT;
							for(uint8_t i=0;i<8;i++)
							{
								TxMessage.Data[i]=imei[i+7]+imeiOffset;
							}
							TxMessage.DLC = 8;
							CAN1SendMsg(&TxMessage);
							listState|=(1<<5);
							break;
						}
						else
						{
							listState&=(~(1<<5));
							break;
						}
					}
					
				if(!(testList&(1<<6))) //CAN2 send 
				{
					GPIO_SetBits(GPIOB, GPIO_PIN_15);
					if(!(listState&(1<<6)))  //
					{
						imeiOffset = 3;
//						CAN1_32bitfilter_Init(0x12468357, 0x1fffffff,1);
						CanTxMessage TxMessage;
						memset(&TxMessage,0,sizeof(CanTxMessage));
						TxMessage.ExtId = 0x12468357;
						TxMessage.IDE = CAN_ID_EXT;
						for(uint8_t i=0;i<9;i++)
						{
							TxMessage.Data[i]=imei[i+7]+imeiOffset;
						}
						TxMessage.DLC = 8;
						TxMessage.RTR =0;
						CAN2SendMsg(&TxMessage);
						listState|=(1<<6);
						break;
					}
					else
					{
						listState&=(~(1<<6));
						break;
					}
				}	
			}while(0);
		}
			
			if(!(testList&(1<<7))) // 安全芯片+232
			{
				if(get_lkt4305_state()) 
				{
					testList |= (1<<7);
				}
			}
			
			if(!(testList&(1<<8))) // 232
			{
				if(flag_rs232test) 
				{
					testList |= (1<<8);
				}
			}			
			
			
			
			if((testList&0x01FF)==0x01FF)
			{			
//				turn_off_sim_3v8();
//				turn_off_3v3();
				BKP_WriteBkpData(BKP_DAT9, 0xa5a5);
				sleep_s(2);
				testDelay++;
				if(testDelay>50)
				{
						BKP_WriteBkpData(BKP_DAT9, 0xffff);		
						if(testState==1)				
						USARTSendOut(RS232_USART,"\r\n---TEST FAILED---\r\n",strlen("\r\n---TEST FAILED---\r\n"));
						testState = 2;	
				}
			}
			else
			{
				testDelay += 1;
				if(testDelay>50)
				{
					for(uint8_t i=0;i<9;i++)
					{
						if(!(testList&(1<<i)))
						{
							USARTSendOut(RS232_USART,listPrintf[i],strlen(listPrintf[i]));
						}
					}
					
					USARTSendOut(RS232_USART,"\r\n---TEST FAILED---\r\n",strlen("\r\n---TEST FAILED---\r\n"));
					
					//testList = 0;
					
	//				for(uint8_t i=0;i<7;i++)
	//				{
	//					if(!(testList&(1<<i)))
	//					{
	//						USARTSendOut(RS232_USART,listPrintf[i],strlen(listPrintf[i]));
	//					}
	//				}
					testState = 2;
				}
			}
		}
	}
	else
	{
		if(testState==0)
		{
			uint8_t i;
			for(i=0; i<sizeof(OBD_vehicle_infor.VIN); i++)
			{
				if(OBD_vehicle_infor.VIN[i]!= testVin[i])
				{
					break;
				}
			}
			
//			//测试
//			i=sizeof(OBD_vehicle_infor.VIN);
			
			if(i==sizeof(OBD_vehicle_infor.VIN))
			{
				RS232_USART_Config(115200);
				testList = 0;
				listState = 0;
				testDelay = 0;
				testState = 1;
			}
	  }
	}
}


void CAN1test(CanRxMessage *pMsg)
{
	uint8_t imei[IMEI_LEN];
	get_imei(imei);
	uint8_t i;
	for(i=0;i<8;i++)
	{
		if(pMsg->Data[i]!=(imei[i+7]+imeiOffset))
		{
			break;
		}
	}
	if(i==8)
	{
		testList |= (1<<(3+imeiOffset));
	}
}


uint8_t getTermianlState(void)
{
	return testState;
}

