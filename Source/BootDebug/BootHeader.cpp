#include "MultiBoot.h"
#include "MultiBoot2.h"

extern "C" void MultiBootEntry(void);

#pragma data_seg(".multiboot")
	MulitBoot::Header MB1Header = 
	{
		MulitBoot::HeaderMagic, 
		MulitBoot::HasAddressFields | MulitBoot::MemoryRequest | MulitBoot::PageAlign | MulitBoot::HasGraphicFields, 
		0 - (MulitBoot::HeaderMagic + (MulitBoot::HasAddressFields | MulitBoot::MemoryRequest | MulitBoot::PageAlign | MulitBoot::HasGraphicFields)),
		0x00000000, 0x00000000, 0x00000000, 0x00000000,		// These will be filled in by the Header Set tool
		intptr_t(&MultiBootEntry),
		1, 80, 25, 0										// Make sure we have 80x25 text mode
	};

	__declspec(align(8)) static MulitBoot2::Header				MB2Header =	{MulitBoot2::HeaderMagic,		0, sizeof(MulitBoot2::Header),					0 - (MulitBoot2::HeaderMagic + sizeof(MulitBoot2::Header))};
	__declspec(align(8)) static MulitBoot2::Tag_LoadInformation	LoadInfo =	{MulitBoot2::LoadInformation,	0, sizeof(MulitBoot2::Tag_LoadInformation),		0x00000000, 0x00000000, 0x00000000, 0x00000000}; // These will be filled in by the Header Set tool
	__declspec(align(8)) static MulitBoot2::Tag_Entrypoint		EntryInfo = {MulitBoot2::Entrypoint,		0, sizeof(MulitBoot2::Tag_Entrypoint),			intptr_t(&MultiBootEntry)};
	__declspec(align(8)) static MulitBoot2::Tag_Align			AlignInfo = {MulitBoot2::Alignment,			0, sizeof(MulitBoot2::Tag_Align)};
	__declspec(align(8)) static MulitBoot2::Tag_ConsoleFlags	ConsoleInfo = {MulitBoot2::ConsoleFlags,	0, sizeof(MulitBoot2::Tag_ConsoleFlags),		0x3};
	__declspec(align(8)) static MulitBoot2::Tag_End				MB2End =	{MulitBoot2::End,				0, sizeof(MulitBoot2::Tag_End)};
#pragma data_seg()

#pragma comment(linker, "/MERGE:.multiboot=.text")
