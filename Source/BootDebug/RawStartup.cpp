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
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8',	/* 9 */
  '9', '0', '-', '=', '\b',	/* Backspace */
  '\t',			/* Tab */
  'q', 'w', 'e', 'r',	/* 19 */
  't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',	/* Enter key */
    0,			/* 29   - Control */
  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',	/* 39 */
 '\'', '`',   0,		/* Left shift */
 '\\', 'z', 'x', 'c', 'v', 'b', 'n',			/* 49 */
  'm', ',', '.', '/',   0,				/* Right shift */
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

void IRQInterrupt(InterruptContext * Context)
{
	// Keyboard
	if(Context->InterruptNumber == 0x021)
	{
		unsigned char scancode = InPortb(0x60);		
		//printf("%02X ", scancode);
		if(scancode & 0x80)
		{
		}
		else
		{
			//printf("%02X ", scancode);
			//printf("%c", kbdus[scancode]);

/*			if (kbdus[scancode] == 27)
			{
				if(KBBufferFirst != KBBufferLast)
				{
					KBBufferFirst++;
					if(KBBufferFirst == 32)
						KBBufferFirst = 0;
				}
			}
			else */
			if((KBBufferLast + 1) == KBBufferFirst || KBBufferFirst == 0 && (KBBufferLast + 1 == 32))
			{
				//printf("Full!");
			}
			else
			{
				KBBuffer[KBBufferLast] = kbdus[scancode];
				KBBufferLast++;
				
				if(KBBufferLast == 32)
					KBBufferLast = 0;
			}

			//printf("%i, %i, %c\n", KBBufferFirst, KBBufferLast, kbdus[scancode]);
		}
	}

	m_InterruptControler.ClearIRQ(m_InterruptControler.MapIntToIRQ(Context->InterruptNumber));
}

void main(int argc, char *argv[]);

extern "C" void MultiBootMain(void *Address, uint32_t Magic)
{
	// At this point we are officially alive, but we're still a long ways away from being up and running.

	// The terminal for the moment is fairly easy.
	RawTerminal TextTerm(0xB8000);
	CurrentTerminal = &TextTerm;

	// Parse the MultiBoot info
	MultiBootInfo MBReader;
	MBReader.LoadMultiBootInfo(Magic, Address);

	// Get the memory subsystem working

	// Lets just start with most of the low memory area and use 16 byte blocks
	RawMemory MemorySystem(0x1000, 0x60000, 0x10);
	MemoryMgr = &MemorySystem;

	// At this point we should be able to display text and allocate memory.

	// Get the execution environment running
	_atexit_init();
	_initterm();

	// So now lets try to get the rest of the world up and running.

	// Step 1: Set our GDT
	GDTManager GDTData;
	// The NULL Segment is automatically added for us
	GDTData.AddGDTEntry(0, 0xFFFFFFFF, GDT::Present | GDT::Operand32Bit, GDT::CodeSegment, 0);
	GDTData.AddGDTEntry(0, 0xFFFFFFFF, GDT::Present | GDT::Operand32Bit, GDT::DataSegment, 0);
	GDTData.SetActive(0x08, 0x10);
	
	// Step 2: Set our IDT
	IDTManager IDTData(0x08, 0x10);

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

	// Step 6: Start the full kernel
	const char * CommandLine = MBReader.CommandLine;

	//_ConvertCommandLineToArgcArgv(
	char * argv[2];
	argv[0] = "BootDebug";
	argv[1] = nullptr;
	
	//printf("Hi!");

	main(1, argv);
	
	//printf("BYe!");


	for(;;);

	//printf("Woops!");
}
