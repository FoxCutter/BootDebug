#include <stdint.h>
#include "GDT.h"
#include "LDT.h"
#include "IDT.h"
#include "MMU.h"

#include "MultiBootInfo.h"
#include "LowLevel.h"
#include "RawMemory.h"
#include "MemoryMap.h"
#include "Thread.h"

#include "HardwareTree.h"
#include "ObjectManager.h"
#include "ACPI.h"

struct CoreComplexObj
{
	CoreComplexObj *Self;
	uint16_t CodeSegment0;
	uint16_t DataSegment0;

	uint16_t CodeSegment1;
	uint16_t DataSegment1;

	uint16_t CodeSegment2;
	uint16_t DataSegment2;

	uint16_t CodeSegment3;
	uint16_t DataSegment3;

	uint16_t CoreSegment;
	uint16_t ThreadSegment;
	uint16_t TaskSegment;

	MultiBootInfo MultiBoot;
	
	RawMemory KernalHeap;
	MemoryPageMap PageMap;

	ThreadInformation *ThreadComplex;
	//ProcessInformation *ProcessComplex;
	//DeviceInformation * DeviceComplex;
	HardwareComplexObj HardwareComplex;
	
	ObjectManager ObjectComplex;

	GDTManager GDTTable;
	LDTManager LDTTable;
	IDTManager IDTTable;

	ACPI ACPIComplex;

	uint64_t TimePassed;

	static CoreComplexObj *GetComplex()
	{
		return reinterpret_cast<CoreComplexObj *>(ReadGS(0));
	}
};
