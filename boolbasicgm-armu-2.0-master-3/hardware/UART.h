


#include "n32g45x.h"

#define BIN_FILE_DATA_CMD     0x01    //BIN文件数据命令

void USART_Sendbyte(USART_Module* USARTx, u8 Data);
//void USARTSendOut(USART_TypeDef* USARTx,const char *buf, u8 len);
void USARTSendOut(USART_Module* USARTx, const char *buf, u16 len);

void COM_CMD_check(void);

uint8_t get_comupdate_mode(void);
int uprintf(char *format, ...);