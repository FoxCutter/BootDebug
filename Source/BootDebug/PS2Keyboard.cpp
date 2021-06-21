#include "PS2Keyboard.h"
#include <intrin.h>
#include "KernalLib.h"
#include "Keycodes.h"
#include "PS2ScanCodes.h"

struct KeyCodeTranslationEntry
{
	uint8_t Value;
	uint8_t Shift;
};

const KeyCodeTranslationEntry KeyCodeTranslationTable[] =
{
	// 0x00
	{0x00},				{0x00},				{0x00},				{0x00},				{0x00},				{0x00},				{0x00},				{0x00},
	{'\b'},				{'\t'},				{'\n'},				{0x00},				{0x00},				{0x00},				{0x00},				{0x00},

	// 0x10
	{0x00},				{0x00},				{0x00},				{0x00},				{0x00},				{0x00},				{0x00},				{0x00},
	{0x00},				{0x00},				{0x00},				{'\x1B'},			{0x00},				{0x00},				{0x00},				{0x00},

	// 0x20
	{' '},				{'!'},				{'"'},				{'#'},				{'$'},				{'%'},				{'&'},				{'\'', '\"'},
	{'('},				{')'},				{'*'},				{'+'},				{',', '<'},			{'-'},				{'.', '>'},			{'/', '?'},

	// 0x30
	{'0', ')'},			{'1', '!'},			{'2', '@'},			{'3', '#'},			{'4', '$'},			{'5', '%'},			{'6', '^'},			{'7', '&'},
	{'8', '*'},			{'9', '('},			{':'},				{';', ':'},			{'<'},				{'=', '+'},			{'>'},				{'?'},

	// 0x40
	{'@'},				{'A'},				{'B'},				{'C'},				{'D'},				{'E'},				{'F'},				{'G'},
	{'H'},				{'I'},				{'J'},				{'K'},				{'L'},				{'M'},				{'N'},				{'O'},

	// 0x50
	{'P'},				{'Q'},				{'R'},				{'S'},				{'T'},				{'U'},				{'V'},				{'W'},
	{'X'},				{'Y'},				{'Z'},				{'[', '{'},			{'\\', '|'},		{']', '}'},			{'^'},				{'_'},

	// 0x60
	{'`', '~'},			{'a', 'A'},			{'b', 'B'},			{'c', 'C'},			{'d', 'D'},			{'e', 'E'},			{'f', 'F'},			{'g', 'G'},
	{'h', 'H'},			{'i', 'I'},			{'j', 'J'},			{'k', 'K'},			{'l', 'L'},			{'m', 'M'},			{'n', 'N'},			{'o', 'O'},

	// 0x70
	{'p', 'P'},			{'q', 'Q'},			{'r', 'R'},			{'s', 'S'},			{'t', 'T'},			{'u', 'U'},			{'v', 'V'},			{'w', 'W'},
	{'x', 'X'},			{'y', 'Y'},			{'z', 'Z'},			{'{'},				{'|'},				{'}'},				{'~'},				{'\x7F'},

	// 0x80
	{0x00},				{0x00},				{0x00},				{0x00},				{0x00},				{0x00},				{0x00},				{0x00},
	{0x00},				{0x00},				{0x00},				{0x00},				{0x00},				{0x00},				{0x00},				{0x00},

	// 0x90
	{0x00},				{0x00},				{0x00},				{0x00},				{0x00},				{0x00},				{0x00},				{0x00},
	{0x00},				{0x00},				{0x00},				{0x00},				{0x00},				{0x00},				{0x00},				{0x00},

	// 0xA0
	{0x00},				{0x00},				{0x00},				{0x00},				{0x00},				{0x00},				{0x00},				{0x00},
	{0x00},				{0x00},				{0x00},				{0x00},				{0x00},				{0x00},				{0x00},				{0x00},

	// 0xB0
	{(uint8_t)KeyCodes::KeyCode_Insert},	{(uint8_t)KeyCodes::KeyCode_End},		{(uint8_t)KeyCodes::KeyCode_Down},		{(uint8_t)KeyCodes::KeyCode_PageUp},	
	{(uint8_t)KeyCodes::KeyCode_Left},		{0x00},									{(uint8_t)KeyCodes::KeyCode_Right},		{(uint8_t)KeyCodes::KeyCode_Home},
	{(uint8_t)KeyCodes::KeyCode_Up},		{(uint8_t)KeyCodes::KeyCode_PageUp},	{(uint8_t)KeyCodes::KeyCode_Delete},	{'/'},				
	{'*'},									{'-'},									{'+'},									{'\n'},

	// 0xC0
	{'0'},				{'1'},				{'2'},				{'3'},				{'4'},				{'5'},				{'6'},				{'7'},
	{'8'},				{'9'},				{'.'},				{0x00},				{0x00},				{0x00},				{0x00},				{0x00},

	// 0xD0
	{0x00},				{0x00},				{0x00},				{0x00},				{0x00},				{0x00},				{0x00},				{0x00},
	{0x00},				{0x00},				{0x00},				{0x00},				{0x00},				{0x00},				{0x00},				{0x00},

	// 0xE0
	{0x00},				{0x00},				{0x00},				{0x00},				{0x00},				{0x00},				{0x00},				{0x00},
	{0x00},				{0x00},				{0x00},				{0x00},				{0x00},				{0x00},				{0x00},				{0x00},

	// 0xF0
	{0x00},				{0x00},				{0x00},				{0x00},				{0x00},				{0x00},				{0x00},				{0x00},
	{0x00},				{0x00},				{0x00},				{0x00},				{0x00},				{0x00},				{0x00},				{0x00},
};

const KeyCodes NumlockMap[]
{
	KeyCodes::KeyCode_Keypad_0,
	KeyCodes::KeyCode_Keypad_1,
	KeyCodes::KeyCode_Keypad_2,
	KeyCodes::KeyCode_Keypad_3,
	KeyCodes::KeyCode_Keypad_4,
	KeyCodes::KeyCode_Keypad_5,
	KeyCodes::KeyCode_Keypad_6,
	KeyCodes::KeyCode_Keypad_7,
	KeyCodes::KeyCode_Keypad_8,
	KeyCodes::KeyCode_Keypad_9,
	KeyCodes::KeyCode_Keypad_Period,
	KeyCodes::KeyCode_Keypad_Slash,
	KeyCodes::KeyCode_Keypad_Star,
	KeyCodes::KeyCode_Keypad_Dash,
	KeyCodes::KeyCode_Keypad_Plus,
	KeyCodes::KeyCode_Keypad_Enter,
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
int ScanCodeSet;

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
	KernalSleep(10);
	return GetData();
}

void WriteCommand(unsigned char Command, unsigned char Data)
{
	WriteCommand(Command);
	KernalSleep(10);

	WriteData(Data);
	KernalSleep(10);
}


unsigned char SendKeyboardCommand(unsigned char Command)
{
	if ((KeyboardState & KeystateEchoLock) == KeystateEchoLock)
		KernalPrintf("(%2.2X", Command);

	unsigned char response = KeyboardResendLastByte;

	int Count = 0;
	while (response == KeyboardResendLastByte && Count < 5)
	{
		if ((KeyboardState & KeystateEchoLock) == KeystateEchoLock)
			KernalPrintf(" $");

		WriteData(Command);
		KernalSleep(10);
		
		if ((KeyboardState & KeystateEchoLock) == KeystateEchoLock)
			KernalPrintf("#");

		while ((GetStatus() & OutputBufferFull) == 0)
		{
			if ((KeyboardState & KeystateEchoLock) == KeystateEchoLock)
				KernalPrintf("_");

			KernalWaitATick();
		}

		response = GetData();

		if ((KeyboardState & KeystateEchoLock) == KeystateEchoLock)
			KernalPrintf("%2.2X", response);

		Count++;
	}

	if ((KeyboardState & KeystateEchoLock) == KeystateEchoLock)
		KernalPrintf(")");

	return response;
}

unsigned char SendKeyboardCommand(unsigned char Command, unsigned char Data)
{
	unsigned char response = SendKeyboardCommand(Command);
	if (response == KeboardAck)
	{
		response = SendKeyboardCommand(Data);
	}

	return response;
}

void SetupPS2Keyboard()
{
	KeyboardState = KeyboardEcho;

	// Disable everything
	WriteCommand(DisableFirstPort);
	KernalSleep(10);
	WriteCommand(DisableSecondPort);
	KernalSleep(10);

	// Clear anything pending
	unsigned char Temp = GetStatus();
	if ((Temp & OutputBufferFull) == OutputBufferFull)
		GetData();

	// Get the configuration byte
	unsigned char config = ReadCommand(ReadConfiguration);
	//KernalPrintf(" KB config: %02X ", config);

	// Self Test
	Temp = ReadCommand(SelfTest);
	KernalPrintf(" PS2 SelfTest: %02X\n", Temp);

	// Turn on Primary Interrupt 
	config |= EnableFirstPortInterrupt;
	// Disable Secondary Interrupt 
	config &= ~EnableSecondPortInterrupt;

	// Disable Translation
	// config &= ~FirstPortTranslation;

	WriteCommand(WriteConfiguration, config);
	ScanCodeSet = 1;

	KernalPrintf(" KB Restart: ");
	SendKeyboardCommand(KeyboardRestarteAndSelftest);
	while ((GetStatus() & OutputBufferFull) == 0)
		KernalWaitATick();

	Temp = GetData();
	KernalPrintf("%2.2X\n", Temp);

	// SendKeyboardCommand(KeyboardScanCode, 2);

	KernalPrintf(" KB ID: ");
	SendKeyboardCommand(KeyboardIdentify);
	while ((GetStatus() & OutputBufferFull) == 0)
		KernalWaitATick();

	Temp = GetData();
	KernalPrintf("%2.2X ", Temp);

	if (Temp == 0xAB)
	{
		KernalSleep(10);
		while ((GetStatus() & OutputBufferFull) == 0)
			KernalWaitATick();
		Temp = GetData();

		KernalPrintf("%2.2X", Temp);
		if (Temp == 0x41)
			ScanCodeSet = 1;
	}

	KernalPrintf("\n");

	// Enable
	WriteCommand(EnableFirstPort);
	KernalSleep(20);

	SendKeyboardCommand(KeyboardEnableScanning);

	// Sanity check that translation is off (because sometimes it's not)
	config = 0;
	config = ReadCommand(ReadConfiguration);
	if (ScanCodeSet == 1 || (config & FirstPortTranslation) == FirstPortTranslation)
	{
		ScanCodeSet = 1;
		KernalPrintf(" Scancode Set 1 (config %2.2X)\n", config);

		config |= FirstPortTranslation;
		WriteCommand(WriteConfiguration, config);
	}
	else
	{
		ScanCodeSet = 2;
		KernalPrintf(" Scancode Set 2 (config %2.2X)\n", config);
	}

	// Clear the Keystate
	KeyboardState = 0;
}

void UpdateKeyboardState(Keystates State, bool Break)
{
	if (!Break)
		KeyboardState |= State;
	else
		KeyboardState &= ~State;
}

extern void InsertKeyboardBuffer(uint16_t ScanCode, uint8_t KeyCode, uint16_t KeyState, bool KeyUp, unsigned char Key);
extern void SystemReset();

bool ProcessKeyCode(KeyCodes &KeyCode, bool KeyUp)
{
	switch (KeyCode)
	{
	case KeyCodes::KeyCode_LeftShift:
		UpdateKeyboardState(KeystateLeftShift, KeyUp);
		KeyCode = KeyCodes::KeyCode_Blank;
		break;

	case KeyCodes::KeyCode_RightShift:
		UpdateKeyboardState(KeystateRightShift, KeyUp);
		KeyCode = KeyCodes::KeyCode_Blank;
		break;

	case KeyCodes::KeyCode_LeftCtrl:
		UpdateKeyboardState(KeystateLeftCtrl, KeyUp);
		KeyCode = KeyCodes::KeyCode_Blank;
		break;

	case KeyCodes::KeyCode_RightCtrl:
		UpdateKeyboardState(KeystateRightCtrl, KeyUp);
		KeyCode = KeyCodes::KeyCode_Blank;
		break;

	case KeyCodes::KeyCode_LeftAlt:
		UpdateKeyboardState(KeystateLeftAlt, KeyUp);
		KeyCode = KeyCodes::KeyCode_Blank;
		break;

	case KeyCodes::KeyCode_RightAlt:
		UpdateKeyboardState(KeystateRightAlt, KeyUp);
		KeyCode = KeyCodes::KeyCode_Blank;
		break;

	case KeyCodes::KeyCode_LeftGUI:
		UpdateKeyboardState(KeystateLeftGUI, KeyUp);
		KeyCode = KeyCodes::KeyCode_Blank;
		break;

	case KeyCodes::KeyCode_RightGUI:
		UpdateKeyboardState(KeystateRightGUI, KeyUp);
		KeyCode = KeyCodes::KeyCode_Blank;
		break;

		// CTRL-ALT-Backspace
	case KeyCodes::KeyCode_Backspace:
		if ((KeyboardState & KeystateCtrl) != 0 && (KeyboardState & KeystateAlt) != 0)
		{
			if (!KeyUp)
				KeyboardState ^= KeystateEchoLock;

			KeyCode = KeyCodes::KeyCode_Blank;
		}
		break;

		// CTRL-ALT-Delete
	case KeyCodes::KeyCode_Delete:
		if ((KeyboardState & KeystateCtrl) != 0 && (KeyboardState & KeystateAlt) != 0)
		{
			KernalSetPauseFullScreen(false);
			KernalPrintf("\nRebooting...\n");
			SystemReset();
			KeyCode = KeyCodes::KeyCode_Blank;
		}
		break;

	case KeyCodes::KeyCode_NumberLock:
		if (!KeyUp)
		{
			KeyboardState ^= KeystateNumLock;
			//SendKeyboardCommand(KeyboardSetLED, KeyboardState & 0x07);
		}
		KeyCode = KeyCodes::KeyCode_Blank;
		break;

	case KeyCodes::KeyCode_CapsLock:
		if (!KeyUp)
		{
			KeyboardState ^= KeystateCapsLock;
			//SendKeyboardCommand(KeyboardSetLED, KeyboardState & 0x07);
		}
		KeyCode = KeyCodes::KeyCode_Blank;
		break;

	case KeyCodes::KeyCode_ScrollLock:
		if (!KeyUp)
		{
			KeyboardState ^= KeystateScrollLock;
			//SendKeyboardCommand(KeyboardSetLED, KeyboardState & 0x07);
		}
		KeyCode = KeyCodes::KeyCode_Blank;
		break;

	case KeyCodes::KeyCode_SysRequest:
		if (!KeyUp)
		{
			KernalPrintf("\nSystem Request...\n");
			KeyCode = KeyCodes::KeyCode_Blank;
		}
		break;
	}

	if (((KeyboardState & KeystateNumLock) == KeystateNumLock) && (KeyCode >= KeyCodes::KeyCode_Keypad_Insert && KeyCode <= KeyCodes::KeyCode_Keypad_Enter))
	{
		int Index = ((uint8_t)KeyCode - (uint8_t)KeyCodes::KeyCode_Keypad_Insert);
		KeyCode = NumlockMap[Index];
	}

	if (KeyCode == KeyCodes::KeyCode_Blank)
		return true;

	return false;
}

unsigned char TranslateKeyCode(KeyCodes &KeyCode, uint16_t KeyState)
{
	unsigned char Value = 0;
	KeyCodeTranslationEntry ScanCode = KeyCodeTranslationTable[(uint8_t)KeyCode];

	if ((KeyState & KeystateCapsLock) != 0 && (KeyCode >= KeyCodes::KeyCode_a && KeyCode <= KeyCodes::KeyCode_z))
	{
		if ((KeyState & KeystateShift) == 0)
		{
			Value = ScanCode.Shift;
		}
		else
		{
			Value = ScanCode.Value;
		}
	}
	else
	{
		if ((KeyState & KeystateShift) != 0 && ScanCode.Shift != 0x00)
		{
			Value = ScanCode.Shift;
		}
		else
		{
			Value = ScanCode.Value;
		}
	}

	if (Value == 0)
		Value = (uint8_t)KeyCode;

	return Value;
}

void KeyboardInterrupt(InterruptContext * OldContext, uintptr_t * Data)
{
	if ((GetStatus() & OutputBufferFull) == 0)
		return;

	bool KeyUp = false;
	bool E0 = false;

	// Step 1: Covert the Scancode to a KeyCode
	uint16_t ScanCode = GetData();
	KeyCodes KeyCode = KeyCodes::KeyCode_Blank;
	
	if (ScanCodeSet == 1)
	{
		if ((KeyboardState & KeystateEchoLock) == KeystateEchoLock)
			KernalPrintf("[");

		if (ScanCode == 0xE0)
		{
			if ((KeyboardState & KeystateEchoLock) == KeystateEchoLock)
				KernalPrintf("%02X ", ScanCode);

			E0 = true;
			ScanCode = ReadData();
		}		
		else if (ScanCode == 0xE1)
		{
			if ((KeyboardState & KeystateEchoLock) == KeystateEchoLock)
				KernalPrintf("%02X ", ScanCode);

			ScanCode = ReadData();
		}

		if ((ScanCode & 0x80) != 0)
		{
			KeyUp = true;
			ScanCode = (ScanCode & 0x7F);

			if ((KeyboardState & KeystateEchoLock) == KeystateEchoLock)
				KernalPrintf("!");
		}

		if ((KeyboardState & KeystateEchoLock) == KeystateEchoLock)
			KernalPrintf("%02X]", ScanCode);

		if (ScanCode >= 0x90)
			return;

		// Get the keycode
		if (!E0)
			KeyCode = ScanCodeSet1[ScanCode];
		else
			KeyCode = ScanCodeSet1E0[ScanCode];
	}
	else if (ScanCodeSet == 2)
	{
		if ((KeyboardState & KeystateEchoLock) == KeystateEchoLock)
			KernalPrintf("[");

		if (ScanCode == 0xE0)
		{
			if ((KeyboardState & KeystateEchoLock) == KeystateEchoLock)
				KernalPrintf("%02X ", ScanCode);

			E0 = true;
			ScanCode = ReadData();
		}
		else if (ScanCode == 0xE1)
		{
			if ((KeyboardState & KeystateEchoLock) == KeystateEchoLock)
				KernalPrintf("%02X ", ScanCode);

			ScanCode = ReadData();
		}

		if (ScanCode == 0xF0)
		{
			KeyUp = true;
			ScanCode = ReadData();

			if ((KeyboardState & KeystateEchoLock) == KeystateEchoLock)
				KernalPrintf("!");
		}

		if ((KeyboardState & KeystateEchoLock) == KeystateEchoLock)
			KernalPrintf("%02X]", ScanCode);

		if (ScanCode >= 0x90)
			return;

		// Get the keycode
		if (!E0)
			KeyCode = ScanCodeSet2[ScanCode];
		else
			KeyCode = ScanCodeSet2E0[ScanCode];
	}

	if (KeyCode == KeyCodes::KeyCode_Blank)
		return;

	// Step 2: Process any speical keycodes
	if (ProcessKeyCode(KeyCode, KeyUp))
		return;

	// Step 3: Convert the Keycode into the actuall display value
	unsigned char InputValue = TranslateKeyCode(KeyCode, KeyboardState);

	//if ((KeyboardState & KeystateEchoLock) == KeystateEchoLock)
	//	KernalPrintf("{%2.2X}", InputValue);

	// Step 4: Add it to the keyboard buffer
	InsertKeyboardBuffer(E0 ? 0xE000 + ScanCode : ScanCode, (uint8_t) KeyCode, KeyboardState, KeyUp, InputValue);

}