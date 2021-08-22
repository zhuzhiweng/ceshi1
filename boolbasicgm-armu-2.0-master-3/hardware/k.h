#ifndef __K_H
#define __K_H	 
#include "sys.h"

typedef struct __FILE FILE;

#define K_LINE              1
#define L_LINE					    2

typedef struct{
			uint8_t len;   
			uint8_t data[15]; 
} K_RxMsg;

typedef struct{
	K_RxMsg k_RxMsg[110];
	u8 currentPointer;//发送到的位置
	u8 revPointer; //接收报文的位置
	u8 count;//报文个数
}K_RxBuff;

typedef enum {OFF = 0, ON} SwitchType;

extern __IO SwitchType Actflay;

extern __IO u8 framelen;
extern __IO u8 tempframelen;
extern __IO u8 LEN;
extern __IO u8 RevFlag;
extern ErrorStatus err;
extern u8 K_bleTxData_len;
extern K_RxMsg k_RxMsg;
extern K_RxBuff k_RxBuff;


extern void usart3_init(u32 Baud);
extern void KL_LINE_Enable(u8 KL);
extern void HL_25MS(void);
extern u8* Send_KWP2000Frame(u8* TxMessage,ErrorStatus* err);
extern u8* Send_ISO9141Frame(u8* TxMessage,ErrorStatus* err);
extern u8 Wakeup_Addr(u8 addr);

#endif

















