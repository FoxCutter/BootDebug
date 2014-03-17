#include "MultiBoot.h"
#include "MultiBoot2.h"

extern "C" void MultiBootEntry(void);

#pragma data_seg(".multiboot")
	static MulitBoot::Header MB1Header = 
	{
		MulitBoot::HeaderMagic, 
		MulitBoot::HasAddressFields | MulitBoot::MemoryRequest, 
		0 - (MulitBoot::HeaderMagic + (MulitBoot::HasAddressFields | MulitBoot::MemoryRequest)),
		0x00000000, 0x00000000, 0x00000000, 0x00000000,		// These will be filled in my the Header Set tool
		intptr_t(&MultiBootEntry),
	};

	static MulitBoot2::Header				MB2Header =	{MulitBoot2::HeaderMagic,		0, sizeof(MulitBoot2::Header),					0 - (MulitBoot2::HeaderMagic + sizeof(MulitBoot2::Header))};
	static MulitBoot2::Tag_LoadInformation	LoadInfo =	{MulitBoot2::LoadInformation,	0, sizeof(MulitBoot2::Tag_LoadInformation),		0x00000000, 0x00000000, 0x00000000, 0x00000000}; // These will be filled in my the Header Set tool
	static MulitBoot2::Tag_Entrypoint		EntryInfo = {MulitBoot2::Entrypoint,		0, sizeof(MulitBoot2::Tag_Entrypoint),			intptr_t(&MultiBootEntry)};
	static MulitBoot2::Tag_End				MB2End =	{MulitBoot2::End,				0, sizeof(MulitBoot2::Tag_End)};
#pragma data_seg()

#pragma comment(linker, "/MERGE:.multiboot=.text")
