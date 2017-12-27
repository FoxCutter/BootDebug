#include <atomic>
#include "ListNode.h"
#pragma once

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

struct ThreadInformation : ListNodeBase<ThreadInformation>
{
	ListEntry Prev;
	ListEntry Next;
	
	uintptr_t *	RealAddress;
	uint32_t ThreadID;
	ThreadState State;
	uint32_t TimeSliceCount;
	uint32_t TimeSliceAllocation;
	uint64_t TickCount;

	uint32_t SavedCR3;
	uint32_t SavedESP;
	uint32_t StackLimit;

	uint32_t StartingPoint;
	uintptr_t * StartingData;

	std::atomic_bool Suspended;
};

class Thread
{
public:
	Thread(void);
	~Thread(void);
};

