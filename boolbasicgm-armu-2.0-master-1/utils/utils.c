
#include "utils.h"


 uint8_t IsAscii(uint8_t ascii)
{
	uint8_t res = 1;
	if(((ascii>='0')&&(ascii <= '9')) || ((ascii>='A')&&(ascii <= 'Z')) || ((ascii>='a')&&(ascii <= 'z')))
	{
		res = 0;
	}
//	if((ascii>=0x20)&&(ascii<=0x7E))
//	{
//		res = 0;
//	}
	return res;
}





