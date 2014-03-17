#include <stdlib.h>
#include <stdio.h>

#include "RawTerminal.h"

void PrintMemory(void *Address, int Length);

void main(int argc, char *argv[])
{
	char InputBuffer[1024];
	bool Done = false;

	unsigned char *DumpAddress = (unsigned char *)0x100000;

	do
	{
		printf("> ");
		gets_s(InputBuffer, 1024);

		switch(InputBuffer[0])
		{
			case 'Q':
			case 'q':
				Done = true;
				break;

			case 'D':
			case 'd':
				PrintMemory(DumpAddress, 0x80);

				DumpAddress += 0x80;
				break;
			case 'F':
			case 'f':
				break;

			case '?':
				break;

			default:
				puts("Unknown Command.");
				break;
		};
	} while (!Done);

	return;
}

void PrintMemory(void *Address, int Length)
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
				unsigned char Byte = *(Base + Pos + x);

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