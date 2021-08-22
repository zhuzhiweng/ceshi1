//#include "stm32f10x_lib.h"
//#include "delay.h"
#include "LKT4305_IIC.h"
//unsigned char IICWriteAddr=0x50;
//unsigned char IICReadAddr=0x51;
//unsigned char nackFlag;		      //非应答标志
#include "cmsis_os.h" 
#include "string.h"
#include "spi_flash.h"
uint8_t iicReadData[512] = {0};

static const uint8_t createKeyflie[]={0x80,0xE0,0x00,0x00,0x07,0x3F,0x01,0x00,0xFF,0xF0,0xFF,0xFF};
static const uint8_t createMainflie[]={0x80,0xE0,0x3f,0x00,0x0d,0x38,0xff,0xff,0xF0,0xF0,0xFf,0xFF,0xFf,0xFF,0xFf,0xFF,0xFf,0xFF};
static const uint8_t earsefile[]={0x80,0x0E,0x00,0x00,0x00};
//static const uint8_t selectfile[]={0x00,0xa4,0x00,0x02,0xef,0x40};
static const uint8_t createPrivateKey[]={0x80,0xE0,0xEF,0x01,0x07,0x3D,0x03,0x00,0xEF,0xF0,0xF0,0xFF};
static const uint8_t createPublicKey[]={0x80,0xE0,0xEF,0x02,0x07,0x3E,0x04,0x00,0xF0,0xF0,0xF0,0xFF};


static const uint8_t AutoCreatekey[]={0x80,0xCE,0xEF,0X01,0x00 };

//5028367CE5BDA0E14D70F77DE07F5DACE416EE11A1AE174C310FEFA7D26F9D6F
static const	uint8_t ICPrivateKey[] = {0x50,0x28,0x36,0x7C,0xE5,0xBD,0xA0,0xE1,0x4D,0x70,0xF7,0x7D,0xE0,0x7F,0x5D,0xAC, 					    
																0xE4,0x16,0xEE,0x11,0xA1,0xAE,0x17,0x4C,0x31,0x0F,0xEF,0xA7,0xD2,0x6F,0x9D,0x6F};
//92EFEB458D2D251E389BBA99C50224A0132B3CDF972D5ADFD06D73F5E2D1A5D7
//533C8169840AF682EC44E09E0BF2FD3A5D5E90555E6D1D4FA3A6B91C1A288B51
static const uint8_t	ICPublicKey[]	=	{0x92,0xEF,0xEB,0x45,0x8D,0x2D,0x25,0x1E,0x38,0x9B,0xBA,0x99,0xC5,0x02,0x24,0xA0,
															0x13,0x2B,0x3C,0xDF,0x97,0x2D,0x5A,0xDF,0xD0,0x6D,0x73,0xF5,0xE2,0xD1,0xA5,0xD7, 
															0x53,0x3C,0x81,0x69,0x84,0x0A,0xF6,0x82,0xEC,0x44,0xE0,0x9E,0x0B,0xF2,0xFD,0x3A,
															0x5D,0x5E,0x90,0x55,0x5E,0x6D,0x1D,0x4F,0xA3,0xA6,0xB9,0x1C,0x1A,0x28,0x8B,0x51};

u8 signatureData[270]={0};
u8 i2cWriteData[270]={0};
 u8 setid[30]={0}; 
//static uint8_t randomNumber[]={0x00,0x84,0x00,0x00,0x08};


/**************************************************
*函数名称：void IIC_IO_INIT(void)
*入口参数：无
*出口参数：无
*功能说明：I2C 引脚初始化
***************************************************/
static void iicInit(void)   
{
	GPIO_InitType	GPIO_InitStructure;
	RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOA, ENABLE);
	RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOB, ENABLE);
	
	GPIO_InitStructure.Pin        = GPIO_PIN_6 | GPIO_PIN_7;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitPeripheral(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.Pin        = GPIO_PIN_1;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitPeripheral(GPIOA, &GPIO_InitStructure);
	IIC_SDA_H;
	IIC_SCL_H;
}

void delay_us(u16 time)
{

	u8 i=0;
	while(time--)
	{
		i=10;
		while(i--)
			;
	}

}

/*********************复位 LKT**********************/
//static void iicReset(void)
//{
//	IIC_REST_L;//RST拉低
//	delay_ms(4);
//	IIC_REST_H ;//RST拉高
//	delay_ms(20);//(1000);
//}
/*********************IIC停止信号 接收***********************/
static void iicStop1(void)
{	
	SDA_OUT();
	IIC_SCL_L; 
	delay_us(2);	
	IIC_SDA_L;
	delay_us(3);
	IIC_SCL_H;
	delay_us(3);
	IIC_SDA_H;
}
/*********************IIC停止信号2发送***********************/
static void iicStop2(void)
{	
	SDA_OUT();
	IIC_SCL_L; 
	delay_us(2);
	IIC_SDA_L;
	delay_us(3);
	IIC_SCL_H;
  delay_us(3);
  IIC_SDA_H;
}




/*********************IIC起始信号***********************/
static void iicStart(void)
{
	SDA_OUT();
	IIC_SDA_H;
	IIC_SCL_H;
	delay_us(6);	
	IIC_SDA_L;//SDA拉低
	delay_us(4);	
	IIC_SCL_L;
}
	
/************MCU向LKT发送应答信号**********************/
static void iicAck(void)
{	
	IIC_SCL_L;
	SDA_OUT();
  IIC_SDA_L;
	delay_us(3);
	IIC_SCL_H; //SCL拉低
	delay_us(4);
	IIC_SCL_L; //SCL 拉低
}

/************MCU向LKT发送非应答信号**********************/
static void iicNoAck(void)
{
	IIC_SCL_L;
	SDA_OUT();
	IIC_SDA_H; //SDA拉高
	delay_us(3);  
	IIC_SCL_H; 
	delay_us(4);
	IIC_SCL_L; //SCL 拉低
}

/**************检测LKT是否返回应答信息**********************/
static uint8_t iicCheckAck(void)
{ 
  uint8_t ack = 0;	
	u8 ucErrTime=0;
	IIC_SCL_L; //SCL 拉低
	SDA_OUT();
	IIC_SDA_H; //SDA
	SDA_IN();
	delay_us(2);
	IIC_SCL_H; //SCL拉高  	
	delay_us(2);	
  while(READ_SDA)
	{
			ucErrTime++;
			if(ucErrTime>250)
			{
				iicStop2();
				ack =1;
				return ack; /*0:ack  1:noack*/			
			}
		
	}
	IIC_SCL_L;
  return ack; /*0:ack  1:noack*/
}


/*************** 发送一个字节数据子函数 **********************/
static uint8_t iicSendByte(uint8_t txdata,uint8_t ack)
{
	uint8_t res = 1;
  SDA_OUT();
	for(uint8_t i=0;i<8;i++)
	{
		IIC_SCL_L;
		delay_us(3);
		if(txdata&0x80)
		{
			IIC_SDA_H;
		}
		else
		{
			IIC_SDA_L;
		}
		delay_us(2);
		IIC_SCL_H;
		delay_us(4);
		txdata <<= 1;
	}
	if(!ack)
	{
		res = iicCheckAck();
	}
	else
	{
		IIC_SCL_L;
		delay_us(3);
		IIC_SDA_H;
		SDA_IN();
		IIC_SCL_H;
		delay_us(3);
		IIC_SCL_L;
		res = 0;
	}
	return res;
}
/*************** 接收一字节子程序 ******************************/
static uint8_t iicRecByte(uint8_t ack)
{
	uint8_t recData = 0;
  SDA_IN();
	for(uint8_t i=0;i<8;i++)
	{
		IIC_SCL_L; 
		delay_us(5);	
		IIC_SCL_H;
		recData <<= 1;
    if(READ_SDA)
		{
			recData |= 0x01;
		}
		delay_us(2);
	}
	if(!ack)
	{
	  iicAck();
	}
	else
	{
		iicNoAck();
	}
  return recData;
}

/****************发送n字节数据子程序 *****************************/
uint8_t LKT4305SendData(uint8_t* pData, uint16_t len)
{
	uint8_t res = 1;
	iicStart();
	do{
		/*发送写地址*/
		if(iicSendByte((uint8_t)LKT4305_WRITE_ADDR,0)) /*0 ACK*/
		{
			iicStop2();
			break;
		}
		/*发送数据长度高8位*/
		if(iicSendByte((uint8_t)(len>>8),0)) /*0 ACK*/
		{
			iicStop2();
			break;
		}
		/*发送数据长度低8位*/
		if(iicSendByte((uint8_t)len,0)) /*0 ACK*/
		{
			iicStop2();
			break;
		}
		/*发送数据*/
		uint16_t i;
		for(i=0;i<len;i++)
		{
			if(iicSendByte(pData[i],0)) /*0 ACK*/
			{
				iicStop2();
				break;
			}
		}
		if(i>=len)
		{
			iicStop2();
			res = 0;
		}
	}while(0);
	
	return res;
}

uint16_t LKT4305RecData(uint8_t* pData, uint16_t maxLen)
{
	uint16_t recLen = 0;
	uint8_t recData;
	iicStart();
	do{
		/*发送读地址*/
		if(iicSendByte((uint8_t)LKT4305_READ_ADDR,0)) /*0 ACK*/
		{
			iicStop1();
			recLen = 0;
			break;
		}
		recData = iicRecByte(0);
		recLen |= (uint16_t)(recData<<8);
		recData = iicRecByte(0);
		recLen |= (uint16_t)recData;
		if(recLen>maxLen)
		{
			iicStop1();
			recLen = 0;
			break;
		}
		for(uint16_t i=0;i<recLen;i++)
		{
			if(i!=recLen-1)
			pData[i] = iicRecByte(0);
			else
			pData[i] = iicRecByte(1);	
		}
		
		
	}while(0);
	iicStop1();
	return recLen;
}

void LKT4305Init(void)
{
	iicInit();
	GPIO_ResetBits(GPIOA, GPIO_PIN_1);
	osDelay(20); 
	GPIO_SetBits(GPIOA, GPIO_PIN_1);
	osDelay(100);

	
}
int WritePublickey(u8 *publickey);

int CreatePlatformPublicKeyFile(void)   
{
	int res=-1;
	u16 len=0;
	const uint8_t createPlatformPublicKey[]={0x80,0xE0,0xEF,0x03,0x07,0x3E,0x04,0x00,0xF0,0xF0,0xF0,0xFF};//80E0 EF03 07 3E0400F0F0F0FF  创建EF03
	res = LKT4305SendData((uint8_t*)createPlatformPublicKey, sizeof(createPlatformPublicKey)); 
	osDelay(10);
	if(res)
	{
		osDelay(50);
		res = LKT4305SendData((uint8_t*)createPlatformPublicKey, sizeof(createPlatformPublicKey)); 
		if(res)
				return -1;	
	}

		osDelay(10);
		len = LKT4305RecData(iicReadData, sizeof(iicReadData));	
	
	if((len==2)&&(iicReadData[0]==0x90)&&(iicReadData[1]==0x00))
		{
			return 0;
		}
		else
		{
			return -1;
		}	
	
}
int WritePlatformPublicKey_32Bytes(u8 *pkey)
{
int res=-1;
	int len=0;
	i2cWriteData[0]=0;
	i2cWriteData[1]=0xD6;
	i2cWriteData[2]=0;
	i2cWriteData[3]=0x00;
	i2cWriteData[4]=0x42;
	i2cWriteData[5]=0x51;
	i2cWriteData[6]=0x20;
	memcpy(&i2cWriteData[7],pkey,64-32);
	res = LKT4305SendData(i2cWriteData, 71-32);
	if(res)
	{	
		osDelay(50);
		res = LKT4305SendData(i2cWriteData, 71-32);		
		if(res)
			return -1;
	}
	osDelay(100);
	len = LKT4305RecData(iicReadData, sizeof(iicReadData));	
	if((len==2)&&(iicReadData[0]==0x90)&&(iicReadData[1]==0x00))
		{
			return 0;
		}
		else
		{
			return -1;
		}
}
//00D6 0000 42 5140 1122334455667788990A0B0C0D0E0F101112131415161718191A1B1C1D1E1F201122334455667788990A0B0C0D0E0F101112131415161718191A1B1C1D1E1F20
int WritePlatformPublicKey(u8 *pkey)
{
int res=-1;
	int len=0;
	i2cWriteData[0]=0;
	i2cWriteData[1]=0xD6;
	i2cWriteData[2]=0;
	i2cWriteData[3]=0x00;
	i2cWriteData[4]=0x42;
	i2cWriteData[5]=0x51;
	i2cWriteData[6]=0x40;
	memcpy(&i2cWriteData[7],pkey,64);
	res = LKT4305SendData(i2cWriteData, 71);
	if(res)
	{	
		osDelay(50);
		res = LKT4305SendData(i2cWriteData, 71);		
		if(res)
			return -1;
	}
	osDelay(100);
	len = LKT4305RecData(iicReadData, sizeof(iicReadData));	
	if((len==2)&&(iicReadData[0]==0x90)&&(iicReadData[1]==0x00))
		{
			return 0;
		}
		else
		{
			return -1;
		}
}
int WriteICPublicKey2File(u8 *pkey)   //64
{
	const u8 selectFile[]={0,0xA4,0,0,0X02,0XEF,0X02};//00A4 0000 02 EF03 选择文件
	int res=-1;
	int len=0;
	res = LKT4305SendData((uint8_t*)selectFile, sizeof(selectFile));
	if(res)
	{
		osDelay(50);
		res = LKT4305SendData((uint8_t*)selectFile, sizeof(selectFile));
			if(res)
				return -1;
	}
	osDelay(50);
	len = LKT4305RecData(iicReadData, sizeof(iicReadData));	
	if((len==2)&&(iicReadData[0]==0x90)&&(iicReadData[1]==0x00))//select OK
		{
					res=WritePlatformPublicKey(pkey);
					return res;		
		}
		else
		{
			return -1;
		}
	
}

//static const uint8_t createPrivateKey[]={0x80,0xE0,0xEF,0x01,0x07,0x3D,0x03,0x00,0xEF,0xF0,0xF0,0xFF};
//static const uint8_t createPublicKey[]={0x80,0xE0,0xEF,0x02,0x07,0x3E,0x04,0x00,0xF0,0xF0,0xF0,0xFF};

int WriteICPrivateKey2File(u8 *pkey)
{
	const u8 selectFile[]={0,0xA4,0,0,0X02,0XEF,0X01};//00A4 0000 02 EF03 选择文件
	int res=-1;
	int len=0;
	res = LKT4305SendData((uint8_t*)selectFile, sizeof(selectFile));
	if(res)
	{
		osDelay(50);
		res = LKT4305SendData((uint8_t*)selectFile, sizeof(selectFile));
			if(res)
				return -1;
	}
	osDelay(50);
	len = LKT4305RecData(iicReadData, sizeof(iicReadData));	
	if((len==2)&&(iicReadData[0]==0x90)&&(iicReadData[1]==0x00))//select OK
		{
					res=WritePlatformPublicKey_32Bytes(pkey);
					return res;		
		}
		else
		{
			return -1;
		}
}
int WritePlatformPublicKey2File(u8 *pkey)
{
	const u8 selectFile[]={0,0xA4,0,0,0X02,0XEF,0X03};//00A4 0000 02 EF03 选择文件
	int res=-1;
	int len=0;
	res = LKT4305SendData((uint8_t*)selectFile, sizeof(selectFile));
	if(res)
	{
		osDelay(50);
		res = LKT4305SendData((uint8_t*)selectFile, sizeof(selectFile));
			if(res)
				return -1;
	}
	osDelay(50);
	len = LKT4305RecData(iicReadData, sizeof(iicReadData));	
	if((len==2)&&(iicReadData[0]==0x90)&&(iicReadData[1]==0x00))//select OK
		{
					res=WritePlatformPublicKey(pkey);
					return res;		
		}
		else
		{
			return -1;
		}
	
}
u8 LKT4305Test(void) 
{

		uint8_t res;
		uint16_t len = 0;
//	
//		res = LKT4305SendData(randomNumber, sizeof(randomNumber));
//		len = LKT4305RecData(iicReadData, sizeof(iicReadData));

//		if(get_keystate()==1)
//			return 1;
//		
		res = LKT4305SendData((uint8_t*)createMainflie, sizeof(createMainflie));									//主文件
		if(res)
		{
			osDelay(50);
			res = LKT4305SendData((uint8_t*)createMainflie, sizeof(createMainflie));	
		}
		osDelay(100);
		len = LKT4305RecData(iicReadData, sizeof(iicReadData));	//6a86	
		osDelay(10);
	//------------------
		res = LKT4305SendData((uint8_t*)earsefile, sizeof(earsefile)); //9000											//擦除所有文件
		if(res)
		{		
			osDelay(50);
			res = LKT4305SendData((uint8_t*)earsefile, sizeof(earsefile)); //9000											//擦除所有文件			
			if(res)
				return 0;
		}
		osDelay(100);	
		len = LKT4305RecData(iicReadData, sizeof(iicReadData));
		if(	!((len==2)&&(iicReadData[0]==0x90)&&(iicReadData[1]==0x00))	)
				return 0;
			osDelay(10);
	//------------------	
		res = LKT4305SendData((uint8_t*)createKeyflie, sizeof(createKeyflie));										//创建秘钥文件
		if(res)
		{
			osDelay(50);
			res = LKT4305SendData((uint8_t*)createKeyflie, sizeof(createKeyflie));	
			if(res)
				return 0;			
		}
				osDelay(500);
			len = LKT4305RecData(iicReadData, sizeof(iicReadData));	
			if(	!((len==2)&&(iicReadData[0]==0x90)&&(iicReadData[1]==0x00))	)
				return 0;
		osDelay(50);
	//------------------		
		res = LKT4305SendData((uint8_t*)createPrivateKey, sizeof(createPrivateKey));							//创建安全芯片私钥文件-签名
		if(res)		
		{
			osDelay(50);
			res = LKT4305SendData((uint8_t*)createPrivateKey, sizeof(createPrivateKey));	
			if(res)
				return 0;			
		}
			osDelay(100);
			len = LKT4305RecData(iicReadData, sizeof(iicReadData));
			if(	!((len==2)&&(iicReadData[0]==0x90)&&(iicReadData[1]==0x00))	)
				return 0;
		osDelay(10);
	//------------------		
		res = LKT4305SendData((uint8_t*)createPublicKey, sizeof(createPublicKey)); 								//创建安全芯片公钥文件-签名
		if(res)		
		{			
			osDelay(100);
			res = LKT4305SendData((uint8_t*)createPublicKey, sizeof(createPublicKey)); 		
			if(res)
				return 0;			
		}	
			osDelay(100);
			len = LKT4305RecData(iicReadData, sizeof(iicReadData));		
				if(	!((len==2)&&(iicReadData[0]==0x90)&&(iicReadData[1]==0x00))	)
					return 0;
		osDelay(100);
	//------------------
		if(CreatePlatformPublicKeyFile()!=0)																													// 创建平台公钥文件--加密			
		return 0;
		osDelay(10);
	//---------------------------------
//	res = LKT4305SendData((uint8_t*)AutoCreatekey, sizeof(AutoCreatekey));//生成一对						//生成安全芯片秘钥对
//	if(res)
//	{
//		osDelay(50);
//		res = LKT4305SendData((uint8_t*)AutoCreatekey, sizeof(AutoCreatekey));//生成一对	
//		if(res)
//			return 0;			
//	}
		res = WriteICPrivateKey2File((uint8_t*)ICPrivateKey );		
		if(res)
		{
			osDelay(50);
			res = WriteICPrivateKey2File((uint8_t*)ICPrivateKey );
			if(res)
				return 0;	
		}

		res = WriteICPublicKey2File((uint8_t*)ICPublicKey);
		if(res)
		{
			osDelay(50);
			res = WriteICPublicKey2File((uint8_t*)ICPublicKey);
			if(res)
				return 0;	
		}

		osDelay(200);
//		len = LKT4305RecData(iicReadData, sizeof(iicReadData));	
//		if( (iicReadData[0]==0x90)&&(iicReadData[1]==0x00) )
//		{
//			if(WritePublickey(iicReadData)!=0)																		// 存储安全芯片公钥
//			{
				set_keystate(1);
				return 1;
//			}
//		}
//		return 0;
}

u8* Signature(u8* pInData,u16 inLen)//改   签名是出来64字节    加密是出来96字节
{
		u8 res=0;
		u8 len=0;
		u8 *p=NULL;

	uint8_t CycleNo = 0;
	uint8_t SuccessCount = 0;
	uint8_t signatureDataLen = 0 ;
	uint8_t CycleNoRemainLen = 0;


		if(inLen <= 255)
		{
				signatureData[0]=0x80;
				signatureData[1]=0xDD;
				signatureData[2]=0xEF;
				signatureData[3]=0x01;
				signatureData[4]=inLen;
				memcpy(&signatureData[5],pInData,inLen);		
				res = LKT4305SendData(signatureData, 5+inLen);
				osDelay(10);
				if(res)
				{
					res = LKT4305SendData(signatureData, 5+inLen);	
					osDelay(10);		
				}			
				len = LKT4305RecData(iicReadData, sizeof(iicReadData));	

		}
		else
		{			
			CycleNoRemainLen = inLen%0x40;
			if(CycleNoRemainLen)
			{
				CycleNo = inLen/0x40+1;
			}
			else
			{
				CycleNo = inLen/0x40;
			}
			for(uint8_t i = 0; i < CycleNo+2 ;i++)
			{

				if(i == 0)				//	C1
				{
					signatureData[0]=0x80;
					signatureData[1]=0xD7;
					signatureData[2]=0x00;
					signatureData[3]=0x00;
					signatureData[4]=0x02;
					signatureData[5]=0xEF;
					signatureData[6]=0x01;
					signatureDataLen = 7;
				}
				else if(i == (CycleNo+2-1))		//	C3
				{
					signatureData[0]=0x80;
					signatureData[1]=0xD7;
					signatureData[2]=0x00;
					signatureData[3]=0x03;
					signatureData[4]=0x40;
					signatureDataLen = 5;
				}
				else if(i == (CycleNo+2-2))    //	C2最后一包
				{
					signatureData[0]=0x80;
					signatureData[1]=0xD7;
					signatureData[2]=0x00;
					signatureData[3]=0x02;
					signatureData[4]=CycleNoRemainLen;
					signatureDataLen = CycleNoRemainLen +5;
					memcpy(&signatureData[5],pInData+((i-1)*0x40),CycleNoRemainLen);
				}
				else    			//	C2 0x32字节数据加密
				{
					signatureData[0]=0x80;
					signatureData[1]=0xD7;
					signatureData[2]=0x00;
					signatureData[3]=0x01;
					signatureData[4]=0x40;	
					signatureDataLen = 0x40 +5;
					memcpy(&signatureData[5],pInData+((i-1)*0x40),0x40);
				}

				res = LKT4305SendData(signatureData,signatureDataLen);
				if(res)
				{
					osDelay(10);
					res = LKT4305SendData(signatureData,signatureDataLen);		
				}
				osDelay(10);
			}
			osDelay(10);
			len = LKT4305RecData(iicReadData, sizeof(iicReadData));
		}

//		osDelay(1);
		if((len==66)&&(iicReadData[64]==0x90)&&(iicReadData[65]==0x00))
		{
			p=iicReadData;		
			return p;
		}
		return NULL;
}
extern u8 SM2ID[16];
u8 Set4305_Sm2id(void)//改   设置SM2id
{
		u8 res=0;
		u8 len=0;
	
		setid[0]=0x80;
		setid[1]=0xD7;
		setid[2]=0x00;
		setid[3]=0x1D;
		setid[4]=0x10;
		memcpy(&setid[5],SM2ID,16);		
		res = LKT4305SendData(setid, 5+16);
		osDelay(100);
		len = LKT4305RecData(iicReadData, sizeof(iicReadData));	
//		osDelay(1);
		if((len==2)&&(iicReadData[0]==0x90)&&(iicReadData[1]==0x00))
		{
			res=1;	
		}
		return res;
}

uint8_t C1C2C3[1024] ={0};   // C1C2C3的大小 > 加密数据字节数+96
u8 *Encrypt(u8* pInData,u16 inLen)
{
	int res=-1;
	int len=0;
	uint8_t CycleNo = 0;
	uint8_t SuccessCount = 0;
	uint8_t i2cWriteDataLen = 0 ;
	uint8_t CycleNoRemainLen = 0;
	if(inLen <= 255)
	{
		i2cWriteData[0]=0x80;
		i2cWriteData[1]=0x46;
		i2cWriteData[2]=0xEF;
		i2cWriteData[3]=0x03;
		i2cWriteData[4]=inLen;	
		
		memcpy(&i2cWriteData[5],pInData,inLen);
		res = LKT4305SendData(i2cWriteData,5+inLen);
		if(res)
		{
			osDelay(50);
			res = LKT4305SendData(i2cWriteData,5+inLen);		
		}
		osDelay(50);
		len = LKT4305RecData(iicReadData, sizeof(iicReadData));	
		if((len==inLen+96+2)&&(iicReadData[inLen+96]==0x90)&&(iicReadData[inLen+96+1]==0x00))
			{
				return iicReadData;//成功
			}
			else
			{
				return NULL;//失败
			}	
	}
	else
	{
			CycleNoRemainLen = inLen%0x60;
			if(CycleNoRemainLen)
			{
				CycleNo = inLen/0x60+1;
			}
			else
			{
				CycleNo = inLen/0x60;
			}
			for(uint8_t i = 0; i < (CycleNo+2) ;i++)
			{
				if(i == 0)				//	C1
				{
					i2cWriteData[0]=0x80;
					i2cWriteData[1]=0xD7;
					i2cWriteData[2]=0x01;
					i2cWriteData[3]=0x00;
					i2cWriteData[4]=0x02;
					i2cWriteData[5]=0xEF;
					i2cWriteData[6]=0x03;
					i2cWriteDataLen = 7;
				}
				else if(i == (CycleNo+2-1))		//	C3
				{
					i2cWriteData[0]=0x80;
					i2cWriteData[1]=0xD7;
					i2cWriteData[2]=0x01;
					i2cWriteData[3]=0x02;
					i2cWriteData[4]=0x20;
					i2cWriteDataLen = 5;
				}
				else if(i == (CycleNo+2-2))    //	C2最后一包
				{
					i2cWriteData[0]=0x80;
					i2cWriteData[1]=0xD7;
					i2cWriteData[2]=0x01;
					i2cWriteData[3]=0x01;
					i2cWriteData[4]=CycleNoRemainLen;
					i2cWriteDataLen = CycleNoRemainLen +5;
					memcpy(&i2cWriteData[5],pInData+((i-1)*0x60),CycleNoRemainLen);
				}
				else    			//	C2 每包数据必须是0x20的倍数
				{
					i2cWriteData[0]=0x80;
					i2cWriteData[1]=0xD7;
					i2cWriteData[2]=0x01;
					i2cWriteData[3]=0x01;
					i2cWriteData[4]=0x60;	
					i2cWriteDataLen = 0x60 +5;
					memcpy(&i2cWriteData[5],pInData+((i-1)*0x60),0x60);
				}

				res = LKT4305SendData(i2cWriteData,i2cWriteDataLen);
				if(res)
				{
					osDelay(50);
					res = LKT4305SendData(i2cWriteData,i2cWriteDataLen);		
				}
				osDelay(50);
				len = LKT4305RecData(iicReadData, sizeof(iicReadData));
	
					if(i == 0)
					{
						if((len==64+2)&&(iicReadData[64]==0x90)&&(iicReadData[64+1]==0x00))  //OK
						{
							memcpy(C1C2C3,iicReadData,len-2);
							SuccessCount++;
						}
					}
					else if(i == (CycleNo+1))
					{
						if((len==32+2)&&(iicReadData[32]==0x90)&&(iicReadData[32+1]==0x00))
						{
				//			memcpy(C1C3C2+64,iicReadData,len-2);
							memcpy(C1C2C3+64+inLen,iicReadData,len-2);
							SuccessCount++;

						}
					}
					else
					{
						if((len==i2cWriteDataLen-5+2)&&(iicReadData[i2cWriteDataLen-5]==0x90)&&(iicReadData[i2cWriteDataLen-5+1]==0x00))
						{
				//			memcpy(C1C3C2+96+(i-1)*0x60,iicReadData,len-2);
								memcpy(C1C2C3+64+(i-1)*0x60,iicReadData,len-2);
							SuccessCount++;

						}
					}
				}
				
				if(SuccessCount ==CycleNo+2)
				{
					return C1C2C3;//成功
				}
				else
				{
					return NULL;//失败
				}	
		}
}

int WritePublickey(u8 *publickey)
{
	int res=-1;
	int len=0;
	i2cWriteData[0]=0;
	i2cWriteData[1]=0xD6;
	i2cWriteData[2]=0x82;
	i2cWriteData[3]=0x00;
	i2cWriteData[4]=0x42;
	i2cWriteData[5]=0x51;
	i2cWriteData[6]=0x40;
	memcpy(&i2cWriteData[7],publickey,64);
	res = LKT4305SendData(i2cWriteData, 71);
	if(res)
	{
		osDelay(10);
		res = LKT4305SendData(i2cWriteData, 71);
		if(res)
			return -1;	
	}
	osDelay(10);
	len = LKT4305RecData(iicReadData, sizeof(iicReadData));	
	if((len==2)&&(iicReadData[0]==0x90)&&(iicReadData[1]==0x00))
		{
			return 0;
		}
		else
		return -1;
}

u8 selectFile[]={0,0xA4,0,0,0X02,0XEF,0X02};
u8 readPublickey[]={0,0xB0,0,0,0X42};
u8 *ReadPublickey(void)
{
	//00A4 0000 02 EF02选择公钥文件
	
	//00B0 0000 40 读取公钥文件
	int res=-1;
	int len=0;
	u8 *p=NULL;
	res = LKT4305SendData(selectFile, sizeof(selectFile));
	if(res)
	{
		osDelay(10);		
		res = LKT4305SendData(selectFile, sizeof(selectFile));
	}
		osDelay(10);	
	len = LKT4305RecData(iicReadData, sizeof(iicReadData));	
	if((len==2)&&(iicReadData[0]==0x90)&&(iicReadData[1]==0x00))
		{
//			return 0;
		}
		else
		{
			return NULL;
		}
		
	res = LKT4305SendData(readPublickey, sizeof(readPublickey));
	osDelay(10);
	if(res)
	{
		osDelay(10);	
		res = LKT4305SendData(readPublickey, sizeof(readPublickey));
	}
	osDelay(10);		
	len = LKT4305RecData(iicReadData, sizeof(iicReadData));	
	if((len==68)&&(iicReadData[66]==0x90)&&(iicReadData[67]==0x00))
		{
			p=&iicReadData[2];
			return p;
		}	
		else
		{
			return NULL;
		}
	
}
