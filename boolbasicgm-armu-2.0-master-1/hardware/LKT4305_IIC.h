/******************************************************************************
  * @file /i2c.h 
  * @author  
  * @version  
  * @date  
  * @brief  Header for i2c.c 
  *****************************************************************************/ 

/* Define to prevent recursive inclusion ------------------------------------ */
#ifndef __LKT4305_IIC_H
#define __LKT4305_IIC_H
#include <sys.h>


#define LKT4305_WRITE_ADDR  0x50
#define LKT4305_READ_ADDR  0x51

#define Successful        0

#define  IICNACK           0xE2
#define  IICRadAddrNACK    0xE3
#define  IICSWError        0xE4
#define  IICRadLenError    0xE5
#define  SCL_TimeOut       0xE6
 
#define  IIC_SDA_H            (PBout(7)=1) //GPIOB->BSRR = GPIO_Pin_7
#define  IIC_SDA_L            (PBout(7)=0) //GPIOB->BRR = GPIO_Pin_7

#define  IIC_SCL_H            (PBout(6)=1) //GPIOB->BSRR = GPIO_Pin_6
#define  IIC_SCL_L            (PBout(6)=0) //GPIOB->BRR = GPIO_Pin_6

#define  IIC_REST_H           (PBout(8)=1) //GPIOB->BSRR = GPIO_Pin_8
#define  IIC_REST_L           (PBout(8)=0) //GPIOB->BRR = GPIO_Pin_8

#define SDA_IN()  {GPIOB->PL_CFG&=0X0FFFFFFF;GPIOB->PL_CFG|=(u32)8<<28;}
#define SDA_OUT() {GPIOB->PL_CFG&=0X0FFFFFFF;GPIOB->PL_CFG|=(u32)3<<28;}

#define READ_SDA   PBin(7)  //输入SDA 

#define  SdaState             GPIOB->IDR & (uint16_t)0x0080
#define  SclState             GPIOB->IDR & (uint16_t)0x0040

  

//extern void IIC_IO_INIT(void);
//extern u8 IIC_WriteData(u8  *Sendbuf , u16 len);
//extern u8 IIC_ReadData(u8  *RecBuf, u16* len);
//extern void IIC_Reset(void);
//extern void start(void);
//extern void stop(void);
//extern void checkack(void);
//extern u8 IIC_SendApdu(u16 Sendlen,u8* Sendbuf,u8* RecBuf,u16* Relen);

uint8_t LKT4305SendData(uint8_t* pData, uint16_t len);
uint16_t LKT4305RecData(uint8_t* pData, uint16_t maxLen);
void LKT4305Init(void);

u8 LKT4305Test(void);
//u16 Signature(u8* pInData,u8 inLen,u8 *pOutData);//改

u8 Set4305_Sm2id(void);		// 设置SM2ID
u8* Signature(u8* pInData,u16 inLen);//改
u8 *ReadPublickey(void);
u8 *Encrypt(u8* pInData,u16 inLen);//公钥加密 结果增加96字节
int CreatePlatformPublicKeyFile(void);
int WritePlatformPublicKey2File(u8 *pkey);
#endif /* __IIC_H */

/************************END OF FILE*******************************************/


