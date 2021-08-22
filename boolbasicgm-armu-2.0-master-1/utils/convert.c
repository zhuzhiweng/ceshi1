/*
 * convert.c
 *
 *  Created on: 2016��7��25��
 *      Author: houxd
 */
#include "convert.h"

#include "stdio.h"
#include "string.h"

int bcd2int(unsigned char bcd)
{
	unsigned int tmp = bcd;
	return (0x0Fu&(tmp>>4))*10+(0x0Fu&(tmp));
}

unsigned char int2bcd(int d)
{
	return (unsigned char)(0x0F0u&((d/10)<<4))+(0x0Fu&(d%10));
}
/*
 * -9 -> '0'
 * 1 -> '1'
 * 10 -> 'a'
 * 15 -> 'f'
 * 100 -> 'f'
 */
char int2asc(int n)
{
	if(n<0)
		return '0';
	else if(n>=0 && n<10)
		return n+'0';
	else if(n>=10 && n<16)
		return (n-10)+'a';
	else
		return 'f';
}
/*
 * -9 -> '0'
 * 1 -> '1'
 * 10 -> 'A'
 * 15 -> 'F'
 * 100 -> 'F'
 */
char int2ASC(int n)
{
	if(n<0)
		return '0';
	else if(n>=0 && n<10)
		return n+'0';
	else if(n>=10 && n<16)
		return (n-10)+'A';
	else
		return 'F';
}
int asc2int(char asc)
{
	if(asc<'0')
		return 0;
	else if(asc>='0' && asc<='9')
		return asc-'0';
	else if(asc>='A' && asc<='F')
		return asc-'A'+10;
	else if(asc>='a' && asc<='f')
		return asc-'a'+10;
	else
		return 15;
}

uint8_t* hx_bin2asc(const void *src,int srclen,void *buf)
{
	uint8_t *p = (uint8_t*)src;
	uint8_t *q = (uint8_t*)buf;
	for(int i=0;i<srclen;i++){
		uint8_t c = *p++;
		*q++ = int2asc((c>>4)&0x0Fu);
		*q++ = int2asc((c>>0)&0x0Fu);
	}
	return (uint8_t*)buf;
}
uint8_t* hx_bin2ASC(const void *src,int srclen,void *buf)
{
	uint8_t *p = (uint8_t*)src;
	uint8_t *q = (uint8_t*)buf;
	for(int i=0;i<srclen;i++){
		uint8_t c = *p++;
		*q++ = int2ASC((c>>4)&0x0Fu);
		*q++ = int2ASC((c>>0)&0x0Fu);
	}
	return (uint8_t*)buf;
}
char* hx_bin2ascstr(const void *src,int srclen,void *buf)
{
	char *res = (char*)hx_bin2asc(src,srclen,buf);
	res[srclen*2] = '\0';
	return res;
}
char* hx_bin2ASCstr(const void *src,int srclen,void *buf)
{
	char *res = (char*)hx_bin2ASC(src,srclen,buf);
	res[srclen*2] = '\0';
	return res;
}
// "1234" => "\12\x34"
char* hx_asc2bin(const void *hexcode, int len, void *bin)
{
    unsigned int tmp;
    int c,i;
    unsigned char *t = (unsigned char*)bin;
	const unsigned char *s = (const unsigned char *)hexcode;
	for(i=0;i<len;i+=2){
		tmp = s[i];
        c = 0x0F & asc2int(tmp);
        c<<=4;
        tmp = s[i+1];
        c += 0x0F & asc2int(tmp);
        *t++ = c;
	}
    return bin;
}

char *hx_strtrim2(char *p, const char *trim_list)
{
	char *s_beg = p;
	char *s_end = s_beg + strlen(s_beg) - 1;
	while(strchr(trim_list,*s_end) && s_end > s_beg)
		*s_end-- = 0;
	while(strchr(trim_list,*s_beg) && s_beg < s_end)
		*s_beg++ = 0;
	//*p = s_beg;
	return s_beg;
}
char *hx_strtrim(char *s)
{
	return hx_strtrim2(s,"\t\r ");
}


//==========================================================
// uint8_t ARRAY <==>  uint16_t(2Bytes) uint32_t(4Bytes) QWORD(8Bytes)
// MSB = Big Endian, LSB = Littie Endian

uint16_t HX_LSB_B2W(const void *data)
{
	const uint8_t *d = data;
    uint16_t res;
    res = (uint16_t)(d[1]);
    res <<= 8;
    res += (uint16_t)(d[0]);
    return res;
}
uint16_t HX_MSB_B2W(const void *data)
{
	const uint8_t *d = data;
    uint16_t res;
    res = (uint16_t)(d[0]);
    res <<= 8;
    res += (uint16_t)(d[1]);
    return res;
}

uint32_t HX_MSB_B2DW(const void *data)
{
	const uint8_t *d = data;
    uint32_t res;
    res = (uint32_t)d[0];
    res <<=8;
    res += (uint32_t) d[1];
    res <<=8;
    res += (uint32_t) d[2];
    res <<=8;
    res += (uint32_t) d[3];
    return res;
}
uint32_t HX_LSB_B2DW(const void *data)
{
	const uint8_t *d = data;
    uint32_t res;
    res = (uint32_t)d[3];
    res <<=8;
    res += (uint32_t) d[2];
    res <<=8;
    res += (uint32_t) d[1];
    res <<=8;
    res += (uint32_t) d[0];
    return res;
}

uint32_t HX_MSB_B2DW24(const void *data)
{
	const uint8_t *d = data;
    uint32_t res;
    res = (uint32_t)d[0];
    res <<=8;
    res += (uint32_t) d[1];
    res <<=8;
    res += (uint32_t) d[2];
    return res;
}
uint32_t HX_LSB_B2DW24(const void *data)
{
	const uint8_t *d = data;
    uint32_t res;
    res = (uint32_t)d[2];
    res <<=8;
    res += (uint32_t) d[1];
    res <<=8;
    res += (uint32_t) d[0];
    return res;
}

uint64_t HX_MSB_B2QW(const void *data)
{
    const uint8_t *d = data;
    uint64_t res;
	res = (uint64_t)d[0];
    res <<=8;
    res += (uint64_t) d[1];
    res <<=8;
    res += (uint64_t) d[2];
    res <<=8;
    res += (uint64_t) d[3];
	res <<=8;
    res += (uint64_t) d[4];
    res <<=8;
    res += (uint64_t) d[5];
    res <<=8;
    res += (uint64_t) d[6];
    res <<=8;
    res += (uint64_t) d[7];
    return res;
}
uint64_t HX_LSB_B2QW(const void *data)
{
	const uint8_t *d = data;
    uint64_t res;
	res = (uint64_t)d[7];
    res <<=8;
    res += (uint64_t) d[6];
    res <<=8;
    res += (uint64_t) d[5];
    res <<=8;
    res += (uint64_t) d[4];
	res <<=8;
    res += (uint64_t) d[3];
    res <<=8;
    res += (uint64_t) d[2];
    res <<=8;
    res += (uint64_t) d[1];
    res <<=8;
    res += (uint64_t) d[0];
    return res;
}

uint8_t * HX_LSB_W2B(uint16_t v,void *p)
{
	uint8_t *_p = p;
    _p[0] =  0xFF & (v>>0);
    _p[1] =  0xFF & (v>>8);
	return _p;
}
uint8_t * HX_MSB_W2B(uint16_t v,void *p)
{
    uint8_t *_p = p;
    _p[0] =  0xFF & (v>>8);
    _p[1] =  0xFF & (v>>0);
	return _p;
}
uint8_t * HX_MSB_DW2B(uint32_t v,void *p)
{
    uint8_t *_p = p;
    _p[0] =  0xFF & (v>>24);
    _p[1] =  0xFF & (v>>16);
    _p[2] =  0xFF & (v>>8);
    _p[3] =  0xFF & (v>>0);
	return _p;
}
uint8_t * HX_MSB_DW24_2B(uint32_t v,void *p)
{
    uint8_t *_p = p;
    _p[0] =  0xFF & (v>>16);
    _p[1] =  0xFF & (v>>8);
    _p[2] =  0xFF & (v>>0);
	return _p;
}
uint8_t * HX_MSB_QW2B(uint64_t v,void *p)
{
	uint8_t *_p = p;
    _p[0] =  0xFF & (v>>56);
    _p[1] =  0xFF & (v>>48);
    _p[2] =  0xFF & (v>>40);
    _p[3] =  0xFF & (v>>32);
    _p[4] =  0xFF & (v>>24);
    _p[5] =  0xFF & (v>>16);
    _p[6] =  0xFF & (v>>8);
    _p[7] =  0xFF & (v>>0);
	return _p;
}
uint8_t * HX_LSB_DW2B(uint32_t v,void *p)
{
    uint8_t *_p = p;
    _p[3] =  0xFF & (v>>24);
    _p[2] =  0xFF & (v>>16);
    _p[1] =  0xFF & (v>>8);
    _p[0] =  0xFF & (v>>0);
	return _p;
}
uint8_t * HX_LSB_DW24_2B(uint32_t v,void *p)
{
    uint8_t *_p = p;
    _p[2] =  0xFF & (v>>16);
    _p[1] =  0xFF & (v>>8);
    _p[0] =  0xFF & (v>>0);
	return _p;
}
uint8_t *HX_LSB_QW2B(uint64_t v,void *p)
{
	uint8_t *_p = p;
    _p[7] =  0xFF & (v>>56);
    _p[6] =  0xFF & (v>>48);
    _p[5] =  0xFF & (v>>40);
    _p[4] =  0xFF & (v>>32);
    _p[3] =  0xFF & (v>>24);
    _p[2] =  0xFF & (v>>16);
    _p[1] =  0xFF & (v>>8);
    _p[0] =  0xFF & (v>>0);
	return _p;
}
void HX_W_REV( uint16_t *v)
{
	uint8_t buf[2];
	*v = HX_LSB_B2W(HX_MSB_W2B(*v,buf));
}
void HX_DW_REV( uint32_t *v)
{
	uint8_t buf[4];
	*v = HX_LSB_B2DW(HX_MSB_DW2B(*v,buf));
}
void HX_QW_REV( uint64_t *v)
{
	uint8_t buf[8];
	*v = HX_LSB_B2QW(HX_MSB_QW2B(*v,buf));
}
uint64_t HX_W_REV2(uint16_t v)
{
	uint8_t buf[2];
	return HX_LSB_B2W(HX_MSB_W2B(v,buf));
}
uint64_t HX_DW_REV2(uint32_t v)
{
	uint8_t buf[4];
	return HX_LSB_B2DW(HX_MSB_DW2B(v,buf));
}
uint64_t HX_QW_REV2(uint64_t v)
{
	uint8_t buf[8];
	return HX_LSB_B2QW(HX_MSB_QW2B(v,buf));
}
uint8_t make_bcc2(uint8_t init, const void *data, int len)
{
	uint8_t res = init;
	const uint8_t *p = (const uint8_t*)data;
	for(int i=0;i<len;i++){
		res ^= p[i];
	}
	return res;
}
uint8_t make_bcc(const void *data, int len)
{
	return make_bcc2(0,data,len);
}
uint8_t make_sum2(uint8_t init,const void *buf, int len)
{
	const uint8_t *p = buf;
	uint8_t res = init;
	for(int i=0;i<len;i++){
		res += p[i];
	}
	return res;
}
uint8_t make_sum(const void *buf, int len)
{
	return make_sum2(0,buf,len);
}

static const uint16_t fcstab[] = {
	0x0000, 0x1189, 0x2312, 0x329b, 0x4624, 0x57ad, 0x6536, 0x74bf,
	0x8c48, 0x9dc1, 0xaf5a, 0xbed3, 0xca6c, 0xdbe5, 0xe97e, 0xf8f7,
	0x1081, 0x0108, 0x3393, 0x221a, 0x56a5, 0x472c, 0x75b7, 0x643e,
	0x9cc9, 0x8d40, 0xbfdb, 0xae52, 0xdaed, 0xcb64, 0xf9ff, 0xe876,
	0x2102, 0x308b, 0x0210, 0x1399, 0x6726, 0x76af, 0x4434, 0x55bd,
	0xad4a, 0xbcc3, 0x8e58, 0x9fd1, 0xeb6e, 0xfae7, 0xc87c, 0xd9f5,
	0x3183, 0x200a, 0x1291, 0x0318, 0x77a7, 0x662e, 0x54b5, 0x453c,
	0xbdcb, 0xac42, 0x9ed9, 0x8f50, 0xfbef, 0xea66, 0xd8fd, 0xc974,
	0x4204, 0x538d, 0x6116, 0x709f, 0x0420, 0x15a9, 0x2732, 0x36bb,
	0xce4c, 0xdfc5, 0xed5e, 0xfcd7, 0x8868, 0x99e1, 0xab7a, 0xbaf3,
	0x5285, 0x430c, 0x7197, 0x601e, 0x14a1, 0x0528, 0x37b3, 0x263a,
	0xdecd, 0xcf44, 0xfddf, 0xec56, 0x98e9, 0x8960, 0xbbfb, 0xaa72,
	0x6306, 0x728f, 0x4014, 0x519d, 0x2522, 0x34ab, 0x0630, 0x17b9,
	0xef4e, 0xfec7, 0xcc5c, 0xddd5, 0xa96a, 0xb8e3, 0x8a78, 0x9bf1,
	0x7387, 0x620e, 0x5095, 0x411c, 0x35a3, 0x242a, 0x16b1, 0x0738,
	0xffcf, 0xee46, 0xdcdd, 0xcd54, 0xb9eb, 0xa862, 0x9af9, 0x8b70,
	0x8408, 0x9581, 0xa71a, 0xb693, 0xc22c, 0xd3a5, 0xe13e, 0xf0b7,
	0x0840, 0x19c9, 0x2b52, 0x3adb, 0x4e64, 0x5fed, 0x6d76, 0x7cff,
	0x9489, 0x8500, 0xb79b, 0xa612, 0xd2ad, 0xc324, 0xf1bf, 0xe036,
	0x18c1, 0x0948, 0x3bd3, 0x2a5a, 0x5ee5, 0x4f6c, 0x7df7, 0x6c7e,
	0xa50a, 0xb483, 0x8618, 0x9791, 0xe32e, 0xf2a7, 0xc03c, 0xd1b5,
	0x2942, 0x38cb, 0x0a50, 0x1bd9, 0x6f66, 0x7eef, 0x4c74, 0x5dfd,
	0xb58b, 0xa402, 0x9699, 0x8710, 0xf3af, 0xe226, 0xd0bd, 0xc134,
	0x39c3, 0x284a, 0x1ad1, 0x0b58, 0x7fe7, 0x6e6e, 0x5cf5, 0x4d7c,
	0xc60c, 0xd785, 0xe51e, 0xf497, 0x8028, 0x91a1, 0xa33a, 0xb2b3,
	0x4a44, 0x5bcd, 0x6956, 0x78df, 0x0c60, 0x1de9, 0x2f72, 0x3efb,
	0xd68d, 0xc704, 0xf59f, 0xe416, 0x90a9, 0x8120, 0xb3bb, 0xa232,
	0x5ac5, 0x4b4c, 0x79d7, 0x685e, 0x1ce1, 0x0d68, 0x3ff3, 0x2e7a,
	0xe70e, 0xf687, 0xc41c, 0xd595, 0xa12a, 0xb0a3, 0x8238, 0x93b1,
	0x6b46, 0x7acf, 0x4854, 0x59dd, 0x2d62, 0x3ceb, 0x0e70, 0x1ff9,
	0xf78f, 0xe606, 0xd49d, 0xc514, 0xb1ab, 0xa022, 0x92b9, 0x8330,
	0x7bc7, 0x6a4e, 0x58d5, 0x495c, 0x3de3, 0x2c6a, 0x1ef1, 0x0f78
};
uint16_t calc_crc(const unsigned char *data, int length)
{
	uint16_t fcs = 0xFFFF;

	int end = length;

	for (int i = 0; i < end; i++)
	{
		fcs = (uint16_t)(((uint16_t)(fcs >> 8)) ^ fcstab[(fcs ^ data[i]) & 0xFF]);
	}

	return (uint16_t)(~fcs);
}
uint16_t calc_crc2(uint16_t init, const void *_data, int length)
{
	uint16_t fcs = init;
	const uint8_t *data = _data;

	int end = length;

	for (int i = 0; i < end; i++)
	{
		fcs = (uint16_t)(((uint16_t)(fcs >> 8)) ^ fcstab[(fcs ^ data[i]) & 0xFF]);
	}

	return (uint16_t)(fcs);
}
