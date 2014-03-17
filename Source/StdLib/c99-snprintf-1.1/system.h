/* $Id: system.h,v 1.1.1.1 2008/01/06 03:24:00 holger Exp $ */

#ifndef SYSTEM_H
#define SYSTEM_H

#include <stdarg.h>

extern "C" 
{
	int rpl_vsnprintf(char *, size_t, const char *, va_list);
	int rpl_snprintf(char *, size_t, const char *, ...);
	int rpl_vasprintf(char **, const char *, va_list);
	int rpl_asprintf(char **, const char *, ...);
}

#define vsnprintf rpl_vsnprintf
#define snprintf rpl_snprintf
#define vasprintf rpl_vasprintf
#define asprintf rpl_asprintf

#endif	/* SYSTEM_H */