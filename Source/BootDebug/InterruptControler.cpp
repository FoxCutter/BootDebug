#include "InterruptControler.h"
#include "LowLevel.h"


InterruptControler::InterruptControler(void)
{
	// We'll start up assuming things are where they are in 16-bit mode
	//IRQBase1 = 0x08;
	//IRQBase2 = 0x70;
}

InterruptControler::~InterruptControler(void)
{
}

uint8_t InterruptControler::MapIntToIRQ(uint8_t Int)
{
	if(Int >= IRQBase1 && Int < IRQBase1 + 8)
	{
		return Int - IRQBase1;
	}

	if(Int >= IRQBase2 && Int < IRQBase2 + 8)
	{
		return Int - IRQBase2;
	}

	return 0xFF;
}

uint8_t InterruptControler::MapIRQToInt(uint8_t IRQ)
{
	if(IRQ >= 8)
		return IRQBase2 + (IRQ - 8);

	return IRQBase1 + IRQ;
}

void InterruptControler::RemapIRQBase(uint8_t NewBase)
{
	// Start with the Initialize code
	OutPortb(0x20, 0x11);
    OutPortb(0xA0, 0x11);
    
	// When we remap the IRQs we will make them linear
	IRQBase1 = NewBase;
	IRQBase2 = NewBase + 8;

	// ICW2 Is the vector offset
	OutPortb(0x21, IRQBase1);
    OutPortb(0xA1, IRQBase2);
    
	// ICW3 is the master slave relationship
	OutPortb(0x21, 0x04);	// Tell chip one that it has a slave on IRQ 2
    OutPortb(0xA1, 0x02);   
    
	// ICW4 is the environment info
	OutPortb(0x21, 0x01);	// We are still x86
    OutPortb(0xA1, 0x01);
    
	// Clear all the IRQ masks
	OutPortb(0x21, 0x00);
    OutPortb(0xA1, 0x00);
}

void InterruptControler::ClearIRQ(uint8_t IRQ)
{
	if (IRQ >= 8)
        OutPortb(0xA0, 0x20);

	OutPortb(0x20, 0x20);
}

void InterruptControler::EnableIRQ(uint8_t IRQ)
{
	if (IRQ >= 8)
	{
		uint8_t Mask = InPortb(0xA1);
		Mask &= ~(1 << (IRQ - 8));

		OutPortb(0xA1, Mask);
	}
	else
	{
		uint8_t Mask = InPortb(0x21);
		Mask &= ~(1 << IRQ);

		OutPortb(0x21, Mask);
	}
}

void InterruptControler::DisableIRQ(uint8_t IRQ)
{
	if (IRQ >= 8)
	{
		uint8_t Mask = InPortb(0xA1);
		Mask |= (1 << (IRQ - 8));

		OutPortb(0xA1, Mask);
	}
	else
	{
		uint8_t Mask = InPortb(0x21);
		Mask |= (1 << IRQ);

		OutPortb(0x21, Mask);
	}
}
