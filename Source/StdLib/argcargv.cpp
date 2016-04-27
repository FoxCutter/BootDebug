//==========================================
// LIBCTINY - Matt Pietrek 2001
// MSDN Magazine, January 2001
//==========================================
#include <string.h>
#include <stdlib.h>
#include "argcargv.h"

#define _MAX_CMD_LINE_ARGS  128

//char * _ppszArgv[_MAX_CMD_LINE_ARGS + 1];

int __cdecl _ConvertCommandLineToArgcArgv( char * pszSysCmdLine, char * _ppszArgv[], int CmdLineMax, bool KeepQuotes )
{
    int cbCmdLine = 0;
    int argc;
    char * pszCmdLine;
    
    // Set to no argv elements, in case we have to bail out
    _ppszArgv[0] = 0;

    // First get a pointer to the system's version of the command line, and
    // figure out how long it is.
    //pszSysCmdLine = GetCommandLine();
    cbCmdLine = strlen(pszSysCmdLine);

    // Allocate memory to store a copy of the command line.  We'll modify
    // this copy, rather than the original command line.  Yes, this memory
    // currently doesn't explicitly get freed, but it goes away when the
    // process terminates.
    pszCmdLine = pszSysCmdLine; 
	//(pszCmdLine = char *)malloc( cbCmdLine+1 );
    if ( !pszCmdLine )
        return 0;

    // Copy the system version of the command line into our copy
    //strcpy( pszCmdLine, pszSysCmdLine );

    if ( '"' == *pszCmdLine )   // If command line starts with a quote ("),
    {                           // it's a quoted filename.  Skip to next quote.
		if(!KeepQuotes)
			pszCmdLine++;

        _ppszArgv[0] = pszCmdLine;  // argv[0] == executable name
        
		if(KeepQuotes)
			pszCmdLine++;
    
        while ( *pszCmdLine && (*pszCmdLine != '"') )
            pszCmdLine++;

        if ( *pszCmdLine )      // Did we see a non-NULL ending?
		{
            if(KeepQuotes)
				*++pszCmdLine = 0;  // Null terminate and advance to next char
			else
				*pszCmdLine++ = 0;  // Null terminate and advance to next char
		}
        else
            return 0;           // Oops!  We didn't see the end quote
    }
    else    // A regular (non-quoted) filename
    {
        _ppszArgv[0] = pszCmdLine;  // argv[0] == executable name

        while ( *pszCmdLine && (' ' != *pszCmdLine) && ('\t' != *pszCmdLine) )
            pszCmdLine++;

        if ( *pszCmdLine )
            *pszCmdLine++ = 0;  // Null terminate and advance to next char
    }

    // Done processing argv[0] (i.e., the executable name).  Now do th
    // actual arguments

    argc = 1;

    while ( 1 )
    {
        // Skip over any whitespace
        while ( *pszCmdLine && (' ' == *pszCmdLine) || ('\t' == *pszCmdLine) )
            pszCmdLine++;

        if ( 0 == *pszCmdLine ) // End of command line???
            return argc;

        if ( '"' == *pszCmdLine )   // Argument starting with a quote???
        {
			if(!KeepQuotes)
				pszCmdLine++;   // Advance past quote character

            _ppszArgv[ argc++ ] = pszCmdLine;
            _ppszArgv[ argc ] = 0;

			if(KeepQuotes)
				pszCmdLine++;   // Advance past quote character

			// Scan to end quote, or NULL terminator
            while ( *pszCmdLine && (*pszCmdLine != '"') )
                pszCmdLine++;
                
            if ( 0 == *pszCmdLine )
                return argc;
            
            if ( *pszCmdLine )
			{
				if(KeepQuotes)
					*++pszCmdLine = 0;  // Null terminate and advance to next char
				else
					*pszCmdLine++ = 0;  // Null terminate and advance to next char
			}
        }
        else                        // Non-quoted argument
        {
            _ppszArgv[ argc++ ] = pszCmdLine;
            _ppszArgv[ argc ] = 0;

            // Skip till whitespace or NULL terminator
            while ( *pszCmdLine && (' '!=*pszCmdLine) && ('\t'!=*pszCmdLine) )
                pszCmdLine++;
            
            if ( 0 == *pszCmdLine )
                return argc;
            
			*pszCmdLine++ = 0;  // Null terminate and advance to next char
        }

        if ( argc >= (CmdLineMax) )
            return argc;
    }
}
