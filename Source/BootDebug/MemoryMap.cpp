#include <string.h>
#include <stdio.h>
#include "MemoryMap.h"
#include "Utility.h"

MemoryMap::MemoryMap(void)
{
	NodeRoot = nullptr;
	FreeList = nullptr;
}

MemoryMap::~MemoryMap(void)
{
}

const uint32_t PageSize = 0x1000;

bool MemoryMap::AddMemoryEntry(uint64_t Address, uint64_t Length, MemoryType Type)
{
	if(FreeList == nullptr)
		return false;
	
	// Make sure everything lines up to pages.
	uint32_t Adjustment = Address % PageSize;
	if(Adjustment != 0)
	{
		Length += Adjustment;
		Address -= Adjustment;
	}

	Adjustment = Length % PageSize;
	if(Adjustment != 0)
	{
		// For ram we'll shrink it down to a full page, for anything else well extend it.
		if(Type == FreeMemory)
			Length -= Adjustment;
		
		else
			Length += PageSize - Adjustment;
	}

	
	MemoryNode * CurrentNode = FreeList;
	FreeList = FreeList->RemoveFromList();

	CurrentNode->BaseAddress = Address;
	CurrentNode->EndAddress = (Address + Length) - 1;
	CurrentNode->Type = Type;
	CurrentNode->Fixed = false;

	InsertNode(CurrentNode);

	return true;
}

bool MemoryMap::SetAllocatedMemory(uint64_t Address, uint64_t Length, MemoryType Type)
{
	if(NodeRoot == nullptr)
		return false;

	// Make sure everything lines up to pages.
	uint32_t Adjustment = Address % PageSize;
	if(Adjustment != 0)
	{
		Length += Adjustment;
		Address -= Adjustment;
	}

	Adjustment = Length % PageSize;
	if(Adjustment != 0)
		Length += PageSize - Adjustment;

	uint64_t EndAddress = (Address + Length) - 1;
	
	MemoryNode * CurrentNode = FindAddress(Address);
	if(CurrentNode == nullptr)
		return false;

	// So we know the node where our memory starts, so lets see if we need to trim it off a larger node
	if(CurrentNode->BaseAddress != Address)
	{
		MemoryNode * NewNode = FreeList;
		FreeList = FreeList->RemoveFromList();

		NewNode->BaseAddress = Address;
		NewNode->EndAddress = CurrentNode->EndAddress;
		NewNode->Type = CurrentNode->Type;
		NewNode->Fixed = CurrentNode->Fixed;

		CurrentNode->EndAddress = Address - 1;
		CurrentNode->InsertAfter(NewNode);

		CurrentNode = NewNode;
	}

	// We found the exact right size of block
	if(CurrentNode->EndAddress == EndAddress)
	{
		CurrentNode->Type = Type;
	}
	else if(EndAddress <= CurrentNode->EndAddress)
	{
		// The end is in the CurrentNode
		MemoryNode * NewNode = FreeList;
		FreeList = FreeList->RemoveFromList();
		
		NewNode->BaseAddress = CurrentNode->BaseAddress;
		NewNode->EndAddress = EndAddress;
		NewNode->Type = Type;
		NewNode->Fixed = false;

		CurrentNode->BaseAddress = EndAddress + 1;
		CurrentNode->InsertBefore(NewNode);

		CurrentNode = NewNode;		
	}
	else
	{
		// The end is in another node.
		MemoryNode * NewNode = CurrentNode->Next;

		// Looks like the block we are trying to mark trails off into no mans land.
		if(NewNode == nullptr || NewNode->BaseAddress > EndAddress)
		{
			// Well, extend the current block then.
			CurrentNode->EndAddress = EndAddress;
		}
		else
		{
			NewNode->BaseAddress = EndAddress + 1;
			CurrentNode->EndAddress = EndAddress;
			CurrentNode->Type = Type;
		}
	}

	NodeRoot = CurrentNode->First();

	return true;
}

uint64_t MemoryMap::AllocatePages(uint64_t MinAddress, uint32_t Length, bool Fixed)
{
	if(NodeRoot == nullptr)
		return UINT64_MAX - 1;

	uint32_t Adjustment = Length % PageSize;
	if(Adjustment != 0)
		Length += PageSize - Adjustment;

	MemoryNode * CurrentNode = NodeRoot->First();

	do
	{
		if(CurrentNode->Type == FreeMemory)
		{
			if(CurrentNode->BaseAddress + Length <= CurrentNode->EndAddress)
				break;
		}

		CurrentNode = CurrentNode->Next;

	} while (CurrentNode != nullptr);

	if(CurrentNode == nullptr)
		return UINT64_MAX - 2;

	if(CurrentNode->BaseAddress + Length != CurrentNode->EndAddress)
	{
		MemoryNode * NewNode = FreeList;
		FreeList = FreeList->RemoveFromList();

		NewNode->BaseAddress = CurrentNode->BaseAddress + Length;
		NewNode->EndAddress = CurrentNode->EndAddress;
		NewNode->Type = CurrentNode->Type;
		NewNode->Fixed = CurrentNode->Fixed;

		CurrentNode->EndAddress = CurrentNode->BaseAddress + Length - 1;
		CurrentNode->InsertAfter(NewNode);
	}

	CurrentNode->Fixed = Fixed;
	CurrentNode->Type = AllocatedMemory;

	NodeRoot = CurrentNode->First();

	return CurrentNode->BaseAddress;
}


MemoryMap::MemoryNode * MemoryMap::FindAddress(uint64_t Address)
{
	if(NodeRoot == nullptr)
		return false;
	
	MemoryNode * Ret = NodeRoot;
	do
	{
		if(Address >= Ret->BaseAddress && Address <= Ret->EndAddress)
			break;

		Ret = Ret->Next;

	} while (Ret != nullptr);

	return Ret;
}

void MemoryMap::InsertNode(MemoryNode *NewNode)
{
	if(NodeRoot == nullptr)
	{
		NodeRoot = NewNode;
		return;
	}

	MemoryNode *CurrentNode = NodeRoot;

	while(CurrentNode != nullptr)
	{
		if(NewNode->EndAddress <= CurrentNode->BaseAddress)
		{
			// The current node is before us, so drop it in place
			CurrentNode->InsertBefore(NewNode);
			break;
		}
		else if(CurrentNode->Next == nullptr)
		{
			// At the end of the list, so the new object goes after us
			CurrentNode->InsertAfter(NewNode);
			break;
		}

		CurrentNode = CurrentNode->Next;
	};

	NodeRoot = CurrentNode->First();

	//if(MemoryMap == nullptr)
	//	return false;

	//MemoryNode *CurrentNode = MemoryMap;
	//while(CurrentNode != nullptr)
	//{
	//	if(NewNode->EndAddress < CurrentNode->StartAddress)
	//	{			
	//		// The current node is before us, so drop it in place
	//		CurrentNode->InsertBefore(NewNode);	
	//		break;
	//	}
	//	else if(CurrentNode->Next == nullptr)
	//	{
	//		// At the end of the list, so the new object goes after us
	//		CurrentNode->InsertAfter(NewNode);
	//		break;
	//	}
	//	else if(NewNode->StartAddress >= CurrentNode->StartAddress && NewNode->EndAddress <= NewNode->EndAddress)
	//	{
	//		// The New node is 100% inside the current node
	//		if(NewNode->StartAddress == CurrentNode->StartAddress && NewNode->EndAddress == CurrentNode->EndAddress)
	//		{
	//			// Same node, not really sure how to handle this one.
	//			return false;
	//		}

	//		// Split up the current node around the new node

	//	}		
	//	else if(NewNode->StartAddress < CurrentNode->StartAddress && NewNode->EndAddress <= CurrentNode->EndAddress)
	//	{			
	//		// Partial overlap on the start
	//		if(NewNode->EndAddress == CurrentNode->EndAddress)
	//		{
	//			// In fact the new node encompasses us.
	//			return false;
	//		}

	//		// We need to cut the overlap out of the old node.
	//		CurrentNode->InsertBefore(NewNode);
	//		CurrentNode->StartAddress = NewNode->EndAddress + 1;
	//	}
	//	else if(NewNode->StartAddress >= CurrentNode->StartAddress && NewNode->EndAddress > CurrentNode->EndAddress)
	//	{
	//		// Partial overlap on the end
	//		if(NewNode->StartAddress == CurrentNode->StartAddress)
	//		{
	//			// In fact the new node encompasses us.
	//			return false;
	//		}

	//		// We need to cut the overlap out of the old node.
	//		CurrentNode->InsertAfter(NewNode);
	//		CurrentNode->EndAddress = NewNode->StartAddress - 1;
	//	}
	//	else
	//	{
	//		CurrentNode = CurrentNode->Next;
	//	}
	//};
}

void MemoryMap::FreeNode(MemoryMap::MemoryNode *Node)
{
	Node->RemoveFromList();

	if(FreeList != nullptr)
		FreeList->InsertFirst(Node);

	FreeList = Node;
}

void MemoryMap::Dump()
{
	MemoryNode * CurrentNode = NodeRoot;
	while(CurrentNode != nullptr)
	{
		printf("     ");
		PrintLongAddress(CurrentNode->BaseAddress);
		printf(" - ");
		PrintLongAddress(CurrentNode->EndAddress);
		printf(", ");
		PrintLongAddress(CurrentNode->EndAddress - CurrentNode->BaseAddress + 1);
		printf(", %s (%u)\n", CurrentNode->Type == FreeMemory ? "RAM" : "Reserved", CurrentNode->Type);
		
		CurrentNode = CurrentNode->Next;
	}
}


//-----------------------------------


SmallMemoryMap::SmallMemoryMap(void)
{
	memset(NodePool, 0, sizeof(MemoryMap::MemoryNode) * 32);
	
	for(int x = 0; x < 32; x++)
	{
		FreeNode(&NodePool[x]);
	}
}


SmallMemoryMap::~SmallMemoryMap(void)
{

}
