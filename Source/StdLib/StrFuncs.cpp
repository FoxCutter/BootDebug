#include <stdarg.h>
#include <stdio.h>
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

extern "C" int __cdecl lib_printf(const char * format, ...)
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


_ACRTIMP int __cdecl __stdio_common_vfprintf(
	_In_                                    unsigned __int64 _Options,
	_Inout_                                 FILE*            _Stream,
	_In_z_ _Printf_format_string_params_(2) char const*      _Format,
	_In_opt_                                _locale_t        _Locale,
	va_list          _ArgList
)
{
	char Buffer[STR_MAX];
	int retValue;
	retValue = rpl_vsnprintf(Buffer, STR_MAX, _Format, _ArgList);

	CurrentTerminal->Write(Buffer, retValue);

	return retValue;
}


_ACRTIMP int __cdecl __stdio_common_vsprintf(
	_In_                                    unsigned __int64 _Options,
	_Out_writes_opt_z_(_BufferCount)        char*            _Buffer,
	_In_                                    size_t           _BufferCount,
	_In_z_ _Printf_format_string_params_(2) char const*      _Format,
	_In_opt_                                _locale_t        _Locale,
	va_list          _ArgList
)
{
	//puts("__stdio_common_vsprintf");
	return rpl_vsnprintf(_Buffer, _BufferCount, _Format, _ArgList);
}

_ACRTIMP_ALT FILE* __cdecl __acrt_iob_func(unsigned _Ix)
{
	return nullptr;
}

