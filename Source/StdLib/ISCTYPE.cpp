//==========================================
// LIBCTINY - Matt Pietrek 2001
// MSDN Magazine, January 2001
//==========================================
#include <ctype.h>

const unsigned short pctype[256] = {
        _CONTROL,              
        _CONTROL,              
        _CONTROL,              
        _CONTROL,              
        _CONTROL,              
        _CONTROL,              
        _CONTROL,              
        _CONTROL,              
        _CONTROL,              
        _SPACE | _CONTROL,     
        _SPACE | _CONTROL,     
        _SPACE | _CONTROL,     
        _SPACE | _CONTROL,     
        _SPACE | _CONTROL,     
        _CONTROL,              
        _CONTROL,              
        _CONTROL,              
        _CONTROL,              
        _CONTROL,              
        _CONTROL,              
        _CONTROL,              
        _CONTROL,              
        _CONTROL,              
        _CONTROL,              
        _CONTROL,              
        _CONTROL,              
        _CONTROL,              
        _CONTROL,              
        _CONTROL,              
        _CONTROL,              
        _CONTROL,              
        _CONTROL,              
        _SPACE | _BLANK,       
        _PUNCT,                
        _PUNCT,                
        _PUNCT,                
        _PUNCT,                
        _PUNCT,                
        _PUNCT,                
        _PUNCT,                
        _PUNCT,                
        _PUNCT,                
        _PUNCT,                
        _PUNCT,                
        _PUNCT,                
        _PUNCT,                
        _PUNCT,                
        _PUNCT,                
        _DIGIT | _HEX,				// 0
        _DIGIT | _HEX,            
        _DIGIT | _HEX,            
        _DIGIT | _HEX,            
        _DIGIT | _HEX,            
        _DIGIT | _HEX,            
        _DIGIT | _HEX,            
        _DIGIT | _HEX,            
        _DIGIT | _HEX,            
        _DIGIT | _HEX,				// 9
        _PUNCT,                
        _PUNCT,                
        _PUNCT,                
        _PUNCT,                
        _PUNCT,                
        _PUNCT,                
        _PUNCT,                
        _UPPER | _HEX,				// A
        _UPPER | _HEX,  
        _UPPER | _HEX,  
        _UPPER | _HEX,  
        _UPPER | _HEX,  
        _UPPER | _HEX,  
        _UPPER,         
        _UPPER,         
        _UPPER,         
        _UPPER,         
        _UPPER,         
        _UPPER,         
        _UPPER,         
        _UPPER,         
        _UPPER,         
        _UPPER,         
        _UPPER,         
        _UPPER,         
        _UPPER,         
        _UPPER,         
        _UPPER,         
        _UPPER,         
        _UPPER,         
        _UPPER,         
        _UPPER,         
        _UPPER,						// Z
        _PUNCT,                
        _PUNCT,                
        _PUNCT,                
        _PUNCT,                
        _PUNCT,                
        _PUNCT,                
        _LOWER | _HEX,				// a 
        _LOWER | _HEX,         
        _LOWER | _HEX,         
        _LOWER | _HEX,         
        _LOWER | _HEX,         
        _LOWER | _HEX,         
        _LOWER,                
        _LOWER,                
        _LOWER,                
        _LOWER,                
        _LOWER,                
        _LOWER,                
        _LOWER,                
        _LOWER,                
        _LOWER,                
        _LOWER,                
        _LOWER,                
        _LOWER,                
        _LOWER,                
        _LOWER,                
        _LOWER,                
        _LOWER,                
        _LOWER,                
        _LOWER,                
        _LOWER,                
        _LOWER,						// z
        _PUNCT,                
        _PUNCT,                
        _PUNCT,                
        _PUNCT,                
        _CONTROL,              
};

extern "C" int __cdecl _isctype ( int c , int mask )
{
    /* c valid between -1 and 255 */
    if (((unsigned)(c + 1)) <= 256)
    {
        return ( pctype[c] & mask ) ;
    }
    else
        return 0;
}

extern "C" int __cdecl toupper ( int c )
{
	if ( (c >= 'a') && (c <= 'z') )
		c = c - ('a' - 'A');

	return c;
}

extern "C" int __cdecl tolower ( int c )
{
    if ( (c >= 'A') && (c <= 'Z') )
        c = c + ('a' - 'A');

	return c;
}

extern "C" int __cdecl isalpha ( int c )
{
	return _isctype(c, _ALPHA);
}

extern "C" int __cdecl isupper ( int c )
{
	return _isctype(c, _UPPER);
}

extern "C" int __cdecl islower ( int c )
{
	return _isctype(c, _LOWER);
}

extern "C" int __cdecl isdigit ( int c )
{
	return _isctype(c, _DIGIT);
}

extern "C" int __cdecl isxdigit ( int c )
{
	return _isctype(c, _HEX);
}

extern "C" int __cdecl isspace ( int c )
{
	return _isctype(c, _SPACE);
}

extern "C" int __cdecl ispunct ( int c )
{
	return _isctype(c, _PUNCT);
}

extern "C" int __cdecl isalnum ( int c )
{
	return _isctype(c, _ALPHA | _DIGIT);
}

extern "C" int __cdecl isprint ( int c )
{
	return _isctype(c, _BLANK | _PUNCT| _ALPHA| _DIGIT);
}

extern "C" int __cdecl isgraph ( int c )
{
	return _isctype(c, _PUNCT| _ALPHA| _DIGIT);
}

extern "C" int __cdecl iscntrl ( int c )
{
	return _isctype(c, _CONTROL);
}
