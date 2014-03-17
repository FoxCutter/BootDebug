//==========================================
// LIBCTINY - Matt Pietrek 2001
// MSDN Magazine, January 2001
//==========================================
//#include <windows.h>
#include <stdio.h>
#include <string.h>

#include "Terminal.h"

extern "C" int __cdecl puts(const char * s)
{
	CurrentTerminal->Write(s, strlen(s));
	int length = CurrentTerminal->Write("\n", 1);
	
    return (int)(length ? length : EOF);
}

extern "C" char * __cdecl gets_s(char * _Buf, rsize_t _Size)
{
	CurrentTerminal->Read(_Buf, _Size);

	return _Buf;
}

