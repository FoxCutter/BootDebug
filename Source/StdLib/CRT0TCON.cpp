//==========================================
// LIBCTINY - Matt Pietrek 2001
// MSDN Magazine, January 2001
// FILE: CRT0TCON.CPP
//==========================================
#include <windows.h>
#include <errno.h>
#include "argcargv.h"
#include "initterm.h"

#include "WinTerminal.h"
#include "WinMemory.h"

// Force the linker to include KERNEL32.LIB
#pragma comment(linker, "/defaultlib:kernel32.lib")
#pragma comment(linker, "/nodefaultlib")

extern "C" int * __cdecl _errno(void)
{
	static int Error;
	
	return &Error;
}

Terminal *CurrentTerminal;
MemoryManager *MemoryMgr;

extern "C" int __cdecl main(int, char **, char **);    // In user's code

//
// Modified version of the Visual C++ startup code.  Simplified to
// make it easier to read.  Only supports ANSI programs.
//
extern "C" void __cdecl mainCRTStartup( void )
{
    WinTerminal DefaultTerm;
	CurrentTerminal = &DefaultTerm;

	WinMemory DefaultMemoryMgr;
	MemoryMgr = &DefaultMemoryMgr;
	
	errno = 0;
	
	int mainret, argc;
	char * argv[129];

    argc = _ConvertCommandLineToArgcArgv( GetCommandLine(), argv, 128 );

    // set up our minimal cheezy atexit table
    _atexit_init();

    // Call C++ constructors
    _initterm();

    mainret = main( argc, argv, 0 );

    _DoExit();

    ExitProcess(mainret);
}
