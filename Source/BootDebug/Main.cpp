#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "Utility.h"

#include "..\StdLib\argcargv.h"

#include "MMU.h"
#include "PCI.h"
#include "OpenHCI.h"
#include "MPConfig.h"
#include "RawTerminal.h"
#include "LowLevel.h"
#include "MultiBootInfo.h"
#include "ACPI.h"

MultiBootInfo * MultiBootHeader = nullptr;
MMU * MMUManager = nullptr;
OpenHCI * USBManager = nullptr;

template <typename DataType>
bool ParseHex(char *String, DataType &Value)
{
	DataType RetValue = 0;

	size_t CurrentLength = 0;
	size_t MaxLength = sizeof(DataType) * 2;	
	
	if(String[0] == 0)
		return true;

	while(*String != 0)
	{
		if(CurrentLength == MaxLength)
			return false;
		
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
		CurrentLength ++;
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

char * NextToken(char *&Input)
{
	if(Input == nullptr)
		return nullptr;

	if(Input[0] == 0)
		return Input;

	char * TokenStart = Input;
	Input = nullptr;

	bool InQuote = false;

	while(TokenStart[0] != 0 && isspace(*TokenStart))
		TokenStart++;

	for(int x = 0; TokenStart[x] != 0; x++)
	{
		if(TokenStart[x] == '"')
		{
			if(x == 0)
				InQuote = true;

			else
				InQuote = false;
		}
		else if(isspace(TokenStart[x]) && !InQuote)
		{
			TokenStart[x] = 0;			
			Input = &TokenStart[x + 1];
			break;
		}
	}

	return TokenStart;
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

static uint32_t LastAddress = 0x100000;

bool ParseAddress(char *Value, uint32_t &Address)
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

void main(int argc, char *argv[])
{
	char InputBuffer[1024];
	bool Done = false;

	PCI PCIBus;
	MPConfig MPData;
	
	uint32_t DumpAddress = 0x100000;

	do
	{
		printf("\00307%08X> ", DumpAddress);
		gets_s(InputBuffer, 1024);

		char * Input = TrimString(InputBuffer);		
		char *CurrentData = NextToken(Input);
		
		switch(toupper(CurrentData[0]))
		{
			case 'D':
				{
					int DumpSize = 1;
					uint32_t Length = 0x80;
					uint32_t Address = DumpAddress;

					switch(toupper(CurrentData[1]))
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
							printf(" Unknown Option [%c]\n", CurrentData[1]);
							continue;

					}

					CurrentData = NextToken(Input);
					if(CurrentData != nullptr)
					{
						if(!ParseAddress(CurrentData, Address))
						{
							printf(" Invalid Address [%s]\n", CurrentData);
							continue;
						}
					}

					CurrentData = NextToken(Input);

					if(CurrentData != nullptr)
					{
						if(!ParseHex(CurrentData, Length))
						{
							printf(" Invalid Length [%s]\n", CurrentData);
							continue;
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
						LastAddress = Address;

						PrintMemoryBlock((void *)Address, Length, DumpSize);
						Address += Length;
					}
					

					DumpAddress = Address;

					
				}
				break;

			case 'S':
				{					
					int DataSize = 1;
					
					switch(toupper(CurrentData[1]))
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
							printf(" Unknown Option [%c]\n", CurrentData[1]);
							continue;

					}					
					
					uint32_t Start = DumpAddress;
					uint32_t Count = 0;
					uint32_t DataLength = 0;

					CurrentData = NextToken(Input);
					if(CurrentData == nullptr)
					{
						puts(" Address Missing");
						continue;
					}
					
					if(!ParseAddress(CurrentData, Start))
					{
						printf(" Invalid Address [%s]\n", CurrentData);
						continue;
					}

					CurrentData = NextToken(Input);
					if(CurrentData == nullptr)
					{
						puts(" Length Missing");
						continue;
					}

					if(!ParseHex(CurrentData, Count))
					{
						printf(" Invalid Length [%s]\n", CurrentData);
						break;
					}
					
					Input = TrimString(Input);

					if(Input == nullptr || Input[0] == 0)
					{
						puts(" Search Data Missing");
						break;

					}
					
					void * Data = new uint8_t[32 * DataSize];

					// We have a search string
					if(Input[0] == '"')
					{
						if(DataSize != 1)
						{
							printf(" String Data is only valid on Byte searches.\n", Input);
							delete Data;
							break;
						}						
						else if(Input[strlen(Input) - 1] != '"')
						{
							printf(" Invalid String Data [%s]\n", Input);
							delete Data;
							break;
						}

						char * InputData = TrimChar(Input, Input[0]);
						DataLength = strlen(InputData);
						
						if(DataLength > 32)
							DataLength = 32;

						memcpy(Data, InputData, DataLength);
					}
					else
					{
						if(!ParseData(Input, Data, DataLength, DataSize))
						{
							delete Data;
							break;
						}
					}

					uint32_t Pos = Start;

					while(true)
					{
						uint32_t Loc = SeachMemory(Pos, Count, Data, DataLength * DataSize, DataSize);

						if(Loc == UINT32_MAX)
							break;

						PrintMemoryBlock((void *)Loc, DataLength * DataSize, DataSize);
						
						Loc += DataLength * DataSize;
						
						Count = Count - (Loc - Pos);
						Pos = Loc;
					}					

					LastAddress = Start;

					delete Data;
				}
				break;

			case 'N':
				{
					CurrentData = NextToken(Input);
					if(CurrentData == nullptr)
					{
						puts("Information Type Missing");
						puts(" Valid: MB, MP, CPUID, PCI, USB");
						continue;
					}
					
					if(_stricmp("ACPI", CurrentData) == 0)
					{
						ACPI::Dump();
					}
					else if(_stricmp("USB", CurrentData) == 0)
					{
						USBManager->Dump();
					}
					else if(_stricmp("MB", CurrentData) == 0)
					{
						MultiBootHeader->Dump();
					}
					else if(_stricmp("MP", CurrentData) == 0)
					{
						MPData.Initilize();
					}
					else if(_stricmp("CPUID", CurrentData) == 0)
					{
						Registers Res;
						CurrentData = NextToken(Input);
						if(CurrentData == nullptr)
						{
							ReadCPUID(0, 0, &Res);
							printf(" Signature: %4.4s%4.4s%4.4s\n", &Res.EBX, &Res.EDX, &Res.ECX);
							printf(" Basic Leaf Count: %X\n", Res.EAX);

							ReadCPUID(1, 0, &Res);
							printf(" CPU Type %X, Family %X, Model %X, Stepping %X\n", (Res.EAX & 0xF000) >> 12, (Res.EAX & 0xF00) >> 8, (Res.EAX & 0xF0) >> 4, Res.EAX & 0x0F);
							printf(" Brand Index: %02X\n", Res.EBX & 0xFF);
							printf(" CLFLUSH Size: %02X\n", (Res.EBX & 0x0000FF00) >> 8);
							printf(" Max ID: %02X\n", (Res.EBX & 0x00FF0000) >> 16);
							printf(" Initial APIC ID: %02X\n", (Res.EBX & 0xFF000000) >> 24);
							printf(" Features 1: %08X\n", Res.EDX);
							printf(" Features 2: %08X\n", Res.ECX);
						}
						else
						{
							uint32_t ParamID = 0;
							ParseHex(CurrentData, ParamID);							

							uint32_t ParamID2 = 0;
							CurrentData = NextToken(Input);
							if(CurrentData == nullptr)
							{
								ParseHex(CurrentData, ParamID2);
							}
							
							ReadCPUID(ParamID, ParamID2, &Res);
							printf(" EAX: %08X, EBX: %08X, ECX: %08X, EDX: %08X\n", Res.EAX, Res.EBX, Res.ECX, Res.EDX);
						}
					}
					else if(_stricmp("PCI", CurrentData) == 0)
					{
						CurrentData = NextToken(Input);
						if(CurrentData == nullptr)
						{
							PCIBus.Initilize();
							break;
						}

						uint32_t DeviceID = 0;
						if(!ParseHex(CurrentData, DeviceID))
						{
							printf(" Invalid Device [%s]\n", CurrentData);
							continue;
						}

						CurrentData = NextToken(Input);
						if(CurrentData == nullptr)
						{
							PCIBus.DumpDevice(DeviceID);
							break;
						}
						
						uint32_t Register = 0;
						if(!ParseHex(CurrentData, Register))
						{
							printf(" Invalid Register [%s]\n", CurrentData);
							continue;
						}

						CurrentData = NextToken(Input);
						if(CurrentData == nullptr)
						{
							uint32_t Value = PCIBus.ReadRegister(DeviceID, Register);
							printf("%02X: %08X\n", Register & 0xFC, Value);
							break;
						}

						uint32_t Value = 0;
						if(!ParseHex(CurrentData, Value))
						{
							printf(" Invalid Value [%s]\n", CurrentData);
							continue;
						}

						PCIBus.SetRegister(DeviceID, Register, Value);
					}
					else
					{
						printf("Invalid arguments [%s]\n", CurrentData);
					}
				}
				
				break;

			case 'R':
				{
					CurrentData = NextToken(Input);
					if(CurrentData == nullptr)
					{
						puts("Register Missing");
						puts(" Valid: MSR, CR0, CR2, CR3, CR4");
						continue;
					}

					if(_stricmp("MSR", CurrentData) == 0)
					{
						CurrentData = NextToken(Input);
						if(CurrentData == nullptr)
						{
							puts("MSR Register Missing");
							break;
						}

						uint32_t Register = 0;
						if(!ParseHex(CurrentData, Register))
						{
							printf(" Invalid MSR Register [%s]\n", CurrentData);
							continue;
						}

						CurrentData = NextToken(Input);
						if(CurrentData == nullptr)
						{
							uint64_t Value = ReadMSR(Register);
							printf(" %016llX\n", Value);
							break;
						}

						uint64_t Value = 0;
						if(!ParseHex(CurrentData, Value))
						{
							printf(" Invalid Value [%s]\n", CurrentData);
							continue;
						}

						WriteMSR(Register, Value);						
					}
					else if(_strnicmp("CR", CurrentData, 2) == 0)
					{
						uint32_t Register = 0;
						switch(CurrentData[2])
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
								printf(" Invalid Control Register [%s]\n", CurrentData);
								continue;
						}

						
						uint32_t Value;					
						bool Set = false;

						CurrentData = NextToken(Input);
						if(CurrentData != nullptr)
						{
							if(!ParseHex(CurrentData, Value))
							{
								printf(" Invalid Value [%s]\n", CurrentData);
								continue;
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
						printf("Invalid arguments [%s]\n", CurrentData);
					}
				}
				
				break;

			case 'F':
				{
				}
				break;

			case 'E':
				{					
					int DataSize = 1;
					
					switch(toupper(CurrentData[1]))
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
							printf(" Unknown Option [%c]\n", CurrentData[1]);
							continue;

					}					
					
					uint32_t Start = DumpAddress;
					uint32_t DataLength = 0;

					CurrentData = NextToken(Input);
					if(CurrentData == nullptr)
					{
						puts(" Address Missing");
						continue;
					}
					
					if(!ParseAddress(CurrentData, Start))
					{
						printf(" Invalid Address [%s]\n", CurrentData);
						continue;
					}

					Input = TrimString(Input);

					if(Input == nullptr || Input[0] == 0)
					{
						puts(" Input Data Missing");
						break;

					}					
					void * Data = new uint8_t[32 * DataSize];

					// We have an Input string
					if(Input[0] == '"')
					{
						if(DataSize != 1)
						{
							printf(" String Data is only valid on Byte Input.\n", Input);
							delete Data;
							break;
						}						
						else if(Input[strlen(Input) - 1] != '"')
						{
							printf(" Invalid String Data [%s]\n", Input);
							delete Data;
							break;
						}

						char * InputData = TrimChar(Input, Input[0]);
						DataLength = strlen(InputData);
						
						if(DataLength > 32)
							DataLength = 32;

						memcpy(Data, InputData, DataLength);
					}
					else
					{
						if(!ParseData(Input, Data, DataLength, DataSize))
						{
							delete Data;
							break;
						}
					}
					
					for(uint32_t x = 0; x < DataLength; x++)
					{
						void *Loc = (void *)Start;
						switch (DataSize)
						{
							case 1:
								reinterpret_cast<uint8_t *>(Loc)[x] = reinterpret_cast<uint8_t *>(Data)[x];
								break;

							case 2:
								reinterpret_cast<uint16_t *>(Loc)[x] = reinterpret_cast<uint16_t *>(Data)[x];
								break;

							case 4:
								reinterpret_cast<uint32_t *>(Loc)[x] = reinterpret_cast<uint32_t *>(Data)[x];
								break;

							case 8:
								reinterpret_cast<uint64_t *>(Loc)[x] = reinterpret_cast<uint64_t *>(Data)[x];
								break;
						}
					}				

					LastAddress = Start;

					delete Data;
				}
				
				break;

			case 'I':
				{
					int DataSize = 1;
					
					switch(toupper(CurrentData[1]))
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
							printf(" Unknown Option [%c]\n", CurrentData[1]);
							continue;

					}					

					CurrentData = NextToken(Input);
					if(CurrentData == nullptr)
					{
						puts(" Port Missing");
						continue;
					}

					uint32_t Port, Res;
					if(!ParseHex(CurrentData, Port))
					{
						printf(" Invalid Port [%s]\n", CurrentData);
						continue;
					}

					switch(DataSize)
					{
						case 1:
							Res = InPortb(Port);
							printf("%02X\n", Res);
							break;

						case 2:
							Res = InPortw(Port);
							printf("%04X\n", Res);
							break;

						case 4:
							Res = InPortd(Port);
							printf("%08X\n", Res);
							break;
					}
				}
				break;

			case 'O':
				{
					int DataSize = 1;
					
					switch(toupper(CurrentData[1]))
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
							printf(" Unknown Option [%c]\n", CurrentData[1]);
							continue;

					}					

					CurrentData = NextToken(Input);
					if(CurrentData == nullptr)
					{
						puts(" Port Missing");
						continue;
					}

					uint32_t Port;
					if(!ParseHex(CurrentData, Port))
					{
						printf(" Invalid Port [%s]\n", CurrentData);
						continue;
					}

					CurrentData = NextToken(Input);
					if(CurrentData == nullptr)
					{
						puts(" Value Missing");
						continue;
					}

					uint32_t Val;
					if(!ParseHex(CurrentData, Val))
					{
						printf(" Invalid Value [%s]\n", CurrentData);
						continue;
					}

					switch(DataSize)
					{
						case 1:
							OutPortb(Port, Val);
							break;

						case 2:
							OutPortw(Port, Val);
							break;

						case 4:
							OutPortd(Port, Val);
							break;
					}
				}
				break;


			case 'W':
				{
					uint32_t Start, Length;
					CurrentData = NextToken(Input);
					if(CurrentData == nullptr)
					{
						puts(" Start Address Missing");
						continue;
					}

					if(!ParseAddress(CurrentData, Start))
					{
						printf(" Invalid Start Address [%s]\n", CurrentData);
						continue;
					}

					CurrentData = NextToken(Input);
					if(CurrentData == nullptr)
					{
						puts(" Length Missing");
						continue;
					}

					if(!ParseHex(CurrentData, Length))
					{
						printf(" Invalid Length [%s]\n", CurrentData);
						continue;
					}

					uint8_t * Data = (uint8_t *)Start;
					for(uint32_t x = 0; x < Length; x++)
					{
						// Spin until the ports ready
						while ((InPortb(0x03F8 + 5) & 0x20) != 0x20);
						OutPortb(0x03F8, Data[x]);
					}
					
					LastAddress = Start;
				}
				break;

			case 'X':
				{
					uint32_t Start = DumpAddress;
					CurrentData = NextToken(Input);
					if(CurrentData == nullptr)
					{
						puts(" Address Missing");
						continue;
					}

					if(!ParseAddress(CurrentData, Start))
					{
						printf(" Invalid Address [%s]\n", CurrentData);
						continue;
					}

					LastAddress = Start;

					MMUManager->PrintAddressInformation(Start);
				}

				break;

			case '?':
				puts("BootDebug Command List");
				puts(" Dump Memory           D[Size] [Address] [Length]");
				puts(" Enter Data            E[Size] Address Data");
				puts(" Search Memory         S[Size] Address Length Data");
				puts(" Read from a Port      I[Size] Port");
				puts(" Write to a Port       O[Size] Port Value");
				puts(" Memory Information    X Address");
				puts(" Register Information  R Register [Value]");
				puts(" General Information   N [Type]");
				puts("");
				puts(" Size = B|W|D|Q: Byte, Word, DWord or QWord");
				break;

			default:
				if(CurrentData[0] != 0)
					printf("Unknown Command [%s]\n", CurrentData);
				break;
		};
	} while (!Done);

	return;
}

