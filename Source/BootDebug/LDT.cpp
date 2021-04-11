#include "LDT.h"
#include "LowLevel.h"
#include "KernalLib.h"

LDTManager::LDTManager(void)
{
	m_Selector = 0;
}


LDTManager::~LDTManager(void)
{
}

void LDTManager::Initilize(DescriptorTable &GDT)
{
	DescriptorTable::Initilize(0x200);

	// Set up LDT 0 to cover 16-bit Memeory 
	AddMemoryEntry(0, 0x10FFEF, DescriptiorData::Present | DescriptiorData::Operand32Bit | DescriptiorData::NonSystemFlag, DescriptiorData::DataReadWrite, 3);

	m_Selector = GDT.AddTableEntry(*this, 3);
}

uint16_t LDTManager::SetActive()
{
	uint16_t RetVal;

	RetVal = SetLDT(m_Selector);

	return RetVal;
}

void LDTManager::Dump()
{
	for(int x = 0; x < m_NextFreeSlot; x++)
	{
		KernalPrintf(" %04X", (x * 8) | 0x04);
		PrintSelector(&m_Table[x]);
		KernalPrintf("\n");
	}
}