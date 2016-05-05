#include "DescriptorTable.h"
#include <stdint.h>
#pragma once

class GDTManager : public DescriptorTable
{
public:
	GDTManager();
	void Initilize(uint32_t BaseAddress);

	DescriptiorData::TablePtr SetActive(uint16_t NewCodeSelector, uint16_t NewDataSelector);
};

