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