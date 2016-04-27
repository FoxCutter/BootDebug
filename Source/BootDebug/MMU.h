#pragma once
#include <stdint.h>

class MMU
{
public:
	MMU(void);
	~MMU(void);

	void PrintAddressInformation(uint32_t Address);
	void Dump();
};

