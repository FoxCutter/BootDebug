#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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

#include "..\StdLib\initterm.h"

void main(int argc, char *argv[]);
extern MultiBootInfo * MultiBootHeader;
extern MMU * MMUManager;

void SystemTrap(InterruptContext * Context)
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
	ASM_HLT;

	for(;;);
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
	while(KBBufferFirst == KBBufferLast);
	char value = KBBuffer[KBBufferFirst];
		
	KBBufferFirst++;
	if(KBBufferFirst == 32)
		KBBufferFirst = 0;

	return value;
}

void IRQInterrupt(InterruptContext * Context)
{
	// Keyboard
	if(Context->InterruptNumber == 0x021)
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
	else if(Context->InterruptNumber == 0x02B)
	{
		// USB
		printf("USB Int\n");
	}

	m_InterruptControler.ClearIRQ(m_InterruptControler.MapIntToIRQ(Context->InterruptNumber));
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
	uint16_t CodeSegment = GDTData.AddGDTEntry(0, 0xFFFFFFFF, GDT::Present | GDT::Operand32Bit, GDT::CodeSegment, 0);
	uint16_t DataSegment = GDTData.AddGDTEntry(0, 0xFFFFFFFF, GDT::Present | GDT::Operand32Bit, GDT::DataSegment, 0);
	GDTData.SetActive(CodeSegment, DataSegment);
	
	// Step 2: Set our IDT
	IDTManager IDTData(CodeSegment, DataSegment);

	// Set up the handlers for System errors
	for(int x = 0; x < 0x20; x++)
	{
		IDTData.SetInterupt(x, SystemTrap);
	}

	// Step 3: Remap IRQs
	m_InterruptControler.RemapIRQBase(0x20);

	// Set up the handlers for IRQs
	for(int x = 0x20; x < 0x30; x++)
	{
		IDTData.SetInterupt(x, IRQInterrupt);
	}

	IDTData.SetActive();
	
	MMU Memory;
	MMUManager = &Memory;

	ASM_STI;
	
	//PCI PCIBus;
	//uint32_t USBID = PCIBus.FindDeviceID(0x0C, 0x03, 0x10);

	//OpenHCI USB;
	//if(USBID != 0xFFFFFFFF)
	//	USB.StartUp(USBID);
	
	//PCI PCIBus;
	//PCIBus.Initilize();
	//PCIBios32.Initilize();

	//Registers Reg;
	//ReadCPUID(0, 0, &Reg);

	//printf("CPUID EAX:%08X EBX:%08X ECX:%08X EDX:%08X\n", Reg.EAX, Reg.EBX, Reg.ECX, Reg.EDX);
	
	//ACPI ACPIObject;
	//ACPIObject.Initilize();

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

	for(;;);

	//printf("Woops!");
}


