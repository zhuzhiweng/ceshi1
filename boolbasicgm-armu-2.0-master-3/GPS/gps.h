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
	char latitude[11];  		//纬度  ddmm.mmmm
	char ns;					//北纬，南纬
	char longitude[12];			//经度  dddmm.mmmm
	char ew;					//东经，西经
	uint32_t Pos_qua;			//定位质量
	uint32_t Satellite_num; 	//卫星数量
	float horizontal_precision;	//水平精度
	float height;				//海拔
}gps_gpgga_t;

typedef struct
{
	char utc_time[11];			//UTC time in hhmmss.sss
	char status;				// A/V A有效， V无效
	char latitude[11];			//纬度  ddmm.mmmm
	char ns;					//北纬，南纬
	char longitude[12];			//经度  dddmm.mmmm
	char ew;					//东经，西经
	float speed;				//速度
	float course;
	char utc_date[7]; 			//UTC date of position fix, ddmmyy format
	char mode_indicator;
	
	//uint8_t checksum;
}gps_gprmc_t;

typedef struct
{
	rtc_time_t time;
	char status;  				// A/V A有效， V无效
	char latitude[10];  		//纬度  ddmm.mmmm
	char ns;					//北纬，南纬
	char longitude[11]; 		//经度  dddmm.mmmm
	char ew;   					//东经，西经
	float speed; 				//速度
	float height; 				//海拔
	uint8_t Satellite_num; 		//卫星数量
}gps_info_t;

void gps_uart_init(u32 bound);
void gps_process_frame(void);
void get_gps_info(gps_info_t *gps);
uint8_t get_gps_time(rtc_time_t *time);
void gps_pwr_init(void);
void gps_pwr_turn_on(void);
void gps_pwr_turn_off(void);

#endif


