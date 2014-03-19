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

#include "..\StdLib\initterm.h"

void SystemTrap(InterruptContext * Context)
{
	if(Context->InterruptNumber == 0x0d)
	{
		printf("GPF Fault at: %08X\n", Context->SourceEIP);
	}
	else
	{	
		printf("System Error: %02X (%08X) at %08X\n", Context->InterruptNumber, Context->ErrorCode, Context->SourceEIP);
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

char KBBuffer[32];
uint8_t KBBufferFirst = 0;
uint8_t KBBufferLast = 0;

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

	m_InterruptControler.ClearIRQ(m_InterruptControler.MapIntToIRQ(Context->InterruptNumber));
}

uint32_t FindMemory(uint32_t Start, uint32_t Count, char *Search)
{
	int SearchLength = strlen(Search);
	if(SearchLength == 0)
		return UINT32_MAX;

	char *Data = (char *)Start;

	int SeachState = 0;

	for(int Pos = 0; Pos != Count; Pos++)
	{
		if(Data[Pos] == Search[SeachState])
		{
			SeachState ++;
			if(SeachState == SearchLength)
			{
				return Start + Pos - SearchLength + 1;
			}
		}
		else
		{
			SeachState = 0;
		}
	}

	return UINT32_MAX;
}

struct BIOSServiceDirectory
{
	uint32_t	Magic;
	uint32_t	EntryPoint;
	uint8_t		HeaderVersion;
	uint8_t		HeaderLength; // in Paragraphs
	uint8_t		Checksum;
	uint8_t		Reserved[5];
};


void main(int argc, char *argv[]);
MultiBootInfo MBReader;
extern "C" void MultiBootMain(void *Address, uint32_t Magic)
{
	// At this point we are officially alive, but we're still a long ways away from being up and running.

	// The terminal for the moment is fairly easy.
	RawTerminal TextTerm(0xB8000);
	CurrentTerminal = &TextTerm;

	// Parse the MultiBoot info
	//MultiBootInfo MBReader;
	MBReader.LoadMultiBootInfo(Magic, Address);

	// Get the memory subsystem working
	// Lets just start with a good area and 16 byte blocks
	RawMemory MemorySystem(0x20000, 0x40000, 0x10);
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
	
	ASM_STI;
	
	// Okay, at this point the system is up and running as best as it can be expected to be. 
	//uint32_t Bios32Adder = FindMemory(0xE0000, 0x20000, "_32_");
	//
	//BIOSServiceDirectory * Bios32Dir = (BIOSServiceDirectory *)Bios32Adder;
	//printf("%08X, %08X\n", Bios32Adder, Bios32Dir->EntryPoint);
	//
	//Registers Reg;
	//Reg.EAX = 0x49435024;
	//Reg.ECX = 0;
	//Reg.EBX = 0;
	//Reg.EDX = 0;
	//printf("A:%08X B:%08X C:%08X D:%08X\n", Reg.EAX, Reg.EBX, Reg.ECX, Reg.EDX);

	//CallService(Bios32Dir->EntryPoint, &Reg);
	//
	//printf("A:%08X B:%08X C:%08X D:%08X\n", Reg.EAX, Reg.EBX, Reg.ECX, Reg.EDX);

	// Step 6: Start the full kernel
	const char * CommandLine = MBReader.CommandLine;

	printf("%08x\n", Address);
	
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
