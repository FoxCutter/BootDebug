#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "Utility.h"

#include "..\StdLib\argcargv.h"

#include "CoreComplex.h"

#include "MMU.h"
#include "OpenHCI.h"
#include "MPConfig.h"
#include "RawTerminal.h"
#include "LowLevel.h"
#include "ACPI.h"
#include "Thread.h"
#include "InterruptControler.h"

#include "Disassembler.h"

MMU * MMUManager = nullptr;
OpenHCI * USBManager = nullptr;

extern InterruptControler m_InterruptControler;


char * CPUIDFlags[] = 
{
	// Features - EDX
	"Floating Point Unit On-Chip",
	"Virtual 8086 Mode Enhancements",
	"Debugging Extensions",
	"Page Size Extension",
	"Time Stamp Counter",
	"Model Specific Registers",
	"Physical Address Extension",
	"Machine Check Exception",
	"CMPXCHG8B Instruction",
	"APIC On-Chip",
	"",
	"SYSENTER and SYSEXIT Instructions",
	"Memory Type Range Registers",
	"Page Global Bit",
	"Machine Check Architecture",
	"Conditional Move Instructions",
	"Page Attribute Table",
	"36-Bit Page Size Extension",
	"Processor Serial Number",
	"CLFLUSH Instruction",
	"",
	"Debug Store",
	"Thermal Monitor and Software Controlled Clock Facilities",
	"Intel MMX Technology",
	"FXSAVE and FXRSTOR Instructions",
	"SSE",
	"SSE2",
	"Self Snoop",
	"Max APIC IDs reserved field is Valid",
	"Thermal Monitor",
	"",
	"Pending Break Enable",
	
	// Features - ECX
	"SSE3",
	"PCLMULQDQ",
	"64-Bit DS Area",
	"MONITOR/MWAIT",
	"CPL Qualified Debug Store",
	"Virtual Machine Extensions",
	"Safer Mode Extensions",
	"Enhanced Intel SpeedStep technology",
	"Thermal Monitor 2",
	"SSSE3",
	"L1 Context ID",
	"IA32_DEBUG_INTERFACE MSR",
	"FMA extensions",
	"CMPXCHG16B Available",
	"xTPR Update Control",
	"Perfmon and Debug Capability",
	"",
	"Process-context identifiers",
	"Memory Map Prefech",
	"SSE4.1",
	"SSE4.2",
	"x2APIC",
	"MOVBE Instruction",
	"POPCNT Instruction",
	"TSC Deadline",
	"AESNI",
	"XSAVE/XRSTOR Instructions",
	"OSXSAVE Enabled",
	"AVX",
	"16-Bit Floating Point Conversion",
	"RDRAND Instruction",
	"",

	// Structured Extended Feature - EBX
	"FS and GS Base",
	"IA32_TSC_ADJUST MSR Supported",
	"",
	"BMI1",
	"Hardware Lock Elision",
	"AVX2",
	"",
	"Supervisor-Mode Execution Prevention",
	"BMI2",
	"Enhanced REP MOVSB/STOSB",
	"INVPCID",
	"Restricted Transactional Memory",
	"Platform Quality of Service Monitoring",
	"Deprecates FPU CS and FPU DS (64-Bit)",
	"",
	"Platform Quality of Service Enforcement",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",

	// Extended Features - EDX
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"SYSCALL/SYSRET available (64-bit)",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"Execute Disable available",
	"",
	"",
	"",
	"",
	"",
	"1-GByte Pages",
	"RDTSCP Avaliable",
	"",
	"EMT64",
	"",
	"",

	// Extended Features - ECX
	"LAHF/SAHF Available (64 bit)",
	"CmpLegacy",
	"Secure Virtual Machine",
	"",
	"AltMovCR8",
	"LZCNT Instruction",
	"",
	"",
	"PREFETCHW Instruction",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
};




#define FPTR_TO_32(p) ((p & 0xFFFF0000) >> 12) + (p & 0xFFFF)

#pragma pack(push, 1)

struct IRQEntry
{
	uint8_t LinkValue;
	uint16_t IRQBitmap;
};

struct PIREntry
{
	uint8_t Bus;
	uint8_t Device;
	IRQEntry INT[4];
	uint8_t SlotNum;
	uint8_t Reserved;
};

struct PIRTable
{
	uint8_t Signature[4];
	uint16_t Version;
	uint16_t Size;
	uint8_t RounterBus;
	uint8_t RounterDevFunction;
	uint16_t PCIExcuslveIRQ;
	uint16_t RounterVenderID;
	uint16_t RounterDeviceID;
	uint32_t MiniportData;
	uint8_t Reserved[11];
	uint8_t Chucksum;

	PIREntry Entries[1];
};

#pragma pack(pop)

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
	if(String == nullptr)
		return nullptr;

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


bool ParseData(char *InputData, void *Data, uint32_t &DataLength, uint32_t DataSize)
{
	bool Pass = true;
	// We have a list of hex values
	DataLength = 0;
	char * CurrentData = NextToken(InputData);
	while(CurrentData != nullptr)
	{
		switch (DataSize)
		{
			case 1:
				Pass = ParseHex(CurrentData, reinterpret_cast<uint8_t *>(Data)[DataLength]);
				break;

			case 2:
				Pass = ParseHex(CurrentData, reinterpret_cast<uint16_t *>(Data)[DataLength]);
				break;

			case 4:
				Pass = ParseHex(CurrentData, reinterpret_cast<uint32_t *>(Data)[DataLength]);
				break;

			case 8:
				Pass = ParseHex(CurrentData, reinterpret_cast<uint64_t *>(Data)[DataLength]);
				break;
		}

		DataLength++;

		if(Pass == false)
		{
			printf(" Invalid Data [%s]\n", CurrentData);	
			return false;
		}

		CurrentData = NextToken(InputData);
	};

	return true;
}


bool ParseAddress(char *Value, uint32_t &Address, uint32_t LastAddress)
{
	bool Error = false;
	char * CurrentData = TrimString(Value);

	int Modifier = 0;
	uint32_t ParsedValue = 0;

	if(CurrentData[0] == '+')
	{
		Modifier = 1;
	
		if(CurrentData[1] == 0)
			Error = true;
		else							
			CurrentData++;
	}
	else if(CurrentData[0] == '-')
	{
		Modifier = 2;

		if(CurrentData[1] == 0)
			Error = true;
		else							
			CurrentData++;
	} 
						
	if(CurrentData[0] == ':')
	{
		Modifier = 3;

		if(CurrentData[1] != 0)
			Error = true;
	}
	else if(CurrentData[0] == '!')
	{
		Modifier = 3;
		Address = LastAddress;

		if(CurrentData[1] != 0)
			Error = true;
	}
	else if(!ParseHex(CurrentData, ParsedValue))
	{
		Error = true;
	}

	if(Error)
	{		
		return false;
	}

	if(Modifier == 0)
		Address = ParsedValue;

	else if(Modifier == 1)
		Address = Address + ParsedValue;
						
	else if(Modifier == 2)
		Address = Address - ParsedValue;

	return true;
}

uint32_t SearchBIOS(const void *Search, uint32_t DataLength, uint32_t Alignment)
{
	// First, check the 1st K of the EBDA.
	uint32_t EBDABase = *(uint16_t *)(0x040E);
	EBDABase = EBDABase << 4;

	uint32_t Address = SeachMemory(EBDABase, 0x400, Search, DataLength, Alignment);

	// If that failed check the BIOS space
	if(Address == UINT32_MAX)
		Address = SeachMemory(0xE0000, 0x20000, Search, DataLength, Alignment);

	return Address;
}

struct CommandSet
{
	uint32_t	CurrentAddress;
	uint32_t	LastAddress;

	uint32_t	ArgCount;
	char *		ArgData[32];
};

typedef void(CommandFunction)(CommandSet & Data);

struct CommandEntry
{
	char Command;
	uint8_t MaxLength;
	CommandFunction *Function;
	char * Help;
};

void DumpCommand(CommandSet & Data);
void EnterCommand(CommandSet & Data);
void SearchCommand(CommandSet & Data);
void PortCommand(CommandSet & Data);
void MemoryCommand(CommandSet & Data);
void RegisterCommand(CommandSet & Data);
void InfoCommand(CommandSet & Data);
void ObjectCommand(CommandSet & Data);
void DisassembleCommand(CommandSet & Data);

CommandEntry Commands[] = {
	{'D',	 2,		&DumpCommand,		"Dump Memory           D[Size] [Address] [Length]"},	
	{'E',	 2,		&EnterCommand,		"Enter Data            E[Size] Address Data"},	
	{'S',	 2,		&SearchCommand,		"Search Memory         S[Size] Address Length Data"},	
	{'I',	 2,		&PortCommand,		"Read from a Port      I[Size] Port"},	
	{'O',	 2,		&PortCommand,		"Write to a Port       O[Size] Port Value"},	
	{'X',	 1,		&MemoryCommand,		"Memory Information    X Address"},	
	{'R',	 1,		&RegisterCommand,	"Register Information  R Register [Value]"},	
	{'N',	 1,		&InfoCommand,		"General Information   N [Type]"},	
	{'B',	 1,		&ObjectCommand,		"Object Information    B [Name]"},					
	{'U',	 3,		&DisassembleCommand,"Disassemble           U[16] [Address] [Length]"},					
	{0,		 0,		nullptr,			""},	
};

void DumpCommand(CommandSet & Data)
{
	int DumpSize = 1;
	uint32_t Length = 0x80;
	uint32_t Address = Data.CurrentAddress;

	switch(toupper(Data.ArgData[0][1]))
	{
		case 0:
		case 'B':
			DumpSize = 1;
			break;
						
		case 'W':
			DumpSize = 2;
			break;

		case 'D':
			DumpSize = 4;
			break;

		case 'Q':
			DumpSize = 8;
			break;

		case 'S':
			DumpSize = 0;
			Length = 128;
			break;

		default:
			printf(" Unknown Option [%c]\n", Data.ArgData[0][1]);
			return;

	}

	if(Data.ArgCount > 1)
	{
		if(!ParseAddress(Data.ArgData[1], Address, Data.LastAddress))
		{
			printf(" Invalid Address [%s]\n", Data.ArgData[1]);
			return;
		}
	}

	if(Data.ArgCount > 2)
	{
		if(!ParseHex(Data.ArgData[2], Length))
		{
			printf(" Invalid Length [%s]\n", Data.ArgData[2]);
			return;
		}
	}

	if(DumpSize == 0)
	{
		printf("\00307%0.8X:", Address);

		for(uint32_t x = 0; x < Length; x++)
		{
			uint8_t Char = *reinterpret_cast<uint8_t *>(Address);
			Address++;

			if( Char == 0)
				break;

			printf("%c", Char);

		}
		printf("\00307\n");
	}
	else 
	{
		Data.LastAddress = Address;

		PrintMemoryBlock((void *)Address, Length, DumpSize);

		Data.CurrentAddress = Address + Length;
	}
}

void SearchCommand(CommandSet & Data)
{
	int DataSize = 1;
					
	switch(toupper(Data.ArgData[0][1]))
	{
		case 0:
		case 'B':
			DataSize = 1;
			break;
						
		case 'W':
			DataSize = 2;
			break;

		case 'D':
			DataSize = 4;
			break;

		case 'Q':
			DataSize = 8;
			break;
						
		default:
			printf(" Unknown Option [%c]\n", Data.ArgData[0][1]);
			return;
	}					
					
	uint32_t Start = Data.CurrentAddress;
	uint32_t Count = 0;
	uint32_t DataLength = 0;

	if(Data.ArgCount < 2)
	{
		puts(" Address Missing");
		return;
	}
					
	if(!ParseAddress(Data.ArgData[1], Start, Data.LastAddress))
	{
		printf(" Invalid Address [%s]\n", Data.ArgData[1]);
		return;
	}

	if(Data.ArgCount < 3)
	{
		puts(" Length Missing");
		return;
	}

	if(!ParseHex(Data.ArgData[2], Count))
	{
		printf(" Invalid Length [%s]\n", Data.ArgData[2]);
		return;
	}
					
	if(Data.ArgCount < 4 || Data.ArgData[3][0] == 0)
	{
		puts(" Search Data Missing");
		return;

	}
					
	void * InputData = new uint8_t[32 * DataSize];

	// We have a search string
	if(Data.ArgData[3][0] == '"')
	{
		if(DataSize != 1)
		{
			printf(" String Data is only valid on Byte searches.\n");
			delete InputData;
			return;
		}						
		else if(Data.ArgData[3][strlen(Data.ArgData[3]) - 1] != '"')
		{
			printf(" Invalid String Data [%s]\n", Data.ArgData[3]);
			delete InputData;
			return;
		}

		char * TrimmedData = TrimChar(Data.ArgData[3], Data.ArgData[3][0]);
		DataLength = strlen(TrimmedData);
						
		if(DataLength > 32)
			DataLength = 32;

		memcpy(InputData, TrimmedData, DataLength);
	}
	else
	{
		for(size_t x = 3; x < Data.ArgCount; x++)
		{
			bool Pass = true;

			switch (DataSize)
			{
				case 1:
					Pass = ParseHex(Data.ArgData[x], reinterpret_cast<uint8_t *>(InputData)[DataLength]);
					break;

				case 2:
					Pass = ParseHex(Data.ArgData[x], reinterpret_cast<uint16_t *>(InputData)[DataLength]);
					break;

				case 4:
					Pass = ParseHex(Data.ArgData[x], reinterpret_cast<uint32_t *>(InputData)[DataLength]);
					break;

				case 8:
					Pass = ParseHex(Data.ArgData[x], reinterpret_cast<uint64_t *>(InputData)[DataLength]);
					break;
			}

			DataLength++;

			if(Pass == false)
			{
				printf(" Invalid Data [%s]\n", Data.ArgData[x]);	
				return;
			}
		}
	}

	uint32_t Pos = Start;

	while(true)
	{
		uint32_t Loc = SeachMemory(Pos, Count, InputData, DataLength * DataSize, DataSize);

		if(Loc == UINT32_MAX)
			break;

		PrintMemoryBlock((void *)Loc, DataLength * DataSize, DataSize);
						
		Loc += DataLength * DataSize;
						
		Count = Count - (Loc - Pos);
		Pos = Loc;
	}					

	Data.LastAddress = Start;

	delete InputData;
}

void EnterCommand(CommandSet & Data)
{
	int DataSize = 1;
					
	switch(toupper(Data.ArgData[0][1]))
	{
		case 0:
		case 'B':
			DataSize = 1;
			break;
						
		case 'W':
			DataSize = 2;
			break;

		case 'D':
			DataSize = 4;
			break;

		case 'Q':
			DataSize = 8;
			break;
						
		default:
			printf(" Unknown Option [%c]\n", Data.ArgData[0][1]);
			return;
	}					
					
	uint32_t Start = Data.CurrentAddress;
	uint32_t DataLength = 0;

	if(Data.ArgCount < 2)
	{
		puts(" Address Missing");
		return;
	}
					
	if(!ParseAddress(Data.ArgData[1], Start, Data.LastAddress))
	{
		printf(" Invalid Address [%s]\n", Data.ArgData[1]);
		return;
	}

	if(Data.ArgCount < 3 || Data.ArgData[2][0] == 0)
	{
		puts(" Input Data Missing");
		return;

	}

	void * InputData = new uint8_t[32 * DataSize];

	// We have a search string
	if(Data.ArgData[2][0] == '"')
	{
		if(DataSize != 1)
		{
			printf(" String Data is only valid on Byte inputs.\n");
			delete InputData;
			return;
		}						
		else if(Data.ArgData[2][strlen(Data.ArgData[2]) - 1] != '"')
		{
			printf(" Invalid String Data [%s]\n", Data.ArgData[2]);
			delete InputData;
			return;
		}

		char * TrimmedData = TrimChar(Data.ArgData[2], Data.ArgData[2][0]);
		DataLength = strlen(TrimmedData);
						
		if(DataLength > 32)
			DataLength = 32;

		memcpy(InputData, TrimmedData, DataLength);
	}
	else
	{
		for(size_t x = 2; x < Data.ArgCount; x++)
		{
			bool Pass = true;

			switch (DataSize)
			{
				case 1:
					Pass = ParseHex(Data.ArgData[x], reinterpret_cast<uint8_t *>(InputData)[DataLength]);
					break;

				case 2:
					Pass = ParseHex(Data.ArgData[x], reinterpret_cast<uint16_t *>(InputData)[DataLength]);
					break;

				case 4:
					Pass = ParseHex(Data.ArgData[x], reinterpret_cast<uint32_t *>(InputData)[DataLength]);
					break;

				case 8:
					Pass = ParseHex(Data.ArgData[x], reinterpret_cast<uint64_t *>(InputData)[DataLength]);
					break;
			}

			DataLength++;

			if(Pass == false)
			{
				printf(" Invalid Data [%s]\n", Data.ArgData[x]);	
				return;
			}
		}
	}

	for(uint32_t x = 0; x < DataLength; x++)
	{
		void *Loc = (void *)Start;
		switch (DataSize)
		{
			case 1:
				reinterpret_cast<uint8_t *>(Loc)[x] = reinterpret_cast<uint8_t *>(InputData)[x];
				break;

			case 2:
				reinterpret_cast<uint16_t *>(Loc)[x] = reinterpret_cast<uint16_t *>(InputData)[x];
				break;

			case 4:
				reinterpret_cast<uint32_t *>(Loc)[x] = reinterpret_cast<uint32_t *>(InputData)[x];
				break;

			case 8:
				reinterpret_cast<uint64_t *>(Loc)[x] = reinterpret_cast<uint64_t *>(InputData)[x];
				break;
		}
	}				

	Data.LastAddress = Start;

	delete InputData;

}

void PortCommand(CommandSet & Data)
{
	bool Write = false;
	if(toupper(Data.ArgData[0][0]) == 'O')
		Write = true;

	int DataSize = 1;
					
	switch(toupper(Data.ArgData[0][1]))
	{
		case 'B':
			DataSize = 1;
			break;
						
		case 'W':
			DataSize = 2;
			break;

		case 0:
		case 'D':
			DataSize = 4;
			break;

		default:
			printf(" Unknown Option [%c]\n", Data.ArgData[0][1]);
			return;

	}					

	if(Data.ArgCount < 2)
	{
		puts(" Port Missing");
		return;
	}

	uint32_t Port, Value;
	if(!ParseHex(Data.ArgData[1], Port))
	{
		printf(" Invalid Port [%s]\n", Data.ArgData[1]);
		return;
	}
	
	if(Write)
	{
		if(Data.ArgCount < 3)
		{
			puts(" Value Missing");
			return;
		}

		if(!ParseHex(Data.ArgData[2], Value))
		{
			printf(" Invalid Value [%s]\n", Data.ArgData[2]);
			return;
		}

		switch(DataSize)
		{
			case 1:
				OutPortb(Port, Value);
				break;

			case 2:
				OutPortw(Port, Value);
				break;

			case 4:
				OutPortd(Port, Value);
				break;
		}
	}
	else
	{
		switch(DataSize)
		{
			case 1:
				Value = InPortb(Port);
				printf("%02X\n", Value);
				break;

			case 2:
				Value = InPortw(Port);
				printf("%04X\n", Value);
				break;

			case 4:
				Value = InPortd(Port);
				printf("%08X\n", Value);
				break;
		}
	}
}


void MemoryCommand(CommandSet & Data)
{
	uint32_t Start = Data.CurrentAddress;
	if(Data.ArgCount < 2)
	{
		puts(" Address Missing");
		return;
	}

	if(!ParseAddress(Data.ArgData[1], Start, Data.LastAddress))
	{
		printf(" Invalid Address [%s]\n", Data.ArgData[1]);
		return;
	}

	Data.LastAddress = Start;

	MMUManager->PrintAddressInformation(Start);
}

void RegisterCommand(CommandSet & Data)
{
	if(Data.ArgCount < 2)
	{
		puts("Register Missing");
		puts(" Valid: MSR, CR0, CR2, CR3, CR4");
		return;
	}

	if(_stricmp("MSR", Data.ArgData[1]) == 0)
	{
		if(Data.ArgCount < 3)
		{
			puts("MSR Register Missing");
			return;
		}

		uint32_t Register = 0;
		if(!ParseHex(Data.ArgData[2], Register))
		{
			printf(" Invalid MSR Register [%s]\n", Data.ArgData[2]);
			return;
		}

		if(Data.ArgCount == 3)
		{
			uint64_t Value = ReadMSR(Register);
			printf(" %016llX\n", Value);
			return;
		}

		uint64_t Value = 0;
		if(!ParseHex(Data.ArgData[3], Value))
		{
			printf(" Invalid Value [%s]\n", Data.ArgData[3]);
			return;
		}

		WriteMSR(Register, Value);						
	}
	else if(_strnicmp("FPU", Data.ArgData[1], 3) == 0)
	{
		uint16_t Value = 0;					
		__asm FSTCW [Value];
		printf(" Control: %04X", Value);
		__asm FSTSW [Value];
		printf(" Status: %04X", Value);
						
		printf("\n");
	}
	else if(_strnicmp("SSE", Data.ArgData[1], 3) == 0)
	{
		if((ReadCR4() & CPUFlags::OSFXSR) != CPUFlags::OSFXSR)
		{
			printf(" SSE not avaliable\n");
			return;
		}

		uint32_t Value = 0;					
		__asm STMXCSR [Value];
		printf(" MXCSR: %08X\n", Value);
	}
	else if(_strnicmp("AVX", Data.ArgData[1], 3) == 0)
	{
		if((ReadCR4() & CPUFlags::OSXSAVEEnabled) != CPUFlags::OSXSAVEEnabled || 
			(ReadXCR0() & 0x06) != 0x06)
		{
			printf(" AVX not avaliable\n");
			return;
		}
		uint32_t Value = 0;					
		__asm VSTMXCSR [Value];
		printf(" MXCSR: %08X\n", Value);
	}
	else if(_strnicmp("XCR0", Data.ArgData[1], 4) == 0)
	{
		if((ReadCR4() & CPUFlags::OSXSAVEEnabled) != CPUFlags::OSXSAVEEnabled)
		{
			printf(" XCR0 not avaliable\n");
			return;
		}
						
		uint64_t Value;					
		bool Set = false;

		if(Data.ArgCount == 3)
		{
			if(!ParseHex(Data.ArgData[2], Value))
			{
				printf(" Invalid Value [%s]\n", Data.ArgData[2]);
				return;
			}
			Set = true;
		}

		if(Set)
		{
			WriteXCR0(Value);
		}
		else
		{
			Value = ReadXCR0();
			printf(" %016llX\n", Value);
		}		
	}
	else if(_strnicmp("CR", Data.ArgData[1], 2) == 0)
	{
		uint32_t Register = 0;
		switch(Data.ArgData[1][2])
		{
			case '0':
				Register = 0;
				break;

			case '2':
				Register = 2;
				break;

			case '3':
				Register = 3;
				break;

			case '4':
				Register = 4;
				break;
							
			default:
				printf(" Invalid Control Register [%s]\n", Data.ArgData[1]);
				return;
		}

						
		uint32_t Value;					
		bool Set = false;

		if(Data.ArgCount == 3)
		{
			if(!ParseHex(Data.ArgData[2], Value))
			{
				printf(" Invalid Value [%s]\n", Data.ArgData[2]);
				return;
			}
			Set = true;
		}
						
		switch(Register)
		{
			case 0:
				if(Set)
				{
					WriteCR0(Value);
				}
				else
				{
					Value = ReadCR0();
					printf(" %08X\n", Value);
				}
				break;

			case 2:
				if(Set)
				{
					WriteCR2(Value);
				}
				else
				{
					Value = ReadCR2();
					printf(" %08X\n", Value);
				}
				break;

			case 3:
				if(Set)
				{
					WriteCR3(Value);
				}
				else
				{
					Value = ReadCR3();
					printf(" %08X\n", Value);
				}
				break;

			case 4:
				if(Set)
				{
					WriteCR4(Value);
				}
				else
				{
					Value = ReadCR4();
					printf(" %08X\n", Value);
				}
				break;
		}
	}
	else
	{
		printf("Invalid arguments [%s]\n", Data.ArgData[1]);
	}
}

void InfoCommand(CommandSet & Data)
{
	if(Data.ArgCount < 2)
	{
		puts("Information Type Missing");
		puts(" Valid Types");
		puts("  ACPI:   ACPI tables and information");
		puts("  APIC:   Advanced Interrupt Controler Information");
		puts("  BIOS:   Bios information");
		puts("  CMOS:   Contents of the CMOS");
		puts("  CPUID:  CPUID information");
		puts("  HW:     Hardware Tree");
		puts("  IOAPIC: I/O APIC Information");
		puts("  MB:     Multiboot Data");
		puts("  MEM:    Memory Map");
		puts("  MMU:    Memory Manager Information");
		puts("  MP:     Multiprocessor Table");
		puts("  PIC:    Interrupt Controler Information");
		puts("  PIR:    PCI Interrupt Routing Table");
		puts("  TI:     Thread Information");
		puts("  USB:    USB1-OHCI information");

		return;
	}
	if(_stricmp("XX", Data.ArgData[1]) == 0)
	{
		OutPortb(0x64, 0x60);
		OutPortb(0x60, 0x05);
		
		OutPortb(0x60, 0xF0);
		OutPortb(0x60, 0x02);

	}
	else if(_stricmp("MMU", Data.ArgData[1]) == 0)
	{
		MMUManager->Dump();
	}
	else if(_stricmp("APIC", Data.ArgData[1]) == 0)
	{
		m_InterruptControler.DumpAPIC();
	}
	else if(_stricmp("PIC", Data.ArgData[1]) == 0)
	{
		m_InterruptControler.DumpPIC();
	}
	else if(_stricmp("IDT", Data.ArgData[1]) == 0)
	{
		CoreComplexObj::GetComplex()->IDTTable.Dump();
	}
	else if(_stricmp("GDT", Data.ArgData[1]) == 0)
	{
		CoreComplexObj::GetComplex()->GDTTable.Dump();
	}
	else if(_stricmp("MEM", Data.ArgData[1]) == 0)
	{
		CoreComplexObj::GetComplex()->PageMap.Dump();
	}
	else if(_stricmp("IOAPIC", Data.ArgData[1]) == 0)
	{
		m_InterruptControler.DumpIOAPIC();						
	}
	else if(_stricmp("TI", Data.ArgData[1]) == 0)
	{
		ThreadInformation *CurrentThread = reinterpret_cast<ThreadInformation *>(ReadFS(8));
						
		if(CurrentThread == nullptr)
			return;

		ASM_CLI;					

		printf("Thread Information\n");
		printf(" Current Thread: %08X\n", CurrentThread->ThreadID);

		CurrentThread = CoreComplexObj::GetComplex()->ThreadComplex;
		while(CurrentThread != nullptr)
		{
			printf(" Thread ID %08X, Ticks %16llX\n", CurrentThread->ThreadID, CurrentThread->TickCount);

			CurrentThread = CurrentThread->Next;
		}


		ASM_STI;

	}
	else if(_stricmp("ACPI", Data.ArgData[1]) == 0)
	{
		if(Data.ArgCount == 2)	
			CoreComplexObj::GetComplex()->ACPIComplex.Dump(nullptr);
		else
			CoreComplexObj::GetComplex()->ACPIComplex.Dump(Data.ArgData[2]);
	}
	else if(_stricmp("USB", Data.ArgData[1]) == 0)
	{
		if(USBManager == nullptr)
			printf(" No USB1-OHCI Device\n");
		else
			USBManager->Dump();
	}
	else if(_stricmp("HW", Data.ArgData[1]) == 0)
	{
		CoreComplexObj::GetComplex()->HardwareComplex.Dump();
	}
	else if(_stricmp("MB", Data.ArgData[1]) == 0)
	{
		CoreComplexObj::GetComplex()->MultiBoot.Dump();
	}
	else if(_stricmp("MP", Data.ArgData[1]) == 0)
	{
		MPConfig MPData;
		MPData.Initilize();
	}
	else if(_stricmp("PIR", Data.ArgData[1]) == 0)
	{
		uint32_t TableAddress = SearchBIOS("$PIR", 4, 0x10);
		if(TableAddress == UINT32_MAX)
		{
			printf(" No $PIR table\n");
		}
		else
		{
			printf(" %08X: PCI Interrupt Routing Table\n", TableAddress);
			PIRTable * Table = reinterpret_cast<PIRTable *>(TableAddress);			
			size_t Count = (Table->Size - 32) / 16;
			if(Table->Size == 0)
				Count = 0;

			printf("  Version %02u.%02u, Size %04X, Miniport %08X\n", (Table->Version & 0xFF00) >> 8, Table->Version & 0xFF, Table->Size, Table->MiniportData);

			printf("  IRQ Rounter Address: %02X:%02X:%02X, Vender:Dev %04X:%04X\n", Table->RounterBus, Table->RounterDevFunction >> 3, Table->RounterDevFunction & 0x07, Table->RounterVenderID, Table->RounterDeviceID);
							
			printf("  PCI Exclusive IRQs: ");
			for(int x = 0; x < 16; x++)
			{
				if(Table->PCIExcuslveIRQ & (0x1 << x))
				{
					printf("%X ", x);
				}
			}

			printf("\n");
							
			printf("  Slot Bus  Dev  Pin  Link  IRQs\n");
			for(size_t x = 0; x < Count; x++)
			{
				for(int y = 0; y < 4; y++)
				{
					if(Table->Entries[x].INT[y].LinkValue == 0)
						continue;

					printf("  % 2u   %02X   %02X    %c   %02X    ", Table->Entries[x].SlotNum, Table->Entries[x].Bus, Table->Entries[x].Device >> 3, 'A' + y, Table->Entries[x].INT[y].LinkValue);
					for(int z = 0; z < 16; z++)
					{
						if(Table->Entries[x].INT[y].IRQBitmap & (0x1 << z))
						{
							printf("%X ", z);
						}
					}

					printf("\n");
				}
			}
		}
	}
	else if(_stricmp("BIOS", Data.ArgData[1]) == 0)
	{
		printf(" Legacy BIOS Vectors\n");
		printf("  %08X: BIOS Data Area\n", 0x400); 
		printf("  %08X: Extended BIOS Data Area\n", (*reinterpret_cast<uint16_t *>(0x40E)) << 4); 
						
		{
			uint32_t *IVT = reinterpret_cast<uint32_t *>(0);
			printf("  %08X: Video Parameter Table\n", FPTR_TO_32(IVT[0x1D])); 
			printf("  %08X: Video Graphics Character Table\n", FPTR_TO_32(IVT[0x1F]));
			printf("  %08X: EGA Video Graphics Character Table\n", FPTR_TO_32(IVT[0x43]));
			printf("  %08X: Diskette Parameter Table\n", FPTR_TO_32(IVT[0x1E]));
			printf("  %08X: Fixed Disk Parameter Table 1\n", FPTR_TO_32(IVT[0x41]));
			printf("  %08X: Fixed Disk Parameter Table 2\n", FPTR_TO_32(IVT[0x46]));
		}

		printf("\n");

		printf(" Legacy BIOS ROMs\n");
		uint32_t TableAddress;
		for(TableAddress = 0xc0000; TableAddress < 0x100000; TableAddress += 0x800)
		{
			if(*reinterpret_cast<uint16_t *>(TableAddress) == 0xAA55)
			{
				printf("   %08X\n", TableAddress);
				uint16_t *Data = reinterpret_cast<uint16_t *>(TableAddress);
				char * ExpansionAddress = reinterpret_cast<char *>(TableAddress);
				if(Data[12] != 0000)
				{
					ExpansionAddress += Data[12];

					// This should point to the PCIR data
					if(strncmp(ExpansionAddress, "PCIR", 4) == 0)
						printf("    PCIR: %08X\n", ExpansionAddress);
				}
								
				ExpansionAddress = reinterpret_cast<char *>(TableAddress);

				if(Data[13] != 0000)
				{
					// Expansion header, should be $PnP, but video can point to the $VBT data
					ExpansionAddress += Data[13];
					if(strncmp(ExpansionAddress, "$PnP", 4) == 0)
						printf("    $PnP: %08X\n", ExpansionAddress);
					else if(strncmp(ExpansionAddress, "$VBT", 4) == 0)
						printf("    $VBT: %08X\n", ExpansionAddress);
				}

				uint32_t PMID = SeachMemory(TableAddress, (Data[1] & 0xFF) * 512, "PMID", 4);
				if(PMID != UINT32_MAX)
				{
					printf("    PMID: %08X\n", PMID);
				}
			}
		}

		printf("\n");

		printf(" BIOS Tables\n");

		// Start with the well know tables
		TableAddress = 0xF0000;
		while((TableAddress = SeachMemory(TableAddress, 0x10000, "$PnP", 4, 0x10)) != UINT32_MAX)
		{
			if(*reinterpret_cast<uint8_t *>(TableAddress + 4) == 0x10)
				printf("   %08X: \"$PnP\" - PnP BIOS Table\n", TableAddress);

			TableAddress += 4;
		};
						
		TableAddress = SearchBIOS("$PMM", 4, 0x10);
		if(TableAddress != UINT32_MAX)
			printf("   %08X: \"$PMM\" - POST Memory Manager\n", TableAddress);

		TableAddress = SearchBIOS("$PIR", 4, 0x10);
		if(TableAddress != UINT32_MAX)
			printf("   %08X: \"$PIR\" - PCI Interrupt Routing Table\n", TableAddress);

		TableAddress = SearchBIOS("IFE$", 4, 0x10);
		if(TableAddress != UINT32_MAX)
			printf("   %08X: \"IFE$\" - EFI Compatablity Table\n", TableAddress);

		TableAddress = SearchBIOS("_MP_", 4, 0x10);
		if(TableAddress != UINT32_MAX)
			printf("   %08X: \"_MP_\" - MultiProcessor Table Pointer\n", TableAddress);

		TableAddress = SearchBIOS("_32_", 4, 0x10);
		if(TableAddress != UINT32_MAX)
			printf("   %08X: \"_32_\" - 32-Bit BIOS Entry Point\n", TableAddress);

		TableAddress = SearchBIOS("_SM_", 4, 0x10);
		if(TableAddress != UINT32_MAX)
		{
			printf("   %08X: \"_SM_\" - SMBios Table Pointer\n", TableAddress);
		}
		else
		{
			TableAddress = SearchBIOS("_DMI_", 5, 0x10);
			if(TableAddress != UINT32_MAX)
				printf("   %08X: \"_DMI_\" - DMI Table Pointer\n", TableAddress);
		}

		TableAddress = SearchBIOS("RSD PTR ", 8, 0x10);
		if(TableAddress != UINT32_MAX)
			printf("   %08X: \"RDS PTR \" - ACPI Root Pointer\n", TableAddress);


		/*
		uint32_t LastAddress = 0xC0000;
						
		while((LastAddress = SeachMemory(LastAddress, 0x40000, "$", 1, 0x01)) != UINT32_MAX)
		{
			if(LastAddress >= 0x100000)
				break;

			if((LastAddress & 0xF) == 0)
			{
				char *Data = reinterpret_cast<char *>(LastAddress);

				if(isalnum(Data[1]) && isalnum(Data[2]) && isalnum(Data[3]))
				{							
					printf("  %08X: %4.4s\n", LastAddress, LastAddress);
				}
			}
			else if((LastAddress & 0xF) == 3)
			{
				char *Data = reinterpret_cast<char *>(LastAddress-3);

				if(isalnum(Data[0]) && isalnum(Data[1]) && isalnum(Data[2]))
				{							
					printf("  %08X: %4.4s\n", LastAddress-4, LastAddress-4);
				}
							
			}

			LastAddress +=1;
		};
		*/


	}
	else if(_stricmp("CMOS", Data.ArgData[1]) == 0)
	{
		char Buffer[17];
		Buffer[16] = 0;
		int Pos = 0;
		for(int x = 0; x < 0x80; x++, Pos++)
		{
			if(x % 0x10 == 0)
			{
				if(x != 0)
				{			
					printf("   %s\n", Buffer);
				}

				printf("      %02X:", x);
				Pos = 0;
			}

			if(x % 0x10 != 0 && x % 0x08 == 0)
				printf("-");
			else
				printf(" ");

			OutPortb(0x70, x);
			InPortb(0);
			InPortb(0);
			InPortb(0);
			InPortb(0);
			uint8_t Val = InPortb(0x71);

			printf("%02X", Val);

			if(Val < ' ' || Val > 127)
				Buffer[Pos] = '.';
			else
				Buffer[Pos] = Val;
		}
						
		printf("   %s\n", Buffer);
	}
	else if(_stricmp("CPUID", Data.ArgData[1]) == 0)
	{
		Registers Res;
		if(Data.ArgCount == 2)
		{
			ReadCPUID(0, 0, &Res);
			uint32_t LeafCount = Res.EAX;
			uint32_t ExtendedLeafCount = 0;
			bool Extended = false;
			////////123456789012345678901234
			printf(" Signature:             %4.4s%4.4s%4.4s\n", &Res.EBX, &Res.EDX, &Res.ECX);
							
			Res.EAX = 0;
			ReadCPUID(0x80000000, 0, &Res);
			Extended = Res.EAX != 0;
			ExtendedLeafCount = Res.EAX;

			if(Extended)							
			{
				ReadCPUID(0x80000002, 0, &Res);
				////////123456789012345678901234
				printf(" Brand:                 %4.4s%4.4s%4.4s%4.4s", &Res.EAX, &Res.EBX, &Res.ECX, &Res.EDX);
				ReadCPUID(0x80000003, 0, &Res);
				printf("%4.4s%4.4s%4.4s%4.4s", &Res.EAX, &Res.EBX, &Res.ECX, &Res.EDX);
				ReadCPUID(0x80000004, 0, &Res);
				printf("%4.4s%4.4s%4.4s%4.4s\n", &Res.EAX, &Res.EBX, &Res.ECX, &Res.EDX);
			}

			////////123456789012345678901234
			printf(" Leaf Count:            %X\n", LeafCount);
			printf(" Extended Leaf Count:   %X\n", ExtendedLeafCount);

			uint32_t Features[5];
			Features[0] = Features[1] = Features[2] = Features[3] = Features[4] = 0;

			ReadCPUID(1, 0, &Res);
			uint8_t Model = (Res.EAX & 0xF0) >> 4;
			uint8_t FamilyID = (Res.EAX & 0xF00) >> 8;

			if(FamilyID == 0x06 || FamilyID == 0x0F)
			{
				Model += (Res.EAX & 0xF0000) >> 12;
			}

			if(FamilyID == 0x0F)
			{
				FamilyID += (Res.EAX & 0xFF00000) >> 20;
			}

			////////123456789012345678901234
			printf(" CPU Type %X, Family %X, Model %X, Stepping %X\n", (Res.EAX & 0xF000) >> 12, FamilyID, Model, Res.EAX & 0x0F);
			printf(" Brand Index:           %02X\n", Res.EBX & 0xFF);
			printf(" CLFLUSH Size:          %02X\n", (Res.EBX & 0x0000FF00) >> 8);
			printf(" Max ID:                %02X\n", (Res.EBX & 0x00FF0000) >> 16);
			printf(" Local APIC ID:         %02X\n", (Res.EBX & 0xFF000000) >> 24);
			printf(" Features 1 (EDX):      %08X\n", Res.EDX);
			printf(" Features 2 (ECX):      %08X\n", Res.ECX);

			Features[0] = Res.EDX;
			Features[1] = Res.ECX;

			if(LeafCount >= 0x07)
			{								
				ReadCPUID(0x07, 0, &Res);
				////////123456789012345678901234
				printf(" Structured Features:   %08X\n", Res.EBX);
				Features[2] = Res.EBX;

			}
							
			if(LeafCount >= 0x0D)
			{								
				ReadCPUID(0x0D, 0, &Res);
				////////123456789012345678901234
				printf(" XCR0 Valid Bits:       %08X:%08X\n", Res.EDX, Res.EAX);
				printf(" XSAVE/XRSTOR size:     %08X\n", Res.EBX);
				printf(" XSAVE/XRSTOR Max:      %08X\n", Res.ECX);
				ReadCPUID(0x0D, 1, &Res);
				////////123456789012345678901234
				printf(" XSAVE Flags (EAX):     %08X\n", Res.EAX);
				printf(" XSAVE XSS size:        %08X\n", Res.EBX);
				printf(" Valid IA32_XSS Bits:   %08X:%08X\n", Res.EDX, Res.ECX);
			}


			if(Extended)							
			{
				ReadCPUID(0x80000001, 0, &Res);
				////////123456789012345678901234
				printf(" ExFeatures 1 (EDX):    %08X\n", Res.EDX);
				printf(" ExFeatures 2 (ECX):    %08X\n", Res.ECX);

				Features[3] = Res.EDX;
				Features[4] = Res.ECX;

				if(ExtendedLeafCount >= 0x80000008)
				{
					ReadCPUID(0x80000008, 0, &Res);
					////////123456789012345678901234
					printf(" Physical Address:      %u\n", Res.EAX & 0xFF);
					printf(" Linear Address:        %u\n", (Res.EAX & 0xFF00) >> 8);
				}
			}
							
			uint32_t Mask = 0;
			for(int x = 0; x < 32 * 5; x++)
			{
				if(Mask == 0)
					Mask = 0x00000001;

				if(Features[x / 32] & Mask)
					printf("  %s\n", CPUIDFlags[x]);

				Mask = Mask << 1;
			}
							
		}
		else
		{
			uint32_t ParamID = 0;
			ParseHex(Data.ArgData[2], ParamID);							

			uint32_t ParamID2 = 0;
			if(Data.ArgCount >= 4)
			{
				ParseHex(Data.ArgData[3], ParamID2);
			}
							
			ReadCPUID(ParamID, ParamID2, &Res);
			printf(" EAX: %08X, EBX: %08X, ECX: %08X, EDX: %08X\n", Res.EAX, Res.EBX, Res.ECX, Res.EDX);
		}
	}
	else
	{
		printf("Invalid arguments [%s]\n", Data.ArgData[1]);
	}
}
	
void ObjectCommand(CommandSet & Data)
{
	CoreComplexObj::GetComplex()->ObjectComplex.DisplayObjects(Data.ArgCount - 1, &Data.ArgData[1]);
}

void DisassembleCommand(CommandSet & Data)
{
	int DataSize = 32;
	uint32_t Length = 0x20;
	uint32_t Address = Data.CurrentAddress;

	if(Data.ArgData[0][1] == 0)
	{
		DataSize = 32;
	}
	else if(strcmp(&Data.ArgData[0][1], "16") == 0)
	{
		DataSize = 16;
	}
	else if(strcmp(&Data.ArgData[0][1], "32") == 0)
	{
		DataSize = 32;
	}
	else if(strcmp(&Data.ArgData[0][1], "64") == 0)
	{
		DataSize = 64;
	}
	else
	{
		printf(" Unknown Option [%s]\n", &Data.ArgData[0][1]);
		return;
	}
					
	if(Data.ArgCount >= 2)
	{
		if(!ParseAddress(Data.ArgData[1], Address, Data.LastAddress))
		{
			printf(" Invalid Address [%s]\n", Data.ArgData[1]);
			return;
		}
	}

	if(Data.ArgCount >= 3)
	{
		if(!ParseHex(Data.ArgData[2], Length))
		{
			printf(" Invalid Length [%s]\n", Data.ArgData[2]);
			return;
		}
	}

	Data.LastAddress = Address;
	Disassembler Dis;
	Data.CurrentAddress = Address + Dis.Disassamble(reinterpret_cast<intptr_t *>(Address), Length, DataSize, DataSize);
}

void main(int argc, char *argv[])
{
	char InputBuffer[0x100];

	CommandSet CommandData;
	CommandData.ArgCount = 0;
	CommandData.CurrentAddress = 0x100000;
	CommandData.LastAddress = 0x100000;

	while(true)
	{
		printf("\00307%08X> ", CommandData.CurrentAddress);
		gets_s(InputBuffer, 0x100);

		char * Input = TrimString(InputBuffer);		
		CommandData.ArgCount = _ConvertCommandLineToArgcArgv(Input, CommandData.ArgData, 31, true);

		if(CommandData.ArgCount == 0 || CommandData.ArgData[0][0] == 0)
			continue;

		//for(int x = 0; x < CommandData.ArgCount; x++)
		//{
		//	printf(" %02u: %s\n", x, CommandData.ArgData[x]);
		//}

		if(CommandData.ArgData[0][0] == '?')
		{
			puts("BootDebug Command List");
			for(CommandEntry * Entry = Commands; Entry->Command != 0; Entry++)
				printf(" %s\n", Entry->Help);

			puts("");
			puts(" Size = B|W|D|Q: Byte, Word, DWord or QWord");

		}
		else
		{
			bool Matched = false;
			for(CommandEntry * Entry = Commands; Entry->Command != 0; Entry++)
			{
				if(toupper(	CommandData.ArgData[0][0] ) == Entry->Command)
				{
					if(Entry->Function != nullptr)
						Entry->Function(CommandData);
				
					Matched = true;

					break;
				}
			}

			if(!Matched)
				printf("Unknown Command [%s]\n", CommandData.ArgData[0]);
		}
	};
}
