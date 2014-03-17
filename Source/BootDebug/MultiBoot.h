#include <stdint.h>
#pragma once

#pragma pack(push, 1)

namespace MulitBoot
{
	const uint32_t HeaderMagic = 0x1BADB002;

	enum HeaderFlags
	{
		PageAlign =			0x00000001,
		MemoryRequest =		0x00000002,
		HasGraphicFields =	0x00000004,
		HasAddressFields =	0x00010000,
	};
	
	struct Header
	{
		uint32_t Magic;
		uint32_t Flags;
		uint32_t Checksum;

		// These are valid if HasAddressFields is set.
		intptr_t header_address;		// if flags[16] is set: Where the multi-boot header ends up in memory
		intptr_t load_address;			// if flags[16] is set: .text address in the file relative to the header
		intptr_t load_end_address;		// if flags[16] is set: End of .data segment
		intptr_t bss_end_address;		// if flags[16] is set: end of BSS segment
		intptr_t entrypoint_address;	// if flags[16] is set: Address of the entry point function in memory

		// These are valid if HasGraphicFields is set.
		uint32_t ModeType;				// if flags[2] is set: 
		uint32_t Width;					// if flags[2] is set: 
		uint32_t Height;				// if flags[2] is set: 
		uint32_t Depth;					// if flags[2] is set: 
	};

	enum BootFlags
	{
		MemoryInfo =		0x00000001,
		BootDeviceInfo =	0x00000002,
		CommandLineInfo =	0x00000004,
		ModuleInfo =		0x00000008,
		AOutSymbols =		0x00000010,
		ELFSymbols =		0x00000020,
		MemoryMapInfo =		0x00000040,
		DriveInfo =			0x00000080,
		ConfigTable =		0x00000100,
		BootLoaderName =	0x00000200,
		ApmTable =			0x00000400,
		VBEInfo =			0x00000800,
		FrameBufferInfo =	0x00001000,
	};

	struct Boot_Module
	{
		intptr_t ModStart;
		intptr_t ModEnd;
		intptr_t String;
		uint32_t Padding;
	};

	struct Boot_AOutSymbols
	{
		uint32_t TabSize;
		uint32_t StringSize;
		intptr_t Address;
		uint32_t Reserved;
	};

	struct Boot_ELFSymbols
	{
		uint32_t Num;
		uint32_t EntrySize;
		intptr_t Address;
		uint32_t Shndx;
	};

	struct MemoryMapEntry
	{
		uint32_t BaseAddressLow;
		uint32_t BaseAddressHigh;
		uint32_t LengthLow;
		uint32_t LengthHigh;
		uint32_t Type;
	};

	struct DriveEntry
	{
		uint32_t Size;
		uint8_t  Number;
		uint8_t  Mode;
		uint16_t Cylinders;
		uint8_t  Heads;
		uint8_t  Sectors;
		uint16_t Ports[1];
	};

	struct Boot_APMTAble
	{
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

	struct FrameBufferPalletEntry
	{
		uint8_t Red;
		uint8_t Green;
		uint8_t Blue;
	};

	struct FrameBufferPalletInfo
	{
		intptr_t Address;
		uint16_t NumColors;
	};

	struct FrameBufferRGBInfo
	{
		uint8_t RedFieldPos;
		uint8_t RedMaskSize;
		uint8_t GreenFieldPos;
		uint8_t GreenMaskSize;
		uint8_t BlueFieldPos;
		uint8_t BlueMaskSize;
	};

	const uint32_t BootMagic = 0x2BADB002;

	enum FrameBufferType
	{
		Indexed = 0,
		RGB = 1,
		Text = 2,
	};

	struct Boot_Header
	{
		uint32_t Flags;
		uint32_t Memory_Lower;
		uint32_t Memory_Upper;

		uint32_t BootDevice;

		intptr_t CommandLine;

		uint32_t Mod_Count;
		intptr_t Mod_Address;
		
		union
		{
			Boot_AOutSymbols AOutSymbolInfo;
			Boot_ELFSymbols  ELFScection;
		};

		uint32_t MemMap_Length;
		intptr_t MemMap_Address;

		uint32_t Drives_Length;
		intptr_t Drives_Address;
		
		intptr_t ConfigTable;

		intptr_t BootLoaderName;

		intptr_t APMTable;

		intptr_t VBE_ControlInfo;
		intptr_t VBE_ModeInfo;
		uint16_t VBE_Mode;
		uint16_t VBE_IntefaceSegment;
		uint16_t VBE_IntefaceOffset;
		uint16_t VBE_IntefaceLength;

		uint64_t FrameBuffer_Address;
		uint32_t FrameBuffer_Pitch;
		uint32_t FrameBuffer_Width;
		uint32_t FrameBuffer_Height;
		uint8_t  FrameBuffer_BPP;
		uint8_t  FrameBuffer_Type;

		union
		{
			FrameBufferPalletInfo PalletInfo;
			FrameBufferRGBInfo    RBGInfo;
		};	
	};


};

#pragma pack(pop)