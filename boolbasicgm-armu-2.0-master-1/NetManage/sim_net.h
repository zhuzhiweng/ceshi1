#ifndef __SIM_NET_H
#define __SIM_NET_H			    
#include "sys.h" 
#include "stdio.h"
#include <string.h>
#include "sim_uart.h"


#define IMEI_LEN  15
#define ICCID_LEN 20

#define FIRMWARE_DOWNLOAD_COMPLETE 0xA5A5A5A5
#define FIRMWARE_VERSION_SEND_FLAG 0x5A5A5A5A

//任务优先级
#define SIM_NET_TASK_PRIO		4
//任务堆栈大小	
#define SIM_NET_STK_SIZE 		512

#define TERMINAL_FIRMWARE_MAX_SIZE  0x20000  //128k

#define SIM_NET_TASK_DEALY 100
typedef enum 
{
	POWER_ON,
	POWER_OFF,
	SET_ATE0,
	AT_CGNSPWER,
	AT_CGNSIPR,
	READ_IMEI,
	READ_ICCID,
	IS_REG,
	GET_SIM_VERSION,
	UPDATE_FIRMWARE,
	SET_CIPMODE,
	NETOPEN,
	SEND_FIRMWARE_VERSION,
	CIPOPEN,
	NETCLOSE,
	ON_LINE,
	SIM_NET_TURN_OFF,
	IDLE
}sim_status_t;

typedef struct
{
	char* send_cmd;
	char* ack_str;
	char* get_str;
	uint16_t get_size;
	uint32_t timeout;
	uint8_t try_cnt;
}at_cmd_t;

uint8_t get_iccid(uint8_t* piccid);
uint8_t get_imei(uint8_t* pimei);
char* get_sim_fw_ver(void);
void sim_net_uart_data_put(uint8_t data);
void reconnect_sim_net(void);
void reset_sim_net(void);
sim_status_t get_net_status(void);
void set_sim_power_on(void);
//任务函数
void sim_net_task(void const *argument);
void sim_net_task_init(void);
uint8_t at_cmd_process(at_cmd_t* at_cmd);



//return  0  success
extern int SetCurrentSocketNum(int value);
extern int  GetCurrentSocketNum(void);
//return  0  success
extern int OpenSocket(int linkNum);
extern int CloseSocket(void);

extern int SocketWrite(u8 linkNum, u16 dataLen);
extern int SocketRead(u8 linkNum,u16 dataLen);
#endif

