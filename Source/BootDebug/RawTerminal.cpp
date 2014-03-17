#include <string.h>
#include "RawTerminal.h"
#include "LowLevel.h"

extern char KBBuffer[];
extern uint8_t KBBufferFirst;
extern uint8_t KBBufferLast;


RawTerminal::RawTerminal(uint32_t ScreenBufferOffest)
{
	m_Row = 25;
	m_Cols = 80;
	m_ScreenBuffer = (uint16_t *) ScreenBufferOffest;
	//m_ScreenBuffer = new uint16_t[m_Row * m_Cols];
	
	m_CurrentRow = m_CurrentCol = 0;
	
	m_CurrentRowStart = m_CurrentRow * m_Cols;
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

int RawTerminal::Write(const char *szData, int cbLength)
{
	int CurrentPos = m_CurrentRowStart + m_CurrentCol;

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

			m_CurrentRowStart = m_CurrentRow * m_Cols;
			CurrentPos = m_CurrentRowStart + m_CurrentCol;
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

			m_CurrentRowStart = m_CurrentRow * m_Cols;
			CurrentPos = m_CurrentRowStart + m_CurrentCol;
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

char NextChar()
{
	while(KBBufferFirst == KBBufferLast);
	
	char value = KBBuffer[KBBufferFirst];
		
	KBBufferFirst++;
	if(KBBufferFirst == 32)
		KBBufferFirst = 0;

	return value;
}

int RawTerminal::Read(char *Buffer, int cbLength)
{
	int Pos = 0;

	bool Done = false;

	do
	{
		char Next = NextChar();

		Write(&Next, 1);

		if(Next == 10 || Next == 13)
			break;

		Buffer[Pos] = Next;
		Pos++;
		Buffer[Pos] = 0;

		if(cbLength == Pos)
			break;
	}
	while(!Done);
	
	return Pos;
	
	//static int count = 0;

	//if(count == 0x10)
	//	Buffer[0] = 'q';
	//else
	//	Buffer[0] = 'd';
	//Buffer[1] = '\0';

	//count++;

	//return 1;
}