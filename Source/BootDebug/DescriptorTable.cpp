#include "DescriptorTable.h"
#include "KernalLib.h"


DescriptorTable::DescriptorTable(void)
{
	m_Table = nullptr;
	m_TableLength = 0;
	m_NextFreeSlot = 0;
}


DescriptorTable::~DescriptorTable(void)
{
}


void DescriptorTable::Initilize(uint16_t EntryCount, bool Fill)
{
	if(m_Table == nullptr)
	{
		uint32_t TableLength = EntryCount * sizeof(DescriptiorData::TableEntry);
		void * Base = KernalPageAllocate(TableLength, KernalPageFlags::Fixed, "Descriptor Table");
	
		if(Base == nullptr)
			KernalPanic(KernalCode::MemoryError, "Unable to allocated fixed memory for a Descriptor Table\n");
	
		m_Table = reinterpret_cast<DescriptiorData::TableEntry *>(Base);
	}

	m_TableLength = EntryCount;
	m_NextFreeSlot = 0;

	if(Fill)
		m_NextFreeSlot = EntryCount;

	DescriptiorData::TableEntry Filler;
	Filler.Present = false;
	Filler.Avaliable1 = 0;
	Filler.Avaliable2 = 0;
	Filler.Avaliable3 = 0;

	for(int x = 0; x < EntryCount; x++)
	{
		m_Table[x] = Filler;
	}
}

void DescriptorTable::BuildMemoryEntry(DescriptiorData::TableEntry *Entry, uint32_t Base, uint32_t Limit, uint16_t Attributes, uint8_t Type, uint8_t DPL)
{	
	Entry->NonSystem = (Attributes & DescriptiorData::NonSystemFlag) == DescriptiorData::NonSystemFlag;
	Entry->Present = (Attributes & DescriptiorData::Present) == DescriptiorData::Present;
	Entry->Avaliable = (Attributes & DescriptiorData::AvaliableBit) == DescriptiorData::AvaliableBit;
	Entry->Big = (Attributes & DescriptiorData::Operand32Bit) == DescriptiorData::Operand32Bit;
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
	Entry->DPL  = DPL;
}

void DescriptorTable::BuildGateEntry(DescriptiorData::TableEntry *Entry, uint16_t Selector, uint32_t Offset, uint8_t ParamCount, bool Present, uint8_t Type, uint8_t DPL)
{
	Entry->OffsetLow  = (Offset & 0x0000FFFF);
	Entry->OffsetHigh = (Offset & 0xFFFF0000) >> 16;
	Entry->Selector = Selector;
	Entry->ParamCount = ParamCount & 0x0F;

	Entry->Type = Type;
	Entry->NonSystem = false;
	Entry->DPL = DPL;
	Entry->Present = Present;
}

void DescriptorTable::BuildTableEntry(DescriptiorData::TableEntry *Entry, DescriptorTable & Table, uint8_t DPL)
{
	BuildMemoryEntry(Entry, reinterpret_cast<uint32_t>(Table.m_Table), Table.m_TableLength * sizeof(DescriptiorData::TableEntry), DescriptiorData::Present, DescriptiorData::LDTSegment, DPL);
}


uint16_t DescriptorTable::AddMemoryEntry(uint32_t Base, uint32_t Limit, uint16_t Attributes, uint8_t Type, uint8_t DPL)
{
	if(m_NextFreeSlot == m_TableLength)
		return 0;

	uint16_t Selector = m_NextFreeSlot << 3;
	
	BuildMemoryEntry(&m_Table[m_NextFreeSlot], Base, Limit, Attributes, Type, DPL);

	m_NextFreeSlot ++;

	return Selector;
}

uint16_t DescriptorTable::AddGateEntry(uint16_t Selector, uint32_t Offset, uint8_t ParamCount, bool Present, uint8_t Type, uint8_t DPL)
{
	if(m_NextFreeSlot == m_TableLength)
		return 0;

	uint16_t NewSelector = m_NextFreeSlot << 3;
	
	BuildGateEntry(&m_Table[m_NextFreeSlot], Selector, Offset, ParamCount, Present, Type, DPL);

	m_NextFreeSlot ++;

	return NewSelector;

}

uint16_t DescriptorTable::AddTableEntry(DescriptorTable & Table, uint8_t DPL)
{
	if(m_NextFreeSlot == m_TableLength)
		return 0;

	uint16_t Selector = m_NextFreeSlot << 3;
	
	BuildTableEntry(&m_Table[m_NextFreeSlot], Table, DPL);

	m_NextFreeSlot ++;

	return Selector;
}

DescriptiorData::TableEntry *DescriptorTable::GetEntry(uint16_t SelectorIndex)
{
	if(SelectorIndex >= m_NextFreeSlot)
		return nullptr;

	return &m_Table[SelectorIndex];
}

void DescriptorTable::UpdateMemoryEntry(uint16_t Selector, uint32_t Base, uint32_t Limit, uint16_t Attributes, uint8_t Type, uint8_t DPL)
{
	uint16_t CleanSelector = Selector >> 3;
	if(CleanSelector >= m_NextFreeSlot)
		return;
	
	BuildMemoryEntry(&m_Table[CleanSelector], Base, Limit, Attributes, Type, DPL);

}

void DescriptorTable::UpdateGateEntry(uint16_t Selector, uint16_t NewSelector, uint32_t Offset, uint8_t ParamCount, bool Present, uint8_t Type, uint8_t DPL)
{
	uint16_t CleanSelector = Selector >> 3;
	if(CleanSelector >= m_NextFreeSlot)
		return;
	
	BuildGateEntry(&m_Table[CleanSelector], NewSelector, Offset, ParamCount, Present, Type, DPL);
}

void DescriptorTable::UpdateTableEntry(uint16_t Selector, DescriptorTable & Table, uint8_t DPL)
{
	uint16_t CleanSelector = Selector >> 3;
	if(CleanSelector >= m_NextFreeSlot)
		return;

	BuildTableEntry(&m_Table[CleanSelector], Table, DPL);
}


void DescriptorTable::PopulateTablePointer(DescriptiorData::TablePtr &Pointer)
{
	Pointer.Address = reinterpret_cast<uint32_t>(m_Table);
	Pointer.Limit = (m_TableLength * sizeof(DescriptiorData::TableEntry)) - 1;
}
	

uint16_t DescriptorTable::FindFreeSelector()
{
	for(uint16_t x = 0; x < m_TableLength; x++)
	{
		if(!m_Table[x].Present)
			return x;
	}

	return -1;
}


void DescriptorTable::Dump()
{
	for(int x = 0; x < m_NextFreeSlot; x++)
	{
		KernalPrintf(" %04X", x * 8);
		PrintSelector(&m_Table[x]);
		KernalPrintf("\n");

	}
}

void DescriptorTable::PrintSelector(DescriptiorData::TableEntry *Entry)
{
	if(Entry->Present)
		KernalPrintf(" P");
	else
		KernalPrintf("  ");
	
	bool CallGate = false;

	if(Entry->NonSystem)
	{
		if(Entry->Type & DescriptiorData::Executable)
		{
			if(Entry->Big)
				KernalPrintf(" 32-Bit Code      E");
			else
				KernalPrintf(" 16-Bit Code      E");
			
			if(Entry->Type & DescriptiorData::ReadWrite)
				KernalPrintf("R");
			else
				KernalPrintf("-");

			if(Entry->Type & DescriptiorData::DirectionConforming)
				KernalPrintf("C");
			else
				KernalPrintf("-");

		}
		else
		{

			if(Entry->Big)
				KernalPrintf(" 32-Bit Data      R");
			else
				KernalPrintf(" 16-Bit Data      R");

			if(Entry->Type & DescriptiorData::ReadWrite)
				KernalPrintf("W");
			else
				KernalPrintf("-");

			if(Entry->Type & DescriptiorData::DirectionConforming)
				KernalPrintf("D");
			else
				KernalPrintf("-");
		}

		if(Entry->Type & DescriptiorData::Accessed)
			KernalPrintf("A");
		else
			KernalPrintf("-");

		if(Entry->Granularity)
			KernalPrintf("G");
		else
			KernalPrintf("-");

	}
	else
	{
		switch(Entry->Type)
		{
			case DescriptiorData::TSS16BitSegment:
				KernalPrintf(" 16-Bit TSS       ----");
				if(Entry->Granularity)
					KernalPrintf("G");
				else
					KernalPrintf("-");
				break;

			case DescriptiorData::LDTSegment:
				KernalPrintf("        LDT       ----");
				if(Entry->Granularity)
					KernalPrintf("G");
				else
					KernalPrintf("-");
				break;

			case DescriptiorData::TSS16BitSegmentBusy:
				KernalPrintf(" 16-Bit TSS       B---");
				if(Entry->Granularity)
					KernalPrintf("G");
				else
					KernalPrintf("-");
				break;

			case DescriptiorData::CallGate16BitSegment:
				KernalPrintf(" 16-Bit Call Gate -----");
				CallGate = true;
				break;

			case DescriptiorData::TaskGateSegment:
				KernalPrintf("        Task Gate -----");
				CallGate = true;
				break;

			case DescriptiorData::IntGate16BitSegment:
				KernalPrintf(" 16-Bit Int Gate- -----");
				CallGate = true;
				break;

			case DescriptiorData::TrapGate16BitSegment:
				KernalPrintf(" 16-Bit Trap Gate -----");
				CallGate = true;
				break;

			case DescriptiorData::TSS32BitSegment:
				KernalPrintf(" 32-Bit TSS       ----");
				if(Entry->Granularity)
					KernalPrintf("G");
				else
					KernalPrintf("-");
				break;

			case DescriptiorData::TSS32BitSegmentBusy:
				KernalPrintf(" 32-Bit TSS       B---");
				if(Entry->Granularity)
					KernalPrintf("G");
				else
					KernalPrintf("-");
				break;

			case DescriptiorData::CallGate32BitSegment:
				KernalPrintf(" 32-Bit Call Gate -----");
				CallGate = true;
				break;

			case DescriptiorData::IntGate32BitSegment:
				KernalPrintf(" 32-Bit Int Gate  -----");
				CallGate = true;
				break;

			case DescriptiorData::TrapGate32BitSegment:
				KernalPrintf(" 32-Bit Trap Gate -----");
				CallGate = true;
				break;

			default:
				KernalPrintf("        Reserved  -----");
				break;
		}

	}

	KernalPrintf(" DPL=%X", Entry->DPL);
	
	if(!Entry->Present)
	{
		KernalPrintf("  AVL=%08X-%02X-%04X\n", Entry->Avaliable1, Entry->Avaliable2, Entry->Avaliable3);
		return;
	}

	if(CallGate)
	{
		KernalPrintf(" Selector=%04X", Entry->Selector);
		if(Entry->Type != DescriptiorData::TaskGateSegment)
			KernalPrintf(" Offset=%04X%04X", Entry->OffsetHigh, Entry->OffsetLow);
		else
			KernalPrintf("                  ");
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
	
		KernalPrintf(" Base=%08X", Base);
		KernalPrintf(" Limit=%08X ", Limit);
	}
	
	if(!CallGate)
		KernalPrintf(" AVL=%X", Entry->Avaliable);	
}