#include <stdarg.h>
#include <string.h>
#include "Terminal.h"
#include "c99-snprintf-1.1\system.h"

// This defines how far a string function will go before bailing out
#define STR_MAX 1024


#pragma function(strlen)
extern "C" size_t strlen(const char *szString)
{
	size_t Length = 0;

	while(szString[Length] != 0 && Length < STR_MAX)
	{
		Length++;
	}

	return Length;
}

#pragma function(strcpy)
extern "C" char * strcpy ( char * destination, const char * source )
{
	size_t Length = 0;

	while(source[Length] != 0 && Length < STR_MAX)
	{
		destination[Length] = source[Length];
		Length++;
	}

	destination[Length] = 0;	

	return destination;
}

extern "C" int __cdecl printf(const char * format, ...)
{
    int retValue;
	char Buffer[STR_MAX];
    va_list argptr;
          
    va_start( argptr, format );
    retValue = vsnprintf( Buffer, STR_MAX, format, argptr );
    va_end( argptr );

	CurrentTerminal->Write(Buffer, retValue);

    return retValue;
}
