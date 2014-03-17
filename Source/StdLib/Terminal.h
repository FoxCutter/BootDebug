#pragma once

class Terminal
{
public:
	virtual int Write(const char *szData, int cbLength) = 0;
	virtual int Read(char *Buffer, int cbLength) = 0;
};

extern Terminal *CurrentTerminal;