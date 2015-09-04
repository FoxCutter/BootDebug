#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <atomic>
#include <new>

#include "RawTerminal.h"
#include "RawMemory.h"
#include "MultiBootInfo.h"
#include "MultiBoot.h"

#include "LowLevel.h"
#include "InterruptControler.h"
#include "Stack.h"

#include "GDT.h"
#include "IDT.h"
#include "MMU.h"

#include "ACPI.h"
#include "PCI.h"

#include "OpenHCI.h"
#include "Thread.h"
#include "MemoryMap.h"

#include "CoreComplex.h"
#include "KernalLib.h"

#include "..\StdLib\initterm.h"

void main(int argc, char *argv[]);
extern MMU * MMUManager;
extern OpenHCI * USBManager;

void SystemTrap(InterruptContext * Context, uintptr_t * Data)
{
	if(Context->InterruptNumber == 0x0d)
	{
		// Bad MSRs through GPFs, so eat them and move on.
		// 0F 30 WRMSR
		// 0F 32 RDMSR
		if(reinterpret_cast<uint16_t *>(Context->SourceEIP)[0] == 0x300F || reinterpret_cast<uint16_t *>(Context->SourceEIP)[0] == 0x320F)
		{
			//KernalPrintf("\nInvalid MSR [%X]\n", Context->ECX);
			Context->EAX = 0x00000000;
			Context->EDX = 0x00000000;
			Context->SourceEFlags = Context->SourceEFlags & ~EFlags::Carry;
			Context->SourceEIP += 2;
			return;
		}
		else
		{
			KernalPrintf("\nGPF Fault %08X at: %08X\n Error: ", Context->ErrorCode, Context->SourceEIP);
		}
	}
	else if(Context->InterruptNumber == 0x0e)
	{
		KernalPrintf("\nPage Fault from: %08X\n", Context->SourceEIP);
		uint32_t CR2 = ReadCR2();
		KernalPrintf("Address: %08X\n", CR2);

		if(Context->ErrorCode & 0x01)
			KernalPrintf("page protection, ");
		else
			KernalPrintf("non-present page, ");

		if(Context->ErrorCode & 0x02)
			KernalPrintf("write, ");
		else
			KernalPrintf("read, ");

		if(Context->ErrorCode & 0x04)
			KernalPrintf("user");
		else
			KernalPrintf("supervisor");

		if(Context->ErrorCode & 0x08)
			KernalPrintf(", reserved bit");

		if(Context->ErrorCode & 0x10)
			KernalPrintf(", instruction fetch");

		KernalPrintf(" (%08X)\n", Context->ErrorCode);

		MMUManager->PrintAddressInformation(CR2);
	}
	else
	{	
		KernalPrintf("\nSystem Error: %02X (%08X) at %08X\n", Context->InterruptNumber, Context->ErrorCode, Context->SourceEIP);
	};

	ASM_CLI;

	for(;;) ASM_HLT;
}

unsigned char kbdus[128] =
{
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8',	'9', '0', '-', '=', '\b',	/* Backspace */
   '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',	/* Enter key */
    0,			/* 29   - Control */
	'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',  
	0,		/* Left shift */
	'\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/',  
	0,				/* Right shift */
	'*',
    0,	/* Alt */
	' ',	/* Space bar */
    0,	/* Caps lock */
    0,	/* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,	/* < ... F10 */
    0,	/* 69 - Num lock*/
    0,	/* Scroll Lock */
    0,	/* Home key */
    0,	/* Up Arrow */
    0,	/* Page Up */
	'-',
    0,	/* Left Arrow */
    0,
    0,	/* Right Arrow */
	'+',
    0,	/* 79 - End key*/
    0,	/* Down Arrow */
    0,	/* Page Down */
    0,	/* Insert Key */
    0,	/* Delete Key */
    0,   0,   0,
    0,	/* F11 Key */
    0,	/* F12 Key */
    0,	/* All other keys are undefined */
};		

unsigned char kbdus_Shift[128] =
{
	0,  27, '!', '@', '#', '$', '%', '^', '&', '*',	'(', ')', '_', '+', '\b',	/* Backspace */
	'\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',	/* Enter key */
    0,			/* 29   - Control */
	'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '\"', '~',  
	0,		/* Left shift */
	'|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?',  
	0,				/* Right shift */
	'*',
    0,	/* Alt */
	' ',	/* Space bar */
    0,	/* Caps lock */
    0,	/* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,	/* < ... F10 */
    0,	/* 69 - Num lock*/
    0,	/* Scroll Lock */
    0,	/* Home key */
    0,	/* Up Arrow */
    0,	/* Page Up */
	'-',
    0,	/* Left Arrow */
    0,
    0,	/* Right Arrow */
	'+',
    0,	/* 79 - End key*/
    0,	/* Down Arrow */
    0,	/* Page Down */
    0,	/* Insert Key */
    0,	/* Delete Key */
    0,   0,   0,
    0,	/* F11 Key */
    0,	/* F12 Key */
    0,	/* All other keys are undefined */
};		

InterruptControler m_InterruptControler;

volatile char KBBuffer[32];
volatile uint8_t KBBufferFirst = 0;
volatile uint8_t KBBufferLast = 0;

uint16_t KeyState = 0;

#define LeftShift	0x01
#define RightShift	0x02
#define LeftCtrl	0x04
#define RightCtrl	0x08
#define LeftAlt		0x10
#define RightAlt	0x20

#define NumLock		0x100
#define	ScrollLock	0x200
#define CapsLock	0x400

void InsertKeyboardBuffer(char Key)
{
	if((KBBufferLast + 1) == KBBufferFirst || KBBufferFirst == 0 && (KBBufferLast + 1 == 32))
	{
		//printf("Full!");
	}
	else if(Key != 0)
	{
		KBBuffer[KBBufferLast] = Key;
		KBBufferLast++;
				
		if(KBBufferLast == 32)
			KBBufferLast = 0;
	}
}

char FetchKeyboardBuffer()
{
	while(KBBufferFirst == KBBufferLast)
	{
		ASM_HLT;
	}

	char value = KBBuffer[KBBufferFirst];
		
	KBBufferFirst++;
	if(KBBufferFirst == 32)
		KBBufferFirst = 0;

	return value;
}

void KeyboardInterrupt(InterruptContext * Context, uintptr_t * Data)
{
	unsigned char scancode = InPortb(0x60);		
	unsigned char scancode2 = 0;
	int Escape = 0;

	if(scancode == 0xE0)
	{
		Escape = 1;
		scancode = InPortb(0x60);
	}
	else if(scancode == 0xE1)
	{
		Escape = 2;
		scancode = InPortb(0x60);
		scancode2 = InPortb(0x60);
	}

	bool KeyUp = ((scancode & 0x80) == 0x80);
	scancode = scancode & 0x7F;

	if(scancode == 0x2A && Escape == 0) // LSHIFT
	{
		if(KeyUp)
			KeyState &= ~LeftShift;
			
		else
			KeyState |= LeftShift;
	}
	else if(scancode == 0x36 && Escape == 0) // RSHIFT
	{
		if(KeyUp)
			KeyState &= ~RightShift;
			
		else
			KeyState |= RightShift;
	}
	else if(!KeyUp && Escape == 0)
	{
		char Key = 0;
		if(KeyState & LeftShift || KeyState & RightShift)
			Key = kbdus_Shift[scancode];
		else
			Key = kbdus[scancode];

		InsertKeyboardBuffer(Key);
	}
}

ThreadInformation *ThreadListHead;

void IdleThread(ThreadInformation *Thread,  uintptr_t * Data)
{
	for(;;) ASM_HLT;
}

ThreadInformation *FindNextThread(ThreadInformation *CurrentThread)
{
	ThreadInformation *NextThread = CurrentThread->Next;
	if(NextThread == nullptr)
		NextThread = ThreadListHead;

	while(NextThread != CurrentThread)
	{
		if(NextThread->Suspended.load() == false)
			break;

		NextThread = NextThread->Next;

		if(NextThread == nullptr)
			NextThread = ThreadListHead;
	}

	return NextThread;
}

#pragma warning(push)
// This turns off the warning: 
//       warning C4731: 'ClockInterrupt' : frame pointer register 'ebp' modified by inline assembly code
// Because that's the point in this case. 
#pragma warning(disable: 4731)

void ClockInterrupt(volatile InterruptContext * Context, uintptr_t * Data)
{
	ThreadInformation *CurrentThread = reinterpret_cast<ThreadInformation *>(ReadFS(8));
	
	CurrentThread->TickCount++;
	if(CurrentThread->TimeSliceCount >= CurrentThread->TimeSliceAllocation)
	{
		CurrentThread->TimeSliceCount = 0;

		ThreadInformation *NextThread = FindNextThread(CurrentThread);
		
		if(NextThread == CurrentThread)
			return;
		
		_asm
		{
			mov ebx, CurrentThread
			mov ecx, NextThread
			lea esi, [ebx]
			lea edi, [ecx]

			push ebp

			mov eax, cr3 
			mov [esi].SavedCR3, eax
			mov edx, [edi].SavedCR3

			mov [esi].SavedESP, esp
			mov esp, [edi].SavedESP

			cmp eax, edx
			je SkipCR		
			mov cr3, edx
		SkipCR:
			pop ebp
			
			mov CurrentThread, ebx
			mov NextThread, ecx

			mov eax, 0
			mov ebx, 0
			mov ecx, 0
			mov edx, 0
			mov edi, 0
			mov esi, 0
		}

		uint16_t OldFS;
		ASM_ReadReg(fs, OldFS);
		GDTManager::UpdateGDTEntry(OldFS, reinterpret_cast<uint32_t>(NextThread), sizeof(ThreadInformation), GDT::Present | GDT::Operand32Bit | GDT::NonSystemFlag, GDT::DataReadWrite, 3);
		ASM_WriteReg(fs, OldFS);
	}
	else
	{
		CurrentThread->TimeSliceCount++;
	}
}


#pragma warning(pop)

typedef void (*ThreadPtr)(ThreadInformation * Context, uintptr_t * Data);

void ThreadStart()
{
	ASM_STI;
	//OutPortb(0x20, 0x20);
	m_InterruptControler.ClearIRQ(0);

	ThreadInformation *CurrentThread = reinterpret_cast<ThreadInformation *>(ReadFS(8));

	ThreadPtr Call = reinterpret_cast<ThreadPtr>(CurrentThread->StartingPoint);

	Call(CurrentThread, CurrentThread->StartingData);

	while(true)
		printf("!");
}

extern MulitBoot::Header MB1Header;

// Okay, same notes on how things are going to 'work' in the future.
// * When we get into MultiBootMain we have a stack and the segments are good, but that's about all we have
// * Step one will be work out a place to put the Core Complex. We will find a block of memory that is Free in the low memory. Starting with 0x10000, 
//    then moving up by 0x10000. This will become the Core Complex
// * The Core Complex will hold our GDT (with only a dozen or so entries), the full IDT and pointers other core structures
// * We'll build our GDT and IDTs, and set up the pointers and reset our segments.
// * Selector 0x50 will be set to point to the base of the Core Complex, and this will be assigned to GS.
// * We will then build up our memory map using the info from the multiboot header
// * Pull out everything from the Multiboot header we need/want. After that assume it's free memory.
// * Set up the basic MMU with direct mapping, and if we have less then 1gig ram, map the ram into the right place 
// * do a full load of the image at the 1gig line. 
// * Set up the Kernel thread, then allow it to take over, jumping us into the relocated kernel.
// * Full Kernel takes ownership of the Core Complex, marks the load location as free.
// * After that we will grab some memory for the Kernel Heap, and set up the raw terminal.
// * Create both the Monitor thread and the idle thread
// * Start the monitor

// Eventual all of this will be in the Core Complex
IDTManager IDTData;
OpenHCI USB;

GDT::TSS MyTSS;

MemoryPageMap BuildMemoryPageMap(MultiBootInfo &MBReader)
{
	// Step 1a: Enumerate over the Memory information in the MB data and work out some information.		
	uint64_t MaxMemory = 0;			// The last byte of memory we know of
	uint32_t HighReserved = 0;		// should be the start of reserved memory between 1 meg and 4 gig
	uint64_t TotalMemory = 0x10000;

	MemoryMapEntry CurrentEntry;
	MBReader.GetFirstMemoryEntry(CurrentEntry);

	do
	{
		TotalMemory += CurrentEntry.Length;

		uint64_t EndAddress = CurrentEntry.BaseAddress + CurrentEntry.Length;
		if(EndAddress > MaxMemory)
			MaxMemory = EndAddress;

		if(HighReserved == 0 && CurrentEntry.Type != 1 && EndAddress < 0x100000000ull)
		{
			if(CurrentEntry.BaseAddress > 0x00100000 && CurrentEntry.BaseAddress > HighReserved)
				HighReserved = static_cast<uint32_t>(CurrentEntry.BaseAddress);
		}
	} while(MBReader.GetNextMemoryEntry(CurrentEntry));

	KernalPrintf("  Total Memory: %08X:%08X\n", (uint32_t)((TotalMemory & 0xFFFFFFFF00000000) >> 32), (uint32_t)(TotalMemory & 0xFFFFFFFF));
	KernalPrintf("  Memory Hole: %08X, Highest Address: %08X:%08X\n", HighReserved, (uint32_t)((MaxMemory & 0xFFFFFFFF00000000) >> 32), (uint32_t)(MaxMemory & 0xFFFFFFFF));

	// Step 1b: Work out how big the map will be.
	// Total pages
	uint32_t PageCount = static_cast<uint32_t>(MaxMemory / 0x1000);
	if(MaxMemory % 0x1000)
		PageCount++;
	
	// Two bytes per a page, means that a given page of memory can map 0x4000 pages (64 megs)
	uint32_t MemoryMapSize = PageCount / 0x4000;
	if(PageCount % 0x4000)
		MemoryMapSize++;
	
	// Work out the Memory map location
	HighReserved = HighReserved - (MemoryMapSize * 0x1000);
	
	// And make sure it starts on a page
	if(HighReserved % 0x1000 != 0)
		HighReserved -= HighReserved % 0x1000;

	// Step 1c: Create the map
	MemoryPageMap TempMap(HighReserved, PageCount);
	
	KernalPrintf("  Memory Map: %08X, Size %08X\n", HighReserved, PageCount, MemoryMapSize * 0x1000);

	// Step 1d set the memory information from the MB data
	MBReader.GetFirstMemoryEntry(CurrentEntry);

	do {
		MemoryType Type = ReservedMemory;
		if(CurrentEntry.Type == 1)
			Type = FreeMemory;

		if(CurrentEntry.Type == 3)
			Type = AllocatedMemory;

		TempMap.SetAllocatedMemory(CurrentEntry.BaseAddress, CurrentEntry.Length, Type);
	} while(MBReader.GetNextMemoryEntry(CurrentEntry));

	// Step 1e set all the know reserved and allocated data objects
	TempMap.SetAllocatedMemory(0x0, 0x10000, ReservedMemory);
	TempMap.SetAllocatedMemory(MB1Header.load_address, MB1Header.bss_end_address - MB1Header.load_address, AllocatedMemory);
	TempMap.SetAllocatedMemory(reinterpret_cast<uint32_t>(MBReader.MBData), MBReader.HeaderLength, AllocatedMemory);
	TempMap.SetAllocatedMemory(HighReserved, MemoryMapSize * 0x1000, AllocatedMemory);	

	{
		ModuleEntry CurrentModule;
		if(MBReader.GetFirstModuleEntry(CurrentModule))
		{
			do {
				TempMap.SetAllocatedMemory(CurrentModule.ModStart, CurrentModule.ModEnd - CurrentModule.ModStart, AllocatedMemory);
			} while(MBReader.GetNextModuleEntry(CurrentModule));
		}
	}

	return TempMap;
}

CoreComplexObj *BuildCoreComplex(MemoryPageMap &MemoryMap)
{
	// Pick an Address for the Core Complex
	uint64_t TempAddress = UINT64_MAX;
	TempAddress = MemoryMap.AllocateRange(0x10000, 0x10000);

	if(TempAddress == UINT64_MAX)
		KernalPanic(KernalCode::GeneralError, "Unable to allocated core complex");
	
	CoreComplexObj *TempComplex = new(reinterpret_cast<void *>(TempAddress)) CoreComplexObj();
	TempComplex->Self = TempComplex;
	TempComplex->PageMap = MemoryMap;
	KernalPrintf("  Core Complex: %08X\n", TempComplex);

	return TempComplex;
}

void BuildGDT(CoreComplexObj *CoreComplex)
{
	// The NULL Segment is automatically added for us
	CoreComplex->CodeSegment0	= CoreComplex->GDTTable.AddGDTEntry(0, 0xFFFFFFFF, GDT::Present | GDT::Operand32Bit | GDT::NonSystemFlag, GDT::CodeExecuteRead, 0);
	CoreComplex->DataSegment0	= CoreComplex->GDTTable.AddGDTEntry(0, 0xFFFFFFFF, GDT::Present | GDT::Operand32Bit | GDT::NonSystemFlag, GDT::DataReadWrite, 0);
	CoreComplex->CodeSegment3	= CoreComplex->GDTTable.AddGDTEntry(0, 0xFFFFFFFF, GDT::Present | GDT::Operand32Bit | GDT::NonSystemFlag, GDT::CodeExecuteRead, 3);
	CoreComplex->DataSegment3	= CoreComplex->GDTTable.AddGDTEntry(0, 0xFFFFFFFF, GDT::Present | GDT::Operand32Bit | GDT::NonSystemFlag, GDT::DataReadWrite, 3);
	CoreComplex->CodeSegment2	= CoreComplex->GDTTable.AddGDTEntry(0, 0xFFFFFFFF, GDT::Present | GDT::Operand32Bit | GDT::NonSystemFlag, GDT::CodeExecuteRead, 2);
	CoreComplex->DataSegment2	= CoreComplex->GDTTable.AddGDTEntry(0, 0xFFFFFFFF, GDT::Present | GDT::Operand32Bit | GDT::NonSystemFlag, GDT::DataReadWrite, 2);
	CoreComplex->CodeSegment1	= CoreComplex->GDTTable.AddGDTEntry(0, 0xFFFFFFFF, GDT::Present | GDT::Operand32Bit | GDT::NonSystemFlag, GDT::CodeExecuteRead, 1);
	CoreComplex->DataSegment1	= CoreComplex->GDTTable.AddGDTEntry(0, 0xFFFFFFFF, GDT::Present | GDT::Operand32Bit | GDT::NonSystemFlag, GDT::DataReadWrite, 1);
	CoreComplex->CoreSegment	= CoreComplex->GDTTable.AddGDTEntry(reinterpret_cast<uint32_t>(CoreComplex), 0x10000, GDT::Present | GDT::Operand32Bit | GDT::NonSystemFlag, GDT::DataReadWrite, 0);
	CoreComplex->ThreadSegment	= CoreComplex->GDTTable.AddGDTEntry(0, sizeof(ThreadInformation), GDT::Present | GDT::Operand32Bit | GDT::NonSystemFlag, GDT::DataReadWrite, 3);
	//CoreComplex->TaskSegment	= CoreComplex->GDTTable.AddGDTEntry(reinterpret_cast<uint32_t>(&MyTSS), sizeof(GDT::TSS), GDT::Present, GDT::TSS32BitSegment, 0);
	
	// Switch to our GDT
	KernalPrintf("  GDT %08X, Entries %02X\n", &CoreComplex->GDTTable, CoreComplex->GDTTable.TableSize());
	CoreComplex->GDTTable.SetActive(CoreComplex->CodeSegment0, CoreComplex->DataSegment0);

	// Set our system segments	
	uint16_t TempSegment = CoreComplex->ThreadSegment;
	ASM_WriteReg(fs, TempSegment);
	
	TempSegment = CoreComplex->CoreSegment;
	ASM_WriteReg(gs, TempSegment);
}

extern "C" void MultiBootMain(void *Address, uint32_t Magic) 
{
	// At this point we are officially alive, but we're still a long ways away from being up and running.
	MultiBootInfo MBReader;

	// Parse the MultiBoot info
	MBReader.LoadMultiBootInfo(Magic, Address);

	// Panic if we don't have a text mode display of some sort
	if(MBReader.FrameBuffer.Type != 0x02)
		ASM_HLT;

	// Set up a simple text terminal for the kernal to use
	RawTerminal TextTerm(static_cast<uint32_t>(MBReader.FrameBuffer.Address), MBReader.FrameBuffer.Width, MBReader.FrameBuffer.Height, MBReader.FrameBuffer.Pitch);
	TextTerm.SetPauseFullScreen(false);
	TextTerm.Clear();
	KernalTerminal = &TextTerm;

	KernalPrintf("Starting up BootDebug...\n");
	KernalPrintf(" MultiBoot v%u %s\n", MBReader.Type, MBReader.BootLoader);

	// Step 1: Build the memory map in physical memory
	KernalPrintf(" Building Memory Map...\n");
	MemoryPageMap TempMap = BuildMemoryPageMap(MBReader);
	
	// Step 2: Build the Core Complex
	KernalPrintf(" Building Core Complex...\n");
	CoreComplexObj *CoreComplex = BuildCoreComplex(TempMap);
	CoreComplex->MultiBoot.LoadMultiBootInfo(Magic, Address);
	
	// Step 3: Set up our GDT
	KernalPrintf(" Building GDT...\n");
	BuildGDT(CoreComplex);

	// Step 2: Set our IDT
	KernalPrintf(" Setting up IDT...\n");
	IDTData.SetSelectors(CoreComplex->CodeSegment0, CoreComplex->DataSegment0);

	// Set up the handlers for System errors
	for(int x = 0; x < 0x20; x++)
	{
		IDTData.SetInterupt(x, SystemTrap);
	}

	IDTData.SetActive();

	// Step 4: Bring up the MMU for the kernel space
	KernalPrintf(" Setting up Memory Manager...\n");

	CurrentTerminal = &TextTerm;

	// Map the Core Complex pages 1:1 into virtual memory
	// Okay, this doesn't do anything like that yet, but still...
	MMU Memory;
	MMUManager = &Memory;
	
	// At this point we will be up and running with virtual memory, so their will be a clear different between Physical and Virtual addresses


	// Step X: Create the Kernal's heap
	// Get a range for our dynamic memory, starting at 2megs and asking for 1 meg	
	uint64_t TempAddress = CoreComplex->PageMap.AllocateRange(0x200000, 0x10000);

	// Start it up with 16 byte blocks
	CoreComplex->KernalHeap.SetupHeap(static_cast<uint32_t>(TempAddress), 0x10000, 0x10);
	MemoryMgr = &CoreComplex->KernalHeap;





	CurrentTerminal = &TextTerm;
	//printf("%016llX\n", sizeof(CoreComplexObj));
	//FinalMap.Dump();


	// At this point we should be able to display text and allocate memory.

	// Get the execution environment running
	KernalPrintf(" Starting Executer...\n");
	_atexit_init();
	_initterm();

	// Step 3: Remap IRQs
	KernalPrintf(" Setting up IRQs...\n");
	m_InterruptControler.RemapIRQBase(0x20);

	m_InterruptControler.SetIDT(0x20, &IDTData);
	m_InterruptControler.SetIRQInterrupt(0x01, KeyboardInterrupt);
	
	// Create the Idle Thread
	KernalPrintf(" Setting up Threads...\n");
	ThreadListHead = reinterpret_cast<ThreadInformation *>(0x20000000);
	ThreadListHead->Prev = 0;
	ThreadListHead->Next = 0;

	ThreadListHead->RealAddress = reinterpret_cast<uintptr_t *>(ThreadListHead);
	ThreadListHead->ThreadID = reinterpret_cast<uint32_t>(ThreadListHead);// 0;
	ThreadListHead->State = PreCreate;
	ThreadListHead->TimeSliceCount = 0;
	ThreadListHead->TimeSliceAllocation = 10;
	ThreadListHead->TickCount = 0;

	ThreadListHead->SavedCR3 = ReadCR3();
	ThreadListHead->SavedESP = 0;
	ThreadListHead->StackLimit = 0x8000;

	ThreadListHead->StartingPoint = reinterpret_cast<uint32_t>(IdleThread); 
	ThreadListHead->StartingData = nullptr;

	ThreadListHead->Suspended = false;
	
	int StackSize = ThreadListHead->StackLimit / 4;
	ThreadListHead->SavedESP = reinterpret_cast<uint32_t>(&(ThreadListHead->Stack[StackSize]));
	Stack::Push(ThreadListHead->SavedESP, reinterpret_cast<uint32_t>(ThreadStart)); // IP
	Stack::Push(ThreadListHead->SavedESP, ThreadListHead->SavedESP); // The old BP

	uint32_t OldESP = ThreadListHead->SavedESP;

	ThreadListHead->SavedESP -= 10 * sizeof(uint32_t);
	Stack::Push(ThreadListHead->SavedESP, OldESP);  // Current BP

	ThreadInformation * MyThread = reinterpret_cast<ThreadInformation *>(reinterpret_cast<uint32_t>(ThreadListHead->Stack) + ThreadListHead->StackLimit + sizeof(ThreadInformation));

	// Convert ourselves into a full thread
	MyThread->RealAddress = reinterpret_cast<uintptr_t *>(MyThread);
	MyThread->ThreadID = reinterpret_cast<uint32_t>(MyThread);// 1;
	MyThread->State = Running;
	MyThread->TimeSliceCount = 0;
	MyThread->TimeSliceAllocation = 30;
	MyThread->TickCount = 0;
	MyThread->StackLimit = 0x8000;
	
	MyThread->StartingPoint = 0;
	MyThread->StartingData = nullptr;

	MyThread->Prev = nullptr;
	MyThread->Next = nullptr;
	MyThread->Suspended = false;

	ThreadListHead->InsertLast(MyThread);

	CoreComplex->GDTTable.UpdateGDTEntry(CoreComplex->ThreadSegment, reinterpret_cast<uint32_t>(MyThread), sizeof(ThreadInformation), GDT::Present | GDT::Operand32Bit | GDT::NonSystemFlag, GDT::DataReadWrite, 3);
	uint16_t TempSegment = CoreComplex->ThreadSegment;
	ASM_WriteReg(fs, TempSegment);

	// And set up the PIT for ~1.5ms 
	int ClockSpeed = 1193180 / 650;
	//int ClockSpeed = 0;

	OutPortb(0x43, 0x34); // Channel 0, Lo/Hi, Mode 2 (Rate generator), Binary
	OutPortb(0x40, ClockSpeed & 0xFF); // Low Byte
	OutPortb(0x40, ClockSpeed >> 8); // High Byte

	m_InterruptControler.SetIRQInterrupt(0x00, (InterruptCallbackPtr)ClockInterrupt);	

	ASM_STI;

	PCI PCIBus;
	uint32_t USBID = PCIBus.FindDeviceID(0x0C, 0x03, 0x10);

	if(USBID != 0xFFFFFFFF)
	{
		USB.StartUp(USBID, &m_InterruptControler);
		USBManager = &USB;
	}

	// Step 6: Start the full kernel
	const char * CommandLine = CoreComplex->MultiBoot.CommandLine;

	KernalPrintf(" Command Line: %s\n", CommandLine);

	//printf("%08x\n", Address);
	
	//_ConvertCommandLineToArgcArgv(
	char * argv[2];
	argv[0] = "BootDebug";
	argv[1] = nullptr;
	
	//printf("Hi!");

	TextTerm.SetPauseFullScreen(true);
	KernalPrintf(" Starting Monitor\n\n");
	
	for(;;)
		main(1, argv);
	
	//printf("BYe!");

	for(;;) ASM_HLT;

	//printf("Woops!");
}


