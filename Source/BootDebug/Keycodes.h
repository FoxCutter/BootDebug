#pragma once
#include <stdint.h>

// Displayable characters will map to their ANSII key codes, none display characters that dosn't have a matching ANSII code will get codes with the high bit set.
enum class KeyCodes : uint8_t
{
	KeyCode_Blank = 0x00,

	KeyCode_Backspace = 0x08,
	KeyCode_Tab,
	KeyCode_Enter,
	KeyCode_Escape = 0x1B,

	// 0x20
	KeyCode_Space = ' ',	
	KeyCode_ExclamationMark = '!',
	KeyCode_QuotationMark = '\"',
	KeyCode_NumberSign = '#',
	KeyCode_DollarSign = '$',
	KeyCode_PercentSign = '%',
	KeyCode_Ampersand = '&',
	KeyCode_Apostrophe = '\'',
	KeyCode_LeftParenthesis = '(',
	KeyCode_RightParenthesis = ')',
	KeyCode_Star = '*',
	KeyCode_Plus = '+',
	KeyCode_Comma = ',',
	KeyCode_Dash = '-',
	KeyCode_Period = '.',
	KeyCode_Slash = '/',
	
	// 0x30
	KeyCode_0 = '0',		
	KeyCode_1 = '1',
	KeyCode_2 = '2',
	KeyCode_3 = '3',
	KeyCode_4 = '4',
	KeyCode_5 = '5',
	KeyCode_6 = '6',
	KeyCode_7 = '7',
	KeyCode_8 = '8',
	KeyCode_9 = '9',		
	KeyCode_Colon = ':',
	KeyCode_Semicolon = ';',
	KeyCode_LessThan = '<',
	KeyCode_Equals = '=',
	KeyCode_GreaterThan = '>',
	KeyCode_QuestionMark = '?',
	
	// 0x40
	KeyCode_AtSign = '@',
	KeyCode_A = 'A',
	KeyCode_B = 'B',
	KeyCode_C = 'C',
	KeyCode_D = 'D',
	KeyCode_E = 'E',
	KeyCode_F = 'F',
	KeyCode_G = 'G',
	KeyCode_H = 'H',
	KeyCode_I = 'I',
	KeyCode_J = 'J',
	KeyCode_K = 'K',
	KeyCode_L = 'L',
	KeyCode_M = 'M',
	KeyCode_N = 'N',
	KeyCode_O = 'O',

	// 0x50
	KeyCode_P = 'P',
	KeyCode_Q = 'Q',
	KeyCode_R = 'R',
	KeyCode_S = 'S',
	KeyCode_T = 'T',
	KeyCode_U = 'U',
	KeyCode_V = 'V',
	KeyCode_W = 'W',
	KeyCode_X = 'X',
	KeyCode_Y = 'Y',
	KeyCode_Z = 'Z',
	KeyCode_LeftSquareBracket = '[',
	KeyCode_Backslash = '\\',
	KeyCode_RightSquareBracket = ']',
	KeyCode_Carret = '^',
	KeyCode_Underline = '_',

	// 0x60
	KeyCode_Backtick = '`',
	KeyCode_a = 'a',
	KeyCode_b = 'b',
	KeyCode_c = 'c',
	KeyCode_d = 'd',
	KeyCode_e = 'e',
	KeyCode_f = 'f',
	KeyCode_g = 'g',
	KeyCode_h = 'h',
	KeyCode_i = 'i',
	KeyCode_j = 'j',
	KeyCode_k = 'k',
	KeyCode_l = 'l',
	KeyCode_m = 'm',
	KeyCode_n = 'n',
	KeyCode_o = 'o',

	// 0x70
	KeyCode_p = 'p',
	KeyCode_q = 'q',
	KeyCode_r = 'r',
	KeyCode_s = 's',
	KeyCode_t = 't',
	KeyCode_u = 'u',
	KeyCode_v = 'v',
	KeyCode_w = 'w',
	KeyCode_x = 'x',
	KeyCode_y = 'y',
	KeyCode_z = 'z',			
	KeyCode_LeftCurlyBacket = '{',
	KeyCode_Pipe = '|',
	KeyCode_RightCurlyBacket = '}',
	KeyCode_Tilde = '~',
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
	KeyCode_PrintScreen,
	KeyCode_Pause,
	KeyCode_Break,

	KeyCode_MultiMedia = 0xE0,

	KeyCode_SysRequest = 0xFF
};


enum Keystates : uint16_t
{
	KeystateScrollLock = 0x01,
	KeystateNumLock = 0x02,
	KeystateCapsLock = 0x04,
	KeystateEchoLock = 0x08,

	KeystateLeftShift = 0x0100,
	KeystateRightShift = 0x0200,
	KeystateShift = KeystateLeftShift | KeystateRightShift,

	KeystateLeftCtrl = 0x0400,
	KeystateRightCtrl = 0x0800,
	KeystateCtrl = KeystateLeftCtrl | KeystateRightCtrl,

	KeystateLeftAlt = 0x1000,
	KeystateRightAlt = 0x2000,
	KeystateAlt = KeystateLeftAlt | KeystateRightAlt,

	KeystateLeftGUI = 0x4000,
	KeystateRightGUI = 0x8000,
	KeystateGUI = KeystateLeftGUI | KeystateRightGUI,

};
