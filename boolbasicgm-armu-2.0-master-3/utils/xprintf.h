/*------------------------------------------------------------------------*/
/* Universal string handler for user console interface  (C)ChaN, 2012     */
/*------------------------------------------------------------------------*/

#ifndef _STRFUNC
#define _STRFUNC
#include <stdarg.h>

#define	_CR_CRLF		1	/* 1: Convert \n ==> \r\n in the output char */
#define	_USE_LONGLONG	1	/* 1: Enable long long integer in type "ll". */
#define	_LONGLONG_t		long long	/* Platform dependent long long integer type */


#define _PRINTF_RTOS_SAFE	1	/* 1: Mutex Wrapped xfprintf xfprintf1 xfputs xfgets*/

extern void xprintf_io_register(void(*out)(unsigned char),unsigned char(*in)(void));

extern void xputc (char c);
extern void xputs (const char* str);
extern void xfputs (void (*func)(unsigned char), const char* str);
extern void xvprintf(const char* fmt, va_list arp);
extern void xprintf (const char* fmt, ...);
extern void xsprintf (char* buff, const char* fmt, ...);
extern void xfprintf (void (*func)(unsigned char), const char*	fmt, ...);
extern void put_dump (const void* buff, unsigned long addr, int len, int width);
#define DW_CHAR		sizeof(char)
#define DW_SHORT	sizeof(short)
#define DW_LONG		sizeof(long)

extern int xgets (char* buff, int len);
extern int xgets_echo (char* buff, int len);
extern int xfgets (unsigned char (*func)(void), char* buff, int len);
extern int xatoi (char** str, long* res);



#endif
