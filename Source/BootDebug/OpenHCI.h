#include <stdint.h>
#include "USB.h"
#include "RawMemory.h"
#pragma once

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
		uint32_t DownStreamPorts : 8;
		uint32_t PowerSwitchingMode : 1;
		uint32_t NoPowerSwitching : 1;
		uint32_t DeviceType : 1;
		uint32_t OverCurrentProtectionMode : 1;
		uint32_t NoOverCurrentProtection : 1;
		uint32_t Reserved : 11;
		uint32_t PowerOnToGood : 8;

		uint16_t DeviceRemovable;
		uint16_t PortPowerControlMask;

		uint32_t RootStatues;
		uint32_t RootPortStatus[1];
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

	enum DirectionVTypes
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
		NotAccessed2 =			0xE,
		NotAccessed =			0xF,
	};
}

#pragma pack(pop)

struct InterruptContext;
struct PipeData;
struct TransferData;
class InterruptControler;
enum PipeState;

typedef void (*OpenHCICallbackPtr)(uint32_t Handle, uintptr_t * Data);

class OpenHCI
{
	RawMemory Allocator;

	uint32_t m_DeviceID;
	uint8_t m_IRQ;
	InterruptControler *m_IntControler;
	volatile OpenHCIData::HCRegisters * Registers;

	PipeData * FreePipeHead;	
	TransferData * FreeTransferDataHead;

	PipeData * DefaultHandle;
	PipeData * IsochronusPipeHead;

	void AddEndpoint(PipeData *Base, PipeData *EndPoint);
	void EnableEndpoint(PipeData *EndPoint);
	void DisableEndpoint(PipeData *EndPoint, PipeState NewState);

	void SpinLockEndpoint(PipeData *EndPoint);

	PipeData *GetEndpoint();
	TransferData *GetTransferData();

	void FreeTransferData(TransferData *Data);

	bool IsPeriodicEndpoint(PipeData *EndPoint);



	bool FetchString(uint32_t Handle, uint8_t ID, uint16_t Language);

public:
	enum PipeType
	{
		BulkPipe,
		InterruptPipe,
		IsochronusPipe,
		ControlPipe,
	};

	void Interrupt(InterruptContext * Context);
	
	OpenHCI(void);
	~OpenHCI(void);

	bool StartUp(uint32_t DeviceID, InterruptControler *IntControler);

	const static uint32_t DefaultDeviceHandle = 0x00000000;

	uint32_t OpenPipe(uint8_t Address, uint8_t Endpoint, PipeType Type, uint16_t MaxPacketSize, uint8_t Frequency = 0xFF, OpenHCICallbackPtr Callback = nullptr, bool LowSpeed = false);
	//uint32_t OpenPipe(uint8_t Address, uint8_t Endpoint, PipeType Type, uint16_t MaxPacketSize, bool LowSpeed = false);
	//uint32_t OpenPeriodicPipe(uint8_t Address, uint8_t Endpoint, PipeType Type, uint16_t MaxPacketSize, uint8_t Frequency, OpenHCICallbackPtr Callback, bool LowSpeed = false);
	bool AdjustMaxPacketSize(uint32_t Handle, uint16_t MaxPacketSize);
	void ClosePipe(uint32_t Handle);
	bool ReadPipe(uint32_t Handle, void *Data, size_t Length, size_t &Read);
	bool WritePipe(uint32_t Handle, void *Data, size_t Length);
	bool DeviceRequest(uint32_t Handle, USBData::USBDeviceRequest &Request, void *Buffer);
	//bool ResetInterruptPipe(uint32_t Handle, Callback);
	//bool DisableInterruptPipe(uint32_t Handle);
	

	void Dump();
};

