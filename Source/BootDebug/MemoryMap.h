#include <stdint.h>
#include "ListNode.h"
#pragma once


enum MemoryType
{
	FreeMemory = 0,
	ReservedMemory = 1,
	AllocatedMemory = 2,
	MemoryHole = 3,
};

class MemoryPageMap
{
public:
	struct MemoryInformation : ListNodeBase<MemoryInformation>
	{
		ListEntry Prev;
		ListEntry Next;

		uint64_t Address;
		uint64_t Length;
		
		MemoryType Type;

		char Name[34];
	};

	MemoryPageMap()
	{
		PageCount = 0;
		PageData = nullptr;
		AllocateList = nullptr;
		FreeList = nullptr;
	}

	MemoryPageMap(uint32_t Address, uint32_t pageCount);
	MemoryPageMap & operator = (MemoryPageMap & LHS);

	void SetAllocatedMemory(uint64_t Address, uint64_t Length, MemoryType Type, char *Name);
	uint64_t AllocateRange(uint64_t MinAddress, uint32_t Length, char *Name);

	void Dump();

private:
	uint32_t PageCount;
	uint8_t *PageData;

	MemoryInformation *AllocateList;
	MemoryInformation *FreeList;

	MemoryInformation *GetMemoryInformation();
};

