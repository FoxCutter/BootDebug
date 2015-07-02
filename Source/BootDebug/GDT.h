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

		NonSystemFlag		= 0x0010,
		Present				= 0x0080,
		AvaliableBit		= 0x1000,
		LongMode			= 0x2000,
		Operand32Bit		= 0x4000,
	};

	enum GDTTypes
	{
		DataReadOnly							= 0,
		DataReadOnlyAccessed					= Accessed,
		DataReadWrite							= ReadWrite,
		DataReadWriteAccessed					= ReadWrite | Accessed,
		DataReadOnlyExpandDown					= DirectionConforming,
		DataReadOnlyExpandDownAccessed			= DirectionConforming | Accessed,
		DataReadWriteExpandDown					= DirectionConforming | ReadWrite,
		DataReadWriteExpandDownAccessed			= DirectionConforming | ReadWrite | Accessed,
		
		CodeExecuteOnly							= Executable,
		CodeExecuteOnlyAccessed					= Executable | Accessed,
		CodeExecuteRead							= Executable | ReadWrite,
		CodeExecuteReadAccessed					= Executable | ReadWrite | Accessed,
		CodeExecuteReadOnlyConforming			= Executable | DirectionConforming,
		CodeExecuteReadOnlyConformingAccessed	= Executable | DirectionConforming | Accessed,
		CodeExecuteReadReadConforming			= Executable | DirectionConforming | ReadWrite,
		CodeExecuteReadReadConformingAccessed	= Executable | DirectionConforming | ReadWrite | Accessed,

		// System Types
		//ReservedSegment		= 0x0,
		TSS16BitSegment			= 0x1,
		LDTSegment				= 0x2,
		TSS16BitSegmentBusy		= 0x3,
		CallGate16BitSegment	= 0x4,
		TaskGateSegment			= 0x5,
		IntGate16BitSegment		= 0x6,
		TrapGate16BitSegment	= 0x7,
		//ReservedSegment		= 0x8,
		TSS32BitSegment			= 0x9,
		//ReservedSegment		= 0xA,
		TSS32BitSegmentBusy		= 0xB,
		CallGate32BitSegment	= 0xC,
		//ReservedSegment		= 0xD,
		IntGate32BitSegment		= 0xE,
		TrapGate32BitSegment	= 0xF,
	};
	
	struct GDTEntry
	{
		union
		{
			struct
			{
				uint16_t LimitLow;
				uint16_t BaseLow;
				uint8_t  BaseMid;
		
				uint16_t Type : 4;
				uint16_t NonSystem : 1;
				uint16_t DLP : 2;
				uint16_t Present : 1;
				
				uint16_t LimitHigh : 4;
				uint16_t Avaliable : 1;
				uint16_t Long : 1;
				uint16_t Big : 1;
				uint16_t Granularity : 1;
		
				uint8_t  BaseHigh;
			};
			
			struct
			{
				uint32_t Avaliable1;
				uint8_t  Avaliable2;
				uint8_t  Reserved;		// Overlaps the Type, NonSystem, DLP and Present bits.
				uint16_t Avaliable3;
			};
		};
	};

	struct GDTPtr
	{
		uint16_t Limit;
		uint32_t Address;
	};

	struct TSS
	{
		uint32_t PreviouseLinkedTask;
		
		// Stacks for Privilege Level changes
		uint32_t ESP0;
		uint32_t SS0;
		uint32_t ESP1;
		uint32_t SS1;
		uint32_t ESP2;
		uint32_t SS2;

		uint32_t CR3;
		
		uint32_t EIP;
		uint32_t EFlags;

		uint32_t EAX;
		uint32_t ECX;
		uint32_t EDX;
		uint32_t EBX;

		uint32_t ESP;
		uint32_t EBP;
		uint32_t ESI;
		uint32_t EDI;

		uint32_t ES;
		uint32_t CS;
		uint32_t SS;
		uint32_t DS;
		uint32_t FS;
		uint32_t GS;
		uint32_t LDTSelector;
		uint16_t DebugTrapFlag;
		uint16_t IOMapBaseAddress;
	};
}

class GDTManager
{
	static const int TableSize = 0x10;
	
	GDT::GDTEntry GDTTable[TableSize];

	int m_NextFreeSlot;

public:
	GDTManager();

	uint16_t AddGDTEntry(uint32_t Base, uint32_t Limit, uint16_t Attributes, uint8_t Type, uint8_t DPL);
	
	GDT::GDTPtr SetActive(uint16_t NewCodeSelector, uint16_t NewDataSelector);

	static void UpdateGDTEntry(uint16_t Selector, uint32_t Base, uint32_t Limit, uint16_t Attributes, uint8_t Type, uint8_t DPL);

	void Dump();
	void PrintSelector(uint16_t Selector);

private:
	static void BuildGDTEntry(GDT::GDTEntry *Entry, uint32_t Base, uint32_t Limit, uint16_t Attributes, uint8_t Type, uint8_t DPL);

};



#pragma pack(pop)