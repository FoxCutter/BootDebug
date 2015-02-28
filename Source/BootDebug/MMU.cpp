#include "MMU.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "LowLevel.h"
#include "Utility.h"

struct PageDirectoryPointerEntry
{
	uint64_t Present : 1;
	uint64_t Reserved : 2;
	uint64_t Write_Through : 1;
	uint64_t Cache_Disabled : 1;
	uint64_t Reserved2 : 4;
	uint64_t Available : 3;
	uint64_t Directory_Address : 52;
};

struct PageDirectoryEntry
{
	uint64_t  Present : 1;
	uint64_t  Writable : 1;
	uint64_t  User_Access : 1;
	uint64_t  Write_Through : 1;
	uint64_t  Cache_Disabled : 1;
	uint64_t  Accessed : 1;
	uint64_t  Ignored : 1;
	uint64_t  Size : 1;
	uint64_t  Available : 4;
	uint64_t  Table_Address : 51;
	uint64_t  ExecuteDisable : 1;
};

struct LargePageDirectoryEntry
{
	uint64_t  Present : 1;
	uint64_t  Writable : 1;
	uint64_t  User_Access : 1;
	uint64_t  Write_Through : 1;
	uint64_t  Cache_Disabled : 1;
	uint64_t  Accessed : 1;
	uint64_t  Dirty : 1;
	uint64_t  Size : 1;
	uint64_t  Globale : 1;
	uint64_t  Available : 3;
	uint64_t  Pat : 1;
	uint64_t  Reserved : 8;
	uint64_t  Page_Address : 42;
	uint64_t  ExecuteDisable : 1;
};

struct PageTableEntry
{
	uint64_t Present : 1;
	uint64_t Writable : 1;
	uint64_t User_Access : 1;
	uint64_t Write_Through : 1;
	uint64_t Cache_Disabled : 1;
	uint64_t Accessed : 1;
	uint64_t Dirty : 1;
	uint64_t Pat : 1;
	uint64_t Global : 1;
	uint64_t Available : 3;
	uint64_t Page_Address : 51;
	uint64_t ExecuteDisable : 1;
};

#pragma section(".PDE", read, write)

__declspec(align(32)) PageDirectoryPointerEntry PDPTE[4];
__declspec(allocate(".PDE")) LargePageDirectoryEntry PDE[2048];
__declspec(allocate(".PDE")) PageTableEntry PTE[512];

MMU::MMU(void)
{
	// Check we have PSE and PAE on this cpu.
	Registers Res;
	ReadCPUID(1, 0, &Res);

	if(((Res.EDX & CPUFlags::PhysicalAddressExtensions) == 0) ||
		((Res.EDX & CPUFlags::PageSizeExtensions) == 0) )
	{
		printf("Paging unavailable\n");
		return;
	}

	
	//std::vector<int> MemoryMap;
	//MemoryMap.push_back(1);
	//MemoryMap.push_back(2);
	//printf("%u, %u, %u, %u\n", sizeof(PageDirectoryPointerEntry), sizeof(PageDirectoryEntry), sizeof(LargePageDirectoryEntry), sizeof(PageTableEntry));

	//printf("PDPTE: %08X", PDPTE);
	//printf(" PDE0: %08X", &PDE[0]);
	//printf(" PDE1: %08X", &PDE[0x200]);
	//printf(" PDE2: %08X", &PDE[0x400]);
	//printf(" PDE3: %08X\n", &PDE[0x600]);

	memset(&PDPTE, 0, sizeof(PageDirectoryPointerEntry) * 4);
	memset(&PDE, 0, sizeof(LargePageDirectoryEntry) * 2048);
	memset(&PTE, 0, sizeof(PageTableEntry) * 512);

	PDPTE[0].Present = true;
	PDPTE[0].Directory_Address = (uint32_t)&PDE[0] >> 12;
	
	PDPTE[1].Present = true;
	PDPTE[1].Directory_Address = (uint32_t)&PDE[0x200] >> 12;

	PDPTE[2].Present = true;
	PDPTE[2].Directory_Address = (uint32_t)&PDE[0x400] >> 12;

	PDPTE[3].Present = true;
	PDPTE[3].Directory_Address = (uint32_t)&PDE[0x600] >> 12;

	uint64_t BaseAddress = 0;
	for(int x = 0; x < 2048; x++)
	{
		PDE[x].Present = true;
		PDE[x].Writable = true;
		PDE[x].User_Access = true;
		PDE[x].Size = true;	
		PDE[x].Page_Address = (BaseAddress) >> 21;

		BaseAddress += 2048 * 1024;
	}

	BaseAddress = 0;
	for(int x = 0; x < 512; x++)
	{
		PTE[x].Present = true;
		PTE[x].Present = true;
		PTE[x].Writable = true;
		PTE[x].User_Access = true;
		PTE[x].Page_Address = (BaseAddress) >> 12;

		BaseAddress += 4096;
	}


	// Map the first 4 high megs into the same 2 megs at 4 gigs
	PDE[0x400].Page_Address = (0x100000000) >> 21;
	PDE[0x401].Page_Address = (0x100000000) >> 21;

	// Make the first 2megs map with 4k pages
	PageDirectoryEntry *PDEE = reinterpret_cast<PageDirectoryEntry *>(PDE);
	PDEE[0].Size = 0;
	PDEE[0].Table_Address = (uint32_t)&PTE[0x0] >> 12;
	//PTE[1].Present = false;

	//printf("Setting CR3 to %08X\n", (uint32_t)PDPTE);
	WriteCR3((uint32_t) PDPTE);

	uint32_t Temp = ReadCR4();
	Temp |= CPUFlags::PhysicalAddressExtensionsEnable;
	WriteCR4(Temp);

	Temp = ReadCR0();
	Temp |= CPUFlags::PagingEnabled;

	//printf("Turning on Paging!\n");
	WriteCR0(Temp);	
}


MMU::~MMU(void)
{
}

void MMU::PrintAddressInformation(uint32_t Address)
{
	if((ReadCR0() && CPUFlags::PagingEnabled) == 0)
	{
		printf("Paging Disabled\n");
	}
	
	// Decode the address
	printf("Virtual Address: %08X\n", Address);
	
	uint64_t RealAddress = 0;
	bool Done = false;

	uint32_t Level1 = (Address & 0xC0000000) >> 30;
	printf(" Level 1: Index: %X", Level1);

	PageDirectoryPointerEntry *Tabel1 = reinterpret_cast<PageDirectoryPointerEntry *>(ReadCR3());
	if(Tabel1[Level1].Present == false)
	{
		printf(", Not Present\n");
		return;
	}
	else
	{
		printf(", Directory Address: ");
		PrintLongAddress(Tabel1[Level1].Directory_Address << 12);
		printf("\n");
	}

	uint32_t Level2 = (Address & 0x3FE00000) >> 21;
	printf(" Level 2: Index: %X", Level2);

	PageDirectoryEntry *Tabel2 = reinterpret_cast<PageDirectoryEntry *>(Tabel1[Level1].Directory_Address << 12);
	if(Tabel2[Level2].Present == false)
	{
		printf(", Not Present\n");
		return;
	}
	else
	{
		if(Tabel2[Level2].Accessed == true)
			printf(", Accessed");

		if(Tabel2[Level2].Size == true)
		{
			LargePageDirectoryEntry *Page2 = reinterpret_cast<LargePageDirectoryEntry *>(Tabel1[Level1].Directory_Address << 12);
			
			if(Page2[Level2].Dirty == true)
				printf(", Dirty");

			printf(", 2M Base Address: ");
			PrintLongAddress(Page2[Level2].Page_Address << 21);
			printf("\n");

			RealAddress = Page2[Level2].Page_Address << 21;
			RealAddress += (Address & 0x001FFFFF);

			Done = true;
		}
		else
		{
			printf(", Table Address: ");
			PrintLongAddress(Tabel2[Level2].Table_Address << 12);
			printf("\n");
		}
	}

	if(!Done)
	{
		uint32_t Level3 = (Address & 0x001FF000) >> 12;
		printf(" Level 3: Index: %X", Level3);

		PageTableEntry *Tabel3 = reinterpret_cast<PageTableEntry *>(Tabel2[Level2].Table_Address << 12);
		if(Tabel3[Level3].Present == false)
		{
			printf(", Not Present\n");
			return;
		}
		else
		{
			if(Tabel3[Level3].Accessed == true)
				printf(", Accessed");

			if(Tabel3[Level3].Dirty == true)
				printf(", Dirty");

			printf(", 4k Base Address: ");
			PrintLongAddress(Tabel3[Level3].Page_Address << 12);
			printf("\n");

			RealAddress = Tabel3[Level3].Page_Address << 12;
			RealAddress += (Address & 0x00000FFF);
		}
	}

	printf("  Physical Address: ");
	PrintLongAddress(RealAddress);
	printf("\n");
	/*

	uint32_t Level1 = (Address & 0xC0000000) >> 30;
	uint32_t Level2 = (Address & 0x3FE00000) >> 21;
	//uint32_t Level3 = (Address & 0x001FFFFF);
	uint32_t Level3 = (Address & 0x001FF000) >> 12;
	uint32_t Level4 = (Address & 0x00000FFF);

	printf(" L1: %X", Level1);
	printf(" L2: %03X", Level2);
	printf(" L3: %03X", Level3);
	printf(" L4: %03X", Level4);
	*/

	//printf("\n");

}