#ifndef _GPS_H
#define _GPS_H
#include "sys.h" 
#include "stdio.h"
#include <string.h>
#include "RTC.h"
#include "n32g45x_gpio.h"

#define GSP_RX_BUF_SIZE 1024
#define GPS_FRAME_MAX_SIZE 100

typedef enum
{
	SEEK_FRAME_HEAD,
	SEEK_ASTERISK_POS,
	SEEK_CHECK_LOW,
	SEEK_CHECK_HIGHT,
	SEEK_END_CHAR
}gps_frame_decode_t;

typedef struct
{
	char utc_time[11];			//UTC time in hhmmss.sss
	char latitude[11];  		//γ��  ddmm.mmmm
	char ns;					//��γ����γ
	char longitude[12];			//����  dddmm.mmmm
	char ew;					//����������
	uint32_t Pos_qua;			//��λ����
	uint32_t Satellite_num; 	//��������
	float horizontal_precision;	//ˮƽ����
	float height;				//����
}gps_gpgga_t;

typedef struct
{
	char utc_time[11];			//UTC time in hhmmss.sss
	char status;				// A/V A��Ч�� V��Ч
	char latitude[11];			//γ��  ddmm.mmmm
	char ns;					//��γ����γ
	char longitude[12];			//����  dddmm.mmmm
	char ew;					//����������
	float speed;				//�ٶ�
	float course;
	char utc_date[7]; 			//UTC date of position fix, ddmmyy format
	char mode_indicator;
	
	//uint8_t checksum;
}gps_gprmc_t;

typedef struct
{
	rtc_time_t time;
	char status;  				// A/V A��Ч�� V��Ч
	char latitude[10];  		//γ��  ddmm.mmmm
	char ns;					//��γ����γ
	char longitude[11]; 		//����  dddmm.mmmm
	char ew;   					//����������
	float speed; 				//�ٶ�
	float height; 				//����
	uint8_t Satellite_num; 		//��������
}gps_info_t;

void gps_uart_init(u32 bound);
void gps_process_frame(void);
void get_gps_info(gps_info_t *gps);
uint8_t get_gps_time(rtc_time_t *time);
void gps_pwr_init(void);
void gps_pwr_turn_on(void);
void gps_pwr_turn_off(void);

#endif


