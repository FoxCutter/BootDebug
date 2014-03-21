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