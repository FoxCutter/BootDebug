#include "Utility.h"
#include <stdio.h>
#include <string.h>

void PrintLongAddress(uint64_t Address)
{
	printf("%08X:%08X", (uint32_t)((Address & 0xFFFFFFFF00000000) >> 32), (uint32_t)(Address & 0xFFFFFFFF));
}

uint32_t SeachMemory(uint32_t Start, uint32_t Count, const void *Search, uint32_t DataLength, uint32_t Alignment)
{
	if(DataLength == 0)
		return UINT32_MAX;

	// The starting address should be aligned
	if(Start % Alignment != 0)
	{
		Count -= Start % Alignment;
		Start = (Start + Alignment) - (Start % Alignment);
	}
	
	uint8_t *Data = reinterpret_cast<uint8_t *>(Start);

	int SeachState = 0;

	for(size_t Pos = 0; Pos < Count; Pos += Alignment)
	{
		if(memcmp(Data + Pos, Search, DataLength) == 0)
			return Start + Pos;
	}

	return UINT32_MAX;
}

bool ValidateChecksum(void *Data, uint16_t Length)
{
	uint8_t Val = 0;
	uint8_t *DataPtr = reinterpret_cast<uint8_t *>(Data);
	for(int x = 0; x < Length; x++)
	{
		Val += DataPtr[x];
	}

	return Val == 0;
}

void PrintMemoryBlock(void *Address, int Length, uint8_t Align)
{
	int Pos = 0;
	int Count = 16;

	unsigned char * Base = (unsigned char *)((int)Address & 0xFFFFFFF0);
	
	Length += (int)Address & 0x0000000F;

	while(Pos < Length)
	{
        int WriteLength = 0;
		
		if (Length - Pos < Count)
            Count = 16; //Length - Pos;

		printf("\00307%0.8X:", Base + Pos);
		WriteLength += 9;

        for (int x = 0; x < Count; x += Align)
        {
            if (x == 8)
                printf("-");
			else
				printf(" ");

			WriteLength++;
			
			if((Base + Pos + x) < Address || (Pos + x >= Length))
			{
				printf("\00308");
			}
			else
			{			
				printf("\00307");
			}

			if(Align == 2)
			{
				uint16_t Data = *(uint16_t *)(Base + Pos + x);

				printf("%4.4X", Data);

				WriteLength += 4;
			}
			else if(Align == 4)
			{
				uint32_t Data = *(uint32_t *)(Base + Pos + x);

				printf("%8.8X", Data);

				WriteLength += 8;
			}
			else if(Align == 8)
			{
				uint64_t Data = *(uint64_t *)(Base + Pos + x);

				printf("%16.16llX", Data);

				WriteLength += 16;
			}
			else
			{
				uint8_t Data = *(uint8_t *)(Base + Pos + x);

				printf("%2.2X", Data);

				WriteLength += 2;
			}
        }

		if(Align == 1)
		{
			for(int x = 0; x < 60 - WriteLength; x++)
				printf(" ");
		}
		else
		{
			for(int x = 0; x < 60- WriteLength - (16 / Align); x++)
				printf(" ");
		}

		for (int x = 0; x < Count; x++)
        {
			if(Align != 1 && (x % Align == 0))
				printf(" ");

			unsigned char Byte = *(Base + Pos + x);
			if (Byte < ' ' || Byte > 127)
				Byte = '.';

            if((Base + Pos + x) < Address || (Pos + x >= Length))
			{
				printf("\00308%c", Byte);
			}
			else
			{			
				printf("\00307%c", Byte);
			}
        }

        printf("\n");

        Pos += Count;
	}
}

void PrintBytes(void *Address, int Length)
{
	PrintMemoryBlock(Address, Length, 1);
}

void PrintWords(void *Address, int Length)
{
	PrintMemoryBlock(Address, Length, 2);
}

void PrintDWords(void *Address, int Length)
{
	PrintMemoryBlock(Address, Length, 4);
}

void PrintQWords(void *Address, int Length)
{
	PrintMemoryBlock(Address, Length, 8);
}