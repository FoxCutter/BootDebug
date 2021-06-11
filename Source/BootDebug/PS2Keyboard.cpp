#include "PS2Keyboard.h"
#include "Keycodes.h"
#include <intrin.h>
#include "LowLevel.h"
#include "KernalLib.h"

constexpr uint8_t ScanCodes[] =
{
	// 0x00
	KeyCode_Blank,			KeyCode_F9,				KeyCode_Blank,				KeyCode_F5,				KeyCode_F3,				KeyCode_F1,				KeyCode_F2,			KeyCode_F12,	
	KeyCode_Blank,			KeyCode_F10,			KeyCode_F8,					KeyCode_F6,				KeyCode_F4,				KeyCode_Tab,			'`',				KeyCode_Blank,	

	// 0x10
	KeyCode_Blank,			KeyCode_LeftAlt,		KeyCode_LeftShift,			KeyCode_Blank,			KeyCode_LeftCtrl,		'q',					'1',				KeyCode_Blank,
	KeyCode_Blank,			KeyCode_Blank,			'z',						's',					'a',					'w',					'2',				KeyCode_Blank,

	// 0x20
	KeyCode_Blank,			'c',					'x',						'd',					'e',					'4',					'3',				KeyCode_Blank,
	KeyCode_Blank,			' ',					'v',						'f',					't',					'r',					'5',				KeyCode_Blank,

	// 0x30
	KeyCode_Blank,			'n',					'b',						'h',					'g',					'y',					'6',				KeyCode_Blank,
	KeyCode_Blank,			KeyCode_Blank,			'm',						'j',					'u',					'7',					'8',				KeyCode_Blank,

	// 0x40
	KeyCode_Blank,			',',					'k',						'i',					'o',					'0',					'9',				KeyCode_Blank,
	KeyCode_Blank,			'.',					'/',						'l',					';',					'p',					'-',				KeyCode_Blank,

	// 0x50
	KeyCode_Blank,			KeyCode_Blank,			'\'',						KeyCode_Blank,			'[',					'=',					KeyCode_Blank,		KeyCode_Blank,
	KeyCode_CapsLock,		KeyCode_RightShift,		KeyCode_Enter,				']',					KeyCode_Blank,			'\\',					KeyCode_Blank,		KeyCode_Blank,

	// 0x60
	KeyCode_Blank,			KeyCode_Blank,			KeyCode_Blank,				KeyCode_Blank,			KeyCode_Blank,			KeyCode_Blank,			KeyCode_Backspace,	KeyCode_Blank,
	KeyCode_Blank,			KeyCode_Keypad_End,		KeyCode_Blank,				KeyCode_Keypad_Left,	KeyCode_Keypad_Home,	KeyCode_Blank,			KeyCode_Blank,		KeyCode_Blank,

	// 0x70
	KeyCode_Keypad_Insert,	KeyCode_Keypad_Delete,	KeyCode_Keypad_Down,		KeyCode_Keypad_Middle,	KeyCode_Keypad_Right,	KeyCode_Keypad_Up,		KeyCode_Escape,		KeyCode_NumberLock,
	KeyCode_F11,			KeyCode_Keypad_Plus,	KeyCode_Keypad_PageDown,	KeyCode_Keypad_Dash,	KeyCode_Keypad_Star,	KeyCode_Keypad_PageUp,	KeyCode_ScrollLock,	KeyCode_Blank,

	// 0x80
	KeyCode_Blank,			KeyCode_Blank,			KeyCode_Blank,				KeyCode_F7,				KeyCode_SysRequest,		KeyCode_Blank,			KeyCode_Blank,		KeyCode_Blank,
	KeyCode_Blank,			KeyCode_Blank,			KeyCode_Blank,				KeyCode_Blank,			KeyCode_Blank,			KeyCode_Blank,			KeyCode_Blank,		KeyCode_Blank,
};

constexpr uint8_t ScanCodesE0[] =
{
	// 0x00
	KeyCode_Blank,			KeyCode_Blank,			KeyCode_Blank,				KeyCode_Blank,			KeyCode_Blank,			KeyCode_Blank,			KeyCode_Blank, 		KeyCode_Blank,	
	KeyCode_Blank,			KeyCode_Blank,			KeyCode_Blank,				KeyCode_Blank,			KeyCode_Blank,			KeyCode_Blank,			KeyCode_Blank,		KeyCode_Blank,

	// 0x10
	KeyCode_MultiMedia,		KeyCode_RightAlt,		KeyCode_Blank,				KeyCode_Blank,			KeyCode_RightCtrl,		KeyCode_MultiMedia,		KeyCode_Blank,		KeyCode_Blank,
	KeyCode_MultiMedia,		KeyCode_Blank,			KeyCode_Blank,				KeyCode_Blank,			KeyCode_Blank,			KeyCode_Blank,			KeyCode_Blank,		KeyCode_LeftGUI,

	// 0x20
	KeyCode_MultiMedia,		KeyCode_MultiMedia,		KeyCode_Blank,				KeyCode_MultiMedia,		KeyCode_Blank,			KeyCode_Blank,			KeyCode_Blank,		KeyCode_RightGUI,
	KeyCode_MultiMedia,		KeyCode_Blank,			KeyCode_Blank,				KeyCode_MultiMedia,		KeyCode_Blank,			KeyCode_Blank,			KeyCode_Blank,		KeyCode_Apps,

	// 0x30
	KeyCode_MultiMedia,		KeyCode_Blank,			KeyCode_MultiMedia,			KeyCode_Blank,			KeyCode_MultiMedia,		KeyCode_Blank,			KeyCode_Blank,		KeyCode_Power,
	KeyCode_MultiMedia,		KeyCode_Blank,			KeyCode_MultiMedia,			KeyCode_MultiMedia,		KeyCode_Blank,			KeyCode_Blank,			KeyCode_Blank,		KeyCode_Sleep,

	// 0x40
	KeyCode_MultiMedia,		KeyCode_Blank,			KeyCode_Blank,				KeyCode_Blank,			KeyCode_Blank,			KeyCode_Blank,			KeyCode_Blank,		KeyCode_Blank,
	KeyCode_MultiMedia,		KeyCode_Blank,			KeyCode_Keypad_Slash,		KeyCode_Blank,			KeyCode_Blank,			KeyCode_MultiMedia,		KeyCode_Blank,		KeyCode_Blank,

	// 0x50
	KeyCode_MultiMedia,		KeyCode_Blank,			KeyCode_Blank,				KeyCode_Blank,			KeyCode_Blank,			KeyCode_Blank,			KeyCode_Blank,		KeyCode_Blank,	
	KeyCode_Blank,			KeyCode_Blank,			KeyCode_Keypad_Enter,		KeyCode_Blank,			KeyCode_Blank,			KeyCode_Blank,			KeyCode_Wake,		KeyCode_Blank,

	// 0x60
	KeyCode_Blank,			KeyCode_Blank,			KeyCode_Blank,				KeyCode_Blank,			KeyCode_Blank,			KeyCode_Blank,			KeyCode_Blank,		KeyCode_Blank,
	KeyCode_Blank,			KeyCode_End,			KeyCode_Blank,				KeyCode_Left,			KeyCode_Home,			KeyCode_Blank,			KeyCode_Blank,		KeyCode_Blank,

	// 0x70
	KeyCode_Insert,			KeyCode_Delete,			KeyCode_Down,				KeyCode_Blank,			KeyCode_Right,			KeyCode_Up,				KeyCode_Blank,		KeyCode_Pause,
	KeyCode_Blank,			KeyCode_Blank,			KeyCode_PageDown,			KeyCode_Blank,			KeyCode_PrintScreen,	KeyCode_PageUp,			KeyCode_Break,		KeyCode_Blank,

	// 0x80
	KeyCode_Blank,			KeyCode_Blank,			KeyCode_Blank,				KeyCode_Blank,			KeyCode_Blank,			KeyCode_Blank,			KeyCode_Blank,		KeyCode_Blank,	
	KeyCode_Blank,			KeyCode_Blank,			KeyCode_Blank,				KeyCode_Blank,			KeyCode_Blank,			KeyCode_Blank,			KeyCode_Blank,		KeyCode_Blank,
};

// Maps a Keycode to a shifted Keycode
constexpr uint8_t ShiftMap[]
{
	KeyCode_Blank,	KeyCode_Blank,	KeyCode_Blank,	KeyCode_Blank,	KeyCode_Blank,	KeyCode_Blank,	KeyCode_Blank,	KeyCode_Blank,	// 0x00
	KeyCode_Blank,	KeyCode_Blank,	KeyCode_Blank,	KeyCode_Blank,	KeyCode_Blank,	KeyCode_Blank,	KeyCode_Blank,	KeyCode_Blank,	// 0x08
	KeyCode_Blank,	KeyCode_Blank,	KeyCode_Blank,	KeyCode_Blank,	KeyCode_Blank,	KeyCode_Blank,	KeyCode_Blank,	KeyCode_Blank,	// 0x10
	KeyCode_Blank,	KeyCode_Blank,	KeyCode_Blank,	KeyCode_Blank,	KeyCode_Blank,	KeyCode_Blank,	KeyCode_Blank,	KeyCode_Blank,	// 0x18
	KeyCode_Blank,	KeyCode_Blank,	KeyCode_Blank,	KeyCode_Blank,	KeyCode_Blank,	KeyCode_Blank,	KeyCode_Blank,	'\"',			// 0x20
	KeyCode_Blank,	KeyCode_Blank,	KeyCode_Blank,	KeyCode_Blank,	'<',			KeyCode_Blank,	'>',			'?',			// 0x28
	')',			'!',			'@',			'#',			'$',			'%',			'^',			'&',			// 0x30
	'*',			'(',			KeyCode_Blank,	':',			'+',			KeyCode_Blank,	KeyCode_Blank,	KeyCode_Blank,	// 0x38
	KeyCode_Blank,	'a',			'b',			'c',			'd',			'e',			'f',			'g',			// 0x40
	'h',			'i',			'j',			'k',			'l',			'm',			'n',			'o',			// 0x48
	'p',			'q',			'r',			's',			't',			'u',			'v',			'w',			// 0x50
	'x',			'y',			'z',			'{',			'|',			'}',			KeyCode_Blank,	KeyCode_Blank,	// 0x58
	KeyCode_Blank,	'A',			'B',			'C',			'D',			'E',			'F',			'G',			// 0x60
	'H',			'I',			'J',			'K',			'L',			'M',			'N',			'O',			// 0x68
	'P',			'Q',			'R',			'S',			'T',			'U',			'V',			'W',			// 0x70
	'X',			'Y',			'Z',			KeyCode_Blank,	KeyCode_Blank,	KeyCode_Blank,	'~',			 KeyCode_Blank,	// 0x7F
};

constexpr uint8_t KeypadMap[]
{
	// New Key Code				New Charater Value
	// Num Lock Off 
	KeyCode_Keypad_Insert,		KeyCode_Insert,
	KeyCode_Keypad_End,			KeyCode_End,
	KeyCode_Keypad_Down,		KeyCode_Down,
	KeyCode_Keypad_PageDown,	KeyCode_PageDown,
	KeyCode_Keypad_Left,		KeyCode_Left,
	KeyCode_Keypad_Middle,		KeyCode_Blank,
	KeyCode_Keypad_Right,		KeyCode_Right,
	KeyCode_Keypad_Home,		KeyCode_Home,
	KeyCode_Keypad_Up,			KeyCode_Up,
	KeyCode_Keypad_PageUp,		KeyCode_PageUp,
	KeyCode_Keypad_Delete,		KeyCode_Delete,
	KeyCode_Keypad_Slash,		'/',
	KeyCode_Keypad_Star,		'*',
	KeyCode_Keypad_Dash,		'-',
	KeyCode_Keypad_Plus,		'+',
	KeyCode_Keypad_Enter,		KeyCode_Enter,

	// Num Lock On
	KeyCode_Keypad_0,			'0',
	KeyCode_Keypad_1,			'1',
	KeyCode_Keypad_2,			'2',
	KeyCode_Keypad_3,			'3',
	KeyCode_Keypad_4,			'4',
	KeyCode_Keypad_5,			'5',
	KeyCode_Keypad_6,			'6',
	KeyCode_Keypad_7,			'7',
	KeyCode_Keypad_8,			'8',
	KeyCode_Keypad_9,			'9',
	KeyCode_Keypad_Period,		'.',
	KeyCode_Keypad_Slash,		'/',
	KeyCode_Keypad_Star,		'*',
	KeyCode_Keypad_Dash,		'-',
	KeyCode_Keypad_Plus,		'+',
	KeyCode_Keypad_Enter,		KeyCode_Enter,

};

enum PS2Commands : unsigned char
{
	ReadByte = 0x20,
	WriteByte = 0x60,			// Plus Data

	ReadConfiguration = 0x20,
	WriteConfiguration = 0x60,  // Plus Data

	SelfTest = 0xAA,
	SelfTestPassed = 0x55,
	SelfTestFailed = 0xFC,

	DiagnosticDump = 0xAC,

	DisableFirstPort = 0xAD,
	EnableFirstPort = 0xAE,
	TestFirstPort = 0xAB,
	WriteFirstPortOutputBuffer = 0xD2,	// Plus Data

	DisableSecondPort = 0xA7,
	EnableSecondPort = 0xA8,
	TestSecondPort = 0xA9,
	WriteSecondPortOutputBuffer = 0xD3,	// Plus Data
	WriteSecondPortInputBuffer = 0xD4,	// Plus Data

	ReadControllerOutputPort = 0xD0,
	WriteControllerOutputPort = 0xD1,	// Plus Data

	PulseOutputPort = 0xF0,

	// Keyboard Commands
	KeyboardSetLED = 0xED,
	KeyboardEcho = 0xEE,
	KeyboardScanCode = 0xF0,
	KeyboardIdentify = 0xF2,
	KeyboardSetTypematic = 0xF3,
	KeyboardEnableScanning = 0xF4,
	KeyboardDisableScanning = 0xF5,
	KeyboardResetToDefault = 0xF6,
	KeyboardRestarteAndSelftest = 0xFF,

	KeyboardError = 0x00,
	KeyboardSelfTestPassed = 0xAA,
	KeboardAck = 0xFA,
	KeyboardSelfTestFailed1 = 0xFC,
	KeyboardSelfTestFailed2 = 0xFD,
	KeyboardResendLastByte = 0xFE,
	KeyboardError2 = 0xFF,

};

enum PS2Bits : unsigned char
{
	// Status Register
	OutputBufferFull = 0x01,
	InputBufferFull = 0x02,
	SystemFlag = 0x04,
	ControllerCommand = 0x08,


	TimeoutError = 0x40,
	ParityError = 0x80,

	// Controller Configuration 
	EnableFirstPortInterrupt = 0x01,
	EnableSecondPortInterrupt = 0x02,
	//SystemFlag				= 0x04,

	DisableFirstPortClock = 0x10,
	DisableSecondPortClock = 0x20,
	FirstPortTranslation = 0x40,

	// Controller Output Port 
	SystemResetLine = 0x01,
	A20Gate = 0x02,
	SecondPortClock = 0x04,
	SecondPortData = 0x08,
	OutputBufferFullFromFirstPort = 0x10,
	OutputBufferFullFromSecondPort = 0x20,
	FirstPortClock = 0x40,
	FirstPortData = 0x80,
}; 

uint16_t KeyboardState;

unsigned char GetData()
{
	return __inbyte(0x60);
}

unsigned char GetStatus()
{
	return __inbyte(0x64);
}

unsigned char ReadData()
{
	while ((GetStatus() & OutputBufferFull) == 0)
		;

	return GetData();
}

void WriteData(unsigned char Data)
{
	__outbyte(0x60, Data);
}

void WriteCommand(unsigned char Command)
{
	__outbyte(0x64, Command);
}

unsigned char ReadCommand(unsigned char Command)
{
	WriteCommand(Command);
	
	return ReadData();
}

void WriteCommand(unsigned char Command, unsigned char Data)
{
	WriteCommand(Command);
	WriteData(Data);
}

void SetupPS2Keyboard()
{
	// Disable everything
	WriteCommand(DisableFirstPort);
	WriteCommand(DisableSecondPort);

	// Clear anything pending
	unsigned char Temp = GetStatus();
	if ((Temp & OutputBufferFull) == OutputBufferFull)
		GetData();

	// Get the configuration byte
	unsigned char config = ReadCommand(ReadConfiguration);
	//KernalPrintf(" KB config: %02X ", config);

	// Self Test
	Temp = ReadCommand(SelfTest);
	//KernalPrintf(" KB Code: %02X\n", Temp);

	// Turn on Primary Interrupt 
	config |= EnableFirstPortInterrupt;
	// Disable Secondary Interrupt 
	config &= ~EnableSecondPortInterrupt;

	// Disable Translation
	config &= ~FirstPortTranslation;

	WriteCommand(WriteConfiguration, config);

	// Clear the Keystate
	KeyboardState = 0;

	// Enable
	WriteCommand(EnableFirstPort);
}

unsigned char TranslateKeyCode(uint8_t &KeyCode, uint16_t KeyState)
{
	unsigned char Value = KeyCode;

	if ((KeyState & KeystateCapsLock) != 0 && (KeyCode >= KeyCode_a && KeyCode <= KeyCode_z))
	{
		unsigned char NewValue = ShiftMap[KeyCode];
		if (NewValue != KeyCode_Blank)
		{
			Value = NewValue;
			KeyCode = NewValue;
		}
	}
	
	if ((KeyState & KeystateShift) != 0)
	{
		unsigned char NewValue = ShiftMap[KeyCode];
		if (NewValue != KeyCode_Blank)
		{
			Value = NewValue;
			KeyCode = NewValue;
		}
	}

	if (KeyCode >= KeyCode_Keypad_Insert && KeyCode <= KeyCode_Keypad_Enter)
	{
		int Index = 0;
		if ((KeyState & KeystateNumLock) == KeystateNumLock || ((KeyState & KeystateShift) != 0))
		{
			Index = 16 * 2;
		}

		Index += (KeyCode - KeyCode_Keypad_Insert) * 2;

		KeyCode = KeypadMap[Index];
		Value = KeypadMap[Index + 1];
	}

	return Value;
}

void UpdateKeyboardState(Keystates State, bool Break)
{
	if (!Break)
		KeyboardState |= State;
	else
		KeyboardState &= ~State;
}

extern void InsertKeyboardBuffer(uint8_t KeyCode, uint16_t KeyState, unsigned char Key);
extern void SystemReset();

void KeyboardInterrupt(InterruptContext * OldContext, uintptr_t * Data)
{
	if ((GetStatus() & OutputBufferFull) == 0)
		return;

	bool Break = false;
	bool E0 = false;
	unsigned char ScanCode = ReadData();
	uint8_t KeyCode = KeyCode_Blank;

	if ((KeyboardState & KeystateEchoLock) == KeystateEchoLock)
		KernalPrintf("[%02X", ScanCode);

	if (ScanCode == 0xE0)
	{
		E0 = true;
		ScanCode = ReadData();

		if ((KeyboardState & KeystateEchoLock) == KeystateEchoLock)
			KernalPrintf(" %02X", ScanCode);
	}

	if(ScanCode == 0xF0)
	{
		Break = true;
		ScanCode = ReadData();

		if ((KeyboardState & KeystateEchoLock) == KeystateEchoLock)
			KernalPrintf(" %02X", ScanCode);
	}	
	
	if (ScanCode == 0xE1)
	{
		unsigned char ScanCode2 = ReadData();

		if ((KeyboardState & KeystateEchoLock) == KeystateEchoLock)
			KernalPrintf(" %02X", ScanCode2);
	}

	if ((KeyboardState & KeystateEchoLock) == KeystateEchoLock)
		KernalPrintf("] ");

	// Get the keycode
	if(!E0)
		KeyCode = ScanCodes[ScanCode];
	else 
		KeyCode = ScanCodesE0[ScanCode];

	// Handle anything that updates the keyboard state
	switch (KeyCode)
	{
		case KeyCode_LeftShift:
			UpdateKeyboardState(KeystateLeftShift, Break);
			KeyCode = KeyCode_Blank;
			break;

		case KeyCode_RightShift:
			UpdateKeyboardState(KeystateRightShift, Break);
			KeyCode = KeyCode_Blank;
			break;

		case KeyCode_LeftCtrl:
			UpdateKeyboardState(KeystateLeftCtrl, Break);
			KeyCode = KeyCode_Blank;
			break;

		case KeyCode_RightCtrl:
			UpdateKeyboardState(KeystateRightCtrl, Break);
			KeyCode = KeyCode_Blank;
			break;

		case KeyCode_LeftAlt:
			UpdateKeyboardState(KeystateLeftAlt, Break);
			KeyCode = KeyCode_Blank;
			break;

		case KeyCode_RightAlt:
			UpdateKeyboardState(KeystateRightAlt, Break);
			KeyCode = KeyCode_Blank;
			break;

		case KeyCode_LeftGUI:
			UpdateKeyboardState(KeystateLeftGUI, Break);
			KeyCode = KeyCode_Blank;
			break;

		case KeyCode_RightGUI:
			UpdateKeyboardState(KeystateRightGUI, Break);
			KeyCode = KeyCode_Blank;
			break;

		// CTRL-ALT-Backspace
		case KeyCode_Backspace:
			if ((KeyboardState & KeystateCtrl) != 0 && (KeyboardState & KeystateAlt) != 0)
			{
				if(!Break)
					KeyboardState ^= KeystateEchoLock;

				KeyCode = KeyCode_Blank;
			}
			break;

		// CTRL-ALT-Delete
		case KeyCode_Delete:
			if ((KeyboardState & KeystateCtrl) != 0 && (KeyboardState & KeystateAlt) != 0)
			{
				KernalSetPauseFullScreen(false);
				KernalPrintf("\nRebooting...\n");
				SystemReset();
			}
			break;

		case KeyCode_NumberLock:
			if(!Break)
				KeyboardState ^= KeystateNumLock;
			KeyCode = KeyCode_Blank;
			break;

		case KeyCode_CapsLock:
			if (!Break)
				KeyboardState ^= KeystateCapsLock;
			KeyCode = KeyCode_Blank;
			break;

		case KeyCode_ScrollLock:
			if (!Break)
				KeyboardState ^= KeystateScrollLock;
			KeyCode = KeyCode_Blank;
			break;

		case KeyCode_SysRequest:
			if (!Break)
			{
				KernalPrintf("\nSystem Request...\n");
				return;
			}
			break;

	}

	if (KeyCode == KeyCode_Blank)
		return;
	
	unsigned char InputValue = TranslateKeyCode(KeyCode, KeyboardState);

	if ((KeyboardState & KeystateEchoLock) == KeystateEchoLock)
		KernalPrintf("{%2.2X}", InputValue);

	if(!Break)
		InsertKeyboardBuffer(KeyCode, KeyboardState, InputValue);

}