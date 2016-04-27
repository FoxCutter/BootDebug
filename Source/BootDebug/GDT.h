#include "DescriptorTable.h"
#include <stdint.h>
#pragma once

class GDTManager : public DescriptorTable
{
public:
	GDTManager();
	void Initilize(uint32_t BaseAddress);

	DescriptiorData::TablePtr SetActive(uint16_t NewCodeSelector, uint16_t NewDataSelector);

	static void UpdateGDTEntry(uint16_t Selector, uint32_t Base, uint32_t Limit, uint16_t Attributes, uint8_t Type, uint8_t DPL);
};

