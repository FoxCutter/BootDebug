#include "OpenHCI.h"
#include "PCI.h"
#include <string.h>
#include <stdio.h>

#pragma section(".HCA", read, write)
__declspec(allocate(".HCA"))  volatile OpenHCIData::HCCA HCCArea;
__declspec(align(32)) volatile OpenHCIData::EndpointDescriptor GenED[10];
__declspec(align(32)) volatile OpenHCIData::GeneralTransferDescriptor GenTD[10];

__declspec(align(32)) uint8_t USBDataBuffer[0xFF];

struct USBDriverRequst
{
	uint8_t Recipient : 5;
	uint8_t Type : 2;
	uint8_t Direction : 1;

	uint8_t Request;
	uint16_t Value;
	uint16_t Index;
	uint16_t Length;
};


OpenHCI::OpenHCI(void)
{
	Registers = nullptr;
	m_DeviceID = 0;
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

bool OpenHCI::DeviceRequest(uint8_t Address, USBData::USBDeviceRequest &Request, void *Buffer)
{
	int TDIndex = 0;

	GenED[0].MaxPacketSize = 0x40;
	GenED[0].Endpoint = 0;
	GenED[0].FunctionAddress = Address;
	GenED[0].Direction = OpenHCIData::DirectionTD;
	GenED[0].Speed = 0;
	GenED[0].QueueHeadPointer = reinterpret_cast<uint32_t>(&GenTD[0]);
	GenED[0].QueueTailPointer = reinterpret_cast<uint32_t>(&GenTD[1]);

	GenTD[TDIndex].Rounding = 1;
	GenTD[TDIndex].Direction = OpenHCIData::DirectionSetup;
	GenTD[TDIndex].DelayInterupt = 7;
	GenTD[TDIndex].DataToggle = 2;
	GenTD[TDIndex].ErrorCount = 0;
	GenTD[TDIndex].ConditionCode = OpenHCIData::NotAccessed;

	GenTD[TDIndex].CurrentBufferPointer = reinterpret_cast<uint32_t>(&Request);
	GenTD[TDIndex].BufferEnd = GenTD[TDIndex].CurrentBufferPointer + sizeof(USBDriverRequst);

	GenTD[TDIndex].NextTransferDescriptor = reinterpret_cast<uint32_t>(&GenTD[TDIndex + 1]);
	GenED[0].QueueTailPointer = reinterpret_cast<uint32_t>(&GenTD[TDIndex + 1]);

	TDIndex++;

	uint32_t Length = Request.Length;
	uint8_t * BufferPtr = &USBDataBuffer[0];

	while(Length != 0)
	{
		uint32_t TDLength = GenED[0].MaxPacketSize;
		//if(TDLength > Length)
			TDLength = Length;

		GenTD[TDIndex].Rounding = 1;
		GenTD[TDIndex].Direction = Request.Direction ? OpenHCIData::DirectionIn : OpenHCIData::DirectionOut;
		GenTD[TDIndex].DelayInterupt = 7;
		GenTD[TDIndex].DataToggle = 3;
		GenTD[TDIndex].ErrorCount = 0;
		GenTD[TDIndex].ConditionCode = OpenHCIData::NotAccessed;

		GenTD[TDIndex].CurrentBufferPointer = reinterpret_cast<uint32_t>(BufferPtr);
		GenTD[TDIndex].BufferEnd = GenTD[TDIndex].CurrentBufferPointer + TDLength;

		GenTD[TDIndex].NextTransferDescriptor = reinterpret_cast<uint32_t>(&GenTD[TDIndex + 1]);
		GenED[0].QueueTailPointer = reinterpret_cast<uint32_t>(&GenTD[TDIndex + 1]);

		TDIndex++;
		Length -=TDLength;
		BufferPtr += TDLength;
	}

	GenTD[TDIndex].Rounding = 1;
	GenTD[TDIndex].Direction = Request.Direction ? OpenHCIData::DirectionOut : OpenHCIData::DirectionIn;
	GenTD[TDIndex].DelayInterupt = 0;
	GenTD[TDIndex].DataToggle = 3;
	GenTD[TDIndex].ErrorCount = 0;
	GenTD[TDIndex].ConditionCode = OpenHCIData::NotAccessed;

	GenTD[TDIndex].CurrentBufferPointer = 0;
	GenTD[TDIndex].BufferEnd = 0;
	GenTD[TDIndex].NextTransferDescriptor = reinterpret_cast<uint32_t>(&GenTD[TDIndex + 1]);
	GenED[0].QueueTailPointer = reinterpret_cast<uint32_t>(&GenTD[TDIndex + 1]);

	Registers->InterruptStatus = OpenHCIData::WritebackDoneHead;

	Registers->ControlHeadED = reinterpret_cast<uint32_t>(&GenED[0]);
	Registers->CommandStatus = OpenHCIData::ControlListFilled;

	while((Registers->InterruptStatus & OpenHCIData::WritebackDoneHead) == 0)
		;

	//printf("%08X: %02X\n", &GenED[0], GenTD[TDIndex].ConditionCode);
	//printf("%08X-%02X, %08X\n", (char *)USBDataBuffer, Request.Length, Buffer);

	if(GenTD[TDIndex].ConditionCode != OpenHCIData::NoError)
		return false;

	if(Request.Length != 0)
	{
		memcpy(Buffer, (char *)USBDataBuffer, Request.Length);		
	}

	return true;
}

bool OpenHCI::FetchString(uint8_t Address, uint8_t ID, uint16_t LanguageID)
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
	DeviceRequest(0x42, Request, &Buffer);

	int count = (sd->Length - 2) / 2;

	for(int x = 0; x < count; x++)
	{
		printf("%c", sd->Value[x]);
	}
	//printf("\n");
	return true;
}


bool OpenHCI::StartUp(uint32_t DeviceID)
{
	m_DeviceID = DeviceID;
	//printf("Open HCI Startup %08X\n", DeviceID);
	
	Registers = reinterpret_cast<OpenHCIData::HCRegisters *>(PCI::ReadRegister(m_DeviceID, 0x10));
	
	uint16_t temp = Registers->FrameInterval;
	Registers->CommandStatus = OpenHCIData::HostControllerReset;
	while(Registers->HostControllerFunctionalState != OpenHCIData::USBSuspened)
		;
	
	Registers->FrameInterval = temp;
	memset((void *)&HCCArea, 0, sizeof(OpenHCIData::HCCA));
	memset((void *)&GenED, 0, sizeof(OpenHCIData::EndpointDescriptor) * 2);
	memset((void *)&GenTD, 0, sizeof(OpenHCIData::GeneralTransferDescriptor) * 2);

	Registers->HCCAPointer = reinterpret_cast<uint32_t>(&HCCArea);

	Registers->InterruptEnable = OpenHCIData::EnableAllInterupts;
	Registers->ControlBulkServiceRatio = OpenHCIData::ControlBulkServiceRatio11;
	Registers->PeriodicListEnable = true;
	Registers->IsochronusEnable = true;
	Registers->ControlListEnable = true;
	Registers->BulkListEnable = true;

	Registers->PeriodicStart = temp - (temp / 10);
	
	Registers->HostControllerFunctionalState = OpenHCIData::USBOperational;

	//for(unsigned int x = 0; x < Registers->DownStreamPorts; x++)
	//{
	//	if(Registers->RootPortStatus[x] & OpenHCIData::PortCurrentConnectStatus)
	//	{	
	//		Registers->RootPortStatus[x] = Registers->RootPortStatus[x] | OpenHCIData::SetPortReset;
	//		while((Registers->RootPortStatus[x] & OpenHCIData::PortResetStatusChange) == 0)
	//			;

	//		uint8_t Buffer[0x200];
	//		
	//		USBData::USBDeviceRequest Request;
	//		Request.Direction = false;
	//		Request.Type = USBData::TypeStandard;
	//		Request.Recipient = USBData::RecipientDevice;
	//		Request.Request = USBData::SetAddress;
	//		Request.Value = 0x42;
	//		Request.Index = 0;
	//		Request.Length = 0;
	//		
	//		DeviceRequest(0, Request, nullptr);
	//		
	//		USBData::DeviceDescriptor DD;

	//		Request.Request = USBData::GetDescriptor;
	//		Request.Direction = true;
	//		Request.Value = 0x100;
	//		Request.Index = 0;
	//		Request.Length = sizeof(USBData::DeviceDescriptor);
	//		DeviceRequest(0x42, Request, &DD);

	//		printf("USB Device\n");
	//		printf(" USB Version %04X\n", DD.USBVersion);
	//		printf(" Class %02X, Subclass %02X, Protocol %02X\n", DD.DeviceClass, DD.DeviceSubClass, DD.DeviceProtocol);
	//		printf(" Manufacturer: ");
	//		FetchString(0x42, DD.Manufacturer, 0x0409);
	//		printf("\n");
	//		printf(" Product: ");
	//		FetchString(0x42, DD.Product, 0x0409);
	//		printf("\n");
	//		printf(" Serial Number: ");
	//		FetchString(0x42, DD.SerialNumber, 0x0409);
	//		printf("\n");
	//		printf(" Endpoint 0 Max Packet Size %02X\n", DD.MaxPacketSize0);
	//		printf(" Vender ID %04X, Product ID %04X, Device Version %04X\n", DD.VenderID, DD.ProductID, DD.DeviceVersion);
	//		printf(" Configuration Count %02X\n", DD.NumConfigurations);
	//		
	//		
	//		Request.Request = USBData::GetDescriptor;
	//		Request.Direction = true;
	//		Request.Value = 0x200;
	//		Request.Index = 0;
	//		Request.Length = 0x100;
	//		DeviceRequest(0x42, Request, &Buffer);

	//		printf("%08X\n", Buffer);

	//	}
	//}
	
	return false;
}
