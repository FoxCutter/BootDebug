#include <stdint.h>
#pragma once
#pragma pack(push, 1)

namespace IDT
{
	enum IDTAttributes
	{
		TypeMask			= 0x0F,
		Operand32Bit		= 0x08,
		//NonSystemFlag		= 0x10,	// All IDT entries are 'System'
		
		DPL0				= 0x00,
		DPL1				= 0x20,
		DPL2				= 0x40,
		DPL3				= 0x60,
		DPLMask				= 0x60,
		
		Present				= 0x80,
	};

	enum IDTTypes
	{		
		InteruptGate_32Bit = 0x06 | Operand32Bit,
		InteruptGate_16Bit = 0x06,

		InteruptTrap_32Bit = 0x07 | Operand32Bit,
		InteruptTrap_16Bit = 0x07,
	};
	
	struct IDTEntry
	{
		uint16_t OffsetLow;
		uint16_t Segment;
		uint8_t  Reserved;		
		uint8_t  Attributes;
		uint16_t OffsetHigh;		
	};

	struct IDTPtr
	{
		uint16_t Limit;
		uint32_t Address;
	};
}

struct InterruptContext;
typedef void (*InterruptCallbackPtr)(InterruptContext * Context, uintptr_t * Data);

struct InterruptData
{
	InterruptCallbackPtr InterruptCallback;
	uintptr_t * Data;
};

class IDTManager
{
	IDT::IDTEntry IDTTable[256];
	InterruptData InterruptCallback[256]; 

public:
	IDTManager();
	void SetSelectors(uint16_t CodeSelector, uint16_t DataSelector);

	void SetInterupt(unsigned int  IntNum, InterruptCallbackPtr CallBack, uintptr_t * Data = nullptr);

	// This version is only used if you want to change it from a Gate (Clears IF) to a trap (Doesn't Clear IF)
	void SetInterupt(unsigned int  IntNum, InterruptCallbackPtr CallBack, uint8_t Type, uintptr_t * Data = nullptr);
	
	// Changes the DPL of the IDT entry
	void SetInteruptDPL(unsigned int  IntNum, InterruptCallbackPtr CallBack, uint8_t DPL, uintptr_t * Data = nullptr);

	IDT::IDTPtr SetActive();

private:
	void BuildIDTEntry(IDT::IDTEntry *Entry, uint16_t Segment, uint32_t Offset, uint8_t Attributes, uint8_t Type, uint8_t DPL);

};


#pragma pack(pop)