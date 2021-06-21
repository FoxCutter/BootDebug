#pragma once
#include <stdint.h>
#include "RawTerminal.h"

extern RawTerminal * KernalTerminal;

enum class KernalCode : uint32_t
{
	NoError = 0,

	MemoryError  = 0x80000001,

	GeneralError = 0xFFFFFFFF,
};

void * KernalAlloc(uint32_t Size);
void KernalFree(void * Address);

int __cdecl KernalPrintf(const char * format, ...);
int __cdecl KernalSprintf(char * Dest, uint32_t cCount, const char * format, ...);
int __cdecl KernalVprintf(const char * format, va_list Args);
int __cdecl KernalPanic(KernalCode Error, const char * format, ...);

void KernalSetPauseFullScreen(bool Value);

void KernalSleep(int Milliseconds);
void KernalWaitATick();

enum class KernalPageFlags
{
	None = 0x0000,
	Fixed = 0x1000,
	Raw = 0x2000
};

// Allocates Memory pages in the Kernal Space
void * KernalPageAllocate(uint32_t Size, KernalPageFlags Flags, char *Name);

//char * KernalGetInput(char * Buffer, uint32_t Size);
//char KernalGetInput();
