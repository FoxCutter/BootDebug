#include <stdint.h>
#pragma once

struct InterruptContext;
class IDTManager;
typedef void (*InterruptCallbackPtr)(InterruptContext * Context, uintptr_t * Data);

typedef struct acpi_table_madt ACPI_TABLE_MADT;

enum class ACPIOffsets;

// Priority level for the interupts, using the APIC fixed int rules (High number has higher priority)
// If it's in Mixed or PIC mode these don't matter AT ALL.
enum class IntPriority
{
	System		= 0xF0,	// 0xF0 - 0xFF

	Higest		= 0xE0, // 0xE0 - 0xEF
	Higher		= 0xD0,	// 0xD0 - 0xDF
	High		= 0xA0, // 0xA0 - 0xCF

	Any			= 0x70, // 0x70 - 0x9F
	
	Low			= 0x40, // 0x40 - 0x6F
	Lower		= 0x30, // 0X30 - 0x3F
	Lowest		= 0x20, // 0x20 - 0x2F
};

class InterruptControler
{
	enum class PICMode
	{
		// Use the full I/O APIC, supports multi-processor, Intlines variable (based on what the IOAPIC supports)
		// Also does automatic rerouting of the old IRQs (0x0 - 0xF) to the new intlines if there is mapping defined in the
		// APIC table. INT vectors will be allocated based on Priority and need.
		IOAPIC = 0x0F,

		// Mixed Mode, Use the APIC with default routing and the PIC with fixed Priorty
		Mixed  = 0x80,		
	
		// Old PIC mode, Intlines 0x0-0xF, With fixed priority (0, 1, 2, 8, 9, A, B, C, D, E, F, 3, 4, 5, 6, 7)
		Legacy = 0xFF,
	};

	struct MappingData
	{
		uint8_t Vector;	// The vector that the IRQ is mapped to 
		uint8_t Mapping;	// What I/O APIC slot this IRQ is mapped with (IRQ 0 is usualy Slot 2)
		InterruptCallbackPtr InterruptCallback;
		uintptr_t * Data;
		
		uint32_t VectorMode;	// High/Low, Edge/Level

	};
	
	MappingData *Mapping;

	// The base of the two IRQ blocks
	PICMode Mode;
	uint32_t APICRegisterBase;
	uint32_t IOAPICRegisterBase;
	uint8_t VectorCount;
	uint8_t BaseVector;

	uint8_t IRQBase1;
	uint8_t IRQBase2;

	IDTManager *m_IDTManager;

	uint32_t GetAPICRegister(ACPIOffsets Reg);
	void SetAPICRegister(ACPIOffsets Reg, uint32_t Value);
	void PrintAPICIntStatus(ACPIOffsets StartIndex);

	uint32_t GetIOAPICRegister(int Reg);
	void SetIOAPICRegister(int Reg, uint32_t Value);

	void SetIOAPICVector(int Vector, uint64_t Value);

	static void IRQInterrupt(InterruptContext * Context, uintptr_t * Data);
	void Interrupt(InterruptContext * Context);

public:
	InterruptControler(void);
	~InterruptControler(void);
	
	void Initialize(IDTManager *IDTManager, ACPI_TABLE_MADT *MADT);


	void SetIRQInterrupt(uint8_t IntLine, IntPriority Priority, InterruptCallbackPtr InterruptCallback, uintptr_t * Data = nullptr);

	void SetSpuriousInterruptVector(uint8_t Vector);

	void ClearIRQ(uint8_t IRQ);
	void EnableIRQ(uint8_t IRQ);
	void DisableIRQ(uint8_t IRQ);

	uint8_t MapIntToIRQ(uint8_t Int);
	uint8_t MapIRQToInt(uint8_t IRQ);

	void SignalInterrupt(uint8_t Int);

	void DumpPIC();
	void DumpAPIC();
	void DumpIOAPIC();
};

