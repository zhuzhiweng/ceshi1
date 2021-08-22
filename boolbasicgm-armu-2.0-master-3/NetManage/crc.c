#include "crc.h"
#include "spi_flash.h"
static	uint32_t m_Table[256];
static uint32_t Reflect(uint32_t ref, uint8_t ch)
{
	uint32_t value = 0;
	for(uint16_t i = 1; i < (ch + 1); i++)
	{
		if(ref & 1)
		{
			value |= 1 << (ch - i);
		}
		ref >>= 1;
	}
	return value;
} 
/******************************************************

*******************************************************/
uint32_t CalcCRC32(uint32_t addr, uint32_t data_len)
{
	uint32_t Result = 0xFFFFFFFF;
	uint32_t len = 0;
	uint8_t data[256];

	uint32_t ulPolynomial = 0x04C11DB7;
	uint16_t i, j;
	
	for(i = 0; i <= 0xFF; i++)
	{
		m_Table[i] = Reflect(i, 8) << 24;
		for (j = 0; j < 8; j++)
		{
			m_Table[i] = (m_Table[i] << 1) ^ (m_Table[i] & ((uint32_t)1 << 31)? ulPolynomial : 0);
		}
		m_Table[i] = Reflect(m_Table[i], 32);
	}

//	while (data_len--)
//	{
//		Result = (Result >> 8) ^ m_Table[(Result & 0xFF) ^ *pdata++];
//	}
	while((len+sizeof(data)) <= data_len)
	{
		read_data_from_flash(data,addr,sizeof(data),FIRMWARE_DATA_FLASH_AREA);
		for(uint16_t i=0;i<sizeof(data);i++)
		{
			Result = (Result >> 8) ^ m_Table[(Result & 0xFF) ^ data[i]];
		}
		addr += sizeof(data);
		len += sizeof(data);
	}
	
	if(len<data_len)
	{
		read_data_from_flash(data,addr,(data_len-len),FIRMWARE_DATA_FLASH_AREA);
		for(uint16_t i=0;i<(data_len-len);i++)
		{
			Result = (Result >> 8) ^ m_Table[(Result & 0xFF) ^ data[i]];
		}
	}
	
	Result ^= 0xFFFFFFFF;
	return Result;
}

