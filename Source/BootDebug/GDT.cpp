#include "GDT.h"
#include <stdio.h>
#include <string.h>
#include "LowLevel.h"

GDTManager::GDTManager()
{
	memset(GDTTable, 0, sizeof(GDT::GDTEntry) * MaxTableSize);
		
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
	if(m_NextFreeSlot == MaxTableSize)
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
	Entry->NonSystem = (Attributes & GDT::NonSystemFlag) == GDT::NonSystemFlag;
	Entry->Present = (Attributes & GDT::Present) == GDT::Present;
	Entry->Avaliable = (Attributes & GDT::AvaliableBit) == GDT::AvaliableBit;
	Entry->Big = (Attributes & GDT::Operand32Bit) == GDT::Operand32Bit;

	Entry->Long = false;
	Entry->BaseLow  = (Base & 0x0000FFFF);
	Entry->BaseMid  = (Base & 0x00FF0000) >> 16;
	Entry->BaseHigh = (Base & 0xFF000000) >> 24;

	if(Limit > 0xFFFFF)
	{
		Entry->Granularity = true;
		Limit = Limit / 0x1000;
	}
	else
	{
		Entry->Granularity = false;
	}

	Entry->LimitLow    = (Limit & 0x0000FFFF);
	Entry->LimitHigh  |= (Limit & 0x000F0000) >> 16;

	Entry->Type = Type;
	Entry->DLP  = DPL;
}

void GDTManager::Dump()
{
	for(uint16_t x = 0; x < m_NextFreeSlot; x ++)
		PrintSelector(x);
}

void GDTManager::PrintSelector(uint16_t Selector)
{
	if(Selector >= m_NextFreeSlot)
		return;
	
	GDT::GDTEntry *Entry = &GDTTable[Selector];

	printf(" %04X", Selector * 8);

	if(Entry->Present)
		printf(" P");
	else
		printf("  ");
	
	bool CallGate = false;

	if(Entry->NonSystem)
	{
		if(Entry->Type & GDT::Executable)
		{
			if(Entry->Big)
				printf(" 32-Bit Code      E");
			else
				printf(" 16-Bit Code      E");
			
			if(Entry->Type & GDT::ReadWrite)
				printf("R");
			else
				printf("-");

			if(Entry->Type & GDT::DirectionConforming)
				printf("C");
			else
				printf("-");

		}
		else
		{

			if(Entry->Big)
				printf(" 32-Bit Data      R");
			else
				printf(" 16-Bit Data      R");

			if(Entry->Type & GDT::ReadWrite)
				printf("W");
			else
				printf("-");

			if(Entry->Type & GDT::DirectionConforming)
				printf("D");
			else
				printf("-");
		}

		if(Entry->Type & GDT::Accessed)
			printf("A");
		else
			printf("-");
	}
	else
	{
		switch(Entry->Type)
		{
			case GDT::TSS16BitSegment:
				printf(" 16-Bit TSS       ----");
				break;

			case GDT::LDTSegment:
				printf("        LDT       ----");
				break;

			case GDT::TSS16BitSegmentBusy:
				printf(" 16-Bit TSS       B---");
				break;

			case GDT::CallGate16BitSegment:
				printf(" 16-Bit Call Gate ----");
				CallGate = true;
				break;

			case GDT::TaskGateSegment:
				printf("        Task Gate ----");
				CallGate = true;
				break;

			case GDT::IntGate16BitSegment:
				printf(" 16-Bit Int Gate- ----");
				CallGate = true;
				break;

			case GDT::TrapGate16BitSegment:
				printf(" 16-Bit Trap Gate ----");
				CallGate = true;
				break;

			case GDT::TSS32BitSegment:
				printf(" 32-Bit TSS       ----");
				break;

			case GDT::TSS32BitSegmentBusy:
				printf(" 32-Bit TSS       B---");
				break;

			case GDT::CallGate32BitSegment:
				printf(" 32-Bit Call Gate ----");
				CallGate = true;
				break;

			case GDT::IntGate32BitSegment:
				printf(" 32-Bit Int Gate  ----");
				CallGate = true;
				break;

			case GDT::TrapGate32BitSegment:
				printf(" 32-Bit Trap Gate ----");
				CallGate = true;
				break;

			default:
				printf("        Reserved  ----");
				break;
		}

	}

	printf(" DPL=%X", Entry->DLP);
	
	if(!Entry->Present)
	{
		printf("  AVL = %08X-%02X-%04X\n", Entry->Avaliable1, Entry->Avaliable2, Entry->Avaliable3);
		return;
	}

	if(CallGate)
	{
		printf(" Selector = %04X", Entry->Selector);
		if(Entry->Type != GDT::TaskGateSegment)
			printf(" Offset = %04X%04X", Entry->OffsetHigh, Entry->OffsetLow);
		else
			printf("                  ");
	}
	else
	{	
		unsigned int Base = 0;
		unsigned int Limit = 0;

		Base = Entry->BaseLow;
		Base |= Entry->BaseMid << 16;
		Base |= Entry->BaseHigh << 24;

		Limit = Entry->LimitLow;
		Limit |= Entry->LimitHigh << 16;

		if(Entry->Granularity)
		{
			Limit *= 0x1000;
			Limit += 0x0FFF;
		}
	
		printf(" Base = %08X", Base);
		printf(" Limit = %08X ", Limit);
	}
	
	if(Entry->Granularity)
		printf(" G");
	else
		printf(" -");

	if(!CallGate)
		printf(" AVL=%X", Entry->Avaliable);

	printf("\n");
}