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
	static const int MemoryMapMaxLength = 0xFF;
public:
	enum MultiBootType
	{
		Version1 = 1,
		Version2 = 2,
	};

	MultiBootInfo(void);
	~MultiBootInfo(void);

	bool LoadMultiBootInfo(uint32_t Signatrue, void *Data);
	void Dump();

	MultiBootType Type;

	const char * CommandLine;
	const char * BootLoader;
	MemoryMapEntry *MemoryMap[MemoryMapMaxLength];
	int MemoryMapLength;

	uint32_t MemoryLow;
	uint32_t MemoryHigh;	

	uint32_t BootDevice;
	uint32_t BootPartition[3];

	uint32_t HeaderLength;
	void *MBData;

private:
	bool LoadMultiBoot1Info(void *Data);
	bool LoadMultiBoot2Info(void *Data);
};

