#include <string.h>
#include "RawTerminal.h"
#include "LowLevel.h"

RawTerminal::RawTerminal(uint32_t ScreenBufferOffest)
{
	m_Row = 25;
	m_Cols = 80;
	m_ScreenBuffer = (uint16_t *) ScreenBufferOffest;
	//m_ScreenBuffer = new uint16_t[m_Row * m_Cols];
	
	m_CurrentRow = m_CurrentCol = 0;
	
	//m_CurrentRowStart = m_CurrentRow * m_Cols;
}


RawTerminal::~RawTerminal(void)
{
}

template <typename T>
T* MemSet(T* dst, T val, size_t count)
{
    T *start = dst;

    while (count--) 
	{
		*dst = val;
		dst++;
    }	
	
	return start;
}

void RawTerminal::Clear()
{
	MemSet<uint16_t>(m_ScreenBuffer, 0x0700, m_Row * m_Cols);
}

int RawTerminal::WriteAt(const char *szData, int cbLength, int Row, int Col)
{
	//int RowStart = Row * m_Cols;
	int CurrentPos = (Row * m_Cols) + Col;

	for(int x = 0; x < cbLength; x++)
	{
		if(szData[x] != '\n')
		{
			// Write the character
			m_ScreenBuffer[CurrentPos] = (0x0700 | szData[x]);

			CurrentPos ++;
			Col ++;
		}

		if(m_CurrentCol == m_Cols || szData[x] == '\n')
		{
			Row ++;
			Col = 0;

			//RowStart = Row * m_Cols;
			CurrentPos = (Row * m_Cols) + Col;
		}

		if(Row == m_Row)
		{
			// Scroll the buffer up a line.
			memcpy(m_ScreenBuffer, m_ScreenBuffer + m_Cols, ((m_Row -1) * m_Cols) * 2);

			// Fill the new blank line
			uint16_t *BufferStart = m_ScreenBuffer + ((m_Row - 1) * m_Cols);
			//memset(m_ScreenBuffer + ((m_Row - 1) * m_Cols), 0x00, m_Cols * 2);
			MemSet<uint16_t>(BufferStart, 0x0700, m_Cols);
			
			Col = 0;
			Row = m_Row - 1;

			//RowStart = Row * m_Cols;
			CurrentPos = (Row * m_Cols) + Col;
		}
	}

	return 0;
}

void RawTerminal::SetCursor(int Row, int Col)
{
	int CurrentPos = (Row * m_Cols) + Col;
	
	// cursor LOW port to vga INDEX register
	OutPortb(0x3D4, 0x0F);
    OutPortb(0x3D5, (unsigned char)(CurrentPos & 0xFF));
    // cursor HIGH port to vga INDEX register
    OutPortb(0x3D4, 0x0E);
    OutPortb(0x3D5, (unsigned char )((CurrentPos >> 8) & 0xFF));
}

int RawTerminal::Write(const char *szData, int cbLength)
{
	int CurrentPos = (m_CurrentRow * m_Cols) + m_CurrentCol;

	for(int x = 0; x < cbLength; x++)
	{
		if(szData[x] != '\n')
		{
			// Write the character
			m_ScreenBuffer[CurrentPos] = (0x0700 | szData[x]);

			CurrentPos ++;
			m_CurrentCol ++;
		}

		if(m_CurrentCol == m_Cols || szData[x] == '\n')
		{
			m_CurrentRow ++;
			m_CurrentCol = 0;

			//m_CurrentRowStart = m_CurrentRow * m_Cols;
			CurrentPos = (m_CurrentRow * m_Cols) + m_CurrentCol;
		}

		if(m_CurrentRow == m_Row)
		{
			// Scroll the buffer up a line.
			memcpy(m_ScreenBuffer, m_ScreenBuffer + m_Cols, ((m_Row -1) * m_Cols) * 2);

			// Fill the new blank line
			uint16_t *BufferStart = m_ScreenBuffer + ((m_Row - 1) * m_Cols);
			//memset(m_ScreenBuffer + ((m_Row - 1) * m_Cols), 0x00, m_Cols * 2);
			MemSet<uint16_t>(BufferStart, 0x0700, m_Cols);
			
			m_CurrentCol = 0;
			m_CurrentRow = m_Row - 1;

			//m_CurrentRowStart = m_CurrentRow * m_Cols;
			CurrentPos = (m_CurrentRow * m_Cols) + m_CurrentCol;
		}
	}

	// cursor LOW port to vga INDEX register
	OutPortb(0x3D4, 0x0F);
    OutPortb(0x3D5, (unsigned char)(CurrentPos & 0xFF));
    // cursor HIGH port to vga INDEX register
    OutPortb(0x3D4, 0x0E);
    OutPortb(0x3D5, (unsigned char )((CurrentPos >> 8) & 0xFF));

	return 0;
}

char FetchKeyboardBuffer();

int RawTerminal::Read(char *Buffer, int cbLength)
{
	int Pos = 0;

	bool Done = false;

	do
	{
		char Next = FetchKeyboardBuffer();

		switch(Next)
		{			
			case '\t':
				// Just eat tabs
				break;

			case '\b':
				if(m_CurrentCol != 0 && Pos != 0)
				{
					m_CurrentCol --;
					WriteAt(" ", 1, m_CurrentRow, m_CurrentCol);
					SetCursor(m_CurrentRow, m_CurrentCol);

					Pos--;
					Buffer[Pos] = 0;
				}
				break;
			
			default:
				Write(&Next, 1);

				Buffer[Pos] = Next;
				Pos++;
				Buffer[Pos] = 0;
				break;
		}
		

		if(Next == 10 || Next == 13)
			break;

		if(cbLength == Pos)
			break;
	}
	while(!Done);
	
	return Pos;
}