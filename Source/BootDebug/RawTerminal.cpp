#include <string.h>
#include "RawTerminal.h"
#include "LowLevel.h"

char FetchKeyboardBuffer();

RawTerminal::RawTerminal(uint32_t ScreenBufferOffest)
{
	m_Row = 25;
	m_Cols = 80;
	m_ScreenBuffer = (uint16_t *) ScreenBufferOffest;
	
	m_CurrentRow = m_CurrentCol = 0;
	m_CurrentColor = 0x0700;
	m_OutputCount = 0;
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
	m_CurrentColor = 0x0700;
	MemSet<uint16_t>(m_ScreenBuffer, m_CurrentColor, m_Row * m_Cols);
}

int RawTerminal::WriteAt(const char *szData, int cbLength, int Row, int Col)
{
	unsigned short OldColor = m_CurrentColor;
	
	//int RowStart = Row * m_Cols;
	int CurrentPos = (Row * m_Cols) + Col;

	for(int x = 0; x < cbLength; x++)
	{
		if(szData[x] != '\n')
		{
			// Write the character
			m_ScreenBuffer[CurrentPos] = (m_CurrentColor | szData[x]);

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
			MemSet<uint16_t>(BufferStart, m_CurrentColor, m_Cols);
			
			Col = 0;
			Row = m_Row - 1;

			//RowStart = Row * m_Cols;
			CurrentPos = (Row * m_Cols) + Col;
		}
	}

	m_CurrentColor = OldColor;

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

	m_OutputCount = 0;
}

int RawTerminal::Write(const char *szData, int cbLength)
{
	int CurrentPos = (m_CurrentRow * m_Cols) + m_CurrentCol;

	int ColorState = 0;

	for(int x = 0; x < cbLength; x++)
	{
		if(ColorState != 0)
		{
			short Data = szData[x];
			if(Data >= '0' && Data <= '9')
				Data = Data - '0';

			else if (Data >= 'a' && Data <= 'f')
				Data = Data - 'a' + 10;

			else if (Data >= 'A' && Data <= 'F')
				Data = Data - 'A' + 10;
			
			if(ColorState == 1 || ColorState == 4)
			{
				// Set the foreground color
				m_CurrentColor = (m_CurrentColor & 0xF000) + (Data << 8);
			}
			else if(ColorState == 2 || ColorState == 3)
			{
				// Set the background color
				m_CurrentColor = (m_CurrentColor & 0x0F00) + (Data << 12);
			}

			if(ColorState == 3)
				ColorState ++;
			else
				ColorState = 0;
		}
		else
		{
		
			if(szData[x] == 0x03)
				ColorState = 3;

			else if(szData[x] == 0x04)
				ColorState = 1;

			else if(szData[x] == 0x05)
				ColorState = 2;

			else 
			{
				if(szData[x] != '\n')
				{
					// Write the character
					m_ScreenBuffer[CurrentPos] = (m_CurrentColor | szData[x]);

					CurrentPos ++;
					m_CurrentCol ++;
				}

				if(m_CurrentCol == m_Cols || szData[x] == '\n')
				{
					m_CurrentRow ++;
					m_OutputCount ++;
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

				if(m_OutputCount == m_Row - 1)
				{
					WriteAt("...Paused...", 12, m_CurrentRow, m_CurrentCol);

					SetCursor(m_CurrentRow, 12);
					
					FetchKeyboardBuffer();

					WriteAt("            ", 12, m_CurrentRow, m_CurrentCol);

					m_OutputCount = 0;
				}
			}
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
	
	m_OutputCount = 0;

	return Pos;
}