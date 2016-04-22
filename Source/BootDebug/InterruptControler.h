#include <stdint.h>
#pragma once

struct InterruptContext;
class IDTManager;
typedef void (*InterruptCallbackPtr)(InterruptContext * Context, uintptr_t * Data);

enum class ACPIOffsets;

class InterruptControler
{
	enum PICMode
	{
		ePIC,
		eAPIC,
		eIOAPIC,
	};

	struct MappingData
	{
		uint16_t Vector;	// The vector that the IRQ is mapped to 
		uint16_t Mapping;	// What I/O APIC slot this IRQ is mapped with (IRQ 0 is usualy Slot 2)
		InterruptCallbackPtr InterruptCallback;
		uintptr_t * Data;
	} Mapping[0x20];

	// The base of the two IRQ blocks
	PICMode Mode;
	uint32_t APICRegisterBase;
	uint32_t IOAPICRegisterBase;
	uint8_t IRQBase1;
	uint8_t IRQBase2;

	IDTManager *m_IDTManager;

	uint32_t GetAPICRegister(ACPIOffsets Reg);
	void SetAPICRegister(ACPIOffsets Reg, uint32_t Value);
	void PrintAPICIntStatus(ACPIOffsets StartIndex);

	uint32_t GetIOAPICRegister(int Reg);
	void SetIOAPICRegister(int Reg, uint32_t Value);


public:
	InterruptControler(void);
	~InterruptControler(void);

	void Interrupt(InterruptContext * Context);

	void SetIDT(int InterruptBase, IDTManager *IDTManager);
	void SetIRQInterrupt(uint8_t IRQ, InterruptCallbackPtr InterruptCallback, uintptr_t * Data = nullptr);

	void SetSpuriousInterruptVector(uint8_t Vector);
	void RemapIRQBase(uint8_t NewBase);
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

