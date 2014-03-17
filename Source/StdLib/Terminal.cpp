#include "Terminal.h"

#include <windows.h>
#pragma comment(linker, "/defaultlib:kernel32.lib")

// This file should have the majority of OS dependent code in it.
//
//int Write(const char *szData, int cbLength)
//{
//    DWORD cbWritten;
//	
//	WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), szData, cbLength, &cbWritten, nullptr);
//
//	return cbWritten;
//}
//
//int Read(char *Buffer, int cbLength)
//{
//	DWORD ReadCount;
//
//	ReadFile(GetStdHandle(STD_INPUT_HANDLE), Buffer, cbLength, & ReadCount, nullptr);
//	
//	return ReadCount;
//}