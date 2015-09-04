#pragma once
#include <stdint.h>
#include "RawTerminal.h"

extern RawTerminal * KernalTerminal;

namespace KernalCode
{
	enum Codes
	{
		NoError = 0,

		GeneralError = 0xFFFFFFFF,
	};
};

void * KernalAlloc(uint32_t Size);
void KernalFree(void * Address);

int KernalPrintf(const char * format, ...);
int KernalPanic(KernalCode::Codes Error, const char * format, ...);

//char * KernalGetInput(char * Buffer, uint32_t Size);
//char KernalGetInput();
