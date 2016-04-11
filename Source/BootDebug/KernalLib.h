#pragma once
#include <stdint.h>
#include "RawTerminal.h"

extern RawTerminal * KernalTerminal;

enum class KernalCode : uint32_t
{
	NoError = 0,

	GeneralError = 0xFFFFFFFF,
};

void * KernalAlloc(uint32_t Size);
void KernalFree(void * Address);

int KernalPrintf(const char * format, ...);
int KernalSprintf(char * Dest, uint32_t cCount, const char * format, ...);
int KernalVprintf(const char * format, va_list Args);
int KernalPanic(KernalCode Error, const char * format, ...);

enum class KernalPageFlags
{
	None = 0x0000,
	Fixed = 0x1000,
	Raw = 0x2000
};

// Allocates Memory pages in the Kernal Space
void * KernalPageAllocate(uint32_t Size, KernalPageFlags Flags);

//char * KernalGetInput(char * Buffer, uint32_t Size);
//char KernalGetInput();
