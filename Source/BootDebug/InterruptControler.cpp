#include "InterruptControler.h"
#include "LowLevel.h"
#include "IDT.h"

#include <stdio.h>

enum ACPIOffsets
{
	LocalACPIID				= 0x02,
	LocalACPIVersion		= 0x03,

	TaskPriority			= 0x08,
	ArbitrantionPriority	= 0x09,
	ProcessorPriority		= 0x0A,
	EndOfInterrupt			= 0x0B,
	RemoteRead				= 0x0C,
	LogicalDestination		= 0x0D,
	DestinationFormat		= 0x0E,
	SpuriousInterruptVector	= 0x0F,
	InServiceBase			= 0x10,
	InServiceMax			= 0x17,
	TriggerModeBase			= 0x18,
	TriggerModeMax			= 0x1F,
	InterruptRequestBase	= 0x20,
	InterruptRequestMax		= 0x27,
	ErrorStatus				= 0x28,

	LVTMachineCheck			= 0x2F,
	InterruptCommandLow		= 0x30,
	InterruptCommandHigh	= 0x31,
	LVTTimer				= 0x32,
	LVTThermalSensor		= 0x33,
	LVTPerformanteCounter	= 0x34,
	LVTInt0					= 0x35,
	LVTInt1					= 0x36,
	LVTErrro				= 0x37,
	TimerInitialCount		= 0x38,
	TimerCurrentCount		= 0x39,

	TimerDeviceConfig		= 0x3E,
};


static void IRQInterrupt(InterruptContext * Context, uintptr_t * Data)
{
	if(Data == nullptr)
		return;

	InterruptControler * Controler = reinterpret_cast<InterruptControler *>(Data);

	Controler->Interrupt(Context);
}

InterruptControler::InterruptControler(void)
{
	UsingAPIC = false;
	APICRegisterBase = 0;

	for(int x = 0; x < 0x10; x++)
	{
		Mapping[x].Data = nullptr;
		Mapping[x].InterruptCallback = nullptr;
	}

	// Use the APIC if it exits
	Registers Res;
	ReadCPUID(1, 0, &Res);

	if((Res.EDX & CPUFlags::APICOnChip))
	{
		uint64_t APICBase = ReadMSR(0x1B);

		if(APICBase & 0x800)
		{
			// Grab the register
			APICRegisterBase = APICBase & 0xFFFFF000;
			UsingAPIC = true;
		}
	}
}

uint32_t InterruptControler::GetAPICRegister(int Reg)
{
	if(!UsingAPIC)
		return 0xFFFFFFFF;

	uint32_t *Register = reinterpret_cast<uint32_t *>(APICRegisterBase + (Reg * 0x10));

	return *Register;
}

void InterruptControler::SetAPICRegister(int Reg, uint32_t Value)
{
	if(!UsingAPIC)
		return;

	uint32_t *Register = reinterpret_cast<uint32_t *>(APICRegisterBase + (Reg * 0x10));

	*Register = Value;
}

InterruptControler::~InterruptControler(void)
{
}

void InterruptControler::Interrupt(InterruptContext * Context)
{
	uint8_t IRQ = MapIntToIRQ(Context->InterruptNumber);

	if(IRQ < 0x10)
	{
		if(Mapping[IRQ].InterruptCallback != nullptr)
			Mapping[IRQ].InterruptCallback(Context, Mapping[IRQ].Data);
	}

	ClearIRQ(IRQ);
}

void InterruptControler::SetIDT(int InterruptBase, IDTManager *oIDTManager)
{
	m_IDTManager = oIDTManager;

	// Set up the handlers for IRQs
	for(int x = InterruptBase; x < InterruptBase + 0x10; x++)
	{
		m_IDTManager->SetInterupt(x, IRQInterrupt, reinterpret_cast<uintptr_t *>(this));
	}
}

void InterruptControler::SetIRQInterrupt(uint8_t IRQ, InterruptCallbackPtr InterruptCallback, uintptr_t * Data)
{
	if(IRQ < 0x10)
	{
		Mapping[IRQ].InterruptCallback = InterruptCallback;
		Mapping[IRQ].Data = Data;
	}
}

uint8_t InterruptControler::MapIntToIRQ(uint8_t Int)
{
	if(Int >= IRQBase1 && Int < IRQBase1 + 8)
	{
		return Int - IRQBase1;
	}

	if(Int >= IRQBase2 && Int < IRQBase2 + 8)
	{
		return (Int - IRQBase2) + 8;
	}

	return 0xFF;
}

uint8_t InterruptControler::MapIRQToInt(uint8_t IRQ)
{
	if(IRQ >= 8)
		return IRQBase2 + (IRQ - 8);

	return IRQBase1 + IRQ;
}

void InterruptControler::SetSpuriousInterruptVector(uint8_t Vector)
{
	if(!UsingAPIC)
		return;

	SetAPICRegister(SpuriousInterruptVector, (GetAPICRegister(SpuriousInterruptVector) & 0xFFFFFF00) | Vector);	
}

void InterruptControler::RemapIRQBase(uint8_t NewBase)
{
	if(UsingAPIC)
	{
		// Mask off all the IRQs, as it will be coming through the APIC now.		
		OutPortb(0x21, 0xFF);
		OutPortb(0xA1, 0xFF);
	}

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
    
	if(UsingAPIC)
	{
		// Officially turn the APIC on
		SetAPICRegister(SpuriousInterruptVector, GetAPICRegister(SpuriousInterruptVector) | 0x100);	
	}
	else
	{
		// Clear all the IRQ masks
		OutPortb(0x21, 0x00);
		OutPortb(0xA1, 0x00);
	}
}

void InterruptControler::ClearIRQ(uint8_t IRQ)
{
	if (IRQ >= 8)
		OutPortb(0xA0, 0x20);

	OutPortb(0x20, 0x20);
}

void InterruptControler::EnableIRQ(uint8_t IRQ)
{
	if(UsingAPIC)
	{
	}
	else
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
}

void InterruptControler::DisableIRQ(uint8_t IRQ)
{
	if(UsingAPIC)
	{
	}
	else
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
}
