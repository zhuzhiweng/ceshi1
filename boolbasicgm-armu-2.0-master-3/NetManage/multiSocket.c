#include "sim_net.h"
#include "MultiSocket.h"
#include "TerminalTest.h"
#include "user_config.h"
//#include "user_config.h"
int socketNum=-1;
extern char IP_data[];
//-------------------------------双联路服务器-------------------------------------------------
//char IP_Bool[] = "\r\nAT+CIPOPEN=1,\"TCP\",\"111.30.107.158\",60023\r\n";  // 布尔国六平台;

//char IP_Bool[] = "\r\nAT+CIPOPEN=1,\"TCP\",\"111.30.107.158\",9400\r\n";  // 布尔VMS生产平台;
char IP_Bool[] = "\r\nAT+CIPOPEN=1,\"TCP\",\"124.70.70.30\",9400\r\n";  // 布尔VMS生产平台;
//static char test_IP_data[50] = TEST_SERVER_IP_PORT;
//char IP_Bool[] = "\r\nAT+CIPOPEN=1,\"TCP\",\"39.104.144.129\",20000\r\n" ;  // 亚美平台;

u8 simRxBuf[BUF_LEN];

UartBuf_t uartRxBuf=
					{
						.pBuf=simRxBuf,
						.read=0,
					  .write=0,
					};

//cmd.send_cmd = "\r\nATE0\r\n";
//			cmd.ack_str = "OK";
//			cmd.get_str = NULL;
//			cmd.get_size = 0;
//			cmd.timeout = 1000;
//			cmd.try_cnt = 3;
//			if(at_cmd_process(&cmd))
//			{
//				sim_net_status = READ_IMEI;
//			}
//			else
//			{
//				reset_sim_net();
//				break;
//			}


//{
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
int  GetCurrentSocketNum(void)
{
	return socketNum;
}
int SetCurrentSocketNum(int value)	
{
	socketNum=value;
	return 0;
}
//return  0  success
int OpenSocket(int linkNum)
{
  int re=-1;
//	if(socketNum==linkNum)
//	{
//		return 0;
//	}
	char tempBuf[10]={0};
	at_cmd_t cmd;
	int linkState=-1;
  switch(linkNum)
  {
  case 0:
				cmd.send_cmd=IP_data;
				cmd.ack_str = "+CIPOPEN: ";
				cmd.get_str = tempBuf;
				cmd.get_size = 9;
				cmd.timeout = 4000;
				cmd.try_cnt = 3;
				if(at_cmd_process(&cmd))
				{
					sscanf(tempBuf,"0,%d",&linkState);
					if(linkState==0)
					{
						re=0;
					}
					else
					{
						re=-1;
					}
				}
				else re=1;
//        if(At_GetRes(&atCmdTable[CIPOPEN],&simDev1)==AT_OK)
//                  {
//                   re=0;
//                  }
//        else re=1;
    break;
  case 1:
		
//				if(!getTermianlState())
//			{
				cmd.send_cmd = IP_Bool;
//			}
//			else
//			{
//			  cmd.send_cmd = test_IP_data;
//			}
	
				cmd.ack_str = "+CIPOPEN: ";
				cmd.get_str = tempBuf;
				cmd.get_size = 9;
				cmd.timeout = 4000;
				cmd.try_cnt = 3;
				if(at_cmd_process(&cmd))
				{
					sscanf(tempBuf,"1,%d",&linkState);
					if(linkState==0)
					{
						re=0;
					}
					else
					{
						re=-1;
					}
				}
				else re=1;
    break;
  default :
    break;
  }
  return re;
}


// { .cmd="+++", .ack="OK", .timeOut=2000, .retry=10,},//PLUS3
//return  0  success
int CloseSocket(void)
{
  int re=-1;
	at_cmd_t cmd;
	cmd.send_cmd="+++";
	cmd.ack_str = "OK";
	cmd.get_str = NULL;
	cmd.get_size = 0;
	cmd.timeout = 5000;
	cmd.try_cnt = 3;
	if(at_cmd_process(&cmd))
	{
		re=1;
	}
	else
	{
//		reset_sim_net();
//		reconnect_sim_net();
	}
//    { .cmd="AT+CIPCLOSE=0\r\n", .ack="+CIPCLOSE: 0", .timeOut=8000, .retry=2,},//
	
	cmd.send_cmd="AT+CIPCLOSE=0\r\n";
	cmd.ack_str = "+CIPCLOSE: 0";
	cmd.get_str = NULL;
	cmd.get_size = 0;
	cmd.timeout = 8000;
	cmd.try_cnt = 2;
	if(at_cmd_process(&cmd))
	{
		re=0;
	}
	else
	{
//		reset_sim_net();
		reconnect_sim_net();
	}
  return re;
}

int SocketWrite(u8 linkNum, u16 dataLen)
{
	at_cmd_t cmd;
	char tempBuf[30]={0};
	snprintf(tempBuf,30,"AT+CIPSEND=%d,%d\r\n",linkNum,dataLen);
	cmd.send_cmd=tempBuf;
	cmd.ack_str = ">";
	cmd.get_str = NULL;
	cmd.get_size = 0;
	cmd.timeout = 2000;
	cmd.try_cnt = 3;
	if(at_cmd_process(&cmd))
	{
		return 0;
	}
	else
		{
	return -1;
		}

}

int SetCipget(void)
{
	at_cmd_t cmd;
	cmd.send_cmd="AT+CIPRXGET=1\r\n";
	cmd.ack_str = "OK";
	cmd.get_str = NULL;
	cmd.get_size = 0;
	cmd.timeout = 2000;
	cmd.try_cnt = 3;
	if(at_cmd_process(&cmd))
	{
		return 0;
	}
	else
	{
		return -1;
	}
}

int SocketRead(u8 linkNum,u16 dataLen)
{
	at_cmd_t cmd;
//	char *s="AT+CIPRXGET=1\r\n";
//	sim_uart_tx(s,strlen(s));
//	osDelay(100);
//	char tempchar[50]={0};
//	MyGets(tempchar,50);
	
	
	
	
	char tempBuf[30]={0};
	char tempRxBuf[30]={0};
	snprintf(tempBuf,30,"AT+CIPRXGET=2,%d,%d\r\n",linkNum,dataLen);
	cmd.send_cmd=tempBuf;
	cmd.ack_str = "+CIPRXGET: ";
	cmd.get_str = tempRxBuf;
	cmd.get_size = 29;
	cmd.timeout = 2000;
	cmd.try_cnt = 1;
	
	if(at_cmd_process(&cmd))
	{
		int res,cid,readLen,restLen;
		res=sscanf(tempRxBuf,"2,%d,%d,%d",&cid,&readLen,&restLen);
		if(res!=3)
		{
			return 0;
		}
		else
		{
			return readLen;
		}
//		return 0;
	}
	else
		{
			return -1;
		}

}

int MyGetchar()//需要改进  套圈问题					
{
	if(uartRxBuf.read==uartRxBuf.write)//KONG
		return -1;
	else
	return uartRxBuf.pBuf[(uartRxBuf.read++)%BUF_LEN];
}
char *MyGets(char *dest,u16 lenMax)//处理\r\n问题去掉\r
{
	int temp=0;
	char c=temp=MyGetchar();
	if(temp==-1)
	{
		return NULL;
	}
	char *p=dest;		
	while(c!='\n'&&lenMax--)
	{
		*p++=c;		
		c=MyGetchar();		
	}
//	*p=0;//处理\r\n问题去掉\r
	return dest;
}
char *MyGetsBylength(char *dest,u16 len)
{
	char *p=dest;
	while(len--)
	{
		*p++=MyGetchar();
	}
	return dest;
}


//int MyGetchar(UartBuf_t *puartRxBuf)
//{
//	if(puartRxBuf->read>puartRxBuf->write)
//		return -1;
//	else
//	return puartRxBuf->pBuf[puartRxBuf->read++];
//}
