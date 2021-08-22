#ifndef __SIM_UART_H
#define __SIM_UART_H			    
#include "sys.h" 
#include "stdio.h"
#include <string.h>
//#include "FreeRTOS.h"
//#include "task.h"
//#include "sim_net.h"
#include "n32g45x_usart.h"
#include "n32g45x_gpio.h"


#define SIM_UART_TX_BUF_SIZE 1024
//#define SIM_UART_RX_BUF_SIZE 1024
#define SIM_UART_RX_BUF_SIZE 2048


void sim_uart_init(u32 bound);
//uint16_t get_sim_data(uint8_t *pdata, uint16_t max_len);
//uint16_t get_sim_char(char* pstr);
//void clear_sim_rx_buf(void);
void get_sim_data_multiple_bytes(uint8_t *pdata, uint16_t data_pos, uint16_t len);
uint8_t sim_uart_tx(uint8_t *pdata, uint16_t len);
uint16_t get_sim_rx_buf_wr_pos(void);
uint8_t get_sim_rx_byte(uint16_t rd_pos);
//uint8_t* get_sim_rx_byte(uint16_t* rd_pos);
uint16_t get_sim_rx_data_size(uint16_t rx_read_pos);
//int str_scanf(char *pcmpstr, char* pgetstr, uint16_t getstr_len, uint16_t timeout);

#endif
