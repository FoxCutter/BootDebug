#include "InterruptControler.h"
#include "LowLevel.h"
#include "IDT.h"

#include <stdio.h>

enum class ACPIOffsets
{
	LocalACPIID						= 0x02,
	LocalACPIVersion				= 0x03,

	TaskPriority					= 0x08,
	ArbitrationPriority				= 0x09,
	ProcessorPriority				= 0x0A,
	EndOfInterrupt					= 0x0B,
	RemoteRead						= 0x0C,
	LogicalDestination				= 0x0D,
	DestinationFormat				= 0x0E,
	SpuriousInterruptVector			= 0x0F,
	InServiceBase					= 0x10,
	InServiceMax					= 0x17,
	TriggerModeBase					= 0x18,
	TriggerModeMax					= 0x1F,
	InterruptRequestBase			= 0x20,
	InterruptRequestMax				= 0x27,
	ErrorStatus						= 0x28,

	LVTMachineCheck					= 0x2F,
	InterruptCommandLow				= 0x30,
	InterruptCommandHigh			= 0x31,
	LVTTimer						= 0x32,
	LVTThermalSensor				= 0x33,
	LVTPerformanteCounter			= 0x34,
	LVTInt0							= 0x35,
	LVTInt1							= 0x36,
	LVTError						= 0x37,
	TimerInitialCount				= 0x38,
	TimerCurrentCount				= 0x39,

	TimerDivideConfig				= 0x3E,
};


enum ACPILVTFlags : uint32_t
{
	LVTVectorMask					= 0x000FF,
	
	LVTDeliveryModeMask				= 0x00700,
	LVTDeliveryModeFixed			= 0x00000,
	LVTDeliveryModeLowestPriority	= 0x00100,
	LVTDeliveryModeSMI				= 0x00200,
	LVTDeliveryModeNMI				= 0x00400,
	LVTDeliveryModeINIT				= 0x00500,
	LVTDeliveryModeStartUp			= 0x00600,
	LVTDeliveryModeExtINT			= 0x00700,
	

	LVTDeliveryPending				= 0x01000,

	LVTPinPolarityHigh				= 0x00000,
	LVTPinPolarityLow				= 0x02000,

	// Used for Level Triggered 
	LVTRemoteIRR					= 0x04000,

	LVTTriggerModeEdge				= 0x00000,
	LVTTriggerModeLevel				= 0x08000,

	LVTMasked						= 0x10000,

	// Timer LVT only
	LVTTimeerMask					= 0x60000,
	LVTTimeerOneShot				= 0x00000,
	LVTTimeerPeriodic				= 0x20000,
	LVTTimeerTSCDeadLine			= 0x40000,

	// Interrupt Command
	LVTDestinationShorthandMask		= 0xC0000,
	LVTDestinationShorthandNone		= 0x00000,
	LVTDestinationShorthandSelf		= 0x40000,
	LVTDestinationShorthandAll		= 0x80000,
	LVTDestinationShorthandNotSelf	= 0xC0000,

	// I/O APIC LVTs
	LVTDeliveryModePhysical			= 0x00000,
	LVTDeliveryModeLogical			= 0x00800,

	// This is for the high half the 64-bit register.
	LVTDestinationMask				= 0xFF000000,
	LVTDestinationShift				= 24,

};

enum ACPIErrors
{
	ACPISendChecksumError			= 0x0001,
	ACPIReciveChecksumError			= 0x0002,
	ACPISendAcceptError				= 0x0004,
	ACPIReciveAcceptError			= 0x0008,
	ACPIRedirectableIPIError		= 0x0010,
	ACPISendIllegalVectorError		= 0x0020,
	ACPIReciveIllegalVectorError	= 0x0040,
	ACPIIllegalRegisterAddress		= 0x0080,	
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
	Mode = ePIC;
	APICRegisterBase = 0;
	IOAPICRegisterBase = 0;

	for(int x = 0; x < 0x20; x++)
	{
		Mapping[x].Vector = 0xFFFF;
		Mapping[x].Mapping = x;
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
			Mode = eAPIC;
		}
	}
}

uint32_t InterruptControler::GetAPICRegister(ACPIOffsets Reg)
{
	if(APICRegisterBase == 0)
		return 0x00000000;

	uint32_t *Register = reinterpret_cast<uint32_t *>(APICRegisterBase + (static_cast<uint32_t>(Reg) * 0x10));

	return *Register;
}

void InterruptControler::SetAPICRegister(ACPIOffsets Reg, uint32_t Value)
{
	if(APICRegisterBase == 0)
		return;

	uint32_t *Register = reinterpret_cast<uint32_t *>(APICRegisterBase + (static_cast<uint32_t>(Reg) * 0x10));

	*Register = Value;
}

uint32_t InterruptControler::GetIOAPICRegister(int Reg)
{
	if(IOAPICRegisterBase == 0)
		return 0x00000000;

	uint32_t volatile *RegisterBase = reinterpret_cast<uint32_t *>(IOAPICRegisterBase);

	RegisterBase[0] = Reg;
	return RegisterBase[4];
}

void InterruptControler::SetIOAPICRegister(int Reg, uint32_t Value)
{
	if(IOAPICRegisterBase == 0)
		return;

	uint32_t volatile *RegisterBase = reinterpret_cast<uint32_t *>(IOAPICRegisterBase);

	RegisterBase[0] = Reg;
	RegisterBase[4] = Value;
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
		{
			Mapping[IRQ].InterruptCallback(Context, Mapping[IRQ].Data);
		}
		else
		{
			printf("IRQ %02X ", IRQ);
		}

		ClearIRQ(IRQ);
	}
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

		EnableIRQ(IRQ);
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
	if(Mode == ePIC)
		return;

	SetAPICRegister(ACPIOffsets::SpuriousInterruptVector, (GetAPICRegister(ACPIOffsets::SpuriousInterruptVector) & 0xFFFFFF00) | Vector);	
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
    
	if(Mode != ePIC)
	{
		//SetAPICRegister(ACPIOffsets::LVTInt0, 0x87FF);
		//SetAPICRegister(ACPIOffsets::LVTInt1, 0x0400);

		SetAPICRegister(ACPIOffsets::LVTInt0, 0xFF | LVTDeliveryModeExtINT	| LVTPinPolarityHigh | LVTTriggerModeLevel);
		SetAPICRegister(ACPIOffsets::LVTInt1, 0x00 | LVTDeliveryModeNMI		| LVTPinPolarityHigh | LVTTriggerModeEdge);

		// Officially turn the APIC on
		SetAPICRegister(ACPIOffsets::SpuriousInterruptVector, GetAPICRegister(ACPIOffsets::SpuriousInterruptVector) | 0x100);	

		for(int x = 0; x < 0x20; x++)
			Mapping[x].Vector = NewBase + x;
	}

	// Disable all IRQ (except for 2 which needs to be on to correctly route them)
	OutPortb(0x21, 0xFB);
	OutPortb(0xA1, 0xFF);

}

void InterruptControler::ClearIRQ(uint8_t IRQ)
{
	if(Mode == eAPIC)
	{
		SetAPICRegister(ACPIOffsets::EndOfInterrupt, 1);
	}
	//else
	{
		if (IRQ >= 8)
			OutPortb(0xA0, 0x20);

		OutPortb(0x20, 0x20);
	}
}

void InterruptControler::EnableIRQ(uint8_t IRQ)
{
	//if(UsingAPIC)
	//{
	//}
	//else
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
	//if(UsingAPIC)
	//{
	//}
	//else
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

void InterruptControler::SignalInterrupt(uint8_t Int)
{
	if(Mode != ePIC)
	{
		SetAPICRegister(ACPIOffsets::InterruptCommandHigh, 0x0);
		SetAPICRegister(ACPIOffsets::InterruptCommandLow, LVTDestinationShorthandSelf | LVTDeliveryModeFixed | Int);
	}
	else
	{
		Registers Res;
		FireInt(Int, &Res);
	}
}

void InterruptControler::PrintAPICIntStatus(ACPIOffsets StartIndex)
{
	uint32_t RealIndex = static_cast<uint32_t>(StartIndex);
	
	uint32_t Value = GetAPICRegister(static_cast<ACPIOffsets>(RealIndex));
	RealIndex++;
	
	uint32_t Mask = 0x010000;
	for(int x = 0x10; x < 0x100; x++)
	{
		if(Value & Mask)
			printf(" %02X", x);
		
		if(x % 32 == 0)
		{
			Mask = 0x01;
			Value = GetAPICRegister(static_cast<ACPIOffsets>(RealIndex));
			RealIndex++;
		}
		else
		{
			Mask <<= 1;
		}
	}
}

void PrintInt(uint32_t Value, bool Internal = false)
{
	printf("Vector: %02X, ", Value & 0xFF);
	printf("Mode: ");
	switch((Value & 0x700) >> 8)
	{
		case 0:
			printf("Fixed    ");
			break;
		case 1:
			printf("Lowest   ");
			break;
		case 2:
			printf("SMI      ");
			break;
		case 4:
			printf("NMI      ");
			break;
		case 5:
			printf("INIT     ");
			break;
		case 7:
			printf("ExtINT   ");
			break;
		default:
			printf("Invalid  ");
			break;			
	}
	
	if(!Internal)
	{
		if(Value & 0x2000)
			printf(" Low");
		else
			printf(" High");
						
		if(Value & 0x8000)
			printf(" Level");
		else
			printf(" Edge");

		if(Value & 0x4000)
			printf(" IRR");
	}

	if(Value & 0x1000)
		printf(" Pending");
							
	if(Value & 0x10000)
		printf(" Masked");
}

void InterruptControler::DumpPIC()
{
	uint16_t Port;
	printf("Mode: %0X\n", Mode);
	printf("IRQ        0 1 2 3 4 5 6 7 8 9 A B C D E F\n");
	Port = InPortb(0x21);
	Port += InPortb(0xA1) << 8;
						
	printf("Masked     ");
	for(int x = 0; x < 16; x++)
	{
		if(Port & 0x1 << x)
		{
			printf("X ");
		}
		else
		{
			printf(". ");
		}
	}

	printf("\n");

	OutPortb(0x20, 0x0a);
	OutPortb(0xA0, 0x0a);
	Port = InPortb(0x20);
	Port += InPortb(0xA0) << 8;
						
	printf("Requested  ");
	for(int x = 0; x < 16; x++)
	{
		if(Port & 0x1 << x)
		{
			printf("X ");
		}
		else
		{
			printf(". ");
		}
	}
	printf("\n");

	OutPortb(0x20, 0x0b);
	OutPortb(0xA0, 0x0b);
	Port = InPortb(0x20);
	Port += InPortb(0xA0) << 8;
						
	printf("In Service ");
	for(int x = 0; x < 16; x++)
	{
		if(Port & 0x1 << x)
		{
			printf("X ");
		}
		else
		{
			printf(". ");
		}
	}
	printf("\n");
	OutPortb(0x20, 0x0a);
	OutPortb(0xA0, 0x0a);
}

void InterruptControler::DumpAPIC()
{
	printf("Mode: %0X\n", Mode);
	printf("APIC (%08X) ID: %02X, Logical ID %02X, Version: %08X\n", APICRegisterBase, GetAPICRegister(ACPIOffsets::LocalACPIID) >> 24, (GetAPICRegister(ACPIOffsets::LocalACPIVersion) & 0xFF000000) >> 24, GetAPICRegister(ACPIOffsets::LocalACPIVersion));
	printf(" Spurious Interrupt Vector: %02X, Error Status %03X\n", GetAPICRegister(ACPIOffsets::SpuriousInterruptVector) & 0xFF, GetAPICRegister(ACPIOffsets::ErrorStatus) & 0x1FF);
	printf(" Task Priority: %02X, Arbitration Priority: %02X, Processor Priority: %02X\n", GetAPICRegister(ACPIOffsets::TaskPriority) & 0xFF, GetAPICRegister(ACPIOffsets::ArbitrationPriority) & 0xFF, GetAPICRegister(ACPIOffsets::ProcessorPriority) & 0xFF);
	printf(" Timer Count: %08X, Initial Count: %08X, Timer Divider: %X\n", GetAPICRegister(ACPIOffsets::TimerCurrentCount), GetAPICRegister(ACPIOffsets::TimerInitialCount), GetAPICRegister(ACPIOffsets::TimerDivideConfig) & 0x0F);

	//printf(" CMCI:     ");
	//PrintInt(Registers[0x2F * 4], true);
	//printf("\n");
						  
	printf(" Timer:    ");
	PrintInt(GetAPICRegister(ACPIOffsets::LVTTimer), true);
	if(GetAPICRegister(ACPIOffsets::LVTTimer) & 0x20000)
		printf(" Periodic");
	else
		printf(" One-Shot");

	printf("\n");

	printf(" Thermal:  ");
	PrintInt(GetAPICRegister(ACPIOffsets::LVTThermalSensor), true);
	printf("\n");

	printf(" Counters: ");
	PrintInt(GetAPICRegister(ACPIOffsets::LVTPerformanteCounter), true);
	printf("\n");

	printf(" LINT0:    ");
	PrintInt(GetAPICRegister(ACPIOffsets::LVTInt0));
	printf("\n");

	printf(" LINT1:    ");
	PrintInt(GetAPICRegister(ACPIOffsets::LVTInt1));
	printf("\n");

	printf(" Error:    ");
	PrintInt(GetAPICRegister(ACPIOffsets::LVTError), true);
	printf("\n");

	printf(" In Service: ");
	PrintAPICIntStatus(ACPIOffsets::InServiceBase);
	printf("\n");
	printf(" Request   : ");
	PrintAPICIntStatus(ACPIOffsets::InterruptRequestBase);
	printf("\n");
	printf(" Trigger   : ");
	PrintAPICIntStatus(ACPIOffsets::TriggerModeBase);
	printf("\n");
}

void InterruptControler::DumpIOAPIC()
{
	// Hack to dump I/O ACPI info						
	IOAPICRegisterBase = 0xFEC00000;

	uint32_t Temp, Temp2;

	Temp = GetIOAPICRegister(0);

	printf("Mode: %0X\n", Mode);
	printf("I/O APIC ID: %02X\n", Temp >> 24); 

	Temp = GetIOAPICRegister(1);

	uint32_t Count = ((Temp & 0xFF0000) >> 16) + 1;

	printf(" Version: %02X, Count: %02X\n", Temp & 0xFF, Count); 
					
	for(uint32_t x = 0; x < Count; x++)
	{
		uint32_t RegBase = 0x10 + (x * 2);

		Temp = GetIOAPICRegister(RegBase);
		Temp2 = GetIOAPICRegister(RegBase + 1);

		printf("INT %02X: ", x);

		if(Temp & 0x800)
		{
			printf("Logical Dest: %04X  ", (Temp2 & 0xFF000000) >> 24);
		}
		else
		{
			printf("Physical Dest: %02X ", (Temp2 & 0x0F000000) >> 24);
		}

		PrintInt(Temp);

		//printf("IRQ%02X: %08X:%08X\n", x, Temp2, Temp);

		printf("\n");
	}
}


