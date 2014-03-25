#include "ACPI.h"
#include <new>
#include <stdio.h>
#include <string.h>

#pragma pack(push, 1)

namespace ACPIData
{
	const uint8_t RSDPSig[] = "RSD PTR ";

	struct RSDP
	{
		uint8_t		Signature[8];
		uint8_t		Checksum;
		uint8_t		OEMID[6];
		uint8_t		Revision;
		uint32_t	RSDTAddress;
		uint32_t	Length;
		uint64_t	XSDTAddress;
		uint8_t		ExtendedChecksum;
		uint8_t		Reserved[3];
	};
	
	struct GenAddress
	{
		uint8_t		AddressSpaceID;
		uint8_t		RegisterBitWith;
		uint8_t		RegisterBitOffset;
		uint8_t		AccessSize;
		uint16_t	Address;
	};

	struct DescriptionHeader
	{
		uint8_t		Signature[4];
		uint32_t	Length;
		uint8_t		Revision;
		uint8_t		Checksum;
		uint8_t		OEMID[6];
		uint8_t		OEMTableID[8];
		uint32_t	OEMRevision;
		uint32_t	CreatorID;
		uint32_t	CreatorRevision;
	};

	const uint8_t RSDTTableSig[] = "RSDT";

	struct RSDTTable
	{
		uint8_t		Signature[4];
		uint32_t	Length;
		uint8_t		Revision;
		uint8_t		Checksum;
		uint8_t		OEMID[6];
		uint8_t		OEMTableID[8];
		uint32_t	OEMRevision;
		uint32_t	CreatorID;
		uint32_t	CreatorRevision;

		//----------
		uint32_t	Entry[1];
	};

	const uint8_t FADTTableSig[] = "FACP";

	struct FADTTable
	{
		uint8_t		Signature[4];
		uint32_t	Length;
		uint8_t		Revision;
		uint8_t		Checksum;
		uint8_t		OEMID[6];
		uint8_t		OEMTableID[8];
		uint32_t	OEMRevision;
		uint32_t	CreatorID;
		uint32_t	CreatorRevision;

		uint32_t	FACSAddress;
		uint32_t	DSDTAddress;
		uint8_t		Reserved;
		uint8_t		PreferredPowerProfile;
		uint16_t	SCI_Int;
		uint32_t	SMI_Cmd;
		uint8_t		ACPI_Enable;
		uint8_t		ACPI_Disable;
		uint8_t		S4Bios_Req;
		uint8_t		PState_Cnt;

		uint32_t	PM1a_EventBlock;
		uint32_t	PM1b_EventBlock;
		uint32_t	PM1a_ControlBlock;
		uint32_t	PM1b_ControlBlock;
		uint32_t	PM2_ControlBlock;
		uint32_t	PM_TimerBlock;
		uint32_t	GPE0_Block;
		uint32_t	GPE1_Block;
		uint8_t		PM1_EventLength;
		uint8_t		PM1_ControLength;
		uint8_t		PM2_ControLength;
		uint8_t		PM_TimeLength;
		uint8_t		GPE0_BlockLength;
		uint8_t		GPE1_BlockLength;
		uint8_t		GPE1_Base;
		uint8_t		CST_CNT;
		uint16_t	Level2Latency;
		uint16_t	Level3Latency;
		uint16_t	FlushSize;
		uint16_t	FlushStride;
		uint8_t		DutyOffset;
		uint8_t		DutyWidth;
		uint8_t		DayAlarm;
		uint8_t		MonthAlarm;
		uint8_t		Centry;
		uint16_t	BootArchitectureFlags;
		uint8_t		Reserved2;
		uint32_t	Flags;

		// Extended data
		GenAddress	ResetReg;
		uint8_t		ResetValue;
		uint8_t		Reserved3[3];
		uint64_t	ExFACSAddress;
		uint64_t	ExDSDTAddress;
		GenAddress	ExPM1a_EventBlock;
		GenAddress	ExPM1b_EventBlock;
		GenAddress	ExPM1a_ControlBlock;
		GenAddress	ExPM1b_ControlBlock;
		GenAddress	ExPM2_ControlBlock;
		GenAddress	ExPM_TimerBlock;
		GenAddress	ExGPE0_Block;
		GenAddress	ExGPE1_Block;
		
	};

};

#pragma pack(pop)

ACPI::ACPI(void)
{
}


ACPI::~ACPI(void)
{
}

uint32_t SeachMemory(uint32_t Start, uint32_t Count, const uint8_t *Search, uint32_t Alignment)
{
	int SearchLength = strlen(reinterpret_cast<const char *>(Search));
	if(SearchLength == 0)
		return UINT32_MAX;

	// The starting address should be aligned
	if(Start % Alignment != 0)
	{
		Count -= Start % Alignment;
		Start = (Start + Alignment) - (Start % Alignment);
	}
	
	uint8_t *Data = reinterpret_cast<uint8_t *>(Start);

	int SeachState = 0;

	for(size_t Pos = 0; Pos < Count; Pos += Alignment)
	{
		if(memcmp(Data + Pos, Search, SearchLength) == 0)
			return Start + Pos;
	}

	return UINT32_MAX;
}

bool ValidateChecksum(void *Data, uint16_t Length)
{
	uint8_t Val = 0;
	uint8_t *DataPtr = reinterpret_cast<uint8_t *>(Data);
	for(int x = 0; x < Length; x++)
	{
		Val += DataPtr[x];
	}

	return Val == 0;
}

#pragma function(memcmp)
int __cdecl memcmp(const void * _Buf1, const void * _Buf2, size_t _Size)
{
	const char * Buf1 = reinterpret_cast<const char *>(_Buf1);
	const char * Buf2 = reinterpret_cast<const char *>(_Buf2);

	for(size_t x = 0; x < _Size; x++)
	{
		if(Buf1[x] - Buf2[x] != 0)
			return Buf1[x] - Buf2[x];
	}
	
	return 0;
}

bool ACPI::Initilize()
{
	// First, check the 1st K of the EBDA.
	uint32_t EBDABase = *(uint16_t *)(0x040E);
	EBDABase = EBDABase << 4;

	uint32_t RSDPAddress = SeachMemory(EBDABase, 0x400, ACPIData::RSDPSig, 0x10);

	// If that failed check the BIOS space
	if(RSDPAddress == UINT32_MAX)
		RSDPAddress = SeachMemory(0xE0000, 0x20000, ACPIData::RSDPSig, 0x10);

	if(RSDPAddress == UINT32_MAX)
		return false;

	ACPIData::RSDP * RSDP = reinterpret_cast<ACPIData::RSDP *>(RSDPAddress);
	
	if(!ValidateChecksum(RSDP, RSDP->Length))
		return false;

	//printf("RSDT%08X, X:%016llX\n", RSDP->RSDTAddress, RSDP->XSDTAddress);

	ACPIData::RSDTTable *RSDTTable = reinterpret_cast<ACPIData::RSDTTable *>(RSDP->RSDTAddress);
	int EntryCount = (RSDTTable->Length - sizeof(ACPIData::DescriptionHeader)) / 4;
	for(int x = 0; x < EntryCount; x++)
	{
		//printf("Entry %u: %08X: ", x, RSDTTable->Entry[x]);

		ACPIData::DescriptionHeader * Blob = reinterpret_cast<ACPIData::DescriptionHeader *>(RSDTTable->Entry[x]);
		//printf("%c%c%c%c (%08X)\n", Blob->Signature[0], Blob->Signature[1], Blob->Signature[2], Blob->Signature[3], Blob->Length);

		if(memcmp(Blob->Signature, ACPIData::FADTTableSig, 4) == 0)
		{
			ACPIData::FADTTable *FADT = reinterpret_cast<ACPIData::FADTTable *>(RSDTTable->Entry[x]);
			//printf("  FACS: %08X DSDT: %08X\n", FADT->FACSAddress, FADT->DSDTAddress);
		}
	}
	
	//uint32_t NextAddress = RSDP->RSDTAddress;
	//
	//while(true)
	//{
	//	if(NextAddress % 0x10 != 0)
	//		NextAddress = (NextAddress + 0x10) - (NextAddress % 0x10);

	//	ACPIData::DescriptionHeader * Blob = reinterpret_cast<ACPIData::DescriptionHeader *>(NextAddress);

	

	//	if(Blob->Signature[0] == 0)
	//		break;

	//	NextAddress += Blob->Length;
	//}



	return false;
}
