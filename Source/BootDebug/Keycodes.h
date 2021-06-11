#pragma once
#include <stdint.h>

// Displayable characters will map to their ANSII key codes, none display characters that dosn't have a matching ANSII code will get codes with the high bit set.
enum KeyCodes : uint8_t
{
	KeyCode_Blank = 0x00,

	KeyCode_Backspace = 0x08,
	KeyCode_Tab,
	KeyCode_Enter,
	KeyCode_Escape = 0x1B,

	KeyCode_Space = ' ',	// 0x20
	KeyCode_0 = '0',		// 0x30
	KeyCode_9 = '9',		// 0x39
	KeyCode_a = 'a',		// 0x61
	KeyCode_z = 'z',		// 0x7A
	KeyCode_Delete = 0x7F,

	KeyCode_Extended = 0x80, // Extended codes

	// Movement
	KeyCode_Insert = KeyCode_Extended,
	KeyCode_End,
	KeyCode_Down,
	KeyCode_PageDown,
	KeyCode_Left,
	KeyCode_Right,
	KeyCode_Home,
	KeyCode_Up,
	KeyCode_PageUp,

	// F keys
	KeyCode_F0 = 0x90,
	KeyCode_F1,
	KeyCode_F2,
	KeyCode_F3,
	KeyCode_F4,
	KeyCode_F5,
	KeyCode_F6,
	KeyCode_F7,
	KeyCode_F8,
	KeyCode_F9,
	KeyCode_F10,
	KeyCode_F11,
	KeyCode_F12,
	KeyCode_F13,
	KeyCode_F14,
	KeyCode_F15,

	// Modifiers (LSB = Right)
	KeyCode_LeftShift = 0xA0,
	KeyCode_RightShift,
	KeyCode_LeftCtrl = 0xA2,
	KeyCode_RightCtrl,
	KeyCode_LeftAlt = 0xA4,
	KeyCode_RightAlt,
	KeyCode_LeftGUI = 0xA6,
	KeyCode_RightGUI,
	KeyCode_AltGr = 0xA8,

	KeyCode_Keypad_Insert = 0xB0,			// 0 / Insert
	KeyCode_Keypad_End,						// 1 / End
	KeyCode_Keypad_Down,					// 2 / Down
	KeyCode_Keypad_PageDown,				// 3 / PageDown
	KeyCode_Keypad_Left,					// 4 / Left 
	KeyCode_Keypad_Middle,					// 5 / Blank
	KeyCode_Keypad_Right,					// 6 / Right
	KeyCode_Keypad_Home,					// 7 / Home
	KeyCode_Keypad_Up,						// 8 / Up
	KeyCode_Keypad_PageUp,					// 9 / PageUp
	KeyCode_Keypad_Delete,					// . / Delete
	KeyCode_Keypad_Slash,
	KeyCode_Keypad_Star,
	KeyCode_Keypad_Dash,
	KeyCode_Keypad_Plus,
	KeyCode_Keypad_Enter,

	KeyCode_Keypad_0 = 0xC0,				// 0 / Insert
	KeyCode_Keypad_1,						// 1 / End
	KeyCode_Keypad_2,						// 2 / Down
	KeyCode_Keypad_3,						// 3 / PageDown
	KeyCode_Keypad_4,						// 4 / Left 
	KeyCode_Keypad_5,						// 5 / Blank
	KeyCode_Keypad_6,						// 6 / Right
	KeyCode_Keypad_7,						// 7 / Home
	KeyCode_Keypad_8,						// 8 / Up
	KeyCode_Keypad_9,						// 9 / PageUp
	KeyCode_Keypad_Period,					// . / Delete


	KeyCode_CapsLock = 0xD0,
	KeyCode_NumberLock,
	KeyCode_ScrollLock,
	KeyCode_Apps,
	KeyCode_Power,
	KeyCode_Sleep,
	KeyCode_Wake,
	KeyCode_PrintScreen,
	KeyCode_Pause,
	KeyCode_Break,

	KeyCode_MultiMedia = 0xE0,

	KeyCode_SysRequest = 0xFF
};


enum Keystates : uint16_t
{
	KeystateLeftShift = 0x01,
	KeystateRightShift = 0x02,
	KeystateShift = KeystateLeftShift | KeystateRightShift,

	KeystateLeftCtrl = 0x04,
	KeystateRightCtrl = 0x08,
	KeystateCtrl = KeystateLeftCtrl | KeystateRightCtrl,

	KeystateLeftAlt = 0x10,
	KeystateRightAlt = 0x20,
	KeystateAlt = KeystateLeftAlt | KeystateRightAlt,

	KeystateLeftGUI = 0x40,
	KeystateRightGUI = 0x80,
	KeystateGUI = KeystateLeftGUI | KeystateRightGUI,

	KeystateNumLock = 0x100,
	KeystateScrollLock = 0x200,
	KeystateCapsLock = 0x400,
	KeystateEchoLock = 0x800,
};
