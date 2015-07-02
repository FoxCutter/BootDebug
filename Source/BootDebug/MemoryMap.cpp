#include <string.h>
#include <stdio.h>
#include "MemoryMap.h"
#include "Utility.h"

const uint32_t PageSize = 0x1000;

MemoryPageMap::MemoryPageMap(uint32_t Address, uint32_t pageCount)
{
	PageData = reinterpret_cast<uint8_t *>(Address);
	PageCount = pageCount;

	// Start by marking all pages in use
	for(uint32_t x = 0; x < PageCount / 4; x++)
		PageData[x] = 0xFF;
}

MemoryPageMap & MemoryPageMap::operator = (MemoryPageMap & LHS)
{
	PageData = LHS.PageData;
	PageCount = LHS.PageCount;

	return *this;
}

void MemoryPageMap::SetAllocatedMemory(uint64_t Address, uint64_t Length, MemoryType Type)
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
}

uint64_t MemoryPageMap::AllocateRange(uint64_t MinAddress, uint32_t Length)
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

	for(uint32_t x = StartPage; x < PageCount; x++)
	{
		if(((PageData[Index] & Mask) >> Offset) == FreeMemory)
		{
			Pages++;
			if(Pages == TotalPages)
			{
				SetAllocatedMemory(StartAddress, Pages * PageSize, AllocatedMemory);
				return StartAddress;
			}
		}
		else
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
	
	for(uint32_t x = 0; x < PageCount; x++)
	{
		if((PageData[Index] & Mask) >> Offset != CurrentType)
		{
			if(CurrentAddress != 0)
			{
				printf("     ");
				PrintLongAddress(StartAddress);
				printf(" - ");
				PrintLongAddress(CurrentAddress - 1);
				printf(", ");
				PrintLongAddress(CurrentAddress - StartAddress);
				switch(CurrentType)
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
			CurrentType = static_cast<MemoryType>( (PageData[Index] & Mask) >> Offset );
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

	printf("     ");
	PrintLongAddress(StartAddress);
	printf(" - ");
	PrintLongAddress(CurrentAddress - 1);
	printf(", ");
	PrintLongAddress(CurrentAddress - StartAddress);
	switch(CurrentType)
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

