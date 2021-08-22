/*
 * timelib.h
 *
 *  Created on: 2017年10月19日
 *      Author: houxd
 */

#ifndef TIMELIB_H_
#define TIMELIB_H_
#include <stdint.h>
#include <time.h>
/**
 * struct tm {
 * int tm_sec; 	// [0,59]
 * int tm_min; 	// [0,59]
 * int tm_hour; // [0,23]
 * int tm_mday; // [1,31]
 * int tm_mon; 	// [0,11]
 * int tm_year; // since 1900
 *
 * }
 */
///default time zone: +8 china
#define _TIMELIB_TZ_DEF			(+8*60*60)
///for asctime output ctrl 0 is en, 1 is zh_cn
#define _TIMELIB_ASC_USE_CN	(1)
/**
 * set of get time zone value.
 * @param tz if not NULL, inner time zone value will be change.
 * @return return current inner time zone value.
 */
extern long __timezone(long *tz);
/**
 * convert tm struct to time_t, use local time zone.
 */
extern time_t mktime_s(struct tm * pt);
/**
 * convert tm struct to time_t, use special time zone.
 */
extern time_t mktime_z(struct tm * pt, long timezone);
/**
 * convert a yyyymmddHHMMSS format to time_t, use special time zone.
 */
extern time_t mktime_from_yymdHMS(uint32_t yy, uint32_t m, uint32_t d,
		 uint32_t H, uint32_t M, uint32_t S, long timezone);
/**
 * convert utc second time_t to tm structure. use local time zone.
 */
extern struct tm *localtime_s(const time_t * pt, struct tm *ltm);
/**
 * convert utc second time_t to tm structure. use special time zone.
 */
extern void localtime_z(const time_t *ptime, long timezone, struct tm *tm_time);
/**
 * dump tm to string. like asctime.
 */
extern char *asctime_s(const struct tm * tp, char *buf, int buflen);
#endif /* TIMELIB_H_ */
