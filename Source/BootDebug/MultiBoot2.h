#include <stdint.h>
#pragma once

#pragma pack(push, 1)

namespace MulitBoot2
{
	struct Header
	{
		uint32_t Magic;
		uint32_t Archetecture;
		uint32_t Length;
		uint32_t Checksum;
	};

	const uint32_t HeaderMagic = 0xE85250D6;

	enum Archetecture
	{
		i386 = 0,
		MIPS = 4,
	};

	struct Tag_End
	{
		uint16_t Type;
		uint16_t Flags;
		uint32_t Size;
	};

	struct Tag_InformationRequest
	{
		uint16_t Type;
		uint16_t Flags;
		uint32_t Size;
	};

	struct Tag_LoadInformation
	{
		uint16_t Type;
		uint16_t Flags;
		uint32_t Size;

		intptr_t header_address;		// Where the multi-boot header ends up in memory
		intptr_t load_address;			// .text address in the file relative to the header
		intptr_t load_end_address;		// End of .data segment
		intptr_t bss_end_address;		// end of BSS segment
	};

	struct Tag_Entrypoint
	{
		uint16_t Type;
		uint16_t Flags;
		uint32_t Size;

		intptr_t entrypoint_address;		// Address of the entry point function in memory

		uint32_t Padding;
	};
	
	struct Tag_ConsoleFlags
	{
		uint16_t Type;
		uint16_t Flags;
		uint32_t Size;
		
		uint32_t ConsoleFlags;
	};

	struct Tag_FrameBuffer
	{
		uint16_t Type;
		uint16_t Flags;
		uint32_t Size;

		uint32_t Width;
		uint32_t Height;
		uint32_t Depth;
	};
	
	struct Tag_Align
	{
		uint16_t Type;
		uint16_t Flags;
		uint32_t Size;
		
		uint32_t Padding;
	};

	enum TagType
	{
		End = 0,
		InformationRequest = 1,
		LoadInformation = 2,
		Entrypoint = 3,
		ConsoleFlags = 4,
		FrameBuffer = 5,
		Alignment = 6,
	};

	enum TagFlags
	{
		None = 0x00,
		Optional = 0x01,
	};

	// --------------------------------

	const uint32_t BootMagic = 0x36D76289;

	struct Boot_Header
	{
		uint32_t Size;
		uint32_t Reserved;
	};

	struct Boot_Entry
	{
		uint32_t Type;
		uint32_t Size;
	};

	enum BootType
	{
		BootDataEnd = 0,
		CommandLine = 1,
		BootLoaderName = 2,
		Modules = 3,
		BasicMemory = 4,
		BIOSBootDevice = 5,
		MemoryMap = 6,
		VBEInfo = 7,
		FrameBufferInfo = 8,
		ELFSymbols = 9,
		APMTable = 10,
		ACPI
	};

	struct Boot_CommandLine
	{
		uint32_t Type;
		uint32_t Size;
		char String[1];
	};

	struct Boot_BootLoaderName
	{
		uint32_t Type;
		uint32_t Size;
		char String[1];
	};

	struct Boot_Module
	{
		uint32_t Type;
		uint32_t Size;
		intptr_t ModStart;
		intptr_t ModEnd;
		char String[1];
	};

	struct Boot_BasicMemory
	{
		uint32_t Type;
		uint32_t Size;
		uint32_t Lower;
		uint32_t Upper;
	};

	struct Boot_BIOSBootDevice
	{
		uint32_t Type;
		uint32_t Size;
		uint32_t BiosDev;
		uint32_t Partition;
		uint32_t SubPartition;
	};

	struct MemoryMapEntry
	{
		uint32_t BaseAddress;
		uint32_t Length;
		uint32_t Type;
		uint32_t Reserved;
	};

	struct Boot_MemoryMap
	{
		uint32_t Type;
		uint32_t Size;
		uint32_t Entry_Size;
		uint32_t Entry_Version;
	};

	struct Boot_VBEInfo
	{
		uint32_t Type;
		uint32_t Size;
		uint16_t Mode;
		uint16_t IntefaceSegment;
		uint16_t IntefaceOffset;
		uint16_t IntefaceLength;
		uint8_t  ControlInfo[512];
		uint8_t  ModeInfo[256];
	};

	struct FrameBufferPalletEntry
	{
		uint8_t  Red;
		uint8_t  Green;
		uint8_t  Blue;
	};

	struct FrameBufferPalletInfo
	{
		uint32_t NumColors;
		FrameBufferPalletEntry Pallet[1];
	};

	struct FrameBufferRGBInfo
	{
		uint8_t  RedFieldPos;
		uint8_t  RedMaskSize;
		uint8_t  GreenFieldPos;
		uint8_t  GreenMaskSize;
		uint8_t  BlueFieldPos;
		uint8_t  BlueMaskSize;
	};

	enum FrameBufferType
	{
		Indexed = 0,
		RGB = 1,
		Text = 2,
	};

	struct Boot_FrameBufferInfo
	{
		uint32_t Type;
		uint32_t Size;
		uint64_t Address;
		uint32_t Pitch;
		uint32_t Width;
		uint32_t Height;
		uint8_t  BPP;
		uint8_t  FrameBufferType;
		uint16_t Reserved;

		union 
		{
			FrameBufferRGBInfo RGBInfo;
			FrameBufferPalletInfo PalletInfo;
		};
	};

	struct Boot_ELFSymbols
	{
		uint32_t Type;
		uint32_t Size;
		uint16_t Num;
		uint16_t EntrySize;
		uint16_t Shndx;
		uint16_t Reserved;
		// Section Headers
	};

	struct Boot_APMTAble
	{
		uint32_t Type;
		uint32_t Size;
		uint16_t Version;
		uint16_t CodeSeg;
		uint32_t Offset;
		uint16_t CodeSeg_16;
		uint16_t DataSeg_16;
		uint16_t Flags;
		uint16_t CodeSegLength;
		uint16_t CodeSeg_16Length;
		uint16_t DataSeg_16Length;
	};
};

#pragma pack(pop)