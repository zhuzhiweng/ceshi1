/*
 * timelib.c
 *
 *  Created on: 2017年10月19日
 *      Author: houxd
 */
#include <timelib.h>
#include <stdio.h>
time_t mktime_z(struct tm * pt, long timezone)
{
	unsigned int year = pt->tm_year + 1900;
	unsigned int mon = pt->tm_mon+1;
	unsigned int day = pt->tm_mday;
	unsigned int hour = pt->tm_hour;
	unsigned int min = pt->tm_min;
	unsigned int sec = pt->tm_sec;
	if (0 >= (int) (mon -= 2)) { /* 1..12 -> 11,12,1..10 */
		mon += 12; /* Puts Feb last since it has leap day */
		year -= 1;
	}
	time_t res = ((((unsigned long) (year / 4 - year / 100 + year / 400
			+ 367 * mon / 12 + day) + year * 365 - 719499) * 24 + hour /* now have hours */
	) * 60 + min /* now have minutes */
	) * 60 + sec; /* finally seconds */

	return res - timezone;
}
time_t mktime_s(struct tm * pt)
{
	return mktime_z(pt,__timezone(NULL));
}
time_t mktime_from_yymdHMS(uint32_t yy, uint32_t m, uint32_t d,
		 uint32_t H, uint32_t M, uint32_t S, long timezone)
{
	struct tm tm;
	tm.tm_year = yy - 1900;
	tm.tm_mon = m - 1;
	tm.tm_mday = d;
	tm.tm_hour = H;
	tm.tm_min = M;
	tm.tm_sec = S;
	return mktime_z(&tm,timezone);
}

static const char days_tbl[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
void localtime_z(const time_t *ptime, long timezone, struct tm *tm_time) {
	uint32_t pass_4year;
	uint32_t hpery;
	time_t raw_time = *ptime;
	raw_time = raw_time + timezone;
	if (raw_time < 0) {
		raw_time = 0;
	}
	tm_time->tm_sec = (int) (raw_time % 60);
	raw_time /= 60;
	tm_time->tm_min = (int) (raw_time % 60);
	raw_time /= 60;
	tm_time->tm_wday = ((raw_time/24)+4)%7;
	pass_4year = ((unsigned int) raw_time / (1461L * 24L));
	tm_time->tm_year = (pass_4year << 2) + 70;
	raw_time %= 1461L * 24L;
	for (;;) {
		hpery = 365 * 24;
		if ((tm_time->tm_year & 3) == 0) {
			hpery += 24;
		}
		if (raw_time < hpery) {
			break;
		}
		tm_time->tm_year++;
		raw_time -= hpery;
	}
	tm_time->tm_hour = (int) (raw_time % 24);
	raw_time /= 24;
	raw_time++;
	if ((tm_time->tm_year & 3) == 0) {
		if (raw_time > 60) {
			raw_time--;
		} else {
			if (raw_time == 60) {
				tm_time->tm_mon = 1;
				tm_time->tm_mday = 29;
				return;
			}
		}
	}
	for (tm_time->tm_mon = 0; days_tbl[tm_time->tm_mon] < raw_time; tm_time->tm_mon++) {
		raw_time -= days_tbl[tm_time->tm_mon];
	}
	tm_time->tm_mday = (int) (raw_time);
	return;
}
long __timezone(long *ptz)
{
	static long tz = _TIMELIB_TZ_DEF;
	if(ptz)
		tz = *ptz;
	return tz;
}
struct tm *localtime_s(const time_t * pt, struct tm *ltm)
{
	time_t t = pt?*pt:time(NULL);
	localtime_z(&t,__timezone(NULL),ltm);
	return ltm;
}
struct tm *localtime(const time_t * pt)
{
	static struct tm ltm;
	return localtime_s(pt,&ltm);
}

#if _TIMELIB_ASC_USE_CN == 0
static const char format[] = "%.3s %.3s%3d %.2d:%.2d:%.2d %d\n";
static const char *const wday_name[] = {"Sun","Mon","Tue","Wed","Thu","Fri","Sat"};
static const char *const month_name[] = {"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
char *asctime_s(const struct tm * tp, char *buf, int buflen)
{
	int n = snprintf(buf, buflen, format,
			(tp->tm_wday < 0 || tp->tm_wday >= 7 ?
					"???" : wday_name[tp->tm_wday]),
			(tp->tm_mon < 0 || tp->tm_mon >= 12 ?
					"???" : month_name[tp->tm_mon]), tp->tm_mday,
			tp->tm_hour, tp->tm_min, tp->tm_sec, 1900 + tp->tm_year);
	if (n < 0)
		return NULL;
	return buf;
}
#else
#if __SUPPORT_CH_DISPLAY
static const char format[] = "%d年%d月%d日 %.2d:%.2d:%.2d 星期%s\n";
static const char *const cn_name[] = {"日","一","二","三","四","五","六"};
#else
static const char format[] = "%d/%d/%d %.2d:%.2d:%.2d Week%s\n";
static const char *const cn_name[] = {"0","1","2","3","4","5","6"};
#endif
char *asctime_s(const struct tm * tp, char *buf, int buflen)
{
	int n = snprintf(buf, buflen, format,
			1900 + tp->tm_year,tp->tm_mon+1,tp->tm_mday,
			tp->tm_hour, tp->tm_min, tp->tm_sec,
			(tp->tm_wday < 0 || tp->tm_wday >= 7 ?
					"?" : cn_name[tp->tm_wday])
			);
	if (n < 0)
		return NULL;
	return buf;
}
#endif

