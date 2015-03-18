#include <atomic>
#pragma once

// Lists seem to be everywhere, so lets just make a simple template to handle them and the most common functions
template<typename T>
struct ListNode
{	
	typedef typename T * ListEntry;
	
	ListEntry Prev;
	ListEntry Next;

	// Get the first node in this list
	inline ListEntry First()
	{
		ListEntry Ret = static_cast<ListEntry>(this);
		while(Ret->Prev != nullptr)
			Ret = Ret->Prev;

		return Ret;
	}

	// Get the last node in this list
	inline ListEntry Last()
	{
		ListEntry Ret = static_cast<ListEntry>(this);
		while(Ret->Next != nullptr)
			Ret = Ret->Next;

		return Ret;
	}

	// Add this node to the end of the passed in list.
	inline void InsertLast(ListEntry List)
	{
		ListEntry Myself = static_cast<ListEntry>(this);
		ListEntry End = List->Last();

		End->Next = Myself;
		Myself->Prev = End;
		Myself->Next = nullptr;
	}

	// Add this node to the head of the passed in list.
	inline void InsertFirst(ListEntry List)
	{
		ListEntry Myself = static_cast<ListEntry>(this);
		ListEntry First = List->First();

		Myself->Next = First;
		Myself->Prev = nullptr;
		First->Prev = nullptr;
	}

	// Remove this node from the list, and return the next node in the list.
	// This way you can do Head = Head.RemoveFromList();
	inline ListEntry RemoveFromList()
	{
		// We always return the next node in the list.
		ListEntry Ret = Next;
		
		if(Prev == nullptr && Next == nullptr)
			return Ret;

		if(Prev == nullptr)
		{
			// We are at the start of our list, so cut out the back pointer on the next block in the chain
			Next->Prev = nullptr;
		}
		else if(Next == nullptr)
		{
			// We're at the end of the chain, so just drop ourselves out.
			Prev->Next = nullptr;
		}
		else
		{
			// Stuck in the middle, so slice ourselves out.
			Prev->Next = Next;
			Next->Prev = Prev;
		}

		Next = nullptr;
		Prev = nullptr;

		return Ret;
	}
};

enum ThreadState
{
	// The Thread information block is just sitting around empty and can be reused
	Unused,

	// The block is good, but there is no context yet
	PreCreate,

	// Currently Suspended.
	Suspended,

	// Running
	Running,

	// Finished Running
	Completed,
};

struct ThreadInformation : ListNode<ThreadInformation>
{
	uintptr_t *	RealAddress;
	uint32_t ThreadID;
	ThreadState State;
	uint32_t TimeSliceCount;
	uint32_t TimeSliceAllocation;
	uint64_t TickCount;

	uint32_t SavedESP;
	uint32_t StackLimit;

	uint32_t StartingPoint;
	uintptr_t * StartingData;

	std::atomic_bool Suspended;

	// Bottom of the stack.
	uint32_t Stack[1];
};


class Thread
{
public:
	Thread(void);
	~Thread(void);
};

