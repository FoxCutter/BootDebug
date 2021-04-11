#include "IDT.h"
#include "KernalLib.h"

#include <string.h>
#include "LowLevel.h"

extern "C" 
{
	extern uint16_t IntData;
	extern uint16_t GlobalData;
	extern uint16_t ThreadData;
	extern uint32_t IntCallback;
	extern uint32_t IntTable[];

	extern void Default_Interrupt();
}

uint8_t DataBlock[128];

InterruptData *InterruptCallbackTable = nullptr;

extern "C" void HandleInterrupt(InterruptContext * Context)
{
	if(InterruptCallbackTable == nullptr)
		return;
	
	// Forward the Interrupt to the correct function
	if(InterruptCallbackTable[Context->InterruptNumber].InterruptCallback != nullptr)
		InterruptCallbackTable[Context->InterruptNumber].InterruptCallback(Context, InterruptCallbackTable[Context->InterruptNumber].Data);
	//else
		//KernalPrintf("!INT %02X ", Context->InterruptNumber);

	return;
}

IDTManager::IDTManager()
{
}


void IDTManager::Initilize(uint16_t CodeSelector, uint16_t DataSelector, uint16_t GlobalSelector, uint16_t ThreadSelector)
{
	DescriptorTable::Initilize(256, true);

	m_CodeSelector = CodeSelector;
	m_DataSelector = DataSelector;
	m_GlobalSelector = GlobalSelector;
	m_ThreadSelector = ThreadSelector;

	GlobalData = GlobalSelector;
	ThreadData = ThreadSelector;
	IntData = DataSelector;
	IntCallback = (uint32_t)HandleInterrupt;
	
	memset(InterruptCallback, 0, sizeof(InterruptData) * 256);
	
	for(int x = 0; x < 256; x++)
	{
		BuildGateEntry(GetEntry(x), CodeSelector, reinterpret_cast<uint32_t>(&Default_Interrupt), 0, true, DescriptiorData::IntGate32BitSegment, 0);
	}

	//KernalPrintf("!INT %08X ", IntCallback);
}

void IDTManager::SetInterupt(unsigned int IntNum, InterruptCallbackPtr CallBack, uintptr_t *Data)
{
	if(IntNum > 255)
		return;

	InterruptCallback[IntNum].InterruptCallback = CallBack;
	InterruptCallback[IntNum].Data = Data;

	BuildGateEntry(GetEntry(IntNum), m_CodeSelector, IntTable[IntNum], 0, true, DescriptiorData::IntGate32BitSegment, 0);
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

		DescriptiorData::TableEntry * Entry = GetEntry(IntNum);

		Entry->Type = Type;
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

		DescriptiorData::TableEntry * Entry = GetEntry(IntNum);

		KernalPrintf("DPL %02X ", Entry->DPL);
		
		Entry->DPL = DPL;
	
		KernalPrintf("DPL %02X ", Entry->DPL);

	}
	ASM_STI
}

DescriptiorData::TablePtr IDTManager::SetActive()
{
	DescriptiorData::TablePtr  TablePtr;
	
	PopulateTablePointer(TablePtr);

	DescriptiorData::TablePtr  OldValue;

	ASM_SIDT(OldValue);
	
	InterruptCallbackTable = InterruptCallback;

	ASM_LIDT(TablePtr);
	
	return OldValue;
}


void IDTManager::Dump()
{
	for(int x = 0; x < 256; x++)
	{
		if(InterruptCallback[x].InterruptCallback != nullptr)
		{
			KernalPrintf(" %02X", x);

			PrintSelector(GetEntry(x));

			KernalPrintf(" PTR=%08X", InterruptCallback[x].InterruptCallback);
			KernalPrintf("\n");
		}
	}
}