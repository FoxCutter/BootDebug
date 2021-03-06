#include "ACPI.h"
#include "PCI.h"
#include "Utility.h"
#include <new>
#include <stdio.h>
#include <string.h>
#include "KernalLib.h"
#include "LowLevel.h"

#include "..\StdLib\c99-snprintf-1.1\system.h"

extern "C"
{
	#include <acpi.h>
	#include <accommon.h>
}


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

	void PrintGenAddress(ACPI_GENERIC_ADDRESS &Address)
	{
		switch(Address.SpaceId)
		{
			case 0:
				KernalPrintf("Mem: %08llX", Address.Address);
				break;

			case 1:
				KernalPrintf("I/O: %08llX, Bit: %02X Width: %02X", Address.Address, Address.BitOffset, Address.BitWidth);
				break;

			case 2:
				KernalPrintf("PCI: 00:%02X:%02X %02X", ((Address.Address & 0xffff00000000) >> 32), ((Address.Address & 0x0000ffff0000) >> 16), (Address.Address & 0x00000000ffff));
				break;
		}
	}
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
	return true;
}

ACPI_STATUS WalkResourceCallback (ACPI_RESOURCE *Resource, void *Context)
{	
	if(Resource->Type == ACPI_RESOURCE_TYPE_END_TAG)
		return AE_OK;

	KernalPrintf("  ");
	//KernalPrintf("%02X ", Resource->Type);
	switch(Resource->Type)
	{
		case ACPI_RESOURCE_TYPE_IRQ:
			KernalPrintf("IRQ:");
			for(int x = 0; x < Resource->Data.Irq.InterruptCount; x++)
			{
				KernalPrintf(" %02X", Resource->Data.Irq.Interrupts[x]);
			}
			break;

		case ACPI_RESOURCE_TYPE_DMA:
			KernalPrintf("DMA:");
			for(int x = 0; x < Resource->Data.Dma.ChannelCount; x++)
			{
				KernalPrintf(" %02X", Resource->Data.Dma.Channels[x]);
			}
			break;

		case ACPI_RESOURCE_TYPE_IO:
			KernalPrintf("IO: %04X-%04X Count: %02X", Resource->Data.Io.Minimum, Resource->Data.Io.Maximum, Resource->Data.Io.AddressLength);
			break;

		case ACPI_RESOURCE_TYPE_FIXED_IO:
			KernalPrintf("Fixed IO: %04X Count: %02X", Resource->Data.FixedIo.Address, Resource->Data.FixedIo.AddressLength);
			break;

		case ACPI_RESOURCE_TYPE_MEMORY24:
			KernalPrintf("Memory: %04X-%04X Length: %04X", Resource->Data.Memory24.Minimum, Resource->Data.Memory24.Maximum, Resource->Data.Memory24.AddressLength);
			break;

		case ACPI_RESOURCE_TYPE_MEMORY32:
			KernalPrintf("Memory: %08X-%08X Length: %08X", Resource->Data.Memory32.Minimum, Resource->Data.Memory32.Maximum, Resource->Data.Memory32.AddressLength);
			break;

		case ACPI_RESOURCE_TYPE_FIXED_MEMORY32:
			KernalPrintf("Fixed Memory: %08X Length: %08X", Resource->Data.FixedMemory32.Address, Resource->Data.FixedMemory32.AddressLength);
			break;

		case ACPI_RESOURCE_TYPE_ADDRESS16:
			if(Resource->Data.Address.ResourceType == ACPI_MEMORY_RANGE)
				KernalPrintf("Memory: ");

			if(Resource->Data.Address.ResourceType == ACPI_IO_RANGE)
				KernalPrintf("Word IO: ");

			if(Resource->Data.Address.ResourceType == ACPI_BUS_NUMBER_RANGE)
				KernalPrintf("Bus: ");

			KernalPrintf("%04X-%04X Length: %04X", Resource->Data.Address16.Address.Minimum, Resource->Data.Address16.Address.Maximum, Resource->Data.Address16.Address.AddressLength);
			break;

		case ACPI_RESOURCE_TYPE_ADDRESS32:
			if(Resource->Data.Address.ResourceType == ACPI_MEMORY_RANGE)
				KernalPrintf("Memory: ");

			if(Resource->Data.Address.ResourceType == ACPI_IO_RANGE)
				KernalPrintf("Dword IO: ");

			if(Resource->Data.Address.ResourceType == ACPI_BUS_NUMBER_RANGE)
				KernalPrintf("Bus: ");

			KernalPrintf("%08X-%08X Length: %08X", Resource->Data.Address32.Address.Minimum, Resource->Data.Address32.Address.Maximum, Resource->Data.Address32.Address.AddressLength);
			break;

		case ACPI_RESOURCE_TYPE_ADDRESS64:
			KernalPrintf("ADD64: ");
			break;

		default:
			KernalPrintf("%02X? ", Resource->Type);
			break;

	}

	KernalPrintf("\n");
	return AE_OK;
}

void PrintType(ACPI_OBJECT_TYPE Type)
{
	switch(Type)
	{
		case ACPI_TYPE_INTEGER:
			KernalPrintf("Integer");
			break;

		case ACPI_TYPE_STRING:
			KernalPrintf("String");
			break;

		case ACPI_TYPE_BUFFER:
			KernalPrintf("Buffer");
			break;

		case ACPI_TYPE_PACKAGE:
			KernalPrintf("Package");
			break;

		case ACPI_TYPE_FIELD_UNIT:
			KernalPrintf("Field Unit");
			break;

		case ACPI_TYPE_DEVICE:
			KernalPrintf("Device");
			break;

		case ACPI_TYPE_EVENT:
			KernalPrintf("Event");
			break;

		case ACPI_TYPE_METHOD:
			KernalPrintf("Method");
			break;

		case ACPI_TYPE_MUTEX:
			KernalPrintf("Mutex");
			break;

		case ACPI_TYPE_REGION:
			KernalPrintf("Region");
			break;

		case ACPI_TYPE_POWER:
			KernalPrintf("Power");
			break;

		case ACPI_TYPE_PROCESSOR:
			KernalPrintf("Processor");
			break;

		case ACPI_TYPE_THERMAL:
			KernalPrintf("Thermal");
			break;

		case ACPI_TYPE_BUFFER_FIELD:
			KernalPrintf("Field (Buffer)");
			break;

		case ACPI_TYPE_DDB_HANDLE:
			KernalPrintf("DDB Handle");
			break;

		case ACPI_TYPE_DEBUG_OBJECT:
			KernalPrintf("Debug");
			break;

		case ACPI_TYPE_LOCAL_REGION_FIELD:
			KernalPrintf("Field (Region)");
			break;

		case ACPI_TYPE_LOCAL_BANK_FIELD:
			KernalPrintf("Field (Bank)");
			break;

		case ACPI_TYPE_LOCAL_INDEX_FIELD:
			KernalPrintf("Field (Index)");
			break;

		case ACPI_TYPE_LOCAL_SCOPE:
			break;

		default:
			KernalPrintf("(%02X)", Type);
			break;


	};
}

ACPI_STATUS WalkCallback (ACPI_HANDLE Object, UINT32 NestingLevel, void *Context, void **ReturnValue)
{
	bool Res = *reinterpret_cast<bool *>(Context);
	
	//if(Info->Type != ACPI_TYPE_DEVICE && Info->Type != ACPI_TYPE_PROCESSOR && Info->Type != ACPI_TYPE_LOCAL_SCOPE)
	//	return(AE_CTRL_DEPTH);

	//if((Info->Valid & ACPI_VALID_STA) && (Info->CurrentStatus & ACPI_STA_DEVICE_PRESENT) == 0 && (Info->CurrentStatus & ACPI_STA_DEVICE_FUNCTIONING) == 0)
	//	return(AE_CTRL_DEPTH);


	if(Res == false)
	{
		ACPI_DEVICE_INFO *Info;
		AcpiGetObjectInfo(Object, &Info);

		for(uint32_t x = 0; x < NestingLevel; x++)
			KernalPrintf(" ");

		KernalPrintf("%4.4s: ", &Info->Name);
		PrintType(Info->Type);

		if(Info->Valid & ACPI_VALID_HID)
			KernalPrintf(" HID: %s", Info->HardwareId.String);

		if(Info->Valid & ACPI_VALID_ADR)
			KernalPrintf(" ADR: %08llX", Info->Address);

		if(Info->Valid & ACPI_VALID_CLS)
			KernalPrintf(" CLS: %s", Info->ClassCode.String);

		if(Info->Valid & ACPI_VALID_CLS)
			KernalPrintf(" CLS: %s", Info->ClassCode.String);

		if(Info->Valid & ACPI_VALID_STA)
		{
			KernalPrintf(" STA: ");

			if(Info->CurrentStatus & ACPI_STA_DEVICE_FUNCTIONING)
				KernalPrintf("F");	// Device is Functioning Correctly
			else
				KernalPrintf("-");

			if(Info->CurrentStatus & ACPI_STA_DEVICE_UI)
				KernalPrintf("V");	// Device should be Visable in the UI
			else
				KernalPrintf("-");

			if(Info->CurrentStatus & ACPI_STA_DEVICE_ENABLED)
				KernalPrintf("E");	// Device is Enabled and decoding
			else
				KernalPrintf("-");

			if(Info->CurrentStatus & ACPI_STA_DEVICE_PRESENT)
				KernalPrintf("P");	// Device is Present
			else
				KernalPrintf("-");
		}
		
		KernalPrintf("\n");
		ACPI_FREE(Info);
	}
	else
	{
		ACPI_BUFFER Path;
		char Buffer[1024];

		Path.Length = 1024;
		Path.Pointer = Buffer;

		AcpiGetName(Object, ACPI_FULL_PATHNAME_NO_TRAILING, &Path);
		KernalPrintf("%s\n", Buffer);

		Path.Length = 1024;

		ACPI_STATUS Status = AcpiGetCurrentResources(Object, &Path);

		if(Status == AE_OK)
		{
			AcpiWalkResourceBuffer(&Path, WalkResourceCallback, nullptr);
		}
	}

	
	return AE_OK;
}


void PrintFlags(uint16_t Flags)
{
	//printf("%04X ", Flags);
	switch (Flags & 0x03)
	{
		case 0:
			KernalPrintf("CONF ");
			break;

		case 1:
			KernalPrintf("HIGH ");
			break;

		case 2:
			KernalPrintf("---- ");
			break;

		case 3:
			KernalPrintf("LOW  ");
			break;
	}

	switch ((Flags >> 2) & 0x03)
	{
		case 0:
			KernalPrintf("CONF");
			break;

		case 1:
			KernalPrintf("EDGE");
			break;

		case 2:
			KernalPrintf("----");
			break;

		case 3:
			KernalPrintf("LVL ");
			break;
	}
}


void PrintMemoryBlock(void *Address, int Length, uint8_t Align);

void ACPI::Dump(char *Options)
{
	AcpiGbl_EnableAmlDebugObject = FALSE;
	AcpiGbl_DbOutputFlags = ACPI_DB_DISABLE_OUTPUT;
	AcpiGbl_DisableSsdtTableInstall = FALSE;

	//AcpiDbgLevel & AcpiDbgLayer
	//AcpiGbl_DbOutputFlags ACPI_DB_CONSOLE_OUTPUT

	ACPI_STATUS Status;
	Status = AcpiInitializeSubsystem ();
	Status = AcpiInitializeTables(nullptr, 16, false);
	Status = AcpiLoadTables();
	Status = AcpiEnableSubsystem(ACPI_FULL_INITIALIZATION);
	Status = AcpiInitializeObjects(ACPI_FULL_INITIALIZATION);
	
	if(AcpiGbl_FADT.ResetRegister.Address >= 0x0000 && AcpiGbl_FADT.ResetRegister.Address <= 0xFFFF)
	{
		AcpiGbl_FADT.Flags |= ACPI_FADT_RESET_REGISTER;
	}

	//ACPI_OBJECT Input;
	//Input.Type = ACPI_TYPE_INTEGER;
	//Input.Integer.Type = ACPI_TYPE_INTEGER;
	//Input.Integer.Value = 1;
	//
	//ACPI_OBJECT_LIST Params;
	//Params.Count = 1;
	//Params.Pointer = &Input;
	//
	//Status = AcpiEvaluateObject(ACPI_ROOT_OBJECT, "_PIC", &Params, nullptr);
	//KernalPrintf("%02X\n", Status);


	if(Options == nullptr)
	{
		ACPI_TABLE_HEADER *Blob;
		int x = 0;
		while(AcpiGetTableByIndex(x, &Blob) != AE_BAD_PARAMETER)
		{
			x++;
			KernalPrintf("  %08X: %4.4s OEMID: %6.6s Rev %02X, Length %04X\n", Blob, Blob->Signature, Blob->OemId, Blob->OemRevision, Blob->Length);
		}
	} 
	else if(_stricmp("OFF", Options) == 0)
	{
		Status = AcpiEnterSleepStatePrep(5);
		KernalPrintf("%02X-", Status);
		AcpiEnterSleepState(5);
		KernalPrintf("%02X\n", Status);
	}
	else if(_stricmp("RESET", Options) == 0)
	{
		Status = AcpiReset();
		KernalPrintf("%02X\n", Status);
	}
	else if(_stricmp("DSDT", Options) == 0)
	{
		bool Res = false;
		AcpiWalkNamespace(0, ACPI_ROOT_OBJECT, UINT32_MAX, WalkCallback, nullptr, &Res, nullptr);
	}
	else if(_stricmp("DEV", Options) == 0)
	{
		bool Res = false;
		AcpiGetDevices(NULL, WalkCallback, &Res, nullptr);
		//AcpiWalkNamespace(0, ACPI_ROOT_OBJECT, UINT32_MAX, WalkCallback, nullptr, &Res, nullptr);
	}
	else if(_stricmp("RES", Options) == 0)
	{
		bool Res = true;
		AcpiGetDevices(NULL, WalkCallback, &Res, nullptr);
		//AcpiWalkNamespace(0, ACPI_ROOT_OBJECT, UINT32_MAX, WalkCallback, nullptr, &Res, nullptr);
	}
	else if(_stricmp("FACP", Options) == 0)
	{
		printf(" Fixed ACPI Description Table (FADT) (%08X)\n", ACPI_FADT_V2_SIZE);
		printf(" FACS: %08X, DSDT: %08X\n", AcpiGbl_FADT.Facs, AcpiGbl_FADT.Dsdt);
		printf(" SCI Int: %04X, Port: %08X, ACPI Enable: %02X, ACPI Disable: %02X\n", AcpiGbl_FADT.SciInterrupt, AcpiGbl_FADT.SmiCommand, AcpiGbl_FADT.AcpiEnable, AcpiGbl_FADT.AcpiDisable); 
		printf(" PM1 Event Block   %08X/%08X, Length: %02x\n", AcpiGbl_FADT.Pm1aEventBlock, AcpiGbl_FADT.Pm1bEventBlock, AcpiGbl_FADT.Pm1EventLength);
		printf(" PM1 Control Block %08X/%08X, Length: %02x\n", AcpiGbl_FADT.Pm1aControlBlock, AcpiGbl_FADT.Pm1bControlBlock, AcpiGbl_FADT.Pm1ControlLength);
		printf(" PM2 Control Block %08X, Length: %02x\n", AcpiGbl_FADT.Pm2ControlBlock, AcpiGbl_FADT.Pm2ControlLength);
		printf(" PM Timer Block    %08X, Length: %02x\n", AcpiGbl_FADT.PmTimerBlock, AcpiGbl_FADT.PmTimerLength);
		printf(" GP0 Event Block   %08X, Length: %02x\n", AcpiGbl_FADT.Gpe0Block, AcpiGbl_FADT.Gpe0BlockLength);
		printf(" GP1 Event Block   %08X, Length: %02x, Base: %02X\n", AcpiGbl_FADT.Gpe1Block, AcpiGbl_FADT.Gpe1BlockLength, AcpiGbl_FADT.Gpe1Base);
		printf(" Boot Flags: %04X\n", AcpiGbl_FADT.BootFlags);
		printf("  Legacy: %c, 8042: %c\n", AcpiGbl_FADT.BootFlags & 0x01 ? 'Y' : 'N', AcpiGbl_FADT.BootFlags & 0x02 ? 'Y' : 'N');
		printf(" Flags: %08X\n", AcpiGbl_FADT.Flags);
		printf(" Reset Reg ");
		ACPIData::PrintGenAddress(AcpiGbl_FADT.ResetRegister);
		printf(", Reset Value: %02X\n", AcpiGbl_FADT.ResetValue);

	}
	else if(_stricmp("APIC", Options) == 0)
	{
		ACPI_TABLE_HEADER *Blob;
		AcpiGetTable("APIC", 0, &Blob);
		if(Blob == nullptr)
			return;
		
		ACPIData::MultiACPITable *Header = reinterpret_cast<ACPIData::MultiACPITable *>(Blob);

		KernalPrintf(" Local APIC: %08X %s\n", Header->LocalACPIAddress, (Header->Flags & 0x01) == 0x01 ? "- 8259 Compatability" : "");
				
		uint32_t Offset = sizeof(ACPIData::MultiACPITable);

		while(Offset < Blob->Length)
		{
			ACPIData::MultiACPITableEntry *Entry = reinterpret_cast<ACPIData::MultiACPITableEntry *>((uint32_t)Blob + Offset);

			switch(Entry->Type)
			{
				case 0:
					KernalPrintf(" Processor ID: %02X, APIC ID: %02X %s\n", Entry->ProcessorLocalAPIC.ACPIProcessorID, Entry->ProcessorLocalAPIC.APICID, Entry->ProcessorLocalAPIC.Flags ? "Enabled" : "");
					break;

				case 1:
					KernalPrintf(" I/O APCI: ID %02X, Address: %08X, Global Int Base: %02X\n", Entry->IOAPIC.IOAPICID, Entry->IOAPIC.IOAPCIAddress, Entry->IOAPIC.GlobalSystemIntBase);
					break;

				case 2:
					KernalPrintf(" APIC Source: ISA:%02X, Global Int: %02X, Flags: ", Entry->IntSourceOverride.Source, Entry->IntSourceOverride.GlobalSystemInt);
					PrintFlags(Entry->IntSourceOverride.Flags);
					KernalPrintf("\n");
					break;

				case 3:
					KernalPrintf(" NMI: Global Int: %08X, Flags: ", Entry->NMISource.GlobalSystemInt);
					PrintFlags(Entry->NMISource.Flags);
					KernalPrintf("\n");
					break;

				case 4:
					KernalPrintf(" ACPI NMI Proc ID: %02X, Local APIC LINT#: %02X, Flags: ", Entry->LocalAPICNMI.ACPIProcessorID, Entry->LocalAPICNMI.LINT);
					PrintFlags(Entry->LocalAPICNMI.Flags);
					KernalPrintf("\n");
					break;

				case 5:
					KernalPrintf(" ACPI Override: Address %08X\n", Entry->LocalAPICAddressOverride.LocalAPICAddress);
					break;

				case 9:
					KernalPrintf(" x2APIC Processor ID: %02X, APIC ID: %02X %s\n", Entry->ProcessorLocalx2APIC.ACPIProcessorUID, Entry->ProcessorLocalx2APIC.x2APICID, Entry->ProcessorLocalx2APIC.Flags ? "Enabled" : "");
					break;

				case 0x0A:
					KernalPrintf(" x2ACPI NMI Proc ID: %02X, Local APIC LINT#: %02X, Flags: ", Entry->Localx2APICNMI.ACPIProcessorUID, Entry->Localx2APICNMI.LINT);
					PrintFlags(Entry->Localx2APICNMI.Flags);
					KernalPrintf("\n");
					break;
						
				default:
					printf(" %X\n", Entry->Type);
					break;

			}

			Offset += Entry->Length;
		};
	}
	else
	{
		ACPI_TABLE_HEADER *Blob;
		uint32_t Instance = 1;
		if(strlen(Options) >= 5)
		{
			Instance = ('0' - Options[4]) + 1;
		}

		if(AcpiGetTable(Options, Instance, &Blob) == AE_OK)
		{
			PrintMemoryBlock(Blob, Blob->Length, 1);
		}
		else
		{
			KernalPrintf("  Table %s not found\n", Options);
		}
	}

	AcpiTerminate();
}

//******************************************************************************************

#define _COMPONENT          ACPI_OS_SERVICES
        ACPI_MODULE_NAME    ("ACPICpp")


/******************************************************************************
 *
 * FUNCTION:    AcpiOsInitialize
 *
 * PARAMETERS:  None
 *
 * RETURN:      Status
 *
 * DESCRIPTION: Init this OSL
 *
 *****************************************************************************/

ACPI_STATUS
AcpiOsInitialize (
    void)
{
	return (AE_OK);
}


/******************************************************************************
 *
 * FUNCTION:    AcpiOsTerminate
 *
 * PARAMETERS:  None
 *
 * RETURN:      Status
 *
 * DESCRIPTION: Nothing to do for windows
 *
 *****************************************************************************/

ACPI_STATUS
AcpiOsTerminate (
    void)
{
    return (AE_OK);
}

#ifndef ACPI_USE_NATIVE_RSDP_POINTER
/******************************************************************************
 *
 * FUNCTION:    AcpiOsGetRootPointer
 *
 * PARAMETERS:  None
 *
 * RETURN:      RSDP physical address
 *
 * DESCRIPTION: Gets the root pointer (RSDP)
 *
 *****************************************************************************/

ACPI_PHYSICAL_ADDRESS
AcpiOsGetRootPointer (
    void)
{
	//AcpiTbScanMemoryForRsdp(
	//return (ACPI_PHYSICAL_ADDRESS)Table;
		
	ACPI_PHYSICAL_ADDRESS Ret;
	AcpiFindRootPointer(&Ret);
	return Ret;
}
#endif

/******************************************************************************
 *
 * FUNCTION:    AcpiOsPredefinedOverride
 *
 * PARAMETERS:  InitVal             - Initial value of the predefined object
 *              NewVal              - The new value for the object
 *
 * RETURN:      Status, pointer to value. Null pointer returned if not
 *              overriding.
 *
 * DESCRIPTION: Allow the OS to override predefined names
 *
 *****************************************************************************/

ACPI_STATUS
AcpiOsPredefinedOverride (
    const ACPI_PREDEFINED_NAMES *InitVal,
    ACPI_STRING                 *NewVal)
{

    if (!InitVal || !NewVal)
    {
        return (AE_BAD_PARAMETER);
    }

    *NewVal = (ACPI_STRING)NULL;
    return (AE_OK);
}

/******************************************************************************
 *
 * FUNCTION:    AcpiOsTableOverride
 *
 * PARAMETERS:  ExistingTable       - Header of current table (probably firmware)
 *              NewTable            - Where an entire new table is returned.
 *
 * RETURN:      Status, pointer to new table. Null pointer returned if no
 *              table is available to override
 *
 * DESCRIPTION: Return a different version of a table if one is available
 *
 *****************************************************************************/

ACPI_STATUS
AcpiOsTableOverride (
    ACPI_TABLE_HEADER       *ExistingTable,
    ACPI_TABLE_HEADER       **NewTable)
{

    if (!ExistingTable || !NewTable)
    {
        return (AE_BAD_PARAMETER);
    }

    *NewTable = (ACPI_TABLE_HEADER *)NULL;

    return (AE_OK);
}

/******************************************************************************
 *
 * FUNCTION:    AcpiOsPhysicalTableOverride
 *
 * PARAMETERS:  ExistingTable       - Header of current table (probably firmware)
 *              NewAddress          - Where new table address is returned
 *                                    (Physical address)
 *              NewTableLength      - Where new table length is returned
 *
 * RETURN:      Status, address/length of new table. Null pointer returned
 *              if no table is available to override.
 *
 * DESCRIPTION: Returns AE_SUPPORT, function not used in user space.
 *
 *****************************************************************************/

ACPI_STATUS
AcpiOsPhysicalTableOverride (
    ACPI_TABLE_HEADER       *ExistingTable,
    ACPI_PHYSICAL_ADDRESS   *NewAddress,
    UINT32                  *NewTableLength)
{

    return (AE_SUPPORT);
}

// **************************************************************************************************************

/******************************************************************************
 *
 * FUNCTION:    AcpiOsMapMemory
 *
 * PARAMETERS:  Where               - Physical address of memory to be mapped
 *              Length              - How much memory to map
 *
 * RETURN:      Pointer to mapped memory. Null on error.
 *
 * DESCRIPTION: Map physical memory into caller's address space
 *
 *****************************************************************************/

void *
AcpiOsMapMemory (
    ACPI_PHYSICAL_ADDRESS   Where,
    ACPI_SIZE               Length)
{
	return (ACPI_TO_POINTER ((ACPI_SIZE) Where));
}


/******************************************************************************
 *
 * FUNCTION:    AcpiOsUnmapMemory
 *
 * PARAMETERS:  Where               - Logical address of memory to be unmapped
 *              Length              - How much memory to unmap
 *
 * RETURN:      None.
 *
 * DESCRIPTION: Delete a previously created mapping. Where and Length must
 *              correspond to a previous mapping exactly.
 *
 *****************************************************************************/

void
AcpiOsUnmapMemory (
    void                    *Where,
    ACPI_SIZE               Length)
{

    return;
}

/******************************************************************************
 *
 * FUNCTION:    AcpiOsAllocate
 *
 * PARAMETERS:  Size                - Amount to allocate, in bytes
 *
 * RETURN:      Pointer to the new allocation. Null on error.
 *
 * DESCRIPTION: Allocate memory. Algorithm is dependent on the OS.
 *
 *****************************************************************************/

void *
AcpiOsAllocate (
    ACPI_SIZE               Size)
{
	return malloc(Size);
	//return (NULL);
}

/******************************************************************************
 *
 * FUNCTION:    AcpiOsFree
 *
 * PARAMETERS:  Mem                 - Pointer to previously allocated memory
 *
 * RETURN:      None.
 *
 * DESCRIPTION: Free memory allocated via AcpiOsAllocate
 *
 *****************************************************************************/

void
AcpiOsFree (
    void                    *Mem)
{
    free(Mem);
}

/******************************************************************************
 *
 * FUNCTION:    AcpiOsReadable
 *
 * PARAMETERS:  Pointer             - Area to be verified
 *              Length              - Size of area
 *
 * RETURN:      TRUE if readable for entire length
 *
 * DESCRIPTION: Verify that a pointer is valid for reading
 *
 *****************************************************************************/

BOOLEAN
AcpiOsReadable (
    void                    *Pointer,
    ACPI_SIZE               Length)
{
    return FALSE;
}

/******************************************************************************
 *
 * FUNCTION:    AcpiOsWritable
 *
 * PARAMETERS:  Pointer             - Area to be verified
 *              Length              - Size of area
 *
 * RETURN:      TRUE if writable for entire length
 *
 * DESCRIPTION: Verify that a pointer is valid for writing
 *
 *****************************************************************************/

BOOLEAN
AcpiOsWritable (
    void                    *Pointer,
    ACPI_SIZE               Length)
{
    return FALSE;
}


// **************************************************************************************************************

ACPI_THREAD_ID
AcpiOsGetThreadId (
    void)
{
    return (1);
}

ACPI_STATUS
AcpiOsExecute (
    ACPI_EXECUTE_TYPE       Type,
    ACPI_OSD_EXEC_CALLBACK  Function,
    void                    *Context)
{
    Function (Context);
    return (AE_OK);
}

/******************************************************************************
 *
 * FUNCTION:    AcpiOsSleep
 *
 * PARAMETERS:  Milliseconds        - Time to sleep
 *
 * RETURN:      None. Blocks until sleep is completed.
 *
 * DESCRIPTION: Sleep at millisecond granularity
 *
 *****************************************************************************/

void
AcpiOsSleep (
    UINT64                  Milliseconds)
{
    return;
}

/******************************************************************************
 *
 * FUNCTION:    AcpiOsStall
 *
 * PARAMETERS:  Microseconds        - Time to stall
 *
 * RETURN:      None. Blocks until stall is completed.
 *
 * DESCRIPTION: Sleep at microsecond granularity
 *
 *****************************************************************************/

void
AcpiOsStall (
    UINT32                  Microseconds)
{
    return;
}

/******************************************************************************
 *
 * FUNCTION:    AcpiOsWaitEventsComplete
 *
 * PARAMETERS:  None
 *
 * RETURN:      None
 *
 * DESCRIPTION: Wait for all asynchronous events to complete. This
 *              implementation does nothing.
 *
 *****************************************************************************/

void
AcpiOsWaitEventsComplete (
    void)
{
	return;
}


// **************************************************************************************************************

/******************************************************************************
 *
 * FUNCTION:    Semaphore stub functions
 *
 * DESCRIPTION: Stub functions used for single-thread applications that do
 *              not require semaphore synchronization. Full implementations
 *              of these functions appear after the stubs.
 *
 *****************************************************************************/

ACPI_STATUS
AcpiOsCreateSemaphore (
    UINT32              MaxUnits,
    UINT32              InitialUnits,
    ACPI_HANDLE         *OutHandle)
{
    *OutHandle = (ACPI_HANDLE) 1;
    return (AE_OK);
}

ACPI_STATUS
AcpiOsDeleteSemaphore (
    ACPI_HANDLE         Handle)
{
    return (AE_OK);
}

ACPI_STATUS
AcpiOsWaitSemaphore (
    ACPI_HANDLE         Handle,
    UINT32              Units,
    UINT16              Timeout)
{
    return (AE_OK);
}

ACPI_STATUS
AcpiOsSignalSemaphore (
    ACPI_HANDLE         Handle,
    UINT32              Units)
{
    return (AE_OK);
}


/******************************************************************************
 *
 * FUNCTION:    Spinlock interfaces
 *
 * DESCRIPTION: Map these interfaces to semaphore interfaces
 *
 *****************************************************************************/

ACPI_STATUS
AcpiOsCreateLock (
    ACPI_SPINLOCK           *OutHandle)
{
    return (AcpiOsCreateSemaphore (1, 1, OutHandle));
}

void
AcpiOsDeleteLock (
    ACPI_SPINLOCK           Handle)
{
    AcpiOsDeleteSemaphore (Handle);
}

ACPI_CPU_FLAGS
AcpiOsAcquireLock (
    ACPI_SPINLOCK           Handle)
{
    AcpiOsWaitSemaphore (Handle, 1, 0xFFFF);
    return (0);
}

void
AcpiOsReleaseLock (
    ACPI_SPINLOCK           Handle,
    ACPI_CPU_FLAGS          Flags)
{
    AcpiOsSignalSemaphore (Handle, 1);
}

/******************************************************************************
 *
 * FUNCTION:    AcpiOsInstallInterruptHandler
 *
 * PARAMETERS:  InterruptNumber     - Level handler should respond to.
 *              ServiceRoutine      - Address of the ACPI interrupt handler
 *              Context             - User context
 *
 * RETURN:      Handle to the newly installed handler.
 *
 * DESCRIPTION: Install an interrupt handler. Used to install the ACPI
 *              OS-independent handler.
 *
 *****************************************************************************/

UINT32
AcpiOsInstallInterruptHandler (
    UINT32                  InterruptNumber,
    ACPI_OSD_HANDLER        ServiceRoutine,
    void                    *Context)
{

	return (AE_OK);
}


/******************************************************************************
 *
 * FUNCTION:    AcpiOsRemoveInterruptHandler
 *
 * PARAMETERS:  Handle              - Returned when handler was installed
 *
 * RETURN:      Status
 *
 * DESCRIPTION: Uninstalls an interrupt handler.
 *
 *****************************************************************************/

ACPI_STATUS
AcpiOsRemoveInterruptHandler (
    UINT32                  InterruptNumber,
    ACPI_OSD_HANDLER        ServiceRoutine)
{

	return (AE_OK);
}

// **************************************************************************************************************

/******************************************************************************
 *
 * FUNCTION:    AcpiOsReadMemory
 *
 * PARAMETERS:  Address             - Physical Memory Address to read
 *              Value               - Where value is placed
 *              Width               - Number of bits (8,16,32, or 64)
 *
 * RETURN:      Value read from physical memory address. Always returned
 *              as a 64-bit integer, regardless of the read width.
 *
 * DESCRIPTION: Read data from a physical memory address
 *
 *****************************************************************************/

ACPI_STATUS
AcpiOsReadMemory (
    ACPI_PHYSICAL_ADDRESS   Address,
    UINT64                  *Value,
    UINT32                  Width)
{
    switch (Width)
    {
    case 8:
    case 16:
    case 32:
    case 64:

        *Value = 0;
        break;

    default:

        return (AE_BAD_PARAMETER);
        break;
    }

    return (AE_OK);
}


/******************************************************************************
 *
 * FUNCTION:    AcpiOsWriteMemory
 *
 * PARAMETERS:  Address             - Physical Memory Address to write
 *              Value               - Value to write
 *              Width               - Number of bits (8,16,32, or 64)
 *
 * RETURN:      None
 *
 * DESCRIPTION: Write data to a physical memory address
 *
 *****************************************************************************/

ACPI_STATUS
AcpiOsWriteMemory (
    ACPI_PHYSICAL_ADDRESS   Address,
    UINT64                  Value,
    UINT32                  Width)
{
    return (AE_OK);
}

/******************************************************************************
 *
 * FUNCTION:    AcpiOsReadPort
 *
 * PARAMETERS:  Address             - Address of I/O port/register to read
 *              Value               - Where value is placed
 *              Width               - Number of bits
 *
 * RETURN:      Value read from port
 *
 * DESCRIPTION: Read data from an I/O port or register
 *
 *****************************************************************************/

ACPI_STATUS
AcpiOsReadPort (
    ACPI_IO_ADDRESS         Address,
    UINT32                  *Value,
    UINT32                  Width)
{
    ACPI_FUNCTION_NAME (OsReadPort);

    switch (Width)
    {
    case 8:
		*Value = InPortb((uint16_t)Address);
        break;

    case 16:
		*Value = InPortw((uint16_t)Address);
        //*Value = 0xFFFF;
        break;

    case 32:
		*Value = InPortd((uint16_t)Address);
        //*Value = 0xFFFFFFFF;
        break;

    default:

        ACPI_ERROR ((AE_INFO, "Bad width parameter: %X", Width));
        return (AE_BAD_PARAMETER);
    }

    return (AE_OK);
}


/******************************************************************************
 *
 * FUNCTION:    AcpiOsWritePort
 *
 * PARAMETERS:  Address             - Address of I/O port/register to write
 *              Value               - Value to write
 *              Width               - Number of bits
 *
 * RETURN:      None
 *
 * DESCRIPTION: Write data to an I/O port or register
 *
 *****************************************************************************/

ACPI_STATUS
AcpiOsWritePort (
    ACPI_IO_ADDRESS         Address,
    UINT32                  Value,
    UINT32                  Width)
{
    ACPI_FUNCTION_NAME (OsWritePort);

	switch(Width)
	{
		case 8:
			OutPortb((uint16_t)Address, Value);
			break;

		case 16:
			OutPortw((uint16_t)Address, Value);
			break;

		case 32:
			OutPortd((uint16_t)Address, Value);
			break;
		
		default:
			ACPI_ERROR ((AE_INFO, "Bad width parameter: %X", Width));
			return (AE_BAD_PARAMETER);

	};

	return (AE_OK);

}

/******************************************************************************
 *
 * FUNCTION:    AcpiOsReadPciConfiguration
 *
 * PARAMETERS:  PciId               - Seg/Bus/Dev
 *              Register            - Device Register
 *              Value               - Buffer where value is placed
 *              Width               - Number of bits
 *
 * RETURN:      Status
 *
 * DESCRIPTION: Read data from PCI configuration space
 *
 *****************************************************************************/

ACPI_STATUS
AcpiOsReadPciConfiguration (
    ACPI_PCI_ID             *PciId,
    UINT32                  Register,
    UINT64                  *Value,
    UINT32                  Width)
{
	uint32_t DevID = PCI::BuildDeviceID((uint8_t)PciId->Bus, (uint8_t)PciId->Device, (uint8_t)PciId->Function);
	*Value = PCI::ReadRegister(DevID, Register, Width / 8);
	
	//KernalPrintf("RI, %X:%X:%X:%X - %X\n", PciId->Segment, PciId->Bus, PciId->Device, PciId->Function, Register);
    //*Value = 0;
    return (AE_OK);
}


/******************************************************************************
 *
 * FUNCTION:    AcpiOsWritePciConfiguration
 *
 * PARAMETERS:  PciId               - Seg/Bus/Dev
 *              Register            - Device Register
 *              Value               - Value to be written
 *              Width               - Number of bits
 *
 * RETURN:      Status
 *
 * DESCRIPTION: Write data to PCI configuration space
 *
 *****************************************************************************/

ACPI_STATUS
AcpiOsWritePciConfiguration (
    ACPI_PCI_ID             *PciId,
    UINT32                  Register,
    UINT64                  Value,
    UINT32                  Width)
{
	uint32_t DevID = PCI::BuildDeviceID((uint8_t)PciId->Bus, (uint8_t)PciId->Device, (uint8_t)PciId->Function);
	PCI::SetRegister(DevID, Register, (uint32_t) Value, Width / 8);

	return (AE_OK);
}


// **************************************************************************************************************

/******************************************************************************
 *
 * FUNCTION:    AcpiOsPrintf
 *
 * PARAMETERS:  Fmt, ...            - Standard printf format
 *
 * RETURN:      None
 *
 * DESCRIPTION: Formatted output
 *
 *****************************************************************************/

void ACPI_INTERNAL_VAR_XFACE
AcpiOsPrintf (
    const char              *Fmt,
    ...)
{
    va_list                 Args;
    UINT8                   Flags;


    Flags = AcpiGbl_DbOutputFlags;
    if (Flags & ACPI_DB_REDIRECTABLE_OUTPUT)
    {
        /* Output is directable to either a file (if open) or the console */

        //if (AcpiGbl_DebugFile)
        //{
        //    /* Output file is open, send the output there */

        //    va_start (Args, Fmt);
        //    vfprintf (AcpiGbl_DebugFile, Fmt, Args);
        //    va_end (Args);
        //}
        //else
        {
            /* No redirection, send output to console (once only!) */

            Flags |= ACPI_DB_CONSOLE_OUTPUT;
        }
    }

    if (Flags & ACPI_DB_CONSOLE_OUTPUT)
    {
        va_start (Args, Fmt);
		KernalVprintf(Fmt, Args);
        va_end (Args);
    }

    return;
}


/******************************************************************************
 *
 * FUNCTION:    AcpiOsVprintf
 *
 * PARAMETERS:  Fmt                 - Standard printf format
 *              Args                - Argument list
 *
 * RETURN:      None
 *
 * DESCRIPTION: Formatted output with argument list pointer
 *
 *****************************************************************************/

void
AcpiOsVprintf (
    const char              *Fmt,
    va_list                 Args)
{
    INT32                   Count = 0;
    UINT8                   Flags;


    Flags = AcpiGbl_DbOutputFlags;
    if (Flags & ACPI_DB_REDIRECTABLE_OUTPUT)
    {
        /* Output is directable to either a file (if open) or the console */

        //if (AcpiGbl_DebugFile)
        //{
        //    /* Output file is open, send the output there */

        //    Count = vfprintf (AcpiGbl_DebugFile, Fmt, Args);
        //}
        //else
        {
            /* No redirection, send output to console (once only!) */

            Flags |= ACPI_DB_CONSOLE_OUTPUT;
        }
    }

    if (Flags & ACPI_DB_CONSOLE_OUTPUT)
    {
        Count = KernalVprintf(Fmt, Args);
    }

    return;
}

/******************************************************************************
 *
 * FUNCTION:    AcpiOsRedirectOutput
 *
 * PARAMETERS:  Destination         - An open file handle/pointer
 *
 * RETURN:      None
 *
 * DESCRIPTION: Causes redirect of AcpiOsPrintf and AcpiOsVprintf
 *
 *****************************************************************************/

void
AcpiOsRedirectOutput (
    void                    *Destination)
{

    //AcpiGbl_OutputFile = Destination;
}


// **************************************************************************************************************


/******************************************************************************
 *
 * FUNCTION:    AcpiOsGetTimer
 *
 * PARAMETERS:  None
 *
 * RETURN:      Current ticks in 100-nanosecond units
 *
 * DESCRIPTION: Get the value of a system timer
 *
 ******************************************************************************/

UINT64
AcpiOsGetTimer (
    void)
{
	return 0;
}



/******************************************************************************
 *
 * FUNCTION:    AcpiOsSignal
 *
 * PARAMETERS:  Function            - ACPICA signal function code
 *              Info                - Pointer to function-dependent structure
 *
 * RETURN:      Status
 *
 * DESCRIPTION: Miscellaneous functions. Example implementation only.
 *
 *****************************************************************************/

ACPI_STATUS
AcpiOsSignal (
    UINT32                  Function,
    void                    *Info)
{

	return (AE_OK);
}


/******************************************************************************
 *
 * FUNCTION:    AcpiOsGetLine
 *
 * PARAMETERS:  Buffer              - Where to return the command line
 *              BufferLength        - Maximum length of Buffer
 *              BytesRead           - Where the actual byte count is returned
 *
 * RETURN:      Status and actual bytes read
 *
 * DESCRIPTION: Formatted input with argument list pointer
 *
 *****************************************************************************/

ACPI_STATUS
AcpiOsGetLine (
    char                    *Buffer,
    UINT32                  BufferLength,
    UINT32                  *BytesRead)
{

    return (AE_OK);
}
