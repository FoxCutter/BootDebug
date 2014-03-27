#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "Utility.h"

#include "..\StdLib\argcargv.h"

#include "PCI.h"
#include "MPConfig.h"
#include "RawTerminal.h"
#include "LowLevel.h"

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

void main(int argc, char *argv[])
{
	char InputBuffer[1024];
	bool Done = false;

	PCI PCIBus;
	MPConfig MPData;
	
	uint32_t DumpAddress = 0x100000;

	do
	{
		printf("> ");
		gets_s(InputBuffer, 1024);

		char * Input= TrimString(InputBuffer);

		int ArgCount = _ConvertCommandLineToArgcArgv(Input);

		switch(toupper(_ppszArgv[0][0]))
		{
			case 'Q':
				Done = true;
				break;

			case 'D':
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

					switch(toupper(_ppszArgv[0][1]))
					{
						default:	
						case 'B':
							PrintBytes((void *)DumpAddress, Length);
							break;

						case 'W':
							PrintWords((void *)DumpAddress, Length);
							break;

						case 'D':
							PrintDWords((void *)DumpAddress, Length);
							break;
					}

					DumpAddress += Length;
				}
				break;

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

			case 'N':
				{
					if(ArgCount < 2)
					{
						puts("Invalid arguments");
						break;
					}

					if(toupper(_ppszArgv[1][0]) == 'M' && toupper(_ppszArgv[1][1]) == 'P')
					{
						MPData.Initilize();
					}
					else if(toupper(_ppszArgv[1][0]) == 'P' && toupper(_ppszArgv[1][1]) == 'C' && toupper(_ppszArgv[1][2]) == 'I')
					{
						if(ArgCount < 3)
						{
							PCIBus.EnumerateBus(0);
						}
						else if(ArgCount == 3)
						{
							uint32_t DeviceID = 0;
							ParseHex(_ppszArgv[2], DeviceID);
							PCIBus.DumpDevice(DeviceID);
						}
						else if(ArgCount == 4)
						{
							uint32_t DeviceID = 0;
							uint32_t Register = 0;
							ParseHex(_ppszArgv[2], DeviceID);
							ParseHex(_ppszArgv[3], Register);
							uint32_t Value = PCIBus.ReadRegister(DeviceID, Register);

							printf(" %08X\n", Value);
						}
						else if(ArgCount >= 5)
						{
							uint32_t DeviceID = 0;
							uint32_t Register = 0;
							uint32_t Value = 0;
							ParseHex(_ppszArgv[2], DeviceID);
							ParseHex(_ppszArgv[3], Register);
							ParseHex(_ppszArgv[4], Value);

							PCIBus.SetRegister(DeviceID, Register, Value);
						}
					}
					else
					{
						puts("Invalid arguments");
					}
				}
				
				break;

			case 'F':
				break;

			case 'E':
				{

				}
				break;

			case 'I':
				{
					if(ArgCount < 2)
					{
						puts("Invalid arguments");
						break;
					}

					uint32_t Port, Res;
					ParseHex(_ppszArgv[1], Port);

					switch(toupper(_ppszArgv[0][1]))
					{
						case 'B':
							Res = InPortb(Port);
							printf("%02X\n", Res);
							break;

						case 'W':
							Res = InPortw(Port);
							printf("%04X\n", Res);
							break;

						default:
						case 'D':
							Res = InPortd(Port);
							printf("%08X\n", Res);
							break;
					}
				}
				break;

			case 'O':
				{
					if(ArgCount < 3)
					{
						puts("Invalid arguments");
						break;
					}

					uint32_t Port, Val;
					ParseHex(_ppszArgv[1], Port);
					ParseHex(_ppszArgv[2], Val);
					switch(toupper(_ppszArgv[0][1]))
					{
						case 'B':
							OutPortb(Port, Val);
							break;

						case 'W':
							OutPortw(Port, Val);
							break;

						default:
						case 'D':
							OutPortd(Port, Val);
							break;
					}
				}
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
