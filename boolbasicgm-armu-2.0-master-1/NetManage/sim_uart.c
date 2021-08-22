#include "sim_uart.h"
#include "define.h"
#include "user_config.h"
#include "Bsp.h"
#include "UART.h"

#include "MultiSocket.h"

uint8_t sim_uart_tx_buf[SIM_UART_TX_BUF_SIZE];
static uint16_t tx_write_pos = 0;
static uint16_t tx_read_pos = 0;
static uint8_t uart_sending_flag = 0;

uint8_t sim_uart_rx_buf[SIM_UART_RX_BUF_SIZE];
//uint8_t sim_uart_rx_buf[SIM_UART_RX_BUF_SIZE];
static uint16_t rx_write_pos = 0;
//static uint16_t rx_read_pos = 0;

/*
//�������ڷ�������
//�������ݵ�ַ
*/
static uint8_t *get_uart_send_data(void)
{
	uint8_t *pdata = NULL;
	
	if(tx_read_pos != tx_write_pos)
	{
		pdata =  &sim_uart_tx_buf[tx_read_pos];
		
		tx_read_pos++;
		if(tx_read_pos >= SIM_UART_TX_BUF_SIZE)
		{
			tx_read_pos = 0;
		}
	}
	return pdata;
}

uint16_t get_sim_rx_buf_wr_pos(void)
{
	uint16_t rx_write_temp;
	USART_ConfigInt(UART5, USART_INT_RXDNE, DISABLE);
	rx_write_temp = rx_write_pos;
	USART_ConfigInt(UART5, USART_INT_RXDNE, ENABLE);
	return rx_write_temp;
}


uint8_t get_sim_rx_byte(uint16_t rd_pos)
{
  return sim_uart_rx_buf[rd_pos];
}

//uint8_t* get_sim_rx_byte(uint16_t* rd_pos)
//{
//	uint8_t* pdata = NULL;
//	if((*rd_pos)!=rx_write_pos)
//	{
//		pdata = &sim_uart_rx_buf[*rd_pos];
//		(*rd_pos)++;
//		if((*rd_pos)>=SIM_UART_RX_BUF_SIZE)
//		{
//			*rd_pos = 0;
//		}
//	}
//	return pdata;
//}
/*
//��ȡsimģ�������
//pdata ���ݵ�ַ
//�������ݴ�С
*/
void get_sim_data_multiple_bytes(uint8_t *pdata, uint16_t data_pos, uint16_t len)
{
	//uint16_t data_size;
	for(uint16_t i = 0; i < len; i++)
	{
		pdata[i] = sim_uart_rx_buf[data_pos];
		
		data_pos++;
		if(data_pos >= SIM_UART_RX_BUF_SIZE)
		{
			data_pos = 0;
		}	
	}
}

uint16_t get_sim_rx_data_size(uint16_t rx_read_pos)
{
	uint16_t data_size = 0;
	
	uint16_t rx_write_temp;
	USART_ConfigInt(UART5, USART_INT_RXDNE, DISABLE);
	rx_write_temp = rx_write_pos;
	USART_ConfigInt(UART5, USART_INT_RXDNE, ENABLE);
	
	if(rx_write_temp > rx_read_pos)
	{
		data_size = rx_write_temp - rx_read_pos;
	}
	else
	{
		if(rx_write_temp < rx_read_pos)
		{
			data_size = SIM_UART_RX_BUF_SIZE - (rx_read_pos - rx_write_temp);
		}
		else
		{
			data_size = 0;
		}
	}
	return data_size;
}

/*
���㴮�ڽ���buf
*/
//void clear_sim_rx_buf(void)
//{
//	rx_read_pos = rx_write_pos;
//}

/*
//����simģ������
//pdata ���ݵ�ַ
//len ���ݴ�С
//���� 1�ɹ���0ʧ�ܻ��������
*/

uint8_t sim_uart_tx(uint8_t *pdata, uint16_t len)
{
	uint16_t reamain_size;
	uint8_t *send_data;
	uint16_t tx_read_pos_temp;
	
	if(USART_GetIntStatus(UART5, USART_INT_TXDE) == SET)
	{
		USART_ConfigInt(UART5, USART_INT_TXDE, DISABLE);//�ر��ж�
		tx_read_pos_temp = tx_read_pos;
		USART_ConfigInt(UART5, USART_INT_TXDE, ENABLE);//�����ж�
	}
	else
	{
		tx_read_pos_temp = tx_read_pos;
	}
	
	if(tx_write_pos > tx_read_pos_temp)
	{
		reamain_size = SIM_UART_TX_BUF_SIZE - (tx_write_pos - tx_read_pos_temp);
	}
	else
	{
		if(tx_write_pos < tx_read_pos_temp)
		{
			reamain_size = tx_read_pos_temp - tx_write_pos;
		}
		else
		{
			reamain_size = SIM_UART_TX_BUF_SIZE;
		}
	}
	
	if((reamain_size > len) && (pdata != NULL)) //���ֻ������reamain_size-1���ֽ�
	{
		for(uint16_t i = 0; i < len; i++)
		{
			sim_uart_tx_buf[tx_write_pos] = pdata[i];
			
			tx_write_pos++;
			if(tx_write_pos >= SIM_UART_TX_BUF_SIZE)
			{
				tx_write_pos = 0;
			}
		}
		
		//usb_tx_data(pdata, len);
		
		USART_ConfigInt(UART5, USART_INT_TXDE, DISABLE);//�ر��ж�
		if(!uart_sending_flag)
		{
			send_data = get_uart_send_data();
			if(send_data != NULL)
			{
				uart_sending_flag = 1;
				USART_SendData(UART5, *send_data);
								
				USART_ConfigInt(UART5, USART_INT_TXDE, ENABLE);//�����ж�
			}
		}
		else
		{
			USART_ConfigInt(UART5, USART_INT_TXDE, ENABLE);//�����ж�
		}
		
		return 1;
	}
	else
	{
		return 0;
	}
}

/*
//����2�жϺ���
*/

void UART5_IRQHandler(void)                	//����2�жϷ������
{
	uint8_t *send_data;
//	uint8_t rx_data;
	
	if(USART_GetIntStatus(UART5, USART_INT_RXDNE) != RESET)  //�����ж�
	{
	
//	uartRxBuf.pBuf[(uartRxBuf.write++)%BUF_LEN]	 =  
	sim_uart_rx_buf[rx_write_pos] = USART_ReceiveData(UART5);//(UART5->DR);	//��ȡ���յ�������
		
#ifdef debug
  USART_Sendbyte(RS232_USART, sim_uart_rx_buf[rx_write_pos]);
#endif	
		
		
		
		rx_write_pos++;
		if(rx_write_pos >= SIM_UART_RX_BUF_SIZE)
		{
			rx_write_pos = 0;
		}	
  } 
	
	if(USART_GetIntStatus(UART5, USART_INT_TXDE) != RESET)  //�����ж�
	{
		send_data = get_uart_send_data();   //�õ���������
		
		if(send_data != NULL)   //
		{
			USART_SendData(UART5, *send_data); //��������
			
			//usb_tx_data(send_data, 1);
		}
		else
		{
			uart_sending_flag = 0;  //�������ڷ��ͱ�־
			USART_ConfigInt(UART5, USART_INT_TXDE, DISABLE);//�ر��ж�
		}	
	}
} 

//��ʼ��IO ����2 
//bound:������
void sim_uart_init(uint32_t bound)
{
  	GPIO_InitType	GPIO_InitStructure;
	NVIC_InitType 	NVIC_InitStructure;
	USART_InitType 	USART_InitStructure;
	
	RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOC, ENABLE);
	RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOD, ENABLE);
	RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_UART5, ENABLE);
	
	//UART5_TX   GPIOC12
	GPIO_InitStructure.Pin        = GPIO_PIN_12;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitPeripheral(GPIOC, &GPIO_InitStructure);
	//UART5_RX   GPIOD2
	GPIO_InitStructure.Pin        = GPIO_PIN_2;	
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
	GPIO_InitPeripheral(GPIOD, &GPIO_InitStructure);	
	
	/* Enable the USARTy Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel            = UART5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelCmd         = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
	
	USART_InitStructure.BaudRate            = bound;
    USART_InitStructure.WordLength          = USART_WL_8B;
    USART_InitStructure.StopBits            = USART_STPB_1;
    USART_InitStructure.Parity              = USART_PE_NO;
    USART_InitStructure.HardwareFlowControl = USART_HFCTRL_NONE;
    USART_InitStructure.Mode                = USART_MODE_RX | USART_MODE_TX;
	
	USART_Init(UART5, &USART_InitStructure);
	
	USART_ConfigInt(UART5, USART_INT_RXDNE, ENABLE);
	USART_Enable(UART5, ENABLE);

  //��ʼ������buffer����
	tx_write_pos = 0;
	tx_read_pos = 0;
	uart_sending_flag = 0;

	rx_write_pos = 0;	
}
