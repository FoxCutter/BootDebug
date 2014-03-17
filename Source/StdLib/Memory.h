#pragma once

class MemoryManager
{
public:
	virtual void * malloc(size_t size, bool Fill) = 0;
	virtual void free(void * p) = 0;
	virtual void * realloc(void * p, size_t size) = 0;
	virtual size_t size(void * p) = 0;
};

extern MemoryManager *MemoryMgr;