#include <stdint.h>
#pragma once

struct MemoryMapEntry
{
	uint64_t BaseAddress;
	uint64_t Length;
	uint32_t Type;
};

class MultiBootInfo
{
	static const int MemoryMapMaxLength = 32;
public:
	enum MultiBootType
	{
		Version1,
		Version2,
	};


	MultiBootInfo(void);
	~MultiBootInfo(void);

	bool LoadMultiBootInfo(uint32_t Signatrue, void *Data);

	MultiBootType Type;

	const char * CommandLine;
	const char * BootLoader;
	MemoryMapEntry *MemoryMap[MemoryMapMaxLength];
	int MemoryMapLength;

	uint32_t MemoryLow;
	uint32_t MemoryHigh;	

	uint32_t BootDevice;
	uint32_t BootPartition[3];

private:
	bool LoadMult1Boot1Info(void *Data);
	bool LoadMult2Boot1Info(void *Data);
};

