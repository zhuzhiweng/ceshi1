#include "spi.h"
#include "n32g45x_spi.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F407������
//SPI ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2014/5/6
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	 

//������SPIģ��ĳ�ʼ�����룬���ó�����ģʽ 						  
//SPI1�ڳ�ʼ��
//�������Ƕ�SPI1�ĳ�ʼ��

//������SPIģ��ĳ�ʼ�����룬���ó�����ģʽ 						  
//SPI1�ڳ�ʼ��
//�������Ƕ�SPI1�ĳ�ʼ��
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

//	/* ���� SPI ʱ�� */
//	//RCC_APB2PeriphClockCmd(RCC_SPI, ENABLE);
//	
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
//	
//	/* ʹ�� GPIO ʱ�� */
//	RCC_APB2PeriphClockCmd(RCC_SCK | RCC_MOSI | RCC_MISO, ENABLE);	

//	/* ���� SPI����SCK��MISO �� MOSIΪ��������ģʽ */
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
//	RCC_APB2PeriphResetCmd(RCC_APB2Periph_SPI1,ENABLE);//��λSPI1
//	RCC_APB2PeriphResetCmd(RCC_APB2Periph_SPI1,DISABLE);//ֹͣ��λSPI1
//	
//	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //����SPI�������˫�������ģʽ:SPI����Ϊ˫��˫��ȫ˫��
//	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		//����SPI����ģʽ:����Ϊ��SPI
//	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;		//����SPI�����ݴ�С:SPI���ͽ���8λ֡�ṹ
//	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;		//����ͬ��ʱ�ӵĿ���״̬Ϊ�ߵ�ƽ
//	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;	//����ͬ��ʱ�ӵĵڶ��������أ��������½������ݱ�����
//	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		//NSS�ź���Ӳ����NSS�ܽţ����������ʹ��SSIλ������:�ڲ�NSS�ź���SSIλ����
//	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;		//���岨����Ԥ��Ƶ��ֵ:������Ԥ��ƵֵΪ256
//	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	//ָ�����ݴ����MSBλ����LSBλ��ʼ:���ݴ����MSBλ��ʼ
//	SPI_InitStructure.SPI_CRCPolynomial = 7;	//CRCֵ����Ķ���ʽ
//	SPI_Init(SPI1, &SPI_InitStructure);  //����SPI_InitStruct��ָ���Ĳ�����ʼ������SPIx�Ĵ���
// 
//	SPI_Cmd(SPI1, ENABLE); //ʹ��SPI����

//	SPI1_ReadWriteByte(0xff);//��������		

//}   
////SPI1�ٶ����ú���
////SPI�ٶ�=fAPB2/��Ƶϵ��
////@ref SPI_BaudRate_Prescaler:SPI_BaudRatePrescaler_2~SPI_BaudRatePrescaler_256  
////fAPB2ʱ��һ��Ϊ84Mhz��
//void SPI1_SetSpeed(u8 SPI_BaudRatePrescaler)
//{
//  assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));//�ж���Ч��
//	SPI1->CR1&=0XFFC7;//λ3-5���㣬�������ò�����
//	SPI1->CR1|=SPI_BaudRatePrescaler;	//����SPI1�ٶ� 
//	SPI_Cmd(SPI1,ENABLE); //ʹ��SPI1
//} 
////SPI1 ��дһ���ֽ�
////TxData:Ҫд����ֽ�
////����ֵ:��ȡ�����ֽ�
//u8 SPI1_ReadWriteByte(u8 TxData)
//{		 			 
// 
//  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET){}//�ȴ���������  
//	
//	SPI_I2S_SendData(SPI1, TxData); //ͨ������SPIx����һ��byte  ����
//		
//  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET){} //�ȴ�������һ��byte  
// 
//	return SPI_I2S_ReceiveData(SPI1); //����ͨ��SPIx������յ�����	
// 		    
//}

//������SPIģ��ĳ�ʼ�����룬���ó�����ģʽ 						  
//SPI2�ڳ�ʼ��
//�������Ƕ�SPI2�ĳ�ʼ��
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

//	/* ���� SPI ʱ�� */
//	//RCC_APB2PeriphClockCmd(RCC_SPI, ENABLE);
//	
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
//	
//	/* ʹ�� GPIO ʱ�� */
//	RCC_APB2PeriphClockCmd(RCC_SCK | RCC_MOSI | RCC_MISO, ENABLE);	

//	/* ���� SPI����SCK��MISO �� MOSIΪ��������ģʽ */
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
//	RCC_APB1PeriphResetCmd(RCC_APB1Periph_SPI2,ENABLE);//��λSPI2
//	RCC_APB1PeriphResetCmd(RCC_APB1Periph_SPI2,DISABLE);//ֹͣ��λSPI2
//	
//	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //����SPI�������˫�������ģʽ:SPI����Ϊ˫��˫��ȫ˫��
//	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		//����SPI����ģʽ:����Ϊ��SPI
//	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;		//����SPI�����ݴ�С:SPI���ͽ���8λ֡�ṹ
//	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;		//����ͬ��ʱ�ӵĿ���״̬Ϊ�ߵ�ƽ
//	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;	//����ͬ��ʱ�ӵĵڶ��������أ��������½������ݱ�����
//	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		//NSS�ź���Ӳ����NSS�ܽţ����������ʹ��SSIλ������:�ڲ�NSS�ź���SSIλ����
//	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;		//���岨����Ԥ��Ƶ��ֵ:������Ԥ��ƵֵΪ256
//	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	//ָ�����ݴ����MSBλ����LSBλ��ʼ:���ݴ����MSBλ��ʼ
//	SPI_InitStructure.SPI_CRCPolynomial = 7;	//CRCֵ����Ķ���ʽ
//	SPI_Init(SPI2, &SPI_InitStructure);  //����SPI_InitStruct��ָ���Ĳ�����ʼ������SPIx�Ĵ���
// 
//	SPI_Cmd(SPI2, ENABLE); //ʹ��SPI����

//	SPI2_ReadWriteByte(0xff);//��������		

//}   
////SPI2�ٶ����ú���
////SPI�ٶ�=fAPB2/��Ƶϵ��
////@ref SPI_BaudRate_Prescaler:SPI_BaudRatePrescaler_2~SPI_BaudRatePrescaler_256  
////fAPB2ʱ��һ��Ϊ84Mhz��
//void SPI2_SetSpeed(u8 SPI_BaudRatePrescaler)
//{
//  assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));//�ж���Ч��
//	SPI2->CR1&=0XFFC7;//λ3-5���㣬�������ò�����
//	SPI2->CR1|=SPI_BaudRatePrescaler;	//����SPI2�ٶ� 
//	SPI_Cmd(SPI2,ENABLE); //ʹ��SPI2
//} 
////SPI2 ��дһ���ֽ�
////TxData:Ҫд����ֽ�
////����ֵ:��ȡ�����ֽ�
//u8 SPI2_ReadWriteByte(u8 TxData)
//{		 			 
// 
//  while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET){}//�ȴ���������  
//	
//	SPI_I2S_SendData(SPI2, TxData); //ͨ������SPIx����һ��byte  ����
//		
//  while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET){} //�ȴ�������һ��byte  
// 
//	return SPI_I2S_ReceiveData(SPI2); //����ͨ��SPIx������յ�����	
// 		    
//}








