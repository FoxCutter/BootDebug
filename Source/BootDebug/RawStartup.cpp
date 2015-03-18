#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <atomic>

#include "RawTerminal.h"
#include "RawMemory.h"
#include "MultiBootInfo.h"

#include "LowLevel.h"
#include "InterruptControler.h"

#include "GDT.h"
#include "IDT.h"
#include "MMU.h"

#include "ACPI.h"
#include "PCI.h"

#include "OpenHCI.h"
#include "Thread.h"

#include "..\StdLib\initterm.h"

void main(int argc, char *argv[]);
extern MultiBootInfo * MultiBootHeader;
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
			printf("\nInvalid MSR [%X]\n", Context->ECX);
			Context->EAX = 0x00000000;
			Context->EDX = 0x00000000;
			Context->SourceEIP += 2;
			return;
		}
		else
		{
			printf("\nGPF Fault %08X at: %08X\n Error: ", Context->ErrorCode, Context->SourceEIP);
		}
	}
	if(Context->InterruptNumber == 0x0e)
	{
		printf("\nPage Fault from: %08X\n", Context->SourceEIP);
		uint32_t CR2 = ReadCR2();
		printf("Address: %08X\n", CR2);

		if(Context->ErrorCode & 0x01)
			printf("page protection, ");
		else
			printf("non-present page, ");

		if(Context->ErrorCode & 0x02)
			printf("write, ");
		else
			printf("read, ");

		if(Context->ErrorCode & 0x04)
			printf("user");
		else
			printf("supervisor");

		if(Context->ErrorCode & 0x08)
			printf(", reserved bit");

		if(Context->ErrorCode & 0x10)
			printf(", instruction fetch");

		printf(" (%08X)\n", Context->ErrorCode);

		MMUManager->PrintAddressInformation(CR2);
	}
	else
	{	
		printf("\nSystem Error: %02X (%08X) at %08X\n", Context->InterruptNumber, Context->ErrorCode, Context->SourceEIP);
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
	ThreadInformation *CurrentThread = reinterpret_cast<ThreadInformation *>(__readfsdword(8));
	
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
			mov [esi].SavedESP, esp
			mov esp, [edi].SavedESP
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

		//NextThread->StartingData = reinterpret_cast<uintptr_t *>(0x12345678);

		uint16_t OldFS;
		ASM_SCR(fs, OldFS);
		GDTManager::UpdateGDTEntry(OldFS, reinterpret_cast<uint32_t>(NextThread), sizeof(ThreadInformation), GDT::Present | GDT::Operand32Bit | GDT::NonSystemFlag, GDT::DataSegment, 3);
		ASM_LCR(fs, OldFS);
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
	OutPortb(0x20, 0x20);

	ThreadInformation *CurrentThread = reinterpret_cast<ThreadInformation *>(__readfsdword(8));

	ThreadPtr Call = reinterpret_cast<ThreadPtr>(CurrentThread->StartingPoint);

	Call(CurrentThread, CurrentThread->StartingData);

	while(true)
		;
}


extern "C" void MultiBootMain(void *Address, uint32_t Magic)
{
	// At this point we are officially alive, but we're still a long ways away from being up and running.

	// The terminal for the moment is fairly easy.
	RawTerminal TextTerm(0xB8000);
	TextTerm.Clear();
	CurrentTerminal = &TextTerm;

	// Parse the MultiBoot info
	MultiBootInfo MBReader;
	MBReader.LoadMultiBootInfo(Magic, Address);
	MultiBootHeader = &MBReader;

	// Get the memory subsystem working
	// Start at the 256 meg mark and give it 16 megs of memory with 16 byte blocks
	RawMemory MemorySystem(0x10000000, 0x100000, 0x10);
	MemoryMgr = &MemorySystem;

	// At this point we should be able to display text and allocate memory.

	// Get the execution environment running
	_atexit_init();
	_initterm();

	// So now lets try to get the rest of the world up and running.

	// Step 1: Set our GDT
	GDTManager GDTData;
	// The NULL Segment is automatically added for us
	uint16_t CodeSegment = GDTData.AddGDTEntry(0, 0xFFFFFFFF, GDT::Present | GDT::Operand32Bit | GDT::NonSystemFlag, GDT::CodeSegment, 0);
	uint16_t DataSegment = GDTData.AddGDTEntry(0, 0xFFFFFFFF, GDT::Present | GDT::Operand32Bit | GDT::NonSystemFlag, GDT::DataSegment, 0);
	uint16_t ThreadSegment = GDTData.AddGDTEntry(0, sizeof(ThreadInformation), GDT::Present | GDT::Operand32Bit | GDT::NonSystemFlag, GDT::DataSegment, 3);
	GDTData.SetActive(CodeSegment, DataSegment);
	
	ASM_LCR(fs, ThreadSegment);

	// Step 2: Set our IDT
	IDTManager IDTData(CodeSegment, DataSegment);

	// Set up the handlers for System errors
	for(int x = 0; x < 0x20; x++)
	{
		IDTData.SetInterupt(x, SystemTrap);
	}

	// Step 3: Remap IRQs
	m_InterruptControler.RemapIRQBase(0x20);

	m_InterruptControler.SetIDT(0x20, &IDTData);
	m_InterruptControler.SetIRQInterrupt(0x01, KeyboardInterrupt);

	IDTData.SetActive();
	
	MMU Memory;
	MMUManager = &Memory;

	// Create the Idle Thread
	ThreadListHead = reinterpret_cast<ThreadInformation *>(0x20000000);
	ThreadListHead->Prev = 0;
	ThreadListHead->Next = 0;

	ThreadListHead->RealAddress = reinterpret_cast<uintptr_t *>(ThreadListHead);
	ThreadListHead->ThreadID = reinterpret_cast<uint32_t>(ThreadListHead);// 0;
	ThreadListHead->State = PreCreate;
	ThreadListHead->TimeSliceCount = 0;
	ThreadListHead->TimeSliceAllocation = 10;
	ThreadListHead->TickCount = 0;

	ThreadListHead->SavedESP = 0;
	ThreadListHead->StackLimit = 0x8000;

	ThreadListHead->StartingPoint = reinterpret_cast<uint32_t>(IdleThread); 
	ThreadListHead->StartingData = nullptr;

	ThreadListHead->Suspended = false;
	
	int StackSize = ThreadListHead->StackLimit / 4;

	StackSize--;
	ThreadListHead->Stack[StackSize] = reinterpret_cast<uint32_t>(ThreadStart); // IP

	StackSize--;
	ThreadListHead->Stack[StackSize] = reinterpret_cast<uint32_t>(&(ThreadListHead->Stack[StackSize + 1])); // The old BP
	ThreadListHead->SavedESP = reinterpret_cast<uint32_t>(&(ThreadListHead->Stack[StackSize]));

	StackSize -= 10;
	ThreadListHead->Stack[StackSize] = ThreadListHead->SavedESP;

	ThreadListHead->SavedESP = reinterpret_cast<uint32_t>(&(ThreadListHead->Stack[StackSize]));

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

	MyThread->InsertLast(ThreadListHead);

	GDTData.UpdateGDTEntry(ThreadSegment, reinterpret_cast<uint32_t>(MyThread), sizeof(ThreadInformation), GDT::Present | GDT::Operand32Bit | GDT::NonSystemFlag, GDT::DataSegment, 3);
	ASM_LCR(fs, ThreadSegment);

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

	OpenHCI USB;
	if(USBID != 0xFFFFFFFF)
	{
		USB.StartUp(USBID, &m_InterruptControler);
		USBManager = &USB;
	}

	// Step 6: Start the full kernel
	const char * CommandLine = MBReader.CommandLine;

	//printf("%08x\n", Address);
	
	//_ConvertCommandLineToArgcArgv(
	char * argv[2];
	argv[0] = "BootDebug";
	argv[1] = nullptr;
	
	//printf("Hi!");

	for(;;)
		main(1, argv);
	
	//printf("BYe!");

	for(;;) ASM_HLT;

	//printf("Woops!");
}


