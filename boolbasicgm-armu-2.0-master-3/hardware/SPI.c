#include "spi.h"
#include "n32g45x_spi.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//SPI 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2014/5/6
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	 

//以下是SPI模块的初始化代码，配置成主机模式 						  
//SPI1口初始化
//这里针是对SPI1的初始化

//以下是SPI模块的初始化代码，配置成主机模式 						  
//SPI1口初始化
//这里针是对SPI1的初始化
//#define RCC_SCK 	RCC_APB2Periph_GPIOA
//#define PORT_SCK	GPIOA
//#define PIN_SCK		GPIO_Pin_5

//#define RCC_MISO 	RCC_APB2Periph_GPIOA
//#define PORT_MISO	GPIOA
//#define PIN_MISO	GPIO_Pin_6

//#define RCC_MOSI 	RCC_APB2Periph_GPIOA
//#define PORT_MOSI	GPIOA
//#define PIN_MOSI	GPIO_Pin_7


//#define SPI_HARD	SPI1
//#define RCC_SPI		RCC_APB2Periph_SPI1
//	
//	/* SPI or I2S mode selection masks */
//	#define SPI_Mode_Select      ((uint16_t)0xF7FF)
//	#define I2S_Mode_Select      ((uint16_t)0x0800) 
//	
//	/* SPI registers Masks */
//	#define CR1_CLEAR_Mask       ((uint16_t)0x3040)
//	#define I2SCFGR_CLEAR_Mask   ((uint16_t)0xF040)

//	/* SPI SPE mask */
//	#define CR1_SPE_Set          ((uint16_t)0x0040)
//	#define CR1_SPE_Reset        ((uint16_t)0xFFBF)


//void SPI1_Init(void)
//{	 

//  SPI_InitTypeDef  SPI_InitStructure;	

//	GPIO_InitTypeDef GPIO_InitStructure;

//	/* 开启 SPI 时钟 */
//	//RCC_APB2PeriphClockCmd(RCC_SPI, ENABLE);
//	
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
//	
//	/* 使能 GPIO 时钟 */
//	RCC_APB2PeriphClockCmd(RCC_SCK | RCC_MOSI | RCC_MISO, ENABLE);	

//	/* 配置 SPI引脚SCK、MISO 和 MOSI为复用推挽模式 */
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_InitStructure.GPIO_Pin = PIN_SCK;	
//	GPIO_Init(PORT_SCK, &GPIO_InitStructure);
//	
//	GPIO_InitStructure.GPIO_Pin = PIN_MISO;	
//	GPIO_Init(PORT_MISO, &GPIO_InitStructure);

//	GPIO_InitStructure.GPIO_Pin = PIN_MOSI;	
//	GPIO_Init(PORT_MOSI, &GPIO_InitStructure);
//		
//	RCC_APB2PeriphResetCmd(RCC_APB2Periph_SPI1,ENABLE);//复位SPI1
//	RCC_APB2PeriphResetCmd(RCC_APB2Periph_SPI1,DISABLE);//停止复位SPI1
//	
//	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //设置SPI单向或者双向的数据模式:SPI设置为双线双向全双工
//	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		//设置SPI工作模式:设置为主SPI
//	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;		//设置SPI的数据大小:SPI发送接收8位帧结构
//	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;		//串行同步时钟的空闲状态为高电平
//	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;	//串行同步时钟的第二个跳变沿（上升或下降）数据被采样
//	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		//NSS信号由硬件（NSS管脚）还是软件（使用SSI位）管理:内部NSS信号有SSI位控制
//	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;		//定义波特率预分频的值:波特率预分频值为256
//	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	//指定数据传输从MSB位还是LSB位开始:数据传输从MSB位开始
//	SPI_InitStructure.SPI_CRCPolynomial = 7;	//CRC值计算的多项式
//	SPI_Init(SPI1, &SPI_InitStructure);  //根据SPI_InitStruct中指定的参数初始化外设SPIx寄存器
// 
//	SPI_Cmd(SPI1, ENABLE); //使能SPI外设

//	SPI1_ReadWriteByte(0xff);//启动传输		

//}   
////SPI1速度设置函数
////SPI速度=fAPB2/分频系数
////@ref SPI_BaudRate_Prescaler:SPI_BaudRatePrescaler_2~SPI_BaudRatePrescaler_256  
////fAPB2时钟一般为84Mhz：
//void SPI1_SetSpeed(u8 SPI_BaudRatePrescaler)
//{
//  assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));//判断有效性
//	SPI1->CR1&=0XFFC7;//位3-5清零，用来设置波特率
//	SPI1->CR1|=SPI_BaudRatePrescaler;	//设置SPI1速度 
//	SPI_Cmd(SPI1,ENABLE); //使能SPI1
//} 
////SPI1 读写一个字节
////TxData:要写入的字节
////返回值:读取到的字节
//u8 SPI1_ReadWriteByte(u8 TxData)
//{		 			 
// 
//  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET){}//等待发送区空  
//	
//	SPI_I2S_SendData(SPI1, TxData); //通过外设SPIx发送一个byte  数据
//		
//  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET){} //等待接收完一个byte  
// 
//	return SPI_I2S_ReceiveData(SPI1); //返回通过SPIx最近接收的数据	
// 		    
//}

//以下是SPI模块的初始化代码，配置成主机模式 						  
//SPI2口初始化
//这里针是对SPI2的初始化
#define RCC_SCK 	RCC_APB2Periph_GPIOB
#define PORT_SCK	GPIOB
#define PIN_SCK		GPIO_Pin_13

#define RCC_MISO 	RCC_APB2Periph_GPIOB
#define PORT_MISO	GPIOB
#define PIN_MISO	GPIO_Pin_14

#define RCC_MOSI 	RCC_APB2Periph_GPIOB
#define PORT_MOSI	GPIOB
#define PIN_MOSI	GPIO_Pin_15


#define SPI_HARD	SPI2
#define RCC_SPI		RCC_APB1Periph_SPI2
	
	/* SPI or I2S mode selection masks */
	#define SPI_Mode_Select      ((uint16_t)0xF7FF)
	#define I2S_Mode_Select      ((uint16_t)0x0800) 
	
	/* SPI registers Masks */
	#define CR1_CLEAR_Mask       ((uint16_t)0x3040)
	#define I2SCFGR_CLEAR_Mask   ((uint16_t)0xF040)

	/* SPI SPE mask */
	#define CR1_SPE_Set          ((uint16_t)0x0040)
	#define CR1_SPE_Reset        ((uint16_t)0xFFBF)


//void SPI2_Init(void)
//{	 

//  SPI_InitTypeDef  SPI_InitStructure;	

//	GPIO_InitTypeDef GPIO_InitStructure;

//	/* 开启 SPI 时钟 */
//	//RCC_APB2PeriphClockCmd(RCC_SPI, ENABLE);
//	
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
//	
//	/* 使能 GPIO 时钟 */
//	RCC_APB2PeriphClockCmd(RCC_SCK | RCC_MOSI | RCC_MISO, ENABLE);	

//	/* 配置 SPI引脚SCK、MISO 和 MOSI为复用推挽模式 */
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_InitStructure.GPIO_Pin = PIN_SCK;	
//	GPIO_Init(PORT_SCK, &GPIO_InitStructure);
//	
//	GPIO_InitStructure.GPIO_Pin = PIN_MISO;	
//	GPIO_Init(PORT_MISO, &GPIO_InitStructure);

//	GPIO_InitStructure.GPIO_Pin = PIN_MOSI;	
//	GPIO_Init(PORT_MOSI, &GPIO_InitStructure);
//		
//	RCC_APB1PeriphResetCmd(RCC_APB1Periph_SPI2,ENABLE);//复位SPI2
//	RCC_APB1PeriphResetCmd(RCC_APB1Periph_SPI2,DISABLE);//停止复位SPI2
//	
//	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //设置SPI单向或者双向的数据模式:SPI设置为双线双向全双工
//	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		//设置SPI工作模式:设置为主SPI
//	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;		//设置SPI的数据大小:SPI发送接收8位帧结构
//	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;		//串行同步时钟的空闲状态为高电平
//	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;	//串行同步时钟的第二个跳变沿（上升或下降）数据被采样
//	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		//NSS信号由硬件（NSS管脚）还是软件（使用SSI位）管理:内部NSS信号有SSI位控制
//	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;		//定义波特率预分频的值:波特率预分频值为256
//	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	//指定数据传输从MSB位还是LSB位开始:数据传输从MSB位开始
//	SPI_InitStructure.SPI_CRCPolynomial = 7;	//CRC值计算的多项式
//	SPI_Init(SPI2, &SPI_InitStructure);  //根据SPI_InitStruct中指定的参数初始化外设SPIx寄存器
// 
//	SPI_Cmd(SPI2, ENABLE); //使能SPI外设

//	SPI2_ReadWriteByte(0xff);//启动传输		

//}   
////SPI2速度设置函数
////SPI速度=fAPB2/分频系数
////@ref SPI_BaudRate_Prescaler:SPI_BaudRatePrescaler_2~SPI_BaudRatePrescaler_256  
////fAPB2时钟一般为84Mhz：
//void SPI2_SetSpeed(u8 SPI_BaudRatePrescaler)
//{
//  assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));//判断有效性
//	SPI2->CR1&=0XFFC7;//位3-5清零，用来设置波特率
//	SPI2->CR1|=SPI_BaudRatePrescaler;	//设置SPI2速度 
//	SPI_Cmd(SPI2,ENABLE); //使能SPI2
//} 
////SPI2 读写一个字节
////TxData:要写入的字节
////返回值:读取到的字节
//u8 SPI2_ReadWriteByte(u8 TxData)
//{		 			 
// 
//  while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET){}//等待发送区空  
//	
//	SPI_I2S_SendData(SPI2, TxData); //通过外设SPIx发送一个byte  数据
//		
//  while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET){} //等待接收完一个byte  
// 
//	return SPI_I2S_ReceiveData(SPI2); //返回通过SPIx最近接收的数据	
// 		    
//}








