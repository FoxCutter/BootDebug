#include <stdint.h>
#include "..\StdLib\Terminal.h"

#pragma once

class RawTerminal :
	public Terminal
{
public:
	RawTerminal(uint32_t ScreenBufferOffest);
	~RawTerminal(void);

	virtual int Write(const char *szData, int cbLength);
	virtual int Read(char *Buffer, int cbLength);

private:
	
	int m_Row;
	int m_Cols;
	unsigned short *m_ScreenBuffer;

	int m_CurrentRow;
	int m_CurrentCol;

	int m_CurrentRowStart;
};

