#include <stdint.h>
#pragma once

struct InterruptContext;
class IDTManager;
typedef void (*InterruptCallbackPtr)(InterruptContext * Context, uintptr_t * Data);

typedef struct acpi_table_madt ACPI_TABLE_MADT;

enum class ACPIOffsets;

class InterruptControler
{
	enum class PICMode
	{
		Legacy = 0xFF,
		Mixed  = 0x80,		
		IOAPIC = 0x0F,
	};

	struct MappingData
	{
		uint8_t Vector;	// The vector that the IRQ is mapped to 
		uint8_t Mapping;	// What I/O APIC slot this IRQ is mapped with (IRQ 0 is usualy Slot 2)
		InterruptCallbackPtr InterruptCallback;
		uintptr_t * Data;
		
		uint32_t VectorMode;	// High/Low, Edge/Level

	} Mapping[0x20];

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


public:
	InterruptControler(void);
	~InterruptControler(void);

	void Initialize(ACPI_TABLE_MADT *MADT, uint8_t LowestVector);

	
	
	void Interrupt(InterruptContext * Context);

	void SetIDT(IDTManager *IDTManager);
	void SetIRQInterrupt(uint8_t IRQ, InterruptCallbackPtr InterruptCallback, uintptr_t * Data = nullptr);

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

