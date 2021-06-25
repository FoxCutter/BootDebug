#include "PS2Keyboard.h"
#include <intrin.h>
#include "KernalLib.h"
#include "Keycodes.h"
#include "PS2ScanCodes.h"
#include "LowLevel.h"

namespace
{
	struct KeyCodeTranslationEntry
	{
		char Value;
		char ShiftValue;
		// uint8_t AltGrValue;
		// uint8_t AltGrShiftValue;
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
		{(char)KeyCodes::KeyCode_Insert},		{(char)KeyCodes::KeyCode_End},			{(char)KeyCodes::KeyCode_Down},			{(char)KeyCodes::KeyCode_PageUp},
		{(char)KeyCodes::KeyCode_Left},			{0x00},									{(char)KeyCodes::KeyCode_Right},		{(char)KeyCodes::KeyCode_Home},
		{(char)KeyCodes::KeyCode_Up},			{(char)KeyCodes::KeyCode_PageUp},		{(char)KeyCodes::KeyCode_Delete},		{'/'},
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
		KeyCodes::KeyCode_Keypad_0,				// Insert / 0  
		KeyCodes::KeyCode_Keypad_1,				// End / 1  
		KeyCodes::KeyCode_Keypad_2,				// Down / 2  
		KeyCodes::KeyCode_Keypad_3,				// PageDown / 3  
		KeyCodes::KeyCode_Keypad_4,				// Left / 4  
		KeyCodes::KeyCode_Keypad_5,				// Blank / 5  
		KeyCodes::KeyCode_Keypad_6,				// Right / 6  
		KeyCodes::KeyCode_Keypad_7,				// Home / 7  
		KeyCodes::KeyCode_Keypad_8,				// Up / 8  
		KeyCodes::KeyCode_Keypad_9,				// PageUp / 9  
		KeyCodes::KeyCode_Keypad_Period,		// Delete / .  
		KeyCodes::KeyCode_Keypad_Slash,
		KeyCodes::KeyCode_Keypad_Star,
		KeyCodes::KeyCode_Keypad_Dash,
		KeyCodes::KeyCode_Keypad_Plus,
		KeyCodes::KeyCode_Keypad_Enter,
	};

	enum PS2Commands : uint8_t
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

		// Mouse Commands
		MouseSetScaling = 0xE6,
		MouseSetResolution = 0xE8,
		MouseStatusRequest = 0xE9,
		MouseSetStreamMode = 0xEA,
		MouseReadData = 0xEB,
		MouseResetWrapMode = 0xEC,
		MouseSetWrapMode = 0xEE,
		MouseSetRemodeMode = 0xF0,
		MouseIdentify = 0xF2,
		MouseSetSampleRate = 0xF3,
		MouseEnableReporting = 0xF4,
		MouseDisableReporting = 0xF5,
		MouseResetToDefault = 0xF6,
		MouseResend = 0xFE,
		MouseRestarteAndSelftest = 0xFF,

		MouseAck = 0xFA,
		MouseSetTestPassed = 0xAA,
		MouseSetTestFailed = 0xFC,
	};

	enum PS2Bits : uint8_t
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

	const int BufferLen = 32;
	uint8_t OutputBuffer[BufferLen];
	uint8_t OutputBufferPos = 0;
	uint8_t OutputBufferLength = 0;
	uint8_t OutputRetryCount = 0;
	bool	PendingResponse = false;

	const int MainCodeSet = 0x00;
	const int ExtendedCodeSet = 0xE0;
	const int SpeicalCodeSet = 0xE1;

	bool SecondPort = false;

	int CodeSet = 0;
	bool KeyUp = false;
}

uint8_t ReadDataPort()
{
	return __inbyte(0x60);
}

uint8_t ReadStatusPort()
{
	return __inbyte(0x64);
}

void WriteDataPort(uint8_t Data)
{
	__outbyte(0x60, Data);
}

void WriteCommandPort(uint8_t Command)
{
	__outbyte(0x64, Command);
}

uint8_t ReadCommand(uint8_t Command)
{
	WriteCommandPort(Command);
	KernalSleep(10);
	return ReadDataPort();
}

void WriteCommand(uint8_t Command)
{
	WriteCommandPort(Command);
	KernalSleep(10);
}

void WriteCommand(uint8_t Command, uint8_t Data)
{
	WriteCommandPort(Command);
	KernalSleep(10);

	WriteDataPort(Data);
	KernalSleep(10);
}

uint8_t WriteKeyboardCommand(uint8_t Command)
{
	if ((KeyboardState & KeystateEchoLock) == KeystateEchoLock)
		KernalPrintf("(%2.2X", Command);

	uint8_t response = KeyboardResendLastByte;

	int Count = 0;
	while (response == KeyboardResendLastByte && Count < 5)
	{
		if ((KeyboardState & KeystateEchoLock) == KeystateEchoLock)
			KernalPrintf("$");

		WriteDataPort(Command);
		KernalSleep(10);
		
		int WaitCount = 0;
		while ((ReadStatusPort() & OutputBufferFull) == 0 && WaitCount < 20)
		{
			if ((KeyboardState & KeystateEchoLock) == KeystateEchoLock)
				KernalPrintf("_");

			KernalWaitATick();

			WaitCount++;
		}

		if (WaitCount >= 20)
			break;

		response = ReadDataPort();

		if ((KeyboardState & KeystateEchoLock) == KeystateEchoLock)
			KernalPrintf(" %2.2X", response);

		Count++;
	}

	if ((KeyboardState & KeystateEchoLock) == KeystateEchoLock)
		KernalPrintf(")");

	return response;
}

uint8_t WriteKeyboardCommand(uint8_t Command, uint8_t Data)
{
	uint8_t response = WriteKeyboardCommand(Command);
	if (response == KeboardAck)
	{
		response = WriteKeyboardCommand(Data);
	}

	return response;
}

uint8_t ReadKeyboardCommand(uint8_t Command)
{
	uint8_t response = WriteKeyboardCommand(Command);
	if (response == KeboardAck)
	{
		while ((ReadStatusPort() & OutputBufferFull) == 0)
			KernalWaitATick();

		response = ReadDataPort();
	}

	return response;
}

uint8_t ReadKeyboardCommand(uint8_t Command, uint8_t Data)
{
	uint8_t response = WriteKeyboardCommand(Command, Data);
	if (response == KeboardAck)
	{
		while ((ReadStatusPort() & OutputBufferFull) == 0)
			KernalWaitATick();

		response = ReadDataPort();
	}

	return response;
}

bool SendQueuedCommand()
{
	if (PendingResponse)
		return false;

	if (OutputBufferLength == 0)
		return true;
	
	if ((ReadStatusPort() & PS2Bits::OutputBufferFull) == PS2Bits::OutputBufferFull)
		return false;

	if ((KeyboardState & KeystateEchoLock) == KeystateEchoLock)
		KernalPrintf("{%2.2X}", OutputBuffer[OutputBufferPos]);

	WriteDataPort(OutputBuffer[OutputBufferPos]);
	OutputBufferPos++;
	OutputBufferLength--;
	PendingResponse = true;

	return true;
}

bool QueueKeyboardCommand(uint8_t Command, uint8_t Data)
{
	// if the buffer is full just throw out this command
	if (OutputBufferPos + OutputBufferLength + 2 >= BufferLen)
		return false;

	if (OutputBufferLength == 0)
		OutputBufferPos = 0;

	int StartPos = OutputBufferPos + OutputBufferLength;

	OutputBuffer[StartPos] = Command;
	OutputBuffer[StartPos + 1] = Data;
	OutputBufferLength += 2;

	if (!PendingResponse)
		return SendQueuedCommand();
	
	return true;
}
bool ProcessKeyCode(KeyCodes &KeyCode, bool KeyUp);

void SetupPS2Keyboard()
{
	KeyboardState = 0;
	ScanCodeSet = 1;

	// Get the configuration byte
	uint8_t config = ReadCommand(ReadConfiguration);

	KernalPrintf(" PS2 config: %02X", config);

	// Disable everything
	WriteKeyboardCommand(KeyboardDisableScanning);
	WriteCommand(DisableFirstPort);
	WriteCommand(DisableSecondPort);

	// Clear anything pending
	uint8_t Temp = ReadStatusPort();
	if ((Temp & OutputBufferFull) == OutputBufferFull)
		ReadDataPort();

	// Disable all Interrupts and Translation
	config = ReadCommand(ReadConfiguration);
	config &= ~(PS2Bits::EnableFirstPortInterrupt | PS2Bits::EnableSecondPortInterrupt | PS2Bits::FirstPortTranslation);
	WriteCommand(WriteConfiguration, config);

	WriteCommand(EnableSecondPort);
	config = ReadCommand(ReadConfiguration);
	if ((config & PS2Bits::DisableSecondPortClock) == 0)
	{
		SecondPort = true;
		KernalPrintf(" (2 Ports)");
		WriteCommand(DisableSecondPort);
	}

	Temp = ReadCommand(SelfTest);
	KernalPrintf("  Self Test: %02X", Temp);
	if (Temp == SelfTestFailed)
		KernalPrintf(" ----Keyboard Controller Self Test Failed----\n");
	
	Temp = ReadCommand(TestFirstPort);
	KernalPrintf(" %02X\n", Temp);
	if (Temp != 0x00)
		KernalPrintf(" ----Keyboard Controller First Port Test Failed----\n");
	
	WriteCommand(EnableFirstPort);

	KernalPrintf(" Keyboard Reset: ");

	Temp = ReadKeyboardCommand(KeyboardRestarteAndSelftest);
	KernalPrintf("%2.2X", Temp);

	if(Temp == KeyboardSelfTestFailed1 || Temp == KeyboardSelfTestFailed2)
		KernalPrintf(" ----Keyboard Self Test Failed----\n");

	// In my testing on real hardware, I've seen:
	// Keyboards that are using Set 2, have translation off, that still translate the keycodes. They return 0x41 as the second ID byte
	// Keyboards where you can disable translation, but it always returns 0x41 as the second ID byte.

	// The result is that I have to be very pessimistic about being able to disable translation, and if there is any indication that
	// we might not be able to, drop back to translated mode and Set 1

	KernalPrintf("  KB ID: ");
	Temp = ReadKeyboardCommand(KeyboardIdentify);
	KernalPrintf("%2.2X", Temp);

	if (Temp == 0xAB)
	{
		KernalSleep(10);
		while ((ReadStatusPort() & OutputBufferFull) == 0)
			KernalWaitATick();
		Temp = ReadDataPort();
		
		KernalPrintf(" %2.2X", Temp);
		
		// 0xAB 0x83 is for a keyboard that has translation disabled.
		if (Temp == 0x83)
			ScanCodeSet = 2;
	}

	// Sanity check that translation is off
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

	// Enable the keyboard
	WriteKeyboardCommand(KeyboardEnableScanning);

	// Turn on Primary Interrupt 
	config = ReadCommand(ReadConfiguration);
	config |= EnableFirstPortInterrupt;
	WriteCommand(WriteConfiguration, config);

	// Clear the Keystate
	KeyboardState = 0;

	QueueKeyboardCommand(KeyboardSetLED, KeyboardState & 0x07);
}

void WriteMouse(uint8_t Value)
{
	WriteCommandPort(WriteSecondPortInputBuffer);
	KernalSleep(10);

	WriteDataPort(Value);
}

uint8_t WriteMouseCommand(uint8_t Command)
{
	WriteMouse(Command);

	int WaitCount = 0;
	while ((ReadStatusPort() & OutputBufferFull) == 0 && WaitCount < 20)
	{
		KernalWaitATick();

		WaitCount++;
	}

	if (WaitCount >= 20)
		return 0xFF;

	return ReadDataPort();
}

uint8_t WriteMouseCommand(uint8_t Command, uint8_t Data)
{
	uint8_t response = WriteMouseCommand(Command);
	if (response == MouseAck)
		response = WriteMouseCommand(Data);

	return response;
}

uint8_t ReadMouseCommand(uint8_t Command)
{
	uint8_t response = WriteMouseCommand(Command);
	if (response == MouseAck)
	{
		while ((ReadStatusPort() & OutputBufferFull) == 0)
			KernalWaitATick();
		
		response = ReadDataPort();
	}

	return response;
}


void SetupPS2Mouse()
{
	if (!SecondPort)
		return;

	uint8_t Temp = ReadCommand(TestSecondPort);
	KernalPrintf(" PS/2 Port 2 Self Test %02X", Temp);
	if (Temp != 0x00)
		KernalPrintf(" ----Keyboard Controller Second Port Test Failed----\n");

	WriteCommand(EnableSecondPort);
	
	Temp = ReadMouseCommand(MouseRestarteAndSelftest);
	KernalPrintf("  Reset %2.2X", Temp);
	if(Temp != MouseSetTestPassed)
		KernalPrintf(" ----Mouse Failed Selftest----\n");
	
	KernalWaitATick();
	while ((ReadStatusPort() & OutputBufferFull) == 0)
		KernalWaitATick();
	Temp = ReadDataPort();

	KernalPrintf(" %2.2X", Temp);

	Temp = ReadMouseCommand(MouseIdentify);
	KernalPrintf("  ID: %2.2X\n", Temp);

	//WriteMouseCommand(MouseSetSampleRate, 200);
	//WriteMouseCommand(MouseSetSampleRate, 100);
	//WriteMouseCommand(MouseSetSampleRate, 80);

	//Temp = ReadMouseCommand(MouseIdentify);
	//KernalPrintf(" %2.2X", Temp);

	//WriteMouseCommand(MouseSetSampleRate, 200);
	//WriteMouseCommand(MouseSetSampleRate, 200);
	//WriteMouseCommand(MouseSetSampleRate, 80);

	//Temp = ReadMouseCommand(MouseIdentify);
	//KernalPrintf(" %2.2X\n", Temp);

	WriteMouseCommand(MouseEnableReporting);

	// Turn on Interrupt 
	uint8_t config = ReadCommand(ReadConfiguration);
	config |= EnableSecondPortInterrupt;
	WriteCommand(WriteConfiguration, config);

}

void UpdateKeyboardState(Keystates State, bool Break)
{
	if (!Break)
		KeyboardState |= State;
	else
		KeyboardState &= ~State;
}

extern void InsertKeyboardBuffer(uint16_t ScanCode, uint8_t KeyCode, uint16_t KeyState, bool KeyUp, uint8_t Key);
extern void SystemReset();

bool ProcessKeyCode(KeyCodes &KeyCode, bool KeyUp)
{
	if (KeyCode == KeyCodes::KeyCode_Blank)
		return false;
	
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
			QueueKeyboardCommand(KeyboardSetLED, KeyboardState & 0x07);
		}
		KeyCode = KeyCodes::KeyCode_Blank;
		break;

	case KeyCodes::KeyCode_CapsLock:
		if (!KeyUp)
		{
			KeyboardState ^= KeystateCapsLock;
			QueueKeyboardCommand(KeyboardSetLED, KeyboardState & 0x07);
		}
		KeyCode = KeyCodes::KeyCode_Blank;
		break;

	case KeyCodes::KeyCode_ScrollLock:
		if (!KeyUp)
		{
			KeyboardState ^= KeystateScrollLock;
			QueueKeyboardCommand(KeyboardSetLED, KeyboardState & 0x07);
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

	case KeyCodes::KeyCode_Pause:
		if (!KeyUp)
		{
			KernalPrintf("\nPause...\n");
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

char TranslateKeyCode(KeyCodes &KeyCode, uint16_t KeyState)
{
	char Value = 0;
	if (KeyCode == KeyCodes::KeyCode_Blank)
		return Value;

	KeyCodeTranslationEntry ScanCode = KeyCodeTranslationTable[(uint8_t)KeyCode];

	if ((KeyState & KeystateCapsLock) != 0 && (KeyCode >= KeyCodes::KeyCode_a && KeyCode <= KeyCodes::KeyCode_z))
	{
		// Shift inverts CapsLock
		if ((KeyState & KeystateShift) == 0)
		{
			Value = ScanCode.ShiftValue;
		}
		else
		{
			Value = ScanCode.Value;
		}
	}
	else
	{
		if ((KeyState & KeystateShift) != 0 && ScanCode.ShiftValue != 0x00)
		{
			Value = ScanCode.ShiftValue;
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
	if ((ReadStatusPort() & OutputBufferFull) == 0)
	{
		if (!PendingResponse && OutputBufferLength != 0)
			SendQueuedCommand();
		
		return;
	}

	uint16_t ScanCode = ReadDataPort();

	if (PendingResponse && (ScanCode == KeboardAck || ScanCode == KeyboardResendLastByte))
	{
		if ((KeyboardState & KeystateEchoLock) == KeystateEchoLock)
			KernalPrintf("{%2.2X}", ScanCode);

		if (ScanCode == KeboardAck)
		{
			PendingResponse = false;
			OutputRetryCount = 0;

			if (OutputBufferLength != 0)
				SendQueuedCommand();
		}
		else if (ScanCode == KeyboardResendLastByte)
		{
			PendingResponse = false;
			if (OutputRetryCount >= 5)
			{				
				OutputRetryCount = OutputBufferPos = OutputBufferLength = 0;
			}
			else if (OutputBufferPos != 0)
			{
				OutputBufferPos--;
				OutputBufferLength++;

				OutputRetryCount++;
				SendQueuedCommand();
			}
		}

		return;
	}

	KeyCodes KeyCode = KeyCodes::KeyCode_Blank;
	bool InsertInput = true;

	// Step 1: Covert the Scancode to a KeyCode
	if (ScanCode == ExtendedCodeSet)
	{
		CodeSet = ExtendedCodeSet;
		InsertInput = false;
	}
	else if (ScanCode == SpeicalCodeSet)
	{
		CodeSet = SpeicalCodeSet;
		InsertInput = false;
	}
	else if (ScanCodeSet == 1)
	{
		if ((ScanCode & 0x80) != 0)
		{
			KeyUp = true;
			ScanCode = (ScanCode & 0x7F);
		}

		if (CodeSet == SpeicalCodeSet && ScanCode == 0x1D)
		{
			// Ignore this code (left ctrl)
		}
		else
		{
			if ((KeyboardState & KeystateEchoLock) == KeystateEchoLock)
			{
				KernalPrintf("[");
				if (CodeSet != MainCodeSet)
					KernalPrintf("%02X ", CodeSet);

				if (KeyUp)
					KernalPrintf("!");

				KernalPrintf("%02X]", ScanCode);
			}

			if (ScanCode < 0x90)
			{
				// Get the keycode
				if (CodeSet == MainCodeSet)
					KeyCode = ScanCodeSet1[ScanCode];
				else
					KeyCode = ScanCodeSet1Extended[ScanCode];
			}
		}
	}
	else if (ScanCodeSet == 2)
	{
		if (ScanCode == 0xF0)
		{
			KeyUp = true;
			InsertInput = false;
		}
		else if (CodeSet == SpeicalCodeSet && ScanCode == 0x14)
		{
			// Ignore this code (left ctrl)
		}
		else
		{
			if ((KeyboardState & KeystateEchoLock) == KeystateEchoLock)
			{
				KernalPrintf("[");
				if (CodeSet != MainCodeSet)
					KernalPrintf("%02X ", CodeSet);

				if (KeyUp)
					KernalPrintf("!");

				KernalPrintf("%02X]", ScanCode);
			}

			if (ScanCode < 0x90)
			{
				// Get the keycode
				if (CodeSet == MainCodeSet)
					KeyCode = ScanCodeSet2[ScanCode];
				else
					KeyCode = ScanCodeSet2Extended[ScanCode];
			}
		}
	}

	if (InsertInput)
	{
		// Step 2: Process any speical keycodes	
		if (ProcessKeyCode(KeyCode, KeyUp))
		{
			KeyUp = false;
			CodeSet = MainCodeSet;
		}
		else
		{
			// Step 3: Convert the Keycode into the actuall display value
			char InputValue = TranslateKeyCode(KeyCode, KeyboardState);

			//if ((KeyboardState & KeystateEchoLock) == KeystateEchoLock)
			//	KernalPrintf("{%2.2X}", InputValue);

			// Step 4: Add it to the keyboard buffer
			InsertKeyboardBuffer((CodeSet << 8) + ScanCode, (uint8_t)KeyCode, KeyboardState, KeyUp, InputValue);

			KeyUp = false;
			CodeSet = MainCodeSet;
		}
	}

	if (!PendingResponse && OutputBufferLength != 0)
		SendQueuedCommand();
}

uint8_t MouseData[3];
uint8_t MouseDataPos = 0;

void MouseInterrupt(InterruptContext * OldContext, uintptr_t * Data)
{
	MouseData[MouseDataPos] = ReadDataPort();
	MouseDataPos++;

	if (MouseDataPos == 3)
	{
		bool Left = (MouseData[0] & 0x01) != 0;
		bool Right = (MouseData[0] & 0x02) != 0;
		bool Middle = (MouseData[0] & 0x04) != 0;

		int16_t xValue = MouseData[1];
		int16_t yValue = MouseData[2];

		//xValue = xValue - ((MouseData[0] << 4) & 0x100);
		//yValue = yValue - ((MouseData[0] << 3) & 0x100);

		if ((MouseData[0] & 0x10) != 0)
			xValue = xValue - 0x100;
		if ((MouseData[0] & 0x20) != 0)
			yValue = yValue - 0x100;

		KernalPrintf("{%c%c%c X:%d Y:%d}", Left ? 'L' : ' ', Middle ? 'M' : ' ', Right ? 'R' : ' ', xValue, yValue);
		MouseDataPos = 0;
	}
}
