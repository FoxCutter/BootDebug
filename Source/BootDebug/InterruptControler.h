#include <stdint.h>
#pragma once

struct InterruptContext;
class IDTManager;
typedef void (*InterruptCallbackPtr)(InterruptContext * Context, uintptr_t * Data);

class InterruptControler
{
	struct MappingData
	{
		InterruptCallbackPtr InterruptCallback;
		uintptr_t * Data;
	} Mapping[0x10];

	// The base of the two IRQ blocks
	uint8_t IRQBase1;
	uint8_t IRQBase2;

	IDTManager *m_IDTManager;

public:
	InterruptControler(void);
	~InterruptControler(void);

	void Interrupt(InterruptContext * Context);

	void SetIDT(int InterruptBase, IDTManager *IDTManager);
	void SetIRQInterrupt(uint8_t IRQ, InterruptCallbackPtr InterruptCallback, uintptr_t * Data = nullptr);

	void RemapIRQBase(uint8_t NewBase);
	void ClearIRQ(uint8_t IRQ);
	void EnableIRQ(uint8_t IRQ);
	void DisableIRQ(uint8_t IRQ);

	uint8_t MapIntToIRQ(uint8_t Int);
	uint8_t MapIRQToInt(uint8_t IRQ);
};

