#include <stdint.h>
#pragma once

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

#define ASM_INVLPG(p) __asm invlpg [p]

#define ASM_SCR(reg, val) __asm { \
	mov eax, reg \
	mov ecx, eax \
}
#define ASM_LCR(reg) __asm mov reg, eax

#pragma pack(push, 1)
struct InterruptContext
{
	// Added by the Int handler
	uint32_t GS;
	uint32_t FS;
	uint32_t ES;
	uint32_t DS;

	// Added by Pushad
	uint32_t EDI;
	uint32_t ESI;
	uint32_t EBP;
	uint32_t ESP;

	uint32_t EBX;
	uint32_t EDX;
	uint32_t ECX;
	uint32_t EAX;

	// Added by the per Int handler
	uint32_t InterruptNumber;
	uint32_t ErrorCode;

	// From the CPU itself (SS:ESP will only be on the stack if we had a Privilege Level Switch, otherwise it will be noise)
	uint32_t SourceEIP;
	uint32_t SourceCS;
	uint32_t SourceEFlags;
	uint32_t SourceESP;
	uint32_t SourceSS;
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
		ProtectedMode						= 0x00000001,
		MonitorCoprocessor					= 0x00000002,
		FPUEmulation						= 0x00000004,
		TaskSwitched						= 0x00000008,
		ExtensionType						= 0x00000010,
		NumericError						= 0x00000020,
		WriteProtection						= 0x00010000,
		AlignmentMask						= 0x00040000,
		NotWriteThrough						= 0x20000000,
		CachingDisabled						= 0x40000000,
		PagingEnabled						= 0x80000000,

		// CR3		
		PageLevelWriteThrough				= 0x00000008,
		PageLevelCatchDisabled				= 0x00000010,

		// CR4
		Virtual8086Extentions				= 0x00000001,
		ProtectedModeVirtualIntperrupts		= 0x00000002,
		TimeStampDisable					= 0x00000004,
		DebuggingExtensionsEnable			= 0x00000008, // Check
		PageSizeExtensionsEnable			= 0x00000010, // Check
		PhysicalAddressExtensionsEnable 	= 0x00000020, // CCheck
		MachineCheckEnable					= 0x00000040,
		PageGlobalEnable					= 0x00000080,
		PreformanceCounterEnabled			= 0x00000100,
		OSFXSR								= 0x00000200,
		OSXMMEXCPT							= 0x00000400,
		VMXEnable							= 0x00002000,
		SMXEnable							= 0x00004000,
		FSGSBASEEnable						= 0x00010000,
		PCIDEnable							= 0x00020000,
		OSXSAVEEnabled						= 0x00040000, // Check
		SMEPEnable							= 0x00100000,	
		
		// XCR0
		X87FPUState							= 0x00000001,
		SSEState							= 0x00000002,
		AVXState							= 0x00000004,

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

	};
}

#define ALIGN_32BIT		__declspec(align(32)) 

#define CARRY_FLAG		0x0001

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

	void ReadCPUID(uint32_t Value, uint32_t Value2, Registers *Result);

	uint64_t ReadMSR(uint32_t Register);
	void WriteMSR(uint32_t Register, uint64_t Value);

	void FireInt(uint32_t IntNum, Registers *Result);

	void CallService(uint32_t ServiceAddress, Registers *Result);
};


#pragma pack(pop)