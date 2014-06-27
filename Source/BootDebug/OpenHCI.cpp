#include "OpenHCI.h"
#include "PCI.h"
#include <stdio.h>

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
		uint32_t Control;
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

		uint32_t RootDescriptiorA;
		uint32_t RootDescriptiorB;
		uint32_t RootStatues;
		uint32_t RootPortStatus[1];
	};

	enum ControlFlags
	{
		CBSRMask =					0x003,
		CBSR0 =						0x000,
		CBSR1 =						0x001,
		CBSR2 =						0x002,
		CBSR3 =						0x003,

		PeriodicListEnable =		0x004,
		IsochronusEnable =			0x008,
		ControlListEnable =			0x010,
		BulkListEnable =			0x020,
		
		USBStateMask =				0x0C0,
		USBReset =					0x000,
		USBResume =					0x040,
		USBOperatinal =				0x080,
		USBSuspened =				0x0C0,

		InterruptRounting =			0x100,
		RemoteWakeupConnected =		0x200,
		RemoteWakeupEnabled =		0x400,
	};

	enum StatusFlags
	{
		HostControllerReset = 0x0001,
		ControlListFilled = 0x0002,
		BulkListFilled = 0x0004,
		OwnershipChangeRequest = 0x0008,

		SchedulingOverrunCountMask = 0x0300,
		SchedulingOverrunCountShift = 0x8,
	};

	enum InterruptFlags
	{
		SchedulingOverrun =			0x0001,
		WritebackDoneHead =			0x0002,
		StartOfFrame =				0x0004,
		ResumeDetected =			0x0008,
		UnrecoverableError =		0x0010,
		FrameNumberOverflow =		0x0020,
		RootHubStatusChange =		0x0040,
		OwnershipChange =			0x4000,
		MasterInterruptEnable =		0x8000,
	};
	
	enum FrameIntervalFlags
	{
		FrameIntervalToggle = 0x8000,
		FrameRemaining = 0x80000000,
	};


	enum RootDescriptiorFlags
	{
		NumPortsMask =					0x000000FF,
		PowerSwitichingMode =			0x00000100,
		NoPowerSwitching =				0x00000200,
		DeviceType =					0x00000400,
		OverCurrentProtectionMode =		0x00000800,
		NoOverCurrentProtection =		0x00001000,

		PowerOnToPowerGoodTimeMask =	0xFF000000,
		PowerOnToPowerGoodTimeShift =	0x18,
	};

	enum RootStatusFlags
	{
		LocalPowerStatus =				0x00000001,
		OverCurrentIndicator =			0x00000002,
		DeviceRemoteWakeupEnable =		0x00008000,
		LocalPowerStatusChange =		0x00010000,
		OverCurrentIndicatorChange =	0x00020000,
		ClearRemoteWakeupEnable =		0x80000000,
	};

	enum PortStatusFlags
	{
		CurrentConnectStatus =				0x00000001,
		PortEnableStatus =					0x00000002,
		PortSuspendStatus =					0x00000004,
		PortOverCurrentIndicator =			0x00000008,
		PortResetStatus =					0x00000010,
		PortPowerStatus =					0x00000100,
		LowSpeedDeviceAttached =			0x00000200,
		ConnectStatusChange =				0x00010000,
		PortEnableStatusChange =			0x00020000,
		PortSuspendStatusChange =			0x00040000,
		PortOverCurrentIndicatorChange =	0x00080000,
		PortResetStatusChange =				0x00100000,
	};

	struct EndpointDescriptor
	{
		uint16_t MaxPacketSize;
		uint16_t Flags;
		uint32_t QueueTailPointer;
		uint32_t QueueHeadPointer;
		uint32_t NextEndpointDescriptior;
	};


	enum EndpointFlags
	{
		FunctionAddressMask =	0x003F,
		EndpointMask =			0x0780,
		EndpointShift =			0x7,
		DirectionMask =			0x1800,
		DirectionShift =		0xB,

		GetDirectionFromTD =	0x0,
		DirectionOut =			0x1,
		DirectionIn =			0x2,

		Speed =					0x2000,
		Skip =					0x4000,
		Format =				0x8000,

		Halted =				0x0001,
		ToggleCarry =			0x0002,
	};

	struct GeneralTransferDescriptor
	{
		uint16_t Flags;
		uint16_t Reserved;
		uint32_t CurrentBufferPointer;
		uint32_t NextTransferDescriptor;
		uint32_t BufferEnd;
	};

	struct IsochronusTransferDescriptor
	{
		uint16_t Flags;
		uint16_t StartingFrame;
		uint32_t BufferPage0;
		uint32_t NextTransferDescriptor;
		uint32_t BufferEnd;
		uint16_t OffsetPSW[8];
	};

	enum TransferDescriptorFlags
	{
		BufferRounding =		0x0004,

		DirectionPIDMask =		0x0018,
		DirectionPIDShift =		0x2,

		DelayInteruptMask =		0x00E0,
		DelayInteruptShift =	0x3,

		DataToggleMask =		0x0300,
		DataToggleShift =		0x5,

		StartingFrameMask =		0x0380,
		StartingFrameShift =	0x7,

		ErrorCountMask =		0x0C00,
		ErrorCountShift =		0xA,

		ConditionCodeMask =		0xF000,
		ConditionCodeShift =	0x12,

		DirectionSetup =		0x00,
	};

	enum PSWFlags
	{
		SizeMask =				0x07FF,
		//ConditionCodeMask =	0xF000,
		//ConditionCodeShift =	0x12,
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
		NotAccessed2 =			0xE,
		NotAccessed =			0xF,
	};
}

#pragma pack(pop)

OpenHCI::OpenHCI(void)
{
}


OpenHCI::~OpenHCI(void)
{
}

bool OpenHCI::StartUp(uint32_t DeviceID)
{
	printf("Open HCI Startup %08X\n", DeviceID);
	
	OpenHCIData::HCRegisters *HRReg = reinterpret_cast<OpenHCIData::HCRegisters *>(PCI::ReadRegister(DeviceID, 0x10));

	printf("HCRegisters, %08X\n", HRReg);

	
	return false;
}
