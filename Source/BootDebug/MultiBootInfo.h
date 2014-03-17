#include <stdint.h>
#pragma once

struct MemoryMapEntry
{
	uint32_t BaseAddressLow;
	uint32_t BaseAddressHigh;
	uint32_t LengthLow;
	uint32_t LengthHigh;
	uint32_t Type;
};

class MultiBootInfo
{
	static const int MemoryMapMaxLength = 32;
public:
	MultiBootInfo(void);
	~MultiBootInfo(void);

	bool LoadMultiBootInfo(uint32_t Signatrue, void *Data);

	const char * CommandLine;
	MemoryMapEntry *MemoryMap[MemoryMapMaxLength];
	int MemoryMapLength;

private:
	bool LoadMult1Boot1Info(void *Data);
	bool LoadMult2Boot1Info(void *Data);
};

