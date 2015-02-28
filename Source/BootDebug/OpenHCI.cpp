#include "OpenHCI.h"
#include "InterruptControler.h"
#include "PCI.h"
#include "LowLevel.h"
#include <string.h>
#include <stdio.h>
#include <atomic>

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

	// Waiting for an Interrupt or Iso connection
	PendingState,

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
	uint32_t ByteCount;
	uint32_t RequestCount;

	std::atomic_flag InProgress;
	
	// Interrupt Callback
	OpenHCICallbackPtr Callback;
	uintptr_t * CallbackData;
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

	
// These are all our Interrupt tables. They could easily be one array, but it's cleaner to number them.
//ALIGN_32BIT volatile OpenHCIData::EndpointDescriptor InterruptEndpoint1_32[32];
//ALIGN_32BIT volatile OpenHCIData::EndpointDescriptor InterruptEndpoint1_16[16];
//ALIGN_32BIT volatile OpenHCIData::EndpointDescriptor InterruptEndpoint1_8[8];
//ALIGN_32BIT volatile OpenHCIData::EndpointDescriptor InterruptEndpoint1_4[4];
//ALIGN_32BIT volatile OpenHCIData::EndpointDescriptor InterruptEndpoint1_2[2];
//ALIGN_32BIT volatile OpenHCIData::EndpointDescriptor InterruptEndpoint1_1[1];


void OpenHCIInterupt(InterruptContext * Context, uintptr_t * Data)
{
	if(Data == nullptr)
		return;

	OpenHCI * Bus = reinterpret_cast<OpenHCI *>(Data);

	Bus->Interrupt(Context);
}

// Start at the 512 meg mark and give it 16 megs of memory with 32 byte alignment
OpenHCI::OpenHCI(void) : Allocator(0x2000000, 0x1000000, 32)
{
	Registers = nullptr;
	m_DeviceID = 0;
	FreePipeHead = nullptr;
	DefaultHandle = nullptr;
	IsochronusPipeHead = nullptr;

	// Start off by creating a batch of Meta Data so we can have them if needed.
	FreePipeHead = new PipeData[32];
	FreeTransferDataHead = new TransferData[32];

	for(int x = 0; x < 32; x++)
	{
		if(x == 0)
		{
			FreePipeHead[x].Prev = nullptr;
			FreeTransferDataHead[x].Prev = nullptr;
		}
		else
		{
			FreePipeHead[x].Prev = &FreePipeHead[x - 1];
			FreeTransferDataHead[x].Prev = &FreeTransferDataHead[x - 1];
		}

		if(x == 32)
		{
			FreePipeHead[x].Next = nullptr;
			FreeTransferDataHead[x].Next = nullptr;
		}
		else
		{
			FreePipeHead[x].Next = &FreePipeHead[x + 1];
			FreeTransferDataHead[x].Next = &FreeTransferDataHead[x + 1];
		}
	
		FreePipeHead[x].State = FreeState;
		FreePipeHead[x].Data = nullptr;
		FreeTransferDataHead[x].Endpoint = nullptr;
	}





	/*
	memset((void *)&InterruptEndpoint1_32, 0, sizeof(OpenHCIData::EndpointDescriptor) * 32);
	memset((void *)&InterruptEndpoint1_16, 0, sizeof(OpenHCIData::EndpointDescriptor) * 16);
	memset((void *)&InterruptEndpoint1_8, 0, sizeof(OpenHCIData::EndpointDescriptor) * 8);
	memset((void *)&InterruptEndpoint1_4, 0, sizeof(OpenHCIData::EndpointDescriptor) * 4);
	memset((void *)&InterruptEndpoint1_2, 0, sizeof(OpenHCIData::EndpointDescriptor) * 2);
	memset((void *)&InterruptEndpoint1_1, 0, sizeof(OpenHCIData::EndpointDescriptor) * 1);

	InterruptEndpoint1_1[0].Skip = true;

	for(int x = 0; x < 2; x++)
	{
		InterruptEndpoint1_2[x].Skip = true;
		InterruptEndpoint1_2[x].NextEndpointDescriptior = reinterpret_cast<uint32_t>(&InterruptEndpoint1_1[x/2]);
	}

	for(int x = 0; x < 4; x++)
	{
		InterruptEndpoint1_4[x].Skip = true;
		InterruptEndpoint1_4[x].NextEndpointDescriptior = reinterpret_cast<uint32_t>(&InterruptEndpoint1_2[x/2]);
	}

	for(int x = 0; x < 8; x++)
	{
		InterruptEndpoint1_8[x].Skip = true;
		InterruptEndpoint1_8[x].NextEndpointDescriptior = reinterpret_cast<uint32_t>(&InterruptEndpoint1_4[x/2]);
	}

	for(int x = 0; x < 16; x++)
	{
		InterruptEndpoint1_16[x].Skip = true;
		InterruptEndpoint1_16[x].NextEndpointDescriptior = reinterpret_cast<uint32_t>(&InterruptEndpoint1_8[x/2]);
	}

	for(int x = 0; x < 32; x++)
	{
		InterruptEndpoint1_32[x].Skip = true;
		InterruptEndpoint1_32[x].NextEndpointDescriptior = reinterpret_cast<uint32_t>(&InterruptEndpoint1_16[x/2]);
	}
	*/
}


OpenHCI::~OpenHCI(void)
{
}

void OpenHCI::Dump()
{
	std::atomic_flag Lock;
	Lock.test_and_set(std::memory_order_acquire);
	
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
		printf("  Port %X: %08X", x + 1, Registers->RootPortStatus[x]);

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
				if(DoneHead->Endpoint->State == CancledState)
				{
					if(DoneHead->LastDataBlock == true)
						DisableEndpoint(DoneHead->Endpoint, CancledState);

					continue;
				}

				DoneHead->Endpoint->CompletionCode = (OpenHCIData::CompletionCodes)DoneHead->TransferDescriptor.ConditionCode;
				
				if(DoneHead->TransferDescriptor.CurrentBufferPointer != 0)
					DoneHead->Endpoint->ByteCount -= DoneHead->TransferDescriptor.BufferEnd - DoneHead->TransferDescriptor.CurrentBufferPointer + 1;
				
				if(DoneHead->LastDataBlock == true)
				{
					DisableEndpoint(DoneHead->Endpoint, DoneHead->TransferDescriptor.ConditionCode == 0 ? CompletedState : ErrorState);

					if(IsPeriodicEndpoint(DoneHead->Endpoint) && DoneHead->Endpoint->Callback != nullptr)
						DoneHead->Endpoint->Callback(reinterpret_cast<uint32_t>(DoneHead->Endpoint), DoneHead->Endpoint->CallbackData);
				}

				TransferData *Old = DoneHead;
				DoneHead = reinterpret_cast<TransferData *>(Old->TransferDescriptor.NextTransferDescriptor);

				// Free the TDs from the end points				
				//if(!IsPeriodicEndpoint(Old->Endpoint))
				{
					Old->Endpoint->Data = DoneHead;
					if(DoneHead != nullptr)
						DoneHead->Prev = nullptr;

					// Free the last block in the old chain as well
					if(Old->LastDataBlock == true && Old->Endpoint->EndPoint.QueueHeadPointer != 0 && (Old->Endpoint->EndPoint.QueueHeadPointer == Old->Endpoint->EndPoint.QueueTailPointer))
					{
						Old->Endpoint->EndPoint.QueueHeadPointer = 0;
						Old->Endpoint->EndPoint.QueueTailPointer = 0;
						FreeTransferData(Old->Endpoint->Data);
						Old->Endpoint->Data = nullptr;
					}
					
					FreeTransferData(Old);
				}

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
	EndPoint->State = NewState;
	EndPoint->EndPoint.Skip = true;
	EndPoint->InProgress.clear();
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

	Registers->InterruptEnable = OpenHCIData::MasterInterruptEnable | OpenHCIData::AllInterupts;
	Registers->InterruptStatus = OpenHCIData::AllInterupts;
	Registers->ControlBulkServiceRatio = OpenHCIData::ControlBulkServiceRatio11;
	Registers->PeriodicListEnable = true;
	Registers->IsochronusEnable = true;
	Registers->ControlListEnable = true;
	Registers->BulkListEnable = true;

	Registers->PeriodicStart = temp - (temp / 10);
	
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

	Registers->ControlHeadED = reinterpret_cast<uint32_t>(DefaultHandle);

	// Create the Isochronus head, this is just a static endpoint that we'll use to have a fix 'head' of the Iso list.
	IsochronusPipeHead = GetEndpoint();
	IsochronusPipeHead->State = IdleState;
	IsochronusPipeHead->Type = IsochronusPipe;
	

	Registers->HostControllerFunctionalState = OpenHCIData::USBOperational;

	unsigned int Count = 0;
	while((Registers->InterruptStatus & OpenHCIData::RootHubStatusChange) == 0 && Count < 0x100000)
		Count++;

	for(unsigned int x = 0; x < Registers->DownStreamPorts; x++)
	{
		if(Registers->RootPortStatus[x] & OpenHCIData::PortCurrentConnectStatus)
		{	
			Registers->RootPortStatus[x] = Registers->RootPortStatus[x] | OpenHCIData::SetPortReset;
			while((Registers->RootPortStatus[x] & OpenHCIData::PortResetStatusChange) == 0)
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

			uint32_t Handle = OpenPipe(0x42, 0, ControlPipe, 0x08);

			USBData::DeviceDescriptor DD;

			Request.Request = USBData::GetDescriptor;
			Request.Direction = true;
			Request.Value = 0x100;
			Request.Index = 0;
			Request.Length = sizeof(USBData::DeviceDescriptor);
			DeviceRequest(Handle, Request, &DD);

			printf("USB Device\n");
			printf(" USB Version %04X\n", DD.USBVersion);
			printf(" Class %02X, Subclass %02X, Protocol %02X\n", DD.DeviceClass, DD.DeviceSubClass, DD.DeviceProtocol);
			printf(" Manufacturer: ");
			FetchString(Handle, DD.Manufacturer, 0x0409);
			printf("\n");
			printf(" Product: ");
			FetchString(Handle, DD.Product, 0x0409);
			printf("\n");
			printf(" Serial Number: ");
			FetchString(Handle, DD.SerialNumber, 0x0409);
			printf("\n");
			printf(" Endpoint 0 Max Packet Size %02X\n", DD.MaxPacketSize0);
			printf(" Vender ID %04X, Product ID %04X, Device Version %04X\n", DD.VenderID, DD.ProductID, DD.DeviceVersion);
			printf(" Configuration Count %02X\n", DD.NumConfigurations);
			
			
			AdjustMaxPacketSize(Handle, DD.MaxPacketSize0);

			uint8_t Buffer[0x200];
			
			Request.Request = USBData::GetDescriptor;
			Request.Direction = true;
			Request.Value = 0x200;
			Request.Index = 0;
			Request.Length = 0x30;
			DeviceRequest(Handle, Request, &Buffer);

			printf("%08X\n", Buffer);

			//uint8_t Buffer2[0x100];
			//Buffer2[0] = 0x42;
			//
			//Request.Recipient = USBData::RecipientInterface;
			//Request.Type = USBData::TypeClass;
			//Request.Direction = false;
			//Request.Request = 0x0B;
			//Request.Value = 0;
			//Request.Index = 0;
			//Request.Length = 0;
			//DeviceRequest(Handle, Request, nullptr);

			//Request.Recipient = USBData::RecipientInterface;
			//Request.Type = USBData::TypeClass;
			//Request.Direction = true;
			//Request.Request = 0x03;
			//Request.Value = 0;
			//Request.Index = 0;
			//Request.Length = 1;
			//DeviceRequest(Handle, Request, &Buffer2);

			//printf("%08X\n", Buffer2);

		}
	}

	return true;
}

uint32_t OpenHCI::OpenPipe(uint8_t Address, uint8_t Endpoint, PipeType Type, uint16_t MaxPacketSize, uint8_t Frequency, OpenHCICallbackPtr Callback, bool LowSpeed)
{
	// Return return the default handle for the default pipe
	if(Address == 0)
		return 0;

	PipeData * Handle = GetEndpoint();

	Handle->State = IdleState;
	Handle->Type = Type;
	
	Handle->EndPoint.FunctionAddress = Address;
	Handle->EndPoint.Endpoint = Endpoint;
	Handle->EndPoint.Direction = OpenHCIData::DirectionTD;
	Handle->EndPoint.Speed = LowSpeed;
	Handle->EndPoint.Skip = true;
	Handle->EndPoint.Format = false; // General TD;
	Handle->EndPoint.MaxPacketSize = MaxPacketSize;	
	Handle->EndPoint.NextEndpointDescriptior = 0;

	switch(Type)
	{
		case ControlPipe:
			if(Registers->ControlHeadED == 0)
				 Registers->ControlHeadED = reinterpret_cast<uint32_t>(Handle);
			else
				AddEndpoint(reinterpret_cast<PipeData *>(Registers->ControlHeadED), Handle);
			break;

		case BulkPipe:
			if(Registers->BulkCurrentED == 0)
				 Registers->BulkCurrentED = reinterpret_cast<uint32_t>(Handle);
			else
				AddEndpoint(reinterpret_cast<PipeData *>(Registers->BulkCurrentED), Handle);
			break;

		case InterruptPipe:
			// Work out which layer the interrupt pipe should go
			Handle->Callback = Callback;
			Handle->CallbackData = nullptr;
			break;

		case IsochronusPipe:
			Handle->EndPoint.Format = true;
			Handle->Callback = Callback;
			Handle->CallbackData = nullptr;

			AddEndpoint(IsochronusPipeHead, Handle);
			break;
	}

	return reinterpret_cast<uint32_t>(Handle);
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
				// DisableInterruptPipe(Handle);
				break;

			case IsochronusPipe:
				// This should never happen.
				break;
		}
	}
	else if(DataHandle->Next == nullptr)
	{
		// We're at the end of the chain, so just drop ourselves out.
		DataHandle->Prev->Next = nullptr;
		DataHandle->Prev->EndPoint.NextEndpointDescriptior = 0;
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

	if(FreePipeHead == nullptr)
		FreePipeHead = DataHandle;

	else
		AddEndpoint(FreePipeHead, DataHandle);
}

bool OpenHCI::WritePipe(uint32_t Handle, void *Data, size_t Length)
{
	if(Handle == DefaultDeviceHandle)
		return false;

	PipeData * DataHandle = reinterpret_cast<PipeData *>(Handle);
	if(DataHandle->Type == IsochronusPipe)
	{
		return false;
	}

	TransferData *DataBlocks = GetTransferData();
	DataHandle->EndPoint.Skip = true;
	DataHandle->Data = DataBlocks;
	DataHandle->EndPoint.QueueHeadPointer = reinterpret_cast<uint32_t>(DataBlocks);

	uint32_t BlockLength = Length;
	uint8_t * BufferPtr = reinterpret_cast<uint8_t *>(Data);

	while(BlockLength != 0)
	{
		DataBlocks->Endpoint = DataHandle;
		
		uint32_t TDLength = DataHandle->EndPoint.MaxPacketSize;
		if(TDLength > BlockLength)
			TDLength = BlockLength;

		DataBlocks->TransferDescriptor.Direction = OpenHCIData::DirectionOut;
		DataBlocks->TransferDescriptor.Rounding = true;
		DataBlocks->TransferDescriptor.DelayInterupt = 7;
		DataBlocks->TransferDescriptor.DataToggle = 0;
		DataBlocks->TransferDescriptor.ErrorCount = 0;
		DataBlocks->TransferDescriptor.ConditionCode = OpenHCIData::NotAccessed;
		DataBlocks->TransferDescriptor.CurrentBufferPointer = reinterpret_cast<uint32_t>(BufferPtr);
		DataBlocks->TransferDescriptor.BufferEnd = DataBlocks->TransferDescriptor.CurrentBufferPointer + TDLength - 1;

		DataBlocks->Next = GetTransferData();

		DataBlocks->TransferDescriptor.NextTransferDescriptor = reinterpret_cast<uint32_t>(DataBlocks->Next);
	
		DataBlocks->Next->Prev = DataBlocks;
		DataBlocks = DataBlocks->Next;

		BlockLength -=TDLength;
		BufferPtr += TDLength;
	}

	DataBlocks->Prev->LastDataBlock = true;
	DataBlocks->Prev->TransferDescriptor.DelayInterupt = 0;
	DataBlocks->Endpoint = DataHandle;
	DataBlocks->TransferDescriptor.BufferEnd = 0;
	DataBlocks->TransferDescriptor.CurrentBufferPointer = 0;
	DataBlocks->TransferDescriptor.ConditionCode = OpenHCIData::NotAccessed;

	DataHandle->EndPoint.QueueTailPointer = reinterpret_cast<uint32_t>(DataBlocks);
	DataHandle->ByteCount = Length;
	DataHandle->RequestCount = Length;	
	DataHandle->State = TranferState;

	Registers->InterruptStatus = OpenHCIData::WritebackDoneHead;

	EnableEndpoint(DataHandle);
	
	// Spin
	SpinLockEndpoint(DataHandle);
	
	if(DataHandle->State != CompletedState)
		return false;

	return true;
}

bool OpenHCI::ReadPipe(uint32_t Handle, void *Data, size_t Length, size_t &Read)
{
	if(Handle == DefaultDeviceHandle)
		return false;

	PipeData * DataHandle = reinterpret_cast<PipeData *>(Handle);
	if(DataHandle->Type == IsochronusPipe)
	{
		return false;
	}

	TransferData *DataBlocks = GetTransferData();
	DataHandle->EndPoint.Skip = true;
	DataHandle->Data = DataBlocks;
	DataHandle->EndPoint.QueueHeadPointer = reinterpret_cast<uint32_t>(DataBlocks);

	uint32_t BlockLength = Length;
	uint8_t * BufferPtr = reinterpret_cast<uint8_t *>(Data);

	while(BlockLength != 0)
	{
		DataBlocks->Endpoint = DataHandle;
		
		uint32_t TDLength = DataHandle->EndPoint.MaxPacketSize;
		if(TDLength > BlockLength)
			TDLength = BlockLength;

		DataBlocks->TransferDescriptor.Direction = OpenHCIData::DirectionIn;
		DataBlocks->TransferDescriptor.Rounding = true;
		DataBlocks->TransferDescriptor.DelayInterupt = 7;
		DataBlocks->TransferDescriptor.DataToggle = 0;
		DataBlocks->TransferDescriptor.ErrorCount = 0;
		DataBlocks->TransferDescriptor.ConditionCode = OpenHCIData::NotAccessed;
		DataBlocks->TransferDescriptor.CurrentBufferPointer = reinterpret_cast<uint32_t>(BufferPtr);
		DataBlocks->TransferDescriptor.BufferEnd = DataBlocks->TransferDescriptor.CurrentBufferPointer + TDLength - 1;

		DataBlocks->Next = GetTransferData();

		DataBlocks->TransferDescriptor.NextTransferDescriptor = reinterpret_cast<uint32_t>(DataBlocks->Next);
	
		DataBlocks->Next->Prev = DataBlocks;
		DataBlocks = DataBlocks->Next;

		BlockLength -=TDLength;
		BufferPtr += TDLength;
	}

	DataBlocks->Prev->LastDataBlock = true;
	DataBlocks->Prev->TransferDescriptor.DelayInterupt = 0;
	DataBlocks->Endpoint = DataHandle;
	DataBlocks->TransferDescriptor.BufferEnd = 0;
	DataBlocks->TransferDescriptor.CurrentBufferPointer = 0;
	DataBlocks->TransferDescriptor.ConditionCode = OpenHCIData::NotAccessed;

	DataHandle->EndPoint.QueueTailPointer = reinterpret_cast<uint32_t>(DataBlocks);
	DataHandle->ByteCount = Length;
	DataHandle->RequestCount = Length;
	DataHandle->State = TranferState;

	Registers->InterruptStatus = OpenHCIData::WritebackDoneHead;
	
	EnableEndpoint(DataHandle);
	
	// Spin
	SpinLockEndpoint(DataHandle);
	
	if(DataHandle->State != CompletedState)
		return false;

	return true;
}

bool OpenHCI::DeviceRequest(uint32_t Handle, USBData::USBDeviceRequest &Request, void *Buffer)
{
	PipeData * DataHandle;
	
	if(Handle == DefaultDeviceHandle)
		DataHandle = DefaultHandle;
	else
		DataHandle = reinterpret_cast<PipeData *>(Handle);

	TransferData * Data = GetTransferData();

	DataHandle->Data = Data;
	DataHandle->EndPoint.QueueHeadPointer = reinterpret_cast<uint32_t>(DataHandle->Data);

	Data->Prev = nullptr;	
	Data->Endpoint = DataHandle;

	Data->TransferDescriptor.Direction = OpenHCIData::DirectionSetup;
	Data->TransferDescriptor.Rounding = true;
	Data->TransferDescriptor.DelayInterupt = 7;
	Data->TransferDescriptor.DataToggle = 2;
	Data->TransferDescriptor.ErrorCount = 0;
	Data->TransferDescriptor.ConditionCode = OpenHCIData::NotAccessed;
	Data->TransferDescriptor.CurrentBufferPointer = reinterpret_cast<uint32_t>(&Request);
	Data->TransferDescriptor.BufferEnd = Data->TransferDescriptor.CurrentBufferPointer + sizeof(USBData::USBDeviceRequest) - 1;

	Data->Next = GetTransferData();

	Data->TransferDescriptor.NextTransferDescriptor = reinterpret_cast<uint32_t>(Data->Next);
	
	Data->Next->Prev = Data;
	Data = Data->Next;

	uint32_t Length = Request.Length;
	uint8_t * BufferPtr = reinterpret_cast<uint8_t *>(Buffer);

	while(Length != 0)
	{
		Data->Endpoint = DataHandle;
		
		uint32_t TDLength = DataHandle->EndPoint.MaxPacketSize;
		if(TDLength > Length)
			TDLength = Length;
		Data->TransferDescriptor.Direction = Request.Direction ? OpenHCIData::DirectionIn : OpenHCIData::DirectionOut;
		Data->TransferDescriptor.Rounding = true;
		Data->TransferDescriptor.DelayInterupt = 7;
		Data->TransferDescriptor.DataToggle = 3;
		Data->TransferDescriptor.ErrorCount = 0;
		Data->TransferDescriptor.ConditionCode = OpenHCIData::NotAccessed;
		Data->TransferDescriptor.CurrentBufferPointer = reinterpret_cast<uint32_t>(BufferPtr);
		Data->TransferDescriptor.BufferEnd = Data->TransferDescriptor.CurrentBufferPointer + TDLength - 1;
		Data->Next = GetTransferData();

		Data->TransferDescriptor.NextTransferDescriptor = reinterpret_cast<uint32_t>(Data->Next);
	
		Data->Next->Prev = Data;
		Data = Data->Next;

		Length -= TDLength;
		BufferPtr += TDLength;
	}

	Data->Endpoint = DataHandle;

	Data->TransferDescriptor.Direction = Request.Direction ? OpenHCIData::DirectionOut : OpenHCIData::DirectionIn;
	Data->TransferDescriptor.Rounding = true;
	Data->TransferDescriptor.DelayInterupt = 0;
	Data->TransferDescriptor.DataToggle = 3;
	Data->TransferDescriptor.ErrorCount = 0;
	Data->TransferDescriptor.ConditionCode = OpenHCIData::NotAccessed;
	Data->TransferDescriptor.CurrentBufferPointer = 0;
	Data->TransferDescriptor.BufferEnd = 0;
	Data->LastDataBlock = true;

	Data->Next = GetTransferData();
	Data->TransferDescriptor.NextTransferDescriptor = reinterpret_cast<uint32_t>(Data->Next);
	
	Data->Next->Prev = Data;
	Data = Data->Next;

	Data->Endpoint = DataHandle;
	Data->Next = nullptr;
	Data->TransferDescriptor.ConditionCode = OpenHCIData::NotAccessed;
	Data->TransferDescriptor.BufferEnd = 0;
	Data->TransferDescriptor.CurrentBufferPointer = 0;

	DataHandle->EndPoint.QueueTailPointer = reinterpret_cast<uint32_t>(Data);

	DataHandle->ByteCount = Request.Length;
	DataHandle->RequestCount = Request.Length;
	DataHandle->State = TranferState;
	Registers->InterruptStatus = OpenHCIData::WritebackDoneHead;

	//printf("Sending\n");

	EnableEndpoint(DataHandle);
	
	// Spin
	SpinLockEndpoint(DataHandle);
	
	//while((Registers->InterruptStatus & OpenHCIData::WritebackDoneHead) == 0)
	//	;

	//printf("%08X: %02X %08X\n", DataHandle, DataHandle->CompletionCode, DataHandle->ByteCount);

	if(DataHandle->State != CompletedState)
		return false;

	return true;


}