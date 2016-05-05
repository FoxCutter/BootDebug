#include "IDT.h"
#include "KernalLib.h"

#include <string.h>
#include "LowLevel.h"

/*
	There has to be a better way to handle interupts then what I have here, there is way to much dumy code and layers to make things 
	work and I want to fix it.

	* Throw out the usless 'error code' and 'interrupt number' in the context. The Error code is only used on 08, 10 - 14 and 17 (and sometimes 18), 
	  so they are the only ones that should care about it.
	* Dump the Callback table, handlers will be called directly
	* There will be a 'generic' functions for calling ints.
	*       Generic_Interrupt to Generic_InterruptEnd
	* Fill in InterruptContext + 1 with the address of the context data
	* The last 4 bytes before Generic_InterruptEnd can be patched to remove the error code from the stack. (replace them with "add esp, 4" 83 C4 04, and "iret" CF)
	* There will be one 'invalid' interrupt handler (also a patched generic) that will be default value for all IDL entries.

	* There will be a structure that holds the meta information about an interrupt (inculding the patched function code). It will be allocated on demand.
	* It will contain the context information about the interrupt (for the int manager) and information about the code that owns it.
	* This will also contain the chained interrupts if there is more then one handler for an IRQ.

	* There are two types of core interrupt handers.
	* one that takes an Interrupt Context (Stock Generic_Interrupt)
	* One that takes an Interrupt Context with Error Code (Generic_Interrupt with Generic_InterruptEnd updated to remove the code)
*/


extern "C" 
{
	extern uint16_t IntData;
	extern uint32_t IntCallback;
	extern uint32_t IntTable[];
	extern uintptr_t Generic_Interrupt, Generic_InterruptEnd;
	extern uintptr_t InterruptSelector, InterruptCall, InterruptContextPtr;
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
	else
		KernalPrintf("!INT %02X ", Context->InterruptNumber);

	return;
}

IDTManager::IDTManager()
{
}


void IDTManager::Initilize(uint16_t CodeSelector, uint16_t DataSelector)
{
	DescriptorTable::Initilize(256, true);

	m_CodeSelector = CodeSelector;
	m_DataSelector = DataSelector;

	IntData = DataSelector;
	IntCallback = (uint32_t)HandleInterrupt;
	
	memset(InterruptCallback, 0, sizeof(InterruptData) * 256);

	int Pos = 0;
	while(IntTable[Pos] != 0)
	{
		BuildGateEntry(GetEntry(Pos), CodeSelector, IntTable[Pos], 0, true, DescriptiorData::IntGate32BitSegment, 0);
		Pos++;
	}

	KernalPrintf("!INT %08X ", IntCallback);
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
		}
	}
}