
#ifndef __BSP_H
#define __BSP_H
#include "n32g45x.h"
#include "define.h"
/**************************结构类型************************************/
typedef enum {APB1 = 0, APB2} APBType;
//typedef enum {OFF = 0, ON} SwitchType;
//typedef enum {Numeric = 0, Character} DataType;
//typedef enum {SYSEND = 0, SYS01,SYS02,SYS03,SYS04,SYS05,SYS06,SYS07,SYSXX,SYSDTC,SYSCDTC,SYSDS,SYSRDS,SYSSTOPDS} MenuType;
;
/**************************函数声明************************************/
void GPS_USART_Config(u32 Baud);
void SIM7600_USART_Config(u32 Baud);

void RS232_USART_Config(u32 Baud);


void GPIOConfig(void);
void NVIC_Config(void);
void CONFIG_ELM327(void);
void CONFIG_STM32(void);
	void ADC_Configuration(void);


///**************************全局变量************************************/
//#define GSP_RX_BUF_SIZE 1024
//uint8_t gps_rx_buf[GSP_RX_BUF_SIZE];
//uint16_t gps_rx_buf_wr_pos = 0;
//uint16_t gps_rx_buf_rd_pos = 0;



/**************************RS232串口配置**********************************/
#define RCC_APBxPeriph_RS232_USART_IO   	RCC_APB2_PERIPH_GPIOA				//RCC_APB2Periph_GPIOA 
#define RCC_APBxPeriph_RS232_USART		 	RCC_APB2_PERIPH_USART1				//RCC_APB2Periph_USART1
#define RS232_USART_TXD				 		GPIO_PIN_9							//GPIO_Pin_9
#define RS232_USART_RXD				 		GPIO_PIN_10							//GPIO_Pin_10
#define RS232_USART_IO						GPIOA
#define RS232_USART	                 		USART1
#define RS232_PinRemap					 	DISABLE
#define RS232_USARTAPB					 	APB2
#define RS232_USART_IRQHandler			 	USART1_IRQHandler
/**************************SIM7600串口配置**********************************/
#define RCC_APBxPeriph_SIM7600_USART_IO   	RCC_APB2Periph_GPIOA
#define RCC_APBxPeriph_SIM7600_USART		RCC_APB1Periph_USART2
#define SIM7600_USART_TXD				 	GPIO_Pin_2
#define SIM7600_USART_RXD				 	GPIO_Pin_3
#define SIM7600_USART_IO					GPIOA
#define SIM7600_USART	                 	USART2
#define SIM7600_PinRemap					DISABLE
#define SIM7600_USARTAPB					APB1
//#define SIM7600_USART_IRQHandler			 USART2_IRQHandler
/**************************串口配置**********************************/
//#define RCC_APBxPeriph_KL327_USART_IO   RCC_APB2Periph_GPIOB
//#define RCC_APBxPeriph_KL327_USART		 RCC_APB1Periph_USART3
//#define KL327_USART_TXD				 GPIO_Pin_10
//#define KL327_USART_RXD				 GPIO_Pin_11
//#define KL327_USART_IO					 GPIOB
//#define KL327_USART	                 USART3
//#define KL327_PinRemap					 DISABLE
//#define KL327_USARTAPB					 APB1
//#define KL327_USART_IRQHandler			 USART1_IRQHandler




///**************************LED配置*************************************/
//#define RCC_APBxPeriph_LED_IO	     RCC_APB2Periph_GPIOA
//#define LED_D2						 GPIO_Pin_6
//#define LED_IO						 GPIOA
///**************************CAN配置*************************************/
//#define CAN_500K                     4
//#define CAN_250K                     8
//#define CAN_125K                     16
//#define RCC_APBxPeriph_CAN_IO        RCC_APB2Periph_GPIOB
//#define CAN_RXD					     GPIO_Pin_8
//#define CAN_TXD						 GPIO_Pin_9
//#define CAN_IO						 GPIOB
//#define CAN_PinRemap				 ENABLE
///**************************KL线配置************************************/
//#define K_LINE                       1
//#define L_LINE					     2
//#define Destroy						 0xFF
//#define RCC_APBxPeriph_KL_LINE_IO    RCC_APB2Periph_GPIOB
//#define RCC_APBxPeriph_KL_LINE	     RCC_APB2Periph_USART1
//#define KL_LINE_TXD				     GPIO_Pin_6
//#define KL_LINE_RXD				     GPIO_Pin_7
//#define KL_LINE_IO				     GPIOB
//#define KL_LINE	                     USART1
//#define KL_LINE_PinRemap		     ENABLE
//#define KL_LINEAPB				     APB2
//#define K_LINE_IRQHandler		     USART2_IRQHandler
///**************************KL控制配置**********************************/
//#define RCC_APB2Periph_SET_K_LINE_IO RCC_APB2Periph_GPIOB
//#define RCC_APB2Periph_SET_L_LINE_IO RCC_APB2Periph_GPIOB
//#define SET_K_LINE_PIN				 GPIO_Pin_3
//#define SET_K_LINE_IO				 GPIOB
//#define SET_L_LINE_PIN				 GPIO_Pin_4
//#define SET_L_LINE_IO				 GPIOB
///**********************************************************************/
#endif
