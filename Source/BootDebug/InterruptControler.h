#include <stdint.h>
#pragma once
class InterruptControler
{
	// The base of the two IRQ blocks
	uint8_t IRQBase1;
	uint8_t IRQBase2;

public:
	InterruptControler(void);
	~InterruptControler(void);

	void RemapIRQBase(uint8_t NewBase);
	void ClearIRQ(uint8_t IRQ);
	void EnableIRQ(uint8_t IRQ);
	void DisableIRQ(uint8_t IRQ);

	uint8_t MapIntToIRQ(uint8_t Int);
	uint8_t MapIRQToInt(uint8_t IRQ);
};

