#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "..\StdLib\argcargv.h"

#include "RawTerminal.h"

void PrintMemory(void *Address, int Length);

bool ParseHex(char *String, uint32_t &Value)
{
	uint32_t RetValue = 0;
	
	if(String[0] == 0)
		return true;

	while(*String != 0)
	{
		char Data = *String;
		
		if(Data >= '0' && Data <= '9')
			Data = Data - '0';

		else if (Data >= 'a' && Data <= 'f')
			Data = Data - 'a' + 10;

		else if (Data >= 'A' && Data <= 'F')
			Data = Data - 'A' + 10;

		else
			return false;

		RetValue *= 0x10;
		RetValue += Data;

		String++;
	}

	Value = RetValue;

	return true;
}

char * TrimChar(char *String, char Value)
{
	// Remove any leading characters
	while(*String != 0 && *String == Value)
		String++;

	// And null out any trailing characters.
	size_t Pos = strlen(String);

	while(Pos != 0)
	{
		char Data = String[Pos - 1];

		if(Data == Value)
			String[Pos - 1] = 0;
		
		else
			break;

		Pos--;
	}

	return String;
}

char * TrimString(char *String)
{
	// Remove any leading characters
	while(*String != 0 && *String == ' ')
		String++;

	// And null out any trailing characters.
	size_t Pos = strlen(String);

	while(Pos != 0)
	{
		char Data = String[Pos - 1];

		if(Data == '\r' || Data == '\n')
			String[Pos - 1] = 0;

		else if(Data == ' ')
			String[Pos - 1] = 0;
		else
			break;

		Pos--;
	}

	return String;
}

uint32_t SeachMemory(uint32_t Start, uint32_t Count, char *Search)
{
	int SearchLength = strlen(Search);
	if(SearchLength == 0)
		return UINT32_MAX;

	char *Data = (char *)Start;

	int SeachState = 0;

	for(int Pos = 0; Pos != Count; Pos++)
	{
		if(Data[Pos] == Search[SeachState])
		{
			SeachState ++;
			if(SeachState == SearchLength)
			{
				return Start + Pos - SearchLength + 1;
			}
		}
		else
		{
			SeachState = 0;
		}
	}

	return UINT32_MAX;
}

#pragma pack(push, 1)
struct MPPointerStruct
{
	uint8_t Signature[4];
	uint32_t DataAddress;
	uint8_t Length;
	uint8_t Rev;
	uint8_t Checksum;
	uint8_t MPFeature[5];
};

struct MPConfigHeader
{
	uint8_t Signature[4];
	uint16_t BaseTableLegth;
	uint8_t Rev;
	uint8_t Checksum;
	uint8_t OEMID[8];
	uint8_t ProductID[12];
	uint32_t OEMTablePointer;
	uint16_t OEMTableSize;
	uint16_t EntryCount;
	uint32_t LAPICAddress;
	uint16_t ExtendedTableLength;
	uint8_t ExtendedTableChecksum;
	uint8_t Reserved;
};

enum MPConfigEntryType
{
	MPCE_Processor = 0,
	MPCE_BUS = 1,
	MPCE_IOAPIC = 2,
	MPCE_IOInterupt = 3,
	MPCE_LocalInterrupt = 4,
};

struct MPConfigTableProcessorEntry
{
	uint8_t Type;
	uint8_t LAPIC_ID;
	uint8_t LAPIC_Ver;
	uint8_t Flags;

	uint32_t CPUSignature;
	uint32_t FeatureFlags;
	uint32_t Reserved[2];
};

struct MPConfigTableBusEntry
{
	uint8_t Type;
	uint8_t Bus_ID;
	uint8_t BusType[6];
};

struct MPConfigTableIOAPCIEntry
{
	uint8_t Type;
	uint8_t Bus_ID;
	uint8_t LAPIC_Ver;
	uint8_t Flags;

	uint32_t IOAPICAddress;
};

struct MPConfigTableInteruptEntry
{
	uint8_t Type;
	uint8_t IntType;
	uint16_t Flags;
	uint8_t SourceBusID;
	uint8_t SourceBusIRQ;
	uint8_t DestIOAPICID;
	uint8_t DestIOAPICINT;
};

#pragma pack(pop)

MPPointerStruct *FindMPPointer(uint32_t Address, uint32_t Length)
{
	while(true)
	{
		uint32_t Loc = SeachMemory(Address, Length, "_MP_");

		if(Loc == UINT32_MAX)
			break;

		MPPointerStruct *Ret = (MPPointerStruct *)Loc;
		char * Check = (char *)Loc;
		char Val = 0;
		for(int x = 0; x < 16 * Ret->Length; x++)
		{
			Val += Check[x];
		}

		if(Val == 0)
			return Ret;

		Loc += 4;
						
		Length = Length - (Loc - Address);
		Address = Loc;
	}

	return nullptr;
}

void PrintMPInfo()
{
	MPPointerStruct * MPPointer = nullptr;

	// First off we need to find the MP data.
	// a. In the first kilobyte of Extended BIOS Data Area (EBDA), or
	uint32_t FPAddress = *(uint16_t *)(0x040E);
	FPAddress = FPAddress << 4;
	if(FPAddress != 0)
	{
		MPPointer = FindMPPointer(FPAddress, 1024);
	}

	// c. In the BIOS ROM address space between 0F0000h and 0FFFFFh.
	if(MPPointer == nullptr)
		MPPointer = FindMPPointer(0xF0000, 0x10000);

	if(MPPointer == nullptr)
	{
		printf("Unable to find MP table\n");
		return;
	}

	MPConfigHeader* MPConfig = (MPConfigHeader *) MPPointer->DataAddress;
	printf("LAPIC %08X\n", MPConfig->LAPICAddress);
	printf("Entry Count %04X\n", MPConfig->EntryCount);
}

void main(int argc, char *argv[])
{
	char InputBuffer[1024];
	bool Done = false;

	uint32_t DumpAddress = 0x100000;

	do
	{
		printf("> ");
		gets_s(InputBuffer, 1024);

		char * Input= TrimString(InputBuffer);

		int ArgCount = _ConvertCommandLineToArgcArgv(Input);

		switch(_ppszArgv[0][0])
		{
			case 'Q':
			case 'q':
				Done = true;
				break;

			case 'D':
			case 'd':
				{
					if(ArgCount > 1)
					{
						if(!ParseHex(_ppszArgv[1], DumpAddress))
						{
							puts("Invalid address");
							break;
						}
					}

					uint32_t Length = 0x80;
					if(ArgCount > 2)
					{
						if(!ParseHex(_ppszArgv[2], Length))
						{
							puts("Invalid length");
							break;
						}
					}

					PrintMemory((void *)DumpAddress, Length);
					DumpAddress += Length;
				}
				break;

			case 's':
			case 'S':
				{
					if(ArgCount < 4)
					{
						puts("Invalid arguments");
						break;
					}
					
					uint32_t Start = 0;								        

					if(!ParseHex(_ppszArgv[1], Start))
					{
						puts("Invalid address");
						break;
					}

					uint32_t Count = 0;

					if(!ParseHex(_ppszArgv[2], Count))
					{
						puts("Invalid count");
						break;
					}

					char *String = _ppszArgv[3];
					if(String[0] == '"' || String[0] == '\'')
						String = TrimChar(String, String[0]);
					
					while(true)
					{
						uint32_t Loc = SeachMemory(Start, Count, String);

						if(Loc == UINT32_MAX)
							break;

						printf("Found %08X\n", Loc);
						
						Loc += strlen(String);
						
						Count = Count - (Loc - Start);
						Start = Loc;
					}

				}
				break;

			case 'I':
			case 'i':
				{
					if(ArgCount < 2)
					{
						puts("Invalid arguments");
						break;
					}

					if(_ppszArgv[1][0] == 'M' && _ppszArgv[1][1] == 'P')
					{
						PrintMPInfo();
					}
					else
					{
						puts("Invalid arguments");
					}
				}
				
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