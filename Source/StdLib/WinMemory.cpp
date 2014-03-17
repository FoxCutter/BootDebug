#include "WinMemory.h"
#include <windows.h>


WinMemory::WinMemory(void)
{
}


WinMemory::~WinMemory(void)
{
}

void * WinMemory::malloc(size_t size, bool Fill)
{
	return HeapAlloc( GetProcessHeap(), (Fill ? HEAP_ZERO_MEMORY : 0), size );
}

void WinMemory::free(void * p)
{
	HeapFree( GetProcessHeap(), 0, p );
}

void * WinMemory::realloc(void * p, size_t size)
{
	return HeapReAlloc( GetProcessHeap(), 0, p, size );
}

size_t WinMemory::size(void * p)
{
	return HeapSize( GetProcessHeap(), 0, p );
}
