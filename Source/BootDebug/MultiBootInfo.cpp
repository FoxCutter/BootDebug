#include "MultiBootInfo.h"
#include "MultiBoot.h"
#include "MultiBoot2.h"

#include <stdio.h>

MultiBootInfo::MultiBootInfo(void)
{
}

MultiBootInfo::~MultiBootInfo(void)
{
}

bool MultiBootInfo::LoadMultiBootInfo(uint32_t Signature, void *Data)
{
	if(Signature == MulitBoot::BootMagic)
	{
		return LoadMult1Boot1Info(Data);
	}
	else if(Signature == MulitBoot2::BootMagic)
	{
		return LoadMult2Boot1Info(Data);
	}
	else
	{
		return false;
	}

	return true;
}

bool MultiBootInfo::LoadMult1Boot1Info(void *Data)
{
	// Right now our main job is to pillage out all the information we can from the header
	MulitBoot::Boot_Header * BootHeader = (MulitBoot::Boot_Header *)Data;
	
	//printf("Multiboot %08X\n", Data);

	if((BootHeader->Flags & MulitBoot::MemoryInfo) == MulitBoot::MemoryInfo)
	{
		//printf(" Basic Memory: %08X, %08x\n", BootHeader->Memory_Lower, BootHeader->Memory_Upper);
	}
	if((BootHeader->Flags & MulitBoot::BootDeviceInfo) == MulitBoot::BootDeviceInfo)
	{
		//printf(" Boot Device:  %08X\n", BootHeader->BootDevice);
	}
	if((BootHeader->Flags & MulitBoot::CommandLineInfo) == MulitBoot::CommandLineInfo)
	{
		CommandLine = reinterpret_cast<char *>(BootHeader->CommandLine);
	}
	if((BootHeader->Flags & MulitBoot::ModuleInfo) == MulitBoot::ModuleInfo)
	{
		MulitBoot::Boot_Module *Entry = (MulitBoot::Boot_Module *)BootHeader->Mod_Address;

		//for(int x = 0; x < BootHeader->Mod_Count; x++)
		//{
		//	printf("Mod S:%08X E:%08X N:%s\n", Entry[x].ModStart, Entry[x].ModEnd, Entry[x].String);
		//}
		
		//printf(" Mods\n");
	}

	// We don't care about symbols
	//if((BootHeader->Flags & MulitBoot::AOutSymbols) == MulitBoot::AOutSymbols)
	//if((BootHeader->Flags & MulitBoot::ELFSymbols) == MulitBoot::ELFSymbols)

	MemoryMapLength = 0;
	if((BootHeader->Flags & MulitBoot::MemoryMapInfo) == MulitBoot::MemoryMapInfo)
	{
		intptr_t Base = 0;		

		while(Base < (intptr_t) BootHeader->MemMap_Length && MemoryMapLength < MemoryMapMaxLength)
		{
			uint32_t *Size = (uint32_t *)(BootHeader->MemMap_Address + Base);
			Base += sizeof(uint32_t);

			MemoryMap[MemoryMapLength] = (MemoryMapEntry *)(BootHeader->MemMap_Address + Base);

			Base += *Size;

			MemoryMapLength++;
		}
	}
	if((BootHeader->Flags & MulitBoot::DriveInfo) == MulitBoot::DriveInfo)
	{
		//printf(" Drive Info\n");
	}
	if((BootHeader->Flags & MulitBoot::ConfigTable) == MulitBoot::ConfigTable)
	{
		//printf(" Config Table\n");
	}
	if((BootHeader->Flags & MulitBoot::BootLoaderName) == MulitBoot::BootLoaderName)
	{
		//printf(" Boot Loader Name: %s\n", BootHeader->BootLoaderName);
	}
	if((BootHeader->Flags & MulitBoot::ApmTable) == MulitBoot::ApmTable)
	{
		//MulitBoot::Boot_APMTAble * Entry = (MulitBoot::Boot_APMTAble *)BootHeader->APMTable;
		//
		//printf(" APM Table: V: %04X, CS: %04X, CL: %04X, CO: %08X\n", Entry->Version, Entry->CodeSeg, Entry->CodeSegLength, Entry->Offset);
	}
	if((BootHeader->Flags & MulitBoot::VBEInfo) == MulitBoot::VBEInfo)
	{
		//printf(" VBE: Mode %04X, Seg: %04X, Offset: %04X, Length %04X\n", BootHeader->VBE_Mode, BootHeader->VBE_IntefaceSegment, BootHeader->VBE_IntefaceOffset, BootHeader->VBE_IntefaceLength);
	}
	if((BootHeader->Flags & MulitBoot::FrameBufferInfo) == MulitBoot::FrameBufferInfo)
	{
		//printf(" Frame Buffer Info\n");
		//printf("  Addr:%08X, P:%04X, W:%04X, H:%04X BPP:%02X\n", (unsigned int)BootHeader->FrameBuffer_Address, BootHeader->FrameBuffer_Pitch, BootHeader->FrameBuffer_Width, BootHeader->FrameBuffer_Height, BootHeader->FrameBuffer_BPP);
		//printf("  Type: %02X\n", BootHeader->FrameBuffer_Type);
	}

	return true;
}

bool MultiBootInfo::LoadMult2Boot1Info(void *Data)
{
	MulitBoot2::Boot_Header * BootHeader = (MulitBoot2::Boot_Header *)Data;

	unsigned char *Pos = (unsigned char *)Data;
	unsigned char *End = (unsigned char *)Data + BootHeader->Size;
	Pos += sizeof(MulitBoot2::Boot_Header);

	MemoryMapLength = 0;

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
				//printf(" Boot Loader: %s\n", Entry->String);
				break;
			}

			case MulitBoot2::Modules:
			{
				MulitBoot2::Boot_Module * Entry = (MulitBoot2::Boot_Module *)Pos;
				printf(" Module: %s\n", Entry->String);
				break;
			}

			case MulitBoot2::BasicMemory:
			{
				MulitBoot2::Boot_BasicMemory* Entry = (MulitBoot2::Boot_BasicMemory *)Pos;
				//printf(" Basic Memory: %08X, %08x\n", Entry->Lower, Entry->Upper);
				break;
			}

			case MulitBoot2::BIOSBootDevice:
			{
				MulitBoot2::Boot_BIOSBootDevice* Entry = (MulitBoot2::Boot_BIOSBootDevice *)Pos;
				//printf(" Boot Dev: %04X/%04x/%04x\n", Entry->BiosDev, Entry->Partition, Entry->SubPartition);
				break;
			}

			case MulitBoot2::MemoryMap:
			{
				MulitBoot2::Boot_MemoryMap* Entry = (MulitBoot2::Boot_MemoryMap *)Pos;
				
				intptr_t Base = sizeof(MulitBoot2::Boot_MemoryMap);
				
				while(Base < (intptr_t) Entry->Size && MemoryMapLength < MemoryMapMaxLength)
				{
					MemoryMap[MemoryMapLength] = (MemoryMapEntry *)(Pos + Base);
					MemoryMapLength++;

					Base += Entry->Entry_Size;
				}
				
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
