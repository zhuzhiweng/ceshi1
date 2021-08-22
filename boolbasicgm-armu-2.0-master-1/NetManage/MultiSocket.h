#ifndef __MULTISOCKET_H
#define __MULTISOCKET_H	

//==============test=======================

#define BUF_LEN  10
extern u8 simRxBuf[BUF_LEN];
typedef struct
{
	u8 *pBuf;
	u16 read;
	u16 write;
//	char* (*getChar)(char*);
}UartBuf_t;

extern UartBuf_t uartRxBuf;

//=============end========
int SocketWrite(u8 linkNum, u16 dataLen);
int SetCipget(void);
int SocketRead(u8 linkNum,u16 dataLen);
char *MyGets(char *dest,u16 lenMax);//处理\r\n问题去掉\r					
char *MyGetsBylength(char *dest,u16 len);

#endif

