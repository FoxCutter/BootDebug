#include "DescriptorTable.h"
#include <stdint.h>
#pragma once

class LDTManager : public DescriptorTable
{
	uint16_t m_Selector;

public:
	LDTManager(void);
	~LDTManager(void);

	void Initilize(DescriptorTable &GDT);

	uint16_t SetActive();

	uint16_t GetSelector() { return m_Selector; }

	uint16_t MakeLDTSelector(uint16_t Selector, uint8_t CPL = 0) { return Selector | 0x04 | (CPL & 0x03); }

	void Dump();
};

