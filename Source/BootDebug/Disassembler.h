#pragma once
#include <stdint.h>

class Disassembler
{
	struct OpcodeData;
	void PrintOpcode(uint8_t *Address, OpcodeData &Data);

	OpcodeData ReadOpcode(uint8_t *Address, uint32_t OpSize, uint32_t AddressSize, bool REX);
public:
	Disassembler(void);
	~Disassembler(void);

	uint32_t Disassamble(intptr_t *Address, uint32_t Length, uint32_t OpSize = 32, uint32_t AddressSize = 32);
};

