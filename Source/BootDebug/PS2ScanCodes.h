#pragma once
#include "Keycodes.h"

const KeyCodes ScanCodeSet1[] =
{
	// 0x00
	KeyCodes::KeyCode_Blank,			KeyCodes::KeyCode_Escape,				KeyCodes::KeyCode_1,					KeyCodes::KeyCode_2,					KeyCodes::KeyCode_3,				KeyCodes::KeyCode_4,				KeyCodes::KeyCode_5,				KeyCodes::KeyCode_6,
	KeyCodes::KeyCode_7,				KeyCodes::KeyCode_8,					KeyCodes::KeyCode_9,					KeyCodes::KeyCode_0,					KeyCodes::KeyCode_Dash,			KeyCodes::KeyCode_Equals,			KeyCodes::KeyCode_Backspace,		KeyCodes::KeyCode_Tab,

	// 0x10
	KeyCodes::KeyCode_q,				KeyCodes::KeyCode_w,					KeyCodes::KeyCode_e,					KeyCodes::KeyCode_r,					KeyCodes::KeyCode_t,				KeyCodes::KeyCode_y,				KeyCodes::KeyCode_u,				KeyCodes::KeyCode_i,
	KeyCodes::KeyCode_o,				KeyCodes::KeyCode_p,					KeyCodes::KeyCode_LeftSquareBracket,	KeyCodes::KeyCode_RightSquareBracket,	KeyCodes::KeyCode_Enter,			KeyCodes::KeyCode_LeftCtrl,		KeyCodes::KeyCode_a,				KeyCodes::KeyCode_s,

	// 0x20
	KeyCodes::KeyCode_d,				KeyCodes::KeyCode_f,					KeyCodes::KeyCode_g,					KeyCodes::KeyCode_h,					KeyCodes::KeyCode_j,				KeyCodes::KeyCode_k,				KeyCodes::KeyCode_l,				KeyCodes::KeyCode_Semicolon,
	KeyCodes::KeyCode_Ampersand,		KeyCodes::KeyCode_Backtick,			KeyCodes::KeyCode_LeftShift,			KeyCodes::KeyCode_Backslash,			KeyCodes::KeyCode_z,				KeyCodes::KeyCode_x,				KeyCodes::KeyCode_c,				KeyCodes::KeyCode_v,

	// 0x30
	KeyCodes::KeyCode_b,				KeyCodes::KeyCode_n,					KeyCodes::KeyCode_m,					KeyCodes::KeyCode_Comma,				KeyCodes::KeyCode_Period,			KeyCodes::KeyCode_Slash,			KeyCodes::KeyCode_RightShift,		KeyCodes::KeyCode_Keypad_Star,
	KeyCodes::KeyCode_LeftAlt,		KeyCodes::KeyCode_Space,				KeyCodes::KeyCode_CapsLock,			KeyCodes::KeyCode_F1,					KeyCodes::KeyCode_F2,				KeyCodes::KeyCode_F3,				KeyCodes::KeyCode_F4,				KeyCodes::KeyCode_F5,

	// 0x40
	KeyCodes::KeyCode_F6,				KeyCodes::KeyCode_F7,					KeyCodes::KeyCode_F8,					KeyCodes::KeyCode_F9,					KeyCodes::KeyCode_F10,			KeyCodes::KeyCode_NumberLock,		KeyCodes::KeyCode_ScrollLock,		KeyCodes::KeyCode_Keypad_Home,
	KeyCodes::KeyCode_Keypad_Up,		KeyCodes::KeyCode_Keypad_PageUp,		KeyCodes::KeyCode_Keypad_Dash,		KeyCodes::KeyCode_Keypad_Left,		KeyCodes::KeyCode_Keypad_Middle,	KeyCodes::KeyCode_Keypad_Right,	KeyCodes::KeyCode_Keypad_Plus,	KeyCodes::KeyCode_Keypad_End,

	// 0x50
	KeyCodes::KeyCode_Keypad_Down,	KeyCodes::KeyCode_Keypad_PageDown,	KeyCodes::KeyCode_Keypad_Insert,		KeyCodes::KeyCode_Keypad_Delete,		KeyCodes::KeyCode_SysRequest,		KeyCodes::KeyCode_Blank,			KeyCodes::KeyCode_Blank,			KeyCodes::KeyCode_F11,
	KeyCodes::KeyCode_F12,			KeyCodes::KeyCode_Blank,				KeyCodes::KeyCode_Blank,				KeyCodes::KeyCode_Blank,				KeyCodes::KeyCode_Blank,			KeyCodes::KeyCode_Blank,			KeyCodes::KeyCode_Blank,			KeyCodes::KeyCode_Blank,

	// 0x60
	KeyCodes::KeyCode_Blank,			KeyCodes::KeyCode_Blank,				KeyCodes::KeyCode_Blank,				KeyCodes::KeyCode_Blank,				KeyCodes::KeyCode_Blank,			KeyCodes::KeyCode_Blank,			KeyCodes::KeyCode_Blank,			KeyCodes::KeyCode_Blank,
	KeyCodes::KeyCode_Blank,			KeyCodes::KeyCode_Blank,				KeyCodes::KeyCode_Blank,				KeyCodes::KeyCode_Blank,				KeyCodes::KeyCode_Blank,			KeyCodes::KeyCode_Blank,			KeyCodes::KeyCode_Blank,			KeyCodes::KeyCode_Blank,

	// 0x70
	KeyCodes::KeyCode_Blank,			KeyCodes::KeyCode_Blank,				KeyCodes::KeyCode_Blank,				KeyCodes::KeyCode_Blank,				KeyCodes::KeyCode_Blank,			KeyCodes::KeyCode_Blank,			KeyCodes::KeyCode_Blank,			KeyCodes::KeyCode_Blank,
	KeyCodes::KeyCode_Blank,			KeyCodes::KeyCode_Blank,				KeyCodes::KeyCode_Blank,				KeyCodes::KeyCode_Blank,				KeyCodes::KeyCode_Blank,			KeyCodes::KeyCode_Blank,			KeyCodes::KeyCode_Blank,			KeyCodes::KeyCode_Blank,

	// 0x80
	KeyCodes::KeyCode_Blank,			KeyCodes::KeyCode_Blank,				KeyCodes::KeyCode_Blank,				KeyCodes::KeyCode_Blank,				KeyCodes::KeyCode_Blank,			KeyCodes::KeyCode_Blank,			KeyCodes::KeyCode_Blank,			KeyCodes::KeyCode_Blank,
	KeyCodes::KeyCode_Blank,			KeyCodes::KeyCode_Blank,				KeyCodes::KeyCode_Blank,				KeyCodes::KeyCode_Blank,				KeyCodes::KeyCode_Blank,			KeyCodes::KeyCode_Blank,			KeyCodes::KeyCode_Blank,			KeyCodes::KeyCode_Blank,
};

const KeyCodes ScanCodeSet1E0[] =
{
	// 0xE0 00
	KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_Blank,			KeyCodes::KeyCode_Blank,			KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_Blank,
	KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_Blank,			KeyCodes::KeyCode_Blank,			KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_Blank,

	// 0xE0 10
	KeyCodes::KeyCode_MultiMedia,	KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_Blank,			KeyCodes::KeyCode_Blank,			KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_Blank,
	KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_MultiMedia,	KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_Keypad_Enter,	KeyCodes::KeyCode_RightCtrl,		KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_Blank,

	// 0xE0 20
	KeyCodes::KeyCode_MultiMedia,	KeyCodes::KeyCode_MultiMedia,	KeyCodes::KeyCode_MultiMedia,	KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_MultiMedia,		KeyCodes::KeyCode_Blank,			KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_Blank,
	KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_Blank,			KeyCodes::KeyCode_Blank,			KeyCodes::KeyCode_MultiMedia,	KeyCodes::KeyCode_Blank,

	// 0xE0 30
	KeyCodes::KeyCode_MultiMedia,	KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_MultiMedia,	KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_Blank,			KeyCodes::KeyCode_Keypad_Slash,	KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_PrintScreen,
	KeyCodes::KeyCode_RightAlt,	KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_Blank,			KeyCodes::KeyCode_Blank,			KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_Blank,

	// 0xE0 40
	KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_Blank,			KeyCodes::KeyCode_Blank,			KeyCodes::KeyCode_Break,		KeyCodes::KeyCode_Home,
	KeyCodes::KeyCode_Up,			KeyCodes::KeyCode_PageUp,		KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_Left,		KeyCodes::KeyCode_Blank,			KeyCodes::KeyCode_Right,			KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_End,

	// 0xE0 50
	KeyCodes::KeyCode_Down,		KeyCodes::KeyCode_PageDown,	KeyCodes::KeyCode_Insert,		KeyCodes::KeyCode_Delete,		KeyCodes::KeyCode_Blank,			KeyCodes::KeyCode_Blank,			KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_Blank,
	KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_LeftGUI,	KeyCodes::KeyCode_RightGUI,		KeyCodes::KeyCode_Apps,			KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_Blank,

	// 0xE0 60
	KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_Blank,			KeyCodes::KeyCode_Blank,			KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_Blank,
	KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_Blank,			KeyCodes::KeyCode_Blank,			KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_Blank,

	// 0xE0 70
	KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_Blank,			KeyCodes::KeyCode_Blank,			KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_Blank,
	KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_Blank,			KeyCodes::KeyCode_Blank,			KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_Blank,

	// 0xE0 80
	KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_Blank,			KeyCodes::KeyCode_Blank,			KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_Blank,
	KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_Blank,			KeyCodes::KeyCode_Blank,			KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_Blank,
};

const KeyCodes ScanCodeSet2[] =
{
	// 0x00
	KeyCodes::KeyCode_Blank,			KeyCodes::KeyCode_F9,				KeyCodes::KeyCode_Blank,				KeyCodes::KeyCode_F5,					KeyCodes::KeyCode_F3,					KeyCodes::KeyCode_F1,				KeyCodes::KeyCode_F2,			KeyCodes::KeyCode_F12,
	KeyCodes::KeyCode_Blank,			KeyCodes::KeyCode_F10,			KeyCodes::KeyCode_F8,					KeyCodes::KeyCode_F6,					KeyCodes::KeyCode_F4,					KeyCodes::KeyCode_Tab,			KeyCodes::KeyCode_Backtick,	KeyCodes::KeyCode_Blank,

	// 0x10
	KeyCodes::KeyCode_Blank,			KeyCodes::KeyCode_LeftAlt,		KeyCodes::KeyCode_LeftShift,			KeyCodes::KeyCode_Blank,				KeyCodes::KeyCode_LeftCtrl,			KeyCodes::KeyCode_q,				KeyCodes::KeyCode_1,			KeyCodes::KeyCode_Blank,
	KeyCodes::KeyCode_Blank,			KeyCodes::KeyCode_Blank,			KeyCodes::KeyCode_z,					KeyCodes::KeyCode_s,					KeyCodes::KeyCode_a,					KeyCodes::KeyCode_w,				KeyCodes::KeyCode_2,			KeyCodes::KeyCode_Blank,

	// 0x20
	KeyCodes::KeyCode_Blank,			KeyCodes::KeyCode_c,				KeyCodes::KeyCode_x,					KeyCodes::KeyCode_d,					KeyCodes::KeyCode_e,					KeyCodes::KeyCode_4,				KeyCodes::KeyCode_3,			KeyCodes::KeyCode_Blank,
	KeyCodes::KeyCode_Blank,			KeyCodes::KeyCode_Space,			KeyCodes::KeyCode_v,					KeyCodes::KeyCode_f,					KeyCodes::KeyCode_t,					KeyCodes::KeyCode_r,				KeyCodes::KeyCode_5,			KeyCodes::KeyCode_Blank,

	// 0x30
	KeyCodes::KeyCode_Blank,			KeyCodes::KeyCode_n,				KeyCodes::KeyCode_b,					KeyCodes::KeyCode_h,					KeyCodes::KeyCode_g,					KeyCodes::KeyCode_y,				KeyCodes::KeyCode_6,			KeyCodes::KeyCode_Blank,
	KeyCodes::KeyCode_Blank,			KeyCodes::KeyCode_Blank,			KeyCodes::KeyCode_m,					KeyCodes::KeyCode_j,					KeyCodes::KeyCode_u,					KeyCodes::KeyCode_7,				KeyCodes::KeyCode_8,			KeyCodes::KeyCode_Blank,

	// 0x40
	KeyCodes::KeyCode_Blank,			KeyCodes::KeyCode_Comma,			KeyCodes::KeyCode_k,					KeyCodes::KeyCode_i,					KeyCodes::KeyCode_o,					KeyCodes::KeyCode_0,				KeyCodes::KeyCode_9,			KeyCodes::KeyCode_Blank,
	KeyCodes::KeyCode_Blank,			KeyCodes::KeyCode_Period,			KeyCodes::KeyCode_Slash,				KeyCodes::KeyCode_l,					KeyCodes::KeyCode_Semicolon,			KeyCodes::KeyCode_p,				KeyCodes::KeyCode_Dash,		KeyCodes::KeyCode_Blank,

	// 0x50
	KeyCodes::KeyCode_Blank,			KeyCodes::KeyCode_Blank,			KeyCodes::KeyCode_Ampersand,			KeyCodes::KeyCode_Blank,				KeyCodes::KeyCode_LeftSquareBracket,	KeyCodes::KeyCode_Equals,			KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_Blank,
	KeyCodes::KeyCode_CapsLock,		KeyCodes::KeyCode_RightShift,		KeyCodes::KeyCode_Enter,				KeyCodes::KeyCode_RightSquareBracket,	KeyCodes::KeyCode_Blank,				KeyCodes::KeyCode_Backslash,		KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_Blank,

	// 0x60
	KeyCodes::KeyCode_Blank,			KeyCodes::KeyCode_Blank,			KeyCodes::KeyCode_Blank,				KeyCodes::KeyCode_Blank,				KeyCodes::KeyCode_Blank,				KeyCodes::KeyCode_Blank,			KeyCodes::KeyCode_Backspace,	KeyCodes::KeyCode_Blank,
	KeyCodes::KeyCode_Blank,			KeyCodes::KeyCode_Keypad_End,		KeyCodes::KeyCode_Blank,				KeyCodes::KeyCode_Keypad_Left,		KeyCodes::KeyCode_Keypad_Home,		KeyCodes::KeyCode_Blank,			KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_Blank,

	// 0x70
	KeyCodes::KeyCode_Keypad_Insert,	KeyCodes::KeyCode_Keypad_Delete,	KeyCodes::KeyCode_Keypad_Down,		KeyCodes::KeyCode_Keypad_Middle,		KeyCodes::KeyCode_Keypad_Right,		KeyCodes::KeyCode_Keypad_Up,		KeyCodes::KeyCode_Escape,		KeyCodes::KeyCode_NumberLock,
	KeyCodes::KeyCode_F11,			KeyCodes::KeyCode_Keypad_Plus,	KeyCodes::KeyCode_Keypad_PageDown,	KeyCodes::KeyCode_Keypad_Dash,		KeyCodes::KeyCode_Keypad_Star,		KeyCodes::KeyCode_Keypad_PageUp,	KeyCodes::KeyCode_ScrollLock,	KeyCodes::KeyCode_Blank,

	// 0x80
	KeyCodes::KeyCode_Blank,			KeyCodes::KeyCode_Blank,			KeyCodes::KeyCode_Blank,				KeyCodes::KeyCode_F7,					KeyCodes::KeyCode_SysRequest,			KeyCodes::KeyCode_Blank,			KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_Blank,
	KeyCodes::KeyCode_Blank,			KeyCodes::KeyCode_Blank,			KeyCodes::KeyCode_Blank,				KeyCodes::KeyCode_Blank,				KeyCodes::KeyCode_Blank,				KeyCodes::KeyCode_Blank,			KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_Blank,
};

const KeyCodes ScanCodeSet2E0[] =
{
	// 0x00
	KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_Blank,			KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_Blank,	KeyCodes::KeyCode_Blank,
	KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_Blank,			KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_Blank,	KeyCodes::KeyCode_Blank,

	// 0x10
	KeyCodes::KeyCode_MultiMedia,	KeyCodes::KeyCode_RightAlt,	KeyCodes::KeyCode_RightShift,		KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_RightCtrl,	KeyCodes::KeyCode_MultiMedia,	KeyCodes::KeyCode_Blank,	KeyCodes::KeyCode_Blank,
	KeyCodes::KeyCode_MultiMedia,	KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_Blank,			KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_Blank,	KeyCodes::KeyCode_LeftGUI,

	// 0x20
	KeyCodes::KeyCode_MultiMedia,	KeyCodes::KeyCode_MultiMedia,	KeyCodes::KeyCode_Blank,			KeyCodes::KeyCode_MultiMedia,	KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_Blank,	KeyCodes::KeyCode_RightGUI,
	KeyCodes::KeyCode_MultiMedia,	KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_Blank,			KeyCodes::KeyCode_MultiMedia,	KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_Blank,	KeyCodes::KeyCode_Apps,

	// 0x30
	KeyCodes::KeyCode_MultiMedia,	KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_MultiMedia,		KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_MultiMedia,	KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_Blank,	KeyCodes::KeyCode_Blank,
	KeyCodes::KeyCode_MultiMedia,	KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_MultiMedia,		KeyCodes::KeyCode_MultiMedia,	KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_Blank,	KeyCodes::KeyCode_Blank,

	// 0x40
	KeyCodes::KeyCode_MultiMedia,	KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_Blank,			KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_Blank,	KeyCodes::KeyCode_Blank,
	KeyCodes::KeyCode_MultiMedia,	KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_Keypad_Slash,	KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_MultiMedia,	KeyCodes::KeyCode_Blank,	KeyCodes::KeyCode_Blank,

	// 0x50
	KeyCodes::KeyCode_MultiMedia,	KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_Blank,			KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_Blank,	KeyCodes::KeyCode_Blank,
	KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_Keypad_Enter,	KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_Blank,	KeyCodes::KeyCode_Blank,

	// 0x60
	KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_Blank,			KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_Blank,	KeyCodes::KeyCode_Blank,
	KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_End,		KeyCodes::KeyCode_Blank,			KeyCodes::KeyCode_Left,		KeyCodes::KeyCode_Home,		KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_Blank,	KeyCodes::KeyCode_Blank,

	// 0x70
	KeyCodes::KeyCode_Insert,		KeyCodes::KeyCode_Delete,		KeyCodes::KeyCode_Down,			KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_Right,		KeyCodes::KeyCode_Up,			KeyCodes::KeyCode_Blank,	KeyCodes::KeyCode_Pause,
	KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_PageDown,		KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_PrintScreen,KeyCodes::KeyCode_PageUp,		KeyCodes::KeyCode_Break,	KeyCodes::KeyCode_Blank,

	// 0x80
	KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_Blank,			KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_Blank,	KeyCodes::KeyCode_Blank,
	KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_Blank,			KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_Blank,		KeyCodes::KeyCode_Blank,	KeyCodes::KeyCode_Blank,
};

