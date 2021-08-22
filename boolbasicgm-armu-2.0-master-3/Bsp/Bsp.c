
/************************************************************************
*@文件: Bsp.h
*@作者:
*@版本:
*@日期:
*@描述:
************************************************************************/
#include "Bsp.h"
#include "includes.h"

/************************************************************************
  * @
  * @描述: 底层全局变量
  * @
  **********************************************************************/

/************************************************************************
  * @描述:  串口配置  调试串口
  * @参数:  Baud:串口波特率
  * @返回值: None
  **********************************************************************/
void RS232_USART_Config(u32 Baud)
{
    u8 data;
	
	GPIO_InitType 	GPIO_InitStructure;
	USART_InitType 	USART_InitStructure;
	if (RS232_USARTAPB  == APB1) 
    {
		RCC_EnableAPB2PeriphClk(RCC_APBxPeriph_RS232_USART_IO | RCC_APB2_PERIPH_AFIO,ENABLE);
        RCC_EnableAPB1PeriphClk(RCC_APBxPeriph_RS232_USART,ENABLE);
    }
    else
    {
        RCC_EnableAPB2PeriphClk(RCC_APBxPeriph_RS232_USART_IO | RCC_APBxPeriph_RS232_USART | RCC_APB2_PERIPH_AFIO,ENABLE);
    }
	if (RS232_PinRemap == ENABLE)
    {
        GPIO_ConfigPinRemap(GPIO_RMP_USART2,ENABLE);
    }
	
	GPIO_InitStructure.Pin        = RS232_USART_TXD;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitPeripheral(RS232_USART_IO, &GPIO_InitStructure);

	GPIO_InitStructure.Pin        = RS232_USART_RXD;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitPeripheral(RS232_USART_IO, &GPIO_InitStructure);	
	
	USART_InitStructure.BaudRate            = Baud;
    USART_InitStructure.WordLength          = USART_WL_8B;
    USART_InitStructure.StopBits            = USART_STPB_1;
    USART_InitStructure.Parity              = USART_PE_NO;
    USART_InitStructure.HardwareFlowControl = USART_HFCTRL_NONE;
    USART_InitStructure.Mode                = USART_MODE_RX | USART_MODE_TX;
	
	USART_Init(RS232_USART, &USART_InitStructure);
//    data = data;
//    data = RS232_USART->DR;
//    data = RS232_USART->SR;
    NVIC_Config();
	
	USART_ConfigInt(RS232_USART, USART_INT_RXDNE, ENABLE);
//	USART_ITConfig(RS232_USART,USART_IT_IDLE,ENABLE);
	USART_Enable(RS232_USART, ENABLE);
}

/************************************************************************
  * @描述:  中断配置
  * @参数:  None
  * @返回值: None
  **********************************************************************/
void GPIOConfig(void)
{
			
//	//------------------ STB 
//		GPIO_InitTypeDef GPIO_InitStructure;
//    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//    GPIO_Init(GPIOB,&GPIO_InitStructure);	
//		//------------------ POWER SWITCH OF SIM7600 
//    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//    GPIO_Init(GPIOA,&GPIO_InitStructure);	
//		//------------------ POWERkey of 7600
//    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//    GPIO_Init(GPIOB,&GPIO_InitStructure);	


//		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
//		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
//	
//	
//		GPIO_ResetBits(GPIOA, GPIO_Pin_8);
//		GPIO_SetBits(GPIOB, GPIO_Pin_5);
//		GPIO_SetBits(GPIOA, GPIO_Pin_1);

				
}
/************************************************************************
  * @描述:  切换配置
  * @参数:  None
  * @返回值: None
  **********************************************************************/
void CONFIG_ELM327(void)
{
//		GPIO_ResetBits(GPIOA, GPIO_Pin_6); // chushihua ELM327
//		GPIO_ResetBits(GPIOA, GPIO_Pin_5);

}
void CONFIG_STM32(void)
{
//		GPIO_SetBits(GPIOA, GPIO_Pin_6); // KL + CAN
//		GPIO_SetBits(GPIOA, GPIO_Pin_5);

}


/************************************************************************
  * @描述:  中断配置
  * @参数:  None
  * @返回值: None
  **********************************************************************/
void NVIC_Config(void)
{
    NVIC_InitType 	NVIC_InitStructure;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);
	
	NVIC_InitStructure.NVIC_IRQChannel            = CAN2_RX0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd         = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel            = UART4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd         = ENABLE;
    NVIC_Init(&NVIC_InitStructure);	
	
	NVIC_InitStructure.NVIC_IRQChannel            = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd         = ENABLE;
    NVIC_Init(&NVIC_InitStructure);	
	
	NVIC_InitStructure.NVIC_IRQChannel            = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd         = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

void ADC_Configuration(void)
{
	GPIO_InitType 	GPIO_InitStructure;
	ADC_InitType 	ADC_InitStructure;
	
	RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOC, ENABLE);
	RCC_EnableAHBPeriphClk(RCC_AHB_PERIPH_ADC1, ENABLE);
	
	GPIO_InitStructure.Pin        = GPIO_PIN_3;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitPeripheral(GPIOC, &GPIO_InitStructure);
	
	ADC_InitStructure.WorkMode       = ADC_WORKMODE_INDEPENDENT;
    ADC_InitStructure.MultiChEn      = DISABLE;
    ADC_InitStructure.ContinueConvEn = DISABLE;
    ADC_InitStructure.ExtTrigSelect  = ADC_EXT_TRIGCONV_NONE;
    ADC_InitStructure.DatAlign       = ADC_DAT_ALIGN_R;
    ADC_InitStructure.ChsNumber      = 1;
    ADC_Init(ADC1, &ADC_InitStructure);

	ADC_ConfigRegularChannel(ADC1, ADC1_Channel_09_PC3, 1, ADC_SAMP_TIME_55CYCLES5);
	
	/* Enable ADC */
    ADC_Enable(ADC1, ENABLE);
    /*Check ADC Ready*/
    while(ADC_GetFlagStatusNew(ADC1,ADC_FLAG_RDY) == RESET);
    /* Start ADC calibration */
    ADC_StartCalibration(ADC1);
    /* Check the end of ADC calibration */
    while (ADC_GetCalibrationStatus(ADC1));
	
	ADC_EnableSoftwareStartConv(ADC1, ENABLE);	 //启动软件转换
}	 




