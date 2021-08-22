#ifndef _GPS_CALCULATION_H
#define _GPS_CALCULATION_H
#include "sys.h" 
#include "stdio.h"
//#include "stdio.h"
#include <string.h>
#include "gps.h"

#pragma pack(1)   //按字节对齐
typedef struct
{
	struct
	{
		uint8_t is_valid:1;
		uint8_t NorS:1;
		uint8_t EorW:1;
		uint8_t reserve:5;
	}status;
	uint8_t longitude[4];
	uint8_t latitude[4];
	float speed;
}GB17691_location_t;
#pragma pack()  //取消指定对齐，恢复缺省对齐

void get_GB17691_gps_info(GB17691_location_t *location);
uint8_t gps_str_to_float(char *str,double *degree);
void CheckPenaltyZone(void);
#endif


