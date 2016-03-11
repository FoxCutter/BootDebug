#include <stdint.h>
#include "USBData.h"
#include "RawMemory.h"
#pragma once

#pragma pack(push, 1)

namespace EHCIData
{
	struct HCCR
	{
		uint8_t Length;
		uint8_t Reserved;
		uint16_t Version;

		struct
		{
			uint32_t PortCount : 4;
			uint32_t PortPowerControl : 1;
			uint32_t Reserved: 2;
			uint32_t PortRoutingRules: 1;
			uint32_t PortsPerCompanionControler : 4;
			uint32_t CompanionControlerCount : 4;
			uint32_t PortIndicators : 1;
			uint32_t Reserved2 : 4;
			uint32_t DebugPortNumber : 4;
		} StructuralParameters;

		struct
		{
			uint32_t Has64BitAddressing : 1;
			uint32_t ProgramableFramesList : 1;
			uint32_t AsynchronousScheduleParkCapability : 1;
			uint32_t Reverved : 1;
			uint32_t IsochronousSchedulingThreshold : 4;
			uint32_t EECP : 8;
		} CapabilityParameters;
	
		uint64_t PortRoutes;
	};

	struct HCOR
	{
		// Command Register
		struct
		{
			uint32_t RunStop : 1;
			uint32_t ControlerReset : 1;
			uint32_t FrameSize : 2;
			uint32_t PeriodicSchedualEnabled : 1;
			uint32_t AsyncSchedualEnabled : 1;
			uint32_t InterruptOnAsyncDoorbell : 1;
			uint32_t LightControlerReset : 1;
			uint32_t AsyncParkModeCount : 2;
			uint32_t Reserved : 1;
			uint32_t AsyncParkModeEnabled : 1;
			uint32_t Reserved2 : 1;
			uint32_t InterruptThresholdControl : 8;
		} Command;
		
		// Status Register
		struct
		{
			uint32_t USBInterrupt : 1;
			uint32_t USBErrorInterrupt : 1;
			uint32_t PortChangeDetected : 1;
			uint32_t FrameListRollover : 1;
			uint32_t HostSystemError : 1;
			uint32_t InterruptOnAsyncAdvance : 1;
			uint32_t Reserved : 6;
			uint32_t ControlerHalted : 1;
			uint32_t Reclimation : 1;
			uint32_t PeriodicSchedualStatus : 1;
			uint32_t AsyncSchedualStatus : 1;
		} Status;

		// Interrupts Register
		struct 
		{
			uint32_t EnableInterrupts : 1;
			uint32_t USBErrorInterruptEnable : 1;
			uint32_t PortChangeDetectedEnable : 1;
			uint32_t FrameListRolloverEnable : 1;
			uint32_t HostSystemErrorEnable : 1;
			uint32_t InterruptOnAsyncAdvanceEnable : 1;
		} Interrupts;
		
		uint32_t FrameIndex;
		uint32_t HighAddressSegment; 
		uint32_t PeriodicListBase;
		uint32_t AsyncListBase;
		uint32_t Reserved[9];

		uint32_t ConfigFlag;

		struct
		{
			uint32_t CurrentConnectStatus : 1;
			uint32_t ConnectStatusChange : 1;
			uint32_t EnableStatus : 1;
			uint32_t EnableStatusChange : 1;
			uint32_t OverCurrentStatus : 1;
			uint32_t OverCurrentStatusChange : 1;
			uint32_t ForceResume : 1;
			uint32_t Suspend : 1;
			uint32_t Reset : 1;
			uint32_t Reserved : 1;
			uint32_t LineStatus : 2;
			uint32_t Power : 1;
			uint32_t Owner : 1;
			uint32_t IndicatorControl : 2;
			uint32_t TestControl : 4;

			uint32_t WakeOnConnect : 1;
			uint32_t WakeOnDisconnect : 1;
			uint32_t PortWakeOnOverCurrent : 1;

		} PortControl[1];
	};


	enum LinkPointerFlags
	{
		TerminateLink			= 0x01,
		IsochronousTDLink		= 0x00,
		QueueHeadLink			= 0x02,
		SplitIsochronousTDLink	= 0x04,
		FrameSpanNodeLink		= 0x06
	};

	struct iTDTransaction
	{
		uint32_t Offset : 12;
		uint32_t PageSelect : 3;
		uint32_t IntOnComplete : 1;
		uint32_t Length : 12;
		uint32_t TransactionError : 1;
		uint32_t BabbleDetected : 1;
		uint32_t DataBufferError : 1;
		uint32_t Active : 1;

	};
	
	struct iTD
	{
		uint32_t			NextLinkPointer;

		iTDTransaction		Transaction0;
		iTDTransaction		Transaction1;
		iTDTransaction		Transaction2;
		iTDTransaction		Transaction3;
		iTDTransaction		Transaction4;
		iTDTransaction		Transaction5;
		iTDTransaction		Transaction6;
		iTDTransaction		Transaction7;

		uint32_t			DeviceAddress : 7;
		uint32_t			Reserved : 1;
		uint32_t			EndPoint : 4;
		uint32_t			BufferPointer0 : 20;

		uint32_t			MaximumPacketSize : 11;
		uint32_t			Direction : 1;
		uint32_t			BufferPointer1 : 20;
		
		uint32_t			Multi : 2;
		uint32_t			Reserved2 : 10;
		uint32_t			BufferPointer2 : 20;

		uint32_t			BufferPointer3;
		uint32_t			BufferPointer4;
		uint32_t			BufferPointer5;
		uint32_t			BufferPointer6;
		uint32_t			BufferPointer7;
	};

	struct siTD
	{
		uint32_t			NextLinkPointer;

		// Endpoint and Transaction Translator Characteristics
		uint32_t			DeviceAddress : 7;
		uint32_t			Reserved1 : 1;
		uint32_t			EndPoint : 4;
		uint32_t			Reserved2 : 4;
		uint32_t			HubAddress : 7;
		uint32_t			Reserved3 : 1;
		uint32_t			PortNumber : 7;
		uint32_t			Direction : 1;

		// Micro-frame Schedule Control
		uint8_t				uFrameSMask;
		uint8_t				uFrameCMask;
		uint16_t			Reserved4;

		// siTD Transfer State
		uint8_t				Status;
		uint8_t				uFrameCProgMask;

		uint16_t			TotalBytes : 10;
		uint16_t			Reserved5 : 4;
		uint16_t			PageSelect : 1;
		uint16_t			IntOnComplete : 1;

		// siTD Buffer Pointer List (plus)
		uint32_t			CurrentOffset : 12;
		uint32_t			BufferPointer0 : 20;

		uint32_t			TCount : 3;
		uint32_t			Position : 2;
		uint32_t			Reserved6 : 7;
		uint32_t			BufferPointer1 : 20;

		uint32_t			BackPointer;
	};

	struct qTD
	{
		uint32_t			NextLinkPointer;
		uint32_t			AltNextLinkPointer;

		uint8_t				Status;
		uint8_t				PIDCode : 2;
		uint8_t				ErrorCounter : 2;
		uint8_t				CurrentPage : 3;
		uint8_t				IntOnComplete : 1;

		uint16_t			TotalBytes : 15;
		uint16_t			DataToggle : 1;

		uint32_t			CurrentOffset : 12;
		uint32_t			BufferPointer0 : 20;
		uint32_t			BufferPointer1;
		uint32_t			BufferPointer2;
		uint32_t			BufferPointer3;
		uint32_t			BufferPointer4;
	};

	struct QueueHead
	{
		uint32_t			NextQueueHeadPointer;

		// Endpoint Characteristics
		uint32_t			DeviceAddress : 7;
		uint32_t			InactiveOnNext : 1;
		uint32_t			EndPoint : 4;
		uint32_t			EndPointSpeed : 2;
		uint32_t			DataToggleControl : 1;
		uint32_t			HeadOfReclamationList : 1;
		uint32_t			MaxPacketLength : 10;
		uint32_t			ControlEndpoint : 1;
		uint32_t			NakCount : 5;

		uint8_t				uFrameSMask;
		uint8_t				uFrameCMask;

		uint16_t			HubAddress : 7;
		uint16_t			PortNumber : 7;
		uint16_t			Multiplier : 2;

		uint32_t			CurrentQueuePointer;
		uint32_t			NextQueuePointer;
		uint32_t			AlternateNextQueuePointer;


		uint8_t				Status;
		uint8_t				PIDCode : 2;
		uint8_t				ErrorCounter : 2;
		uint8_t				CurrentPage : 3;
		uint8_t				IntOnComplete : 1;

		uint16_t			TotalBytes : 15;
		uint16_t			DataToggle : 1;

		uint32_t			CurrentOffset : 12;
		uint32_t			BufferPointer0 : 20;

		uint32_t			CProgMask : 12;
		uint32_t			BufferPointer1 : 20;

		uint32_t			FrameTag : 5;
		uint32_t			SBytes : 7;
		uint32_t			BufferPointer2 : 20;

		uint32_t			BufferPointer3;
		uint32_t			BufferPointer4;

	};

	struct gcnew
	{
		uint32_t NormalPathLinkPointer;
		uint32_t BackPathLinkPointer;
	};
}

#pragma pack(pop)

class EnhancedHCI
{
public:
	EnhancedHCI(void);
	~EnhancedHCI(void);
};

