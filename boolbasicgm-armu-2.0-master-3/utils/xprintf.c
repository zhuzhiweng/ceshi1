/*------------------------------------------------------------------------/
/  Universal String Handler for Console Input and Output
/-------------------------------------------------------------------------/
/
/ Copyright (C) 2012, ChaN, all right reserved.
/
/ xprintf module is an open source software. Redistribution and use of
/ xprintf module in source and binary forms, with or without modification,
/ are permitted provided that the following condition is met:
/
/ 1. Redistributions of source code must retain the above copyright notice,
/    this condition and the following disclaimer.
/
/ This software is provided by the copyright holder and contributors "AS IS"
/ and any warranties related to this software are DISCLAIMED.
/ The copyright owner or contributors be NOT LIABLE for any damages caused
/ by use of this software.
/
/-------------------------------------------------------------------------*/

#include "xprintf.h"
#include "ctype.h"

#if (_PRINTF_RTOS_SAFE == 1)
#include "rtos_wrapper.h"
static struct RtosMutex_t *mtx = 0;
static void _X_MUTEX_LOCK()
{
	if(mtx==0)
		RTOS_ASSERT(mtx=rtos_mutex_create());
	rtos_mutex_lock(mtx);
}
static void _X_MUTEX_UNLOCK()
{
	if(mtx==0)
		RTOS_ASSERT(mtx=rtos_mutex_create());
	rtos_mutex_unlock(mtx);
}
#else
#define _X_MUTEX_LOCK()
#define _X_MUTEX_UNLOCK()
#endif


void (*xfunc_out)(unsigned char);	/* Pointer to the output stream */
static char *outptr;

/*----------------------------------------------*/
/* Put a character                              */
/*----------------------------------------------*/

void _xputc (char c)
{
	if (_CR_CRLF && c == '\n') _xputc('\r');		/* CR -> CRLF */

	if (outptr) {		/* Destination is memory */
		*outptr++ = (unsigned char)c;
		return;
	}
	if (xfunc_out) {	/* Destination is device */
		xfunc_out((unsigned char)c);
	}
}
void xputc(char c)
{
	_X_MUTEX_LOCK();
	_xputc(c);
	_X_MUTEX_UNLOCK();
}



/*----------------------------------------------*/
/* Put a null-terminated string                 */
/*----------------------------------------------*/

void _xputs (					/* Put a string to the default device */
	const char* str				/* Pointer to the string */
)
{
	while (*str) {
		_xputc(*str++);
	}
}
void xputs(const char* str)
{
	_X_MUTEX_LOCK();
	_xputs(str);
	_X_MUTEX_UNLOCK();
}

void xfputs (					/* Put a string to the specified device */
	void(*func)(unsigned char),	/* Pointer to the output function */
	const char*	str				/* Pointer to the string */
)
{
	void (*pf)(unsigned char);
	_X_MUTEX_LOCK();

	pf = xfunc_out;		/* Save current output device */
	xfunc_out = func;	/* Switch output to specified device */
	while (*str) {		/* Put the string */
		_xputc(*str++);
	}
	xfunc_out = pf;		/* Restore output device */
	_X_MUTEX_UNLOCK();
}



/*----------------------------------------------*/
/* Formatted string output                      */
/*----------------------------------------------*/
/*  xprintf_io_register(_segger_jlink_rtt_putc,_segger_jlink_rtt_getc);
	xprintf(" |%d%p",4,"\x12\x34\xaa\xff");					//"1234aaff"
	xprintf(" |%d%*",11,"\x00\x55\x66\x44Google\x00");		//"\x00UfDGoogle\x00"
	xprintf(" |%d", 1234);									//"1234"
	xprintf(" |%6d,%3d%%", -200, 5);	                    //"  -200,  5%"
	xprintf(" |%-6u", 100);				                    //"100   "
	xprintf(" |%ld", 12345678);			                    //"12345678"
	xprintf(" |%llu", 0x100000000);		                    //"4294967296"	<_USE_LONGLONG>
	xprintf(" |%04x", 0xA3);			                    //"00a3"
	xprintf(" |%08lX", 0x123ABC);		                    //"00123ABC"
	xprintf(" |%016b", 0x550F);			                    //"0101010100001111"
	xprintf(" |%s", "String");			                    //"String"
	xprintf(" |%-5s", "abc");			                    //"abc  "
	xprintf(" |%5s", "abc");			                    //"  abc"
	xprintf(" |%c", 'a');				                    //"a"
	xprintf(" |%f", 10.0);            	                    //<xprintf lacks floating point support. Use regular printf.>
*/

void _xvprintf (
	const char*	fmt,	/* Pointer to the format string */
	va_list arp			/* Pointer to arguments */
)
{
	unsigned int r, i, j, w, f;
	char s[24], c, d, *p;
#if _USE_LONGLONG
	_LONGLONG_t v;
	unsigned _LONGLONG_t vs;
#else
	long v;
	unsigned long vs;
#endif


	for (;;) {
		c = *fmt++;					/* Get a format character */
		if (!c) break;				/* End of format? */
		if (c != '%') {				/* Pass it through if not a % sequense */
			_xputc(c); continue;
		}
		f = 0;						/* Clear flags */
		c = *fmt++;					/* Get first char of the sequense */
		if (c == '0') {				/* Flag: left '0' padded */
			f = 1; c = *fmt++;
		} else {
			if (c == '-') {			/* Flag: left justified */
				f = 2; c = *fmt++;
			}
		}
		for (w = 0; c >= '0' && c <= '9'; c = *fmt++) {	/* Minimum width */
			w = w * 10 + c - '0';
		}
		if (c == 'l' || c == 'L') {	/* Prefix: Size is long */
			f |= 4; c = *fmt++;
#if _USE_LONGLONG
			if (c == 'l' || c == 'L') {	/* Prefix: Size is long long */
				f |= 8; c = *fmt++;
			}
#endif
		}
		if (!c) break;				/* End of format? */
		d = c;
		if (d >= 'a') d -= 0x20;
		switch (d) {				/* Type is... */
		case 'S' :					/* String */
			p = va_arg(arp, char*);
			for (j = 0; p[j]; j++) ;
			while (!(f & 2) && j++ < w) _xputc(' ');
			_xputs(p);
			while (j++ < w) _xputc(' ');
			continue;
		case 'C' :					/* Character */
			_xputc((char)va_arg(arp, int)); continue;
		case 'B' :					/* Binary */
			r = 2; break;
		case 'O' :					/* Octal */
			r = 8; break;
		case 'D' :					/* Signed decimal */
			if(*fmt=='%'){
				f =*(fmt+1);
				if(f=='p'||f=='P'){
					f-='p'-'a';
					i=va_arg(arp, int);
					p = va_arg(arp, char*);
					while(i-->0){
						j = *p++;
						c = (j>>4)&0x0Fu;
						_xputc(c<10?c+'0':(c-10)+f);
						c = j&0xFu;
						_xputc(c<10?c+'0':(c-10)+f);
					}
					fmt+=2;
					continue;
				}else if(f=='*'){
					i=va_arg(arp, int);
					p = va_arg(arp, char*);
					while(i-->0){
						j = *p++;
						if(isprint(j))
							_xputc(j);
						else{
							_xputc('\\');
							_xputc('x');
							c = (j>>4)&0x0Fu;
							_xputc(c<10?c+'0':(c-10)+f);
							c = j&0xFu;
							_xputc(c<10?c+'0':(c-10)+f);
						}
					}
					fmt+=2;
					continue;
				}
			}
		case 'U' :					/* Unsigned decimal */
			r = 10; break;
		case 'X' :					/* Hexdecimal */
			r = 16; break;
		default:					/* Unknown type (passthrough) */
			_xputc(c); continue;
		}

		/* Get an argument and put it in numeral */
#if _USE_LONGLONG
		if (f & 8) {	/* long long argument? */
			v = va_arg(arp, _LONGLONG_t);
		} else {
			if (f & 4) {	/* long argument? */
				v = (d == 'D') ? (long)va_arg(arp, long) : (long)va_arg(arp, unsigned long);
			} else {		/* int/short/char argument */
				v = (d == 'D') ? (long)va_arg(arp, int) : (long)va_arg(arp, unsigned int);
			}
		}
#else
		if (f & 4) {	/* long argument? */
			v = va_arg(arp, long);
		} else {		/* int/short/char argument */
			v = (d == 'D') ? (long)va_arg(arp, int) : (long)va_arg(arp, unsigned int);
		}
#endif
		if (d == 'D' && v < 0) {	/* Negative value? */
			v = 0 - v; f |= 16;
		}
		i = 0; vs = v;
		do {
			d = (char)(vs % r); vs /= r;
			if (d > 9) d += (c == 'x') ? 0x27 : 0x07;
			s[i++] = d + '0';
		} while (vs != 0 && i < sizeof s);
		if (f & 16) s[i++] = '-';
		j = i; d = (f & 1) ? '0' : ' ';
		while (!(f & 2) && j++ < w) _xputc(d);
		do _xputc(s[--i]); while (i != 0);
		while (j++ < w) _xputc(' ');
	}
}

void xvprintf(const char* fmt, va_list arp)
{
	_X_MUTEX_LOCK();
	_xvprintf(fmt,arp);
	_X_MUTEX_UNLOCK();
}
void xprintf (			/* Put a formatted string to the default device */
	const char*	fmt,	/* Pointer to the format string */
	...					/* Optional arguments */
)
{
	va_list arp;


	va_start(arp, fmt);
	_xvprintf(fmt, arp);
	va_end(arp);
}


void xsprintf (			/* Put a formatted string to the memory */
	char* buff,			/* Pointer to the output buffer */
	const char*	fmt,	/* Pointer to the format string */
	...					/* Optional arguments */
)
{
	va_list arp;


	outptr = buff;		/* Switch destination for memory */

	va_start(arp, fmt);
	_xvprintf(fmt, arp);
	va_end(arp);

	*outptr = 0;		/* Terminate output string with a \0 */
	outptr = 0;			/* Switch destination for device */
}


void xfprintf (					/* Put a formatted string to the specified device */
	void(*func)(unsigned char),	/* Pointer to the output function */
	const char*	fmt,			/* Pointer to the format string */
	...							/* Optional arguments */
)
{
	va_list arp;
	void (*pf)(unsigned char);
	_X_MUTEX_LOCK();

	pf = xfunc_out;		/* Save current output device */
	xfunc_out = func;	/* Switch output to specified device */

	va_start(arp, fmt);
	_xvprintf(fmt, arp);
	va_end(arp);

	xfunc_out = pf;		/* Restore output device */
	_X_MUTEX_UNLOCK();
}



/*----------------------------------------------*/
/* Dump a line of binary dump                   */
/*----------------------------------------------*/

void put_dump (
	const void* buff,		/* Pointer to the array to be dumped */
	unsigned long addr,		/* Heading address value */
	int len,				/* Number of items to be dumped */
	int width				/* Size of the items (DF_CHAR, DF_SHORT, DF_LONG) */
)
{
	int i;
	const unsigned char *bp;
	const unsigned short *sp;
	const unsigned long *lp;


	xprintf("%08lX ", addr);		/* address */

	switch (width) {
	case DW_CHAR:
		bp = buff;
		for (i = 0; i < len; i++)		/* Hexdecimal dump */
			xprintf(" %02X", bp[i]);
		_xputc(' ');
		for (i = 0; i < len; i++)		/* ASCII dump */
			_xputc((unsigned char)((bp[i] >= ' ' && bp[i] <= '~') ? bp[i] : '.'));
		break;
	case DW_SHORT:
		sp = buff;
		do								/* Hexdecimal dump */
			xprintf(" %04X", *sp++);
		while (--len);
		break;
	case DW_LONG:
		lp = buff;
		do								/* Hexdecimal dump */
			xprintf(" %08LX", *lp++);
		while (--len);
		break;
	}

	_xputc('\n');
}


unsigned char (*xfunc_in)(void);	/* Pointer to the input stream */

/*----------------------------------------------*/
/* Get a line from the input                    */
/*----------------------------------------------*/
static int _LINE_ECHO =  0;	/* 1: Echo back input chars in xgets function */
int _xgets (		/* 0:End of stream, 1:A line arrived */
	char* buff,	/* Pointer to the buffer */
	int len		/* Buffer length */
)
{
	int c, i;


	if (!xfunc_in) return 0;		/* No input function specified */

	i = 0;
	for (;;) {
		c = xfunc_in();				/* Get a char from the incoming stream */
		if (!c) return 0;			/* End of stream? */
		if (c == '\r') break;		/* End of line? */
		if (c == '\b' && i) {		/* Back space? */
			i--;
			if (_LINE_ECHO) _xputc((unsigned char)c);
			continue;
		}
		if (c >= ' ' && i < len - 1) {	/* Visible chars */
			buff[i++] = c;
			if (_LINE_ECHO) _xputc((unsigned char)c);
		}
	}
	buff[i] = 0;	/* Terminate with a \0 */
	if (_LINE_ECHO) _xputc('\n');
	return 1;
}

int xgets(char* buff, int len)
{
	int r;
	_X_MUTEX_LOCK();
	r = xgets(buff, len);
	_X_MUTEX_UNLOCK();
	return r;
}
int xgets_echo (char* buff, int len)
{
	_X_MUTEX_LOCK();
	_LINE_ECHO = 1;
	int r = _xgets(buff,len);
	_LINE_ECHO = 0;
	_X_MUTEX_UNLOCK();
	return r;
}
int _xfgets (	/* 0:End of stream, 1:A line arrived */
	unsigned char (*func)(void),	/* Pointer to the input stream function */
	char* buff,	/* Pointer to the buffer */
	int len		/* Buffer length */
)
{
	unsigned char (*pf)(void);
	int n;
	_X_MUTEX_LOCK();


	pf = xfunc_in;			/* Save current input device */
	xfunc_in = func;		/* Switch input to specified device */
	n = xgets(buff, len);	/* Get a line */
	xfunc_in = pf;			/* Restore input device */

	_X_MUTEX_UNLOCK();
	return n;
}




/*----------------------------------------------*/
/* Get a value of the string                    */
/*----------------------------------------------*/
/*	"123 -5   0x3ff 0b1111 0377  w "
	    ^                           1st call returns 123 and next ptr
	       ^                        2nd call returns -5 and next ptr
                   ^                3rd call returns 1023 and next ptr
                          ^         4th call returns 15 and next ptr
                               ^    5th call returns 255 and next ptr
                                  ^ 6th call fails and returns 0
*/

int xatoi (			/* 0:Failed, 1:Successful */
	char **str,		/* Pointer to pointer to the string */
	long *res		/* Pointer to the valiable to store the value */
)
{
	unsigned long val;
	unsigned char c, r, s = 0;


	*res = 0;

	while ((c = **str) == ' ') (*str)++;	/* Skip leading spaces */

	if (c == '-') {		/* negative? */
		s = 1;
		c = *(++(*str));
	}

	if (c == '0') {
		c = *(++(*str));
		switch (c) {
		case 'x':		/* hexdecimal */
			r = 16; c = *(++(*str));
			break;
		case 'b':		/* binary */
			r = 2; c = *(++(*str));
			break;
		default:
			if (c <= ' ') return 1;	/* single zero */
			if (c < '0' || c > '9') return 0;	/* invalid char */
			r = 8;		/* octal */
		}
	} else {
		if (c < '0' || c > '9') return 0;	/* EOL or invalid char */
		r = 10;			/* decimal */
	}

	val = 0;
	while (c > ' ') {
		if (c >= 'a') c -= 0x20;
		c -= '0';
		if (c >= 17) {
			c -= 7;
			if (c <= 9) return 0;	/* invalid char */
		}
		if (c >= r) return 0;		/* invalid char for current radix */
		val = val * r + c;
		c = *(++(*str));
	}
	if (s) val = 0 - val;			/* apply sign if needed */

	*res = val;
	return 1;
}

void xprintf_io_register(void(*out)(unsigned char),unsigned char(*in)(void))
{
	xfunc_out = out;
	xfunc_in = in;
}

