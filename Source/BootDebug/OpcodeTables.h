#pragma once

// How to parse this opcode
enum ASMParseTypes
{	
	// Invalid opcode
	ParseInvalid,
	
	// Single byte command that has a byte memory access
	ParseNoParamByte,
	
	// Single byte command
	ParseNoParam,
	
	// Has a standard ModRM (+ SIB) byte attached to it for Byte data (AL/AH)
	ParseModRMByte,

	// Has a standard ModRM (+ SIB) byte attached to it for OpSize Data
	ParseModRM,
	
	// Has a standard ModRM (+ SIB) as well as 3rd immediate Byte length parameter
	ParseModRMPlusByte,

	// Has a standard ModRM (+ SIB) as well as 3rd immediate OpSize length parameter
	ParseModRMPlus,

	// There is an Immediate Byte value following the ModR/M
	ParseImmediateByte,

	// Weird place where the Immediate is always a byte, but the Mod/RM is based on the opsize
	ParseOpImmediateByte,

	// There is an Immediate OpSizeed value following the ModR/M
	ParseImmediate,
	
	// There is an Immediate Byte value following the opcode
	ParseOnlyImmediateByte,

	// There is an Immediate word value following the opcode
	ParseOnlyImmediateWord,

	// There is an Immediate OpSized value following the opcode
	ParseOnlyImmediate,

	// There is an Immediate Address OpSized value following the opcode (16 = 32, 32 = 48)
	ParseOnlyFarAddress,

	// Everything after this is a prefix
	ParsePrefix,
	
	// This is a segment Prefix
	ParseSegmentPrefix,

	ParseOpSizePrefix,
	ParseAdSizePrefix,

	// Two Byte Opcode Prefix
	ParseTwoBytePrefix,

	// Uses the Parse rules in the parent of the group to do the parsing
	ParseGroupFromParent,

	// Uses the Params in the parent of the group to do the parsing
	ParseParamsFromParent,
};

// Where the Parameter comes from
enum ASMParamTypes
{	
	ParamNone,

	// Parameter comes from the Mod R/M Bits
	ParamModRM,
	
	// Parameter comes from the Reg bits
	ParamReg,

	// Parameter comes from the Reg bits (AX or EAX)
	ParamRegOpSize,

	// Parameter comes from the Reg bits, Control Register
	ParamRegControl,

	// Parameter comes from the Reg bits, Debug Register
	ParamRegDebug,

	// Parameter comes from the Reg bits, and stands for a Segment
	ParamRegSeg,

	// Parameter comes from the Immediate Data
	ParamImmediate,

	// Parameter is an offset that comes from Immediate Data
	ParamOffset,

	// Parameter is a fix address for a jump/call
	ParamAddress,

	// Parameter is a fix address for a far jump/call
	ParamFarAddress,

	// A fixed value of '1'
	ParamOne,
	
	// Everything after this flag is a fixed register/segment value
	ParamRegDirect,

	RegAL,
	RegBL,
	RegCL,
	RegDL,
	RegAH,
	RegBH,
	RegCH,
	RegDH,

	RegAX,	// AX/EAX/RAX depending on Operand Size
	RegBX,	// BX/EBX/RBX depending on Operand Size
	RegCX,	// CX/ECX/RCX depending on Operand Size
	RegDX,	// DX/EDX/RDX depending on Operand Size
	RegSP,	// SP/ESP/RSP depending on Operand Size
	RegBP,	// BP/EBP/RBP depending on Operand Size
	RegSI,	// SI/ESI/RSI depending on Operand Size
	RegDI,	// DI/EDI/RDI depending on Operand Size

	Reg8r,
	Reg9r,
	Reg10r,
	Reg11r,
	Reg12r,
	Reg13r,
	Reg14r,
	Reg15r,

	SegCS,
	SegDS,
	SegES,
	SegSS,
	SegFS,
	SegGS,
};

struct ASMTableEntry
{
	char * Name;				// Memonic of the opcode
	ASMParseTypes ParseType;	// How to parse
	int Flags;					// Unused, but I want to have it here so I don't have to relay out the tables if I do need it.
	int ParamCount;				// Number of Parametrs in the code
	ASMParamTypes Params[3];	// Information on each Parameter.
};

ASMTableEntry SingleByteOpCodes[256] =
{
	// 00
	{"ADD",		ParseModRMByte,			0,	2,	{ParamModRM, ParamReg}},
	{"ADD",		ParseModRM,				0,	2,	{ParamModRM, ParamReg}},
	{"ADD",		ParseModRMByte,			0,	2,	{ParamReg, ParamModRM}},
	{"ADD",		ParseModRM,				0,	2,	{ParamReg, ParamModRM}},
	{"ADD",		ParseOnlyImmediateByte,	0,	2,	{RegAL, ParamImmediate}},
	{"ADD",		ParseOnlyImmediate,		0,	2,	{RegAX, ParamImmediate}},
	{"PUSH",	ParseNoParam,			0,	1,	{SegES}},
	{"POP",		ParseNoParam,			0,	1,	{SegES}},
	
	// 08
	{"OR ",		ParseModRMByte,			0,	2,	{ParamModRM, ParamReg}},
	{"OR ",		ParseModRM,				0,	2,	{ParamModRM, ParamReg}},
	{"OR ",		ParseModRMByte,			0,	2,	{ParamReg, ParamModRM}},
	{"OR ",		ParseModRM,				0,	2,	{ParamReg, ParamModRM}},
	{"OR ",		ParseOnlyImmediateByte,	0,	2,	{RegAL, ParamImmediate}},
	{"OR ",		ParseOnlyImmediate,		0,	2,	{RegAX, ParamImmediate}},
	{"PUSH",	ParseNoParam,			0,	1,	{SegCS}},
	{"",		ParseTwoBytePrefix,		0,	0,	{}},		// Two-Byte Opcode Prefix

	// 10
	{"ADC",		ParseModRMByte,			0,	2,	{ParamModRM, ParamReg}},
	{"ADC",		ParseModRM,				0,	2,	{ParamModRM, ParamReg}},
	{"ADC",		ParseModRMByte,			0,	2,	{ParamReg, ParamModRM}},
	{"ADC",		ParseModRM,				0,	2,	{ParamReg, ParamModRM}},
	{"ADC",		ParseOnlyImmediateByte,	0,	2,	{RegAL, ParamImmediate}},
	{"ADC",		ParseOnlyImmediate,		0,	2,	{RegAX, ParamImmediate}},
	{"PUSH",	ParseNoParam,			0,	1,	{SegSS}},
	{"POP",		ParseNoParam,			0,	1,	{SegSS}},

	// 18
	{"SBB",		ParseModRMByte,			0,	2,	{ParamModRM, ParamReg}},
	{"SBB",		ParseModRM,				0,	2,	{ParamModRM, ParamReg}},
	{"SBB",		ParseModRMByte,			0,	2,	{ParamReg, ParamModRM}},
	{"SBB",		ParseModRM,				0,	2,	{ParamReg, ParamModRM}},
	{"SBB",		ParseOnlyImmediateByte,	0,	2,	{RegAL, ParamImmediate}},
	{"SBB",		ParseOnlyImmediate,		0,	2,	{RegAX, ParamImmediate}},
	{"PUSH",	ParseNoParam,			0,	1,	{SegDS}},
	{"POP",		ParseNoParam,			0,	1,	{SegDS}},

	// 20
	{"AND",		ParseModRMByte,			0,	2,	{ParamModRM, ParamReg}},
	{"AND",		ParseModRM,				0,	2,	{ParamModRM, ParamReg}},
	{"AND",		ParseModRMByte,			0,	2,	{ParamReg, ParamModRM}},
	{"AND",		ParseModRM,				0,	2,	{ParamReg, ParamModRM}},
	{"AND",		ParseOnlyImmediateByte,	0,	2,	{RegAL, ParamImmediate}},
	{"AND",		ParseOnlyImmediate,		0,	2,	{RegAX, ParamImmediate}},
	{"ES ",		ParseSegmentPrefix,		0,	1,	{SegES}},
	{"DAA",		ParseNoParam,			0,	0,	{}},

	// 28
	{"SUB",		ParseModRMByte,			0,	2,	{ParamModRM, ParamReg}},
	{"SUB",		ParseModRM,				0,	2,	{ParamModRM, ParamReg}},
	{"SUB",		ParseModRMByte,			0,	2,	{ParamReg, ParamModRM}},
	{"SUB",		ParseModRM,				0,	2,	{ParamReg, ParamModRM}},
	{"SUB",		ParseOnlyImmediateByte,	0,	2,	{RegAL, ParamImmediate}},
	{"SUB",		ParseOnlyImmediate,		0,	2,	{RegAX, ParamImmediate}},
	{"CS ",		ParseSegmentPrefix,		0,	1,	{SegCS}},
	{"DAS",		ParseNoParam,			0,	0,	{}},

	// 30
	{"XOR",		ParseModRMByte,			0,	2,	{ParamModRM, ParamReg}},
	{"XOR",		ParseModRM,				0,	2,	{ParamModRM, ParamReg}},
	{"XOR",		ParseModRMByte,			0,	2,	{ParamReg, ParamModRM}},
	{"XOR",		ParseModRM,				0,	2,	{ParamReg, ParamModRM}},
	{"XOR",		ParseOnlyImmediateByte,	0,	2,	{RegAL, ParamImmediate}},
	{"XOR",		ParseOnlyImmediate,		0,	2,	{RegAX, ParamImmediate}},
	{"SS ",		ParseSegmentPrefix,		0,	1,	{SegSS}},
	{"AAA",		ParseNoParam,			0,	0,	{}},

	// 38
	{"CMP",		ParseModRMByte,			0,	2,	{ParamModRM, ParamReg}},
	{"CMP",		ParseModRM,				0,	2,	{ParamModRM, ParamReg}},
	{"CMP",		ParseModRMByte,			0,	2,	{ParamReg, ParamModRM}},
	{"CMP",		ParseModRM,				0,	2,	{ParamReg, ParamModRM}},
	{"CMP",		ParseOnlyImmediateByte,	0,	2,	{RegAL, ParamImmediate}},
	{"CMP",		ParseOnlyImmediate,		0,	2,	{RegAX, ParamImmediate}},
	{"DS ",		ParseSegmentPrefix,		0,	1,	{SegDS}},
	{"AAS",		ParseNoParam,			0,	0,	{}},

	// 40
	{"INC",		ParseNoParam,			0,	1,	{RegAX}},
	{"INC",		ParseNoParam,			0,	1,	{RegCX}},
	{"INC",		ParseNoParam,			0,	1,	{RegDX}},
	{"INC",		ParseNoParam,			0,	1,	{RegBX}},
	{"INC",		ParseNoParam,			0,	1,	{RegSP}},
	{"INC",		ParseNoParam,			0,	1,	{RegBP}},
	{"INC",		ParseNoParam,			0,	1,	{RegSI}},
	{"INC",		ParseNoParam,			0,	1,	{RegDI}},

	// 48
	{"DEC",		ParseNoParam,			0,	1,	{RegAX}},
	{"DEC",		ParseNoParam,			0,	1,	{RegCX}},
	{"DEC",		ParseNoParam,			0,	1,	{RegDX}},
	{"DEC",		ParseNoParam,			0,	1,	{RegBX}},
	{"DEC",		ParseNoParam,			0,	1,	{RegSP}},
	{"DEC",		ParseNoParam,			0,	1,	{RegBP}},
	{"DEC",		ParseNoParam,			0,	1,	{RegSI}},
	{"DEC",		ParseNoParam,			0,	1,	{RegDI}},

	// 50
	{"PUSH",	ParseNoParam,			0,	1,	{RegAX}},
	{"PUSH",	ParseNoParam,			0,	1,	{RegCX}},
	{"PUSH",	ParseNoParam,			0,	1,	{RegDX}},
	{"PUSH",	ParseNoParam,			0,	1,	{RegBX}},
	{"PUSH",	ParseNoParam,			0,	1,	{RegSP}},
	{"PUSH",	ParseNoParam,			0,	1,	{RegBP}},
	{"PUSH",	ParseNoParam,			0,	1,	{RegSI}},
	{"PUSH",	ParseNoParam,			0,	1,	{RegDI}},

	// 58
	{"POP",		ParseNoParam,			0,	1,	{RegAX}},
	{"POP",		ParseNoParam,			0,	1,	{RegCX}},
	{"POP",		ParseNoParam,			0,	1,	{RegDX}},
	{"POP",		ParseNoParam,			0,	1,	{RegBX}},
	{"POP",		ParseNoParam,			0,	1,	{RegSP}},
	{"POP",		ParseNoParam,			0,	1,	{RegBP}},
	{"POP",		ParseNoParam,			0,	1,	{RegSI}},
	{"POP",		ParseNoParam,			0,	1,	{RegDI}},

	// 60
	{"PUSHA",	ParseNoParam,			0,	0,	{}},
	{"POPA",	ParseNoParam,			0,	0,	{}},
	{"BOUND",	ParseModRM,				0,	2,	{ParamReg, ParamModRM}},
	{"ARPL",	ParseModRM,				0,	2,	{ParamModRM, ParamReg}},
	{"FS ",		ParseSegmentPrefix,		0,	1,	{SegFS}},
	{"GS ",		ParseSegmentPrefix,		0,	1,	{SegGS}},
	{"",		ParseOpSizePrefix,		0,	0,	{}},
	{"",		ParseAdSizePrefix,		0,	0,	{}},

	// 68
	{"PUSH",	ParseOnlyImmediate,		0,	1,	{ParamImmediate}},
	{"IMUL",	ParseModRMPlus,			0,	3,	{ParamReg, ParamModRM, ParamImmediate}},
	{"PUSH",	ParseOnlyImmediateByte,	0,	1,	{ParamImmediate}},
	{"IMUL",	ParseModRMPlusByte,		0,	3,	{ParamReg, ParamModRM, ParamImmediate}},
	{"INS",		ParseNoParamByte,		0,	2,	{RegSI, RegDX}},
	{"INS",		ParseNoParam,			0,	2,	{RegSI, RegDX}},
	{"OUTS",	ParseNoParamByte,		0,	2,	{RegDX, RegDI}},
	{"OUTS",	ParseNoParam,			0,	2,	{RegDX, RegDI}},

	// 70
	{"JO ",		ParseOnlyImmediateByte,	0,	1,	{ParamOffset}},
	{"JNO",		ParseOnlyImmediateByte,	0,	1,	{ParamOffset}},
	{"JC ",		ParseOnlyImmediateByte,	0,	1,	{ParamOffset}},
	{"JNC",		ParseOnlyImmediateByte,	0,	1,	{ParamOffset}},
	{"JZ ",		ParseOnlyImmediateByte,	0,	1,	{ParamOffset}},
	{"JNZ",		ParseOnlyImmediateByte,	0,	1,	{ParamOffset}},
	{"JBE",		ParseOnlyImmediateByte,	0,	1,	{ParamOffset}},
	{"JNBE",	ParseOnlyImmediateByte,	0,	1,	{ParamOffset}},

	// 78
	{"JS ",		ParseOnlyImmediateByte,	0,	1,	{ParamOffset}},
	{"JNS",		ParseOnlyImmediateByte,	0,	1,	{ParamOffset}},
	{"JP ",		ParseOnlyImmediateByte,	0,	1,	{ParamOffset}},
	{"JNP",		ParseOnlyImmediateByte,	0,	1,	{ParamOffset}},
	{"JL ",		ParseOnlyImmediateByte,	0,	1,	{ParamOffset}},
	{"JNL",		ParseOnlyImmediateByte,	0,	1,	{ParamOffset}},
	{"JLE",		ParseOnlyImmediateByte,	0,	1,	{ParamOffset}},
	{"JNLE",	ParseOnlyImmediateByte,	0,	1,	{ParamOffset}},

	// 80
	{"\x01",	ParseImmediateByte,		0,	2,	{ParamModRM, ParamImmediate}},
	{"\x01",	ParseImmediate,			0,	2,	{ParamModRM, ParamImmediate}},
	{"\x01",	ParseImmediateByte,		0,	2,	{ParamModRM, ParamImmediate}},
	{"\x01",	ParseOpImmediateByte,	0,	2,	{ParamModRM, ParamImmediate}},
	{"TEST",	ParseModRMByte,			0,	2,	{ParamModRM, ParamReg}},
	{"TEST",	ParseModRM,				0,	2,	{ParamModRM, ParamReg}},
	{"XCHG",	ParseModRMByte,			0,	2,	{ParamModRM, ParamReg}},
	{"XCHG",	ParseModRM,				0,	2,	{ParamModRM, ParamReg}},

	// 88
	{"MOV",		ParseModRMByte,			0,	2,	{ParamModRM, ParamReg}},
	{"MOV",		ParseModRM,				0,	2,	{ParamModRM, ParamReg}},
	{"MOV",		ParseModRMByte,			0,	2,	{ParamReg, ParamModRM}},
	{"MOV",		ParseModRM,				0,	2,	{ParamReg, ParamModRM}},
	{"MOV",		ParseModRM,				0,	2,	{ParamModRM, ParamRegSeg}},
	{"LEA",		ParseModRM,				0,	2,	{ParamReg, ParamModRM}},
	{"MOV",		ParseModRM,				0,	2,	{ParamRegSeg, ParamModRM}},
	{"POP",		ParseModRM,				0,	1,	{ParamModRM}},

	// 90
	{"NOP",		ParseNoParam,			0,	0,	{}},
	{"XCHG",	ParseNoParam,			0,	2,	{RegAX, RegCX}},
	{"XCHG",	ParseNoParam,			0,	2,	{RegAX, RegDX}},
	{"XCHG",	ParseNoParam,			0,	2,	{RegAX, RegBX}},
	{"XCHG",	ParseNoParam,			0,	2,	{RegAX, RegSP}},
	{"XCHG",	ParseNoParam,			0,	2,	{RegAX, RegBP}},
	{"XCHG",	ParseNoParam,			0,	2,	{RegAX, RegSI}},
	{"XCHG",	ParseNoParam,			0,	2,	{RegAX, RegDI}},

	// 98
	{"CBW",		ParseNoParam,			0,	0,	{}},
	{"CWD",		ParseNoParam,			0,	0,	{}},
	{"CALL",	ParseOnlyFarAddress,	0,	1,	{ParamFarAddress}},
	{"WAIT",	ParseNoParam,			0,	0,	{}},
	{"PUSHF",	ParseNoParam,			0,	0,	{}},
	{"POPF",	ParseNoParam,			0,	0,	{}},
	{"SAHF",	ParseNoParam,			0,	0,	{}},
	{"LAHF",	ParseNoParam,			0,	0,	{}},

	// A0
	{"MOV",		ParseOnlyImmediateByte,	0,	2,	{RegAL, ParamAddress}},
	{"MOV",		ParseOnlyImmediate,		0,	2,	{RegAX, ParamAddress}},
	{"MOV",		ParseOnlyImmediateByte,	0,	2,	{ParamAddress, RegAL}},
	{"MOV",		ParseOnlyImmediate,		0,	2,	{ParamAddress, RegAX}},
	{"MOVS",	ParseNoParamByte,		0,	2,	{RegDI, RegSI}},
	{"MOVS",	ParseNoParam,			0,	2,	{RegDI, RegSI}},
	{"CMPS",	ParseNoParamByte,		0,	2,	{RegSI, RegDI}},
	{"CMPS",	ParseNoParam,			0,	2,	{RegSI, RegDI}},

	// A8
	{"TEST",	ParseOnlyImmediateByte,	0,	2,	{RegAL, ParamImmediate}},
	{"TEST",	ParseOnlyImmediate,		0,	2,	{RegAX, ParamImmediate}},
	{"STOS",	ParseNoParamByte,		0,	1,	{RegDI}},
	{"STOS",	ParseNoParam,			0,	1,	{RegDI}},
	{"LODS",	ParseNoParamByte,		0,	1,	{RegSI}},
	{"LODS",	ParseNoParam,			0,	1,	{RegSI}},
	{"SCAS",	ParseOnlyImmediateByte,	0,	2,	{RegAL, ParamOffset}},
	{"SCAS",	ParseOnlyImmediate,		0,	2,	{RegAX, ParamOffset}},

	// B0
	{"MOV",		ParseOnlyImmediateByte,	0,	2,	{RegAL, ParamImmediate}},
	{"MOV",		ParseOnlyImmediateByte,	0,	2,	{RegCL, ParamImmediate}},
	{"MOV",		ParseOnlyImmediateByte,	0,	2,	{RegDL, ParamImmediate}},
	{"MOV",		ParseOnlyImmediateByte,	0,	2,	{RegBL, ParamImmediate}},
	{"MOV",		ParseOnlyImmediateByte,	0,	2,	{RegAH, ParamImmediate}},
	{"MOV",		ParseOnlyImmediateByte,	0,	2,	{RegCH, ParamImmediate}},
	{"MOV",		ParseOnlyImmediateByte,	0,	2,	{RegDH, ParamImmediate}},
	{"MOV",		ParseOnlyImmediateByte,	0,	2,	{RegBH, ParamImmediate}},

	// B8
	{"MOV",		ParseOnlyImmediate,		0,	2,	{RegAX, ParamImmediate}},
	{"MOV",		ParseOnlyImmediate,		0,	2,	{RegCX, ParamImmediate}},
	{"MOV",		ParseOnlyImmediate,		0,	2,	{RegDX, ParamImmediate}},
	{"MOV",		ParseOnlyImmediate,		0,	2,	{RegBX, ParamImmediate}},
	{"MOV",		ParseOnlyImmediate,		0,	2,	{RegSP, ParamImmediate}},
	{"MOV",		ParseOnlyImmediate,		0,	2,	{RegBP, ParamImmediate}},
	{"MOV",		ParseOnlyImmediate,		0,	2,	{RegSI, ParamImmediate}},
	{"MOV",		ParseOnlyImmediate,		0,	2,	{RegDI, ParamImmediate}},

	// C0
	{"\x02",	ParseImmediateByte,		0,	2,	{ParamModRM, ParamImmediate}},
	{"\x02",	ParseOpImmediateByte,	0,	2,	{ParamModRM, ParamImmediate}},
	{"RET",		ParseOnlyImmediateWord,	0,	1,	{ParamImmediate}},
	{"RET",		ParseNoParam,			0,	0,	{}},
	{"LES",		ParseModRM,				0,	2,	{ParamReg, ParamModRM}},
	{"LDS",		ParseModRM,				0,	2,	{ParamReg, ParamModRM}},
	{"\x0B",	ParseImmediateByte,		0,	2,	{ParamModRM, ParamImmediate}},
	{"\x0B",	ParseImmediate,			0,	2,	{ParamModRM, ParamImmediate}},

	// C8
	{"ENTER",	ParseInvalid,			0,	2,	{ParamImmediate, ParamImmediate}},
	{"LEAVE",	ParseNoParam,			0,	0,	{}},
	{"FRET",	ParseOnlyImmediateWord,	0,	1,	{ParamImmediate}},
	{"FRET",	ParseNoParam,			0,	0,	{}},
	{"INT3",	ParseNoParam,			0,	0,	{}},
	{"INT",		ParseOnlyImmediateByte,	0,	1,	{ParamImmediate}},
	{"INTO",	ParseNoParam,			0,	0,	{}},
	{"IRET",	ParseNoParam,			0,	0,	{}},

	// D0
	{"\x02",	ParseModRMByte,			0,	2,	{ParamModRM, ParamOne}},
	{"\x02",	ParseModRM,				0,	2,	{ParamModRM, ParamOne}},
	{"\x02",	ParseModRMByte,			0,	2,	{ParamModRM, RegCL}},
	{"\x02",	ParseModRM,				0,	2,	{ParamModRM, RegCL}},
	{"AAM",		ParseOnlyImmediateByte,	0,	1,	{ParamImmediate}},
	{"AAD",		ParseOnlyImmediateByte,	0,	1,	{ParamImmediate}},
	{"",		ParseInvalid,			0,	0,	{}},
	{"XLAT",	ParseNoParam,			0,	0,	{}},

	// D8
	{"ESC",		ParseModRM,				0,	0,	{}},
	{"ESC",		ParseModRM,				0,	0,	{}},
	{"ESC",		ParseModRM,				0,	0,	{}},
	{"ESC",		ParseModRM,				0,	0,	{}},
	{"ESC",		ParseModRM,				0,	0,	{}},
	{"ESC",		ParseModRM,				0,	0,	{}},
	{"ESC",		ParseModRM,				0,	0,	{}},
	{"ESC",		ParseModRM,				0,	0,	{}},

	// E0
	{"LOOPNE",	ParseOnlyImmediateByte,	0,	1,	{ParamOffset}},
	{"LOOPE",	ParseOnlyImmediateByte,	0,	1,	{ParamOffset}},
	{"LOOP",	ParseOnlyImmediateByte,	0,	1,	{ParamOffset}},
	{"JCXZ",	ParseOnlyImmediateByte,	0,	1,	{ParamOffset}},
	{"IN ",		ParseOnlyImmediateByte,	0,	2,	{RegAL, ParamImmediate}},
	{"IN ",		ParseOnlyImmediateByte,	0,	2,	{RegAX, ParamImmediate}},
	{"OUT",		ParseOnlyImmediateByte,	0,	2,	{ParamImmediate, RegAL}},
	{"OUT",		ParseOnlyImmediateByte,	0,	2,	{ParamImmediate, RegAX}},

	// E8
	{"CALL",	ParseOnlyImmediate,		0,	1,	{ParamOffset}},
	{"JMP",		ParseOnlyImmediate,		0,	1,	{ParamOffset}},
	{"JMP",		ParseOnlyFarAddress,	0,	1,	{ParamFarAddress}},
	{"JMP",		ParseOnlyImmediateByte,	0,	1,	{ParamOffset}},
	{"IN ",		ParseNoParam,			0,	2,	{RegAL, RegDX}},
	{"IN ",		ParseNoParam,			0,	2,	{RegAX, RegDX}},
	{"OUT",		ParseNoParam,			0,	2,	{RegDX, RegAL}},
	{"OUT",		ParseNoParam,			0,	2,	{RegDX, RegAX}},

	// F0
	{"LOCK",	ParsePrefix,			0,	0,	{}},
	{"",		ParseInvalid,			0,	0,	{}},
	{"REPNE",	ParsePrefix,			0,	0,	{}},
	{"REP",		ParsePrefix,			0,	0,	{}},
	{"HLT",		ParseNoParam,			0,	0,	{}},
	{"CMC",		ParseNoParam,			0,	0,	{}},
	{"\x03",	ParseModRMByte,			0,	0,	{}},
	{"\x03",	ParseModRM,				0,	0,	{}},

	// F8
	{"CLC",		ParseNoParam,			0,	0,	{}},
	{"STC",		ParseNoParam,			0,	0,	{}},
	{"CLI",		ParseNoParam,			0,	0,	{}},
	{"STI",		ParseNoParam,			0,	0,	{}},
	{"CLD",		ParseNoParam,			0,	0,	{}},
	{"STD",		ParseNoParam,			0,	0,	{}},
	{"\x04",	ParseModRMByte,			0,	1,	{ParamModRM}},
	{"\x05",	ParseModRM,				0,	1,	{ParamModRM}},
};

ASMTableEntry TwoByteOpCodes[256] =
{
	// 00
	{"\x06",	ParseModRM,				0,	1,	{ParamModRM}},
	{"\x07",	ParseModRM,				0,	1,	{ParamModRM}},
	{"LAR",		ParseModRM,				0,	2,	{ParamReg, ParamModRM}},
	{"LSL",		ParseModRM,				0,	2,	{ParamReg, ParamModRM}},
	{"",		ParseInvalid,			0,	0,	{}},
	{"SYSCALL",	ParseNoParam,			0,	0,	{}},
	{"CLTS",	ParseNoParam,			0,	0,	{}},
	{"SYSRET",	ParseNoParam,			0,	0,	{}},
	
	// 08
	{"INVD",	ParseNoParam,			0,	0,	{}},
	{"WBINVD",	ParseNoParam,			0,	0,	{}},
	{"",		ParseInvalid,			0,	0,	{}},
	{"UD2",		ParseNoParam,			0,	0,	{}},
	{"",		ParseInvalid,			0,	0,	{}},
	{"PREFETCH",ParseModRM,				0,	1,	{ParamModRM}},
	{"",		ParseInvalid,			0,	0,	{}},
	{"",		ParseInvalid,			0,	0,	{}},

	// 10
	{"",		ParseInvalid,			0,	0,	{}},
	{"",		ParseInvalid,			0,	0,	{}},
	{"",		ParseInvalid,			0,	0,	{}},
	{"",		ParseInvalid,			0,	0,	{}},
	{"",		ParseInvalid,			0,	0,	{}},
	{"",		ParseInvalid,			0,	0,	{}},
	{"",		ParseInvalid,			0,	0,	{}},
	{"",		ParseInvalid,			0,	0,	{}},

	// 18
	{"\x10",	ParseNoParam,			0,	0,	{}},
	{"",		ParseInvalid,			0,	0,	{}},
	{"",		ParseInvalid,			0,	0,	{}},
	{"",		ParseInvalid,			0,	0,	{}},
	{"",		ParseInvalid,			0,	0,	{}},
	{"",		ParseInvalid,			0,	0,	{}},
	{"",		ParseInvalid,			0,	0,	{}},
	{"NOP",		ParseModRM,				0,	1,	{ParamModRM}},

	// 20
	{"MOV",		ParseModRM,				0,	2,	{ParamModRM, ParamRegControl}},
	{"MOV",		ParseModRM,				0,	2,	{ParamModRM, ParamRegDebug}},
	{"MOV",		ParseModRM,				0,	2,	{ParamRegControl, ParamModRM}},
	{"MOV",		ParseModRM,				0,	2,	{ParamRegDebug, ParamModRM}},
	{"",		ParseInvalid,			0,	0,	{}},
	{"",		ParseInvalid,			0,	0,	{}},
	{"",		ParseInvalid,			0,	0,	{}},
	{"",		ParseInvalid,			0,	0,	{}},

	// 28
	{"",		ParseInvalid,			0,	0,	{}},
	{"",		ParseInvalid,			0,	0,	{}},
	{"",		ParseInvalid,			0,	0,	{}},
	{"",		ParseInvalid,			0,	0,	{}},
	{"",		ParseInvalid,			0,	0,	{}},
	{"",		ParseInvalid,			0,	0,	{}},
	{"",		ParseInvalid,			0,	0,	{}},
	{"",		ParseInvalid,			0,	0,	{}},

	// 30
	{"WRMSR",	ParseNoParam,			0,	0,	{}},
	{"RDSTC",	ParseNoParam,			0,	0,	{}},
	{"RDMSR",	ParseNoParam,			0,	0,	{}},
	{"RDPMC",	ParseNoParam,			0,	0,	{}},
	{"SYSENTER",ParseNoParam,			0,	0,	{}},
	{"SYSEXIT",	ParseNoParam,			0,	0,	{}},
	{"",		ParseInvalid,			0,	0,	{}},
	{"GETSEC",	ParseNoParam,			0,	0,	{}},

	// 38
	{"",		ParseInvalid,			0,	0,	{}},
	{"",		ParseInvalid,			0,	0,	{}},
	{"",		ParseInvalid,			0,	0,	{}},
	{"",		ParseInvalid,			0,	0,	{}},
	{"",		ParseInvalid,			0,	0,	{}},
	{"",		ParseInvalid,			0,	0,	{}},
	{"",		ParseInvalid,			0,	0,	{}},
	{"",		ParseInvalid,			0,	0,	{}},

	// 40
	{"CMOVO",	ParseModRM,				0,	2,	{ParamReg, ParamModRM}},
	{"CMOVNO",	ParseModRM,				0,	2,	{ParamReg, ParamModRM}},
	{"CMOVB",	ParseModRM,				0,	2,	{ParamReg, ParamModRM}},
	{"CMOVNB",	ParseModRM,				0,	2,	{ParamReg, ParamModRM}},
	{"CMOVZ",	ParseModRM,				0,	2,	{ParamReg, ParamModRM}},
	{"CMOVNZ",	ParseModRM,				0,	2,	{ParamReg, ParamModRM}},
	{"CMOVBE",	ParseModRM,				0,	2,	{ParamReg, ParamModRM}},
	{"CMOVNBE",	ParseModRM,				0,	2,	{ParamReg, ParamModRM}},

	// 48
	{"CMOVS",	ParseModRM,				0,	2,	{ParamReg, ParamModRM}},
	{"CMOVNS",	ParseModRM,				0,	2,	{ParamReg, ParamModRM}},
	{"CMOVP",	ParseModRM,				0,	2,	{ParamReg, ParamModRM}},
	{"CMOVNP",	ParseModRM,				0,	2,	{ParamReg, ParamModRM}},
	{"CMOVL",	ParseModRM,				0,	2,	{ParamReg, ParamModRM}},
	{"CMOVNL",	ParseModRM,				0,	2,	{ParamReg, ParamModRM}},
	{"CMOVLE",	ParseModRM,				0,	2,	{ParamReg, ParamModRM}},
	{"CMOVNLE",	ParseModRM,				0,	2,	{ParamReg, ParamModRM}},

	// 50
	{"",		ParseInvalid,			0,	0,	{}},
	{"",		ParseInvalid,			0,	0,	{}},
	{"",		ParseInvalid,			0,	0,	{}},
	{"",		ParseInvalid,			0,	0,	{}},
	{"",		ParseInvalid,			0,	0,	{}},
	{"",		ParseInvalid,			0,	0,	{}},
	{"",		ParseInvalid,			0,	0,	{}},
	{"",		ParseInvalid,			0,	0,	{}},

	// 58
	{"",		ParseInvalid,			0,	0,	{}},
	{"",		ParseInvalid,			0,	0,	{}},
	{"",		ParseInvalid,			0,	0,	{}},
	{"",		ParseInvalid,			0,	0,	{}},
	{"",		ParseInvalid,			0,	0,	{}},
	{"",		ParseInvalid,			0,	0,	{}},
	{"",		ParseInvalid,			0,	0,	{}},
	{"",		ParseInvalid,			0,	0,	{}},

	// 60
	{"",		ParseInvalid,			0,	0,	{}},
	{"",		ParseInvalid,			0,	0,	{}},
	{"",		ParseInvalid,			0,	0,	{}},
	{"",		ParseInvalid,			0,	0,	{}},
	{"",		ParseInvalid,			0,	0,	{}},
	{"",		ParseInvalid,			0,	0,	{}},
	{"",		ParseInvalid,			0,	0,	{}},
	{"",		ParseInvalid,			0,	0,	{}},

	// 68
	{"",		ParseInvalid,			0,	0,	{}},
	{"",		ParseInvalid,			0,	0,	{}},
	{"",		ParseInvalid,			0,	0,	{}},
	{"",		ParseInvalid,			0,	0,	{}},
	{"",		ParseInvalid,			0,	0,	{}},
	{"",		ParseInvalid,			0,	0,	{}},
	{"",		ParseInvalid,			0,	0,	{}},
	{"",		ParseInvalid,			0,	0,	{}},

	// 70
	{"",		ParseInvalid,			0,	0,	{}},
	{"\x0C",	ParseInvalid,			0,	0,	{}},
	{"\x0D",	ParseInvalid,			0,	0,	{}},
	{"\x0E",	ParseInvalid,			0,	0,	{}},
	{"",		ParseInvalid,			0,	0,	{}},
	{"",		ParseInvalid,			0,	0,	{}},
	{"",		ParseInvalid,			0,	0,	{}},
	{"",		ParseInvalid,			0,	0,	{}},

	// 78
	{"VMREAD",	ParseModRM,				0,	2,	{ParamModRM, ParamReg}},
	{"VMWRITE",	ParseModRM,				0,	2,	{ParamReg, ParamModRM}},
	{"",		ParseInvalid,			0,	0,	{}},
	{"",		ParseInvalid,			0,	0,	{}},
	{"",		ParseInvalid,			0,	0,	{}},
	{"",		ParseInvalid,			0,	0,	{}},
	{"",		ParseInvalid,			0,	0,	{}},
	{"",		ParseInvalid,			0,	0,	{}},

	// 80
	{"JO ",		ParseOnlyImmediate,		0,	1,	{ParamOffset}},
	{"JNO",		ParseOnlyImmediate,		0,	1,	{ParamOffset}},
	{"JC ",		ParseOnlyImmediate,		0,	1,	{ParamOffset}},
	{"JNC",		ParseOnlyImmediate,		0,	1,	{ParamOffset}},
	{"JZ ",		ParseOnlyImmediate,		0,	1,	{ParamOffset}},
	{"JNZ",		ParseOnlyImmediate,		0,	1,	{ParamOffset}},
	{"JBE",		ParseOnlyImmediate,		0,	1,	{ParamOffset}},
	{"JNBE",	ParseOnlyImmediate,		0,	1,	{ParamOffset}},

	// 88
	{"JS ",		ParseOnlyImmediate,		0,	1,	{ParamOffset}},
	{"JNS",		ParseOnlyImmediate,		0,	1,	{ParamOffset}},
	{"JP ",		ParseOnlyImmediate,		0,	1,	{ParamOffset}},
	{"JNP",		ParseOnlyImmediate,		0,	1,	{ParamOffset}},
	{"JL ",		ParseOnlyImmediate,		0,	1,	{ParamOffset}},
	{"JNL",		ParseOnlyImmediate,		0,	1,	{ParamOffset}},
	{"JLE",		ParseOnlyImmediate,		0,	1,	{ParamOffset}},
	{"JNLE",	ParseOnlyImmediate,		0,	1,	{ParamOffset}},

	// 90
	{"SETO",	ParseModRMByte,			0,	1,	{ParamModRM}},
	{"SETNO",	ParseModRMByte,			0,	1,	{ParamModRM}},
	{"SETC",	ParseModRMByte,			0,	1,	{ParamModRM}},
	{"SETNC",	ParseModRMByte,			0,	1,	{ParamModRM}},
	{"SETZ",	ParseModRMByte,			0,	1,	{ParamModRM}},
	{"SETNZ",	ParseModRMByte,			0,	1,	{ParamModRM}},
	{"SETBE",	ParseModRMByte,			0,	1,	{ParamModRM}},
	{"SETNBE",	ParseModRMByte,			0,	1,	{ParamModRM}},

	// 98
	{"SETS",	ParseModRMByte,			0,	1,	{ParamModRM}},
	{"SETNS",	ParseModRMByte,			0,	1,	{ParamModRM}},
	{"SETP",	ParseModRMByte,			0,	1,	{ParamModRM}},
	{"SETNP",	ParseModRMByte,			0,	1,	{ParamModRM}},
	{"SETL",	ParseModRMByte,			0,	1,	{ParamModRM}},
	{"SETNL",	ParseModRMByte,			0,	1,	{ParamModRM}},
	{"SETLE",	ParseModRMByte,			0,	1,	{ParamModRM}},
	{"SETNLE",	ParseModRMByte,			0,	1,	{ParamModRM}},

	// A0
	{"PUSH",	ParseNoParam,			0,	1,	{SegFS}},
	{"POP",		ParseNoParam,			0,	1,	{SegFS}},
	{"CPUID",	ParseNoParam,			0,	0,	{}},
	{"BT ",		ParseModRM,				0,	2,	{ParamReg, ParamModRM}},
	{"SHLD",	ParseModRMPlusByte,		0,	3,	{ParamModRM, ParamReg, ParamImmediate}},
	{"SHLD",	ParseModRM,				0,	3,	{ParamModRM, ParamReg, RegCL}},
	{"",		ParseInvalid,			0,	0,	{}},
	{"",		ParseInvalid,			0,	0,	{}},

	// A8
	{"PUSH",	ParseNoParam,			0,	1,	{SegES}},
	{"POP",		ParseNoParam,			0,	1,	{SegES}},
	{"RSM",		ParseNoParam,			0,	0,	{}},
	{"BTS",		ParseModRM,				0,	2,	{ParamModRM, ParamReg}},
	{"SHRD",	ParseModRMPlusByte,		0,	3,	{ParamModRM, ParamReg, ParamImmediate}},
	{"SHRD",	ParseModRM,				0,	3,	{ParamModRM, ParamReg, RegCL}},
	{"\x0F",	ParseInvalid,			0,	0,	{}},
	{"IMUL",	ParseModRM,				0,	2,	{ParamReg, ParamModRM}},

	// B0
	{"CMPXCHG",	ParseModRM,				0,	2,	{ParamModRM, ParamReg}},
	{"CMPXCHG",	ParseModRM,				0,	2,	{ParamReg, ParamModRM}},
	{"LSS",		ParseModRM,				0,	2,	{ParamReg, ParamModRM}},
	{"BTR",		ParseModRM,				0,	2,	{ParamModRM, ParamReg}},
	{"LFS",		ParseModRM,				0,	2,	{ParamReg, ParamModRM}},
	{"LGS",		ParseModRM,				0,	2,	{ParamReg, ParamModRM}},
	{"MOVZX",	ParseModRMByte,			0,	2,	{ParamReg, ParamModRM}},
	{"MOVZX",	ParseModRM,				0,	2,	{ParamReg, ParamModRM}},

	// B8
	{"JMPE",	ParseNoParam,			0,	0,	{}},
	{"\x0A",	ParseInvalid,			0,	0,	{}},
	{"\x08",	ParseModRMPlusByte,		0,	2,	{ParamModRM, ParamImmediate}},
	{"BTC",		ParseModRM,				0,	2,	{ParamModRM, ParamReg}},
	{"BSF",		ParseModRM,				0,	2,	{ParamReg, ParamModRM}},
	{"BSR",		ParseModRM,				0,	2,	{ParamReg, ParamModRM}},
	{"MOVSX",	ParseModRMByte,			0,	2,	{ParamRegOpSize, ParamModRM}},
	{"MOVSX",	ParseModRM,				0,	2,	{ParamReg, ParamModRM}},

	// C0
	{"XADD",	ParseModRMByte,			0,	2,	{ParamReg, ParamModRM}},
	{"XADD",	ParseModRM,				0,	2,	{ParamReg, ParamModRM}},
	{"",		ParseInvalid,			0,	0,	{}},
	{"",		ParseInvalid,			0,	0,	{}},
	{"",		ParseInvalid,			0,	0,	{}},
	{"",		ParseInvalid,			0,	0,	{}},
	{"",		ParseInvalid,			0,	0,	{}},
	{"\x09",	ParseInvalid,			0,	0,	{}},

	// C8
	{"",		ParseInvalid,			0,	0,	{}},
	{"",		ParseInvalid,			0,	0,	{}},
	{"",		ParseInvalid,			0,	0,	{}},
	{"",		ParseInvalid,			0,	0,	{}},
	{"",		ParseInvalid,			0,	0,	{}},
	{"",		ParseInvalid,			0,	0,	{}},
	{"",		ParseInvalid,			0,	0,	{}},
	{"",		ParseInvalid,			0,	0,	{}},

	// D0
	{"",		ParseInvalid,			0,	0,	{}},
	{"",		ParseInvalid,			0,	0,	{}},
	{"",		ParseInvalid,			0,	0,	{}},
	{"",		ParseInvalid,			0,	0,	{}},
	{"",		ParseInvalid,			0,	0,	{}},
	{"",		ParseInvalid,			0,	0,	{}},
	{"",		ParseInvalid,			0,	0,	{}},
	{"",		ParseInvalid,			0,	0,	{}},

	// D8
	{"",		ParseInvalid,			0,	0,	{}},
	{"",		ParseInvalid,			0,	0,	{}},
	{"",		ParseInvalid,			0,	0,	{}},
	{"",		ParseInvalid,			0,	0,	{}},
	{"",		ParseInvalid,			0,	0,	{}},
	{"",		ParseInvalid,			0,	0,	{}},
	{"",		ParseInvalid,			0,	0,	{}},
	{"",		ParseInvalid,			0,	0,	{}},

	// E0
	{"",		ParseInvalid,			0,	0,	{}},
	{"",		ParseInvalid,			0,	0,	{}},
	{"",		ParseInvalid,			0,	0,	{}},
	{"",		ParseInvalid,			0,	0,	{}},
	{"",		ParseInvalid,			0,	0,	{}},
	{"",		ParseInvalid,			0,	0,	{}},
	{"",		ParseInvalid,			0,	0,	{}},
	{"",		ParseInvalid,			0,	0,	{}},

	// E8
	{"",		ParseInvalid,			0,	0,	{}},
	{"",		ParseInvalid,			0,	0,	{}},
	{"",		ParseInvalid,			0,	0,	{}},
	{"",		ParseInvalid,			0,	0,	{}},
	{"",		ParseInvalid,			0,	0,	{}},
	{"",		ParseInvalid,			0,	0,	{}},
	{"",		ParseInvalid,			0,	0,	{}},
	{"",		ParseInvalid,			0,	0,	{}},

	// F0
	{"",		ParseInvalid,			0,	0,	{}},
	{"",		ParseInvalid,			0,	0,	{}},
	{"",		ParseInvalid,			0,	0,	{}},
	{"",		ParseInvalid,			0,	0,	{}},
	{"",		ParseInvalid,			0,	0,	{}},
	{"",		ParseInvalid,			0,	0,	{}},
	{"",		ParseInvalid,			0,	0,	{}},
	{"",		ParseInvalid,			0,	0,	{}},

	// F8
	{"",		ParseInvalid,			0,	0,	{}},
	{"",		ParseInvalid,			0,	0,	{}},
	{"",		ParseInvalid,			0,	0,	{}},
	{"",		ParseInvalid,			0,	0,	{}},
	{"",		ParseInvalid,			0,	0,	{}},
	{"",		ParseInvalid,			0,	0,	{}},
	{"",		ParseInvalid,			0,	0,	{}},
	{"",		ParseInvalid,			0,	0,	{}},
};

ASMTableEntry OpcodeGroups[][8] =
{
	// Group 1
	{
		{"ADD",		ParseParamsFromParent,	0,	0,	{}},
		{"OR ",		ParseParamsFromParent,	0,	0,	{}},
		{"ADC",		ParseParamsFromParent,	0,	0,	{}},
		{"SBB",		ParseParamsFromParent,	0,	0,	{}},
		{"AND",		ParseParamsFromParent,	0,	0,	{}},
		{"SUB",		ParseParamsFromParent,	0,	0,	{}},
		{"XOR",		ParseParamsFromParent,	0,	0,	{}},
		{"CMP",		ParseParamsFromParent,	0,	0,	{}},
	},
	// Group 2
	{
		{"ROL",		ParseParamsFromParent,	0,	0,	{}},
		{"ROR",		ParseParamsFromParent,	0,	0,	{}},
		{"RCL",		ParseParamsFromParent,	0,	0,	{}},
		{"RCR",		ParseParamsFromParent,	0,	0,	{}},
		{"SHL",		ParseParamsFromParent,	0,	0,	{}},
		{"SHR",		ParseParamsFromParent,	0,	0,	{}},
		{"",		ParseInvalid,			0,	0,	{}},
		{"SAR",		ParseParamsFromParent,	0,	0,	{}},
	},

	// Group 3
	{
		{"TEST",	ParseGroupFromParent,	0,	2,	{ ParamModRM, ParamImmediate }},
		{"",		ParseInvalid,			0,	0,	{}},
		{"NOT",		ParseGroupFromParent,	0,	1,	{ ParamModRM }},
		{"NEG",		ParseGroupFromParent,	0,	1,	{ ParamModRM }},
		{"MUL",		ParseGroupFromParent,	0,	2,	{ RegAX, ParamModRM}},
		{"IMUL",	ParseGroupFromParent,	0,	2,	{ RegAX, ParamModRM}},
		{"DIV",		ParseGroupFromParent,	0,	2,	{ RegAX, ParamModRM}},
		{"IDIV",	ParseGroupFromParent,	0,	2,	{ RegAX, ParamModRM}},
	},

	// Group 4
	{
		{"INC",		ParseParamsFromParent,	0,	0,	{}},
		{"DEC",		ParseParamsFromParent,	0,	0,	{}},
		{"",		ParseInvalid,			0,	0,	{}},
		{"",		ParseInvalid,			0,	0,	{}},
		{"",		ParseInvalid,			0,	0,	{}},
		{"",		ParseInvalid,			0,	0,	{}},
		{"",		ParseInvalid,			0,	0,	{}},
		{"",		ParseInvalid,			0,	0,	{}},
	},

	// Group 5
	{
		{"INC",		ParseParamsFromParent,	0,	0,	{}},
		{"DEC",		ParseParamsFromParent,	0,	0,	{}},
		{"CALL",	ParseGroupFromParent,	0,	1,	{ParamModRM}},
		{"CALL",	ParseOnlyFarAddress,	0,	1,	{ParamFarAddress}},
		{"JMP",		ParseGroupFromParent,	0,	1,	{ParamModRM}},
		{"JMP",		ParseOnlyFarAddress,	0,	1,	{ParamFarAddress}},
		{"PUSH",	ParseParamsFromParent,	0,	0,	{}},
		{"",		ParseInvalid,			0,	0,	{}},
	},

	// Group 6
	{
		{"SLDT",	ParseParamsFromParent,	0,	0,	{}},
		{"STR",		ParseParamsFromParent,	0,	0,	{}},
		{"LLDT",	ParseParamsFromParent,	0,	0,	{}},
		{"LTR",		ParseParamsFromParent,	0,	0,	{}},
		{"VERR",	ParseParamsFromParent,	0,	0,	{}},
		{"VERW",	ParseParamsFromParent,	0,	0,	{}},
		{"",		ParseInvalid,			0,	0,	{}},
		{"",		ParseInvalid,			0,	0,	{}},
	},

	// Group 7
	{
		{"SGDT",	ParseParamsFromParent,	0,	0,	{}},
		{"SIDT",	ParseParamsFromParent,	0,	0,	{}},
		{"LGDT",	ParseParamsFromParent,	0,	0,	{}},
		{"LIDT",	ParseParamsFromParent,	0,	0,	{}},
		{"SMSW",	ParseParamsFromParent,	0,	0,	{}},
		{"",		ParseInvalid,			0,	0,	{}},
		{"LMSW",	ParseParamsFromParent,	0,	0,	{}},
		{"INVLPG",	ParseParamsFromParent,	0,	0,	{}},
	},

	// Group 8
	{
		{"",		ParseInvalid,			0,	0,	{}},
		{"",		ParseInvalid,			0,	0,	{}},
		{"",		ParseInvalid,			0,	0,	{}},
		{"",		ParseInvalid,			0,	0,	{}},
		{"BT ",		ParseParamsFromParent,	0,	0,	{}},
		{"BTS",		ParseParamsFromParent,	0,	0,	{}},
		{"BTR",		ParseParamsFromParent,	0,	0,	{}},
		{"BTC",		ParseGroupFromParent,	0,	0,	{}},
	},

	// Group 9
	{
	},

	// Group 10
	{
	},

	// Group 11
	{
		{"MOV",		ParseParamsFromParent,	0,	0,	{}},
		{"",		ParseInvalid,			0,	0,	{}},
		{"",		ParseInvalid,			0,	0,	{}},
		{"",		ParseInvalid,			0,	0,	{}},
		{"",		ParseInvalid,			0,	0,	{}},
		{"",		ParseInvalid,			0,	0,	{}},
		{"",		ParseInvalid,			0,	0,	{}},
		{"",		ParseInvalid,			0,	0,	{}},
	},

	// Group 12
	{
	},

	// Group 13
	{
	},

	// Group 14
	{
	},

	// Group 15
	{
	},

	// Group 16
	{
	},

};