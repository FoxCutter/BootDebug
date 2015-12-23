#include "IDT.h"
#include <stdio.h>
#include <string.h>
#include "LowLevel.h"

/*
	There has to be a better way to handle interupts then what I have here, there is way to much dumy code and layers to make things 
	work and I want to fix it.

	* Throw out the usless 'error code' and 'interrupt number' in the context. The Error code is only used on 08, 10 - 14 and 17 (and sometimes 18), 
	  so they are the only ones that should care about it.
	* Dump the Callback table, handlers will be called directly
	* There will be a 'generic' functions for calling ints.
	* When adding a new handler the generic function will be copied in system memory and patched with the correct function address.
	* Also patched will be the extra 'pop' needed for functions that take error code, removing it in the majority of cases
	* There will be one 'invalid' interrupt handler (also a patched generic) that will be default value for all IDL entries.
	* This does lose the Context Data associated with Interrupts, but that is acceptable as we are now being specific with what gets called when
*/


extern "C" 
{
	extern uint16_t IntData;
	extern uint32_t IntCallback;
	extern uint32_t IntTable[];
}

InterruptData *InterruptCallbackTable = nullptr;

extern "C" void HandleInterrupt(InterruptContext * Context)
{
	if(InterruptCallbackTable == nullptr)
		return;
	
	// Forward the Interrupt to the correct function
	if(InterruptCallbackTable[Context->InterruptNumber].InterruptCallback != nullptr)
		InterruptCallbackTable[Context->InterruptNumber].InterruptCallback(Context, InterruptCallbackTable[Context->InterruptNumber].Data);
	else
		printf("INT %02X ", Context->InterruptNumber);

	return;
}

IDTManager::IDTManager()
{
}

void IDTManager::SetSelectors(uint16_t CodeSelector, uint16_t DataSelector)
{
	IntData = DataSelector;
	IntCallback = (uint32_t)HandleInterrupt;
	
	memset(IDTTable, 0, sizeof(IDT::IDTEntry) * 256);
	memset(InterruptCallback, 0, sizeof(InterruptData) * 256);

	int Pos = 0;
	while(IntTable[Pos] != 0)
	{
		BuildIDTEntry(&IDTTable[Pos], CodeSelector, IntTable[Pos], IDT::Present, IDT::InteruptGate_32Bit, 0);
		Pos++;
	}
}

void IDTManager::SetInterupt(unsigned int IntNum, InterruptCallbackPtr CallBack, uintptr_t *Data)
{
	if(IntNum > 255)
		return;

	InterruptCallback[IntNum].InterruptCallback = CallBack;
	InterruptCallback[IntNum].Data = Data;
}

void IDTManager::SetInterupt(unsigned int IntNum, InterruptCallbackPtr CallBack, uint8_t Type, uintptr_t *Data)
{
	if(IntNum > 255)
		return;

	// As we are directly tweaking the IDT we're going to turn off interrupts for the moment.	
	ASM_CLI
	{
		InterruptCallback[IntNum].InterruptCallback = CallBack;
		InterruptCallback[IntNum].Data = Data;

		IDT::IDTEntry & Entry = IDTTable[IntNum];

		Entry.Attributes &= ~IDT::TypeMask;
	
		Entry.Attributes |= (Type & IDT::TypeMask);
	}
	ASM_STI
}

void IDTManager:: SetInteruptDPL(unsigned int  IntNum, InterruptCallbackPtr CallBack, uint8_t DPL, uintptr_t * Data)
{
	if(IntNum > 255)
		return;

	// As we are directly tweaking the IDT we're going to turn off interrupts for the moment.	
	ASM_CLI
	{
		InterruptCallback[IntNum].InterruptCallback = CallBack;
		InterruptCallback[IntNum].Data = Data;

		IDT::IDTEntry & Entry = IDTTable[IntNum];

		printf("ATR %02X ", Entry.Attributes);
		
		Entry.Attributes &= ~IDT::DPLMask;
		Entry.Attributes |= DPL << 5;
	
		printf("ATR %02X\n", Entry.Attributes);

	}
	ASM_STI
}

IDT::IDTPtr IDTManager::SetActive()
{
	IDT::IDTPtr TablePtr;
	
	TablePtr.Address = (uint32_t)&IDTTable;
	TablePtr.Limit = (256 * sizeof(IDT::IDTEntry)) - 1;

	IDT::IDTPtr OldValue;

	ASM_SIDT(OldValue);
	
	InterruptCallbackTable = InterruptCallback;

	ASM_LIDT(TablePtr);
	
	return OldValue;
}

void IDTManager::BuildIDTEntry(IDT::IDTEntry *Entry, uint16_t Segment, uint32_t Offset, uint8_t Attributes, uint8_t Type, uint8_t DPL)
{
	Entry->Attributes = Attributes;
	
	Entry->OffsetLow  = (Offset & 0x0000FFFF);
	Entry->OffsetHigh = (Offset & 0xFFFF0000) >> 16;
	Entry->Segment = Segment;
	Entry->Reserved = 0;

	Entry->Attributes |= (Type & IDT::TypeMask);
	Entry->Attributes |= (DPL << 4);
}
