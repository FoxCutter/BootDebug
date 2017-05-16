#include "DescriptorTable.h"
#include <stdint.h>
#include "ListNode.h"
#pragma once

struct InterruptContext;
typedef void (*InterruptCallbackPtr)(InterruptContext * Context, uintptr_t * Data);

struct InterruptData
{
	InterruptCallbackPtr InterruptCallback;
	uintptr_t * Data;
};

struct InterruptMeta : ListNodeBase<InterruptMeta>
{
	// The ID of the interrupt this is the meta data for.
	uint16_t Interrupt;

	// This is where we store our function code.
	__declspec(align(32)) uint8_t Callback[0x100];

	ListEntry Prev;
	ListEntry Next;
};

class IDTManager : DescriptorTable
{
	uint16_t m_CodeSelector;
	uint16_t m_DataSelector;
	uint16_t m_GlobalSelector;

	InterruptData InterruptCallback[256]; 

	InterruptMeta::ListEntry MetaData;

public:
	IDTManager();

	void Initilize(uint16_t CodeSelector, uint16_t DataSelector, uint16_t GlobalSelector);

	void SetInterupt(unsigned int  IntNum, InterruptCallbackPtr CallBack, uintptr_t * Data = nullptr);

	// This version is only used if you want to change it from a Gate (Clears IF) to a trap (Doesn't Clear IF)
	void SetInterupt(unsigned int  IntNum, InterruptCallbackPtr CallBack, uint8_t Type, uintptr_t * Data = nullptr);
	
	// Changes the DPL of the IDT entry
	void SetInteruptDPL(unsigned int  IntNum, InterruptCallbackPtr CallBack, uint8_t DPL, uintptr_t * Data = nullptr);

	DescriptiorData::TablePtr SetActive();

	void Dump();

};
