#ifndef __CRC_H
#define __CRC_H
#include "sys.h" 
#include "stdio.h"
#include <string.h>

//uint32_t CalcCRC32(uint8_t* pdata, uint32_t data_len);
uint32_t CalcCRC32(uint32_t addr, uint32_t data_len);

#endif


