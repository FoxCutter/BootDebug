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
}

#pragma pack(pop)

class EnhancedHCI
{
public:
	EnhancedHCI(void);
	~EnhancedHCI(void);
};

