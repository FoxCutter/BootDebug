#include "GDT.h"
#include "LowLevel.h"

GDTManager::GDTManager()
{
}

void GDTManager::Initilize(uint32_t BaseAddress)
{
	SetTable(reinterpret_cast<DescriptiorData::TableEntry *>(BaseAddress));
	DescriptorTable::Initilize(0x10);

	// Add the null selector
	AddMemoryEntry(0, 0, 0, 0, 0);
}

DescriptiorData::TablePtr GDTManager::SetActive(uint16_t NewCodeSelector, uint16_t NewDataSelector)
{
	DescriptiorData::TablePtr TablePtr;
	
	PopulateTablePointer(TablePtr);

	DescriptiorData::TablePtr OldValue;

	ASM_SGDT(OldValue);
	
	ASM_LGDT(TablePtr);
	
	SwapSelectors(NewCodeSelector, NewDataSelector);
	
	return OldValue;
}

