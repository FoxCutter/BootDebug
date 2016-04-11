#include "MPConfig.h"
#include "Utility.h"
#include <new>
#include <stdio.h>
#include <string.h>

#pragma pack(push, 1)

namespace MPData
{
	const uint8_t MPPointerSig[] = "_MP_";
	
	struct MPPointerStruct
	{
		uint8_t Signature[4];
		uint32_t DataAddress;
		uint8_t Length;
		uint8_t Rev;
		uint8_t Checksum;
		uint8_t MPFeature[5];
	};

	struct MPConfigHeader
	{
		uint8_t Signature[4];
		uint16_t BaseTableLegth;
		uint8_t Rev;
		uint8_t Checksum;
		uint8_t OEMID[8];
		uint8_t ProductID[12];
		uint32_t OEMTablePointer;
		uint16_t OEMTableSize;
		uint16_t EntryCount;
		uint32_t LAPICAddress;
		uint16_t ExtendedTableLength;
		uint8_t ExtendedTableChecksum;
		uint8_t Reserved;
	};

	enum MPConfigEntryType
	{
		MPCE_Processor = 0,
		MPCE_BUS = 1,
		MPCE_IOAPIC = 2,
		MPCE_IOInterupt = 3,
		MPCE_LocalInterrupt = 4,
	};

	struct MPConfigTableProcessorEntry
	{
		uint8_t Type;
		uint8_t LAPIC_ID;
		uint8_t LAPIC_Ver;
		uint8_t Flags;

		uint32_t CPUSignature;
		uint32_t FeatureFlags;
		uint32_t Reserved[2];
	};

	struct MPConfigTableBusEntry
	{
		uint8_t Type;
		uint8_t Bus_ID;
		uint8_t BusType[6];
	};

	struct MPConfigTableIOAPCIEntry
	{
		uint8_t Type;
		uint8_t IOAPIC_ID;
		uint8_t IOLAPIC_Ver;
		uint8_t Flags;

		uint32_t IOAPICAddress;
	};

	struct MPConfigTableInteruptEntry
	{
		uint8_t Type;
		uint8_t IntType;
		uint16_t Flags;
		uint8_t SourceBusID;
		uint8_t SourceBusIRQ;
		uint8_t DestIOAPICID;
		uint8_t DestIOAPICINT;
	};
}

#pragma pack(pop)

MPConfig::MPConfig(void)
{
}


MPConfig::~MPConfig(void)
{
}

char * TypeToString(uint8_t Type)
{
	switch (Type)
	{
		case 0:
			return "INT";
		case 1:
			return "NMI";
		case 2:
			return "SMI";
		case 3:
			return "Ext";
		default:
			return "UNK";
	}
}

void PrintFlags(uint16_t Flags);


bool MPConfig::Initilize()
{
	// First, check the 1st K of the EBDA.
	uint32_t EBDABase = *(uint16_t *)(0x040E);
	EBDABase = EBDABase << 4;

	uint32_t MPPointerAddress = SeachMemory(EBDABase, 0x400, MPData::MPPointerSig, 4, 0x10);

	// If that failed check the BIOS space
	if(MPPointerAddress == UINT32_MAX)
		MPPointerAddress = SeachMemory(0xE0000, 0x20000, MPData::MPPointerSig, 4, 0x10);

	if(MPPointerAddress == UINT32_MAX)
		return false;

	MPData::MPPointerStruct * MPPointer = reinterpret_cast<MPData::MPPointerStruct *>(MPPointerAddress);
	
	if(!ValidateChecksum(MPPointer, MPPointer->Length * 16))
		return false;

	MPData::MPConfigHeader* MPConfig = (MPData::MPConfigHeader *) MPPointer->DataAddress;

	uint8_t * Data = (uint8_t *)(MPPointer->DataAddress + sizeof(MPData::MPConfigHeader));
	
	for(int x = 0; x < MPConfig->EntryCount; x++)
	{
		switch(*Data)
		{
			case MPData::MPCE_Processor:
				{
					MPData::MPConfigTableProcessorEntry *Entry = (MPData::MPConfigTableProcessorEntry *)Data;

					printf("Processor: ID %02X, Flags %08X, Sig %08X, Feature %08X\n", Entry->LAPIC_ID, Entry->Flags, Entry->CPUSignature, Entry->FeatureFlags);

					Data += 20;
				}
				break;

			case MPData::MPCE_BUS:
				{
					MPData::MPConfigTableBusEntry *Entry = (MPData::MPConfigTableBusEntry *)Data;

					printf("Bus: ID %02X, Type: %6.6s\n", Entry->Bus_ID, Entry->BusType);

					Data += 8;
				}
				break;

			case MPData::MPCE_IOAPIC:
				{
					MPData::MPConfigTableIOAPCIEntry *Entry = (MPData::MPConfigTableIOAPCIEntry *)Data;

					printf("I/O APCI: ID %02X, Flags: %02X, Address: %08X\n", Entry->IOAPIC_ID, Entry->Flags, Entry->IOAPICAddress);

					Data += 8;
				}
				break;

			case MPData::MPCE_IOInterupt:
				{
					MPData::MPConfigTableInteruptEntry *Entry = (MPData::MPConfigTableInteruptEntry *)Data;

					printf("I/O Int: Type %s ", TypeToString(Entry->IntType));
					PrintFlags(Entry->Flags);
					printf(", Source Bus:IRQ %02X:%02X, Dest I/O:INT %02X:%02X\n", Entry->SourceBusID, Entry->SourceBusIRQ, Entry->DestIOAPICID, Entry->DestIOAPICINT);
					//printf("I/O Int: Type %02X, Source Bus %02X, Source IRQ %02X, Dest I/O APIC: %02X, Dest Int %02X\n", Entry->IntType, Entry->SourceBusID, Entry->SourceBusIRQ, Entry->DestIOAPICID, Entry->DestIOAPICINT);

					Data += 8;
				}
				break;

			case MPData::MPCE_LocalInterrupt:
				{
					MPData::MPConfigTableInteruptEntry *Entry = (MPData::MPConfigTableInteruptEntry *)Data;

					printf("Loc Int: Type %s ", TypeToString(Entry->IntType));
					PrintFlags(Entry->Flags);
					printf(", Source Bus:IRQ %02X:%02X, Dest APIC ", Entry->SourceBusID, Entry->SourceBusIRQ);
					if(Entry->DestIOAPICID == 0xFF)
						printf("ALL");
					else
						printf("%02X ", Entry->DestIOAPICID);

					printf(":LINT%01X\n", Entry->DestIOAPICINT);

					Data += 8;
				}
				break;

			default:
				return false;

		}
	}

	return true;
}