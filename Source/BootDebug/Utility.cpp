#include "Utility.h"
#include <stdio.h>
#include <string.h>

uint32_t SeachMemory(uint32_t Start, uint32_t Count, const void *Search, uint32_t Alignment)
{
	int SearchLength = strlen(reinterpret_cast<const char *>(Search));
	if(SearchLength == 0)
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
		if(memcmp(Data + Pos, Search, SearchLength) == 0)
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

void PrintBytes(void *Address, int Length)
{
	int Pos = 0;
	int Count = 16;

	unsigned char * Base = (unsigned char *)((int)Address & 0xFFFFFFF0);
	
	Length += (int)Address & 0x0000000F;

	while(Pos < Length)
	{
        int WriteLength = 0;
		
		if (Length - Pos < Count)
            Count = Length - Pos;

		printf("%0.8X:", Base + Pos);
		WriteLength += 9;

        for (int x = 0; x < Count; x++)
        {
            if (x == 8)
                printf("-");
			else
				printf(" ");

			WriteLength++;

            if((Base + Pos + x) < Address)
			{
				printf("  ");
			}
			else
			{			
				uint8_t Byte = *(Base + Pos + x);

				printf("%2.2X", Byte);
			}

			WriteLength += 2;
        }

		for(int x = 0; x < 60 - WriteLength; x++)
			printf(" ");

		for (int x = 0; x < Count; x++)
        {
            if((Base + Pos + x) < Address)
			{
				printf(" ");
			}
			else
			{			
				unsigned char Byte = *(Base + Pos + x);

				if (Byte < ' ' || Byte > 127)
					printf(".");
				else
					printf("%c", Byte);
			}
        }

        printf("\n");

        Pos += Count;
	}
}

void PrintWords(void *Address, int Length)
{
	int Pos = 0;
	int Count = 16;

	unsigned char * Base = (unsigned char *)((int)Address & 0xFFFFFFF0);
	
	Length += (int)Address & 0x0000000F;

	while(Pos < Length)
	{
        int WriteLength = 0;
		
		if (Length - Pos < Count)
            Count = Length - Pos;

		printf("%0.8X:", Base + Pos);
		WriteLength += 9;

        for (int x = 0; x < Count; x += 2)
        {
            if (x == 8)
                printf("-");
			else
				printf(" ");

			WriteLength++;

            if((Base + Pos + x) < Address)
			{
				printf("    ");
			}
			else
			{			
				uint16_t Data = *(uint16_t *)(Base + Pos + x);

				printf("%04X", Data);
			}

			WriteLength += 4;
        }

		for(int x = 0; x < 52 - WriteLength; x++)
			printf(" ");

		for (int x = 0; x < Count; x++)
        {
            if(x % 2 == 0)
				printf(" ");
			
			if((Base + Pos + x) < Address)
			{
				printf(" ");
			}
			else
			{			
				unsigned char Byte = *(Base + Pos + x);

				if (Byte < ' ' || Byte > 127)
					printf(".");
				else
					printf("%c", Byte);
			}
        } 

        printf("\n");

        Pos += Count;
	}
}

void PrintDWords(void *Address, int Length)
{
	int Pos = 0;
	int Count = 16;

	unsigned char * Base = (unsigned char *)((int)Address & 0xFFFFFFF0);
	
	Length += (int)Address & 0x0000000F;

	while(Pos < Length)
	{
        int WriteLength = 0;
		
		if (Length - Pos < Count)
            Count = Length - Pos;

		printf("%0.8X:", Base + Pos);
		WriteLength += 9;

        for (int x = 0; x < Count; x += 4)
        {
            if (x == 8)
                printf("-");
			else
				printf(" ");

			WriteLength++;

            if((Base + Pos + x) < Address)
			{
				printf("        ");
			}
			else
			{			
				uint32_t Data = *(uint32_t *)(Base + Pos + x);

				printf("%08X", Data);
			}

			WriteLength += 8;
        }

		for(int x = 0; x < 56 - WriteLength; x++)
			printf(" ");

		for (int x = 0; x < Count; x++)
        {
            if(x % 4 == 0)
				printf(" ");

			if((Base + Pos + x) < Address)
			{
				printf(" ");
			}
			else
			{			
				unsigned char Byte = *(Base + Pos + x);

				if (Byte < ' ' || Byte > 127)
					printf(".");
				else
					printf("%c", Byte);
			}
        }

        printf("\n");

        Pos += Count;
	}
}