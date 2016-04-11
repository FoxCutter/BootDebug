#pragma once
#include "ListNode.h"
#include "KernalLib.h"

struct HardwareTree : ListNode<HardwareTree>
{
	//ListEntry Prev;
	//ListEntry Next;
	HardwareTree * Parent;
	HardwareTree * Child;

	// ID string of the device
	char ID[16];

	// A text name of the device
	char Name[32];
};

struct HardwareComplexObj
{
	HardwareTree * Root;
	HardwareTree * Free;

	void Initilize(void * Memory, uint32_t Length)
	{
		Free = reinterpret_cast<HardwareTree *>(Memory);
		Free->Prev = Free->Next = nullptr;

		HardwareTree * Pos = Free;
		// Build the free list
		for(size_t x = 1; x < Length / sizeof(HardwareTree); x++)
		{
			Free[x].Prev = Free[x].Next = nullptr;			
			Pos->InsertAfter(&Free[x]);
			Pos = Pos->Next;
		}

		Root = Free;
		Free = Free->RemoveFromList();

		Root->Child = Root->Parent = nullptr;
		memcpy(Root->ID, "Root", 16);
		memcpy(Root->Name, "Computer", 32);
	}

	HardwareTree * Add(char * ID, char * Name, HardwareTree * Parent = nullptr)
	{
		HardwareTree * Entry = Free;
		Free = Free->RemoveFromList();

		if(Parent == nullptr)
			Entry->Parent = Root;
		else
			Entry->Parent = Parent;

		memcpy(Entry->ID, ID, 16);
		memcpy(Entry->Name, Name, 32);

		if(Entry->Parent->Child == nullptr)
			Entry->Parent->Child = Entry;
		else
			Entry->Parent->Child->InsertLast(Entry);

		return Entry;
	}

	void Dump(HardwareTree * Node = nullptr, uint32_t Depth = 0)
	{
		HardwareTree * Entry = Node;
		if(Entry == nullptr)
			Entry = Root;

		while(Entry != nullptr)
		{
			KernalPrintf("%*.0s %s - %s\n", Depth, " ", Entry->ID, Entry->Name);
			if(Entry->Child != nullptr)
				Dump(Entry->Child, Depth + 1);

			Entry = Entry->Next;
		};
	}
};
