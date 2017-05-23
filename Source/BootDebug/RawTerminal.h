#include <stdint.h>
#include "..\StdLib\Terminal.h"

#pragma once

class RawTerminal :
	public Terminal
{
public:
	RawTerminal(uint32_t ScreenBufferOffest, uint32_t Width, uint32_t Height, uint32_t Pitch);
	~RawTerminal(void);

	int WriteAt(const char *szData, int cbLength, int Row, int Col);
	void SetCursor(int Row, int Col);
	virtual int Write(const char *szData, int cbLength);
	virtual int Read(char *Buffer, int cbLength);

	void Clear();

	void SetPauseFullScreen(bool Value) { m_PauseFullScreen = Value; }
	void TogglePauseFullScreen() { m_PauseFullScreen = !m_PauseFullScreen; }
private:
	
	int m_Row;
	int m_Cols;
	unsigned short *m_ScreenBuffer;

	int m_CurrentRow;
	int m_CurrentCol;
	unsigned short m_CurrentColor;

	int m_OutputCount;

	bool m_PauseFullScreen;
};

