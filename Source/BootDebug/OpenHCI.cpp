#include "OpenHCI.h"
#include "InterruptControler.h"
#include "PCI.h"
#include "LowLevel.h"
#include <string.h>
#include <stdio.h>
#include <atomic>

#include "Utility.h"

#pragma pack(push, 1)

namespace OpenHCIData
{
	struct HCCA
	{
		uint32_t InterruptTable[32];
		uint16_t FrameNumber;
		uint16_t Pad1;
		uint32_t DoneHead;
		uint8_t  Reserved[116];
	};

	struct HCRegisters
	{
		uint32_t Revision;
		
		uint32_t ControlBulkServiceRatio : 2;
		uint32_t PeriodicListEnable : 1;
		uint32_t IsochronusEnable : 1;
		uint32_t ControlListEnable : 1;
		uint32_t BulkListEnable : 1;
		uint32_t HostControllerFunctionalState : 2;
		uint32_t InterruptRounting : 1;
		uint32_t RemoteWakeupConnected : 1;
		uint32_t RemoteWakeupEnabled : 1;

		uint32_t CommandStatus;

		uint32_t InterruptStatus;
		uint32_t InterruptEnable;
		uint32_t InterruptDisable;

		uint32_t HCCAPointer;
		uint32_t PeriodCurrentED;
		uint32_t ControlHeadED;
		uint32_t ControlCurrentED;
		uint32_t BulkHeadED;
		uint32_t BulkCurrentED;
		uint32_t DoneHead;

		uint16_t FrameLargestDataPacket;
		uint16_t FrameInterval;
		uint32_t FrameRemaining;
		uint32_t FrameNumber;
		uint32_t PeriodicStart;
		uint32_t LSThreshold;

		// Root Hub
		uint8_t  DownStreamPorts;

		uint16_t PowerSwitchingMode : 1;
		uint16_t NoPowerSwitching : 1;
		uint16_t DeviceType : 1;
		uint16_t OverCurrentProtectionMode : 1;
		uint16_t NoOverCurrentProtection : 1;

		uint8_t  PowerOnToGood;

		uint16_t DeviceRemovable;
		uint16_t PortPowerControlMask;

		uint32_t RootStatues;
		USBHub::PortStatusAndChange RootPortStatus[1];
	};

	enum ControlValues
	{
		ControlBulkServiceRatio11	= 0x0,
		ControlBulkServiceRatio21	= 0x1,
		ControlBulkServiceRatio31	= 0x2,
		ControlBulkServiceRatio41	= 0x3,

		USBReset					= 0x0,
		USBResume					= 0x1,
		USBOperational				= 0x2,
		USBSuspened					= 0x3,
	};

	enum StatusFlags
	{
		HostControllerReset =			0x0001,
		ControlListFilled =				0x0002,
		BulkListFilled =				0x0004,
		OwnershipChangeRequest =		0x0008,

		SchedulingOverrunCountMask =	0x30000,
		SchedulingOverrunCountShift =	0x10,
	};

	enum InterruptFlags
	{
		SchedulingOverrun		= 0x00000001,
		WritebackDoneHead		= 0x00000002,
		StartOfFrame			= 0x00000004,
		ResumeDetected			= 0x00000008,
		UnrecoverableError		= 0x00000010,
		FrameNumberOverflow		= 0x00000020,
		RootHubStatusChange		= 0x00000040,
		OwnershipChange			= 0x40000000,
		MasterInterruptEnable	= 0x80000000,

		// Well, almost all, doesn't have the SOF interrupt as we don't need/want it.
		AllInterupts			= 0x8000007B,
	};
	
	enum FrameIntervalFlags
	{
		FrameIntervalToggle		= 0x8000,
		FrameRemainingToggle	= 0x80000000,
	};


	enum RootStatusFlags
	{
		LocalPowerStatus				= 0x00000001,
		OverCurrentIndicator			= 0x00000002,
		DeviceRemoteWakeupEnable		= 0x00008000,
		LocalPowerStatusChange			= 0x00010000,
		OverCurrentIndicatorChange		= 0x00020000,

		ClearGlobalPower				= 0x00000001,
		SetGlobalPower					= 0x00010000,
		ClearRemoteWakeupEnable			= 0x80000000,
	};

	enum PortStatusFlags
	{
		PortCurrentConnectStatus		= 0x00000001,
		PortEnableStatus				= 0x00000002,
		PortSuspendStatus				= 0x00000004,
		PortOverCurrentIndicator		= 0x00000008,
		PortResetStatus					= 0x00000010,
		PortPowerStatus					= 0x00000100,
		LowSpeedDeviceAttached			= 0x00000200,

		PortConnectStatusChange			= 0x00010000,
		PortEnableStatusChange			= 0x00020000,
		PortSuspendStatusChange			= 0x00040000,
		PortOverCurrentIndicatorChange	= 0x00080000,
		PortResetStatusChange			= 0x00100000,

		ClearPortEnable					= 0x00000001,
		SetPortEnable					= 0x00000002,
		SetPortSuspend					= 0x00000004,
		ClearSuspendStatus				= 0x00000008,
		SetPortReset					= 0x00000010,
		SetPortPower					= 0x00000100,
		ClearPortPower					= 0x00000200,
	};


	struct EndpointDescriptor
	{
		uint32_t FunctionAddress : 7;
		uint32_t Endpoint : 4;
		uint32_t Direction : 2;
		uint32_t Speed : 1;
		uint32_t Skip : 1;
		uint32_t Format : 1;
		uint32_t MaxPacketSize : 11;
		
		uint32_t QueueTailPointer;
		uint32_t QueueHeadPointer;
		uint32_t NextEndpointDescriptior;
	};


	enum EndpointFlags
	{
		Halted =				0x0001,
		ToggleCarry =			0x0002,
	};

	struct GeneralTransferDescriptor
	{
		uint32_t Reserved : 18;
		uint32_t Rounding : 1;
		uint32_t Direction : 2;
		uint32_t DelayInterupt : 3;
		uint32_t DataToggle : 2;
		uint32_t ErrorCount : 2;
		uint32_t ConditionCode : 4;

		uint32_t CurrentBufferPointer;
		uint32_t NextTransferDescriptor;
		uint32_t BufferEnd;
	};

	struct IsochronusTransferDescriptor
	{
		uint16_t StartingFrame;
		uint16_t Reserved : 5;
		uint16_t DelayInterupt : 3;
		uint16_t FrameCount : 3;
		uint16_t Reserved2 : 1;
		uint16_t ConditionCode : 4;

		uint32_t BufferPage0;
		uint32_t NextTransferDescriptor;
		uint32_t BufferEnd;

		union
		{
			uint16_t Offset;
			struct
			{
				uint16_t Size : 11;
				uint16_t Reserved : 1;
				uint16_t ConditionCode : 4;
			} PSW;
		} OffsetPSW[8];
	};

	enum DirectionTypes
	{
		DirectionSetup =		0x00,
		DirectionOut =			0x01,
		DirectionIn =			0x02,

		DirectionTD =			0x00,
	};


	enum CompletionCodes
	{
		NoError	=				0x0,
		CRCError =				0x1,
		ButStuffingError =		0x2,
		DataToggleMismatch =	0x3,
		Stall =					0x4,
		DeviceNotResponding =	0x5,
		PIDCheckFailure =		0x6,
		UnexpectedPID =			0x7,
		DataOverrun =			0x8,
		DataUnderrun =			0x9,
		// Reserved =			0xA,
		// Reserved =			0xB,
		BufferOverrun =			0xC,
		BufferUnderrun =		0xD,
		InProgres =				0xE,
		NotAccessed =			0xF,
	};
}

#pragma pack(pop)


#pragma section(".HCA", read, write)
__declspec(allocate(".HCA"))  volatile OpenHCIData::HCCA HCCArea;


enum PipeState
{
	// Pipe object is free to be used
	FreeState,

	// Created, but not doing anything
	IdleState,
		
	// Sending/Receiving data
	TranferState,

	// Finished with no issues
	CompletedState,

	// Canceled, Shutdown everything!
	CancledState,

	// Something went wrong
	ErrorState
};

struct TransferData;

struct PipeData
{
	// Pointer to the end point of this pipe
	__declspec(align(32)) OpenHCIData::EndpointDescriptor EndPoint;

	OpenHCI::PipeType Type;
	PipeState State;

	TransferData * Data;

	PipeData *Prev;
	PipeData *Next;

	OpenHCIData::CompletionCodes CompletionCode;
	uint32_t Remainder;
	uint32_t RequestCount;

	std::atomic_flag InProgress;
	
	// Interrupt Callback
	OpenHCICallbackPtr Callback;
	uintptr_t * CallbackData;

	uint32_t TotalBandwidth;
};
	
struct TransferData
{
	__declspec(align(32)) union
	{
		OpenHCIData::IsochronusTransferDescriptor IsoTransferDescriptor;
		OpenHCIData::GeneralTransferDescriptor TransferDescriptor;
	};

	PipeData * Endpoint;

	TransferData *Prev;
	TransferData *Next;

	// A flag in case we get only half the TDs back as complete
	bool LastDataBlock;
};

enum InterruptIndexs
{
	Interrupt_32ms = 0,
	Interrupt_16ms = 1,
	Interrupt_8ms  = 2,
	Interrupt_4ms  = 3,
	Interrupt_2ms  = 4,
	Interrupt_1ms  = 5,
};

void OpenHCIInterupt(InterruptContext * Context, uintptr_t * Data)
{
	if(Data == nullptr)
		return;

	OpenHCI * Bus = reinterpret_cast<OpenHCI *>(Data);

	Bus->Interrupt(Context);
}

void OpenHCI::FillInterruptTable(PipeData *Table, int Length, PipeData *LinkTable, int LinkTableLength)
{
	for(int x = 0, link = 0; x < Length; x++, link++)
	{
		if(link == LinkTableLength)
			link = 0;

		Table[x].EndPoint.Skip = true;
		Table[x].Type = ReservedPipe;
		Table[x].State = IdleState;
		Table[x].EndPoint.NextEndpointDescriptior = reinterpret_cast<uint32_t>(&LinkTable[link]);
	}
}

// Start at the 512 meg mark and give it 16 megs of memory with 32 byte alignment
OpenHCI::OpenHCI(void)
{
	Registers = nullptr;
	m_DeviceID = 0;
	FreePipeHead = nullptr;
	FreeTransferDataHead = nullptr;
	DefaultHandle = nullptr;
	IsochronusPipeHead = nullptr;
	RootHubChangeCallback = nullptr;
	RootHubChangeData = nullptr;

	// Start off by creating a batch of Meta Data so we can have them if needed.
	PipeData * PipeDataBlock = new PipeData[32];
	TransferData * TransferDataBlock = new TransferData[32];

	for(int x = 0; x < 32; x++)
	{
		FreeEndpoint(&PipeDataBlock[x]);
		FreeTransferData(&TransferDataBlock[x]);
	}

	// Create the default Endpoint.
	DefaultHandle = GetEndpoint();
	DefaultHandle->State = IdleState;
	DefaultHandle->Type = ControlPipe;
	
	DefaultHandle->EndPoint.FunctionAddress = 0;
	DefaultHandle->EndPoint.Endpoint = 0;
	DefaultHandle->EndPoint.Direction = OpenHCIData::DirectionTD;
	DefaultHandle->EndPoint.Speed = 0;
	DefaultHandle->EndPoint.Skip = true;
	DefaultHandle->EndPoint.Format = false; // General TD;
	DefaultHandle->EndPoint.MaxPacketSize = 0x08;	// Give it the least amount just to be safe.
	DefaultHandle->EndPoint.NextEndpointDescriptior = 0;

	// Create the Isochronous head, this is just a static endpoint that we'll use to have a fix 'head' of the Iso list.
	IsochronusPipeHead = GetEndpoint();
	IsochronusPipeHead->State = IdleState;
	IsochronusPipeHead->Type = ReservedPipe;

	// Then build the Interrupt endpoints.
	InterruptEndpoints[Interrupt_32ms] = new PipeData[32];
	InterruptEndpoints[Interrupt_16ms] = new PipeData[16];
	InterruptEndpoints[ Interrupt_8ms] = new PipeData[ 8];
	InterruptEndpoints[ Interrupt_4ms] = new PipeData[ 4];
	InterruptEndpoints[ Interrupt_2ms] = new PipeData[ 2];
	InterruptEndpoints[ Interrupt_1ms] = new PipeData[ 1];

	// And then connect them in a descending tree
	FillInterruptTable(InterruptEndpoints[Interrupt_32ms], 32, InterruptEndpoints[Interrupt_16ms], 16);
	FillInterruptTable(InterruptEndpoints[Interrupt_16ms], 16, InterruptEndpoints[ Interrupt_8ms],  8);
	FillInterruptTable(InterruptEndpoints[ Interrupt_8ms],  8, InterruptEndpoints[ Interrupt_4ms],  4);
	FillInterruptTable(InterruptEndpoints[ Interrupt_4ms],  4, InterruptEndpoints[ Interrupt_2ms],  2);
	FillInterruptTable(InterruptEndpoints[ Interrupt_2ms],  2, InterruptEndpoints[ Interrupt_1ms],  1);
	FillInterruptTable(InterruptEndpoints[ Interrupt_1ms],  1, IsochronusPipeHead, 1);
}


OpenHCI::~OpenHCI(void)
{
}

void OpenHCI::Dump()
{
	if(Registers == nullptr)
		return;

	printf("Open HCI Rev %03X (Dev %08X, Reg %08X)\n", Registers->Revision, m_DeviceID, Registers);
	printf("  State: %02X, Command Status: %08X\n", Registers->HostControllerFunctionalState, Registers->CommandStatus);
	printf("  Interrupt Status: %08X, Enable: %08X\n", Registers->InterruptStatus, Registers->InterruptEnable);
	printf("  HCCA Pointer: %08X\n", Registers->HCCAPointer);
	printf("  Period Endpoint %08X\n", Registers->PeriodCurrentED);
	printf("  Control Head Endpoint %08X, Control Current Endpoint %08X\n", Registers->ControlHeadED, Registers->ControlCurrentED);
	printf("  Bulk Head Endpoint %08X, Bulk Current Endpoint %08X\n", Registers->BulkHeadED, Registers->BulkCurrentED);
	printf("  Frame Interval: %04X, Frame Largest Packet: %04X\n", Registers->FrameInterval, Registers->FrameLargestDataPacket);
	printf("  Frame Number: %08X, Frame Remaining: %08X\n", Registers->FrameNumber, Registers->FrameRemaining);
	printf("  Periodic Start: %08X, Threshold: %08X\n", Registers->PeriodicStart, Registers->LSThreshold);

	printf(" Root Hub\n");
	printf("  Ports %X", Registers->DownStreamPorts);
	
	if(Registers->NoPowerSwitching)
	{
		printf(", Always Powered");
	}
	else
	{
		if(Registers->PowerSwitchingMode)
			printf(", Individually Powered");
		else
			printf(", Gang Powered");

	}

	if(!Registers->NoOverCurrentProtection)
	{
		if(Registers->OverCurrentProtectionMode)
			printf(", Per-Port Over Current");
		else
			printf(", Collective Over Current");
	}

	printf("\n");

	//int Ports = Registers->DownStreamPorts;
	//printf("  Removable: %04X, Power Control Mask: %04X, Status: %08X, Devices: %02X\n", Registers->DeviceRemovable, Registers->PortPowerControlMask, Registers->RootStatues, Ports);
	printf("  Status: %08X\n", Registers->RootStatues);
	for(unsigned int x = 0; x < Registers->DownStreamPorts; x++)
	{
		printf("  Port %X: %08X", x + 1, Registers->RootPortStatus[x].StatusAndChanged);

		if(Registers->DeviceRemovable & ((1 << x) + 1))
			printf(", Removable");

		if(!Registers->NoPowerSwitching && Registers->PowerSwitchingMode)
		{
			if(Registers->PortPowerControlMask & ((1 << x) + 1))
				printf(", Power Switchable");
			else
				printf(", Gang Powered");
		}

		printf("\n");
	}

}

bool OpenHCI::IsPeriodicEndpoint(PipeData *Handle)
{
	if(Handle->Type == InterruptPipe)
		return true;

	if(Handle->Type == IsochronusPipe)
		return true;

	return false;
}

void OpenHCI::Interrupt(InterruptContext * Context)
{
	Registers->InterruptDisable = OpenHCIData::MasterInterruptEnable;
	printf("USB");
	if( (Registers->InterruptStatus & OpenHCIData::UnrecoverableError) || 
		(Registers->InterruptStatus & OpenHCIData::OwnershipChange) )

	{
		printf("USB System Error! Halting!\n");
		
		ASM_CLI;

		for(;;) ASM_HLT;
	}

	if(Registers->InterruptStatus & OpenHCIData::WritebackDoneHead)
	{
		
		if(HCCArea.DoneHead != 0)
		{					
			uint32_t Current = HCCArea.DoneHead & 0xFFFFFFFE;
			uint32_t Next = 0;
			TransferData * DoneHead; 
			// Inverse the order
			do
			{
				DoneHead = reinterpret_cast<TransferData *>(Current);
				
				Current = DoneHead->TransferDescriptor.NextTransferDescriptor;
				DoneHead->TransferDescriptor.NextTransferDescriptor = Next;
				Next = reinterpret_cast<uint32_t>(DoneHead);
			} while (Current != 0);

			// And process them
			do
			{							
				DoneHead->Endpoint->CompletionCode = (OpenHCIData::CompletionCodes)DoneHead->TransferDescriptor.ConditionCode;
				
				if(DoneHead->TransferDescriptor.CurrentBufferPointer != 0)
				{
					DoneHead->Endpoint->Remainder += DoneHead->TransferDescriptor.BufferEnd - DoneHead->TransferDescriptor.CurrentBufferPointer + 1;
				}
				
				TransferData *Old = DoneHead;
				DoneHead = reinterpret_cast<TransferData *>(Old->TransferDescriptor.NextTransferDescriptor);

				// Handle any errors that came up in this TD;
				if(Old->Endpoint->CompletionCode != OpenHCIData::NoError && Old->Endpoint->CompletionCode != OpenHCIData::InProgres)
					DisableEndpoint(Old->Endpoint, ErrorState);	

				else if(Old->LastDataBlock == true)
					DisableEndpoint(Old->Endpoint, CompletedState);	
				
				if(Old->Endpoint->State != TranferState)
				{
					if(IsPeriodicEndpoint(Old->Endpoint) && Old->Endpoint->Callback != nullptr)
					{
						Old->Endpoint->Callback(reinterpret_cast<uint32_t>(Old->Endpoint), Old->Endpoint->CallbackData);
					}
				}

				// If we successfully completed the transfer, clear out the end point data
				if(Old->Endpoint->State == CompletedState)
					ClearEndpointData(Old->Endpoint);

			} while (DoneHead != nullptr);
		}
	}

	if(Registers->InterruptStatus & OpenHCIData::ResumeDetected)
	{
	}

	if(Registers->InterruptStatus & OpenHCIData::FrameNumberOverflow)
	{
	}

	if(Registers->InterruptStatus & OpenHCIData::SchedulingOverrun)
	{
	}

	if(Registers->InterruptStatus & OpenHCIData::RootHubStatusChange)
	{
		if(RootHubChangeCallback != nullptr)
			RootHubChangeCallback(UINT32_MAX, RootHubChangeData);
	}

	Registers->InterruptStatus = OpenHCIData::AllInterupts;
	Registers->InterruptEnable = OpenHCIData::MasterInterruptEnable;
}

bool OpenHCI::FetchString(uint32_t Handle, uint8_t ID, uint16_t LanguageID)
{
	uint16_t Buffer[0x100];
	memset(Buffer, 0, 0x200);

	USBData::StringDescriptor *sd = reinterpret_cast<USBData::StringDescriptor *>(Buffer);

	USBData::USBDeviceRequest Request;

	Request.Recipient = USBData::RecipientDevice;
	Request.Type = USBData::TypeStandard;
	Request.Direction = true;
	Request.Request = USBData::GetDescriptor;
	Request.Value = (USBData::String << 8) + ID;
	Request.Index = LanguageID;
	Request.Length = 0x200;
	
	DeviceRequest(Handle, Request, &Buffer);

	if(ID == 0)
		return false;
	
	int count = (sd->Length - 2) / 2;

	for(int x = 0; x < count; x++)
	{
		printf("%c", sd->Value[x]);
	}
	//printf("\n");
	return true;
}

void OpenHCI::AddEndpoint(PipeData *Base, PipeData *EndPoint)
{
	do
	{
		if(Base->Next == nullptr)
		{
			// This looks weird, but is needed for Interrupts. Each level of the Interrupt table points to an entry in the next level
			// while Prev/Next point only to the entries in that level. So in this case Next is null, but NextEndpointDescriptior is valid, 
			// so make sure to save it.
			EndPoint->EndPoint.NextEndpointDescriptior = Base->EndPoint.NextEndpointDescriptior;
			Base->EndPoint.NextEndpointDescriptior = reinterpret_cast<uint32_t>(EndPoint);
			
			Base->Next = EndPoint;
			
			EndPoint->Prev = Base;
			break;
		}
		else
		{
			Base = Base->Next;
		}
	} while(Base != nullptr);
}

void OpenHCI::EnableEndpoint(PipeData *EndPoint)
{
	// Clear the error flag
	EndPoint->CompletionCode = OpenHCIData::InProgres;
	EndPoint->EndPoint.QueueHeadPointer = EndPoint->EndPoint.QueueHeadPointer & 0xFFFFFFFE;
	EndPoint->InProgress.test_and_set();
	EndPoint->EndPoint.Skip = false;

	switch (EndPoint->Type)
	{
		case ControlPipe:
			Registers->CommandStatus = OpenHCIData::ControlListFilled;
			break;

		case BulkPipe:
			Registers->CommandStatus = OpenHCIData::BulkListFilled;
			break;

		// These happen when they happen
		case InterruptPipe:
		case IsochronusPipe:
			break;
	}
}

void OpenHCI::SpinLockEndpoint(PipeData *EndPoint)
{
	// Spin until the InProgess flag is cleared, and reset it.
	while(EndPoint->InProgress.test_and_set())
		;

	// Clear it again
	EndPoint->InProgress.clear();
}

void OpenHCI::DisableEndpoint(PipeData *EndPoint, PipeState NewState)
{
	EndPoint->EndPoint.Skip = true;
	EndPoint->State = NewState;
	EndPoint->InProgress.clear();
}

void OpenHCI::FreeEndpoint(PipeData *EndPoint)
{
	if(EndPoint == nullptr)
		return;

	// Add it to the head of the free list
	if(FreePipeHead != nullptr)
		FreePipeHead->Prev = EndPoint;
	
	EndPoint->State = FreeState;
	EndPoint->Data = nullptr;
	EndPoint->Prev = nullptr;
	EndPoint->Next = FreePipeHead;	

	FreePipeHead = EndPoint;
}

void OpenHCI::ClearEndpointData(PipeData *EndPoint)
{
	TransferData *Current = EndPoint->Data;	

	while(Current != nullptr)
	{
		TransferData *Next = Current->Next;
		FreeTransferData(Current);
		Current = Next;
	};

	EndPoint->Data = nullptr;
	EndPoint->EndPoint.QueueHeadPointer = EndPoint->EndPoint.QueueTailPointer = 0;
}


PipeData *OpenHCI::GetEndpoint()
{
	PipeData * Ret = FreePipeHead;

	if(Ret == nullptr)
	{
		Ret = new PipeData();
	}
	else
	{
		FreePipeHead = Ret->Next;
		FreePipeHead->Prev = nullptr;
	}

	Ret->State = FreeState;
	Ret->EndPoint.NextEndpointDescriptior = 0;
	Ret->Next = nullptr;
	Ret->Prev = nullptr;
	Ret->Data = nullptr;
	Ret->CompletionCode = OpenHCIData::NoError;
	Ret->TotalBandwidth = 0;

	return Ret;
}

TransferData *OpenHCI::GetTransferData()
{
	TransferData * Ret = FreeTransferDataHead;

	if(Ret == nullptr)
	{
		Ret = new TransferData();
	}
	else
	{
		FreeTransferDataHead = Ret->Next;
		FreeTransferDataHead->Prev = nullptr;
	}

	FreeTransferDataHead->TransferDescriptor.BufferEnd = 0;
	FreeTransferDataHead->TransferDescriptor.CurrentBufferPointer = 0;
	FreeTransferDataHead->TransferDescriptor.NextTransferDescriptor = 0;
	Ret->Next = Ret->Prev = nullptr;
	Ret->Endpoint = nullptr;
	Ret->LastDataBlock = false;

	return Ret;
}

void OpenHCI::FreeTransferData(TransferData *Data)
{
	if(Data == nullptr)
		return;

	// Add it to the head of the free list
	if(FreeTransferDataHead != nullptr)
		FreeTransferDataHead->Prev = Data;
	
	Data->Prev = nullptr;
	Data->Next = FreeTransferDataHead;	

	FreeTransferDataHead = Data;
}


bool OpenHCI::StartUp(uint32_t DeviceID, InterruptControler *IntControler)
{
	m_DeviceID = DeviceID;
	//printf("Open HCI Startup %08X\n", DeviceID);
	
	Registers = reinterpret_cast<OpenHCIData::HCRegisters *>(PCI::ReadRegister(m_DeviceID, 0x10));
	m_IntControler = IntControler;

	if(Registers->InterruptRounting)
	{
		Registers->CommandStatus = OpenHCIData::OwnershipChangeRequest;

		while(Registers->InterruptRounting)
			;
	}

	m_IRQ = PCI::ReadRegister(m_DeviceID, 0x3C) & 0x000000FF;
	m_IntControler->SetIRQInterrupt(m_IRQ, OpenHCIInterupt, reinterpret_cast<uintptr_t *>(this));
	m_IntControler->EnableIRQ(m_IRQ);

	uint16_t temp = Registers->FrameInterval;
	Registers->CommandStatus = OpenHCIData::HostControllerReset;
	while(Registers->HostControllerFunctionalState != OpenHCIData::USBSuspened)
		;
	
	Registers->FrameInterval = temp;
	memset((void *)&HCCArea, 0, sizeof(OpenHCIData::HCCA));
	
	Registers->HCCAPointer = reinterpret_cast<uint32_t>(&HCCArea);

	// Set up the Interrupt table to point to our endpoints.
	for(int x = 0; x < 32; x++)
		HCCArea.InterruptTable[x] = reinterpret_cast<uint32_t>(&InterruptEndpoints[Interrupt_32ms][x]);

	Registers->InterruptEnable = OpenHCIData::MasterInterruptEnable | OpenHCIData::AllInterupts;
	Registers->InterruptStatus = OpenHCIData::AllInterupts;
	Registers->ControlBulkServiceRatio = OpenHCIData::ControlBulkServiceRatio11;
	Registers->PeriodicListEnable = true;
	Registers->IsochronusEnable = true;
	Registers->ControlListEnable = true;
	Registers->BulkListEnable = true;

	Registers->PeriodicStart = temp - (temp / 10);
	
	Registers->ControlHeadED = reinterpret_cast<uint32_t>(DefaultHandle);
	
	Registers->HostControllerFunctionalState = OpenHCIData::USBOperational;

	/*
	unsigned int Count = 0;
	while((Registers->InterruptStatus & OpenHCIData::RootHubStatusChange) == 0 && Count < 0x250000)
		Count++;

	for(unsigned int x = 0; x < Registers->DownStreamPorts; x++)
	{
		if(Registers->RootPortStatus[x].Status.Connection)
		{	
			Registers->RootPortStatus[x].Status.Reset = true;
			while(!Registers->RootPortStatus[x].Changed.ResetChange)
				;

			USBData::USBDeviceRequest Request;
			Request.Direction = false;
			Request.Type = USBData::TypeStandard;
			Request.Recipient = USBData::RecipientDevice;
			Request.Request = USBData::SetAddress;
			Request.Value = 0x42;
			Request.Index = 0;
			Request.Length = 0;
			
			DeviceRequest(0, Request, nullptr);

			PipeInformation Pipe;
			Pipe.Address = 0x42;
			Pipe.Endpoint = 0;
			Pipe.Type = ControlPipe;
			Pipe.MaxPacketSize = 0x08;
			uint32_t Handle = OpenPipe(Pipe);

			USBData::DeviceDescriptor DD;

			Request.Request = USBData::GetDescriptor;
			Request.Direction = true;
			Request.Value = 0x100;
			Request.Index = 0;
			Request.Length = sizeof(USBData::DeviceDescriptor);
			DeviceRequest(Handle, Request, &DD);

			AdjustMaxPacketSize(Handle, DD.MaxPacketSize0);
			
			printf("USB Device\n");
			printf(" USB Version %04X\n", DD.USBVersion);
			printf(" Class %02X, Subclass %02X, Protocol %02X\n", DD.DeviceClass, DD.DeviceSubClass, DD.DeviceProtocol);
			printf(" Manufacturer:  ");
			FetchString(Handle, DD.Manufacturer, 0x0409);
			printf("\n");
			printf(" Product:       ");
			FetchString(Handle, DD.Product, 0x0409);
			printf("\n");
			printf(" Serial Number: ");
			FetchString(Handle, DD.SerialNumber, 0x0409);
			printf("\n");
			printf(" Endpoint 0 Max Packet Size %02X\n", DD.MaxPacketSize0);
			printf(" Vender ID %04X, Product ID %04X, Device Version %04X\n", DD.VenderID, DD.ProductID, DD.DeviceVersion);
			printf(" Configuration Count %02X\n", DD.NumConfigurations);
				
			ALIGN_32BIT uint8_t Buffer2[0x200];
			Buffer2[0] = 0x42;
			
			Request.Recipient = USBData::RecipientDevice;
			Request.Type = USBData::TypeStandard;
			Request.Direction = true;
			Request.Request = USBData::GetDescriptor;
			Request.Value = 0x0200;
			Request.Index = 0;
			Request.Length = 0x100;
			DeviceRequest(Handle, Request, &Buffer2);

			printf("\n%08X: %08X %08X\n", Handle, Buffer2, GetByteCount(Handle));

			Request.Type = USBData::TypeStandard;
			Request.Recipient = USBData::RecipientDevice;
			Request.Request = USBData::SetConfiguration;
			Request.Direction = false;
			Request.Value = 1;
			Request.Index = 0;
			Request.Length = 0;
			DeviceRequest(Handle, Request, nullptr);

			Request.RequestType = 0x21;
			Request.Request = 0x0B;
			Request.Direction = false;
			Request.Value = 0;
			Request.Index = 0;
			Request.Length = 0;
			DeviceRequest(Handle, Request, nullptr);

			//Request.RequestType = 0x21;
			//Request.Request = 0x0A;
			//Request.Direction = false;
			//Request.Value = 0x0202;
			//Request.Index = 0;
			//Request.Length = 0;
			//DeviceRequest(Handle, Request, nullptr);

			//Pipe.Endpoint = 1;
			//Pipe.Frequencey = 0x0A;
			//Pipe.Type = InterruptPipe;
			//Pipe.LowSpeedDevice = true;
			//Pipe.ReadFromDevice = true;
			//uint32_t IntHandle = OpenPipe(Pipe);

			//
			ALIGN_32BIT uint8_t Buffer[0x200];
			//printf("Key...\n", Buffer);
			//size_t Read;
			//do
			//{
			//	ReadPipe(IntHandle, Buffer, 8, Read, false);
			//	WaitOnPipe(IntHandle);
			//	if(GetPipeStatus(IntHandle) != 0)
			//		printf("ERROR\n");
			//	else
			//		PrintMemoryBlock(Buffer, GetByteCount(IntHandle), 1);
			//}
			//while(true);
		}
	}
	*/
	return true;
}


uint32_t OpenHCI::OpenPipe(PipeInformation &Pipe)
{
	if(Pipe.Address == 0 )
		return 0;

	if(Pipe.Type == ReservedPipe || (Pipe.Type == ControlPipe && Pipe.Endpoint != 0))
		return UINT32_MAX;

	PipeData * DataHandle = GetEndpoint();

	DataHandle->State = IdleState;
	DataHandle->Type = Pipe.Type;
	
	DataHandle->EndPoint.FunctionAddress = Pipe.Address;
	DataHandle->EndPoint.Endpoint = Pipe.Endpoint;
	DataHandle->EndPoint.Speed = Pipe.LowSpeedDevice;
	DataHandle->EndPoint.Skip = true;
	DataHandle->EndPoint.Format = false; // General TD;
	DataHandle->EndPoint.MaxPacketSize = Pipe.MaxPacketSize;	
	DataHandle->EndPoint.NextEndpointDescriptior = 0;

	if(Pipe.ReadFromDevice)
		DataHandle->EndPoint.Direction = OpenHCIData::DirectionIn;
	else
		DataHandle->EndPoint.Direction = OpenHCIData::DirectionOut;

	switch(Pipe.Type)
	{
		case ControlPipe:
			// Control Pipes are bi-directional, so the TD will provide the direction information. 
			DataHandle->EndPoint.Direction = OpenHCIData::DirectionTD;

			if(Registers->ControlHeadED == 0)
				 Registers->ControlHeadED = reinterpret_cast<uint32_t>(DataHandle);
			else
				AddEndpoint(reinterpret_cast<PipeData *>(Registers->ControlHeadED), DataHandle);
			break;

		case BulkPipe:
			if(Registers->BulkCurrentED == 0)
				 Registers->BulkCurrentED = reinterpret_cast<uint32_t>(DataHandle);
			else
				AddEndpoint(reinterpret_cast<PipeData *>(Registers->BulkCurrentED), DataHandle);
			break;

		case InterruptPipe:
			// Work out which layer the interrupt pipe should go
			{
				int Index = 0;
				int Time = 32;
				do
				{
					if(Time <= Pipe.Frequencey)
						break;
					Time = Time / 2;
					Index++;
				}
				while(Time != 0);

				if(Index > Interrupt_1ms)
					Index = Interrupt_1ms;

				// Not going to even try to load balance at this time
				AddEndpoint(InterruptEndpoints[Index], DataHandle);
			}
			
			DataHandle->Callback = Pipe.CompletionCallback;
			DataHandle->CallbackData = Pipe.CallbackData;
			break;

		case IsochronusPipe:
			DataHandle->EndPoint.Format = true;
			DataHandle->Callback = Pipe.CompletionCallback;
			DataHandle->CallbackData = Pipe.CallbackData;

			AddEndpoint(IsochronusPipeHead, DataHandle);
			break;
	}

	return reinterpret_cast<uint32_t>(DataHandle);
}

bool OpenHCI::AdjustMaxPacketSize(uint32_t Handle, uint16_t MaxPacketSize)
{
	if(Handle == DefaultDeviceHandle)
		return false;

	PipeData * DataHandle = reinterpret_cast<PipeData *>(Handle);

	DataHandle->EndPoint.MaxPacketSize = MaxPacketSize;

	return true;
}

void OpenHCI::ClosePipe(uint32_t Handle)
{
	if(Handle == DefaultDeviceHandle)
		return;

	PipeData * DataHandle = reinterpret_cast<PipeData *>(Handle);
	
	DataHandle->EndPoint.Skip = true;

	// Remove the endpoint from it's ED List..	
	if(DataHandle->Prev == nullptr)
	{
		uint32_t EDLink = 0;

		// We are at the start of our list, so cut out the back pointer on the next block in the chain
		if(DataHandle->Next != nullptr)
		{
			DataHandle->Next->Prev = nullptr;
			EDLink = reinterpret_cast<uint32_t>(DataHandle->Next);
		}

		switch(DataHandle->Type)
		{
			case ControlPipe:
				Registers->ControlHeadED = EDLink;
				break;

			case BulkPipe:
				Registers->BulkCurrentED = EDLink;
				break;

			case InterruptPipe:
			case IsochronusPipe:
				// This should never happen as the head of their respective lists are not public
				break;
		}
	}
	else if(DataHandle->Next == nullptr)
	{
		// We're at the end of the chain, so just drop ourselves out.
		// This looks weird, but is needed for Interrupts. Each level of the Interrupt table points to an entry in the next level
		// while Prev/Next point only to the entries in that level. So in this case Next is null, but NextEndpointDescriptior is valid, 
		// so make sure to save it.
		DataHandle->Prev->EndPoint.NextEndpointDescriptior = DataHandle->EndPoint.NextEndpointDescriptior;
		DataHandle->Prev->Next = nullptr;
	}
	else
	{
		// Stuck in the middle, but easy enough to deal with.
		DataHandle->Prev->Next = DataHandle->Next;
		DataHandle->Prev->EndPoint.NextEndpointDescriptior = reinterpret_cast<uint32_t>(DataHandle->Next);
		DataHandle->Next->Prev = DataHandle->Prev;
	}

	DataHandle->Prev = nullptr;
	DataHandle->Next = nullptr;
	DataHandle->Data = nullptr;
	DataHandle->State = FreeState;

	FreeEndpoint(DataHandle);
}

void OpenHCI::BuildTransferData(PipeData * DataHandle, void *Data, size_t Length, int Direction, int DataToggle, bool MarkLastBlock)
{
	// Create the TD we're going to be using and make sure to save it.
	TransferData *DataRoot = nullptr;
	TransferData *DataCurrent = nullptr;

	uint32_t BlockLength = Length;
	uint8_t * BufferPtr = reinterpret_cast<uint8_t *>(Data);

	do
	{
		if(DataRoot == nullptr)
		{
			// Set it at the root
			DataRoot = GetTransferData();
			DataCurrent = DataRoot;
		}
		else
		{
			// Create the new TD we'll be working with.
			TransferData *Current = GetTransferData();

			// Link it onto the chain
			DataCurrent->Next = Current;
			DataCurrent->TransferDescriptor.NextTransferDescriptor = reinterpret_cast<uint32_t>(Current);
			Current->Prev = DataCurrent;

			DataCurrent = Current;
		}
		
		DataCurrent->Endpoint = DataHandle;
		
		// A TD can't cross more then two physical pages in memory, so we'll limit the size to 4k to make sure it only ever is in two
		uint32_t TDLength = 0x1000;
		if(TDLength > BlockLength)
			TDLength = BlockLength;

		DataCurrent->TransferDescriptor.Direction = Direction;
		DataCurrent->TransferDescriptor.Rounding = true;
		DataCurrent->TransferDescriptor.DelayInterupt = 7;
		DataCurrent->TransferDescriptor.DataToggle = DataToggle;
		DataCurrent->TransferDescriptor.ErrorCount = 0;
		DataCurrent->TransferDescriptor.ConditionCode = OpenHCIData::NotAccessed;
		if(Length == 0)
		{
			DataCurrent->TransferDescriptor.CurrentBufferPointer = 0;
			DataCurrent->TransferDescriptor.BufferEnd = 0;
		}
		else
		{
			DataCurrent->TransferDescriptor.CurrentBufferPointer = reinterpret_cast<uint32_t>(BufferPtr);
			DataCurrent->TransferDescriptor.BufferEnd = DataCurrent->TransferDescriptor.CurrentBufferPointer + TDLength - 1;
		}

		BlockLength -=TDLength;
		BufferPtr += TDLength;
	} while(BlockLength != 0);

	if(MarkLastBlock)
	{
		DataCurrent->LastDataBlock = true;
		DataCurrent->TransferDescriptor.DelayInterupt = 0;
	}

	// Now patch the new list into the existing list.
	if(DataHandle->Data == nullptr)
	{
		DataHandle->Data = DataRoot;
		DataHandle->EndPoint.QueueHeadPointer = reinterpret_cast<uint32_t>(DataRoot);
		DataHandle->EndPoint.QueueTailPointer = reinterpret_cast<uint32_t>(DataCurrent);
	}
	else
	{
		TransferData *DataChain = DataHandle->Data;		

		while(DataChain->Next != nullptr)
			DataChain = DataChain->Next;

		DataChain->Next = DataRoot;
		DataChain->TransferDescriptor.NextTransferDescriptor = reinterpret_cast<uint32_t>(DataRoot);
		DataHandle->EndPoint.QueueTailPointer = reinterpret_cast<uint32_t>(DataCurrent);
	}
}

void OpenHCI::AddTerminalTransferData(PipeData * DataHandle)
{
	TransferData *DataChain = DataHandle->Data;		

	while(DataChain->Next != nullptr)
		DataChain = DataChain->Next;

	DataChain->Next = GetTransferData();
	DataChain->Next->Prev = DataChain;
	DataChain->Next->Endpoint = DataHandle;

	if(DataHandle->Type == IsochronusPipe)
	{
		DataChain->IsoTransferDescriptor.NextTransferDescriptor = reinterpret_cast<uint32_t>(DataChain->Next);
		DataChain->Next->IsoTransferDescriptor.ConditionCode = OpenHCIData::NotAccessed;
		DataChain->Next->IsoTransferDescriptor.NextTransferDescriptor = 0;
		DataChain->Next->IsoTransferDescriptor.BufferPage0 = 0;
		DataChain->Next->IsoTransferDescriptor.BufferEnd = 0;
		DataChain->Next->IsoTransferDescriptor.StartingFrame = 0;
		DataChain->Next->IsoTransferDescriptor.FrameCount = 0;
	}
	else
	{
		DataChain->TransferDescriptor.NextTransferDescriptor = reinterpret_cast<uint32_t>(DataChain->Next);
		DataChain->Next->TransferDescriptor.NextTransferDescriptor = 0;
		DataChain->Next->TransferDescriptor.CurrentBufferPointer = 0;
		DataChain->Next->TransferDescriptor.BufferEnd = 0;
		DataChain->Next->TransferDescriptor.ConditionCode = OpenHCIData::NotAccessed;
	}

	DataHandle->EndPoint.QueueTailPointer = reinterpret_cast<uint32_t>(DataChain->Next);
}


bool OpenHCI::WritePipe(uint32_t Handle, void *Data, size_t Length, bool Block)
{
	if(Handle == DefaultDeviceHandle)
		return false;

	PipeData * DataHandle = reinterpret_cast<PipeData *>(Handle);
	if(!DataHandle->EndPoint.Skip && DataHandle->Data != nullptr)
		return false;

	if(DataHandle->Type == IsochronusPipe)
	{
		return false;
	}

	BuildTransferData(DataHandle, Data, Length, OpenHCIData::DirectionOut);
	AddTerminalTransferData(DataHandle);
	
	DataHandle->Remainder = 0;
	DataHandle->RequestCount = Length;
	DataHandle->State = TranferState;

	Registers->InterruptStatus = OpenHCIData::WritebackDoneHead;

	EnableEndpoint(DataHandle);

	// Never block on Interrupt Pipes
	if(DataHandle->Type == InterruptPipe)
		Block = false;

	// Spin
	if(Block)
	{
		SpinLockEndpoint(DataHandle);
	
		if(DataHandle->State != CompletedState)
			return false;
	}

	return true;
}

bool OpenHCI::ReadPipe(uint32_t Handle, void *Data, size_t Length, size_t &Read, bool Block)
{
	if(Handle == DefaultDeviceHandle)
		return false;

	PipeData * DataHandle = reinterpret_cast<PipeData *>(Handle);

	if(!DataHandle->EndPoint.Skip && DataHandle->Data != nullptr)
		return false;

	if(DataHandle->Type == IsochronusPipe)
	{
		return false;
	}

	DisableEndpoint(DataHandle, IdleState);
	ClearEndpointData(DataHandle);

	BuildTransferData(DataHandle, Data, Length, OpenHCIData::DirectionIn);
	AddTerminalTransferData(DataHandle);
	
	DataHandle->Remainder = 0;
	DataHandle->RequestCount = Length;
	DataHandle->State = TranferState;

	Registers->InterruptStatus = OpenHCIData::WritebackDoneHead;
	
	EnableEndpoint(DataHandle);
	
	Read = 0;

	// Never block on Interrupt Pipes
	if(DataHandle->Type == InterruptPipe)
		Block = false;

	if(Block)
	{
		// Spin
		SpinLockEndpoint(DataHandle);
	
		Read = DataHandle->RequestCount - DataHandle->Remainder;

		if(DataHandle->State != CompletedState)
			return false;
	}

	return true;
}

bool OpenHCI::DeviceRequest(uint32_t Handle, USBData::USBDeviceRequest &Request, void *Buffer)
{
	PipeData * DataHandle;
	
	if(Handle == DefaultDeviceHandle)
		DataHandle = DefaultHandle;
	else
		DataHandle = reinterpret_cast<PipeData *>(Handle);

	if(DataHandle->Type != ControlPipe)
		return false;

	DataHandle->Remainder = 0;
	DataHandle->RequestCount = Request.Length;
	
	// Build the request block
	BuildTransferData(DataHandle, &Request, sizeof(USBData::USBDeviceRequest), OpenHCIData::DirectionSetup, 2, false);
	//DataHandle->RequestCount += sizeof(USBData::USBDeviceRequest);

	// Add the data
	if(Request.Length != 0)
	{
		BuildTransferData(DataHandle, Buffer, Request.Length,  Request.Direction ? OpenHCIData::DirectionIn : OpenHCIData::DirectionOut, 3, false);
		//DataHandle->RequestCount += Request.Length;
	}

	// Add the status block
	BuildTransferData(DataHandle, nullptr, 0, Request.Direction ? OpenHCIData::DirectionOut : OpenHCIData::DirectionIn, 3, true);

	// And cap it off
	AddTerminalTransferData(DataHandle);

	//printf("Sending\n");

	EnableEndpoint(DataHandle);
		
	//printf("Spinning\n");
	
	// Spin
	SpinLockEndpoint(DataHandle);
	
	//printf("%08X: %02X %08X\n", DataHandle, DataHandle->CompletionCode, DataHandle->ByteCount);

	if(DataHandle->State != CompletedState)
		return false;

	return true;
}

uint32_t OpenHCI::GetPipeStatus(uint32_t Handle)
{
	PipeData * DataHandle;
	
	if(Handle == DefaultDeviceHandle)
		DataHandle = DefaultHandle;
	else
		DataHandle = reinterpret_cast<PipeData *>(Handle);

	return DataHandle->CompletionCode;
}

void OpenHCI::ClearPipeError(uint32_t Handle)
{
	PipeData * DataHandle;
	
	if(Handle == DefaultDeviceHandle)
		DataHandle = DefaultHandle;
	else
		DataHandle = reinterpret_cast<PipeData *>(Handle);

	if(!DataHandle->EndPoint.Skip && (DataHandle->EndPoint.QueueHeadPointer & 0x00000001) == 0)
		return;

	// The endpoint shouldn't be in use, but lets just mark it skipped to be sure
	DataHandle->EndPoint.Skip = true;
	
	// Remove any TDs attached to it the end point
	ClearEndpointData(DataHandle);
	
	// And disable it with the idle state
	DisableEndpoint(DataHandle, IdleState);
}

void OpenHCI::WaitOnPipe(uint32_t Handle)
{
	PipeData * DataHandle;
	
	if(Handle == DefaultDeviceHandle)
		DataHandle = DefaultHandle;
	else
		DataHandle = reinterpret_cast<PipeData *>(Handle);

	SpinLockEndpoint(DataHandle);
}

bool OpenHCI::PipeCompleted(uint32_t Handle)
{
	PipeData * DataHandle;
	
	if(Handle == DefaultDeviceHandle)
		DataHandle = DefaultHandle;
	else
		DataHandle = reinterpret_cast<PipeData *>(Handle);

	bool Ret = DataHandle->InProgress.test_and_set();

	if(!Ret)
	{
		DataHandle->InProgress.clear();
		return true;
	}

	return false;
}

uint32_t OpenHCI::GetByteCount(uint32_t Handle)
{
	PipeData * DataHandle;
	
	if(Handle == DefaultDeviceHandle)
		DataHandle = DefaultHandle;
	else
		DataHandle = reinterpret_cast<PipeData *>(Handle);

	return DataHandle->RequestCount - DataHandle->Remainder;
}

// Root hub info
uint8_t OpenHCI::NumberOfPorts()
{
	if(Registers == nullptr)
		return 0;

	return Registers->DownStreamPorts;
}

uint8_t OpenHCI::PowerOnToGood()
{
	if(Registers == nullptr)
		return 0;

	return Registers->PowerOnToGood;
}

uint8_t OpenHCI::MaxCurrent()
{
	return 0;
}

bool OpenHCI::IsDeviceRemovable(uint8_t PortNumber)
{
	if(Registers == nullptr)
		return false;

	if(PortNumber == 0 || PortNumber > Registers->DownStreamPorts)
		return false;

	uint32_t Mask = (0x01 << PortNumber);

	return (Registers->DeviceRemovable & Mask) != 0;
}

// Returns a bitmap of what had change. Bit 0 is the hub, and the rest of the bits are the ports.
uint32_t OpenHCI::StatusChangeBitmap()
{
	uint32_t Ret = 0;

	if(Registers == nullptr)
		return Ret;
	
	uint32_t Mask = 0x01;

	if(Registers->RootStatues & OpenHCIData::LocalPowerStatusChange)
		Ret |= Mask;
	
	if(Registers->RootStatues & OpenHCIData::OverCurrentIndicatorChange)
		Ret |= Mask;

	Mask = Mask << 1;

	for(int x = 0; x < Registers->DownStreamPorts; x++)
	{
		if((Registers->RootPortStatus[x].StatusAndChanged & 0xFFFF0000) != 0)
			Ret |= Mask;

		Mask = Mask << 1;
	}

	return Ret;
}

USBHub::HubStatusAndChange OpenHCI::GetHubStatusAndChanged()
{
	USBHub::HubStatusAndChange Ret;
	
	if(Registers == nullptr)
	{
		Ret.StatusAndChanged = 0;
		return Ret;
	}

	Ret.StatusAndChanged = Registers->RootStatues;

	return Ret;
}

void OpenHCI::ClearHubChanged(USBHub::HubStatusAndChange &Flags)
{
	uint32_t Set = 0;

	// This will always be zero on the root hub, so no need to set it.
	//if(Flags.Changed.LocalPowerStatusChange)
	//	Set |= OpenHCIData::LocalPowerStatusChange;

	if(Flags.Changed.OverCurrentIndicatorChange)
		Set |= OpenHCIData::OverCurrentIndicatorChange;

	Registers->RootStatues = Set;
}

USBHub::PortStatusAndChange OpenHCI::GetPortStatusAndChanged(uint8_t PortNumber)
{
	USBHub::PortStatusAndChange Ret;
	Ret.StatusAndChanged = 0;
	
	if(Registers == nullptr)
		return Ret;

	if(PortNumber == 0 || PortNumber > Registers->DownStreamPorts)
		return Ret;

	Ret.StatusAndChanged = Registers->RootPortStatus[PortNumber - 1].StatusAndChanged;

	return Ret;
}

void OpenHCI::ClearPortChanged(uint8_t PortNumber, USBHub::PortStatusAndChange &Flags)
{
	if(Registers == nullptr)
		return;

	if(PortNumber == 0 || PortNumber > Registers->DownStreamPorts)
		return;

	Registers->RootPortStatus[PortNumber - 1].StatusAndChanged = Flags.StatusAndChanged;

	return;
}

void OpenHCI::SetRootHubChangeInterrupt(OpenHCICallbackPtr CallBack, uintptr_t *CallbackData)
{
	if(Registers == nullptr)
		return;

	Registers->InterruptDisable = OpenHCIData::RootHubStatusChange;
	
	RootHubChangeCallback = CallBack;
	RootHubChangeData = CallbackData;

	if(RootHubChangeCallback != nullptr)
		Registers->InterruptEnable = OpenHCIData::RootHubStatusChange;
}


