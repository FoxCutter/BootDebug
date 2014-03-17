#include "Terminal.h"
#pragma once

class WinTerminal :
	public Terminal
{
public:
	WinTerminal(void);
	~WinTerminal(void);

	virtual int Write(const char *szData, int cbLength);
	virtual int Read(char *Buffer, int cbLength);
};
