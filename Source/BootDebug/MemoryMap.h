#include <stdint.h>
#include "ListNode.h"
#pragma once

enum MemoryType
{
	FreeMemory = 1,
	ReservedMemory,
	RecoverableMemory,
	HibernationMemory,	
	AllocatedMemory = 0x010,
};

class MemoryMap
{
protected:
	struct MemoryNode : ListNode<MemoryNode>
	{
		uint64_t BaseAddress;
		uint64_t EndAddress;
		MemoryType Type;
		bool Fixed;
	};

	void FreeNode(MemoryNode *Node);
	void InsertNode(MemoryNode *Node);
	MemoryNode * FindAddress(uint64_t Address);

private:
	MemoryNode *NodeRoot;
	MemoryNode *FreeList;

public:
	MemoryMap(void);
	~MemoryMap(void);

	bool AddMemoryEntry(uint64_t Address, uint64_t Length, MemoryType Type);
	bool SetAllocatedMemory(uint64_t Address, uint64_t Length, MemoryType Type);
	uint64_t AllocatePages(uint64_t MinAddress, uint32_t Length, bool Fixed);

	void Dump();
};

// A smaller memory map with a fixed block of nodes used for building the initial Map (which is then used to allocate a larger pool for the full map).
class SmallMemoryMap : public MemoryMap
{
	MemoryMap::MemoryNode NodePool[32];

public:
	SmallMemoryMap(void);
	~SmallMemoryMap(void);
};

