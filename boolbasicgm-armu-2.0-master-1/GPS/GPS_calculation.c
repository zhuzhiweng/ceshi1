#include "GPS_calculation.h"
#include "spi_flash.h"
#include "power_ctl.h"
uint8_t PenaltyZoneFlag = 0;


static GB17691_location_t GB17691_location;

//static uint32_t multiplied_by10(uint32_t multiplier)
//{
//	return ((multiplier << 3) + (multiplier << 1));
//}

/*
经度格式：dddmm.mmmm
纬度格式：ddmm.mmmm
*/
uint8_t gps_str_to_float(char *str,double *degree)
{
	uint8_t i,j;
	uint8_t str_len;
	uint32_t degree_int = 0;
	uint32_t minute_int = 0;
	uint32_t minute_decima = 0;
	double decima_denominator = 60;
	
	str_len = strlen(str);
	if(str_len > 11)
	{
		return 0;
	}
	
	for(i = 0; i < str_len; i++)
	{
		if(str[i] == '.')   //找小数点
		{
			break;
		}
	}
	
	if((i != 4) && (i!= 5)) //经度小数点在从0开始的第5位，纬度的小数点在从0开始的第4位
	{
		return 0;
	}
	
	//得到整数部分
	for(j=0; j <(i-2);j++)
	{
		if((str[j] < '0') || (str[j] > '9'))
		{
			return 0;
		}
		degree_int = degree_int * 10 + (str[j] - '0');
	}
	
	//得到分度的整数部分
	for(j=i-2; j<i; j++)
	{
		if((str[j] < '0') || (str[j] > '9'))
		{
			return 0;
		}
		minute_int = minute_int * 10 + (str[j] - '0');
	}
	
	//得到分度的小数部分
	for(j=i+1;j<str_len;j++)
	{
		if((str[j] < '0') || (str[j] > '9'))
		{
			return 0;
		}
		minute_decima = minute_decima * 10 + (str[j] - '0');
		decima_denominator *= 10;
	}
	
	*degree = (double)degree_int + minute_int / (double)60 + minute_decima / decima_denominator;
	
	return 1;
}


uint8_t GetGpsState(void)
{
	gps_info_t cur_gps;
	uint8_t	res = 0;
	double degree_double;
	char str[15];

	get_gps_info(&cur_gps);
	
	while(cur_gps.status == 'A')
	{
		res = 1;
		memcpy(str,cur_gps.longitude,sizeof(cur_gps.longitude));
		str[(uint8_t)sizeof(cur_gps.longitude)] = 0;
		if(!gps_str_to_float(str,&degree_double))
		{
			res = 0;
			break;
		}
		memcpy(str,cur_gps.latitude,sizeof(cur_gps.latitude));
		str[(uint8_t)sizeof(cur_gps.latitude)] = 0;
		if(!gps_str_to_float(str,&degree_double))
		{
			res = 0;
			break;
		}
		break;
	}

	return res;
}

uint8_t IsPenaltyZone(void)
{
	//新疆
	//P1,左下角
	//78.8648,33.324303
	//78864800,33324303
	//P2右下角
	//91.844093,35.406611
	//91844093,35406611
	//P3右上角
	//97.455269,42.849523
	//97455269,42849523
//天津

//左下116.997384,39.116832
//116997384,39116832
//右下 
//117.053403,39.148583  天津光达产业园
//117053403,39148583
//右上
//117.09636,39.193546
//117096360,39193546


	double degree_double;
	uint32_t longitude_x_int;
	uint32_t latitude_y_int;
	char str[15];
//新疆
//	//左下
	int P1_longitude_x_int=78864800;
	int P1_latitude_y_int =33324303;
	//右下
	int P2_longitude_x_int=91844093;
	int P2_latitude_y_int= 35406611;
	//右上
	int P3_longitude_x_int=97455269;
	int P3_latitude_y_int= 42849523;
//天津
	//左下
//	int P1_longitude_x_int=116997384;
//	int P1_latitude_y_int =39116832;
//	//右下
//	int P2_longitude_x_int=117053403;
//	int P2_latitude_y_int= 39148583;
//	//右上
//	int P3_longitude_x_int=117096360;
//	int P3_latitude_y_int= 39193546;
	uint8_t res = 0;
//新疆
	float K1 = 0.16;
	float K2 = 1.33;
////天津
//	float K1 = 0.57;
//	float K2 = 1.05;

	int X_X;
	int Y_Y;
	gps_info_t cur_gps;

	get_gps_info(&cur_gps);
	
	while(cur_gps.status == 'A')
	{
		memcpy(str,cur_gps.longitude,sizeof(cur_gps.longitude));
		str[(uint8_t)sizeof(cur_gps.longitude)] = 0;
		if(!gps_str_to_float(str,&degree_double))
		{
			break;
		}
		longitude_x_int = (uint32_t)(degree_double *1000000);


		memcpy(str,cur_gps.latitude,sizeof(cur_gps.latitude));
		str[(uint8_t)sizeof(cur_gps.latitude)] = 0;
		if(!gps_str_to_float(str,&degree_double))
		{
			break;
		}
		latitude_y_int = (uint32_t)(degree_double *1000000);

//longitude_x_int =83940298; //第一区域
//latitude_y_int=  39859516;

//longitude_x_int =87104633; //第二区域
//latitude_y_int=  34809420;

//longitude_x_int =93102152; //第三区域
//latitude_y_int=  39689158;

//longitude_x_int =89752766;//89752766,31107212
//latitude_y_int = 31107212;

//longitude_x_int =117001912;//117.001912,39.169215
//latitude_y_int = 39169215;

		if((longitude_x_int < P1_longitude_x_int) &&(latitude_y_int > P1_latitude_y_int))
		{
			res =1;
		}
		else if((longitude_x_int < P2_longitude_x_int) &&(latitude_y_int > P2_latitude_y_int))
		{
			res =1;
		}
		else if((longitude_x_int < P3_longitude_x_int) &&(latitude_y_int > P3_latitude_y_int))
		{
			res =1;
		}
		else if((longitude_x_int > P1_longitude_x_int) &&(longitude_x_int < P2_longitude_x_int)&& \
						(latitude_y_int > P1_latitude_y_int)&&(latitude_y_int < P2_latitude_y_int))
		{
				X_X = longitude_x_int - P1_longitude_x_int;
				Y_Y =	latitude_y_int -	P1_latitude_y_int;
				if(Y_Y > (X_X *K1))
				{
					res =1;
				}	
		}
		else if((longitude_x_int > P2_longitude_x_int) && (longitude_x_int < P3_longitude_x_int) && \
						(latitude_y_int > P2_latitude_y_int) && (latitude_y_int < P3_latitude_y_int))
		{
				X_X = longitude_x_int - P2_longitude_x_int;
				Y_Y	=	latitude_y_int -	P2_latitude_y_int;
				if(Y_Y > (X_X *K2))
				{
					res =1;
				}	
		}
		break;
	}

	return res;

}



void CheckPenaltyZone(void)
{
	uint8_t  GpsState;	
	uint8_t  PenaltyZoneIs;

	GpsState = GetGpsState();					//return 1 有效  0无效
	PenaltyZoneIs = IsPenaltyZone();		//return 1 在禁区内 0之外


	if(PenaltyZoneFlag)
	{
		if(GpsState&&(!PenaltyZoneIs))
		{
			PenaltyZoneFlag = 0;
			Set_evads(7,0xF);
		}
	}
	else
	{
		if(PenaltyZoneIs)
		{
			PenaltyZoneFlag = 1;
			Set_evads(7,1);
			turn_off_sim_3v8();
		}
	}
}


void get_GB17691_gps_info(GB17691_location_t *location)
{
	double degree_double;
	uint32_t degree_int;
	char str[15];
	gps_info_t cur_gps;
	
	get_gps_info(&cur_gps);
	GB17691_location.status.is_valid = 1;    //0:有效定位  1:无效定位
	
	while(cur_gps.status == 'A')
	{
		memcpy(str,cur_gps.longitude,sizeof(cur_gps.longitude));
		str[(uint8_t)sizeof(cur_gps.longitude)] = 0;
		if(!gps_str_to_float(str,&degree_double))
		{
			break;
		} 
		degree_int = (uint32_t)(degree_double *1000000);
		for(uint8_t i = 0; i < sizeof(GB17691_location.longitude);i++)
		{
		  GB17691_location.longitude[sizeof(GB17691_location.longitude)- 1 - i] = (uint8_t)(degree_int & 0xff);
			degree_int >>=8;
		}
		
		memcpy(str,cur_gps.latitude,sizeof(cur_gps.latitude));
		str[(uint8_t)sizeof(cur_gps.latitude)] = 0;
		if(!gps_str_to_float(str,&degree_double))
		{
			break;
		}
		degree_int = (uint32_t)(degree_double *1000000);
		for(uint8_t i = 0; i < sizeof(GB17691_location.latitude);i++)
		{
		  GB17691_location.latitude[sizeof(GB17691_location.latitude)- 1 - i] = (uint8_t)(degree_int & 0xff);
			degree_int >>=8;
		}
		
		GB17691_location.status.is_valid = 0;
		if(cur_gps.ns == 'S')
		{
		  GB17691_location.status.NorS = 1;
		}
		else
		{
			GB17691_location.status.NorS = 0;
		}
		
		if(cur_gps.ew == 'W')
		{
			GB17691_location.status.EorW = 1;
		}
		else
		{
			GB17691_location.status.EorW = 0;
		}
		
		GB17691_location.speed=cur_gps.speed;
		break;
	}
	
	memcpy(location,&GB17691_location,sizeof(GB17691_location_t));
}



