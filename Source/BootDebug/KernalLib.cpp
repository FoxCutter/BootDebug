#include <stdarg.h>
#include "KernalLib.h"
#include "LowLevel.h"
#include "c99-snprintf-1.1\system.h"

// This defines how far a string function will go before bailing out
#define STR_MAX 1024

RawTerminal * KernalTerminal;

void * KernalAlloc(uint32_t Size)
{
	return nullptr;
}

void KernalFree(void * Address)
{
	return;
}

int KernalPrintf(const char * format, ...)
{
    int retValue;
	char Buffer[STR_MAX];
    va_list argptr;
          
    va_start( argptr, format );
    retValue = vsnprintf( Buffer, STR_MAX, format, argptr );
    va_end( argptr );

	KernalTerminal->Write(Buffer, retValue);

    return retValue;
}

int KernalPanic(KernalCode::Codes Error, const char * format, ...)
{
	KernalPrintf("\n\nKERNAL PANIC! Code: %08X\n   ", Error);
	KernalPrintf(format);

	for(;;)
	{
		ASM_HLT;
	}

	return 0;
}
