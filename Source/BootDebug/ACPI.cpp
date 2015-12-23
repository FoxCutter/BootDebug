#include "ACPI.h"
#include "Utility.h"
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
		uint64_t	Address;
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
		uint8_t		CreatorID[4];
		uint32_t	CreatorRevision;

		//----------
		uint32_t	Entry[1];
	};

	const uint8_t XSDTTableSig[] = "XSDT";

	struct XSDTTable
	{
		DescriptionHeader Header;

		//----------
		uint64_t	Entry[1];
	};

	const uint8_t FADTTableSig[] = "FACP";

	struct FADTTable
	{
		DescriptionHeader Header;

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
		uint8_t		PM1_ControlLength;
		uint8_t		PM2_ControlLength;
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

	const uint8_t FACSTableSig[] = "FACS";

	struct FACSTable
	{
		uint8_t		Signature[4];
		uint32_t	Length;
		uint32_t	HardwareSignature;
		uint32_t	FirmwareWakingVector;
		uint32_t	GlobalLock;
		uint32_t	Flags;
		uint64_t	ExFirmwareWakingVector;
		uint8_t		Version;
		uint8_t		Reserved[3];
		uint32_t	OSPMFlags;
	};


	const uint8_t MultiACPITableSig[] = "APIC";

	struct MultiACPITable
	{
		DescriptionHeader	Header;
		uint32_t			LocalACPIAddress;
		uint32_t			Flags;
	};

	struct MultiACPITableEntry
	{
		uint8_t		Type;
		uint8_t		Length;

		union
		{
			struct
			{
				uint8_t	ACPIProcessorID;
				uint8_t	APICID;
				uint32_t Flags;
			} ProcessorLocalAPIC;

			struct
			{
				uint8_t	IOAPICID;
				uint8_t Reserved;
				uint32_t IOAPCIAddress;
				uint32_t GlobalSystemIntBase;
			} IOAPIC;

			struct
			{
				uint8_t	Bus;
				uint8_t Source;
				uint32_t GlobalSystemInt;
				uint16_t Flags;
			} IntSourceOverride;

			struct
			{
				uint16_t Flags;
				uint32_t GlobalSystemInt;
			} NMISource;

			struct
			{
				uint8_t		ACPIProcessorID;
				uint16_t	Flags;
				uint8_t		LINT;
			} LocalAPICNMI;

			struct
			{
				uint16_t	Reserved;
				uint64_t	LocalAPICAddress;
			} LocalAPICAddressOverride;

			struct
			{
				uint8_t		IOSAPICID;
				uint8_t		Reserved;
				uint32_t	GlobalSystemIntBase;
				uint64_t	IOAPCIAddress;
			} IOSAPIC;

			struct
			{
				uint8_t		ACPIProcessorID;
				uint8_t		LocalSAPICID;
				uint8_t		Reserved[3];
				uint32_t	Flags;
				uint32_t	ACPIProcessorUID;
				uint8_t		ACPIProcessorUIDString[1];
			} LocalSAPIC;
			
			struct
			{
				uint16_t	Flags;
				uint8_t		Type;
				uint8_t		ProcessorID;
				uint8_t		ProcessorEID;
				uint8_t		IOSAPICVector;
				uint32_t	GloablSystemInt;
				uint32_t	PlatformIntFlags;
			} PlatfromIntSource;

			struct
			{
				uint16_t	Reserved;
				uint32_t	x2APICID;
				uint32_t	Flags;
				uint32_t	ACPIProcessorUID;
			} ProcessorLocalx2APIC;

			struct
			{
				uint16_t	Flags;
				uint32_t	ACPIProcessorUID;
				uint8_t		LINT;
				uint8_t		Reserved[3];
			} Localx2APICNMI;
		};
	};
};

#pragma pack(pop)

ACPI::ACPI(void)
{
}


ACPI::~ACPI(void)
{
}

bool ACPI::Initilize()
{
	// First, check the 1st K of the EBDA.
	uint32_t EBDABase = *(uint16_t *)(0x040E);
	EBDABase = EBDABase << 4;

	uint32_t RSDPAddress = SeachMemory(EBDABase, 0x400, ACPIData::RSDPSig, 8, 0x10);

	// If that failed check the BIOS space
	if(RSDPAddress == UINT32_MAX)
		RSDPAddress = SeachMemory(0xE0000, 0x20000, ACPIData::RSDPSig, 8, 0x10);

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


void ACPI::Dump(char *Options)
{
	// First, check the 1st K of the EBDA.
	uint32_t EBDABase = *(uint16_t *)(0x040E);
	EBDABase = EBDABase << 4;

	uint32_t RSDPAddress = SeachMemory(EBDABase, 0x400, ACPIData::RSDPSig, 8, 0x10);

	// If that failed check the BIOS space
	if(RSDPAddress == UINT32_MAX)
		RSDPAddress = SeachMemory(0xE0000, 0x20000, ACPIData::RSDPSig, 8, 0x10);

	if(RSDPAddress == UINT32_MAX)
	{
		printf("RSDP Data not found\n");
		return;
	}

	ACPIData::RSDP * RSDP = reinterpret_cast<ACPIData::RSDP *>(RSDPAddress);
	

	ACPIData::XSDTTable *XSDTTable = reinterpret_cast<ACPIData::XSDTTable *>(RSDP->XSDTAddress);
	int EntryCount = (XSDTTable->Header.Length - sizeof(ACPIData::DescriptionHeader)) / 8;

	if(Options == nullptr)
	{
		printf("ACPI OEM: %.6s Rev %02X (%08X)\n", RSDP->OEMID, RSDP->Revision, RSDP);
		printf(" RSDT: %08X, XSDT: ", RSDP->RSDTAddress); PrintLongAddress(RSDP->XSDTAddress); printf("\n");
		printf(" %08X: %.4s OEMID: %.6s Rev %02X, Entry Count %04X\n", XSDTTable, XSDTTable->Header.Signature, XSDTTable->Header.OEMID, XSDTTable->Header.OEMRevision, EntryCount);
	
		for(int x = 0; x < EntryCount; x++)
		{
			ACPIData::DescriptionHeader * Blob = reinterpret_cast<ACPIData::DescriptionHeader *>(XSDTTable->Entry[x]);
			printf("  %08X: %.4s OEMID: %.6s Rev %02X, Length %08X\n", Blob, Blob->Signature, Blob->OEMID, Blob->OEMRevision, Blob->Length);
		
			if(memcmp(Blob->Signature, ACPIData::FADTTableSig, 4) == 0)
			{
				ACPIData::FADTTable *FADT = reinterpret_cast<ACPIData::FADTTable *>(XSDTTable->Entry[x]);
				printf("   FACS: %08X DSDT: %08X\n", FADT->FACSAddress, FADT->DSDTAddress);
			}

		}
	} 
	else if(_stricmp("APIC", Options) == 0)
	{
		for(int x = 0; x < EntryCount; x++)
		{
			ACPIData::DescriptionHeader * Blob = reinterpret_cast<ACPIData::DescriptionHeader *>(XSDTTable->Entry[x]);
		
			if(memcmp(Blob->Signature, ACPIData::MultiACPITableSig, 4) == 0)
			{
				ACPIData::MultiACPITable *MADT = reinterpret_cast<ACPIData::MultiACPITable *>(XSDTTable->Entry[x]);
				printf("   Local APIC: %08X\n", MADT->LocalACPIAddress);
				
				uint32_t Offset = sizeof(ACPIData::MultiACPITable);


				while(Offset < MADT->Header.Length)
				{
					ACPIData::MultiACPITableEntry *Entry = reinterpret_cast<ACPIData::MultiACPITableEntry *>(XSDTTable->Entry[x] + Offset);

					switch(Entry->Type)
					{
						case 0:
							printf("   0: ACPI Proc ID: %02X, APIC ID: %02X, Flags: %08X\n", Entry->ProcessorLocalAPIC.ACPIProcessorID, Entry->ProcessorLocalAPIC.APICID, Entry->ProcessorLocalAPIC.Flags);
							break;

						case 1:
							printf("   1: I/O ACPI ID: %02X, I/O APIC Address: %08X, Global Int Base: %08X\n", Entry->IOAPIC.IOAPICID, Entry->IOAPIC.IOAPCIAddress, Entry->IOAPIC.GlobalSystemIntBase);
							break;

						case 2:
							printf("   2: Bus: %02X, Source: %02X, Global Int: %08X, Flags: %04X\n", Entry->IntSourceOverride.Bus, Entry->IntSourceOverride.Source, Entry->IntSourceOverride.GlobalSystemInt, Entry->IntSourceOverride.Flags);
							break;

						case 3:
							printf("   3: Global Int: %08X, Flags: %04X\n", Entry->NMISource.GlobalSystemInt, Entry->NMISource.Flags);
							break;

						case 4:
							printf("   4:  ACPI Proc ID: %02X, Flags: %04X, Local APIC LINT#: %02X\n", Entry->LocalAPICNMI.ACPIProcessorID, Entry->LocalAPICNMI.Flags, Entry->LocalAPICNMI.LINT);
							break;

						case 5:
							printf("   5:\n");
							break;

						case 6:
							printf("   6:\n");
							break;

						case 7:
							printf("   7:\n");
							break;
						case 8:
							printf("   8:\n");
							break;

						case 9:
							printf("   9: ACPI Proc UID: %08X, x2APIC ID: %02X, Flags: %08X\n", Entry->ProcessorLocalx2APIC.ACPIProcessorUID, Entry->ProcessorLocalx2APIC.x2APICID, Entry->ProcessorLocalx2APIC.Flags);
							break;

						case 0x0A:
							printf("   A:  ACPI Proc UID: %08X, Flags: %04X, Local APIC LINT#: %02X\n", Entry->Localx2APICNMI.ACPIProcessorUID, Entry->Localx2APICNMI.Flags, Entry->Localx2APICNMI.LINT);
							break;
						
						default:
							printf("   %X\n", Entry->Type);
							break;

					}

					Offset += Entry->Length;
				};
			}
		}
	}
	else if(_stricmp("FACP", Options) == 0)
	{
		ACPIData::FADTTable *FADT = nullptr;

		for(int x = 0; x < EntryCount; x++)
		{
			ACPIData::DescriptionHeader * Blob = reinterpret_cast<ACPIData::DescriptionHeader *>(XSDTTable->Entry[x]);
		
			if(memcmp(Blob->Signature, ACPIData::FADTTableSig, 4) == 0)
			{
				FADT = reinterpret_cast<ACPIData::FADTTable *>(XSDTTable->Entry[x]);
			}
		}

		if(FADT == nullptr)
		{
			printf(" Fixed ACPI Description Table (FADT) is missing\n");
			return;
		}

		printf(" Fixed ACPI Description Table (FADT) (%08X)\n", FADT);
		printf(" FACS: %08X, DSDT: %08X\n", FADT->FACSAddress, FADT->DSDTAddress);
		printf(" SCI Int: %04X, Port: %08X, ACPI Enable: %02X, ACPI Disable: %02X\n", FADT->SCI_Int, FADT->SMI_Cmd, FADT->ACPI_Enable, FADT->ACPI_Disable); 
		printf(" PM1 Event Block   %08X/%08X, Length: %02x\n", FADT->PM1a_EventBlock, FADT->PM1b_EventBlock, FADT->PM1_EventLength);
		printf(" PM1 Control Block %08X/%08X, Length: %02x\n", FADT->PM1a_ControlBlock, FADT->PM1b_ControlBlock, FADT->PM1_ControlLength);
		printf(" PM2 Control Block %08X, Length: %02x\n", FADT->PM2_ControlBlock, FADT->PM2_ControlLength);
		printf(" PM Timer Block    %08X, Length: %02x\n", FADT->PM_TimeLength, FADT->PM_TimeLength);
		printf(" GP0 Event Block   %08X, Length: %02x\n", FADT->GPE0_Block, FADT->GPE0_BlockLength);
		printf(" GP1 Event Block   %08X, Length: %02x, Base: %02X\n", FADT->GPE1_Block, FADT->GPE1_BlockLength, FADT->GPE1_Base);
		printf(" Boot Flags: %04X\n", FADT->BootArchitectureFlags);
		printf("  Legacy: %c, 8042: %c\n", FADT->BootArchitectureFlags & 0x01 ? 'Y' : 'N', FADT->BootArchitectureFlags & 0x02 ? 'Y' : 'N');
		printf(" Flags: %08X\n", FADT->Flags);

	}	
	else
	{
		printf("%s\n", Options);
	}

}
