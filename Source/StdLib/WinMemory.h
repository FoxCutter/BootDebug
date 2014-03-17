#include "Memory.h"
#pragma once

class WinMemory : 
	public MemoryManager
{
public:
	WinMemory(void);
	~WinMemory(void);

	void * malloc(size_t size, bool Fill);
	void free(void * p);
	void * realloc(void * p, size_t size);
	size_t size(void * p);
};

