#include <stdint.h>
#pragma once

void PrintLongAddress(uint64_t Address);
uint32_t SeachMemory(uint32_t Start, uint32_t Count, const void *Search, uint32_t DataLength, uint32_t Alignment = 0x01);
bool ValidateChecksum(void *Data, uint16_t Length);

void PrintMemoryBlock(void *Address, int Length, uint8_t Size);

void PrintBytes(void *Address, int Length);
void PrintWords(void *Address, int Length);
void PrintDWords(void *Address, int Length);
void PrintQWords(void *Address, int Length);

char * NextToken(char *&Input);

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