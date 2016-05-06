#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <atomic>
#include <new>

#include "RawTerminal.h"
#include "RawMemory.h"
#include "MultiBootInfo.h"
#include "MultiBoot.h"

#include "LowLevel.h"
#include "InterruptControler.h"
#include "Stack.h"

#include "GDT.h"
#include "IDT.h"
#include "MMU.h"

#include "ACPI.h"
#include "PCI.h"
#include "IDE.h"
#include "AHCI.h"

#include "OpenHCI.h"
#include "Thread.h"
#include "MemoryMap.h"

#include "CoreComplex.h"
#include "KernalLib.h"

#include "PEData.h"

#include "..\StdLib\argcargv.h"
#include "..\StdLib\initterm.h"

void main(int argc, char *argv[]);
extern MMU * MMUManager;
extern OpenHCI * USBManager;

void PrintContext(volatile InterruptContext * Context)
{
	KernalPrintf(" %08X ES:%04X DS:%04X FS:%04X GS:%04X - %08X\n", &Context->Segments.ES, Context->Segments.ES & 0xFFFF, Context->Segments.DS & 0xFFFF, Context->Segments.FS & 0xFFFF, Context->Segments.GS & 0xFFFF, &Context->Segments.GS);
	KernalPrintf(" %08X EDI:%08X ESI:%08X EBP:%08X ESP:%08X - %08X\n", &Context->Registers.EDI, Context->Registers.EDI, Context->Registers.ESI, Context->Registers.EBP, Context->Registers.ESP, &Context->Registers.ESP);
	KernalPrintf(" %08X EBX:%08X EDX:%08X ECX:%08X EAX:%08X - %08X\n", &Context->Registers.EBX, Context->Registers.EBX, Context->Registers.EDX, Context->Registers.ECX, Context->Registers.EAX, &Context->Registers.EAX);
	KernalPrintf(" %08X Int: %02X, Error: %08X - %08X\n", &Context->InterruptNumber, Context->InterruptNumber, Context->ErrorCode, &Context->ErrorCode);
	KernalPrintf(" %08X EIP:%08X CS:%04X EFLAGS:%08X - %08X\n", &Context->Origin.Return.Address, Context->Origin.Return.Address, Context->Origin.Return.Selector & 0xFFFF, Context->Origin.EFlags, &Context->Origin.EFlags);
	KernalPrintf(" %08X ESP:%08X SS:%04X - %08X\n", &Context->Origin.Stack.Address, Context->Origin.Stack.Address, Context->Origin.Stack.Selector & 0xFFFF, &Context->Origin.Stack.Selector);
	KernalPrintf(" %08X ES:%04X DS:%04X FS:%04X GS:%04X - %08X\n", &Context->Origin.V86Segments.ES, Context->Origin.V86Segments.ES & 0xFFFF, Context->Origin.V86Segments.DS & 0xFFFF, Context->Origin.V86Segments.FS & 0xFFFF, Context->Origin.V86Segments.GS & 0xFFFF, &Context->Origin.V86Segments.GS);

}

void SystemTrap(volatile InterruptContext * Context, uintptr_t * Data)
{
	if(Context->InterruptNumber == 0x0d)
	{
		// Bad MSRs through GPFs, so eat them and move on.
		// 0F 30 WRMSR
		// 0F 32 RDMSR
		if(reinterpret_cast<uint16_t *>(Context->Origin.Return.Address)[0] == 0x300F || reinterpret_cast<uint16_t *>(Context->Origin.Return.Address)[0] == 0x320F)
		{
			//KernalPrintf("\nInvalid MSR [%X]\n", Context->Registers.ECX);
			Context->Registers.EAX = 0x00000000;
			Context->Registers.EDX = 0x00000000;
			Context->Origin.EFlags = Context->Origin.EFlags & ~EFlags::Carry;
			Context->Origin.Return.Address += 2;
			return;
		}
		else if(Context->Origin.EFlags & EFlags::Virtual8086Mode)
		{	
			// Check for a simple segement overflow and return
			if(Context->Origin.Return.Address == 0x10000)
				return;

			//KernalPrintf(" %08X %08X %08X\n", Context, Data, &Context);
			//PrintContext(Context);
			
			uint32_t * Stack = reinterpret_cast<uint32_t *>((Context->Origin.Stack.Selector << 4) + 0xFF00);
			uint8_t * Memory = reinterpret_cast<uint8_t *>((Context->Origin.Return.Selector << 4) + Context->Origin.Return.Address);

			bool Address32 = false;
			bool Opcode32 = false;
			
			int Index = 0;
			bool Done = false;
			while(!Done)
			{
				switch(Memory[Index])
				{
					// OP Size Prefix
					case 0x66:
						Opcode32 = true;
						break;

					// Address size Prefix
					case 0x67:
						Address32 = true;
						break;				

					//// REPNE
					//case 0xF3:
					//	break;

					//// REP
					//case 0xf4:
					//	break;
					//
					//case 0xF0:	// LOCK
					//case 0x26:	// ES
					//case 0x2E:	// CS
					//case 0x36:	// SS
					//case 0x3E:	// DS
					//case 0x64:	// FS
					//case 0x65:	// GS
					//	break;
					//					
					//case 0xCC:	// INT 3
					//case 0xCD:	// Int Immediate Byte
					//case 0xCE:	// INTO (int 4)
					//	break;

					//// IRET
					//case 0xCF:
					//	break;

					//// Pushf
					//case 0x9C:
					//	break;

					//// PopF
					//case 0x9D:
					//	break;

					//// CLI
					//case 0xFA:
					//	break;

					//// STI
					//case 0xFB:
					//	break;

					//// IN Immediate Byte
					//case 0xE4:
					//	break;

					//// IN Immediate Byte
					//case 0xE5:
					//	break;

					//// IN No Param
					//case 0xEC:	
					//	break;
					//
					//// IN No Param
					//case 0xED:
					//	break;
					//
					//// OUT Immediate Byte
					//case 0xE6:
					//	break;

					//// OUT Immediate Byte
					//case 0xE7:
					//	break;

					//// OUT No Param
					//case 0xEE:
					//	break;

					//// OUT No Param
					//case 0xEF:
					//	break;

					//// INS No Param
					//case 0x6C: 
					//	break;

					//// INS No Param
					//case 0x6D:
					//	break;

					//// OUTS No Param
					//case 0x6E:
					//	break;

					//// OUTS No Param
					//case 0x6F:
					//	break;

					default:
						KernalPrintf("\nGPF Fault %08X-%02X at: %04X:%04X (%08X) in Virtual Monitor\n", Context->ErrorCode, Memory[Index], Context->Origin.Return.Selector, Context->Origin.Return.Address + Index, &Memory[Index]);
						KernalPrintf(" v86 Stack: %04X:%04X (%08X) %08X, %08X, %08X\n", Context->Origin.Stack.Selector, Context->Origin.Stack.Address, &Stack[0], Stack[-2], Stack[-4], Stack[-6]);
						Done = true;
						break;
				}

				Index++;
			}
			//Context->Origin.Return.Address += Index;

			// Return from the v86 mode
			Context->Origin.EFlags = (Context->Origin.EFlags & ~EFlags::Virtual8086Mode) | 0x3000 | EFlags::InterruptEnable;
			Context->Origin.Stack.Selector = Stack[-3];
			Context->Origin.Stack.Address = Stack[-4];

			Context->Origin.Return.Selector = Stack[-5];
			Context->Origin.Return.Address = Stack[-6];

			Context->Segments.DS = Stack[-3];
			//Context->Segments.FS = 0x53; // Stack[-3];
			//Context->Segments.GS = 0x4B; // Stack[-3];
		}
		else
		{
			KernalPrintf("\nGPF Fault %08X at: %04X:%08X. %08X\n", Context->ErrorCode, Context->Origin.Return.Selector, Context->Origin.Return.Address, Context->Origin.EFlags);
		}
	}
	else if(Context->InterruptNumber == 0x0e)
	{
		KernalPrintf("\nPage Fault from: %08X\n", Context->Origin.Return.Address);
		uint32_t CR2 = ReadCR2();
		KernalPrintf("Address: %08X\n", CR2);

		if(Context->ErrorCode & 0x01)
			KernalPrintf("page protection, ");
		else
			KernalPrintf("non-present page, ");

		if(Context->ErrorCode & 0x02)
			KernalPrintf("write, ");
		else
			KernalPrintf("read, ");

		if(Context->ErrorCode & 0x04)
			KernalPrintf("user");
		else
			KernalPrintf("supervisor");

		if(Context->ErrorCode & 0x08)
			KernalPrintf(", reserved bit");

		if(Context->ErrorCode & 0x10)
			KernalPrintf(", instruction fetch");

		KernalPrintf(" (%08X)\n", Context->ErrorCode);

		MMUManager->PrintAddressInformation(CR2);
	}
	else if(Context->InterruptNumber == 0x12)
	{
		KernalPrintf("\nMachine Check at %08X\n", Context->Origin.Return.Address);
		uint64_t Error;
		
		Error = ReadMSR(0x17A); // IA32_MCG_STATUS
		if(Error & 0x01)
			return;
	}
	else
	{	
		KernalPrintf("\nSystem Error: %02X (%08X) at %04X:%08X\n", Context->InterruptNumber, Context->ErrorCode, Context->Origin.Return.Selector, Context->Origin.Return.Address);
	};

	PrintContext(Context);
	
	ASM_CLI;

	for(;;) ASM_HLT;
}

// Displayable characters will map to their ANSII key codes, none display characters that dosn't have a matching ANSII code will get codes with the high bit set.
enum class KeyCodes : uint8_t
{	
	Blank				= 0x00,

	Backspace			= 0x08,
	Tab,
	Enter,

	Escape				= 0x1B,

	Space				= 0x20,
	Exclimation,
	DoubleQuote,
	Hash,
	DollarSign,
	Percent,
	Ampersand,
	SingleQuote,
	LeftParentheses,
	RightParentheses,
	Star,
	Plus,
	Comma,
	Dash,
	Period,
	Slash,

	_0					= 0x30,
	_1,
	_2,
	_3,
	_4,
	_5,
	_6,
	_7,
	_8,
	_9,
	Colon,
	Semicolon,
	LessThen,
	Equals,
	GreaterThen,
	QuestionMark,

	AtSign				= 0x40,
	_a,
	_b,
	_c,
	_d,
	_e,
	_f,
	_g,
	_h,
	_i,
	_j,
	_k,
	_l,
	_m,
	_n,
	_o,
	_p,
	_q,
	_r,
	_s,
	_t,
	_u,
	_v,
	_w,
	_x,
	_y,
	_z,	
	LeftBracket,
	BackSlash,
	RightBracket,
	Carrot,
	UnderLine,

	ReverseQuote		= 0x60,
	_A,
	_B,
	_C,
	_D,
	_E,
	_F,
	_G,
	_H,
	_I,
	_J,
	_K,
	_L,
	_M,
	_N,
	_O,
	_P,
	_Q,
	_R,
	_S,
	_T,
	_U,
	_V,
	_W,
	_X,
	_Y,
	_Z,	
	LeftCurlyBrace,
	Pipe,
	RightCurlyBrace,
	Tilde,
	Delete,
	
	Extended_1			= 0x81, // Extended codes
	
	// F keys
	F1					= 0x91,
	F2,
	F3,
	F4,
	F5,
	F6,
	F7,
	F8,
	F9,
	F10,
	F11,
	F12,
	F13,
	F14,
	F15,
	
	Insert,
	PrintScreen,
	Pause,
	Break,

	// Movement
	Up					= 0xA0,
	Down,
	Left,
	Right,
	PageUp,
	PageDown,
	Home,
	End,

	// Modifiers (LSB = Right)
	LeftShift			= 0xB0,
	RightShift,
	LeftCtrl			= 0xB2,
	RightCtrl,
	LeftAlt				= 0xB4,
	RightAlt,
	LeftGUI				= 0xB6,
	RightGUI,
	
	Keypad_0			= 0xC0,		// 0 / Insert
	Keypad_1,						// 1 / End
	Keypad_2,						// 2 / Down
	Keypad_3,						// 3 / PageDown
	Keypad_4,						// 4 / Left 
	Keypad_5,						// 5 / Blank
	Keypad_6,						// 6 / Right
	Keypad_7,						// 7 / Home
	Keypad_8,						// 8 / Up
	Keypad_9,						// 9 / PageUp
	Keypad_Period,
	Keypad_Slash,
	Keypad_Star,
	Keypad_Dash,
	Keypad_Plus,
	Keypad_Enter,

	
	CapsLock			= 0xF0,
	NumberLock,
	ScrollLock,
	Apps,
	Power,
	Sleep,
	Wake,
	SysRequest			= 0xFF
};

KeyCodes kbdus2[] =
{
//	x0						x1						x2						x3							x4						x5						x6						x7						
//	x8						x9						xA						xB							xC						xD						xE						xF
	KeyCodes::Blank,		KeyCodes::F9,			KeyCodes::Blank,		KeyCodes::F5,				KeyCodes::F3,			KeyCodes::F1,			KeyCodes::F2,			KeyCodes::F12,			// 00	
	KeyCodes::Blank,		KeyCodes::F10,			KeyCodes::F8,			KeyCodes::F6,				KeyCodes::F4,			KeyCodes::Tab,			KeyCodes::Tilde,		KeyCodes::Blank,		// 08
	KeyCodes::Blank,		KeyCodes::LeftAlt,		KeyCodes::LeftShift,	KeyCodes::Blank,			KeyCodes::LeftCtrl,		KeyCodes::_q,			KeyCodes::_1,			KeyCodes::Blank,		// 10
	KeyCodes::Blank,		KeyCodes::Blank,		KeyCodes::_z,			KeyCodes::_s,				KeyCodes::_a,			KeyCodes::_w,			KeyCodes::_2,			KeyCodes::Blank,		// 18
	KeyCodes::Blank,		KeyCodes::_c,			KeyCodes::_x,			KeyCodes::_d,				KeyCodes::_e,			KeyCodes::_4,			KeyCodes::_3,			KeyCodes::Blank,		// 20	
	KeyCodes::Blank,		KeyCodes::Space,		KeyCodes::_v,			KeyCodes::_f,				KeyCodes::_t,			KeyCodes::_r,			KeyCodes::_5,			KeyCodes::Blank,		// 28
	KeyCodes::Blank,		KeyCodes::_n,			KeyCodes::_b,			KeyCodes::_h,				KeyCodes::_g,			KeyCodes::_y,			KeyCodes::_6,			KeyCodes::Blank,		// 30
	KeyCodes::Blank,		KeyCodes::Blank,		KeyCodes::_m,			KeyCodes::_j,				KeyCodes::_u,			KeyCodes::_7,			KeyCodes::_8,			KeyCodes::Blank,		// 38
	KeyCodes::Blank,		KeyCodes::Comma,		KeyCodes::_k,			KeyCodes::_i,				KeyCodes::_o,			KeyCodes::_0,			KeyCodes::_9,			KeyCodes::Blank,		// 40
	KeyCodes::Blank,		KeyCodes::Period,		KeyCodes::Slash,		KeyCodes::_l,				KeyCodes::Semicolon,	KeyCodes::_p,			KeyCodes::Dash,			KeyCodes::Blank,		// 48
	KeyCodes::Blank,		KeyCodes::Blank,		KeyCodes::SingleQuote,	KeyCodes::Blank,			KeyCodes::LeftBracket,	KeyCodes::Equals,		KeyCodes::Blank,		KeyCodes::Blank,		// 50
	KeyCodes::CapsLock,		KeyCodes::RightShift,	KeyCodes::Enter,		KeyCodes::RightBracket,		KeyCodes::Blank,		KeyCodes::BackSlash,	KeyCodes::Blank,		KeyCodes::Blank,		// 58
	KeyCodes::Blank,		KeyCodes::Blank,		KeyCodes::Blank,		KeyCodes::Blank,			KeyCodes::Blank,		KeyCodes::Blank,		KeyCodes::Backspace,	KeyCodes::Blank,		// 60
	KeyCodes::Blank,		KeyCodes::Keypad_1,		KeyCodes::Blank,		KeyCodes::Keypad_4,			KeyCodes::Keypad_7,		KeyCodes::Blank,		KeyCodes::Blank,		KeyCodes::Blank,		// 68
	KeyCodes::Keypad_0,		KeyCodes::Keypad_Period,KeyCodes::Keypad_2,		KeyCodes::Keypad_5,			KeyCodes::Keypad_6,		KeyCodes::Keypad_8,		KeyCodes::Escape,		KeyCodes::NumberLock,	// 70
	KeyCodes::F11,			KeyCodes::Keypad_Plus,	KeyCodes::Keypad_3,		KeyCodes::Keypad_Dash,		KeyCodes::Keypad_Star,	KeyCodes::Keypad_9,		KeyCodes::ScrollLock,	KeyCodes::Blank,		// 78

	KeyCodes::Blank,		KeyCodes::Blank,		KeyCodes::Blank,		KeyCodes::F7,				KeyCodes::SysRequest,	KeyCodes::Blank,		KeyCodes::Blank,		KeyCodes::Blank,		// 80
};

KeyCodes kbdusShift[] =
{
//	x0						x1						x2						x3							x4						x5						x6						x7						
//	x8						x9						xA						xB							xC						xD						xE						xF
	KeyCodes::Blank,		KeyCodes::F9,			KeyCodes::Blank,		KeyCodes::F5,				KeyCodes::F3,			KeyCodes::F1,			KeyCodes::F2,			KeyCodes::F12,			// 00	
	KeyCodes::Blank,		KeyCodes::F10,			KeyCodes::F8,			KeyCodes::F6,				KeyCodes::F4,			KeyCodes::Tab,			KeyCodes::ReverseQuote,	KeyCodes::Blank,		// 08
	KeyCodes::Blank,		KeyCodes::LeftAlt,		KeyCodes::LeftShift,	KeyCodes::Blank,			KeyCodes::LeftCtrl,		KeyCodes::_Q,			KeyCodes::Exclimation,	KeyCodes::Blank,		// 10
	KeyCodes::Blank,		KeyCodes::Blank,		KeyCodes::_Z,			KeyCodes::_S,				KeyCodes::_A,			KeyCodes::_W,			KeyCodes::AtSign,		KeyCodes::Blank,		// 18
	KeyCodes::Blank,		KeyCodes::_C,			KeyCodes::_X,			KeyCodes::_D,				KeyCodes::_E,			KeyCodes::DollarSign,	KeyCodes::Hash,			KeyCodes::Blank,		// 20	
	KeyCodes::Blank,		KeyCodes::Space,		KeyCodes::_V,			KeyCodes::_F,				KeyCodes::_T,			KeyCodes::_R,			KeyCodes::Percent,		KeyCodes::Blank,		// 28
	KeyCodes::Blank,		KeyCodes::_N,			KeyCodes::_B,			KeyCodes::_H,				KeyCodes::_G,			KeyCodes::_Y,			KeyCodes::Carrot,		KeyCodes::Blank,		// 30
	KeyCodes::Blank,		KeyCodes::Blank,		KeyCodes::_M,			KeyCodes::_J,				KeyCodes::_U,			KeyCodes::Ampersand,	KeyCodes::Star,			KeyCodes::Blank,		// 38
	KeyCodes::Blank,		KeyCodes::LessThen,		KeyCodes::_K,			KeyCodes::_I,				KeyCodes::_O,			KeyCodes::RightParentheses,	KeyCodes::LeftParentheses,	KeyCodes::Blank,// 40
	KeyCodes::Blank,		KeyCodes::GreaterThen,	KeyCodes::QuestionMark,	KeyCodes::_L,				KeyCodes::Colon,		KeyCodes::_P,			KeyCodes::UnderLine,	KeyCodes::Blank,		// 48
	KeyCodes::Blank,		KeyCodes::Blank,		KeyCodes::DoubleQuote,	KeyCodes::Blank,			KeyCodes::LeftCurlyBrace,	KeyCodes::Plus,		KeyCodes::Blank,		KeyCodes::Blank,		// 50
	KeyCodes::CapsLock,		KeyCodes::RightShift,	KeyCodes::Enter,		KeyCodes::RightCurlyBrace,	KeyCodes::Blank,		KeyCodes::Pipe,			KeyCodes::Blank,		KeyCodes::Blank,		// 58
	KeyCodes::Blank,		KeyCodes::Blank,		KeyCodes::Blank,		KeyCodes::Blank,			KeyCodes::Blank,		KeyCodes::Blank,		KeyCodes::Backspace,	KeyCodes::Blank,		// 60
	KeyCodes::Blank,		KeyCodes::Keypad_1,		KeyCodes::Blank,		KeyCodes::Keypad_4,			KeyCodes::Keypad_7,		KeyCodes::Blank,		KeyCodes::Blank,		KeyCodes::Blank,		// 68
	KeyCodes::Keypad_0,		KeyCodes::Keypad_Period,KeyCodes::Keypad_2,		KeyCodes::Keypad_5,			KeyCodes::Keypad_6,		KeyCodes::Keypad_8,		KeyCodes::Escape,		KeyCodes::NumberLock,	// 70
	KeyCodes::F11,			KeyCodes::Keypad_Plus,	KeyCodes::Keypad_3,		KeyCodes::Keypad_Dash,		KeyCodes::Keypad_Star,	KeyCodes::Keypad_9,		KeyCodes::ScrollLock,	KeyCodes::Blank,		// 78

	KeyCodes::Blank,		KeyCodes::Blank,		KeyCodes::Blank,		KeyCodes::F7,				KeyCodes::SysRequest,	KeyCodes::Blank,		KeyCodes::Blank,		KeyCodes::Blank,		// 80
};

KeyCodes kbdusE0[] =
{
//	x0						x1						x2						x3							x4						x5						x6						x7						
//	x8						x9						xA						xB							xC						xD						xE						xF
	KeyCodes::Blank,		KeyCodes::Blank,		KeyCodes::Blank,		KeyCodes::Blank,			KeyCodes::Blank,		KeyCodes::Blank,		KeyCodes::Blank,		KeyCodes::Blank,		// 00
	KeyCodes::Blank,		KeyCodes::Blank,		KeyCodes::Blank,		KeyCodes::Blank,			KeyCodes::Blank,		KeyCodes::Blank,		KeyCodes::Blank,		KeyCodes::Blank,		// 08
	KeyCodes::Blank,		KeyCodes::RightAlt,		KeyCodes::Blank,		KeyCodes::Blank,			KeyCodes::RightCtrl,	KeyCodes::Blank,		KeyCodes::Blank,		KeyCodes::Blank,		// 10
	KeyCodes::Blank,		KeyCodes::Blank,		KeyCodes::Blank,		KeyCodes::Blank,			KeyCodes::Blank,		KeyCodes::Blank,		KeyCodes::Blank,		KeyCodes::LeftGUI,		// 18
	KeyCodes::Blank,		KeyCodes::Blank,		KeyCodes::Blank,		KeyCodes::Blank,			KeyCodes::Blank,		KeyCodes::Blank,		KeyCodes::Blank,		KeyCodes::RightGUI,		// 20
	KeyCodes::Blank,		KeyCodes::Blank,		KeyCodes::Blank,		KeyCodes::Blank,			KeyCodes::Blank,		KeyCodes::Blank,		KeyCodes::Blank,		KeyCodes::Apps,			// 28
	KeyCodes::Blank,		KeyCodes::Blank,		KeyCodes::Blank,		KeyCodes::Blank,			KeyCodes::Blank,		KeyCodes::Blank,		KeyCodes::Blank,		KeyCodes::Power,		// 30
	KeyCodes::Blank,		KeyCodes::Blank,		KeyCodes::Blank,		KeyCodes::Blank,			KeyCodes::Blank,		KeyCodes::Blank,		KeyCodes::Blank,		KeyCodes::Sleep,		// 38
	KeyCodes::Blank,		KeyCodes::Blank,		KeyCodes::Blank,		KeyCodes::Blank,			KeyCodes::Blank,		KeyCodes::Blank,		KeyCodes::Blank,		KeyCodes::Blank,		// 40
	KeyCodes::Blank,		KeyCodes::Blank,		KeyCodes::Keypad_Slash,	KeyCodes::Blank,			KeyCodes::Blank,		KeyCodes::Blank,		KeyCodes::Blank,		KeyCodes::Blank,		// 48
	KeyCodes::Blank,		KeyCodes::Blank,		KeyCodes::Blank,		KeyCodes::Blank,			KeyCodes::Blank,		KeyCodes::Blank,		KeyCodes::Blank,		KeyCodes::Blank,		// 50
	KeyCodes::Blank,		KeyCodes::Blank,		KeyCodes::Keypad_Enter,	KeyCodes::Blank,			KeyCodes::Blank,		KeyCodes::Blank,		KeyCodes::Wake,			KeyCodes::Blank,		// 58
	KeyCodes::Blank,		KeyCodes::Blank,		KeyCodes::Blank,		KeyCodes::Blank,			KeyCodes::Blank,		KeyCodes::Blank,		KeyCodes::Blank,		KeyCodes::Blank,		// 60
	KeyCodes::Blank,		KeyCodes::End,			KeyCodes::Blank,		KeyCodes::Left,				KeyCodes::Home,			KeyCodes::Blank,		KeyCodes::Blank,		KeyCodes::Blank,		// 68
	KeyCodes::Insert,		KeyCodes::Delete,		KeyCodes::Down,			KeyCodes::Blank,			KeyCodes::Right,		KeyCodes::Up,			KeyCodes::Blank,		KeyCodes::Blank,		// 70
	KeyCodes::Blank,		KeyCodes::Blank,		KeyCodes::PageDown,		KeyCodes::Blank,			KeyCodes::PrintScreen,	KeyCodes::PageUp,		KeyCodes::Break,		KeyCodes::Blank,		// 78

	KeyCodes::Blank,		KeyCodes::Blank,		KeyCodes::Blank,		KeyCodes::Blank,			KeyCodes::Blank,		KeyCodes::Blank,		KeyCodes::Blank,		KeyCodes::Blank,		// 80
};


uint8_t kbdus[128] =
{
//	x0		x1		x2		x3		x4		x5		x6		x7		x8		x9		xA		xB		xC		xD		xE		xF
	0,		27,		'1',	'2',	'3',	'4',	'5',	'6',	'7',	'8',	'9',	'0',	'-',	'=',	'\b',	'\t',	// 0x
	'q',	'w',	'e',	'r',	't',	'y',	'u',	'i',	'o',	'p',	'[',	']',	'\n',	0,		'a',	's',	// 1x
	'd',	'f',	'g',	'h',	'j',	'k',	'l',	';',	'\'',	'`',	0,		'\\',	'z',	'x',	'c',	'v',	// 2x
	'b',	'n',	'm',	',',	'.',	'/',	0,		'*',	0,		' ',	0,		0,		0,		0,		0,		0,		// 3x
	0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		'-',	0,		0,		0,		'+',	0,		// 4x
	0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		// 5x
	0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		// 6x
	0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		// 7x
};

uint8_t kbdus_Shift[128] =
{
//	x0		x1		x2		x3		x4		x5		x6		x7		x8		x9		xA		xB		xC		xD		xE		xF
	0,		27,		'!',	'@',	'#',	'$',	'%',	'^',	'&',	'*',	'(',	')',	'_',	'+',	'\b',	'\t',	// 0x
	'Q',	'W',	'E',	'R',	'T',	'Y',	'U',	'I',	'O',	'P',	'{',	'}',	'\n',	0,		'A',	'S',	// 1x
	'D',	'F',	'G',	'H',	'J',	'K',	'L',	':',	'\"',	'~',	0,		'|',	'Z',	'X',	'C',	'V',	// 2x
	'B',	'N',	'M',	'<',	'>',	'?',	0,		'*',	0,		' ',	0,		0,		0,		0,		0,		0,		// 3x
	0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		'-',	0,		0,		0,		'+',	0,		// 4x
	0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		// 5x
	0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		// 6x
	0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		// 7x
};


InterruptControler m_InterruptControler;

volatile char KBBuffer[32];
volatile uint8_t KBBufferFirst = 0;
volatile uint8_t KBBufferLast = 0;

uint16_t KeyState = 0;

#define LeftShift	 0x01
#define RightShift	 0x02
#define LeftCtrl	 0x04
#define RightCtrl	 0x08
#define LeftAlt		 0x10
#define RightAlt	 0x20
#define LeftGUI		 0x40
#define RightGUI	 0x80

#define NumLock		0x100
#define	ScrollLock	0x200
#define CapsLock	0x400
#define EchoLock	0x800

void InsertKeyboardBuffer(char Key)
{
	if((KBBufferLast + 1) == KBBufferFirst || KBBufferFirst == 0 && (KBBufferLast + 1 == 32))
	{
		//printf("Full!");
	}
	else if(Key != 0)
	{
		KBBuffer[KBBufferLast] = Key;
		KBBufferLast++;
				
		if(KBBufferLast == 32)
			KBBufferLast = 0;
	}
}

char FetchKeyboardBuffer()
{
	while(KBBufferFirst == KBBufferLast)
	{
		ASM_HLT;
	}

	char value = KBBuffer[KBBufferFirst];
		
	KBBufferFirst++;
	if(KBBufferFirst == 32)
		KBBufferFirst = 0;

	return value;
}

void KeyboardInterrupt(InterruptContext * OldContext, uintptr_t * Data)
{
	unsigned char scancode = InPortb(0x60);		
	unsigned char scancode2 = 0;
	int Escape = 0;

	if(KeyState & EchoLock)
		KernalPrintf("[%02X", scancode);

	if(scancode == 0xE0)
	{
		Escape = 1;
		scancode = InPortb(0x60);

		if(KeyState & EchoLock)
			KernalPrintf(" %02X", scancode);
	}
	else if(scancode == 0xE1)
	{
		Escape = 2;
		scancode = InPortb(0x60);
		scancode2 = InPortb(0x60);

		if(KeyState & EchoLock)
			KernalPrintf(" %02X %02X", scancode, scancode2);
	}

	if(KeyState & EchoLock)
		KernalPrintf("] ");

	bool KeyUp = ((scancode & 0x80) == 0x80);
	scancode = scancode & 0x7F;

	if(KeyUp && scancode == 0x0E && (KeyState & LeftCtrl) && (KeyState & LeftAlt)) // CTRL-ALT-Backspace
	{
		KeyState ^= EchoLock;
	}

	if(scancode == 0x2A && Escape == 0) // LSHIFT
	{
		if(KeyUp)
			KeyState &= ~LeftShift;
			
		else
			KeyState |= LeftShift;
	}
	else if(scancode == 0x36 && Escape == 0) // RSHIFT
	{
		if(KeyUp)
			KeyState &= ~RightShift;
			
		else
			KeyState |= RightShift;
	}
	else if(scancode == 0x1D && Escape == 0) // LCTRL
	{
		if(KeyUp)
			KeyState &= ~LeftCtrl;
			
		else
			KeyState |= LeftCtrl;
	}
	else if(scancode == 0x1D && Escape == 1) // RCTRL
	{
		if(KeyUp)
			KeyState &= ~RightCtrl;
			
		else
			KeyState |= RightCtrl;
	}

	else if(scancode == 0x38 && Escape == 0) // LALT
	{
		if(KeyUp)
			KeyState &= ~LeftAlt;
			
		else
			KeyState |= LeftAlt;
	}
	else if(scancode == 0x38 && Escape == 1) // RALT
	{
		if(KeyUp)
			KeyState &= ~RightAlt;
			
		else
			KeyState |= RightAlt;
	}
	else if(!KeyUp && Escape == 0)
	{
		char Key = 0;	
		if(KeyState & LeftShift || KeyState & RightShift)
			Key = kbdus_Shift[scancode];
		else
			Key = kbdus[scancode];

		InsertKeyboardBuffer(Key);
	}
}

ThreadInformation *ThreadListHead;

void IdleThread(ThreadInformation *Thread,  uintptr_t * Data)
{
	for(;;) ASM_HLT;
}

ThreadInformation *FindNextThread(ThreadInformation *CurrentThread)
{
	ThreadInformation *NextThread = CurrentThread->Next;
	if(NextThread == nullptr)
		NextThread = ThreadListHead;

	while(NextThread != CurrentThread)
	{
		if(NextThread->Suspended.load() == false)
			break;

		NextThread = NextThread->Next;

		if(NextThread == nullptr)
			NextThread = ThreadListHead;
	}

	return NextThread;
}

#pragma warning(push)
// This turns off the warning: 
//       warning C4731: 'ClockInterrupt' : frame pointer register 'ebp' modified by inline assembly code
// Because that's the point in this case. 
#pragma warning(disable: 4731)

void ClockInterrupt(volatile InterruptContext * OldContext, uintptr_t * Data)
{
	ThreadInformation *CurrentThread = reinterpret_cast<ThreadInformation *>(ReadFS(8));
	
	CurrentThread->TickCount++;
	if(CurrentThread->TimeSliceCount >= CurrentThread->TimeSliceAllocation)
	{
		CurrentThread->TimeSliceCount = 0;

		ThreadInformation *NextThread = FindNextThread(CurrentThread);
		
		if(NextThread == CurrentThread)
			return;
		
		_asm
		{
			mov ebx, CurrentThread
			mov ecx, NextThread
			lea esi, [ebx]
			lea edi, [ecx]

			push ebp

			mov eax, cr3 
			mov [esi].SavedCR3, eax
			mov edx, [edi].SavedCR3

			mov [esi].SavedESP, esp
			mov esp, [edi].SavedESP

			cmp eax, edx
			je SkipCR		
			mov cr3, edx
		SkipCR:
			pop ebp
			
			mov CurrentThread, ebx
			mov NextThread, ecx

			mov eax, 0
			mov ebx, 0
			mov ecx, 0
			mov edx, 0
			mov edi, 0
			mov esi, 0
		}

		uint16_t OldFS;
		ASM_ReadReg(fs, OldFS);
		CoreComplexObj::GetComplex()->GDTTable.UpdateMemoryEntry(OldFS, reinterpret_cast<uint32_t>(NextThread), sizeof(ThreadInformation), DescriptiorData::Present | DescriptiorData::Operand32Bit | DescriptiorData::NonSystemFlag, DescriptiorData::DataReadWrite, 3);
		ASM_WriteReg(fs, OldFS);
	}
	else
	{
		CurrentThread->TimeSliceCount++;
	}
}


#pragma warning(pop)

typedef void (*ThreadPtr)(ThreadInformation * Context, uintptr_t * Data);

void ThreadStart()
{
	ASM_STI;
	//OutPortb(0x20, 0x20);
	m_InterruptControler.ClearIRQ(0);

	ThreadInformation *CurrentThread = reinterpret_cast<ThreadInformation *>(ReadFS(8));

	ThreadPtr Call = reinterpret_cast<ThreadPtr>(CurrentThread->StartingPoint);

	Call(CurrentThread, CurrentThread->StartingData);

	while(true)
		printf("!");
}

extern MulitBoot::Header MB1Header;

// Okay, same notes on how things are going to 'work' in the future.
// * When we get into MultiBootMain we have a stack and the segments are good, but that's about all we have
// * Step one will be work out a place to put the Core Complex. We will find a block of memory that is Free in the low memory. Starting with 0x10000, 
//    then moving up by 0x10000. This will become the Core Complex
// * The Core Complex will hold our GDT (with only a dozen or so entries), the full IDT and pointers other core structures
// * We'll build our GDT and IDTs, and set up the pointers and reset our segments.
// * Selector 0x50 will be set to point to the base of the Core Complex, and this will be assigned to GS.
// * We will then build up our memory map using the info from the multiboot header
// * Pull out everything from the Multiboot header we need/want. After that assume it's free memory.
// * Set up the basic MMU with direct mapping, and if we have less then 1gig ram, map the ram into the right place 
// * do a full load of the image at the 1gig line. 
// * Set up the Kernel thread, then allow it to take over, jumping us into the relocated kernel.
// * Full Kernel takes ownership of the Core Complex, marks the load location as free.
// * After that we will grab some memory for the Kernel Heap, and set up the raw terminal.
// * Create both the Monitor thread and the idle thread
// * Start the monitor

// Eventual all of this will be in the Core Complex
OpenHCI USB;

DescriptiorData::TaskStateSegment MyTSS;

MemoryPageMap BuildMemoryPageMap(MultiBootInfo &MBReader)
{
	// Step 1a: Enumerate over the Memory information in the MB data and work out some information.		
	uint64_t MaxMemory = 0;			// The last byte of memory we know of
	uint32_t HighReserved = 0;		// should be the start of reserved memory between 1 meg and 4 gig
	uint64_t TotalMemory = 0x10000; // The count always seems to be 64k off, so start with that assumed.

	MemoryMapEntry CurrentEntry;
	MBReader.GetFirstMemoryEntry(CurrentEntry);

	do
	{
		TotalMemory += CurrentEntry.Length;

		uint64_t EndAddress = CurrentEntry.BaseAddress + CurrentEntry.Length;
		if(EndAddress > MaxMemory)
			MaxMemory = EndAddress;

		if(HighReserved == 0 && CurrentEntry.Type != 1 && EndAddress < 0x100000000ull)
		{
			if(CurrentEntry.BaseAddress > 0x00100000 && CurrentEntry.BaseAddress > HighReserved)
				HighReserved = static_cast<uint32_t>(CurrentEntry.BaseAddress);
		}
	} while(MBReader.GetNextMemoryEntry(CurrentEntry));

	KernalPrintf("  Total Memory: %08X:%08X\n", (uint32_t)((TotalMemory & 0xFFFFFFFF00000000) >> 32), (uint32_t)(TotalMemory & 0xFFFFFFFF));
	KernalPrintf("  Memory Hole: %08X, Highest Address: %08X:%08X\n", HighReserved, (uint32_t)((MaxMemory & 0xFFFFFFFF00000000) >> 32), (uint32_t)(MaxMemory & 0xFFFFFFFF));

	// Step 1b: Work out how big the map will be.
	// Total pages
	uint32_t PageCount = static_cast<uint32_t>(MaxMemory / 0x1000);
	if(MaxMemory % 0x1000)
		PageCount++;
	
	// Two bytes per a page, means that a given page of memory can map 0x4000 pages (64 megs)
	uint32_t MemoryMapSize = PageCount / 0x4000;
	if(PageCount % 0x4000)
		MemoryMapSize++;
	
	// Work out the Memory map location
	HighReserved = HighReserved - (MemoryMapSize * 0x1000);
	
	// And make sure it starts on a page
	if(HighReserved % 0x1000 != 0)
		HighReserved -= HighReserved % 0x1000;

	// Step 1c: Create the map
	MemoryPageMap TempMap(HighReserved, PageCount);
	
	KernalPrintf("  Memory Map: %08X, Size %08X\n", HighReserved, MemoryMapSize * 0x1000);

	// Step 1d set the memory information from the MB data
	MBReader.GetFirstMemoryEntry(CurrentEntry);

	do {
		MemoryType Type = ReservedMemory;
		if(CurrentEntry.Type == 1)
			Type = FreeMemory;

		if(CurrentEntry.Type == 3)
			Type = AllocatedMemory;

		TempMap.SetAllocatedMemory(CurrentEntry.BaseAddress, CurrentEntry.Length, Type);
	} while(MBReader.GetNextMemoryEntry(CurrentEntry));

	// Step 1e set all the know reserved and allocated data objects
	TempMap.SetAllocatedMemory(0x0, 0x10000, ReservedMemory);
	TempMap.SetAllocatedMemory(MB1Header.load_address, MB1Header.bss_end_address - MB1Header.load_address, AllocatedMemory);
	TempMap.SetAllocatedMemory(reinterpret_cast<uint32_t>(MBReader.MBData), MBReader.HeaderLength, AllocatedMemory);
	TempMap.SetAllocatedMemory(HighReserved, MemoryMapSize * 0x1000, AllocatedMemory);	

	{
		ModuleEntry CurrentModule;
		if(MBReader.GetFirstModuleEntry(CurrentModule))
		{
			do {
				TempMap.SetAllocatedMemory(CurrentModule.ModStart, CurrentModule.ModEnd - CurrentModule.ModStart, AllocatedMemory);
			} while(MBReader.GetNextModuleEntry(CurrentModule));
		}
	}

	return TempMap;
}

CoreComplexObj *BuildCoreComplex(MemoryPageMap &MemoryMap)
{
	// Pick an Address for the Core Complex
	uint64_t TempAddress = UINT64_MAX;
	TempAddress = MemoryMap.AllocateRange(0x10000, 0x10000);

	if(TempAddress == UINT64_MAX)
		KernalPanic(KernalCode::GeneralError, "Unable to allocated core complex");
	
	CoreComplexObj *TempComplex = new(reinterpret_cast<void *>(TempAddress)) CoreComplexObj();
	TempComplex->Self = TempComplex;
	TempComplex->PageMap = MemoryMap;
	KernalPrintf("  Core Complex: %08X\n", TempComplex);

	return TempComplex;
}

void BuildGDT(CoreComplexObj *CoreComplex)
{	
	uint64_t TempAddress = UINT64_MAX;
	TempAddress = CoreComplex->PageMap.AllocateRange(0x10000, 4096);

	CoreComplex->GDTTable.Initilize((uint32_t)TempAddress);

	// The NULL Segment is automatically added for us
	CoreComplex->CodeSegment0	= CoreComplex->GDTTable.AddMemoryEntry(0, 0xFFFFFFFF, DescriptiorData::Present | DescriptiorData::Operand32Bit | DescriptiorData::NonSystemFlag, DescriptiorData::CodeExecuteRead, 0);
	CoreComplex->DataSegment0	= CoreComplex->GDTTable.AddMemoryEntry(0, 0xFFFFFFFF, DescriptiorData::Present | DescriptiorData::Operand32Bit | DescriptiorData::NonSystemFlag, DescriptiorData::DataReadWrite, 0);
	CoreComplex->CodeSegment3	= CoreComplex->GDTTable.AddMemoryEntry(0, 0xFFFFFFFF, DescriptiorData::Present | DescriptiorData::Operand32Bit | DescriptiorData::NonSystemFlag, DescriptiorData::CodeExecuteRead, 3);
	CoreComplex->DataSegment3	= CoreComplex->GDTTable.AddMemoryEntry(0, 0xFFFFFFFF, DescriptiorData::Present | DescriptiorData::Operand32Bit | DescriptiorData::NonSystemFlag, DescriptiorData::DataReadWrite, 3);
	CoreComplex->CodeSegment2	= CoreComplex->GDTTable.AddMemoryEntry(0, 0xFFFFFFFF, DescriptiorData::Present | DescriptiorData::Operand32Bit | DescriptiorData::NonSystemFlag, DescriptiorData::CodeExecuteRead, 2);
	CoreComplex->DataSegment2	= CoreComplex->GDTTable.AddMemoryEntry(0, 0xFFFFFFFF, DescriptiorData::Present | DescriptiorData::Operand32Bit | DescriptiorData::NonSystemFlag, DescriptiorData::DataReadWrite, 2);
	CoreComplex->CodeSegment1	= CoreComplex->GDTTable.AddMemoryEntry(0, 0xFFFFFFFF, DescriptiorData::Present | DescriptiorData::Operand32Bit | DescriptiorData::NonSystemFlag, DescriptiorData::CodeExecuteRead, 1);
	CoreComplex->DataSegment1	= CoreComplex->GDTTable.AddMemoryEntry(0, 0xFFFFFFFF, DescriptiorData::Present | DescriptiorData::Operand32Bit | DescriptiorData::NonSystemFlag, DescriptiorData::DataReadWrite, 1);
	CoreComplex->CoreSegment	= CoreComplex->GDTTable.AddMemoryEntry(reinterpret_cast<uint32_t>(CoreComplex), 0x10000, DescriptiorData::Present | DescriptiorData::Operand32Bit | DescriptiorData::NonSystemFlag, DescriptiorData::DataReadWrite, 0);
	CoreComplex->ThreadSegment	= CoreComplex->GDTTable.AddMemoryEntry(0, sizeof(ThreadInformation), DescriptiorData::Present | DescriptiorData::Operand32Bit | DescriptiorData::NonSystemFlag, DescriptiorData::DataReadWrite, 3);
	CoreComplex->TaskSegment	= CoreComplex->GDTTable.AddMemoryEntry(reinterpret_cast<uint32_t>(&MyTSS), sizeof(DescriptiorData::TaskStateSegment), DescriptiorData::Present, DescriptiorData::TSS32BitSegment, 0);
	
	// Switch to our GDT
	DescriptiorData::TablePtr Pointer;
	CoreComplex->GDTTable.PopulateTablePointer(Pointer);
	
	KernalPrintf("  GDT %08X, Entries %02X\n", Pointer.Address, CoreComplex->GDTTable.TableSize());
	CoreComplex->GDTTable.SetActive(CoreComplex->CodeSegment0, CoreComplex->DataSegment0);

	// Set our system segments	
	uint16_t TempSegment = CoreComplex->ThreadSegment;
	ASM_WriteReg(fs, TempSegment);
	
	TempSegment = CoreComplex->CoreSegment;
	ASM_WriteReg(gs, TempSegment);
}

// The .bss section get's merged to the end of the .data section with Visual Studio, relying on the loader to 
// clear the memory out at the end of the block. We need to make that work here.
void ClearBSS(intptr_t ImageHeader)
{
	intptr_t Address = ImageHeader;
	
	KernalPrintf("Clear BSS Data for image at %08X\n", Address);

	PEFile::IMAGE_DOS_HEADER * DOSHeader = reinterpret_cast<PEFile::IMAGE_DOS_HEADER *>(Address);
	if (DOSHeader->e_magic != 0x5A4D)
	{
		// The magic number is bad, so whatever this is it's not a PE file
		return;
	}

	Address += DOSHeader->e_lfanew;

	PEFile::IMAGE_NT_HEADERS * NTHeader = reinterpret_cast<PEFile::IMAGE_NT_HEADERS *>(Address);
	//KernalPrintf(" NT Header Loc: %08X\n", NTHeader);
    if (NTHeader->Signature != 0x00004550)
    {
        // And the signature is wrong or missing, so shut 'er down.
		KernalPrintf("     NO NT\n");
		return;
    }

	Address += sizeof(PEFile::IMAGE_NT_HEADERS);

    // And read in the optional header
    PEFile::IMAGE_OPTIONAL_HEADER32 * OptionalHeader = reinterpret_cast<PEFile::IMAGE_OPTIONAL_HEADER32 *>(Address);

    if (OptionalHeader->Magic != PEFile::OptionSignature::NT_32Bit)
    {
        // We only work with 32 bit headers at the moment
		KernalPrintf("     NO 32\n");
        return;
    }

	Address += sizeof(PEFile::IMAGE_OPTIONAL_HEADER32);
	Address += sizeof(PEFile::IMAGE_DATA_DIRECTORY) * OptionalHeader->NumberOfRvaAndSizes;

	for(unsigned int x = 0; x < NTHeader->FileHeader.NumberOfSections; x++)
	{
		PEFile::IMAGE_SECTION_HEADER * SectionHeader = reinterpret_cast<PEFile::IMAGE_SECTION_HEADER *>(Address);
		//KernalPrintf("  %8.8s\n", SectionHeader->Name);
		if(strncmp(SectionHeader->Name, ".data", 5) == 0)
		{
			void * Ptr = reinterpret_cast<void *>(ImageHeader + SectionHeader->VirtualAddress + SectionHeader->SizeOfRawData);
			if(SectionHeader->SizeOfRawData < SectionHeader->VirtualSize)
			{
				//KernalPrintf("  Patch. Base %08X, RD %08X, VS %08X, %08X\n", Ptr, SectionHeader->SizeOfRawData, SectionHeader->VirtualSize, SectionHeader->VirtualSize - SectionHeader->SizeOfRawData);
				memset(Ptr, 0x00, SectionHeader->VirtualSize - SectionHeader->SizeOfRawData);
			}
		}

		Address += sizeof(PEFile::IMAGE_SECTION_HEADER);
	}
	
	//NTHeader->FileHeader.NumberOfSections
}

extern "C" void MultiBootMain(void *Address, uint32_t Magic) 
{
	// At this point we are officially alive, but we're still a long ways away from being up and running.
	MultiBootInfo MBReader;

	// Parse the MultiBoot info
	MBReader.LoadMultiBootInfo(Magic, Address);

	// Panic if we don't have a text mode display of some sort
	if(MBReader.FrameBuffer.Type != 0x02)
		ASM_HLT;

	// Set up a simple text terminal for the kernal to use
	RawTerminal TextTerm(static_cast<uint32_t>(MBReader.FrameBuffer.Address), MBReader.FrameBuffer.Width, MBReader.FrameBuffer.Height, MBReader.FrameBuffer.Pitch);
	TextTerm.SetPauseFullScreen(false);
	TextTerm.Clear();
	KernalTerminal = &TextTerm;

	KernalPrintf("Starting up BootDebug...\n");
	KernalPrintf(" MultiBoot v%u %s\n", MBReader.Type, MBReader.BootLoader);

	ClearBSS(MB1Header.load_address);
	
	// Step 1: Build the memory map in physical memory
	KernalPrintf(" Building Memory Map...\n");
	MemoryPageMap TempMap = BuildMemoryPageMap(MBReader);
	
	// Step 2: Build the Core Complex
	KernalPrintf(" Building Core Complex...\n");
	CoreComplexObj *CoreComplex = BuildCoreComplex(TempMap);
	CoreComplex->MultiBoot.LoadMultiBootInfo(Magic, Address);
	
	// Step 3: Set up our GDT
	KernalPrintf(" Building GDT...\n");
	BuildGDT(CoreComplex);

	// Step 3b: Set up the LDT
	CoreComplex->LDTTable.Initilize(CoreComplex->GDTTable);
	CoreComplex->LDTTable.SetActive();

	// Step 2: Set our IDT
	KernalPrintf(" Setting up IDT...\n");
	CoreComplex->IDTTable.Initilize(CoreComplex->CodeSegment0, CoreComplex->DataSegment0, CoreComplex->CoreSegment);

	// Set up the handlers for System errors
	for(int x = 0; x < 0x20; x++)
	{
		CoreComplex->IDTTable.SetInterupt(x, (InterruptCallbackPtr)SystemTrap);
	}

	CoreComplex->IDTTable.SetActive();

	// Step 4: Bring up the MMU for the kernel space
	KernalPrintf(" Setting up Memory Manager...\n");

	CurrentTerminal = &TextTerm;


	// Map the Core Complex pages 1:1 into virtual memory
	// Okay, this doesn't do anything like that yet, but still...
	MMU Memory;
	MMUManager = &Memory;
	
	// At this point we will be up and running with virtual memory, so their will be a clear different between Physical and Virtual addresses
	
	// Grab 64k for the hardware tree.
	void * Temp = KernalPageAllocate(0x10000, KernalPageFlags::None);
	memset(Temp, 0, 0x10000);
	CoreComplex->HardwareComplex.Initilize(Temp, 0x10000);

	KernalPrintf(" Hardware Root: %08X\n", CoreComplex->HardwareComplex.Root);

	CoreComplex->HardwareComplex.Add("VGA", "VGA display");	



	// Step X: Create the Kernal's heap
	// Get a range for our dynamic memory, starting at 2megs and asking for 16 meg	
	uint64_t TempAddress = CoreComplex->PageMap.AllocateRange(0x200000, 0x1000000);

	// Start it up with 16 byte blocks
	CoreComplex->KernalHeap.SetupHeap(static_cast<uint32_t>(TempAddress), 0x1000000, 0x10);
	MemoryMgr = &CoreComplex->KernalHeap;

	CoreComplex->ObjectComplex.Setup();

	CurrentTerminal = &TextTerm;
	//printf("%016llX\n", sizeof(CoreComplexObj));
	//FinalMap.Dump();

	// Grab the ACPI tabeles.
	//ACPI_STATUS Status = AcpiInitializeSubsystem ();


	// At this point we should be able to display text and allocate memory.

	// Get the execution environment running
	KernalPrintf(" Starting Executer...\n");
	_atexit_init();
	_initterm();

	KernalPrintf(" Loading ACPI...\n");
	CoreComplex->ACPIComplex.InitilizeTables();
	
	//ACPI_TABLE_HEADER *Blob = nullptr;
	//AcpiGetTable(ACPI_SIG_MADT, 0, &Blob);
	
	// Step 3: Remap IRQs
	KernalPrintf(" Setting up IRQs...\n");

	m_InterruptControler.Initialize(&CoreComplex->IDTTable, reinterpret_cast<ACPI_TABLE_MADT *>(CoreComplex->ACPIComplex.GetTable("APIC")));

	m_InterruptControler.SetIRQInterrupt(0x01, IntPriority::High, KeyboardInterrupt);

	//KernalPrintf(" '%f'\n", fcos);

	CoreComplex->HardwareComplex.Add("KB", "PS/2 Keyboard");	

	// Create the Idle Thread
	KernalPrintf(" Setting up Threads...\n");
	ThreadListHead = reinterpret_cast<ThreadInformation *>(0x20000000);
	ThreadListHead->Prev = 0;
	ThreadListHead->Next = 0;

	ThreadListHead->RealAddress = reinterpret_cast<uintptr_t *>(ThreadListHead);
	ThreadListHead->ThreadID = reinterpret_cast<uint32_t>(ThreadListHead);// 0;
	ThreadListHead->State = PreCreate;
	ThreadListHead->TimeSliceCount = 0;
	ThreadListHead->TimeSliceAllocation = 10;
	ThreadListHead->TickCount = 0;

	ThreadListHead->SavedCR3 = ReadCR3();
	ThreadListHead->SavedESP = 0;
	ThreadListHead->StackLimit = 0x8000;

	ThreadListHead->StartingPoint = reinterpret_cast<uint32_t>(IdleThread); 
	ThreadListHead->StartingData = nullptr;

	ThreadListHead->Suspended = false;
	
	int StackSize = ThreadListHead->StackLimit / 4;
	ThreadListHead->SavedESP = reinterpret_cast<uint32_t>(&(ThreadListHead->Stack[StackSize]));
	Stack::Push(ThreadListHead->SavedESP, reinterpret_cast<uint32_t>(ThreadStart)); // IP
	Stack::Push(ThreadListHead->SavedESP, ThreadListHead->SavedESP); // The old BP

	uint32_t OldESP = ThreadListHead->SavedESP;

	ThreadListHead->SavedESP -= 10 * sizeof(uint32_t);
	Stack::Push(ThreadListHead->SavedESP, OldESP);  // Current BP

	ThreadInformation * MyThread = reinterpret_cast<ThreadInformation *>(reinterpret_cast<uint32_t>(ThreadListHead->Stack) + ThreadListHead->StackLimit + sizeof(ThreadInformation));

	// Convert ourselves into a full thread
	MyThread->RealAddress = reinterpret_cast<uintptr_t *>(MyThread);
	MyThread->ThreadID = reinterpret_cast<uint32_t>(MyThread);// 1;
	MyThread->State = Running;
	MyThread->TimeSliceCount = 0;
	MyThread->TimeSliceAllocation = 30;
	MyThread->TickCount = 0;
	MyThread->StackLimit = 0x8000;
	
	MyThread->StartingPoint = 0;
	MyThread->StartingData = nullptr;

	MyThread->Prev = nullptr;
	MyThread->Next = nullptr;
	MyThread->Suspended = false;

	ThreadListHead->InsertLast(MyThread);

	CoreComplex->GDTTable.UpdateMemoryEntry(CoreComplex->ThreadSegment, reinterpret_cast<uint32_t>(MyThread), sizeof(ThreadInformation), DescriptiorData::Present | DescriptiorData::Operand32Bit | DescriptiorData::NonSystemFlag, DescriptiorData::DataReadWrite, 3);
	uint16_t TempSegment = CoreComplex->ThreadSegment;
	ASM_WriteReg(fs, TempSegment);

	// And set up the PIT for ~1.5ms 
	int ClockSpeed = 1193180 / 650;
	//int ClockSpeed = 0;

	OutPortb(0x43, 0x34); // Channel 0, Lo/Hi, Mode 2 (Rate generator), Binary
	OutPortb(0x40, ClockSpeed & 0xFF); // Low Byte
	OutPortb(0x40, ClockSpeed >> 8); // High Byte

	m_InterruptControler.SetIRQInterrupt(0x00, IntPriority::System, (InterruptCallbackPtr)ClockInterrupt);	

	//m_InterruptControler.EnableIRQ(2);
	//m_InterruptControler.EnableIRQ(9);
	//m_InterruptControler.ClearIRQ(0x00);

	ASM_STI;

	MyTSS.SS0 = CoreComplex->DataSegment0;
	MyTSS.ESP0 = 0x41004;
	
	SetTR(CoreComplex->TaskSegment);

	HardwareTree * PCIRoot = CoreComplex->HardwareComplex.Add("PCI", "PCI Bus");
	
	PCI PCIBus;
	PCIBus.Initilize(PCIRoot);

	uint32_t USBID = PCIBus.FindDeviceID(0x0C, 0x03, 0x10);

	if(USBID != 0xFFFFFFFF)
	{
		USB.StartUp(USBID, &m_InterruptControler);
		USBManager = &USB;
	}

	IDE IDEDriver;
	IDEDriver.Setup(PCIBus);

	AHCI SATADriver;
	SATADriver.Setup(PCIBus);

	CoreComplex->ACPIComplex.Enable();

	// Step 6: Start the full kernel
	char * CommandLine = (char * )KernalAlloc(strlen(CoreComplex->MultiBoot.CommandLine) + 11);
	memcpy(CommandLine, "BootDebug ", 10);
	memcpy(&CommandLine[10], CoreComplex->MultiBoot.CommandLine, strlen(CoreComplex->MultiBoot.CommandLine) + 1);

	KernalPrintf(" Command Line: %s\n", CommandLine);

	char *argv[32];
	int argc = _ConvertCommandLineToArgcArgv(CommandLine, argv, 31);

	TextTerm.SetPauseFullScreen(true);
	KernalPrintf(" Starting Monitor\n\n");
	
	while(KBBufferFirst != KBBufferLast)
		FetchKeyboardBuffer();

	for(;;)
		main(argc, argv);
	
	//printf("BYe!");

	for(;;) ASM_HLT;

	//printf("Woops!");
}


