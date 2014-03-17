#include <stdint.h>
#pragma once
#pragma pack(push, 1)

namespace GDT
{
	enum GDTAttributes
	{
		Accessed			= 0x0001,
		ReadWrite			= 0x0002,
		DirectionConforming = 0x0004,
		Executable			= 0x0008,
		TypeMask			= 0x000F,
		NonSystemFlag		= 0x0010,
		DPL0				= 0x0000,
		DPL1				= 0x0020,
		DPL2				= 0x0040,
		DPL3				= 0x0060,
		DPLMask				= 0x0060,
		Present				= 0x0080,

		LimitHighMask		= 0x0F00,
		AvaliableBit		= 0x1000,
		//LongMode			= 0x2000,
		Operand32Bit		= 0x4000,
		Granularity4k		= 0x8000,
	};

	enum GDTTypes
	{
		DataSegmentReadOnly = 0,
		CodeSegmentReadOnly = Executable,

		DataSegment = ReadWrite,
		CodeSegment = ReadWrite | Executable,


		// System Types
		TSS_32Bit = 0x1001,
		TSS_16Bit = 0x0001,
	};
	
	struct GDTEntry
	{
		uint16_t LimitLow;
		uint16_t BaseLow;
		uint8_t  BaseMid;
		
		uint16_t Attributes;
		
		uint8_t  BaseHigh;
	};

	struct GDTPtr
	{
		uint16_t Limit;
		uint32_t Address;
	};
}

class GDTManager
{
	static const int TableSize = 10;
	
	GDT::GDTEntry GDTTable[TableSize];

	int m_NextFreeSlot;

public:
	GDTManager();

	uint16_t AddGDTEntry(uint32_t Base, uint32_t Limit, uint16_t Attributes, uint8_t Type, uint8_t DPL);
	
	GDT::GDTPtr SetActive(uint16_t NewCodeSelector, uint16_t NewDataSelector);

	void PrintSelector(uint16_t Selector);

private:
	void BuildGDTEntry(GDT::GDTEntry *Entry, uint32_t Base, uint32_t Limit, uint16_t Attributes, uint8_t Type, uint8_t DPL);

};



#pragma pack(pop)