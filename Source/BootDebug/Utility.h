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
