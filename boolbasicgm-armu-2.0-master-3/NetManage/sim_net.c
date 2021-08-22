#include "sim_net.h"
#include "define.h"
//#include "my_flash.h"

//#include "usb_interface.h"
//#include "sd_interface.h"
#include "user_config.h"
#include "sim_pwrkey.h"
#include "spi_flash.h"
#include "cmsis_os.h" 
#include "crc.h"
#include "power_ctl.h"
#include "GB17691A.h"
#include "string.h"
#include "obd.h"
#include "MultiSocket.h"
#include "TerminalTest.h"

#define __PATH   "F:/"
#define __PATH2   "C:/"

u32 iwdg_net=0;
extern u32 iwdg_17691;
static sim_status_t sim_net_status = SIM_NET_TURN_OFF;//POWER_OFF;
static uint8_t iccid_status = 0;
static uint8_t imei_status = 0;
//static uint8_t retry_cnt = 0;
static uint16_t sim_dealy_cnt = 0;
static uint8_t flag_upsoftware=0;   // =1 下载了新程序 下载完立即重启更新

u8 state_net0=0xff;  //第一链路连接状态  0代表链接成功
u8 state_net1=0xff;  //第二链路连接状态
u8 state_net2=0xff;  //第三链路连接状态

char IP_data[70] = GB17691_SERVER_IP_PORT;
static uint8_t SimcomModleVer = 1;

static char ftp_ip[70] = FTP_SERVER_IP_PORT;
//static char version_ip[50] = VERSION_SERVER_IP_PORT;

//static uint8_t timeout = 0;

//static char IP_data[50] = "\r\nAT+CIPOPEN=0,\"TCP\",\"120.220.14.212\",19006\r\n";  //沂星
//"\r\nAT+CIPOPEN=0,\"TCP\",\"123.127.164.36\",50001\r\n"; //通联
//static char ftp_ip[70] = "AT+CFTPSLOGIN=\"111.30.107.158\",60024,\"admin\",\"bool2018\",0\r\n";
//static char ftp_ip[70] = "AT+CFTPSLOGIN=\"67.208.93.232\",21,\"zyf\",\"zyf\",0\r\n";
//"AT+CFTPSLOGIN=\"111.30.107.158\",60024,\"admin\",\"bool2018\",0\r\n";
//static char IP_data[50] = "\r\nAT+CIPOPEN=0,\"TCP\",\"60.12.185.130\",39002\r\n";
//static char IP_data[60] = "\r\nAT+CIPOPEN=0,\"TCP\",\"tbox-pre.chechezhi.cn\",39002\r\n";
//"\r\nAT+CIPOPEN=0,\"TCP\",\"123.127.164.36\",50004\r\n";  //tonglian
//static char version_ip[50] = "\r\nAT+CIPOPEN=0,\"TCP\",\"120.220.14.212\",19006\r\n";  //沂星
//static char version_ip[50] = "\r\nAT+CIPOPEN=0,\"TCP\",\"119.6.100.171\",50006\r\n";  //成都

static uint8_t sim_net_data[1024];
static uint8_t temp_sim_net_data[1024];
static uint8_t temp_sim_net_data1[1024];
static uint8_t temp_sim_net_data2[1024];

static char imei_buf[IMEI_LEN];// = {0x38,0x36,0x31,0x33,0x31,0x31,0x30,0x30,0x39,0x39,0x35,0x39,0x31,0x32,0x32};//861311009959122
//static char imei_buf1[IMEI_LEN] = {0x38,0x36,0x38,0x38,0x32,0x31,0x30,0x34,0x33,0x30,0x30,0x38,0x32,0x36,0x37};// BOOL  CESHI01
//static char imei_buf1[IMEI_LEN] = {0x38,0x36,0x38,0x38,0x32,0x31,0x30,0x34,0x33,0x34,0x39,0x36,0x34,0x32,0x31};// 亚美		  BL0002
static char iccid_buf[ICCID_LEN];
static uint8_t fw_version_status = 0;
static char sim_firmware_version[50];
static uint8_t open_version_server_flag = 0;
static uint16_t sim_data_rd_pos = 0;
static uint32_t terminal_firmware;
static char device_type[32];
static uint32_t firmware_bin_size;

static uint32_t imei_location=0;
static uint32_t sum_version_location=0;
static uint32_t sum_UpdataMode_location=0;
static uint32_t location_offset;

struct FW_INFO{
	     uint32_t Txt_Version_Location;
       char Txt_Dev_Type[32];
	     char Txt_Fw_Version[8];
	     int Txt_Update_Mode;
	     uint32_t Txt_Updata_Mode_Location;
};
static struct FW_INFO fw_info[16];


extern uint8_t GB17691_CMD_data[256]; 
//static const at_cmd_t at_cmd[] = {
//	                                 {.send_cmd = "\r\nAT+CGATT?\r\n",.ack_str = "+CGATT: 1",.get_str = NULL,.timeout = 100,.try_cnt = 3},
//																	 {.send_cmd = "\r\nAT+FSCD=C:/\r\n",.ack_str = "OK",.get_str = NULL,.timeout = 100,.try_cnt = 3},
//																	 {.send_cmd = "\r\nAT+FSLS\r\n",.ack_str = "",.get_str = NULL,.timeout = 100,.try_cnt = 3},
//																	 {.send_cmd = "\r\nAT+CFTPSSINGLEIP=1\r\n",.ack_str = "OK",.get_str = NULL,.timeout = 100,.try_cnt = 3},
//																	 {.send_cmd = "\r\nAT+CFTPSSTART\r\n",.ack_str = "+CFTPSSTART: 0",.get_str = NULL,.timeout = 1000,.try_cnt = 3},
//																	 {.send_cmd = "\r\nAT+CFTPSLOGIN=\r\n",.ack_str = "+CFTPSLOGIN: 0",.get_str = NULL,.timeout = 10000,.try_cnt = 3},
//																	 {.send_cmd = "\r\nAT+CFTPSGETFILE=\r\n",.ack_str = "+CFTPSGETFILE: 0",.get_str = NULL,.timeout = 50000,.try_cnt = 3},
//																	 {.send_cmd = "\r\nAT+CFTPSLOGOUT\r\n",.ack_str = "+CFTPSLOGOUT: 0",.get_str = NULL,.timeout = 1000,.try_cnt = 3},
//																	 {.send_cmd = "\r\nAT+CFTPSSTOP\r\n",.ack_str = "+CFTPSSTOP: 0",.get_str = NULL,.timeout = 1000,.try_cnt = 3},
//                                 };

static int str_scanf(char *pcmpstr, char* pgetstr, uint16_t getstr_len, uint32_t timeout)
{
	uint16_t rx_buf_pos_temp;
	uint16_t i = 0;
	uint16_t rx_size = 0;
	uint16_t get_size = 0;
	char get_char;
	
	uint8_t scanf_cmpstr_flag = 0;
	uint16_t cmpstr_len = strlen(pcmpstr);
	
	if(timeout > 600000)
	{
		timeout = 600000;
	}
	timeout /= 10;
	
	do{
		iwdg_net++;
		//sim_data_wr_pos = get_sim_rx_buf_wr_pos();
		if(!scanf_cmpstr_flag)
		{
		  if(get_sim_rx_data_size(sim_data_rd_pos) >= cmpstr_len)
			{
				rx_buf_pos_temp = sim_data_rd_pos;
				
				for(i=0; i<cmpstr_len;i++)
				{
					get_char = get_sim_rx_byte(rx_buf_pos_temp);
					if(pcmpstr[i] != get_char)//sim_net_data_buf[rx_buf_pos_temp])
					{
						break;
					}
					else
					{
					  rx_buf_pos_temp++;
					  if(rx_buf_pos_temp >= SIM_UART_RX_BUF_SIZE)
					  {
						  rx_buf_pos_temp = 0;
					  }
					}
				}

        if(i==cmpstr_len)
				{
					scanf_cmpstr_flag = 1;
					sim_data_rd_pos = rx_buf_pos_temp;
					if(pgetstr == NULL)
					{
						return 0;
					}
				}
				else
				{
					sim_data_rd_pos++;
					if(sim_data_rd_pos >= SIM_UART_RX_BUF_SIZE)
					{
						sim_data_rd_pos = 0;
					}
				}
			}
			else
			{
				if(timeout>0)
				{
					timeout--;
					osDelay(10);
				}
				else
				{
					return -1;
				}
			}
	  }
		else
		{
			rx_size = get_sim_rx_data_size(sim_data_rd_pos);
			if(rx_size > 0)
			{
				if(pgetstr != NULL)
				{
					for(i = 0; i < rx_size; i++)
					{
						get_char = get_sim_rx_byte(sim_data_rd_pos);//sim_net_data_buf[sim_data_rd_pos];
						
						sim_data_rd_pos++;
						if(sim_data_rd_pos >= SIM_UART_RX_BUF_SIZE)
						{
							sim_data_rd_pos = 0;
						}
						
						if(get_char == '\n')
						{
							return 0;
						}
						if(get_char != '\r')
						{
						  pgetstr[get_size] = get_char;
							get_size++;
							if(get_size >= getstr_len)
							{
								return 0;
							}
						}
					}
				}
			}
			else
			{
				if(timeout>0)
				{
					timeout--;
					osDelay(10);
				}
				else
				{
					return -1;
				}
			}
		}
  }while(1);
}


//static void get_sim_uart_str(char* pchar, uint16_t max_size, uint32_t timeout)
//{
//	uint8_t data;
//	uint32_t time_out = timeout / 5;
//	while((time_out--)>0)
//	{
//		if(get_sim_rx_data_size(sim_data_rd_pos)>0)
//		{
//			data = get_sim_rx_byte(sim_data_rd_pos);
//		}
//		else
//		{
//		  vTaskDelay(10);
//		}
//	}
//}


uint8_t at_cmd_process(at_cmd_t* at_cmd)
{
	uint8_t res;
	uint8_t done = 0;
	uint8_t retry_cnt = 0;
	sim_data_rd_pos = get_sim_rx_buf_wr_pos(); //清零接收
	do{
		iwdg_net++;
		iwdg_17691++;
		//sim_uart_tx((uint8_t*)at_cmd->send_cmd, strlen(at_cmd->send_cmd));
		do
		{
		  res = sim_uart_tx((uint8_t*)at_cmd->send_cmd, strlen(at_cmd->send_cmd));  //需要判断串口BUF是否满？
		}while(!res);
		
//		sd_write_data(NULL,(uint8_t*)at_cmd->send_cmd, strlen(at_cmd->send_cmd), ACSII_DATA,SEND);
#ifdef debug
   USARTSendOut(USART1, "\r\n", 2);		
	 USARTSendOut(USART1, (const char *)at_cmd->send_cmd, strlen(at_cmd->send_cmd));	 // 
   USARTSendOut(USART1, "\r\n", 2);
#endif
		
		if((str_scanf(at_cmd->ack_str, at_cmd->get_str, at_cmd->get_size, at_cmd->timeout))==0)
		{
			done = 1;
			return done;
		}
		else
		{
			retry_cnt++;
			if(retry_cnt >= at_cmd->try_cnt)
			{
				done = 0;
				return done;
			}
		}
	}while(1);
}

static uint8_t at_cgatt(void)
{
	at_cmd_t cmd;
	cmd.send_cmd = "\r\nAT+CGATT?\r\n";
	cmd.ack_str = "+CGATT: 1";
	cmd.get_str = NULL;
	cmd.get_size = 0;
	cmd.timeout = 100;
	cmd.try_cnt = 3;
	if(at_cmd_process(&cmd))
	{
		return 1;
	}
	else
	{
		return 0;
	}	
}

static uint8_t at_fscd(char* path)
{
	char send_char[50] = {0};
	uint8_t offset = 0;
	at_cmd_t cmd;
	memcpy(send_char,"\r\nAT+FSCD=",strlen("\r\nAT+FSCD="));
	offset = strlen("\r\nAT+FSCD=");
	for(uint8_t i=0;i<strlen(path);i++)
	{
		send_char[offset++] = path[i];
	}
	send_char[offset++] = '\r';
	send_char[offset++] = '\n';
	send_char[offset++] = '\0';
	cmd.send_cmd = send_char;//"\r\nAT+FSCD=C:/\r\n";
	cmd.ack_str = "OK";
	cmd.get_str = NULL;
	cmd.get_size = 0;
	cmd.timeout = 100;
	cmd.try_cnt = 3;
	if(at_cmd_process(&cmd))
	{
		return 1;
	}
	else
	{
		return 0;
	}	
}

static uint8_t at_fsls(void)
{
  at_cmd_t cmd;
	cmd.send_cmd = "\r\nAT+FSLS\r\n";
	cmd.ack_str = "OK";
	cmd.get_str = NULL;
	cmd.get_size = 0;
	cmd.timeout = 5000;
	cmd.try_cnt = 3;
	if(at_cmd_process(&cmd))
	{
		return 1;
	}
	else
	{
		return 0;
	}	
}

static uint8_t at_fsdel(char *fl_name)
{
	at_cmd_t cmd;
	uint8_t offset;
	char send_char[100];
	memset(send_char,0,sizeof(send_char));
	memcpy(send_char,"\r\nAT+FSDEL=",strlen("\r\nAT+FSDEL="));
	offset = strlen("\r\nAT+FSDEL=");
	send_char[offset++] = '"';
	for(uint8_t i=0;i<strlen(fl_name);i++)
	{
		send_char[offset++] = fl_name[i];
	}
	send_char[offset++] = '"';
	send_char[offset++] = '\r';
	send_char[offset++] = '\n';
	send_char[offset++] = '\0';
	cmd.send_cmd = send_char;
	cmd.ack_str = "OK";
	cmd.get_str = NULL;
	cmd.get_size = 0;
	cmd.timeout = 2000;
	cmd.try_cnt = 3;
	if(at_cmd_process(&cmd))
	{
		return 1;
	}
	else
	{
		return 0;
	}	
}

static uint8_t at_cftpssingleip(void)
{
	at_cmd_t cmd;
	cmd.send_cmd = "\r\nAT+CFTPSSINGLEIP=1\r\n";
	cmd.ack_str = "OK";
	cmd.get_str = NULL;
	cmd.get_size = 0;
	cmd.timeout = 100;
	cmd.try_cnt = 3;
	if(at_cmd_process(&cmd))
	{
		return 1;
	}
	else
	{
		return 0;
	}	
}

static uint8_t at_cftpsstart(void)
{
	at_cmd_t cmd;
	cmd.send_cmd = "\r\nAT+CFTPSSTART\r\n";
	cmd.ack_str = "+CFTPSSTART: 0";
	cmd.get_str = NULL;
	cmd.get_size = 0;
	cmd.timeout = 1000;
	cmd.try_cnt = 3;
	if(at_cmd_process(&cmd))
	{
		return 1;
	}
	else
	{
		return 0;
	}	
}

static uint8_t at_cfslogin(void)
{
	at_cmd_t cmd;
	char get_char[50];
	uint8_t retry = 3;
	do{
		memset(get_char,0,sizeof(get_char));
		cmd.send_cmd = ftp_ip;
		cmd.ack_str = "+CFTPSLOGIN: ";
		cmd.get_str = get_char;
		cmd.get_size = sizeof(get_char);
		cmd.timeout = 20000;
		cmd.try_cnt = retry;
		if(at_cmd_process(&cmd))
		{
			int res;
			if(sscanf(get_char,"%d",&res)==1)
			{
				if(res==0)
				{
					return 1;
				}
			}
			retry--;
			if(retry==0)
			{
			  return 0;
			}
		}
		else
		{
			return 0;
		}	
  }while(1);
}

static uint8_t at_cftpgetfile(char* file,uint32_t timeout,uint8_t retry)
{
	at_cmd_t cmd;
	uint8_t i=0;
	char get_char[50] = {0};
  char send_char[100] = {0};
  memcpy(send_char,"\r\nAT+CFTPSGETFILE=",strlen("\r\nAT+CFTPSGETFILE="));
	uint8_t offset = strlen("\r\nAT+CFTPSGETFILE=");
	send_char[offset++] = '"';
	send_char[offset++] = '/';
  memcpy(&send_char[offset],__CUSTOMER_NAME,strlen(__CUSTOMER_NAME));
	offset += strlen(__CUSTOMER_NAME);
	send_char[offset++] = '/';
  memcpy(&send_char[offset],__DEVICE_TYPE,strlen(__DEVICE_TYPE));
	offset += strlen(__DEVICE_TYPE);
	send_char[offset++] = '/';
	for(i=0;i<strlen(file);i++)
	{
		send_char[offset++] = file[i];
	}
	send_char[offset++] = '"';
	send_char[offset++] = ',';
	if((SimcomModleVer == 4)||(SimcomModleVer == 5)||(SimcomModleVer == 6))
{
	send_char[offset++] = '1';//'0';
}
else
{
	send_char[offset++] = '0';//'0';
}
send_char[offset++] = '\r';
	send_char[offset++] = '\n';
	send_char[offset++] = '\0';
	
	do{
		iwdg_net++;
		memset(get_char,0,sizeof(get_char));
		cmd.send_cmd = send_char;
		cmd.ack_str = "+CFTPSGETFILE: ";
		cmd.get_str = get_char;
		cmd.get_size = sizeof(get_char);
		cmd.timeout = timeout;
		cmd.try_cnt = retry;
		if(at_cmd_process(&cmd))
		{
			int res;
      if(sscanf(get_char,"%d",&res)==1)
			{
				if(res==0)
				{
					return 1;
				}
				if(res==9)
				{
					return 0;
				}				

			}				
			retry--;
			if(retry==0)
			{
			  return 0;
			}
		}
		else
		{
			return 0;
		}
  }while(1);	
}

/*************************************************************************************
功能：激活PDP上下文，获取IP地址
作者：黎尧才
参数：void
返回值：1：操作成功 0：操作失败
**************************************************************************************/
static uint8_t at_cgpaddr(void){
      at_cmd_t cmd;
			cmd.send_cmd = "\r\nAT+CGPADDR\r\n";
			cmd.ack_str = "+CGPADDR: ";
			cmd.get_str = NULL;
			cmd.get_size = 0;
			cmd.timeout = 5000;
			cmd.try_cnt = 3;
			if(at_cmd_process(&cmd))
			{
				return 1;
			}
			else
			{
				return 0;
			}			
}

///******************************************************************************************
//功能：中止FTP所有操作
//作者：黎尧才
//参数：void
//返回值：1：操作成功  0：操作失败
//*******************************************************************************************/
//static uint8_t at_cftpsabort(void)
//{
//  at_cmd_t cmd;
//	cmd.send_cmd = "\r\nAT+CFTPSABORT\r\n";
//	cmd.ack_str = "+CFTPSABORT:";
//	cmd.get_str = NULL;
//	cmd.get_size = 0;
//	cmd.timeout = 5000;
//	cmd.try_cnt = 3;
//	if(at_cmd_process(&cmd))
//	{
//		return 1;
//	}
//	else
//	{
//		return 0;
//	}	
//}
/******************************************************************************************
功能：从FTP服务器下载文件到模块RAM中
作者：黎尧才
参数：file:下载文件名 timeout: AT指令执行超时时间，retry: AT指令重试次数， size :下载文件位置偏移
返回值：1：下载成功  0：下载失败
*******************************************************************************************/
static uint8_t at_cftpget(char* file,uint32_t timeout,uint8_t retry,uint32_t size)
{
	at_cmd_t cmd;
	uint8_t i=0;
	char get_char[50] = {0};
  char send_char[100] = {0};

	char re_success1[6]="GET: 0";
	char re_success2[11]=":RECV EVENT";
	char re_fail1[6]="GET: 9";

  memcpy(send_char,"\r\nAT+CFTPSGET=",strlen("\r\nAT+CFTPSGET="));
	uint8_t offset = strlen("\r\nAT+CFTPSGET=");
	send_char[offset++] = '"';
	send_char[offset++] = '/';
  memcpy(&send_char[offset],__CUSTOMER_NAME,strlen(__CUSTOMER_NAME));
	offset += strlen(__CUSTOMER_NAME);
	send_char[offset++] = '/';
  memcpy(&send_char[offset],__DEVICE_TYPE,strlen(__DEVICE_TYPE));
	offset += strlen(__DEVICE_TYPE);
	send_char[offset++] = '/';
	for(i=0;i<strlen(file);i++)
	{
		send_char[offset++] = file[i];
	}
	send_char[offset++] = '"';
	send_char[offset++] = ',';
	
	sprintf(&send_char[offset],"%d", size);
	offset = strlen(send_char);
	send_char[offset++] = ',';
	
	send_char[offset++] = '1';
	send_char[offset++] = '\r';
	send_char[offset++] = '\n';
	send_char[offset++] = '\0';
	
	do{
		iwdg_net++;
		memset(get_char,0,sizeof(get_char));
		cmd.send_cmd = send_char;
//	  cmd.ack_str = "+CFTPSGET: ";
	  cmd.ack_str = "+CFTPS";
		cmd.get_str = get_char;
//		cmd.get_size = 0;
		cmd.get_size = 10;
		cmd.timeout = timeout;
		cmd.try_cnt = retry;
		if(at_cmd_process(&cmd))
		{
//			int res;
//      if(sscanf(get_char,"%d",&res)==1)
//			{
//				if(res==0)
//				{
//					return 1;
//				}
//				if(res==9)
//				{
//					return 0;
//				}				

//			}				
			int res;
			if((memcmp(get_char,re_success1,6))||(memcmp(get_char,re_success2,11)) )
				return 1;
			if(memcmp(get_char,re_fail1,6)) 
				return 0;			
			
			retry--;
			if(retry==0)
			{
			  return 0;
			}
		}
		else
		{
			return 0;
		}
  }while(1);	

}

/******************************************************************************************
功能：查看从FTP服务器下载到RAM中文件大小，并将响应值返回
作者：黎尧才
参数：void
返回值：返回指针
*******************************************************************************************/
static char* at_cftpscacherd(void)
{
	at_cmd_t cmd;
	char get_char[50]={0};
	char* res=NULL;
	
	cmd.send_cmd = "\r\nAT+CFTPSCACHERD?\r\n";
	cmd.ack_str = "+CFTPSCACHERD: ";
	cmd.get_str = get_char;
	cmd.get_size = sizeof(get_char);
	cmd.timeout = 5000;
	cmd.try_cnt = 5;
	if(at_cmd_process(&cmd))
	{
		res =get_char;
		return res;
	}
	else
	{
		return res;
	}	
}

/******************************************************************************************
功能：从模块RAM输出文件至串口?最大1024字节，并将响应值返回
作者：黎尧才
参数：void
返回值：命令响应值
*******************************************************************************************/
static char* at_cftpscacherd_To_UART(void)
{
	at_cmd_t cmd;
	char get_char[50]={0};
	char* res=NULL;
	
	cmd.send_cmd = "\r\nAT+CFTPSCACHERD\r\n";
	cmd.ack_str = "+CFTPSGET: ";
	//cmd.ack_str = "OK";
	cmd.get_str = get_char;
	cmd.get_size = sizeof(get_char);
	cmd.timeout = 5000;
	cmd.try_cnt = 5;
	if(at_cmd_process(&cmd))
	{
		res =get_char;
		return res;
	}
	else
	{
		return res;
	}	
}

/******************************************************************************************
功能：从模块RAM输出整个固件文件至串口，并存入MCU的FLASH
作者：黎尧才
参数：void
返回值：1 获得文件成功  0 获得文件失败
*******************************************************************************************/
static uint8_t get_firmware1(void)
{
//	char str_buf[50]={0};
	char get_char[50]={0};
	char get_char1[50]={0};
	uint32_t data_len;
	uint32_t firmware_bin_len=0;
	uint8_t m,n;
  
	firmware_bin_size = 0;
	char* pchar1=at_cftpscacherd();
  if(pchar1 == NULL)
		{
			return 0;
		}
		memcpy(get_char1,pchar1,strlen(pchar1));
		
		sscanf(get_char1,"%d",&firmware_bin_len);
		if(firmware_bin_len>96*1024){  //stm32f105 flash  放应用程序最大96K
				return 0;
		}else{
			m=firmware_bin_len/1024;
			if(firmware_bin_len%1024!=0){
				m=m+1;
			}
 	 }
		
	 for(n=0;n<m;n++)
	 {
	  osDelay(40);  //最大发1024个字节
		memset(get_char,0,sizeof(get_char));
		char* pchar =at_cftpscacherd_To_UART();
		if(pchar == NULL)
		{
			return 0;
		}
		memcpy(get_char,pchar,strlen(pchar));
		
		if(strcmp(get_char,"0")==0)
		{
			return 1;
		}
		else
		{
			
			if(sscanf(get_char,"DATA,%d",&data_len)==1)
			{
				if(data_len>1024)
				{
					return 0;
				}
				
				uint8_t retry = 3;
				do{
					if(get_sim_rx_data_size(sim_data_rd_pos) >= data_len)
					{
						break;
					}
					else
					{
						if(retry>0)
						{
							retry--;
							osDelay(40);  //最大发1024个字节
						}
						else
						{	
							return 0;
						}
					}
				}while(1);
				
				get_sim_data_multiple_bytes(sim_net_data, sim_data_rd_pos, data_len);
				sim_data_rd_pos += data_len;
				if(sim_data_rd_pos>=SIM_UART_RX_BUF_SIZE)
				{
					sim_data_rd_pos -= SIM_UART_RX_BUF_SIZE;
				}
				
				if(!write_data_to_flash(sim_net_data,FIRMWARE_START_ADDR+firmware_bin_size,data_len,FIRMWARE_DATA_FLASH_AREA))
				{
					return 0;
				}
				
				firmware_bin_size += data_len;
				if(firmware_bin_size > TERMINAL_FIRMWARE_MAX_SIZE)  //128K byte
				{
					return 0;
				}
				
				if((firmware_bin_size==firmware_bin_len)||(data_len<1024))
				{
					USARTSendOut(USART1, "\r\nFirmware update success,Please reset！！！！！！！！！！！！！！\r\n", strlen("\r\nFirmware update success,Please reset！！！！！！！！！！！！！！\r\n"));
					flag_upsoftware=1;
					return 1;
				}
			}
			else
			{
				return 0;
			}
		}
		
//		if(str_scanf("+CFTPSGET:0", NULL, NULL, 1000)!=0)
//		{
//			return 0;
//		}
		
	}
	 return 0;
}


static uint8_t at_cftpsstop(void)
{
	at_cmd_t cmd;
	cmd.send_cmd = "\r\nAT+CFTPSSTOP\r\n";
	cmd.ack_str = "+CFTPSSTOP: 0";
	cmd.get_str = NULL;
	cmd.get_size = 0;
	cmd.timeout = 5000;
	cmd.try_cnt = 5;
	if(at_cmd_process(&cmd))
	{
		return 1;
	}
	else
	{
		return 0;
	}	
}

static char* at_cftrantx(char* path, char* file, uint32_t* location, uint32_t size)
{
	at_cmd_t cmd;
	uint32_t i=0;
  char send_char[100] = {0};
	char get_char[50] = {0};
	char* res = NULL;
	
	memcpy(send_char,"\r\nAT+CFTRANTX=",strlen("\r\nAT+CFTRANTX="));
	uint8_t offset = strlen("\r\nAT+CFTRANTX=");
	send_char[offset++] = '"';
	for(i=0;i<strlen(path);i++)
	{
		send_char[offset++] = path[i];
	}
	for(i=0;i<strlen(file);i++)
	{
		send_char[offset++] = file[i];
	}
	send_char[offset++] = '"';
	if(location != NULL)
	{
		send_char[offset++] = ',';
    sprintf(&send_char[offset],"%d,%d",*location, size);
	}
	offset = strlen(send_char);
	send_char[offset++] = '\r';
	send_char[offset++] = '\n';
	send_char[offset++] = '\0';

	cmd.send_cmd = send_char;
	cmd.ack_str = "+CFTRANTX: ";
	cmd.get_str = get_char;
	cmd.get_size = sizeof(get_char);
	cmd.timeout = 5000;
	cmd.try_cnt = 3;
	if(at_cmd_process(&cmd))
	{
		res = get_char;
		return res;
	}
	else
	{
		return res;
	}
}

static uint8_t at_cftpslogout(void)
{
	at_cmd_t cmd;
	cmd.send_cmd = "\r\nAT+CFTPSLOGOUT\r\n";
	cmd.ack_str = "+CFTPSLOGOUT: 0";
	cmd.get_str = NULL;
	cmd.get_size = 0;
	cmd.timeout = 5000;
	cmd.try_cnt = 5;
	if(at_cmd_process(&cmd))
	{
		return 1;
	}
	else
	{
		return 0;
	}	
}

//static uint8_t at_cftpslist(void)
//{
//	at_cmd_t cmd;
//	uint8_t i=0;
//  char send_char[100] = {0};
//  memcpy(send_char,"\r\nAT+CFTPSLIST=",strlen("\r\nAT+CFTPSLIST="));
//	uint8_t offset = strlen("\r\nAT+CFTPSLIST=");
//	send_char[offset++] = '"';
//	send_char[offset++] = '/';
//  memcpy(&send_char[offset],__CUSTOMER_NAME,strlen(__CUSTOMER_NAME));
//	offset += strlen(__CUSTOMER_NAME);
//	send_char[offset++] = '/';
//  memcpy(&send_char[offset],__DEVICE_TYPE,strlen(__DEVICE_TYPE));
//	offset += strlen(__DEVICE_TYPE);
//	send_char[offset++] = '"';
//	send_char[offset++] = '\r';
//	send_char[offset++] = '\n';
//	send_char[offset++] = '\0';
//	cmd.send_cmd = send_char;
//	cmd.ack_str = "+CFTPSLIST: ";
//	cmd.get_str = NULL;
//	cmd.get_size = 0;
//	cmd.timeout = 5000;
//	cmd.try_cnt = 3;
//	if(at_cmd_process(&cmd))
//	{
//		return 1;
//	}
//	else
//	{
//		return 0;
//	}	
//}

//static uint8_t delete_file_txt(void)
//{
//	at_cmd_t cmd;
//	uint8_t i;
//	char str_buf[50];
//	char get_char[50];
//	uint8_t done = 0;
//	
//	cmd.send_cmd = "\r\nAT+FSLS\r\n";
//	sim_uart_tx((uint8_t*)cmd.send_cmd, strlen(cmd.send_cmd));
//	uint8_t retry = 3;
//	do{
//		memset(get_char,0,sizeof(get_char));
//		if((str_scanf(__DEVICE_TYPE,get_char, sizeof(get_char), 1500))==0)
//		{
//			if(strcmp(get_char,".txt")==0)
//			{
//				uint8_t offset;
//				memset(str_buf,0,sizeof(str_buf));
//				memcpy(str_buf,"\r\nAT+FSDEL=",strlen("\r\nAT+FSDEL="));
//				offset = strlen("\r\nAT+FSDEL=");
//				str_buf[offset++] = '"';
//				memcpy(&str_buf[offset],__DEVICE_TYPE,strlen(__DEVICE_TYPE));
//				offset += strlen(__DEVICE_TYPE);

//				for(i=0;i<strlen(get_char);i++)
//				{
//					str_buf[offset++] = get_char[i];
//				}
//				str_buf[offset++] = '"';
//				str_buf[offset++] = '\r';
//				str_buf[offset++] = '\n';
//				str_buf[offset++] = '\0';
//				cmd.send_cmd = str_buf;
//				cmd.ack_str = "OK";
//				cmd.get_str = NULL;
//				cmd.get_size = 0;
//				cmd.timeout = 100;
//				cmd.try_cnt = 3;
//				if(at_cmd_process(&cmd))
//				{
//					done = 1;
//					break;
//				}
//			}
//		}
//		retry--;
//	}while(retry>0);
//	
//	return done;
//}

//static uint8_t delete_file_bin(void)
//{
//	at_cmd_t cmd;
//	uint8_t i;
//	char str_buf[50];
//	char get_char[50];
//	uint8_t done = 0;
//	
//	cmd.send_cmd = "\r\nAT+FSLS\r\n";
//	sim_uart_tx((uint8_t*)cmd.send_cmd, strlen(cmd.send_cmd));
//	uint8_t retry = 3;
//	do{
//		memset(get_char,0,sizeof(get_char));
//		if((str_scanf(__DEVICE_TYPE,get_char, sizeof(get_char), 1500))==0)
//		{
//			if(strcmp(&get_char[9],".bin")==0)
//			{
//				uint8_t offset;
//				memset(str_buf,0,sizeof(str_buf));
//				memcpy(str_buf,"\r\nAT+FSDEL=",strlen("\r\nAT+FSDEL="));
//				offset = strlen("\r\nAT+FSDEL=");
//				str_buf[offset++] = '"';
//				memcpy(&str_buf[offset],__DEVICE_TYPE,strlen(__DEVICE_TYPE));
//				offset += strlen(__DEVICE_TYPE);
//				for(i=0;i<strlen(get_char);i++)
//				{
//					str_buf[offset++] = get_char[i];
//				}
//				str_buf[offset++] = '"';
//				str_buf[offset++] = '\r';
//				str_buf[offset++] = '\n';
//				str_buf[offset++] = '\0';
//				cmd.send_cmd = str_buf;
//				cmd.ack_str = "OK";
//				cmd.get_str = NULL;
//				cmd.get_size = 0;
//				cmd.timeout = 100;
//				cmd.try_cnt = 3;
//				if(at_cmd_process(&cmd))
//				{
//					done = 1;
//					break;
//				}
//			}
//		}
//		retry--;
//	}while(retry>0);
//	
//	return done;
//}

//static uint8_t check_device_info(uint8_t* pdata, uint16_t* len, char* check_str)
//{
//	char str_cache[30];
//	uint8_t str_len = strlen(check_str);
//	do{
//		if(*len >= str_len)
//		{
//			memset(str_cache,0,sizeof(str_cache));
//			memcpy(str_cache,pdata,str_len);
//			if(strcmp(str_cache,check_str)==0)
//			{
//				(*len) -= str_len;
//				for(uint16_t i=0;i<(*len);i++)
//				{
//					pdata[i] = pdata[i+str_len];
//				}
//				return 1;
//			}
//			else
//			{
//				(*len)--;
//				for(uint16_t i=0;i<(*len);i++)
//				{
//					pdata[i] = pdata[i+1];
//				}
//			}
//		}
//		else
//		{
//			return 0;
//		}
//  }while(1);
//}


/******************************************************************************************
功能:从指定范围查找，是否有指定字符串，并计算查找的偏移量
作者：黎尧才
参数：uint8_t* pdata 查找范围的指针，uint16_t* len查找范围的长度，char* check_str 指定查找的字符串
      uint32_t* offset 查找的偏移量
返回值1: 查找到指定字符   0 : 没有查找到指定字符 
*******************************************************************************************/
static uint8_t check_device_info1(uint8_t* pdata, uint16_t* len, char* check_str,uint32_t* offset) //
{
	char str_cache[30];
	uint8_t str_len = strlen(check_str);
	*offset=0;         //一次检查偏移量的绝对值

	do{
		if(*len >= str_len)
		{
			memset(str_cache,0,sizeof(str_cache));
			memcpy(str_cache,pdata,str_len);
			if(strcmp(str_cache,check_str)==0)
			{
				(*len) -= str_len;
				for(uint16_t i=0;i<(*len);i++)
				{
					pdata[i] = pdata[i+str_len];
				}
				(*offset)=(*offset)+str_len;
				return 1;
			}
			else
			{
				(*len)--;
				for(uint16_t i=0;i<(*len);i++)
				{
					pdata[i] = pdata[i+1];
				}
				(*offset)++;
			}
		}
		else
		{
			return 0;
		}
  }while(1);
}

/******************************************************************************************
功能：从模块FLASH读取TXT文件，从TXT文件中获取升级信息，并判断是否升级
作者：黎尧才
参数：void
返回值res 1: 升级  0 : 不升级
*******************************************************************************************/
static uint8_t is_update_firmware1(void)
{
	char send_char[50];
	char get_char[50];
	uint16_t i;
	uint8_t retry = 3;
	uint16_t data_len = 0;
	uint16_t temp_data_len=0;
	uint16_t temp_data_len1=0;
	uint16_t temp_data_len2=0;
	
	uint8_t res = 0;
	uint32_t get_tx_size;
	uint32_t get_file_size = 0;
	uint8_t j=0,k=0,l=0;
	
	memset(send_char,0,sizeof(send_char));
	memcpy(send_char,__DEVICE_TYPE,strlen(__DEVICE_TYPE));
	i = strlen(__DEVICE_TYPE);
	send_char[i++] = '.';
	send_char[i++] = 't';
	send_char[i++] = 'x';
	send_char[i++] = 't';
	send_char[i++] = '\0';
	imei_location=0;
	sum_version_location=0;
	sum_UpdataMode_location=0;
	uint8_t imei_flag=0;
	uint8_t dev_type_flag=0;
	uint8_t check_head_flag=0;
	uint8_t check_fw_head_flag=0;
	uint8_t check_UpdataMode_head_flag=0;
	
	do{
		iwdg_net++;
		memset(get_char,0,sizeof(get_char));
char* pchar;
	if((SimcomModleVer == 4)||(SimcomModleVer == 5)||(SimcomModleVer == 6))
	{
		pchar = at_cftrantx(__PATH2,send_char,&get_file_size,512);
	}
	else
	{
		pchar = at_cftrantx(__PATH,send_char,&get_file_size,512);
	}
		if(pchar == NULL)
		{
			break;
		}
		memcpy(get_char,pchar,strlen(pchar));
		if(strcmp(get_char,"0")==0)
		{
			break;
		}
		
		if(sscanf(get_char,"DATA,%d",&get_tx_size)!=1)
		{	
			break;
		}
		
		if(get_tx_size > 512)
		{
			break;
		}
		
		retry = 3;
		do{
			if(get_sim_rx_data_size(sim_data_rd_pos) >= get_tx_size)
			{
				break;
			}
			else
			{
				if(retry>0)
				{
					retry--;
			    osDelay(20);  //最大发512个字节
				}
				else
				{				
					return res;
				}
			}
		}while(1);
		
		get_sim_data_multiple_bytes(&sim_net_data[data_len], sim_data_rd_pos, get_tx_size);
		
		memcpy(temp_sim_net_data+temp_data_len,sim_net_data+data_len,get_tx_size);
		
		memcpy(temp_sim_net_data1+temp_data_len1,sim_net_data+data_len,get_tx_size);
		
		memcpy(temp_sim_net_data2+temp_data_len2,sim_net_data+data_len,get_tx_size);
		
		sim_data_rd_pos += (uint16_t)get_tx_size;
		if(sim_data_rd_pos>=SIM_UART_RX_BUF_SIZE)
		{
			sim_data_rd_pos -= SIM_UART_RX_BUF_SIZE;
		}
		get_file_size += (uint16_t)get_tx_size;
		data_len += (uint16_t)get_tx_size;
		
    temp_data_len+=(uint16_t)get_tx_size;
	  temp_data_len1+=(uint16_t)get_tx_size;
		temp_data_len2+=(uint16_t)get_tx_size;
			
    //从TXT文本中获取 设备设备类型信息，存入结构体数组中		
	  do{	
			  if(check_head_flag==0){
					  if(temp_data_len<strlen(__DEVICE_TYPE)){
							  break;
							}
				    if(check_device_info1(temp_sim_net_data, &temp_data_len, "DEVICE_TYPE:",&location_offset)){
							check_head_flag = 1;
						}
				}
				
				if(check_head_flag==1){
				   if(temp_data_len>=strlen(__DEVICE_TYPE)){
					   memset(fw_info[j].Txt_Dev_Type,0,sizeof(fw_info[j].Txt_Dev_Type));
						 memcpy(fw_info[j].Txt_Dev_Type,temp_sim_net_data,strlen(__DEVICE_TYPE)); 
						 j++;
					    temp_data_len -= strlen(__DEVICE_TYPE);
						  for(i=0;i<temp_data_len;i++)
							{
								temp_sim_net_data[i] = temp_sim_net_data[i+strlen(__DEVICE_TYPE)];
							} 
							check_head_flag=0;
							if(temp_data_len<strlen(__DEVICE_TYPE)){
							  break;
							}		
						}else{
							break;
						}
					 
				}		
		}while(1);
		
	 //从TXT文本中获取 设备升级固件版本信息，存入结构体数组中
		do{	
			  if(check_fw_head_flag==0){
					  if(temp_data_len1<strlen("FIRMWARE_VERSION:")){
							  break;
							}
				    if(check_device_info1(temp_sim_net_data1, &temp_data_len1, "FIRMWARE_VERSION:",&location_offset)){
							check_fw_head_flag = 1;
							sum_version_location+=location_offset;
						  fw_info[k].Txt_Version_Location=sum_version_location;
						}else{
						  sum_version_location+=location_offset; 
						}
				}
			  
				if(check_fw_head_flag==1){
					if(temp_data_len1>=8)
					{
						memset(fw_info[k].Txt_Fw_Version,0,sizeof(fw_info[k].Txt_Fw_Version));
						memcpy(fw_info[k].Txt_Fw_Version,temp_sim_net_data1,8); //版本号8个字节
						k++;
					    temp_data_len1 -= 8;
							for(i=0;i<temp_data_len1;i++)
							{
								temp_sim_net_data1[i] = temp_sim_net_data1[i+8];
							}
							sum_version_location+=8;
							check_fw_head_flag=0;
						 if(temp_data_len1<strlen("FIRMWARE_VERSION:")){
							  break;
							}		
					}else{
						  break;
					}
				}
		}while(1);
		
    //从TXT文本中获取 升级模式信息 存入结构体数组中
		do{	
			  if(check_UpdataMode_head_flag==0){
					  if(temp_data_len2<strlen("UPDATE_MODE:")){
							  break;
							}
				    if(check_device_info1(temp_sim_net_data2, &temp_data_len2, "UPDATE_MODE:",&location_offset)){
							check_UpdataMode_head_flag = 1;
							sum_UpdataMode_location+=location_offset;
							fw_info[l].Txt_Updata_Mode_Location=sum_UpdataMode_location;
						}else{
						  sum_UpdataMode_location+=location_offset;
						}
				}
			  
				if(check_UpdataMode_head_flag==1){
					if(temp_data_len2>=1)
					{
							fw_info[l].Txt_Update_Mode=temp_sim_net_data2[0];
						  l++;
							temp_data_len2 -= 1;
								for(i=0;i<temp_data_len2;i++){
									temp_sim_net_data2[i] = temp_sim_net_data2[i+1];
								}
							check_UpdataMode_head_flag=0;
						  if(temp_data_len2<strlen("UPDATE_MODE:")){
							  break;
							}		
					}else{
						  break;
					}
				}
		}while(1);
		
	  //从TXT文本中获取，IMEI信息	
		//step_flag=3;
	//	if(step_flag==3)//找IMEI号
		//{
		//	if(check_head_flag==0)
		//	{
				char str_buf[50];
				memset(str_buf,0,sizeof(str_buf));
				memcpy(str_buf,imei_buf,sizeof(imei_buf));
				
				if(check_device_info1(sim_net_data, &data_len, str_buf,&location_offset))
				{
				  //res = 1;
					imei_location+=location_offset;
					imei_flag=1;
				  break;
				}else{
					imei_location+=location_offset;
				}
		//	}
		//}
	
		if(str_scanf("+CFTRANTX: 0", NULL, NULL, 1000)!=0)
		{
			break;
		}
		else
		{
			//if(res ||(get_tx_size<512))
			if(imei_flag ||(get_tx_size<512))	
			{
				break;
			}
		}
  }while(1);
	
	if(imei_flag==1){
		do{
			if(fw_info[k-1].Txt_Version_Location>imei_location){
				k--;			
				}else{
					char my_type[DEVICE_TYPE_MAX_LEN+1];
					memset(device_type, 0, sizeof(device_type));
					memcpy(device_type,fw_info[k-1].Txt_Dev_Type,strlen(__DEVICE_TYPE));
					memset(my_type,0,sizeof(my_type));
					memcpy(my_type,__DEVICE_TYPE,strlen(__DEVICE_TYPE));
					for(i=0;i<(strlen(__DEVICE_TYPE)-2);i++)//后两位不用匹配
					{
						if(my_type[i] != device_type[i])
						{
							res=0;
							return res;
						}
					}
			   if(sscanf(fw_info[k-1].Txt_Fw_Version,"%08x",&terminal_firmware)==1)
					{		
						if(terminal_firmware > CURRENT_VER)
						{
						  res=1;
						}
						else
						{
							res=0;
						}
					}
				break;
			}	
			if(k==0){
				break;
			}	
		}while(1);
	}else{
		do{	
			k--;
			if(fw_info[k].Txt_Update_Mode==0x30){
		      char my_type[DEVICE_TYPE_MAX_LEN+1];
					memset(device_type, 0, sizeof(device_type));
					memcpy(device_type,fw_info[k].Txt_Dev_Type,strlen(__DEVICE_TYPE));
					memset(my_type,0,sizeof(my_type));
					memcpy(my_type,__DEVICE_TYPE,strlen(__DEVICE_TYPE));
					for(i=0;i<(strlen(__DEVICE_TYPE)-2);i++)//后两位不用匹配
					{
						if(my_type[i] != device_type[i])
						{
							dev_type_flag=1;
							break;
						}
					}
					if(dev_type_flag==1){
						 dev_type_flag=0;
					}else{
						if(sscanf(fw_info[k].Txt_Fw_Version,"%08x",&terminal_firmware)==1){
							if(terminal_firmware > CURRENT_VER){
								res=1;
								return res;
							}
						}
					}					
		  }
		if(k==0){
			break;
	  }
	 }while(1);
	}
	return res;
}

//static uint8_t is_update_firmware(void)
//{
//	//char str_buf[50];
//	char send_char[50];
//	char get_char[50];
//	uint16_t i;
//	uint8_t retry = 3;
//	uint8_t step_flag = 0;
//	uint8_t check_head_flag = 0;
//	uint16_t data_len = 0;
//	uint8_t res = 0;
//	uint32_t get_tx_size;
//	uint32_t get_file_size = 0;
//	int update_mode;
//	
//	memset(send_char,0,sizeof(send_char));
//	memcpy(send_char,__DEVICE_TYPE,strlen(__DEVICE_TYPE));
//	i = strlen(__DEVICE_TYPE);
//	send_char[i++] = '.';
//	send_char[i++] = 't';
//	send_char[i++] = 'x';
//	send_char[i++] = 't';
//	send_char[i++] = '\0';
//	
//	do{
//	
//		memset(get_char,0,sizeof(get_char));
//		char* pchar = at_cftrantx(__PATH,send_char,&get_file_size,512);
//		if(pchar == NULL)
//		{
//			break;
//		}
//		memcpy(get_char,pchar,strlen(pchar));
//		if(strcmp(get_char,"0")==0)
//		{
//			break;
//		}
//		
//		if(sscanf(get_char,"DATA,%d",&get_tx_size)!=1)
//		{	
//			break;
//		}
//		
//		if(get_tx_size > 512)
//		{
//			break;
//		}
//		
//		retry = 3;
//		do{
//			if(get_sim_rx_data_size(sim_data_rd_pos) >= get_tx_size)
//			{
//				break;
//			}
//			else
//			{
//				if(retry>0)
//				{
//					retry--;
//			    osDelay(20);  //最大发512个字节
//				}
//				else
//				{				
//					return res;
//				}
//			}
//		}while(1);
//		
//		get_sim_data_multiple_bytes(&sim_net_data[data_len], sim_data_rd_pos, get_tx_size);
//		sim_data_rd_pos += (uint16_t)get_tx_size;
//		if(sim_data_rd_pos>=SIM_UART_RX_BUF_SIZE)
//		{
//			sim_data_rd_pos -= SIM_UART_RX_BUF_SIZE;
//		}
//		get_file_size += (uint16_t)get_tx_size;
//		data_len += (uint16_t)get_tx_size;
//		

//		
//		if(step_flag==1)//找版本号
//		{
//			if(check_head_flag==0)
//			{
//				if(check_device_info(sim_net_data, &data_len, "FIRMWARE_VERSION:"))
//				{
//				  check_head_flag = 1;
//				}
//			}
//			
//			if(check_head_flag==1)
//			{
//				if(data_len>=sizeof(uint32_t))
//				{
//					char version[8];
//					memset(version,0,sizeof(version));
//					memcpy(version,sim_net_data,8); //版本号8个字节
//					if(sscanf(version,"%08x",&terminal_firmware)==1)
//					{
//						if(terminal_firmware > CURRENT_VER)
//						{
//						  data_len -= 8;
//							for(i=0;i<data_len;i++)
//							{
//								sim_net_data[i] = sim_net_data[i+8];
//							}
//						  check_head_flag = 0;
//						  step_flag = 2;
//						}
//						else
//						{
//							break;
//						}
//					}
//					else
//					{
//						break;
//					}
//				}
//			}	
//		}
//		
//		if(step_flag==2)
//		{
//			if(check_head_flag==0)
//			{
//				if(check_device_info(sim_net_data, &data_len, "UPDATE_MODE:"))
//				{
//				  check_head_flag = 1;
//				}
//			}
//			
//			if(check_head_flag==1)
//			{
//				if(data_len>=sizeof(uint8_t))
//				{
//					char mode[2] = {0};
//					mode[0] = sim_net_data[0];
//					if(sscanf(mode,"%08x",&update_mode)==1)
//					{
//						data_len--;
//						for(i=0;i<data_len;i++)
//						{
//							sim_net_data[i] = sim_net_data[i+1];
//						}
//						if(update_mode == 0) //全部升级
//						{
//							res = 1;
//						}
//						else
//						{
//							if(update_mode == 1) //部分升级，查找IMEI号
//							{
//								check_head_flag = 0;
//						    step_flag = 3;
//							}
//							else
//							{
//							  break;
//							}
//						}
//					}
//					else
//					{
//						break;
//					}
//				}
//			}			
//		}
//		
//		if(step_flag==3)//找IMEI号
//		{
//			if(check_head_flag==0)
//			{
//				char str_buf[50];
//				memset(str_buf,0,sizeof(str_buf));
//				memcpy(str_buf,imei_buf,sizeof(imei_buf));
//				if(check_device_info(sim_net_data, &data_len, str_buf))
//				{
//				  res = 1;	
//				}
//			}
//		}
//		
//		if(str_scanf("+CFTRANTX: 0", NULL, NULL, 1000)!=0)
//		{
//			break;
//		}
//		else
//		{
//			if(res ||(get_tx_size<512))
//			{
//				break;
//			}
//		}
//  }while(1);
//	
//	return res;
//}

//static uint8_t at_fsattri(char *str)
//{
//	at_cmd_t cmd;
//	uint8_t offset;
//	char send_char[50];
//	//char get_char[50] = {0};
//	memset(send_char,0,sizeof(send_char));
//	memcpy(send_char,"\r\nAT+FSATTRI=",strlen("\r\nAT+FSATTRI="));
//	offset = strlen("\r\nAT+FSATTRI=");
////	send_char[offset++] = '"';
////	for(uint8_t i=0;i<strlen(fl_name);i++)
////	{
////		send_char[offset++] = fl_name[i];
////	}
////	send_char[offset++] = '"';
//	memcpy(&send_char[offset],str,strlen(str));
//	offset += (strlen(str));
//	send_char[offset++] = '\r';
//	send_char[offset++] = '\n';
//	send_char[offset++] = '\0';
//	cmd.send_cmd = send_char;
//	cmd.ack_str = "+FSATTRI: ";
//	cmd.get_str = NULL;//get_char;
//	cmd.get_size = 0;
//	cmd.timeout = 2000;
//	cmd.try_cnt = 3;
//	if(at_cmd_process(&cmd))
//	{
//		return 1;
//	}
//	else
//	{
//		return 0;
//	}		
//}
#define HTTP_BINNAME  "firmware.bin"
static uint8_t get_firmware_http(void)
{
	char str_buf[50]={0};
	char get_char[50];
	uint32_t data_len;
	uint8_t i;

	firmware_bin_size = 0;
	memset(str_buf,0,sizeof(str_buf));
	memcpy(str_buf,HTTP_BINNAME,strlen(HTTP_BINNAME));
 i = strlen(HTTP_BINNAME);
//	i = 26;
//	str_buf[i++] = '_';
//	str_buf[i++] = (char)((terminal_firmware & 0xf0000000) >> 28) + 0x30;
//	str_buf[i++] = (char)((terminal_firmware & 0x0f000000) >> 24) + 0x30;
//	str_buf[i++] = (char)((terminal_firmware & 0x00f00000) >> 20) + 0x30;
//	str_buf[i++] = (char)((terminal_firmware & 0x000f0000) >> 16) + 0x30;
//	str_buf[i++] = (char)((terminal_firmware & 0x0000f000) >> 12) + 0x30;
//	str_buf[i++] = (char)((terminal_firmware & 0x00000f00) >> 8) + 0x30;
//	str_buf[i++] = (char)((terminal_firmware & 0x000000f0) >> 4) + 0x30;
//	str_buf[i++] = (char)(terminal_firmware & 0x0000000f) + 0x30;
//	str_buf[i++] = '.';
//	str_buf[i++] = 'b';
//	str_buf[i++] = 'i';
//	str_buf[i++] = 'n';
	str_buf[i++] = '\0';
	
	//at_fsattri(str_buf);

	do{
		memset(get_char,0,sizeof(get_char));
		char* pchar;
	if((SimcomModleVer == 4)||(SimcomModleVer == 5)||(SimcomModleVer == 6))
		{
			pchar = at_cftrantx(__PATH2,str_buf,&firmware_bin_size,512);
		}
		else
		{	
			pchar = at_cftrantx(__PATH,str_buf,&firmware_bin_size,512);
		}
		if(pchar == NULL)
		{
			return 0;
		}
		memcpy(get_char,pchar,strlen(pchar));
		
		if(strcmp(get_char,"0")==0)
		{
			return 1;
		}
		else
		{
			if(sscanf(get_char,"DATA,%d",&data_len)==1)
			{
				if(data_len>512)
				{
					return 0;
				}
				
				uint8_t retry = 3;
				do{
					if(get_sim_rx_data_size(sim_data_rd_pos) >= data_len)
					{
						break;
					}
					else
					{
						if(retry>0)
						{
							retry--;
							osDelay(20);  //最大发512个字节
						}
						else
						{	
							return 0;
						}
					}
				}while(1);
				
				get_sim_data_multiple_bytes(sim_net_data, sim_data_rd_pos, data_len);
				sim_data_rd_pos += data_len;
				if(sim_data_rd_pos>=SIM_UART_RX_BUF_SIZE)
				{
					sim_data_rd_pos -= SIM_UART_RX_BUF_SIZE;
				}
				
				if(!write_data_to_flash(sim_net_data,FIRMWARE_START_ADDR+firmware_bin_size,data_len,FIRMWARE_DATA_FLASH_AREA))
				{
					return 0;
				}
				
				firmware_bin_size += data_len;
				if(firmware_bin_size > TERMINAL_FIRMWARE_MAX_SIZE)  //128K byte
				{
					return 0;
				}
				 
				if(data_len<512)
				{
					USARTSendOut(USART1, "\r\nFirmware update success,Please reset！！！！！！！！！！！！！！\r\n", strlen("\r\nFirmware update success,Please reset！！！！！！！！！！！！！！\r\n"));
					flag_upsoftware=1;
					return 1;
				}
			}
			else
			{
				return 0;
			}
		}
		
//		if(str_scanf("+CFTRANTX: 0", NULL, NULL, 1000)!=0)
//		{
//			return 0;
//		}
		
	}while(1);
}
static uint8_t get_firmware(void)
{
	char str_buf[50]={0};
	char get_char[50];
	uint32_t data_len;
	uint8_t i;

	firmware_bin_size = 0;
	memset(str_buf,0,sizeof(str_buf));
	memcpy(str_buf,device_type,strlen(device_type));
	i = strlen(device_type);
	str_buf[i++] = '_';
	str_buf[i++] = (char)((terminal_firmware & 0xf0000000) >> 28) + 0x30;
	str_buf[i++] = (char)((terminal_firmware & 0x0f000000) >> 24) + 0x30;
	str_buf[i++] = (char)((terminal_firmware & 0x00f00000) >> 20) + 0x30;
	str_buf[i++] = (char)((terminal_firmware & 0x000f0000) >> 16) + 0x30;
	str_buf[i++] = (char)((terminal_firmware & 0x0000f000) >> 12) + 0x30;
	str_buf[i++] = (char)((terminal_firmware & 0x00000f00) >> 8) + 0x30;
	str_buf[i++] = (char)((terminal_firmware & 0x000000f0) >> 4) + 0x30;
	str_buf[i++] = (char)(terminal_firmware & 0x0000000f) + 0x30;
	str_buf[i++] = '.';
	str_buf[i++] = 'b';
	str_buf[i++] = 'i';
	str_buf[i++] = 'n';
	str_buf[i++] = '\0';
	
	//at_fsattri(str_buf);

	do{
		memset(get_char,0,sizeof(get_char));
	char* pchar;
	if((SimcomModleVer == 4)||(SimcomModleVer == 5)||(SimcomModleVer == 6))
		{
			pchar = at_cftrantx(__PATH2,str_buf,&firmware_bin_size,512);
		}
		else
		{
			pchar = at_cftrantx(__PATH,str_buf,&firmware_bin_size,512);
		}

		if(pchar == NULL)
		{
			return 0;
		}
		memcpy(get_char,pchar,strlen(pchar));
		
		if(strcmp(get_char,"0")==0)
		{
			return 1;
		}
		else
		{
			if(sscanf(get_char,"DATA,%d",&data_len)==1)
			{
				if(data_len>512)
				{
					return 0;
				}
				
				uint8_t retry = 3;
				do{
					if(get_sim_rx_data_size(sim_data_rd_pos) >= data_len)
					{
						break;
					}
					else
					{
						if(retry>0)
						{
							retry--;
							osDelay(20);  //最大发512个字节
						}
						else
						{	
							return 0;
						}
					}
				}while(1);
				
				get_sim_data_multiple_bytes(sim_net_data, sim_data_rd_pos, data_len);
				sim_data_rd_pos += data_len;
				if(sim_data_rd_pos>=SIM_UART_RX_BUF_SIZE)
				{
					sim_data_rd_pos -= SIM_UART_RX_BUF_SIZE;
				}
				
				if(!write_data_to_flash(sim_net_data,FIRMWARE_START_ADDR+firmware_bin_size,data_len,FIRMWARE_DATA_FLASH_AREA))
				{
					return 0;
				}
				
				firmware_bin_size += data_len;
				if(firmware_bin_size > TERMINAL_FIRMWARE_MAX_SIZE)  //128K byte
				{
					return 0;
				}
				 
				if(data_len<512)
				{
					USARTSendOut(USART1, "\r\nFirmware update success,Please reset！！！！！！！！！！！！！！\r\n", strlen("\r\nFirmware update success,Please reset！！！！！！！！！！！！！！\r\n"));
					flag_upsoftware=1;
					return 1;
				}
			}
			else
			{
				return 0;
			}
		}
		
//		if(str_scanf("+CFTRANTX: 0", NULL, NULL, 1000)!=0)
//		{
//			return 0;
//		}
		
	}while(1);
}

static uint8_t check_crc32(void)
{
	uint32_t get_crc;
	uint32_t calculate_crc; //crc32 4字节
	uint8_t data[4];
	calculate_crc = CalcCRC32(FIRMWARE_START_ADDR, firmware_bin_size - 4);
	read_data_from_flash(data,(FIRMWARE_START_ADDR+firmware_bin_size-4),4,FIRMWARE_DATA_FLASH_AREA);
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

static uint8_t is_download_complete(void)
{
	uint8_t i;
	char type[32];
	uint32_t firmware_ver;
	if(get_firmware_info(type,&firmware_ver) == FIRMWARE_DOWNLOAD_COMPLETE)
	{
		//有已下载的固件
		for(i=0;i<(strlen(__DEVICE_TYPE)-2);i++)
		{
			if(device_type[i]!=type[i])
			{
				break;
			}
		}
		
		if(i==(strlen(__DEVICE_TYPE)-2))
		{
			if(terminal_firmware > firmware_ver) //服务器上版本大于下载的版本
			{
				return 0;//下载固件
			}
			else
			{
				return 1;//不下载固件
			}
		}
		else  //下载的固件设备类型不对
		{
			clear_firmware_info();
			return 0; //下载固件
		}
	}
	else //无下载的固件
	{
		return 0;
	}
}


//HTTP 服务
static uint8_t at_httpinit(void)
{
	at_cmd_t cmd;
	cmd.send_cmd = "\r\nAT+httpinit\r\n";
	cmd.ack_str = "OK";
	cmd.get_str = NULL;
	cmd.get_size = 0;
	cmd.timeout = 2000;
	cmd.try_cnt = 2;
	if(at_cmd_process(&cmd))
	{
		return 1;
	}
	else
	{
		return 0;
	}	
}
static uint8_t at_httppara0(void)
{
	at_cmd_t cmd;
	cmd.send_cmd = "\r\nAT+HTTPPARA=\"VERSION\",1\r\n";
	cmd.ack_str = "OK";
	cmd.get_str = NULL;
	cmd.get_size = 0;
	cmd.timeout = 2000;
	cmd.try_cnt = 2;
	if(at_cmd_process(&cmd))
	{
		return 1;
	}
	else
	{
		return 0;
	}	
}
//#define VERSION_HTTP_ADDR  "\r\nAT+HTTPPARA=\"URL\",\"http://116.228.187.106:10088/obd-web/obd-versions/download?supplierId=2&version=6\"\r\n"
//#define VERSION_HTTP_ADDR  "\r\nAT+HTTPPARA=\"URL\",\"http://ivansi.szcloudnet.com/OBDS5S7600V07_20092707.bin\"\r\n"
//#define VERSION_HTTP_ADDR  "\r\nAT+HTTPPARA=\"URL\",\"http://116.228.187.106:10088/obd-web/obd-versions/download?supplierId=4&version=7\"\r\n"
u8 http_url[256]={0};
static uint8_t at_httppara(void)
{
	at_cmd_t cmd;
	uint8_t i=0;
	char get_char[50] = {0};
  char send_char[100] = {0};

		cmd.send_cmd = http_url;
		cmd.ack_str = "OK";
		cmd.get_str = NULL;
		cmd.timeout = 20000;
		cmd.try_cnt = 2;
	if(at_cmd_process(&cmd))
	{
		return 1;
	}
	else
	{
		return 0;
	}	
}
u8 http_filename[40];
//staticn u8 http_filename[]="OBDS5S7600V07_11111111.bin";
static uint8_t at_httpaction(void)
{
	at_cmd_t cmd;
	char get_char[50];
	uint8_t retry = 3;
	do{
		cmd.send_cmd = "\r\nAT+HTTPACTION=0\r\n";
		cmd.ack_str = "+HTTPACTION: ";
		cmd.get_str = get_char;
		cmd.get_size = sizeof(get_char);
		cmd.timeout = 20000;
		cmd.try_cnt = retry;
		if(at_cmd_process(&cmd))
		{
			int res;
			if(sscanf(get_char,"%d",&res)==1)
			{
				if(res==0)
				{
					return 1;
				}
			}
			retry--;
			if(retry==0)
			{
			  return 0;
			}
		}
		else
		{
			return 0;
		}
		
  }while(1);
}
//#define HTTP_BINNAME  "OBDS5S7600V07_20093002.bin"

static uint8_t at_httphead(void)
{
	at_cmd_t cmd;
	char get_char[50];
	uint8_t retry = 3;
	do{
		cmd.send_cmd = "\r\nAT+HTTPHEAD\r\n";
		cmd.ack_str = "filename=";
//		cmd.ack_str = "+HTTPHEAD";
		cmd.get_str = get_char;
		cmd.get_size = 26;
		cmd.timeout = 20000;
		cmd.try_cnt = retry;
		if(at_cmd_process(&cmd))
		{
	//		memcpy(http_filename,get_char,26);
			memcpy(http_filename,HTTP_BINNAME,26);
			
			return 1;
//			int res;
//			if(sscanf(get_char,"%d",&res)==1)
//			{
//				if(res==0)
//				{
//					return 1;
//				}
//			}
//			retry--;
//			if(retry==0)
//			{
//			  return 0;
//			}
		}
		else
		{
			retry--;
		}
		if(retry==0)
		{
		  return 0;
		}	
  }while(1);
}
static uint8_t at_httpread(void)
{
	at_cmd_t cmd;
	char get_char[50];
	uint8_t retry = 3;
	do{
		cmd.send_cmd = "\r\nAT+HTTPREAD=0,500\r\n";
		cmd.ack_str = "+HTTPREAD: DATA,";
		cmd.get_str = get_char;
		cmd.get_size = 50;
		cmd.timeout = 20000;
		cmd.try_cnt = retry;
		if(at_cmd_process(&cmd))
		{
//			memcpy(http_filename,get_char,26);
			return 1;
//			int res;
//			if(sscanf(get_char,"%d",&res)==1)
//			{
//				if(res==0)
//				{
//					return 1;
//				}
//			}
//			retry--;
//			if(retry==0)
//			{
//			  return 0;
//			}
		}
		else
		{
			retry--;
		}
		if(retry==0)
		{
		  return 0;
		}	
  }while(1);
}



static uint8_t at_httppreadfile(uint32_t timeout,uint8_t retry)
{
	at_cmd_t cmd;
	uint8_t i=0;
	char get_char[50] = {0};
  char send_char[100] = {0};
  memcpy(send_char,"\r\nAT+HTTPREADFILE=",strlen("\r\nAT+HTTPREADFILE="));
	uint8_t offset = strlen("\r\nAT+HTTPREADFILE=");
	send_char[offset++] = '"';
//	send_char[offset++] = '/';
//  memcpy(&send_char[offset],__CUSTOMER_NAME,strlen(__CUSTOMER_NAME));
//	offset += strlen(__CUSTOMER_NAME);
//	send_char[offset++] = '/';
//  memcpy(&send_char[offset],__DEVICE_TYPE,strlen(__DEVICE_TYPE));
//	offset += strlen(__DEVICE_TYPE);
//	send_char[offset++] = '/';
//	for(i=0;i<26;i++)
//	{
			memcpy(&send_char[offset],HTTP_BINNAME,strlen(HTTP_BINNAME));
offset += strlen(HTTP_BINNAME);
//		send_char[offset++] = http_filename[i];
//	}
	send_char[offset++] = '"';
//	send_char[offset++] = ',';
//	send_char[offset++] = '1';//'0';
	send_char[offset++] = '\r';
	send_char[offset++] = '\n';
	send_char[offset++] = '\0';
	
	do{
		iwdg_net++;
		memset(get_char,0,sizeof(get_char));
		cmd.send_cmd = send_char;
		cmd.ack_str = "+HTTPREADFILE: ";
		cmd.get_str = get_char;
		cmd.get_size = sizeof(get_char);
		cmd.timeout = timeout;
		cmd.try_cnt = retry;
		if(at_cmd_process(&cmd))
		{
			int res;
			if(sscanf(get_char,"%d",&res)==1)
			{
				if(res==0)
				{
					return 1;
				}
				if(res==9)
				{
					return 0;
				}				
			}				
			retry--;
			if(retry==0)
			{
			  return 0;
			}
		}
		else
		{
			return 0;
		}
  }while(1);	
}



//static uint8_t at_fsattri(char *str)
//{
//	at_cmd_t cmd;
//	uint8_t offset;
//	char send_char[50];
//	//char get_char[50] = {0};
//	memset(send_char,0,sizeof(send_char));
//	memcpy(send_char,"\r\nAT+FSATTRI=",strlen("\r\nAT+FSATTRI="));
//	offset = strlen("\r\nAT+FSATTRI=");
////	send_char[offset++] = '"';
////	for(uint8_t i=0;i<strlen(fl_name);i++)
////	{
////		send_char[offset++] = fl_name[i];
////	}
////	send_char[offset++] = '"';
//	memcpy(&send_char[offset],str,strlen(str));
//	offset += (strlen(str));
//	send_char[offset++] = '\r';
//	send_char[offset++] = '\n';
//	send_char[offset++] = '\0';
//	cmd.send_cmd = send_char;
//	cmd.ack_str = "+FSATTRI: ";
//	cmd.get_str = NULL;//get_char;
//	cmd.get_size = 0;
//	cmd.timeout = 2000;
//	cmd.try_cnt = 3;
//	if(at_cmd_process(&cmd))
//	{
//		return 1;
//	}
//	else
//	{
//		return 0;
//	}		
//}












uint8_t flag_http_update = 0 ;

static void update_firmware(void)
{
	uint8_t i;
	char str_buf[50];
	uint8_t ftp_start_flag = 0;
	uint8_t ftp_login_flag = 0;
	do{
//   if(!at_cgdcont())
//		{
//			break;
//		}
		iwdg_net++;
	if((SimcomModleVer == 4)||(SimcomModleVer == 5)||(SimcomModleVer == 6))
	{
		if(!at_fscd(__PATH2))
		{
			break;
		}
	}
	else
	{
		if(!at_fscd(__PATH))
		{
			break;
		}
	}
		at_fsls(); 
    if(!at_cgatt())
		{
			break;
		}
		if(!at_cgpaddr())   //激活上下文，获取IP地址  20190731 LYC ADD
		{
			break;
		}	


		//启动FTPS业务
		if(!at_cftpsstart())
		{
			break;
		}
		ftp_start_flag = 1;
				//使能FTPS相关业务
		if(!at_cftpssingleip())
		{
			break;
		}
		//连接FTPS服务器
		if(!at_cfslogin())
		{
		  break;
		}
		ftp_login_flag = 1;
		iwdg_net++;
		at_fsls();//_L里不加会出问题

		//下载文件txt
		memset(str_buf,0,sizeof(str_buf));
		memcpy(str_buf,__DEVICE_TYPE,strlen(__DEVICE_TYPE));
		i = strlen(__DEVICE_TYPE);
		str_buf[i++] = '.';
		str_buf[i++] = 't';
		str_buf[i++] = 'x';
		str_buf[i++] = 't';
		str_buf[i++] = '\0';
		if(!at_cftpgetfile(str_buf,15000,3))
		{
			at_fsdel(str_buf);  //下载失败删除残留文件
      break;
		}
		iwdg_net++;
		//at_fsattri(str_buf);
		
    //判断是否升级
		if(!is_update_firmware1())
		{
			at_fsdel(str_buf); //不升级，删除文件
			break;
		}
		at_fsdel(str_buf); //删除文件
		
		if(is_download_complete())//下载完成否
		{
			break;
		}
			
		//B11版本增加
		at_cftpslogout(); //推出FTPS连接
		
		at_cftpsstop();   //关闭FTPS业务
			
		if(!at_cgpaddr()) //激活上下文，获取IP地址  20190731 LYC ADD
		{
			break;
		}

		//启动FTPS业务
		if(!at_cftpsstart())
		{
			break;
		}
		ftp_start_flag = 1;
				//使能FTPS相关业务
		if(!at_cftpssingleip())
		{
			break;
		}
		//连接FTPS服务器
		if(!at_cfslogin())
		{
		  break;
		}
		ftp_login_flag = 1;
		//end B11版本增加
		iwdg_net++;
		at_fsls();//_L里不加会出问题
		
		//下载文件bin
		memset(str_buf,0,sizeof(str_buf));
		memcpy(str_buf,device_type,strlen(device_type));
		i = strlen(device_type);
		str_buf[i++] = '_';
		str_buf[i++] = (char)((terminal_firmware & 0xf0000000) >> 28) + 0x30;
		str_buf[i++] = (char)((terminal_firmware & 0x0f000000) >> 24) + 0x30;
		str_buf[i++] = (char)((terminal_firmware & 0x00f00000) >> 20) + 0x30;
		str_buf[i++] = (char)((terminal_firmware & 0x000f0000) >> 16) + 0x30;
		str_buf[i++] = (char)((terminal_firmware & 0x0000f000) >> 12) + 0x30;
		str_buf[i++] = (char)((terminal_firmware & 0x00000f00) >> 8) + 0x30;
		str_buf[i++] = (char)((terminal_firmware & 0x000000f0) >> 4) + 0x30;
		str_buf[i++] = (char)(terminal_firmware & 0x0000000f) + 0x30;
		str_buf[i++] = '.';
		str_buf[i++] = 'b';
		str_buf[i++] = 'i';
		str_buf[i++] = 'n';
		str_buf[i++] = '\0';
		
if((SimcomModleVer == 4)||(SimcomModleVer == 5)||(SimcomModleVer == 6))
{
//将bin文件下载到模块FLASH退换成下载至模块RAM中		
	  if(!at_cftpgetfile(str_buf,300000,3))		
		{
			at_fsdel(str_buf); //下载失败删除残留文件
      break;
		}	
		//at_fsattri(str_buf);
		if(at_cftpslogout())
		{
			ftp_login_flag = 0;
		}
		
		if(at_cftpsstop())
		{
			ftp_start_flag = 0;
		}		
		if(!firmware_flash_init())
		{
			at_fsdel(str_buf); //flash初始化失败，删除bin文件
			break;
		}
				osDelay(2000);
		if(!get_firmware())
		{
			at_fsdel(str_buf);  //获得bin文件失败，删除bin文件
			break;
		}
  
		at_fsdel(str_buf);  //获得bin文件成功，删除bin文件
		at_fsls(); 
}
else
{
		if(!at_cftpget(str_buf,22000,3,0)) //从FTP服务器下载固件bin文件到模块的RAM中，20190730 lyc modify
		{
      break;
		}
		iwdg_net++;
		if(!firmware_flash_init())
		{
			break;
		}
		osDelay(300);
		if(!get_firmware1()) //串口从模块ram获得bin文件
		{
			break;
		}	
		if(at_cftpslogout()) 
		{
			ftp_login_flag = 0;
		}
		if(at_cftpsstop())
		{
			ftp_start_flag = 0;
		}
			
}
		if(!check_crc32())
		{
			flag_upsoftware=0;
			break;
		}
    set_firmware_info(device_type, terminal_firmware, firmware_bin_size, FIRMWARE_DOWNLOAD_COMPLETE);
	}while(0);
	
	if(ftp_login_flag)
	{
	  at_cftpslogout();//推出FTPS连接
	}
	
	if(ftp_start_flag)
	{
	  at_cftpsstop();//关闭FTPS业务
	}	

if(flag_http_update == 1)
{
do{
			#ifdef debug
			USARTSendOut(USART1, http_url,sizeof(http_url));
			#endif
		osDelay(2000);
	    if(!at_httpinit())
		{
			break;
		}
		if(SimcomModleVer ==3)  //CNSEshanghai需要有这一步
		{
					if(!at_httppara0())
				{
					break;
				}				
		}
    if(!at_httppara())
		{
			break;
		}
	    if(!at_httpaction())
		{
			break;
		}
//	    if(!at_httphead())
//		{
//			break;
//		}
//	    if(!at_httpread())
//		{
//			break;
//		}
//			at_fsls();	
//		at_fsattri(http_filename);
//		if(!get_firmware())
//		{
//			at_fsdel(http_filename);  //获得bin文件失败，删除bin文件
//			break;
//		}
		
		if(!at_httppreadfile(15000,3))
		{
//			at_fsattri(http_filename);
			at_fsdel(HTTP_BINNAME);  //获得bin文件失败，删除bin文件
			break;
		}
//		at_fsattri(http_filename);
//		at_fsls();//_L里不加会出问题
//		if(!get_firmware())
		if(!get_firmware_http())
		{
			at_fsdel(HTTP_BINNAME);  //获得bin文件失败，删除bin文件
			break;
		}	
		if(!check_crc32())
		{
			flag_upsoftware=0;
			break;
		}		
//		
		set_firmware_info(device_type, terminal_firmware, firmware_bin_size, FIRMWARE_DOWNLOAD_COMPLETE);
		at_fsdel(HTTP_BINNAME); 
		
		//------------------------HTTP服务升级结束---------------------------------
//		at_fsattri(http_filename);
		break;
	}while(0);
flag_http_update = 0;
memset(http_url,0,sizeof(http_url));
memset(http_filename,0,sizeof(http_filename));

	}



}

//void sim_net_uart_data_put(uint8_t data)
//{
//	sim_net_data_buf[sim_data_wr_pos] = data;
//	sim_data_wr_pos++;
//	if(sim_data_wr_pos >= sizeof(sim_net_data_buf))
//	{
//		sim_data_wr_pos = 0;
//	}
//}

uint8_t get_iccid(uint8_t* piccid)
{
	if(iccid_status && (piccid != NULL))
	{
		for(uint8_t i = 0; i < sizeof(iccid_buf); i++)
		{
			piccid[i] = iccid_buf[i];
			
		}
		judge_is_ASCII((char *)piccid,sizeof(iccid_buf));  //20190325_lycadd
  }
	return iccid_status;
}

uint8_t get_imei(uint8_t* pimei)
{
	if(imei_status && (pimei != NULL))
	{
		for(uint8_t i = 0; i < sizeof(imei_buf); i++)
		{
//			pimei[i] = imei_buf1[i];
			pimei[i] = imei_buf[i];
		}
  }
	return imei_status;
}

char* get_sim_fw_ver(void)
{
	char* pFwVer = NULL; 
	if(fw_version_status)
	{
		pFwVer = sim_firmware_version;
	}
	return pFwVer;
}

sim_status_t get_net_status(void)
{
	return sim_net_status;
}

void reset_sim_net(void)
{
	sim_dealy_cnt = 0;
	sim_net_status = POWER_OFF;
}

void reconnect_sim_net(void)
{
	sim_net_status = NETOPEN;
}

void set_sim_power_on(void)
{
	sim_dealy_cnt = 0;
	sim_net_status = POWER_ON;
}

static uint8_t sim_power_on(void)
{
	if(sim_dealy_cnt <= 1000/SIM_NET_TASK_DEALY) //关闭电源1秒钟
	{
		set_sim_pwrkey();
	  turn_off_sim_3v8();
	}
	else
	{
		if(sim_dealy_cnt <= (1000+1000)/SIM_NET_TASK_DEALY) //打开电源1秒钟
		{
			set_sim_pwrkey();
			turn_on_sim_3v8();
		}
		else
		{
			if(sim_dealy_cnt <= (1000+1000+1700)/SIM_NET_TASK_DEALY) //pwrkey低电平0.5秒
			{
				reset_sim_pwrkey();
			}
			else
			{
				if(sim_dealy_cnt <= (1000+1000+1700+15000)/SIM_NET_TASK_DEALY) //pwrkey高电平15秒，等待串口有效
				{
				  set_sim_pwrkey();
				}
				else
				{
				  return 1;
				}
			}
		}
	}
	sim_dealy_cnt++;
	return 0;
}

static uint8_t sim_power_off(void)
{
	if(sim_dealy_cnt <= (500/SIM_NET_TASK_DEALY)) //pwrkey低电平0.5秒
	{
	  set_sim_pwrkey();
	}
	else
	{
		if(sim_dealy_cnt <= (500+6000)/SIM_NET_TASK_DEALY)//pwrkey低电平6秒，手册最少2.5秒
		{
			reset_sim_pwrkey();
		}
		else
		{
			if(sim_dealy_cnt <= (500+6000+30000)/SIM_NET_TASK_DEALY) //等待30秒,status输出低电平
			{
			  set_sim_pwrkey();
			}
			else
			{
			  return 1;
			}
		}
	}
	sim_dealy_cnt++;
	return 0;
}


extern uint8_t get_power_state(void);

extern char *MyGets(char *dest,u16 lenMax);

void Set_SimcomModleVer(void)
{
//【CM】        LE11B11 SIM7600M21
//【CNSE】      LE30B01 SIM7600M11_NA_OTA_CUS_BGZ
//【CNSE-上海】 LE30B02 SIM7600M11_NA_OTA_CUS_BGZ
//【ASR】			 A39B06A  7600C-L1V
char CM[] ="LE11B11";
char CNSE[] ="LE30B01";
char CNSE_SHANGHAI[] ="LE30B02";
char ASR[] ="A39B06A";
char ASR2[] ="A39B04A";
char ASR_CAT1[]="A43B02A";
int res;
	char SimSoftware[8];
	SimSoftware[7]=0;
	memcpy(SimSoftware,sim_firmware_version,7);
	if(!strcmp(CM ,SimSoftware))
	{
		SimcomModleVer =1;
	}
	else if(!strcmp(CNSE ,SimSoftware))
	{
		SimcomModleVer =2;
	}
	else if(!strcmp(CNSE_SHANGHAI ,SimSoftware))
	{
		SimcomModleVer =3;
	}
	else if(!strcmp(ASR ,SimSoftware))
	{
		SimcomModleVer =4;
	}
	else if(!strcmp(ASR2 ,SimSoftware))
	{
		SimcomModleVer =5;
	}
	else if(!strcmp(ASR_CAT1 ,SimSoftware))
	{
		SimcomModleVer =6;
	}
	else	
		SimcomModleVer =4;
}

void sim_net_ctr(void)
{
//	u32 ipData=GetUdsPara(_IP);
//	u32 port=GetUdsPara(_PORT);
//	u8 *pIp=(u8*)&ipData;
//	if( !( (pIp[0]==0xff)&&(pIp[1]==0xff)&&(pIp[2]==0xff)&&(pIp[3]==0xff)	)	)
//	snprintf(IP_data,50,"\r\nAT+CIPOPEN=0,\"TCP\",\"%d.%d.%d.%d\",%d\r\n",pIp[0],pIp[1],pIp[2],pIp[3],port);
	
	at_cmd_t cmd;
	
	if(!get_power_state())
	{
		if(get_login_state()!=1)
		{
			if((sim_net_status!=SIM_NET_TURN_OFF) && (sim_net_status!=POWER_ON) && (sim_net_status!=POWER_OFF))
			{
				reset_sim_net();
			}
	  }
	}
	else
	{
		if(sim_net_status==SIM_NET_TURN_OFF)
		{
			sim_net_status = POWER_ON;
		}
	}
	
	switch(sim_net_status)
	{
		case POWER_ON:
		{
			
			if(sim_power_on())
			{
			  sim_net_status = SET_ATE0;	
			}
			break;
		}

		case SET_ATE0:
		{
			cmd.send_cmd = "\r\nATE0\r\n";
			cmd.ack_str = "OK";
			cmd.get_str = NULL;
			cmd.get_size = 0;
			cmd.timeout = 3000;
			cmd.try_cnt = 5;
			if(at_cmd_process(&cmd))
			{
				sim_net_status = READ_IMEI;
			}
			else
			{
				reset_sim_net();
				break;
			}
		}
		case READ_IMEI:
		{
			if(!imei_status)
			{
				cmd.send_cmd = "\r\nATI\r\n";
				cmd.ack_str = "IMEI: ";
				cmd.get_str = imei_buf;
				cmd.get_size = sizeof(imei_buf);
				cmd.timeout = 1000;
				cmd.try_cnt = 3;
				if(at_cmd_process(&cmd))
				{
					imei_status = 1;
					sim_net_status = READ_ICCID;
				}
				else
				{
					reset_sim_net();
					break;
				}
			}
			else
			{
				sim_net_status = READ_ICCID;
				//继续读ICCID
			}
		}
		case READ_ICCID:
		{
			if(!iccid_status)
			{
				cmd.send_cmd = "\r\nAT+CICCID\r\n";
				cmd.ack_str = "+ICCID: ";
				cmd.get_str = iccid_buf;
				cmd.get_size = sizeof(iccid_buf);
				cmd.timeout = 1000;
				cmd.try_cnt = 3;
				if(at_cmd_process(&cmd))
				{
					iccid_status = 1;
					sim_dealy_cnt = 0;
					sim_net_status = IS_REG;
				}
				else
				{
					reset_sim_net();
					break;
				}	
			}
			else
			{
				sim_dealy_cnt = 0;
				sim_net_status = IS_REG;
				//继续判断是否注册
			}
		}
		case IS_REG:
		{
			char get_char[4] = {0};
			cmd.send_cmd = "\r\nAT+CGREG?\r\n";
			cmd.ack_str = "+CGREG: ";
			cmd.get_str = get_char;
			cmd.get_size = sizeof(get_char)-1;
			cmd.timeout = 1000;
			cmd.try_cnt = 10;
			
			do{
				if(!at_cmd_process(&cmd))
				{
					reset_sim_net();
					break;
				}
				int n,stat,res;
				res = sscanf(get_char,"%d,%d",&n,&stat);
				if(res!=2)
				{
					reset_sim_net();
					break;
				}
				if(stat==1||stat==5)
				{
					sim_net_status = GET_SIM_VERSION;
					break;
				}
				else
				{
				  sim_dealy_cnt++;
					if(sim_dealy_cnt>250)  //100*250=50000 延时25秒等待注册
					{
						reset_sim_net();
					  break;
					}
				}
			}while(0);
			break;
		}
		
		case GET_SIM_VERSION:
		{
			fw_version_status = 0;
			memset(sim_firmware_version,0,sizeof(sim_firmware_version));
			cmd.send_cmd = "\r\nAT+GMR\r\n";
			cmd.ack_str = "+GMR:";
			cmd.get_str = sim_firmware_version;
			cmd.get_size = sizeof(sim_firmware_version)-1;
			cmd.timeout = 1000;
			cmd.try_cnt = 3;
			if(at_cmd_process(&cmd))
			{
			  sim_net_status = UPDATE_FIRMWARE;
				if(sim_firmware_version[0]==0x20)   //删除软件版本中的空格
				{
					for(u8 i=0;i<sizeof(sim_firmware_version);i++)
					{
						sim_firmware_version[i]=sim_firmware_version[i+1];
						if(sim_firmware_version[i]==0x00)
							break;
					}				
				}
				Set_SimcomModleVer();
				fw_version_status = 1;
			}
			else
			{
				reset_sim_net();
				break;
			}	
		}
		
		case UPDATE_FIRMWARE:
		{
			if(getTermianlState()==0)
			{
				if(!open_version_server_flag)
				{
					update_firmware();
					open_version_server_flag = 1;
				}
			}
			
			if(flag_upsoftware)
				MCU_RESET();
			sim_net_status = SET_CIPMODE;
		}
		
		case SET_CIPMODE:
		{
			cmd.send_cmd = "\r\nAT+CIPMODE=0\r\n";
			cmd.ack_str = "OK";
			cmd.get_str = NULL;
			cmd.get_size = 0;
			cmd.timeout = 1000;
			cmd.try_cnt = 3;
			if(at_cmd_process(&cmd))
			{
			  sim_net_status = NETOPEN;
			}
			else
			{
				reset_sim_net();
				break;
			}	
		}
		
		case NETOPEN:
		{
			char get_char[3] = {0};
			cmd.send_cmd = "\r\nAT+NETOPEN\r\n";
			cmd.ack_str = "+NETOPEN: ";
			cmd.get_str = get_char;
			cmd.get_size = sizeof(get_char)-1;
			cmd.timeout = 10000;
			cmd.try_cnt = 3;
			if(at_cmd_process(&cmd))
			{
				int res,err;
				res = sscanf(get_char,"%d",&err);
				if(res==1 && err==0)
				{
					//sim_net_status = SEND_FIRMWARE_VERSION;
					sim_net_status = CIPOPEN;
					break;
				}
				else
				{
				  reset_sim_net();
				  break;
				}
			}
			else
			{
				reset_sim_net();
				break;
			}	
		}
//		case SEND_FIRMWARE_VERSION:
//		{
//			if(!open_version_server_flag)
//			{
//			  send_firmware_version();
//				if(open_version_server_flag)
//				{
//					reset_sim_net();
//					break;
//				}
//				else
//				{
//			   sim_net_status = CIPOPEN;
//				}
//			}
//			else
//			{
//				sim_net_status = CIPOPEN;
//			}
//		}
		case CIPOPEN:
		{
//			if(!getTermianlState())
//			{
//				cmd.send_cmd = IP_data;
//			}
//			else
//			{
//			  cmd.send_cmd = test_IP_data;
//			}
//			cmd.ack_str = "CONNECT 115200";
//			cmd.get_str = NULL;
//			cmd.get_size = 0;
//			cmd.timeout = 10000;
//			cmd.try_cnt = 3;
//			if(at_cmd_process(&cmd))
//			{
//				open_version_server_flag = 0;
//        sim_net_status = ON_LINE;				
//			}
			 SetCipget();
			 int res;
//			int res=OpenSocket(0);//shanghai
			 state_net0=OpenSocket(0);//艾可蓝
			 state_net1=OpenSocket(1);

			if((state_net0==0)||(state_net1==0))
			{
				open_version_server_flag = 0;
        sim_net_status = ON_LINE;	
			}
			else
			{
				reset_sim_net();
				break;
			}
		}
		case ON_LINE:
		{
//			if((str_scanf("CLOSED", NULL, 0, 0)==0)) //|| ((!get_power_state()) && (!get_login_state())))
//			{
//				reset_sim_net();
//			}
//			int sk0=SocketRead(0,512);
//			int sk1=SocketRead(1,512);	
			
	//		int sk0=SocketRead(0,256);
//				if(getTermianlState())
//***************************************************
				int sk0;
//			if(state_net1==0)
//				sk0=SocketRead(1,256); 
//			else	
//				state_net1=OpenSocket(1);
//****************************************************
			  sk0=SocketRead(0,256);//艾可蓝
				sk0=SocketRead(1,256);//生产用
			
			
			
//	int				sk0=SocketRead(1,256);
			iwdg_net++;
//			if(sk0>0)
//			{
//				get_sim_data_multiple_bytes(GB17691_CMD_data, sim_data_rd_pos, sk0);
//				GB17691CheckCmd();
//				sim_data_rd_pos += sk0;
//				if(sim_data_rd_pos>=SIM_UART_RX_BUF_SIZE)
//				{
//					sim_data_rd_pos -= SIM_UART_RX_BUF_SIZE;
//				}
//				sk0=0;
//			}
			
			break;
		}
		case NETCLOSE:
		{
			char get_char[3]={0};
			
			cmd.send_cmd = "\r\nAT+NETCLOSE\r\n";
			cmd.ack_str = "+NETCLOSE: ";
			cmd.get_str = get_char;
			cmd.get_size = sizeof(get_char)-1;
			cmd.timeout = 5000;
			cmd.try_cnt = 3;
			if(at_cmd_process(&cmd))
			{
				int res,err;
				res = sscanf(get_char,"%d",&err);
				if(res==1 && err==0)
				{
					sim_net_status = IDLE;
				}
				break;
			}
			else
			{
				break;
			}
		}
		case POWER_OFF:
		{
			if(sim_power_off())
			{
				sim_dealy_cnt = 0;
				if(get_power_state()==1)
				{
			    sim_net_status = POWER_ON;
				}
				else
				{
					sim_net_status = SIM_NET_TURN_OFF;
				}
			}
			break;
		}
		case SIM_NET_TURN_OFF:
		{
			turn_off_sim_3v8();
			break;
		}
		default:break;
	}
	
}


//void sim_net_task(void *pvParameters)
//{    
//	sim_pwrkey_init();
//	sim_uart_init(115200);     	//初始化串口
//	while(1)
//	{
//    sim_net_ctr();
//		osDelay(200);
//	}
//}

void sim_net_task(void const *argument)
{    
	sim_gpio_init();
	sim_uart_init(115200);     	//初始化串口
	while(1)
	{
		iwdg_net++;
		sim_net_ctr();
		osDelay(SIM_NET_TASK_DEALY);
	}
}
osThreadId tid_Thread;
osThreadDef (sim_net_task, osPriorityNormal, 1, 1000);
void sim_net_task_init(void)
{
	tid_Thread = osThreadCreate (osThread(sim_net_task), NULL);
}



