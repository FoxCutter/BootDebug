#include <stdarg.h>
#include <intrin.h>
#include "CoreComplex.h"
#include "KernalLib.h"
#include "LowLevel.h"
#include "c99-snprintf-1.1\system.h"

// This defines how far a string function will go before bailing out
#define STR_MAX 1024

RawTerminal * KernalTerminal;

void * KernalAlloc(uint32_t Size)
{
	return CoreComplexObj::GetComplex()->KernalHeap.malloc(Size, false);
}

void KernalFree(void * Address)
{
	CoreComplexObj::GetComplex()->KernalHeap.free(Address);
}

void KernalSetPauseFullScreen(bool Value)
{
	KernalTerminal->SetPauseFullScreen(Value);
}


void KernalWaitATick()
{
	uint64_t StartTime = CoreComplexObj::GetComplex()->TimePassed;

	while (StartTime == CoreComplexObj::GetComplex()->TimePassed)
	{
		__halt();
	}
}

void KernalSleep(int Milliseconds)
{
	uint64_t EndTime = CoreComplexObj::GetComplex()->TimePassed + (Milliseconds << 4);

	while (CoreComplexObj::GetComplex()->TimePassed < EndTime)
	{
		__halt();
	}
}

int __cdecl KernalSprintf(char * Dest, uint32_t cCount, const char * format, ...)
{
    int retValue;
    va_list argptr;

    va_start( argptr, format );
    retValue = vsnprintf( Dest, cCount, format, argptr );
    va_end( argptr );
	
	return retValue;	
}

int __cdecl KernalPrintf(const char * format, ...)
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

int __cdecl KernalVprintf(const char * format, va_list Args)
{
    int retValue;
	char Buffer[STR_MAX];

    retValue = vsnprintf( Buffer, STR_MAX, format, Args );

	KernalTerminal->Write(Buffer, retValue);

    return retValue;
}

int __cdecl KernalPanic(KernalCode Error, const char * format, ...)
{
	KernalPrintf("\n\nKERNAL PANIC! Code: %08X\n   ", Error);
	KernalPrintf(format);

	for(;;)
	{
		ASM_HLT;
	}

	return 0;
}

void * KernalPageAllocate(uint32_t Size, KernalPageFlags Flags, char *Name)
{
	CoreComplexObj * CoreComplex = CoreComplexObj::GetComplex();
		
	uint64_t Address = CoreComplex->PageMap.AllocateRange(0x10000, Size, Name);

	return reinterpret_cast<void *>(Address);
}
