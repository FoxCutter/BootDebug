#include "WinTerminal.h"

#include <windows.h>
#pragma comment(linker, "/defaultlib:kernel32.lib")

WinTerminal::WinTerminal(void)
{
}


WinTerminal::~WinTerminal(void)
{
	
}

int WinTerminal::Write(const char *szData, int cbLength)
{
    DWORD cbWritten;
	
	WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), szData, cbLength, &cbWritten, nullptr);

	return cbWritten;
}

int WinTerminal::Read(char *Buffer, int cbLength)
{
	DWORD ReadCount;

	ReadFile(GetStdHandle(STD_INPUT_HANDLE), Buffer, cbLength-1, & ReadCount, nullptr);
	Buffer[ReadCount] = 0;
	
	return ReadCount;
}