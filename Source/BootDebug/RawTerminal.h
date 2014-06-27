#include <stdint.h>
#include "..\StdLib\Terminal.h"

#pragma once

class RawTerminal :
	public Terminal
{
public:
	RawTerminal(uint32_t ScreenBufferOffest);
	~RawTerminal(void);

	int WriteAt(const char *szData, int cbLength, int Row, int Col);
	void SetCursor(int Row, int Col);
	virtual int Write(const char *szData, int cbLength);
	virtual int Read(char *Buffer, int cbLength);

	void Clear();

private:
	
	int m_Row;
	int m_Cols;
	unsigned short *m_ScreenBuffer;

	int m_CurrentRow;
	int m_CurrentCol;
	unsigned short m_CurrentColor;

	int m_OutputCount;
};

