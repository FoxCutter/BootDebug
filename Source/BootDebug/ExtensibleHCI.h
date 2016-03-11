#include <stdint.h>
#pragma once

#pragma pack(push, 1)

namespace XHCIData
{
	struct HCCR
	{
		uint8_t Length;
		uint8_t Reserved;
		uint16_t Version;

		struct
		{
			uint32_t MaxDeviceSlots : 8;
			uint32_t MaxInterrupters : 11;
			uint32_t Reserved : 5;
			uint32_t MaxPorts : 8;

			uint32_t IsoSchedulingThreshold : 4;
			uint32_t EventRingSegmentMax : 4;
			uint32_t Reserved2 : 13;
			uint32_t MaxScratchPadBuffersHi : 5;
			uint32_t ScratchPadRestore : 1;
			uint32_t MaxScratchPadBuffersLo : 5;

			uint8_t U1DeviceExitLatency;
			uint8_t Reserved3;
			uint16_t U2DeviceExitLatency;
		} StructuralParameters;

		struct
		{
			uint32_t Has64BitAddressing : 1;
			uint32_t BWNegotiation : 1;
			uint32_t ContextSize: 1;
			uint32_t PortPowerControl : 1;
			uint32_t PortIndicators : 1;
			uint32_t LightHCReset : 1;
			uint32_t LatencyToleranceMessaging : 1;
			uint32_t NoSecondarySIDSupport : 1;
			uint32_t ParseAllEventData : 1;
			uint32_t StoppedShortPacket : 1;
			uint32_t StoppedEDTLA : 1;
			uint32_t ContiguousFrameID : 1;
			uint32_t MaxPSASize : 4;
			uint32_t xECP : 16;
		} CapabilityParameters;
	
		uint32_t DoorbellOffset;
		uint32_t RuntimeRegisterSpaceOffset;

		struct
		{
			uint32_t U3Entry : 1;
			uint32_t CMC : 1;
			uint32_t ForceSaveContext: 1;
			uint32_t ComplianceTransition : 1;
			uint32_t LargeESITPayload : 1;
			uint32_t ConfigInformation : 1;
		} CapabilityParameters2;
	};

	struct HCOR
	{
		// Command Register
		struct
		{
			uint32_t RunStop : 1;
			uint32_t ControlerReset : 1;
			uint32_t EnableInterrupts : 1;
			uint32_t HostSystemError : 1;
			uint32_t Reserved : 3;
			uint32_t LightControlerReset : 1;
			uint32_t ControllerSaveState : 1;
			uint32_t ControllerRestoreState : 1;
			uint32_t EnableWrapEvent : 1;
			uint32_t EnableU3Stop: 1;
			uint32_t StoppedShortPacketEnable: 1;
			uint32_t CMEEnable: 1;
		} Command;
		
		// Status Register
		struct
		{
			uint32_t ControlerHalted : 1;
			uint32_t Reserved : 1;
			uint32_t HostSystemError : 1;
			uint32_t USBInterrupt : 1;
			uint32_t PortChangeDetected : 1;
			uint32_t Reserved2 : 3;
			uint32_t SaveStatus : 1;
			uint32_t RestoreStatus : 1;
			uint32_t SaveRestoreError : 1;
			uint32_t ControllerNotReady : 1;
			uint32_t HostCnotrollerError : 1;
		} Status;

		uint32_t PageSize;
		uint32_t Reserved[2];
		uint32_t DeviceNotificationControl;

		// Command Ring Control Register
		struct 
		{
			uint64_t RingCycleState : 1;
			uint64_t CommandStop : 1;
			uint64_t CommandAbort : 1;
			uint64_t CommandRingRunning : 1;
			uint64_t Reserved : 2;
			uint64_t CommandRingPointer;
		} CommandRingControl;
		
		uint32_t Reserved2[4];
		uint64_t DevieContextBase;

		// Configure Register
		struct 
		{
			uint32_t NumberOfDeviceSlotsEnabled : 8;
			uint32_t U3EntryEnabled : 1;
			uint32_t ConfigInformationEnambled : 1;
		} Configure;
	};


	struct PortRegisters
	{
		struct
		{
			uint32_t CurrentConnectStatus : 1;
			uint32_t PortEnabled : 1;
			uint32_t Reserved : 1;
			uint32_t OverCurrent : 1;
			uint32_t Reset : 1;
			uint32_t LinkState : 4;
			uint32_t PortPower : 1;
			uint32_t PortSpeed : 4;
			uint32_t IndicatorControl : 2;
			uint32_t PortLinkStateWrite : 1;

			uint32_t ConnectStatusChange : 1;
			uint32_t EnableStatusChange : 1;
			uint32_t WarmPortResetChange : 1;
			uint32_t OverCurrentStatusChange : 1;
			uint32_t PortResetChange : 1;
			uint32_t PortLinkStateChange : 1;
			uint32_t PortConfigErrorChange : 1;

			uint32_t ColdAttachStatus : 1;
			uint32_t WakeOnConnect : 1;
			uint32_t WakeOnDisconnect : 1;
			uint32_t PortWakeOnOverCurrent : 1;

			uint32_t Reserved2 : 2;

			uint32_t DeviceRemovable : 1;
			uint32_t WarmPortReset : 1;
		} Control;

		struct
		{
			uint8_t U1Timeout;
			uint8_t U2Timeout;
			uint16_t ForceLinkPMAccept : 1;
		} Power;

		struct
		{
			uint8_t L1Status : 3;
			uint8_t RemoweWakeEnable : 1;
			uint8_t BestEffortLatency : 4;
			uint8_t L1DeviceSlot;
			uint16_t HardwareLPMEnabled : 1;
			uint16_t Reserved : 11;
			uint16_t PortTestControl : 4;
		} Link;

		struct
		{
			uint16_t LinkErrorCount;
			uint16_t RXLaneCount : 4;
			uint16_t TXLaneCount : 4;
		} HardwareLPM;
	};
}

#pragma pack(pop)

class ExtensibleHCI
{
public:
	ExtensibleHCI(void);
	~ExtensibleHCI(void);
};

