#include <stdint.h>
#include <type_traits>
#pragma once

template<typename EnumType, std::enable_if_t<std::is_enum<EnumType>::value, bool> = true>
constexpr EnumType operator | (EnumType lhs, EnumType rhs)
{
	using BaseType = std::underlying_type_t <EnumType>;
	return static_cast<EnumType>(static_cast<BaseType>(lhs) | static_cast<BaseType>(rhs));
}

template<typename EnumType, std::enable_if_t<std::is_enum<EnumType>::value, bool> = true>
constexpr EnumType operator & (EnumType lhs, EnumType rhs)
{
	using BaseType = std::underlying_type_t <EnumType>;
	return static_cast<EnumType>(static_cast<BaseType>(lhs) & static_cast<BaseType>(rhs));
}

template<typename EnumType, std::enable_if_t<std::is_enum<EnumType>::value, bool> = true>
constexpr EnumType operator ^ (EnumType lhs, EnumType rhs)
{
	using BaseType = std::underlying_type_t <EnumType>;
	return static_cast<EnumType>(static_cast<BaseType>(lhs) ^ static_cast<BaseType>(rhs));
}

template<typename EnumType, std::enable_if_t<std::is_enum<EnumType>::value, bool> = true>
constexpr EnumType operator ~ (EnumType lhs)
{
	using BaseType = std::underlying_type_t <EnumType>;
	return static_cast<EnumType>(~static_cast<BaseType>(lhs));
}

template<typename EnumType, typename ShiftType, std::enable_if_t<std::is_enum<EnumType>::value, bool> = true>
constexpr EnumType operator << (EnumType lhs, ShiftType rhs)
{
	using BaseType = std::underlying_type_t <EnumType>;
	return static_cast<EnumType>(static_cast<BaseType>(lhs) << rhs);
}

template<typename EnumType, typename ShiftType, std::enable_if_t<std::is_enum<EnumType>::value, bool> = true>
constexpr EnumType operator >> (EnumType lhs, ShiftType rhs)
{
	using BaseType = std::underlying_type_t <EnumType>;
	return static_cast<EnumType>(static_cast<BaseType>(lhs) >> rhs);
}



template<typename EnumType, std::enable_if_t<std::is_enum<EnumType>::value, bool> = true>
constexpr EnumType operator |= (EnumType &lhs, EnumType rhs)
{
	lhs = lhs | rhs;
	return lhs;
}

template<typename EnumType, std::enable_if_t<std::is_enum<EnumType>::value, bool> = true>
constexpr EnumType operator &= (EnumType &lhs, EnumType rhs)
{
	lhs = lhs & rhs;
	return lhs;
}

template<typename EnumType, std::enable_if_t<std::is_enum<EnumType>::value, bool> = true>
constexpr EnumType operator ^= (EnumType &lhs, EnumType rhs)
{
	lhs = lhs ^ rhs;
	return lhs;
}

template<typename EnumType, typename ShiftType, std::enable_if_t<std::is_enum<EnumType>::value, bool> = true>
constexpr EnumType operator <<= (EnumType &lhs, ShiftType rhs)
{
	lhs = lhs << rhs;
	return lhs;
}

template<typename EnumType, typename ShiftType, std::enable_if_t<std::is_enum<EnumType>::value, bool> = true>
constexpr EnumType operator >>= (EnumType &lhs, ShiftType rhs)
{
	lhs = lhs >> rhs;
	return lhs;
}


// This header just holds a number of low level function declarations, they function themselves will more then likely be 
// write in ASM.

// Defines for the inline ASM so they can easily be updated/Replaced
#define ASM_HLT __asm hlt

#define ASM_CLI __asm cli
#define ASM_STI __asm sti

#define ASM_SGDT(p) __asm sgdt [p]
#define ASM_LGDT(p) __asm lgdt [p]

#define ASM_SIDT(p) __asm sidt [p]
#define ASM_LIDT(p) __asm lidt [p]

#define ASM_STR(p) __asm str [p]
#define ASM_LTR(p) __asm ltr [p]

#define ASM_INVLPG(p) __asm invlpg [p]

#define ASM_ReadReg(reg, val) __asm { __asm mov [val], reg }
#define ASM_WriteReg(reg, val) __asm { __asm mov reg, [val] }

#pragma pack(push, 1)

struct SegmentContext
{
	uint32_t ES;
	uint32_t DS;
	uint32_t FS;
	uint32_t GS;
};

struct FullPointer
{
	uint32_t Address;
	uint32_t Selector;
};

struct InterruptReturnContext
{
	// Return Address and flags
	// EIP/CS
	FullPointer		Return;
	uint32_t		EFlags;
	
	// Old stack if there was a stack switch
	// ESP/SS
	FullPointer		Stack;

	// Virtual X86 Mode segments if EFlags.VM = 1
	SegmentContext	V86Segments;
};

struct RegisterContext
{
	// In PUSHAD/POPAD order
	union 
	{
		uint32_t EDI;
		uint16_t DI;
	};
	
	union 
	{
		uint32_t ESI;
		uint16_t SI;
	};

	union 
	{
		uint32_t EBP;
		uint16_t BP;
	};

	union 
	{
		uint32_t ESP;
		uint16_t SP;
	};

	union 
	{
		uint32_t EBX;
		uint16_t BX;
		struct
		{
			uint8_t BL;
			uint8_t BH;
		};
	};

	union 
	{
		uint32_t EDX;
		uint16_t DX;
		struct
		{
			uint8_t DL;
			uint8_t DH;
		};
	};

	union 
	{
		uint32_t ECX;
		uint16_t CX;
		struct
		{
			uint8_t CL;
			uint8_t CH;
		};
	};

	union 
	{
		uint32_t EAX;
		uint16_t AX;
		struct
		{
			uint8_t AL;
			uint8_t AH;
		};
	};
};

struct InterruptContext
{
	SegmentContext			Segments;
	RegisterContext			Registers;
	uint32_t				InterruptNumber;
	uint32_t				ErrorCode;
	InterruptReturnContext	Origin;
};

struct Registers
{
	union 
	{
		uint32_t EAX;
		struct
		{
			uint8_t AL;
			uint8_t AH;
		};
	};

	union 
	{
		uint32_t EBX;
		struct
		{
			uint8_t BL;
			uint8_t BH;
		};
	};

	union 
	{
		uint32_t ECX;
		struct
		{
			uint8_t CL;
			uint8_t CH;
		};
	};

	union 
	{
		uint32_t EDX;
		struct
		{
			uint8_t DL;
			uint8_t DH;
		};
	};

	uint32_t ESI;
	uint32_t EDI;

	uint32_t EFlags;
};

namespace CPUFlags
{
	enum IDs
	{
		// CR0
		ProtectedMode						= 0x00000001,		// 0
		MonitorCoprocessor					= 0x00000002,		// 1
		FPUEmulation						= 0x00000004,		// 2
		TaskSwitched						= 0x00000008,		// 3
		ExtensionType						= 0x00000010,		// 4
		NumericException					= 0x00000020,		// 5
		// Reserved							= 0x00000040,		// 6
		// Reserved							= 0x00000080,		// 7
		// Reserved							= 0x00000100,		// 8
		// Reserved							= 0x00000200,		// 9
		// Reserved							= 0x00000400,		// 10
		// Reserved							= 0x00000800,		// 11
		// Reserved							= 0x00001000,		// 12
		// Reserved							= 0x00002000,		// 13
		// Reserved							= 0x00004000,		// 14
		// Reserved							= 0x00008000,		// 15
		WriteProtection						= 0x00010000,		// 16
		// Reserved							= 0x00020000,		// 17
		AlignmentMask						= 0x00040000,		// 18
		// Reserved							= 0x00080000,		// 19
		// Reserved							= 0x00100000,		// 20
		// Reserved							= 0x00200000,		// 21
		// Reserved							= 0x00400000,		// 22
		// Reserved							= 0x00800000,		// 23
		// Reserved							= 0x01000000,		// 24
		// Reserved							= 0x02000000,		// 25
		// Reserved							= 0x04000000,		// 26
		// Reserved							= 0x08000000,		// 27
		// Reserved							= 0x10000000,		// 28
		NotWriteThrough						= 0x20000000,		// 29
		CachingDisabled						= 0x40000000,		// 30
		PagingEnabled						= 0x80000000,		// 31

		// CR3		
		PageLevelWriteThrough				= 0x00000008,
		PageLevelCatchDisabled				= 0x00000010,

		// CR4
		Virtual8086Extentions				= 0x00000001,		// 0
		ProtectedModeVirtualIntperrupts		= 0x00000002,		// 1
		TimeStampDisable					= 0x00000004,		// 2
		DebuggingExtensionsEnable			= 0x00000008,		// 3	Check
		PageSizeExtensionsEnable			= 0x00000010,		// 4	Check
		PhysicalAddressExtensionsEnable 	= 0x00000020,		// 5	Check
		MachineCheckEnable					= 0x00000040,		// 6
		PageGlobalEnable					= 0x00000080,		// 7
		PreformanceCounterEnabled			= 0x00000100,		// 8
		OSFXSR								= 0x00000200,		// 9	Operating System Support for FXSAVE and FXRSTOR instructions
		OSXMMEXCPT							= 0x00000400,		// 10	Operating System Support for Unmasked SIMD Floating-Point Exceptions 
		// Reserved							= 0x00000800,		// 11
		// Reserved							= 0x00001000,		// 12
		VMXEnable							= 0x00002000,		// 13
		SMXEnable							= 0x00004000,		// 14
		// Reserved							= 0x00008000,		// 15
		FSGSBASEEnable						= 0x00010000,		// 16
		PCIDEnable							= 0x00020000,		// 17
		OSXSAVEEnabled						= 0x00040000,		// 18	XSAVE and Processor Extended States-Enable Bit
		// Reserved							= 0x00080000,		// 19
		SMEPEnable							= 0x00100000,		// 20
		// Reserved							= 0x00200000,		// 21
		// Reserved							= 0x00400000,		// 22
		// Reserved							= 0x00800000,		// 23
		// Reserved							= 0x01000000,		// 24
		// Reserved							= 0x02000000,		// 25
		// Reserved							= 0x04000000,		// 26
		// Reserved							= 0x08000000,		// 27
		// Reserved							= 0x10000000,		// 28
		// Reserved							= 0x20000000,		// 29
		// Reserved							= 0x40000000,		// 30
		// Reserved							= 0x80000000,		// 31

		// XCR0
		X87FPUState							= 0x00000001,		// 0
		SSEState							= 0x00000002,		// 1
		AVXState							= 0x00000004,		// 2

		// CPUID - EDX
		FPUOnChip							= 0x00000001,		// 0
		Virtual8086Enhancments				= 0x00000002,		// 1
		DebuggingExtensions					= 0x00000004,		// 2
		PageSizeExtensions					= 0x00000008,		// 3
		TimeStampCounter					= 0x00000010,		// 4
		MSRs								= 0x00000020,		// 5
		PhysicalAddressExtensions			= 0x00000040,		// 6
		MachineCheck						= 0x00000080,		// 7
		CX8									= 0x00000100,		// 8
		APICOnChip							= 0x00000200,		// 9
		// Reserved							= 0x00000400,		// 10
		SYSInstructions						= 0x00000800,		// 11
		MemTypeRangeRegs					= 0x00001000,		// 12
		PageGlobal							= 0x00002000,		// 13
		MachineCheckArchitecture			= 0x00004000,		// 14
		ConditionalMove						= 0x00008000,		// 15
		PAT									= 0x00010000,		// 16
		PageSizeExtensions36				= 0x00020000,		// 17
		ProcessorSerialNumber				= 0x00040000,		// 18
		CLFSH								= 0x00080000,		// 19
		// Reserved							= 0x00100000,		// 20
		DebugStore							= 0x00200000,		// 21
		ThermalMonitorAndClock				= 0x00400000,		// 22
		MMX									= 0x00800000,		// 23
		FXSaveRestore						= 0x01000000,		// 24
		SSE									= 0x02000000,		// 25
		SSE2								= 0x04000000,		// 26
		SelfSnoop							= 0x08000000,		// 27
		MaxAPICIDValid						= 0x10000000,		// 28
		ThermalMonitor						= 0x20000000,		// 29
		// Reserved							= 0x40000000,		// 30
		PendingBreakEnable					= 0x80000000,		// 31

		// CPUID - ECX
		SSE3								= 0x00000001,		// 0
		PCLMULQDQ							= 0x00000002,		// 1
		DSArea64Bit							= 0x00000004,		// 2
		Monitor								= 0x00000008,		// 3
		CPUDebucStore						= 0x00000010,		// 4
		VirtualMachineExtensions			= 0x00000020,		// 5
		SaferModeExtensions					= 0x00000040,		// 6
		EnchacedSpeedStep					= 0x00000080,		// 7
		ThermalMonitor2						= 0x00000100,		// 8
		SSSE3								= 0x00000200,		// 9
		L1ContextID							= 0x00000400,		// 10
		SiliconDebug						= 0x00000800,		// 11
		FMAExtensions						= 0x00001000,		// 12
		CMPXCHG16B							= 0x00002000,		// 13
		xTPRUpdate							= 0x00004000,		// 14
		PerfmonDebug						= 0x00008000,		// 15
		// Reserved							= 0x00010000,		// 16
		PCID								= 0x00020000,		// 17
		DCAPrefetch							= 0x00040000,		// 18
		SSE4_1								= 0x00080000,		// 19
		SSE4_2								= 0x00100000,		// 20
		x2APIC								= 0x00200000,		// 21
		MOVEBE								= 0x00400000,		// 22
		POPCNT								= 0x00800000,		// 23
		TSCDeadline							= 0x01000000,		// 24
		AESInstructions						= 0x02000000,		// 25
		XSAVE								= 0x04000000,		// 26
		OSXSAVE								= 0x08000000,		// 27
		AVXInstuctions						= 0x10000000,		// 28
		F16C								= 0x20000000,		// 29
		RDRAND								= 0x40000000,		// 30
		// Reserved							= 0x80000000,		// 31

		// CPUID 7 - EBX
		FSGSBASE							= 0x00000001,		// 0
		IA32_TSC_ADJUST						= 0x00000002,		// 1
		// Reserved							= 0x00000004,		// 2
		BMI1								= 0x00000008,		// 3
		HLE									= 0x00000010,		// 4
		AVX2								= 0x00000020,		// 5
		// Reserved							= 0x00000040,		// 6
		SMEP								= 0x00000080,		// 7
		BMI2								= 0x00000100,		// 8
		Enhanced_MOVSB						= 0x00000200,		// 9
		INVPCID								= 0x00000400,		// 10
		RTM									= 0x00000800,		// 11
		PQM									= 0x00001000,		// 12
		FPU_CS_DS_Deprecated				= 0x00002000,		// 13
		PQE									= 0x00004000,		// 14
		// Reserved							= 0x00008000,		// 15
		// Reserved							= 0x00010000,		// 16
		// Reserved							= 0x00020000,		// 17
		// Reserved							= 0x00040000,		// 18
		// Reserved							= 0x00080000,		// 19
		// Reserved							= 0x00100000,		// 20
		// Reserved							= 0x00200000,		// 21
		// Reserved							= 0x00400000,		// 22
		// Reserved							= 0x00800000,		// 23
		// Reserved							= 0x01000000,		// 24
		// Reserved							= 0x02000000,		// 25
		// Reserved							= 0x04000000,		// 26
		// Reserved							= 0x08000000,		// 27
		// Reserved							= 0x10000000,		// 28
		// Reserved							= 0x20000000,		// 29
		// Reserved							= 0x40000000,		// 30
		// Reserved							= 0x80000000,		// 31

		// CPUID 80000001 - EDX
		// Reserved							= 0x00000001,		// 0
		// Reserved							= 0x00000002,		// 1
		// Reserved							= 0x00000004,		// 2
		// Reserved							= 0x00000008,		// 3
		// Reserved							= 0x00000010,		// 4
		// Reserved							= 0x00000020,		// 5
		// Reserved							= 0x00000040,		// 6
		// Reserved							= 0x00000080,		// 7
		// Reserved							= 0x00000100,		// 8
		// Reserved							= 0x00000200,		// 9
		// Reserved							= 0x00000400,		// 10
		SYSCALL 							= 0x00000800,		// 11
		// Reserved							= 0x00001000,		// 12
		// Reserved							= 0x00002000,		// 13
		// Reserved							= 0x00004000,		// 14
		// Reserved							= 0x00008000,		// 15
		// Reserved							= 0x00010000,		// 16
		// Reserved							= 0x00020000,		// 17
		// Reserved							= 0x00040000,		// 18
		// Reserved							= 0x00080000,		// 19
		ExecuteDisable						= 0x00100000,		// 20
		// Reserved							= 0x00200000,		// 21
		// Reserved							= 0x00400000,		// 22
		// Reserved							= 0x00800000,		// 23
		// Reserved							= 0x01000000,		// 24
		// Reserved							= 0x02000000,		// 25
		GBytePages							= 0x04000000,		// 26
		RDSTCP								= 0x08000000,		// 27
		// Reserved							= 0x10000000,		// 28
		EMT64								= 0x20000000,		// 29
		// Reserved							= 0x40000000,		// 30
		// Reserved							= 0x80000000,		// 31

		// CPUID 80000001 - ECX
		LAHF								= 0x00000001,		// 0
		// Reserved							= 0x00000002,		// 1
		// Reserved							= 0x00000004,		// 2
		// Reserved							= 0x00000008,		// 3
		// Reserved							= 0x00000010,		// 4
		LZCNT								= 0x00000020,		// 5
		// Reserved							= 0x00000040,		// 6
		// Reserved							= 0x00000080,		// 7
		PREFETCHW							= 0x00000100,		// 8
		// Reserved							= 0x00000200,		// 9
		// Reserved							= 0x00000400,		// 10
		// Reserved							= 0x00000800,		// 11
		// Reserved							= 0x00001000,		// 12
		// Reserved							= 0x00002000,		// 13
		// Reserved							= 0x00004000,		// 14
		// Reserved							= 0x00008000,		// 15
		// Reserved							= 0x00010000,		// 16
		// Reserved							= 0x00020000,		// 17
		// Reserved							= 0x00040000,		// 18
		// Reserved							= 0x00080000,		// 19
		// Reserved							= 0x00100000,		// 20
		// Reserved							= 0x00200000,		// 21
		// Reserved							= 0x00400000,		// 22
		// Reserved							= 0x00800000,		// 23
		// Reserved							= 0x01000000,		// 24
		// Reserved							= 0x02000000,		// 25
		// Reserved							= 0x04000000,		// 26
		// Reserved							= 0x08000000,		// 27
		// Reserved							= 0x10000000,		// 28
		// Reserved							= 0x20000000,		// 29
		// Reserved							= 0x40000000,		// 30
		// Reserved							= 0x80000000,		// 31

	};
}

#define ALIGN_32BIT		__declspec(align(32)) 

namespace EFlags
{
	enum Flags
	{
		Carry						= 0x00000001,		// 0
		// Reserved	(Always 1)		= 0x00000002,		// 1
		Parity						= 0x00000004,		// 2
		// Reserved					= 0x00000008,		// 3
		AuxiliaryCarry				= 0x00000010,		// 4
		// Reserved					= 0x00000020,		// 5
		Zero						= 0x00000040,		// 6
		Sign						= 0x00000080,		// 7
		Trap						= 0x00000100,		// 8
		InterruptEnable				= 0x00000200,		// 9
		Direction					= 0x00000400,		// 10
		Overflow					= 0x00000800,		// 11
		IOPL						= 0x00003000,		// 12 & 13
		NestedTask					= 0x00004000,		// 14
		// Reserved					= 0x00008000,		// 15
		Resume						= 0x00010000,		// 16
		Virtual8086Mode				= 0x00020000,		// 17
		AlignmentCheck				= 0x00040000,		// 18
		VirtualInterruptFlag		= 0x00080000,		// 19
		VirtualInterruptPending		= 0x00100000,		// 20
		CPUID						= 0x00200000,		// 21
		// Reserved					= 0x00400000,		// 22
		// Reserved					= 0x00800000,		// 23
		// Reserved					= 0x01000000,		// 24
		// Reserved					= 0x02000000,		// 25
		// Reserved					= 0x04000000,		// 26
		// Reserved					= 0x08000000,		// 27
		// Reserved					= 0x10000000,		// 28
		// Reserved					= 0x20000000,		// 29
		// Reserved					= 0x40000000,		// 30
		// Reserved					= 0x80000000,		// 31

	};
}

extern "C"
{
	void SwapSelectors(uint32_t CodeSeg, uint32_t DataSeg);

	void OutPortb(uint16_t Port, uint8_t Value);
	void OutPortw(uint16_t Port, uint16_t Value);
	void OutPortd(uint16_t Port, uint32_t Value);

	uint8_t  InPortb(uint16_t Port);
	uint16_t InPortw(uint16_t Port);
	uint32_t InPortd(uint16_t Port);

	uint32_t ReadCR0();
	uint32_t ReadCR2();
	uint32_t ReadCR3();
	uint32_t ReadCR4();
	uint64_t ReadXCR0();

	void WriteCR0(uint32_t Value);
	void WriteCR2(uint32_t Value);
	void WriteCR3(uint32_t Value);
	void WriteCR4(uint32_t Value);
	void WriteXCR0(uint64_t Value);

	uint32_t ReadFS(uint32_t Offset);
	uint32_t ReadGS(uint32_t Offset);

	void ReadCPUID(uint32_t Value, uint32_t Value2, Registers *Result);

	uint64_t ReadMSR(uint32_t Register);
	void WriteMSR(uint32_t Register, uint64_t Value);

	void FireInt(uint32_t IntNum, Registers *Result);

	void CallService(uint32_t ServiceAddress, Registers *Result);
	void StartV86(uint16_t CS, uint16_t IP, uint16_t SS, uint16_t SP);

	uint16_t SetLDT(uint16_t Selector);
	uint16_t SetTR(uint16_t Selector);
};



#pragma pack(pop)