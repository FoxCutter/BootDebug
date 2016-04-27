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

void GDTManager::UpdateGDTEntry(uint16_t Selector, uint32_t Base, uint32_t Limit, uint16_t Attributes, uint8_t Type, uint8_t DPL)
{
	DescriptiorData::TablePtr OldValue;
	ASM_SGDT(OldValue);

	int MaxSelector = (OldValue.Limit + 1) / sizeof(DescriptiorData::TableEntry);
	DescriptiorData::TableEntry *Table = reinterpret_cast<DescriptiorData::TableEntry *>(OldValue.Address);

	uint16_t CleanSelector = Selector >> 3;
	if(CleanSelector == 0 || CleanSelector >= MaxSelector)
		return;
	
	BuildMemoryEntry(&Table[CleanSelector], Base, Limit, Attributes, Type, DPL);
}

