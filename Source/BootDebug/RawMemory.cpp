#include "RawMemory.h"
#include <string.h>

struct RawMemory::MemoryHeader
{
	uint32_t Signature;		
	uint32_t BlockSize;
	uint32_t AllocatedSize;
	MemoryHeader *NextBlock;
	
	uint8_t Filler[16];

	enum HeaderSignatrues
	{
		Allocated =		0xABCDEF01,
		Free =			0xABCDEF02,
		End =			0xABCDEFFF,
	};
};

RawMemory::RawMemory()
{	
	m_BlockChain = nullptr;
	m_LastBlock = nullptr;

	m_BlockSize = 0;
}

RawMemory::~RawMemory(void)
{
}

void RawMemory::SetupHeap(uint32_t HeapBase, uint32_t HeapSize, uint32_t BlockSize)
{
	if(m_BlockChain != nullptr)
		return;

	// First off, lets tweak things to make sure the heap starts on a block
	uint32_t Adjust = HeapBase % BlockSize;
	if(Adjust != 0)
	{
		HeapBase += BlockSize - Adjust;
		HeapSize -= BlockSize - Adjust;
	}

	// And ends on a block.
	Adjust = BlockSize % BlockSize;
	if(Adjust != 0)
		HeapSize -= Adjust;	

	m_BlockSize = BlockSize;

	// Create the first block in the chain
	MemoryHeader * CurrentBlock = (MemoryHeader *)HeapBase;
	MemoryHeader * EndBlock = (MemoryHeader *)((char *)HeapBase + HeapSize - sizeof(MemoryHeader));

	CurrentBlock->Signature = MemoryHeader::Free;
	CurrentBlock->BlockSize = HeapSize - sizeof(MemoryHeader);
	CurrentBlock->AllocatedSize = 0;
	CurrentBlock->NextBlock = EndBlock;
	memset(CurrentBlock->Filler, 0x90, 16);			

	EndBlock->Signature = MemoryHeader::End;
	EndBlock->BlockSize = sizeof(MemoryHeader);
	EndBlock->AllocatedSize = 0;
	EndBlock->NextBlock = CurrentBlock;
	memset(EndBlock->Filler, 0x90, 16);			

	m_BlockChain = CurrentBlock;
	m_LastBlock = EndBlock;
}


void * RawMemory::malloc(size_t size, bool Fill)
{
	if(size == 0)
		return nullptr;
	
	// Pad it out to the size of a full block
	uint32_t Padding = size % m_BlockSize;

	// Make sure we have some sort of padding.
	if(Padding == 0)
		Padding = m_BlockSize;
	else
		Padding = m_BlockSize - Padding;


	// And add it all up to the full size of the block we are looking for.
	uint32_t BlockSize = sizeof(MemoryHeader);
	BlockSize += size;
	BlockSize += Padding;

	// Work out how much we want left in the memory block when we split it.
	uint32_t MinRemaining = sizeof(MemoryHeader) + (m_BlockSize * 2);

	MemoryHeader * CurrentBlock = m_BlockChain;

	// Walk the chain
	while(CurrentBlock->Signature != MemoryHeader::End)
	{
		//printf("CurrentBlock: %08X\n", CurrentBlock);
		//printf("  SG: %08X\n", CurrentBlock->Signature);
		//printf("  BS: %08X\n", CurrentBlock->BlockSize);
		//printf("  AS: %08X\n", CurrentBlock->AllocatedSize);
		//printf("  NB: %08X\n", CurrentBlock->NextBlock);
		
		if(CurrentBlock->Signature == MemoryHeader::Free && CurrentBlock->BlockSize >= BlockSize)
		{
			// Chop the block (if we have room)
			if(CurrentBlock->BlockSize >= BlockSize + MinRemaining)
			{
				MemoryHeader * NextBlock = (MemoryHeader *)((char *)CurrentBlock + BlockSize);
			
				NextBlock->Signature = MemoryHeader::Free;
				NextBlock->BlockSize = CurrentBlock->BlockSize - BlockSize;
				NextBlock->AllocatedSize = 0;
				NextBlock->NextBlock = CurrentBlock->NextBlock;

				CurrentBlock->NextBlock = NextBlock;
				CurrentBlock->BlockSize = BlockSize;
			}

			CurrentBlock->Signature = MemoryHeader::Allocated;
			CurrentBlock->AllocatedSize = size;

			memset(CurrentBlock->Filler, 0x90, 16);			
			void *Data = ++CurrentBlock;

			if(Fill)
				memset(Data, 0, size);

			memset((char *)Data + size, 0x90, Padding);
			
			return Data;
		}

		CurrentBlock = CurrentBlock->NextBlock;
	}
	
	return nullptr;
}

void RawMemory::free(void * p)
{
	if(p == nullptr)
		return;
	
	MemoryHeader * CurrentBlock = FindHeader(p);
	if(CurrentBlock == nullptr)
		return;

	CurrentBlock->Signature = MemoryHeader::Free;
	CurrentBlock->AllocatedSize = 0;

	return;
}

void * RawMemory::realloc(void * p, size_t size)
{
	if(p == nullptr)
		return malloc(size, false);
	
	MemoryHeader * CurrentBlock = FindHeader(p);
	if(CurrentBlock == nullptr)
		return nullptr;

	if(size <= CurrentBlock->AllocatedSize)
	{
		CurrentBlock->AllocatedSize = size;
		return CurrentBlock++;
	}

	char * NewBuffer = (char *)malloc(size, false);
	if(NewBuffer == nullptr)
		return nullptr;

	memcpy(NewBuffer, p, size);

	free(p);

	return NewBuffer;
}

size_t RawMemory::size(void * p)
{
	MemoryHeader * CurrentBlock = FindHeader(p);
	if(CurrentBlock == nullptr)
		return 0;

	if(CurrentBlock->Signature == MemoryHeader::Free)
		return 0;
	
	return CurrentBlock->AllocatedSize;
}

RawMemory::MemoryHeader *RawMemory::FindHeader(void *p)
{
	if(p == nullptr)
		return nullptr;

	if(p < m_BlockChain)
		return nullptr;

	if(p > m_LastBlock)
		return nullptr;
	
	MemoryHeader * CurrentBlock = (MemoryHeader *)p;
	CurrentBlock--;

	if(CurrentBlock->Signature == MemoryHeader::Allocated || 
   	   CurrentBlock->Signature == MemoryHeader::Free || 
	   CurrentBlock->Signature == MemoryHeader::End)
		return CurrentBlock;

	return nullptr;
}