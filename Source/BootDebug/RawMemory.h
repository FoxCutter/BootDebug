#include <stdint.h>
#include "..\StdLib\Memory.h"

#pragma once
class RawMemory :
	public MemoryManager
{
	struct MemoryHeader;

public:
	RawMemory(uint32_t HeapBase, uint32_t HeapSize, uint32_t BlockSize);
	~RawMemory(void);

	void * malloc(size_t size, bool Fill);
	void free(void * p);
	void * realloc(void * p, size_t size);
	size_t size(void * p);

private:
	MemoryHeader *FindHeader(void *p);
	
	MemoryHeader *m_BlockChain;
	MemoryHeader *m_LastBlock;

	uint32_t m_BlockSize;
};

