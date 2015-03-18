#include "GDT.h"
#include <stdio.h>
#include <string.h>
#include "LowLevel.h"

GDTManager::GDTManager()
{
	memset(GDTTable, 0, sizeof(GDT::GDTEntry) * TableSize);
		
	m_NextFreeSlot = 0;

	AddGDTEntry(0, 0, 0, 0, 0);
}

GDT::GDTPtr GDTManager::SetActive(uint16_t NewCodeSelector, uint16_t NewDataSelector)
{
	GDT::GDTPtr TablePtr;
	
	TablePtr.Address = (uint32_t)&GDTTable;
	TablePtr.Limit = (m_NextFreeSlot * sizeof(GDT::GDTEntry)) - 1;

	GDT::GDTPtr OldValue;

	ASM_SGDT(OldValue);
	
	ASM_LGDT(TablePtr);
	
	SwapSelectors(NewCodeSelector, NewDataSelector);
	
	return OldValue;
}

uint16_t GDTManager::AddGDTEntry(uint32_t Base, uint32_t Limit, uint16_t Attributes, uint8_t Type, uint8_t DPL)
{
	if(m_NextFreeSlot == TableSize)
		return 0;

	uint16_t Selector = m_NextFreeSlot << 3;
	
	BuildGDTEntry(&GDTTable[m_NextFreeSlot], Base, Limit, Attributes, Type, DPL);

	m_NextFreeSlot ++;

	return Selector;
}


void GDTManager::UpdateGDTEntry(uint16_t Selector, uint32_t Base, uint32_t Limit, uint16_t Attributes, uint8_t Type, uint8_t DPL)
{
	GDT::GDTPtr OldValue;
	ASM_SGDT(OldValue);

	int MaxSelector = (OldValue.Limit + 1) / sizeof(GDT::GDTEntry);
	GDT::GDTEntry *Table = reinterpret_cast<GDT::GDTEntry *>(OldValue.Address);

	uint16_t CleanSelector = Selector >> 3;
	if(CleanSelector == 0 || CleanSelector >= MaxSelector)
		return;
	
	BuildGDTEntry(&Table[CleanSelector], Base, Limit, Attributes, Type, DPL);
}

void GDTManager::BuildGDTEntry(GDT::GDTEntry *Entry, uint32_t Base, uint32_t Limit, uint16_t Attributes, uint8_t Type, uint8_t DPL)
{
	Entry->Attributes = Attributes;
	
	Entry->BaseLow  = (Base & 0x0000FFFF);
	Entry->BaseMid  = (Base & 0x00FF0000) >> 16;
	Entry->BaseHigh = (Base & 0xFF000000) >> 24;

	if(Limit > 0xFFFFF)
	{
		Entry->Attributes |= GDT::Granularity4k;
		Limit = Limit / 0x1000;
	}

	Entry->LimitLow    = (Limit & 0x0000FFFF);
	Entry->Attributes |= (Limit & 0x000F0000) >> 8;

	Entry->Attributes |= (Type & GDT::TypeMask);
	Entry->Attributes |= (DPL << 4);
}

void GDTManager::PrintSelector(uint16_t Selector)
{
	if(Selector >= m_NextFreeSlot)
		return;
	
	GDT::GDTEntry *Entry = &GDTTable[Selector];
	
	unsigned int Base = 0;
	unsigned int Limit = 0;

	Base = Entry->BaseLow;
	Base |= Entry->BaseMid << 16;
	Base |= Entry->BaseHigh << 24;

	Limit = Entry->LimitLow;
	Limit |= (Entry->Attributes & GDT::LimitHighMask) << 8;

	if(Entry->Attributes & GDT::Granularity4k)
	{
		Limit *= 0x1000;
		Limit += 0x0FFF;
	}

	printf("Base:       %08X\n", Base);
	printf("Limit:      %08X\n", Limit);
	printf("DPL:        ");

	switch(Entry->Attributes & GDT::DPLMask)
	{
		case GDT::DPL0:
			printf("0\n");
			break;

		case GDT::DPL1:
			printf("1\n");
			break;

		case GDT::DPL2:
			printf("2\n");
			break;

		case GDT::DPL3:
			printf("3\n");
			break;
	}

	printf("Attributes: ");

	if(Entry->Attributes & GDT::Accessed)
		printf("Accessed ");

	if(Entry->Attributes & GDT::ReadWrite)
		printf("R/W ");

	if(Entry->Attributes & GDT::DirectionConforming)
		printf("D/C ");

	if(Entry->Attributes & GDT::Executable)
		printf("Executable ");

	if(Entry->Attributes & GDT::NonSystemFlag)
		printf("Non-System ");

	if(Entry->Attributes & GDT::Present)
		printf("Present ");

	if(Entry->Attributes & GDT::Operand32Bit)
		printf("32Bit ");

	if(Entry->Attributes & GDT::Granularity4k)
		printf("4k ");

	printf("\n", Entry->Attributes);

}