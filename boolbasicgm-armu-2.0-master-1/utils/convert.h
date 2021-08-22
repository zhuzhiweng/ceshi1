/*
 * convert.h
 *
 *  Created on: 2016��7��25��
 *      Author: houxd
 */

#ifndef __CONVERT_H__
#define	__CONVERT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/**
	'\x19'	 <==>  19u
*/
extern int bcd2int(unsigned char bcd);
extern unsigned char int2bcd(int d);

/**
 * -9 -> '0'
 * 1 -> '1'
 * 10 -> 'a'
 * 15 -> 'f'
 * 100 -> 'f'
 */
extern char int2asc(int n);
/**
 * -9 -> '0'
 * 1 -> '1'
 * 10 -> 'A'
 * 15 -> 'F'
 * 100 -> 'F'
 */
extern char int2ASC(int n);
/**
 * '0' -> 0
 * 'B' -> 11
 * 'c' -> 12
 */
extern int asc2int(char asc);

extern uint8_t* hx_bin2asc(const void *src,int srclen,void *buf);
extern uint8_t* hx_bin2ASC(const void *src,int srclen,void *buf);
/**
 * like hx_bin2asc ,but add a '\0' end of converation
 */
extern char* hx_bin2ascstr(const void *src,int srclen,void *buf);
extern char* hx_bin2ASCstr(const void *src,int srclen,void *buf);
/**
	"\xAA\x55"  <==>  "AA55"
*/
extern char* hx_asc2bin(const void *hexcode, int len, void *bin);

/**
 * trim string. “t12t3t” trim 't' => "12t3"
 * hx_strtrim is trim "\t\r\n "
 */
extern char *hx_strtrim(char *s);
extern char *hx_strtrim2(char *p, const char *trim_list);
#define strtrim 	hx_strtrim2
/**
 * uint8_t ARRAY <==>  uint16_t(2Bytes) uint32_t(4Bytes) QWORD(8Bytes)
 * MSB = Big Endian,
 * LSB = Littie Endian
 */
extern uint16_t HX_LSB_B2W(const void *data);
extern uint16_t HX_MSB_B2W(const void *data);
extern uint32_t HX_MSB_B2DW(const void *data);
extern uint32_t HX_LSB_B2DW(const void *data);
extern uint32_t HX_MSB_B2DW24(const void *data);
extern uint32_t HX_LSB_B2DW24(const void *data);
extern uint64_t HX_MSB_B2QW(const void *data);
extern uint64_t HX_LSB_B2QW(const void *data);
extern uint8_t *HX_LSB_W2B(uint16_t v,void *p);
extern uint8_t *HX_MSB_W2B(uint16_t v,void *p);
extern uint8_t *HX_MSB_DW2B(uint32_t v,void *p);
extern uint8_t *HX_LSB_DW2B(uint32_t v,void *p);
extern uint8_t * HX_MSB_DW24_2B(uint32_t v,void *p);
extern uint8_t * HX_LSB_DW24_2B(uint32_t v,void *p);
extern uint8_t *HX_MSB_QW2B(uint64_t v,void *p);
extern uint8_t *HX_LSB_QW2B(uint64_t v,void *p);
extern void HX_W_REV(uint16_t *v);
extern void HX_DW_REV(uint32_t *v);
extern void HX_QW_REV(uint64_t *v);
extern uint64_t HX_W_REV2(uint16_t v);
extern uint64_t HX_DW_REV2(uint32_t v);
extern uint64_t HX_QW_REV2(uint64_t v);

/**
 * bcc8, sum8 ,crc16 checks.
 */
extern uint8_t make_bcc(const void *data, int len);
extern uint8_t make_bcc2(uint8_t init, const void *data, int len);
extern uint8_t make_sum(const void *buf, int len);
extern uint8_t make_sum2(uint8_t init,const void *buf, int len);
extern uint16_t calc_crc(const uint8_t *data, int length);
extern uint16_t calc_crc2(uint16_t init, const void *data, int length);

#ifdef __cplusplus
}
#endif


#endif
