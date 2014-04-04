//==========================================
// LIBCTINY - Matt Pietrek 2001
// MSDN Magazine, January 2001
//==========================================
#include <windows.h>
#include <string.h>

extern "C" int __cdecl _strcmpi(const char *s1, const char *s2)
{
    return _stricmp( s1, s2 );
}

extern "C" int __cdecl _stricmp(const char *s1, const char *s2)
{
	return _strnicmp(s1, s2, 1024);
}

extern "C" int __cdecl _strnicmp(const char *s1, const char *s2, size_t _MaxCount)
{
	for(size_t x = 0; x < _MaxCount; x++)
	{
		if(s1[x] == 0 && s2[x] == 0)
			return 0;

		if(s1[x] == 0)
			return -1;

		if(s2[x] == 0)
			return 1;

		if(toupper(s1[x]) - toupper(s2[x]) != 0)
			return toupper(s1[x]) - toupper(s2[x]);
	}

	return 0;
}
