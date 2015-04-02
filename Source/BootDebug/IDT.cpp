#include "IDT.h"
#include <stdio.h>
#include <string.h>
#include "LowLevel.h"

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

		Entry.Attributes |= ~IDT::TypeMask;
	
		Entry.Attributes |= (Type & IDT::TypeMask);
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

	Entry->Attributes |= (Type & IDT::TypeMask);
	Entry->Attributes |= (DPL << 4);
}
