#include "sys.h"
#include <string.h> 
#include "k.h"
#include "BSP.h"
#include "delay.h"



//////////////////////////////记录匹配情况//////////////////////////////////////

#define K_LINE               1
#define L_LINE					     2
															
u8 K_bleTxData_len = 0;
K_RxMsg k_RxMsg;
K_RxBuff k_RxBuff;


u8 initialmatch=0;
u32 FreezeFrameDTCFlg=0;	
u32 ConsecutiveFrameFlg = 0;
///////		K_BUS通讯		//////////////
void usart3_init(u32 Baud)																
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	 NVIC_InitTypeDef NVIC_InitStructure;
	
    RCC_APB2PeriphClockCmd(RCC_APBxPeriph_KL327_USART_IO | RCC_APB2Periph_AFIO,ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APBxPeriph_KL327_USART,ENABLE);
	
	/* USART3 GPIO config */
    GPIO_InitStructure.GPIO_Pin = KL327_USART_TXD;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(KL327_USART_IO,&GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = KL327_USART_RXD;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(KL327_USART_IO,&GPIO_InitStructure);
	

  /* USART3 mode config */
	USART_InitStructure.USART_BaudRate   = Baud;              //波特率10400
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;   //字长8位
	USART_InitStructure.USART_StopBits   = USART_StopBits_1;      //1位停止字节
	USART_InitStructure.USART_Parity     = USART_Parity_No;       //无奇偶校验
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//打开Rx接收和Tx发送功能
	USART_Init(USART3, &USART_InitStructure);  

	
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);									//允许接收中断
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
	USART_Cmd(USART3, ENABLE);	//初始化
	
}

/************************************************************************
  * @描述:  K线中断
  * @参数:  None
  * @返回值: None
  **********************************************************************/
__IO u8	LEN;
__IO u8 ACCBYTE = 0;
__IO u8 RxRAM[100];
__IO u8 framelen = 0;//请求时进行了初始化
__IO u8 tempframelen = 0;
__IO u8 RevFlag=0;


//void USART3_IRQHandler(void)
//{
//  u8 i;
//  ACCBYTE = 0;
//	if (USART_GetFlagStatus(USART3,USART_IT_RXNE)!= RESET){	
//					if(RevFlag == 0){
//								RxRAM[LEN++] = USART3->DR;
//								for (i = framelen; i < LEN-1; i++){
//									 ACCBYTE += RxRAM[i];//这个内部不能加打印
//								}	
//								if ((RxRAM[0] == 0x55 && LEN >= 3) || (Actflay == ON && LEN >= 3 )) //地址激活 接收 和KW判断
//								{
//									initialmatch = 1;
//								}else if (ACCBYTE == RxRAM[LEN-1] && LEN > framelen +3 ){  //累加和效验一帧数据接收完毕 (比前6步程序进行了优化)
//									initialmatch = 1;
//								}
//					}else{
//								if(framelen != 0){
//										tempframelen	=	framelen;
//											RxRAM[LEN++] = USART3->DR;
//											for (i = framelen; i < LEN-1; i++){
//													ACCBYTE += RxRAM[i];//这个内部不能加打印
//											}
//											if(ACCBYTE == RxRAM[LEN-1] && LEN > framelen +6){  //累加和效验一帧数据接收完毕 (比前6步程序进行了优化)
//														if(RxRAM[framelen+3] ==0x42 &&	RxRAM[framelen+4] ==0x02 &&	
//															(RxRAM[framelen+5] ==0x00 ||RxRAM[framelen+5] ==0x01 ||RxRAM[framelen+5] ==0x02||RxRAM[framelen+5] ==0x03)){
//																if(RxRAM[framelen+6] == 0x00 && RxRAM[framelen+7] == 0x00){//无冻结帧
//																		FreezeFrameDTCFlg = 0;
//																}else{
//																		FreezeFrameDTCFlg = 1;
//																}
//														}
//																memset(k_RxBuff.k_RxMsg[k_RxBuff.revPointer].data,0,15);
//																memcpy(k_RxBuff.k_RxMsg[k_RxBuff.revPointer].data,(u8 *)RxRAM+framelen+3,LEN-framelen-4);
//																k_RxBuff.k_RxMsg[k_RxBuff.revPointer].len = LEN-framelen-4;

//																k_RxBuff.revPointer++;
//																k_RxBuff.count++;
//																initialmatch = 1;
//																LEN = 0;
//																memset((u8 *)RxRAM,0,100);
//																ConsecutiveFrameFlg ++;
//																framelen = 0;
//										}			
//							}else{
//											RxRAM[LEN++] = USART3->DR;
//											for (i = 0; i < LEN-1; i++){
//													ACCBYTE += RxRAM[i];//这个内部不能加打印
//											}
//											if(ACCBYTE == RxRAM[LEN-1] && LEN >tempframelen + 3){  //此处只接受"一次请求多报文回复的情况, 此处LEN>tempframelen +3 没有根据
//															memset(k_RxBuff.k_RxMsg[k_RxBuff.revPointer].data,0,15);														
//															memcpy(k_RxBuff.k_RxMsg[k_RxBuff.revPointer].data,(u8 *)RxRAM+3,LEN-4);	
//															k_RxBuff.k_RxMsg[k_RxBuff.revPointer].len = LEN-4;																																
//	
//															k_RxBuff.revPointer++;
//															k_RxBuff.count++;
//															ConsecutiveFrameFlg ++;				
//															memset((u8 *)RxRAM,0,100);
//														LEN = 0;
//											}
//								}
//					}
//		}		
//}


void KL_LINE_Enable(u8 KL)
{
    GPIO_InitTypeDef GPIO_InitStructure;
		RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB,&GPIO_InitStructure);	
		GPIO_SetBits(GPIOB, GPIO_Pin_10);
	
	
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA,&GPIO_InitStructure);		
		GPIO_SetBits(GPIOA, GPIO_Pin_1);
	
	
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB,&GPIO_InitStructure);	
		GPIO_SetBits(GPIOB, GPIO_Pin_8);
	
	
    delay_ms(100);
		
		if (KL == 1)
			{
					GPIO_WriteBit(GPIOA, GPIO_Pin_1, Bit_RESET);
			}
			else if (KL == 2)
			{
					GPIO_WriteBit(GPIOB, GPIO_Pin_8, Bit_RESET);
			}
			else if (KL == 3)
			{
					GPIO_WriteBit(GPIOA, GPIO_Pin_1, Bit_RESET);
					GPIO_WriteBit(GPIOB, GPIO_Pin_8, Bit_RESET);
			}
}



void HL_25MS(void)
{
  KL_LINE_Enable(K_LINE);
	GPIO_WriteBit(GPIOB, GPIO_Pin_10, Bit_SET);
  delay_s(3);
	GPIO_WriteBit(GPIOB, GPIO_Pin_10, Bit_RESET);
  delay_ms(25); 
	GPIO_WriteBit(GPIOB, GPIO_Pin_10, Bit_SET);
  delay_ms(25); 
  usart3_init(10400);
}


/************************************************************************
  * @描述:  发送一帧K线数据 (KWP2000协议)
  * @参数:  u8* TxMessage,ErrorStatus* err
  * @返回值: u8*
  **********************************************************************/
//__IO u8 framelen = 0;  上移到void USART3_IRQHandler(void)上面了
u8* Send_KWP2000Frame(u8* TxMessage,ErrorStatus* err)
{
    u32 i;
    u8 data = 0;
    LEN = 0;
    initialmatch = 0;
    if ( TxMessage[0] == 0x80 || TxMessage[0] == 0xc0)
    {
        framelen = TxMessage[3] + 5;
    }
    else if ((TxMessage[0] & 0xf0) == 0x80)
    {
        framelen = TxMessage[0] - 0x7c;
    }
    else if ((TxMessage[0] & 0xf0) == 0xc0)
    {
        framelen = TxMessage[0] - 0xbc;
    }

    for (i = 0; i < framelen-1; i++)				//生成累加和效验字节
    {
        data += TxMessage[i];
    }
    for (i = 0; i < framelen - 1; i++)
    {
        USART_SendData( USART3, TxMessage[i]);
        delay_ms(6);
    }
		USART_SendData(USART3, data);

    for (i = 0; i < 10000000/2; i++)	//大概等待1.5秒//需要后期调试主频不一样计数速度不一样
    {
       if (initialmatch == 1) break;
    }
      *err =(ErrorStatus) initialmatch;
    return (u8*)(RxRAM+framelen);					//目前返回值不对 前面有个framelen = 0
}

u8* Send_ISO9141Frame(u8* TxMessage,ErrorStatus* err)
{
    u32 i;
    u8 data = 0;
    LEN = 0;
 //  RxFlay = ERROR;
		initialmatch = 0;
    framelen = TxMessage[0];
    for (i = 0; i < framelen-1; i++)				//生成累加和效验字节
    {
        data +=	TxMessage[i+1];
    }
    for (i = 0; i < framelen - 1; i++)
    {
        USART_SendData(USART3, TxMessage[i+1]);
        delay_ms(6);
    }
    USART_SendData(USART3, data);
    for (i = 0; i < 10000000/2; i++)	//大概等待1.5秒
    {
       if (initialmatch == 1) break;
    }
    *err =(ErrorStatus) initialmatch;
    return (u8*)(RxRAM+framelen);
}

__IO SwitchType Actflay = OFF;
u8 Wakeup_Addr(u8 addr)
{  
		u32 i;
		RevFlag = 0;
    KL_LINE_Enable(K_LINE | L_LINE);
    GPIO_WriteBit(GPIOB,GPIO_Pin_10, Bit_SET);
    delay_s(2);
    GPIO_WriteBit(GPIOB, GPIO_Pin_10, Bit_RESET);
    delay_ms(200);
    for (i = 0; i < 8; i++)
    {
        if (((addr >>  i)&0x01) != 0)
        {
            GPIO_WriteBit(GPIOB, GPIO_Pin_10, Bit_SET);
        }
        else
        {
            GPIO_WriteBit(GPIOB, GPIO_Pin_10, Bit_RESET);
        }
        delay_ms(200);
    }
    GPIO_WriteBit(GPIOB, GPIO_Pin_10, Bit_SET);
		delay_ms(100);
 //   KL_LINE_Enable(K_LINE);
		usart3_init(10400);
		LEN = 0;
		delay_ms(100);
    initialmatch = 0;
    for (i = 0; i < 10000000/3; i++)	//大概等待1.5秒
    {
        if (initialmatch == 1) break;
    }
    if (initialmatch == 1)
    {
        delay_ms(35);
        initialmatch = 0;
			  Actflay = ON;
        USART_SendData(USART3,~RxRAM[2]);
				delay_ms(50);	
        for (i = 0; i < 10000000; i++)	//大概等待1.5秒
        {
            if (initialmatch == SUCCESS) break;
        }
				delay_ms(100);	
        Actflay = OFF;
    }
    return (u8 )initialmatch;
}




