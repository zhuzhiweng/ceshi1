#include "UART.h"
#include "spi_flash.h"
#include "power_ctl.h"
#include "crc.h"
#include "Bsp.h"

#define COM_CMD_CHECK_DEALY  (100)  //��������ʱ 100ms
#define COM_CMD_CHECK_INTERVAL 100  //��������100ms
#define FIRMWARE_UART_RX_BUF_SIZE 1024

uint8_t Firmware_uart_rx_buf[FIRMWARE_UART_RX_BUF_SIZE];  //���ڻ���
static uint16_t firmware_uart_rx_write_pos=0;  //��������д����λ��
static uint16_t COM_rd_pos = 0;           //��λ��
uint8_t BIN_NAME_BUF[32];
uint8_t BIN_FREAM_INFO_BUF[10];
uint8_t Firmware_buf[FIRMWARE_UART_RX_BUF_SIZE];
uint32_t BIN_FILE_SIZE;
static uint8_t last_data0=0;
static uint8_t last_data1=0;

static uint8_t COM_check_cmd_info_flag=0;
static uint16_t frame_len=0;              
static uint8_t COM_CMD_check_dealy;        //��������ʱ
uint8_t comupdate_mode_flag=0;   //��������ģʽ��־ 0 ��������ģʽ��1����������ģʽ
static uint8_t FrameNo=0;
uint8_t flag_rs232test=0; //232���ڲ��Ա��


//static char temp_BufTx1[19]={0x23,0x23,0x81,0x00,0x00,0x00,0x08,0x01,0x0,0x08,0x44,0x41,0x54,0x41,0x20,0x45,0x52,0x52,0xF5}; //DATA ERR,
//static char temp_BufTx2[18]={0x23,0x23,0x81,0x00,0x00,0x00,0x07,0x01,0x00,0x07,0x44,0x41,0x54,0x41,0x20,0x4F,0x4B,0xB4}; //DATA OK
//static char temp_BufTx3[23]={0x23,0x23,0x81,0x00,0x00,0x00,0x0C,0x01,0x00,0x0C,0x42,0x49,0x4E,0x20,0x46,0x49,0x4C,0x45,0x20,0x45,0x52,0x52,0x86}; //BIN FILE ERR
//static char temp_BufTx4[22]={0x23,0x23,0x81,0x00,0x00,0x00,0x0B,0x01,0x00,0x0B,0x42,0x49,0x4E,0x20,0x46,0x49,0x4C,0x45,0x20,0x4F,0x4B,0xC7};//BIN FILE OK
	
static char temp_BufTx1[8]={0x44,0x41,0x54,0x41,0x20,0x45,0x52,0x52}; //DATA ERR,
static char temp_BufTx2[7]={0x44,0x41,0x54,0x41,0x20,0x4F,0x4B};      //DATA OK
static char temp_BufTx3[7]={0x42,0x49,0x4E,0x20,0x45,0x52,0x52};      //BIN ERR
static char temp_BufTx4[6]={0x42,0x49,0x4E,0x20,0x4F,0x4B};           //BIN OK	
		
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
//int fputc(int ch, FILE *f)
//{      
//	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)==RESET){}; 
//  USART_SendData(USART1,(uint8_t)ch);   
//	return ch;
//}

int fputc(int ch, FILE *f)
{
	while(USART_GetFlagStatus(USART1, USART_FLAG_TXC) == RESET);		//�ȴ��ϴη��ͽ���
	USART_SendData(USART1, (unsigned char)ch);				//�������ݵ�����
	return ch;
}

void USART_Sendbyte(USART_Module* USARTx, u8 Data)
{
	USART_SendData(USARTx,Data);
	while(USART_GetFlagStatus(USARTx, USART_FLAG_TXDE) == RESET){}; 		
}

void USARTSendOut(USART_Module* USARTx, const char *buf, u16 len)			
{
	u16 i;	

	for(i = 0; i < len; i++) 
	{
		USART_SendData(USARTx, *buf++); 	                                  //�����յ������ݷ��͵���λ��
    while(USART_GetFlagStatus(USARTx, USART_FLAG_TXDE) == RESET){}; 		//	�ȴ��������͵���λ�Ĵ���
	}

}

#define  CONSOLE_MAX   40

char consoleBuf[CONSOLE_MAX];
__align(4) char consoleBufTx[CONSOLE_MAX];
u8 consoleCnt=0;
int a,b,c,d;

char ipAdress[]="";


#include <stdarg.h>
int uprintf(char *format, ...)
{
		va_list aptr;
   int ret;

   va_start(aptr, format);
   ret = vsprintf(consoleBufTx, format, aptr);
   va_end(aptr);
//	if(consoleCnt<CONSOLE_MAX)
	USARTSendOut(USART1,consoleBufTx,strlen(consoleBufTx));
  return(ret);
		
		
}

uint8_t vin_info[17]={0};//�Ϻ�VIN�¼�
uint8_t flag_set_vin=0;//�Ϻ�VIN�¼�
uint8_t flag_set_evads=0;//�Ϻ�VIN�¼�

static uint8_t comupdate_check_crc32(void)    //��д��FLASH��BIN�ļ�У��
{
	uint32_t get_crc;
	uint32_t calculate_crc; //crc32 4�ֽ�
	uint8_t data[4];
	calculate_crc = CalcCRC32(FIRMWARE_START_ADDR, BIN_FILE_SIZE - 4);
	read_data_from_flash(data,(FIRMWARE_START_ADDR+BIN_FILE_SIZE-4),4,FIRMWARE_DATA_FLASH_AREA);
	get_crc = (uint32_t)data[0] + (uint32_t)(data[1] << 8)+ (uint32_t)(data[2] << 16)+ (uint32_t)(data[3] << 24);
	if(calculate_crc == get_crc)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

////��Ƭ�������λ
//void MCU_RESET(void)
//{
//__set_FAULTMASK(1); //���������ж�
//NVIC_SystemReset(); //��λ
//}
//��������ģʽ
uint8_t get_comupdate_mode(void){
 return comupdate_mode_flag;
}
	
uint16_t get_rx_buf_wr_pos(void)     //��ȡ���ڻ���дλ��
{
	uint16_t rx_write_temp;
	USART_ConfigInt(USART1, USART_INT_RXDNE, DISABLE);
	rx_write_temp = firmware_uart_rx_write_pos;
	USART_ConfigInt(USART1, USART_INT_RXDNE, ENABLE);
	return rx_write_temp;
}
uint8_t get_rx_byte(uint16_t rd_pos)  //��ȡ���ڻ����ֵ
{
  return Firmware_uart_rx_buf[rd_pos];
}

void get_data_multiple_bytes(uint8_t *pdata, uint16_t data_pos, uint16_t len)  //�Ӵ��ڻ����ж�ȡ����
{
	for(uint16_t i = 0; i < len; i++)
	{
		pdata[i] = Firmware_uart_rx_buf[data_pos];
		
		data_pos++;
		if(data_pos >= FIRMWARE_UART_RX_BUF_SIZE)
		{
			data_pos = 0;
		}	
	}
}

uint16_t get_rx_data_size(uint16_t rx_read_pos) //��ȡ�����յ����Ĵ�С
{
	uint16_t data_size = 0;
	
	uint16_t rx_write_temp;
	USART_ConfigInt(USART1, USART_INT_RXDNE, DISABLE);
	rx_write_temp = firmware_uart_rx_write_pos;
	USART_ConfigInt(USART1, USART_INT_RXDNE, ENABLE);
	
	if(rx_write_temp > rx_read_pos)
	{
		data_size = rx_write_temp - rx_read_pos;
	}
	else
	{
		if(rx_write_temp < rx_read_pos)
		{
			data_size = FIRMWARE_UART_RX_BUF_SIZE - (rx_read_pos - rx_write_temp);
		}
		else
		{
			data_size = 0;
		}
	}
	return data_size;
}

void COM_CMD_analysis(uint8_t *pdata, uint16_t len)
{
	char device_type[32];
	
	uint8_t temp_FrameNo=0;
	
	uint32_t terminal_firmware=0x20011501;
	memcpy(device_type,"OBDS5S7600V06",strlen("OBDS5S7600V06"));
	switch(pdata[2])
	{
		 case BIN_FILE_DATA_CMD:
			 				
		 	 			if(!write_data_to_flash(pdata+10,FIRMWARE_START_ADDR+BIN_FILE_SIZE,len-11,FIRMWARE_DATA_FLASH_AREA))  //�̼�����д��FLASH��
						{
						//	uprintf("DATA ERR \r\n");
							USARTSendOut(USART1,temp_BufTx1,8);
							
						}else{
						//	uprintf("DATA OK\r\n");
							USARTSendOut(USART1,temp_BufTx2,7);
							uprintf("\r\n");
							//BIN_FILE_SIZE +=(frame_len-10); //for test
							temp_FrameNo=pdata[7];         //20200327
							if(temp_FrameNo == (FrameNo+1)){
								BIN_FILE_SIZE +=(frame_len-11);
								FrameNo = temp_FrameNo;
							}
						}
							
						if(BIN_FILE_SIZE == (pdata[3]<<24 | pdata[4]<<16 | pdata[5]<<8 | pdata[6]))  //128K byte
						{		
					     if(!comupdate_check_crc32()){
								//	uprintf("BIN FILE ERR\r\n");
								  osDelay(20);
							    USARTSendOut(USART1,temp_BufTx3,7);
							    if(!firmware_flash_init())
										{
												break;
										}
									BIN_FILE_SIZE=0;
			            break;
		            }else{
							     set_firmware_info(device_type, terminal_firmware, BIN_FILE_SIZE, 0xA5A5A5A5);
								 //		uprintf("BIN FILE OK\r\n");
									  osDelay(20);
							      USARTSendOut(USART1,temp_BufTx4,6);
										BIN_FILE_SIZE=0;
										osDelay(100);
										MCU_RESET();  //ϵͳ�����λ
						    }
//							 //set_firmware_info(device_type, terminal_firmware, BIN_FILE_SIZE, 0xA5A5A5A5);
//							 USARTSendOut(USART1,temp_BufTx4,22);
//							// uprintf("BIN FILE OK\r\n");
//						   BIN_FILE_SIZE=0;
//							 osDelay(100);
//							 MCU_RESET();  //ϵͳ�����λ
					 	}else if(BIN_FILE_SIZE > (pdata[3]<<24 | pdata[4]<<16 | pdata[5]<<8 | pdata[6])){
						      USARTSendOut(USART1,temp_BufTx3,7);
							    if(!firmware_flash_init())
										{
												break;
										}
									BIN_FILE_SIZE=0;
			            break;
						}else{
						    break;
						}
			 break;		
		default:break;
	}
}

void COM_CMD_check(void)
{

	do{
			if(COM_check_cmd_info_flag == 0)
			{
				if(get_rx_data_size(COM_rd_pos) > 10)  //���ٽ��յ�10���ֽڣ��ſ�ʼ����
				{
					  get_data_multiple_bytes(Firmware_buf, COM_rd_pos, 10);
						if(Firmware_buf[0] == 0x23)//�ж���ʼ�� 
						{
								if(Firmware_buf[1] ==0x23)
								{
									frame_len = (uint16_t)(Firmware_buf[8] << 8) + (uint16_t)Firmware_buf[9];		
								//	frame_len += 10;   //for test
									frame_len += 11;
 								  //���ݵ�Ԫ���ȣ�����10�ֽڵ���Ϣ��1���ֽڵ�У�飬	
									if(frame_len <= sizeof(Firmware_buf))  //�ж������Ƿ񳬳���Χ,�ٽ���ѭ��
									{
										COM_check_cmd_info_flag = 1;
										COM_CMD_check_dealy = 0;
									}		
								}
					  }	
					if(COM_check_cmd_info_flag == 0)
					{
						COM_rd_pos++;
						if(COM_rd_pos >= FIRMWARE_UART_RX_BUF_SIZE)
						{
							COM_rd_pos = 0;
						}
					}
				}
				else
				{
					break;
				}
		}
		else  //�ҵ�֡ͷ���Ӵ��ڻ����ȡ���ݷ���
		{
			if(get_rx_data_size(COM_rd_pos) >= frame_len)
			{
				COM_check_cmd_info_flag = 0;
				
				get_data_multiple_bytes(Firmware_buf, COM_rd_pos, frame_len); //�Ӵ��ڻ����ж�ȡframe_len�����ݵ�Firmware_buf
				  
				if(EvalBCC_FromBytes(&Firmware_buf[2], frame_len-3) == Firmware_buf[frame_len-1]){ //����У��
		//	  if((Firmware_buf[frame_len-2]==0xaa) && (Firmware_buf[frame_len-1]==0x55)){  //for test
					COM_rd_pos += frame_len;
					if(COM_rd_pos >= FIRMWARE_UART_RX_BUF_SIZE)
					{
						COM_rd_pos -= FIRMWARE_UART_RX_BUF_SIZE;
					}
					COM_CMD_analysis(Firmware_buf, frame_len);  //����֡����
				}else{     //������֡ͷ
					COM_rd_pos++;
					if( COM_rd_pos >= FIRMWARE_UART_RX_BUF_SIZE)
					{
						COM_rd_pos = 0;
					}
					//	uprintf("DATA ERR \r\n");
					USARTSendOut(USART1,temp_BufTx1,8);	
			  }
			}
			else
			{
				if(COM_CMD_check_dealy < COM_CMD_CHECK_DEALY) //��ʱ1s
				{
					COM_CMD_check_dealy++;
				  break;
				}
				else
				{
					COM_check_cmd_info_flag = 0;
					COM_rd_pos++;
					if(COM_rd_pos >= FIRMWARE_UART_RX_BUF_SIZE)
					{
						COM_rd_pos = 0;
					}
				}
			}
		}
	}while(1);
}

void RS232_USART_IRQHandler(void)
{
	u8 data=0;	
	if(USART_GetIntStatus(RS232_USART, USART_INT_RXDNE) != RESET)//�����ж�
	{
		data=
		Firmware_uart_rx_buf[firmware_uart_rx_write_pos]=USART_ReceiveData(USART1);
		firmware_uart_rx_write_pos++;
		if(firmware_uart_rx_write_pos>=FIRMWARE_UART_RX_BUF_SIZE)
			firmware_uart_rx_write_pos=0;
	}
	
	if(comupdate_mode_flag==0){   
	if(data=='\r')  //�س��� һ�������к���00 �� 0D ����������������ַ�ʽ���ʺϴ����������������Ҳ���0D �� 00 �����
	{		
		char* p=strstr((char*)Firmware_uart_rx_buf,"SET:");
		if(p)
		{
			int re=-1;
			re=sscanf(p,"SET:%d,%d,%d,%d",&a,&b,&c,&d);
				if(re==4)
				{
//				SetUdsPara(a,b,c,d);
				SetUdsPara(_PROTOCOL,a);
				SetUdsPara(_DCUID,b);	
				SetUdsPara(_NEED_COMPARE,c);
				SetUdsPara(_MODE,d);
				memset(Firmware_uart_rx_buf,0,FIRMWARE_UART_RX_BUF_SIZE);
				consoleCnt=0;
				uprintf("SET OK\r\n");
				return ;				
				}
				else
				{
				 memset(Firmware_uart_rx_buf,0,FIRMWARE_UART_RX_BUF_SIZE);
				 firmware_uart_rx_write_pos=0;
					uprintf("SET ERR\r\n");
					return;
				}				
		}
		
		p=strstr((char*)Firmware_uart_rx_buf,"VIN:");
		if(p)
		{
			memcpy(vin_info,p+4,17);
			flag_set_vin=1;
			memset(Firmware_uart_rx_buf,0,FIRMWARE_UART_RX_BUF_SIZE);
			firmware_uart_rx_write_pos=0;
			return ;
		}
		
		p=strstr((char*)Firmware_uart_rx_buf,"RS232TEST;");
		if(p)
		{
			flag_rs232test=1;
			memset(Firmware_uart_rx_buf,0,FIRMWARE_UART_RX_BUF_SIZE);
			firmware_uart_rx_write_pos=0;
			return ;
		}	
		
		p=strstr((char*)Firmware_uart_rx_buf,"IP:");		
		if(p)
		{
			//u8 ip[4]={0};
			u32 ip[4]={0};  //for clear warning
			int re=-1;
			re=sscanf(p,"IP:%d.%d.%d.%d",&ip[0],&ip[1],&ip[2],&ip[3]);
			if(re==4)
			{
			u32 ipData= *(u32*)ip;
			SetUdsPara(_IP,ipData);
			memset(Firmware_uart_rx_buf,0,FIRMWARE_UART_RX_BUF_SIZE);
			firmware_uart_rx_write_pos=0;
			uprintf("SET OK\r\n");
			return ;
			}
			else
			{
			 memset(Firmware_uart_rx_buf,0,FIRMWARE_UART_RX_BUF_SIZE);
				firmware_uart_rx_write_pos=0;
				uprintf("SET ERR\r\n");
				return;
			}
		}
		p=strstr((char*)Firmware_uart_rx_buf,"PORT:");			
		if(p)
		{
			int re=-1;
			int port=0;
			re=sscanf(p,"PORT:%d",&port);
			if(re==1)
			{
			
			SetUdsPara(_PORT,port);
			memset(Firmware_uart_rx_buf,0,FIRMWARE_UART_RX_BUF_SIZE);
			firmware_uart_rx_write_pos=0;
			uprintf("SET OK\r\n");
			return ;
			}
			else
			{
			 memset(Firmware_uart_rx_buf,0,FIRMWARE_UART_RX_BUF_SIZE);
				firmware_uart_rx_write_pos=0;
				uprintf("SET ERR\r\n");
				return;
			}
		}
    p=strstr((char*)Firmware_uart_rx_buf,"EVADE:");			
		if(p)
		{
			int re=-1;
			int evade=0;
			re=sscanf(p,"EVADE:%d",&evade);
			if(re==1)
			{
			
			SetUdsPara(_EVADES,evade);
		  memset(Firmware_uart_rx_buf,0,FIRMWARE_UART_RX_BUF_SIZE);
			firmware_uart_rx_write_pos=0;
			uprintf("SET OK\r\n");
			return ;
			}
			else
			{
			 memset(Firmware_uart_rx_buf,0,FIRMWARE_UART_RX_BUF_SIZE);
			 firmware_uart_rx_write_pos=0;
				uprintf("SET ERR\r\n");
				return;
			}
		}	
		p=strstr((char*)Firmware_uart_rx_buf,"COMUPDATE");    //����յ����������������봮������ģʽ���ص�ͨѶģ���Դ����̼�FLASH��
		if(p)
		{
			memset(Firmware_uart_rx_buf,0,FIRMWARE_UART_RX_BUF_SIZE);
			firmware_uart_rx_write_pos=0;
			//uprintf("COMUPDATE READY OK\r\n");	
			uprintf("COM OK\r\n");
			comupdate_mode_flag=1;
			turn_off_sim_3v8();//�ص�ģ��ĵ�Դ���Է���������ʱ��ͬʱ����FTP����
			if(!firmware_flash_init()){
				return;
			}
			return ;
		}
	  memset(Firmware_uart_rx_buf,0,FIRMWARE_UART_RX_BUF_SIZE);
		firmware_uart_rx_write_pos=0;	
	}
 }
	
}

//void USART1_IRQHandler(void) 
//{
//	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //�����ж�
//	{
//		gps_rx_buf[gps_rx_buf_wr_pos] = USART_ReceiveData(USART1);//(USART1->DR);	//��ȡ���յ�������
//		
//		gps_rx_buf_wr_pos++;
//		if(gps_rx_buf_wr_pos >= GSP_RX_BUF_SIZE)
//		{
//			gps_rx_buf_wr_pos = 0;
//		}
//	}
//	
	
//	uint8_t data;	
//				if(USART_GetFlagStatus(USART1, USART_FLAG_ORE) != RESET)//���ڽ������
//	{
//			USART_ReceiveData(USART1);
//			USART_ClearFlag(USART1,USART_FLAG_ORE);		
//	}	
//	
//	/*�жϽ���*/		

//	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)//�����жϣ����յ������ݱ�����0x0d 0x0a��β��
//	{
//		data = USART_ReceiveData(USART1);

//	}
//}


