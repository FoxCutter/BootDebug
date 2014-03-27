//==========================================
// LIBCTINY - Matt Pietrek 2001
// MSDN Magazine, January 2001
//==========================================

#include "Memory.h"
#include <string.h>

extern "C" void * __cdecl malloc(size_t size)
{
	return MemoryMgr->malloc(size, false); 
}

extern "C" void __cdecl free(void * p)
{
	MemoryMgr->free(p);
}

extern "C" void * __cdecl realloc(void * p, size_t size)
{
    if ( p )
		return MemoryMgr->realloc(p, size);
    else    // 'p' is 0, and HeapReAlloc doesn't act like realloc() here
		return MemoryMgr->malloc(size, false);
}

extern "C" void * __cdecl calloc(size_t nitems, size_t size)
{
    return MemoryMgr->malloc(nitems * size, true );
}

extern "C" size_t __cdecl _msize(void * p)
{
	return MemoryMgr->size(p);
}


// ---------------------------

void * __cdecl operator new(unsigned int s)
{
    return calloc(s, 1);
}

void __cdecl operator delete( void * p )
{
    free(p);
}


// -------------------

#pragma function(memcpy)
extern "C" void * __cdecl memcpy (void * dst, const void * src, size_t count)
{
        void * ret = dst;

        /*
         * copy from lower addresses to higher addresses
         */
        while (count--) {
                *(char *)dst = *(char *)src;
                dst = (char *)dst + 1;
                src = (char *)src + 1;
        }

        return(ret);
}

#pragma function(memset)
extern "C" void * __cdecl memset (void *dst, int val, size_t count)
{
        void *start = dst;

        while (count--) {
                *(char *)dst = (char)val;
                dst = (char *)dst + 1;
        }

        return(start);
}

#pragma function(memcmp)
int __cdecl memcmp(const void * _Buf1, const void * _Buf2, size_t _Size)
{
	const char * Buf1 = reinterpret_cast<const char *>(_Buf1);
	const char * Buf2 = reinterpret_cast<const char *>(_Buf2);

	for(size_t x = 0; x < _Size; x++)
	{
		if(Buf1[x] - Buf2[x] != 0)
			return Buf1[x] - Buf2[x];
	}
	
	return 0;
}
