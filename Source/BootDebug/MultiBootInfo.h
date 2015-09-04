#include <stdint.h>
#pragma once

struct MemoryMapEntry
{
	uint64_t BaseAddress;
	uint64_t Length;
	uint32_t Type;
};

struct ModuleEntry
{
	intptr_t ModStart;
	intptr_t ModEnd;
	char * CommandLine;
};

struct FrameBufferData
{
	uint64_t Address;
	uint32_t Pitch;
	uint32_t Width;
	uint32_t Height;
	uint8_t  BPP;
	uint8_t  Type;
};

class MultiBootInfo
{
public:
	enum MultiBootType
	{
		Version1 = 1,
		Version2 = 2,
	};

	MultiBootInfo(void);
	~MultiBootInfo(void);

	bool LoadMultiBootInfo(uint32_t Signatrue, void *Data);
	
	bool GetFirstMemoryEntry(MemoryMapEntry &Entry);
	bool GetNextMemoryEntry(MemoryMapEntry &Entry);

	bool GetFirstModuleEntry(ModuleEntry &Entry);
	bool GetNextModuleEntry(ModuleEntry &Entry);


	void Dump();

	MultiBootType Type;

	const char * CommandLine;
	const char * BootLoader;

	uint32_t MemoryLow;
	uint32_t MemoryHigh;	

	uint32_t BootDevice;
	uint32_t BootPartition[3];

	FrameBufferData FrameBuffer;

	uint32_t HeaderLength;
	void *MBData;

private:
	bool LoadMultiBoot1Info(void *Data);
	bool LoadMultiBoot2Info(void *Data);

	void *MemoryMapData;
	intptr_t CurrentMemoryMapData;

	void *CurrentModuleData;
};

