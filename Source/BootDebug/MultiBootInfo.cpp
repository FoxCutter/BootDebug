#include "MultiBootInfo.h"
#include "MultiBoot.h"
#include "MultiBoot2.h"

#include "Utility.h"

#include <stdio.h>
#include <string.h>

MultiBootInfo::MultiBootInfo(void)
{
	MemoryLow = MemoryLow = 0;
	BootDevice = BootPartition[0] = BootPartition[1] = BootPartition[2] = 0xFF;
	CommandLine = BootLoader = nullptr;
	MBData = nullptr;
	HeaderLength = 0;
	MemoryMapData = nullptr;
	CurrentModuleData = nullptr;
	CurrentMemoryMapData = 0;
}

MultiBootInfo::~MultiBootInfo(void)
{
}

bool MultiBootInfo::LoadMultiBootInfo(uint32_t Signature, void *Data)
{
	MBData = Data;

	if(Signature == MulitBoot::BootMagic)
	{
		return LoadMultiBoot1Info(Data);
	}
	else if(Signature == MulitBoot2::BootMagic)
	{
		return LoadMultiBoot2Info(Data);
	}
	else
	{
		return false;
	}

	return true;
}

void MultiBootInfo::Dump()
{
	printf("MultiBoot Info (%08X), size: %08X\n", MBData, HeaderLength);
	printf("    Version: %u\n", Type);
	printf("    Memory Info: Low %uk, High %uk\n", MemoryLow, MemoryHigh);
	printf("    Boot Device Info: Device %02X, Partition %02X:%02X:%02X\n", BootDevice, BootPartition[0], BootPartition[1], BootPartition[2]);
	printf("    Command Line: %s\n", CommandLine);
	printf("    Boot Loader: %s\n", BootLoader);
	printf("    Memory Map: %08X\n", MemoryMapData);
	printf("    Frame Buffer: %08X (%02X) %02ux%02u, P:%04X, BPP:%02X\n", (unsigned int)FrameBuffer.Address, FrameBuffer.Type, FrameBuffer.Width, FrameBuffer.Height, FrameBuffer.Pitch, FrameBuffer.BPP);

	MemoryMapEntry CurrentEntry;
	if(GetFirstMemoryEntry(CurrentEntry))
	{
		uint64_t TotalRam = 0;
		uint64_t TotalReserved = 0;

		printf("           Start                End               Length       Type\n");

		do
		{
			printf("     ");
			PrintLongAddress(CurrentEntry.BaseAddress);
			printf(" - ");
			PrintLongAddress(CurrentEntry.BaseAddress + CurrentEntry.Length - 1);
			printf(", ");
			PrintLongAddress(CurrentEntry.Length);
			printf(", ");

			switch(CurrentEntry.Type)
			{
				case 1:
					printf("RAM");
					TotalRam += CurrentEntry.Length;
					break;

				default:
				case 2:
					printf("Reserved");
					TotalReserved += CurrentEntry.Length;
					break;

				case 3:
					printf("Recoverable");
					TotalRam += CurrentEntry.Length;
					break;

				case 4:
					printf("Hibernation");
					TotalReserved += CurrentEntry.Length;
					break;
			}

			printf(" (%u)\n", CurrentEntry.Type);
		} while(GetNextMemoryEntry(CurrentEntry));
	
		printf("     Totals:\n      RAM:      ");
		PrintLongAddress(TotalRam);
		printf("\n      Reserved: ");
		PrintLongAddress(TotalReserved);
		printf("\n      Memory:   ");
		PrintLongAddress(TotalRam + TotalReserved + 0x10000);
		printf("\n");
	}

	printf("    Loaded Modules:\n");
	ModuleEntry CurrentModule;
	if(GetFirstModuleEntry(CurrentModule))
	{
		printf("           Start                End        Command Line\n");

		do
		{
			printf("     ");
			PrintLongAddress(CurrentModule.ModStart);
			printf(" - ");
			PrintLongAddress(CurrentModule.ModEnd);
			printf(" %s\n", CurrentModule.CommandLine);

		} while (GetNextModuleEntry(CurrentModule));
	}

}

bool MultiBootInfo::GetFirstMemoryEntry(MemoryMapEntry &Entry)
{
	if(MemoryMapData == nullptr)
	{
		Entry.BaseAddress = UINT64_MAX;
		Entry.Length = UINT64_MAX;
		Entry.Type = UINT32_MAX;

		return false;
	}

	if(Type == Version1)
	{
		MulitBoot::Boot_Header * BootHeader = (MulitBoot::Boot_Header *)MBData;
		CurrentMemoryMapData = reinterpret_cast<intptr_t>(MemoryMapData);

		Entry = *reinterpret_cast<MemoryMapEntry *>(CurrentMemoryMapData + sizeof(uint32_t));
	}
	else if(Type == Version2)
	{
		MulitBoot2::Boot_MemoryMap* MapEntry = (MulitBoot2::Boot_MemoryMap *)MemoryMapData;
		CurrentMemoryMapData = reinterpret_cast<intptr_t>(MemoryMapData) + sizeof(MulitBoot2::Boot_MemoryMap);

		Entry = *reinterpret_cast<MemoryMapEntry *>(CurrentMemoryMapData);
	}
	
	return true;
}

bool MultiBootInfo::GetNextMemoryEntry(MemoryMapEntry &Entry)
{
	if(MemoryMapData == nullptr || CurrentMemoryMapData == 0)
	{
		Entry.BaseAddress = UINT64_MAX;
		Entry.Length = UINT64_MAX;
		Entry.Type = UINT32_MAX;

		return false;
	}

	if(Type == Version1)
	{
		MulitBoot::Boot_Header * BootHeader = (MulitBoot::Boot_Header *)MBData;

		uint32_t Size = *reinterpret_cast<uint32_t *>(CurrentMemoryMapData);

		if(CurrentMemoryMapData + sizeof(uint32_t) + Size >= BootHeader->MemMap_Address + BootHeader->MemMap_Length)
		{
			Entry.BaseAddress = UINT64_MAX;
			Entry.Length = UINT64_MAX;
			Entry.Type = UINT32_MAX;

			return false;
		}

		CurrentMemoryMapData += sizeof(uint32_t) + Size;
		Entry = *reinterpret_cast<MemoryMapEntry *>(CurrentMemoryMapData + sizeof(uint32_t));
	}
	else if(Type == Version2)
	{
		MulitBoot2::Boot_MemoryMap* MapEntry = (MulitBoot2::Boot_MemoryMap *)MemoryMapData;
		
		if(CurrentMemoryMapData + MapEntry->Entry_Size >= reinterpret_cast<intptr_t>(MemoryMapData) + MapEntry->Size)
		{
			Entry.BaseAddress = UINT64_MAX;
			Entry.Length = UINT64_MAX;
			Entry.Type = UINT32_MAX;

			return false;
		}

		CurrentMemoryMapData += MapEntry->Entry_Size;

		Entry = *reinterpret_cast<MemoryMapEntry *>(CurrentMemoryMapData);
	}

	return true;

}

bool MultiBootInfo::GetFirstModuleEntry(ModuleEntry &Entry)
{
	if(Type == Version1)
	{
		MulitBoot::Boot_Header * BootHeader = (MulitBoot::Boot_Header *)MBData;

		if((BootHeader->Flags & MulitBoot::ModuleInfo) == 0)
		{
			Entry.ModStart = 0;
			Entry.ModEnd = 0;
			Entry.CommandLine = nullptr;

			return false;
		}

		CurrentModuleData = (void *)BootHeader->Mod_Address;
		MulitBoot::Boot_Module *ModEntry = (MulitBoot::Boot_Module *)BootHeader->Mod_Address;		

		Entry.ModStart = ModEntry->ModStart;
		Entry.ModEnd = ModEntry->ModEnd;
		Entry.CommandLine = reinterpret_cast<char *>(ModEntry->String);		
	}
	else if(Type == Version2)
	{
		MulitBoot2::Boot_Header * BootHeader = (MulitBoot2::Boot_Header *)MBData;

		unsigned char *Pos = (unsigned char *)MBData;
		unsigned char *End = (unsigned char *)MBData + BootHeader->Size;
		Pos += sizeof(MulitBoot2::Boot_Header);

		while(Pos < End)
		{			
			while ((int)Pos % 8 != 0)
				Pos++;

			MulitBoot2::Boot_Entry * CurrentHeader = (MulitBoot2::Boot_Entry *)Pos;

			if(CurrentHeader->Type == MulitBoot2::Modules)
			{
				CurrentModuleData = Pos;
				
				MulitBoot2::Boot_Module * ModEntry = (MulitBoot2::Boot_Module *)Pos;

				Entry.ModStart = ModEntry->ModStart;
				Entry.ModEnd = ModEntry->ModEnd;
				Entry.CommandLine = ModEntry->String;	

				return true;
			}
		
			Pos += CurrentHeader->Size;
		};

		Entry.ModStart = 0;
		Entry.ModEnd = 0;
		Entry.CommandLine = nullptr;

		return false;
	}
	
	return true;
}

bool MultiBootInfo::GetNextModuleEntry(ModuleEntry &Entry)
{
	if(CurrentModuleData == nullptr)
	{
		Entry.ModStart = 0;
		Entry.ModEnd = 0;
		Entry.CommandLine = nullptr;

		return false;
	}

	if(Type == Version1)
	{
		MulitBoot::Boot_Header * BootHeader = (MulitBoot::Boot_Header *)MBData;

		MulitBoot::Boot_Module *ModEntry = reinterpret_cast<MulitBoot::Boot_Module *>(CurrentModuleData);		

		if(reinterpret_cast<intptr_t>(&ModEntry[1]) >= BootHeader->Mod_Address + static_cast<intptr_t>(BootHeader->Mod_Count * sizeof(MulitBoot::Boot_Module)))
		{
			Entry.ModStart = 0;
			Entry.ModEnd = 0;
			Entry.CommandLine = nullptr;

			return false;
		}

		CurrentModuleData = &ModEntry[1];

		Entry.ModStart = ModEntry[1].ModStart;
		Entry.ModEnd = ModEntry[1].ModEnd;
		Entry.CommandLine = reinterpret_cast<char *>(ModEntry[1].String);		
	}
	else if(Type == Version2)
	{
		MulitBoot2::Boot_Header * BootHeader = (MulitBoot2::Boot_Header *)MBData;

		unsigned char *Pos = (unsigned char *)CurrentModuleData;
		unsigned char *End = (unsigned char *)MBData + BootHeader->Size;
		MulitBoot2::Boot_Entry * CurrentHeader = (MulitBoot2::Boot_Entry *)Pos;
		Pos += CurrentHeader->Size;

		while(Pos < End)
		{
			while ((int)Pos % 8 != 0)
				Pos++;

			CurrentHeader = (MulitBoot2::Boot_Entry *)Pos;

			if(CurrentHeader->Type == MulitBoot2::Modules)
			{
				CurrentModuleData = Pos;
				
				MulitBoot2::Boot_Module * ModEntry = (MulitBoot2::Boot_Module *)Pos;

				Entry.ModStart = ModEntry->ModStart;
				Entry.ModEnd = ModEntry->ModEnd;
				Entry.CommandLine = ModEntry->String;	

				return true;
			}
		
			Pos += CurrentHeader->Size;
		};

		Entry.ModStart = 0;
		Entry.ModEnd = 0;
		Entry.CommandLine = nullptr;

		return false;
	}

	return true;

}

bool MultiBootInfo::LoadMultiBoot1Info(void *Data)
{
	Type = Version1;
	
	// Right now our main job is to pillage out all the information we can from the header
	MulitBoot::Boot_Header * BootHeader = (MulitBoot::Boot_Header *)Data;
	
	intptr_t TempAddress = 0;
	intptr_t LastAddress = reinterpret_cast<intptr_t>(BootHeader) + sizeof(MulitBoot::Boot_Header);
	
	if((BootHeader->Flags & MulitBoot::MemoryInfo) == MulitBoot::MemoryInfo)
	{
		MemoryLow = BootHeader->Memory_Lower;
		MemoryHigh = BootHeader->Memory_Upper;
	}
	if((BootHeader->Flags & MulitBoot::BootDeviceInfo) == MulitBoot::BootDeviceInfo)
	{
		BootDevice =       (BootHeader->BootDevice & 0xFF000000) >> 24;
		BootPartition[0] = (BootHeader->BootDevice & 0x00FF0000) >> 16;
		BootPartition[1] = (BootHeader->BootDevice & 0x0000FF00) >> 8;
		BootPartition[2] = (BootHeader->BootDevice & 0x000000FF);
	}
	if((BootHeader->Flags & MulitBoot::CommandLineInfo) == MulitBoot::CommandLineInfo)
	{
		CommandLine = reinterpret_cast<char *>(BootHeader->CommandLine);

		TempAddress = BootHeader->CommandLine + strlen(CommandLine);

		if(TempAddress > LastAddress)
			LastAddress = TempAddress;
	}
	if((BootHeader->Flags & MulitBoot::ModuleInfo) == MulitBoot::ModuleInfo)
	{
		MulitBoot::Boot_Module *Entry = (MulitBoot::Boot_Module *)BootHeader->Mod_Address;

		TempAddress = BootHeader->Mod_Address + (sizeof(MulitBoot::Boot_Module) * BootHeader->Mod_Count);

		if(TempAddress > LastAddress)
			LastAddress = TempAddress;
	}
 
	// We don't care about symbols
	if((BootHeader->Flags & MulitBoot::AOutSymbols) == MulitBoot::AOutSymbols)
	{
		TempAddress = BootHeader->AOutSymbolInfo.Address + BootHeader->AOutSymbolInfo.StringSize + BootHeader->AOutSymbolInfo.TabSize + BootHeader->AOutSymbolInfo.Reserved;

		if(TempAddress > LastAddress)
			LastAddress = TempAddress;

	}
	if((BootHeader->Flags & MulitBoot::ELFSymbols) == MulitBoot::ELFSymbols)
	{
		TempAddress = BootHeader->ELFScection.Address + (BootHeader->ELFScection.EntrySize * BootHeader->ELFScection.Num);

		if(TempAddress > LastAddress)
			LastAddress = TempAddress;
	} 
	
	if((BootHeader->Flags & MulitBoot::MemoryMapInfo) == MulitBoot::MemoryMapInfo)
	{
		MemoryMapData = reinterpret_cast<void *>(BootHeader->MemMap_Address);

		TempAddress = BootHeader->MemMap_Address + BootHeader->MemMap_Length;

		if(TempAddress > LastAddress)
			LastAddress = TempAddress;
	}	
	
	if((BootHeader->Flags & MulitBoot::DriveInfo) == MulitBoot::DriveInfo)
	{
		TempAddress = BootHeader->Drives_Address + BootHeader->Drives_Length;

		if(TempAddress > LastAddress)
			LastAddress = TempAddress;

	}
	/*
	if((BootHeader->Flags & MulitBoot::ConfigTable) == MulitBoot::ConfigTable)
	{
	}
	*/

	if((BootHeader->Flags & MulitBoot::BootLoaderName) == MulitBoot::BootLoaderName)
	{
		BootLoader = reinterpret_cast<char *>(BootHeader->BootLoaderName);

		TempAddress = BootHeader->BootLoaderName + strlen(BootLoader);

		if(TempAddress > LastAddress)
			LastAddress = TempAddress;

	}
	if((BootHeader->Flags & MulitBoot::ApmTable) == MulitBoot::ApmTable)
	{
		MulitBoot::Boot_APMTAble * Entry = (MulitBoot::Boot_APMTAble *)BootHeader->APMTable;
		
		//printf(" APM Table: V: %04X, CS: %04X, CL: %04X, CO: %08X\n", Entry->Version, Entry->CodeSeg, Entry->CodeSegLength, Entry->Offset);

		TempAddress = BootHeader->APMTable + sizeof(MulitBoot::Boot_APMTAble);

		if(TempAddress > LastAddress)
			LastAddress = TempAddress;

	}
	if((BootHeader->Flags & MulitBoot::VBEInfo) == MulitBoot::VBEInfo)
	{
		TempAddress = BootHeader->VBE_ControlInfo + 512;

		if(TempAddress > LastAddress)
			LastAddress = TempAddress;
		
		TempAddress = BootHeader->VBE_ModeInfo + 256;

		if(TempAddress > LastAddress)
			LastAddress = TempAddress;

		//printf(" VBE: Mode %04X, Seg: %04X, Offset: %04X, Length %04X\n", BootHeader->VBE_Mode, BootHeader->VBE_IntefaceSegment, BootHeader->VBE_IntefaceOffset, BootHeader->VBE_IntefaceLength);
	}
	if((BootHeader->Flags & MulitBoot::FrameBufferInfo) == MulitBoot::FrameBufferInfo)
	{
		FrameBuffer.Type = BootHeader->FrameBuffer_Type;
		FrameBuffer.Address = BootHeader->FrameBuffer_Address;
		FrameBuffer.Pitch = BootHeader->FrameBuffer_Pitch;
		FrameBuffer.Width = BootHeader->FrameBuffer_Width;
		FrameBuffer.Height = BootHeader->FrameBuffer_Height;
		FrameBuffer.BPP = BootHeader->FrameBuffer_BPP;
		
		//printf(" Frame Buffer Info\n");
		//printf("  Addr:%08X, P:%04X, W:%04X, H:%04X BPP:%02X\n", (unsigned int)BootHeader->FrameBuffer_Address, BootHeader->FrameBuffer_Pitch, BootHeader->FrameBuffer_Width, BootHeader->FrameBuffer_Height, BootHeader->FrameBuffer_BPP);
		//printf("  Type: %02X\n", BootHeader->FrameBuffer_Type);
		//printf("  Pallet: A: %08X C: %04X\n", BootHeader->PalletInfo.Address, BootHeader->PalletInfo.NumColors);
	}

	HeaderLength = LastAddress - reinterpret_cast<intptr_t>(BootHeader);
	return true;
}

bool MultiBootInfo::LoadMultiBoot2Info(void *Data)
{
	Type = Version2;

	MulitBoot2::Boot_Header * BootHeader = (MulitBoot2::Boot_Header *)Data;

	unsigned char *Pos = (unsigned char *)Data;
	unsigned char *End = (unsigned char *)Data + BootHeader->Size;
	Pos += sizeof(MulitBoot2::Boot_Header);

	// This one is easy
	HeaderLength = BootHeader->Size;

	while(Pos < End)
	{
        while ((int)Pos % 8 != 0)
            Pos++;

		MulitBoot2::Boot_Entry * CurrentHeader = (MulitBoot2::Boot_Entry *)Pos;

		switch(CurrentHeader->Type)
		{
			case MulitBoot2::CommandLine:
			{
				MulitBoot2::Boot_CommandLine * Entry = (MulitBoot2::Boot_CommandLine *)Pos;
				CommandLine = reinterpret_cast<char *>(Entry->String);
				break;
			}

			case MulitBoot2::BootLoaderName:
			{
				MulitBoot2::Boot_BootLoaderName * Entry = (MulitBoot2::Boot_BootLoaderName *)Pos;
				BootLoader = Entry->String;
				break;
			}

			case MulitBoot2::Modules:
			{
				MulitBoot2::Boot_Module * Entry = (MulitBoot2::Boot_Module *)Pos;
				//printf(" Module: %08X, %08X, %s\n", Entry->ModStart, Entry->ModEnd, Entry->String);
				break;
			}
			 
			case MulitBoot2::BasicMemory:
			{
				MulitBoot2::Boot_BasicMemory* Entry = (MulitBoot2::Boot_BasicMemory *)Pos;
				MemoryLow = Entry->Lower;
				MemoryHigh = Entry->Upper;
				break;
			}

			case MulitBoot2::BIOSBootDevice:
			{
				MulitBoot2::Boot_BIOSBootDevice* Entry = (MulitBoot2::Boot_BIOSBootDevice *)Pos;

				BootDevice = Entry->BiosDev;
				BootPartition[0] = Entry->Partition;
				BootPartition[1] = Entry->SubPartition;
				BootPartition[2] = UINT32_MAX;

				break;
			}

			case MulitBoot2::MemoryMap:
			{
				MemoryMapData = Pos;
				break;
			}

			case MulitBoot2::VBEInfo:
			{
				MulitBoot2::Boot_VBEInfo* Entry = (MulitBoot2::Boot_VBEInfo *)Pos;
				//printf(" VBEInfo: Mode %04X, Seg: %04X, Offset: %04X, Length %04X\n", Entry->Mode, Entry->IntefaceSegment, Entry->IntefaceOffset, Entry->IntefaceLength);
				break;
			}

			case MulitBoot2::FrameBufferInfo:
			{
				MulitBoot2::Boot_FrameBufferInfo* Entry = (MulitBoot2::Boot_FrameBufferInfo *)Pos;
				FrameBuffer.Type = Entry->FrameBufferType;
				FrameBuffer.Address = Entry->Address;
				FrameBuffer.Pitch = Entry->Pitch;
				FrameBuffer.Width = Entry->Width;
				FrameBuffer.Height = Entry->Height;
				FrameBuffer.BPP = Entry->BPP;

		
				//printf(" Frame Buffer Info\n");
				//printf("  Addr:%08X, P:%04X, W:%04X, H:%04X BPP:%02X\n", (unsigned int)Entry->Address, Entry->Pitch, Entry->Width, Entry->Height, Entry->BPP);
				//printf("  Type: %02X\n", Entry->FrameBufferType);
				break;
			}

			case MulitBoot2::APMTable:
			{
				MulitBoot2::Boot_APMTAble* Entry = (MulitBoot2::Boot_APMTAble *)Pos;
				//printf(" APM Table: V: %04X, CS: %04X, CL: %04X, CO: %08X\n", Entry->Version, Entry->CodeSeg, Entry->CodeSegLength, Entry->Offset);
				break;
			}

			default:
			case MulitBoot2::ELFSymbols:
			case MulitBoot2::BootDataEnd:
				break;
		}
		
		Pos += CurrentHeader->Size;
	}

	return true;
}
