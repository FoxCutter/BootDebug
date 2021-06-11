#define _CRT_SECURE_NO_WARNINGS
#include <string.h>
#include <stdio.h>
#include "MemoryMap.h"
#include "Utility.h"
#include "KernalLib.h"

const uint32_t PageSize = 0x1000;

MemoryPageMap::MemoryPageMap(uint32_t Address, uint32_t pageCount)
{
	PageData = reinterpret_cast<uint8_t *>(Address + 0x2000);
	PageCount = pageCount;
	FreeList = reinterpret_cast<MemoryInformation *>(Address);

	// Start by marking all pages in use
	for(uint32_t x = 0; x < PageCount / 4; x++)
		PageData[x] = 0xFF;

	// Built the Memeory Information Pool
	for (int x = 0; x < 0x2000 / sizeof(MemoryInformation); x++)
	{
		FreeList[x].Address = 0;
		FreeList[x].Length = 0;
		FreeList[x].Name[0] = 0;
		FreeList[x].Next = nullptr;
		FreeList[x].Prev = nullptr;

		if (x != 0)
		{
			FreeList[x].Prev = &FreeList[x - 1];
			FreeList[x - 1].Next = &FreeList[x];
		}	
	}
}

MemoryPageMap & MemoryPageMap::operator = (MemoryPageMap & LHS)
{
	PageData = LHS.PageData;
	PageCount = LHS.PageCount;
	AllocateList = LHS.AllocateList;
	FreeList = LHS.FreeList;

	return *this;
}

MemoryPageMap::MemoryInformation *MemoryPageMap::GetMemoryInformation()
{
	MemoryInformation * Ret = FreeList;
	if (Ret == nullptr)
	{
		Ret = new MemoryInformation();
	}
	else
	{
		FreeList = Ret->Next;
		FreeList->Prev = nullptr;
	}

	Ret->Address = 0;
	Ret->Length = 0;
	Ret->Type = FreeMemory;
	Ret->Name[0] = 0;
	Ret->Next = nullptr;
	Ret->Prev = nullptr;
	
	return Ret;
}

void MemoryPageMap::SetAllocatedMemory(uint64_t Address, uint64_t Length, MemoryType Type, char *Name)
{
	uint32_t StartPage = static_cast<uint32_t>(Address / PageSize);
	uint32_t EndPage = static_cast<uint32_t>((Address + Length) / PageSize);
	if((Address + Length) % PageSize != 0)
		EndPage++;

	uint32_t Index = StartPage / 4;
	uint32_t Offset = (StartPage % 4) * 2;
	uint32_t Mask = 0x3 << Offset;
	
	for(uint32_t x = StartPage; x < EndPage; x++)
	{
		PageData[Index] &= ~Mask;
		PageData[Index] |= ((Type << Offset) & Mask);

		if(Offset == 6)
		{
			Index++;
			Offset = 0;
			Mask = 0x3;
		}
		else
		{
			Offset += 2;
			Mask = Mask << 2;
		}
	}

	if (Type != FreeMemory)
	{
		MemoryInformation * Info = GetMemoryInformation();

		Info->Address = Address;
		Info->Length = Length;
		Info->Type = Type;
		strncpy(Info->Name, Name, 32);

		if (AllocateList == nullptr)
			AllocateList = Info;
		else
			AllocateList->InsertLast(Info);
	}
}

uint64_t MemoryPageMap::AllocateRange(uint64_t MinAddress, uint32_t Length, char *Name)
{
	uint32_t StartPage = static_cast<uint32_t>(MinAddress / PageSize);
	uint32_t TotalPages = static_cast<uint32_t>(Length / PageSize);
	if(Length % PageSize != 0)
		TotalPages++;

	uint32_t Index = StartPage / 4;
	uint32_t Offset = (StartPage % 4) * 2;
	uint32_t Mask = 0x3 << Offset;
	
	uint64_t StartAddress = StartPage * PageSize;
	uint64_t CurrentAddress = StartAddress;
	uint32_t Pages = 0;

	uint32_t Boundry = PageSize;

	for(uint32_t x = StartPage; x < PageCount; x++)
	{
		if(((PageData[Index] & Mask) >> Offset) == FreeMemory)
		{
			Pages++;
			if(Pages == TotalPages)
			{
				SetAllocatedMemory(StartAddress, Pages * PageSize, AllocatedMemory, Name);
				return StartAddress;
			}
		}
		else
		{
			StartAddress = CurrentAddress + PageSize;
			Pages = 0;
		}

		if(Pages == 1 && StartAddress % Boundry != 0)
		{
			StartAddress = CurrentAddress + PageSize;
			Pages = 0;
		}

		if(Offset == 6)
		{
			Index++;
			Offset = 0;
			Mask = 0x3;
		}
		else
		{
			Offset += 2;
			Mask = Mask << 2;
		}

		CurrentAddress += PageSize;
	}

	return UINT64_MAX;
}

void MemoryPageMap::Dump()
{
	uint32_t Index = 0;
	uint32_t Offset = 0;
	uint32_t Mask = 0x3;

	uint64_t StartAddress = 0;
	uint64_t CurrentAddress = 0;
	MemoryType CurrentType = FreeMemory;

	printf("           Start                End               Length       Type\n");

	for (uint32_t x = 0; x < PageCount; x++)
	{
		if ((PageData[Index] & Mask) >> Offset != CurrentType)
		{
			if (CurrentAddress != 0)
			{
				printf("     ");
				PrintLongAddress(StartAddress);
				printf(" - ");
				PrintLongAddress(CurrentAddress - 1);
				printf(", ");
				PrintLongAddress(CurrentAddress - StartAddress);
				switch (CurrentType)
				{
				case FreeMemory:
					printf(", RAM\n");
					break;

				case MemoryHole:
					printf(", Hole\n");
					break;

				case AllocatedMemory:
					printf(", Allocated\n");
					break;

				default:
					printf(", Reserved\n");
					break;
				}
			}

			StartAddress = CurrentAddress;
			CurrentType = static_cast<MemoryType>((PageData[Index] & Mask) >> Offset);
		}

		if (Offset == 6)
		{
			Index++;
			Offset = 0;
			Mask = 0x3;
		}
		else
		{
			Offset += 2;
			Mask = Mask << 2;
		}

		CurrentAddress += PageSize;
	}

	printf("     ");
	PrintLongAddress(StartAddress);
	printf(" - ");
	PrintLongAddress(CurrentAddress - 1);
	printf(", ");
	PrintLongAddress(CurrentAddress - StartAddress);
	switch (CurrentType)
	{
	case FreeMemory:
		printf(", RAM\n");
		break;

	case MemoryHole:
		printf(", Hole\n");
		break;

	case AllocatedMemory:
		printf(", Allocated\n");
		break;

	default:
		printf(", Reserved\n");
		break;
	}

	MemoryInformation * Cur = AllocateList;

	while (Cur != nullptr)
	{
		printf("     ");
		PrintLongAddress(Cur->Address);
		printf(", ");
		PrintLongAddress(Cur->Length);
		printf(" - %s\n", Cur->Name);

		Cur = Cur->Next;
	}


}

