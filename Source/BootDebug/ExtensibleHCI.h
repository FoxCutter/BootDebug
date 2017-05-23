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
		uint64_t DevieContextArrayBase;

		// Configure Register
		struct 
		{
			uint32_t NumberOfDeviceSlotsEnabled : 8;
			uint32_t U3EntryEnabled : 1;
			uint32_t ConfigInformationEnabled : 1;
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

		union
		{
			struct
			{
				struct
				{
					uint8_t U1Timeout;
					uint8_t U2Timeout;
					uint16_t ForceLinkPMAccept : 1;
				} PowerManagment;

				struct
				{
					uint16_t LinkErrorCount;
					uint16_t RXLaneCount : 4;
					uint16_t TXLaneCount : 4;
				} LinkInfo;

				uint32_t HardwardLPMControl; // Reserved
	
			} USB3;

			struct
			{
				struct
				{
					uint8_t L1Status : 3;
					uint8_t RemoteWakeEnable : 1;
					uint8_t BestEffortServiceLatence : 4;
		
					uint8_t L1DeviceSlot;

					uint32_t HardwareLPMEnable : 1;
					uint32_t Reserved : 27;
					uint32_t PortTestControl : 4;
				} PowerManagment;

				uint32_t LinkInfo; // Reserved;

				struct
				{
					uint32_t HIRDM : 2;
					uint32_t L1Timeout : 8;
					uint32_t BESLD : 4;

				} HardwardLPMControl;

			} USB2;
		};
	};

	struct InterrupterRegisterSet
	{
		struct 
		{
			uint32_t Pending : 4;
			uint32_t Enable : 1;
		};

		uint16_t InterruptModerationInterval;
		uint16_t InterruptModerationCounter;

		uint16_t EventRingSegmentTableSize;

		uint8_t Reserved[6];

		uint64_t EventRingEnqueuePointer;
		struct 
		{
			uint64_t ERSTSegmentIndex : 4;
			uint64_t EventHandlerBusy : 1;
			uint64_t EventRingDequeuePointer;
		} EventRingDequeue;
	};
	
	struct DoorbellRegister
	{
		uint8_t Target;
		uint8_t Reserved;
		uint16_t StreamID;
	};
	
	
	struct TransferDescriptor
	{
		uint64_t Dequeue;
		uint64_t Enqueue;
	};

	struct NormalTransferRequestBlock
	{
		uint64_t Data;
		uint32_t Length : 17;
		uint32_t Size : 5;
		uint32_t InterrupterTarget : 10;
		
		uint16_t Cycle : 1;
		uint16_t EvalutateNextTRB : 1;
		uint16_t InterruptOnShortPacket : 1;
		uint16_t NoSnoop : 1;
		uint16_t Chain : 1;
		uint16_t InterruptOnCompletion : 1;
		uint16_t ImmediateData : 1;
		uint16_t TransferBurstCount : 2;
		uint16_t BlockEventInterrupt : 1;
		uint16_t Type : 6;

		union
		{
			uint16_t TransferType;
			uint16_t Direction;
			
			struct 
			{
				uint16_t TransferLastBurstPacketCount : 4;
				uint16_t FrameID : 11;
				uint16_t StartIsochASAP : 1;
			};

			uint16_t Reserved;
		};
	};

	struct EventTransferRequestBlock
	{
		uint64_t Data;
		uint32_t Length : 24;
		uint32_t CompletionCode: 8;
		
		uint16_t Cycle : 1;
		uint16_t Reserved1 : 1;
		uint16_t EventData : 1;
		uint16_t Reserved2 : 7;
		uint16_t Type : 6;

		uint8_t OriginID; // Slot or VF ID
		uint8_t SlotID;
	};

	struct CommandTransferRequestBlock
	{
		uint64_t Data;
		uint32_t Reserved1;
		
		uint16_t Cycle : 1;
		uint16_t Reserved2 : 9;
		uint16_t Type : 6;

		union
		{
			struct {
				uint8_t ExtraA;
				uint8_t ExtraB;
			};

			uint16_t ExtraC;
		};
	};

	struct LinkTransferRequestBlock
	{
		uint64_t Data;
		uint32_t Reserved : 22;
		uint32_t InterrupterTarget : 10;
		
		uint16_t Cycle : 1;
		uint16_t ToggleCycle : 1;
		uint16_t Reserved1 : 2;
		uint16_t Chain : 1;
		uint16_t InterruptOnCompletion : 1;
		uint16_t Reserved2 : 4;
		uint16_t Type : 6;

		uint16_t Reserved3;
	};

	enum class TRBType
	{
		Reserved = 0,
		
		// Transfer Ring
		Normal,
		SetupStage,
		DataStage,
		StatusStage,
		Isoch,
		Link,		// Also Allowed in Command
		EventData,
		NoOp,

		// Command Ring
		EnableSlot,
		DisableSlot,
		AddressDevice,
		ConfigureEndpoint,
		EvaluateContext,
		ResetEndpoint,
		StopEntpoint,
		SetTRDDequeuePointer,
		ResetDevice,
		ForceEvent,
		NegotiateBandwidth,
		SetLatencyToleranceValue,
		GetPortBandwidth,
		ForceHeader,
		NoOpCommand,

		// Event Ring
		TrasferEvent = 32,
		CommandCompeltion,
		PortStatusChange,
		BandwidthRequest,
		Doorbell,
		HostController,
		DeviceNotification,
		MFINDEXWrap,
		
	};

	struct EnpointContext
	{
		uint8_t State;
		uint8_t Multi: 2;
		uint8_t MaxPrimaryStreams : 5;
		uint8_t LinearStreamArray : 1;
		uint8_t Interval;
		uint8_t MaxESITPayloadHi;

		uint8_t Reserved1 : 1;
		uint8_t ErrorCount : 2;
		uint8_t Type : 3;
		uint8_t Reserved2 : 1;
		uint8_t HostInitiateDisable : 1;
			
		uint8_t MaxBurstSize;
		uint16_t MaxPacketSize;
		uint64_t DequeuePointer;
		uint16_t AdverageTRBLength;
		uint16_t MaxESTIPayloadLo;
		uint32_t xHCIReserved[3];
	};
		
	enum class EndPointState 
	{
		Disabled = 0,
		Running,
		Halted,
		Stopped,
		Error,
	};
	
	enum class EndPointType 
	{
		Invalid = 0,
		IsochOut,
		BulkOut,
		InterruptOut,
		Control,
		IsochIn,
		BulkIn,
		InterruptIn,
	};

	struct DeviceContext
	{
		struct
		{
			uint32_t Route : 20;
			uint32_t Speed : 4;
			uint32_t Reserved1 : 1;
			uint32_t MultiTT : 1;
			uint32_t Hub : 1;
			uint32_t ContextEntries: 5;

			uint16_t MaxExitLatancy;
			uint8_t RootHubPort;
			uint8_t NumberOfPorts;
			
			uint8_t TTHubSlotID;
			uint8_t TTPortNumber;
			uint16_t TTThinkTime : 2;
			uint16_t Reserved2 : 4;
			uint16_t InterrupterTarget : 10;

			uint8_t USBDeviceAddress;
			uint16_t Reserved3;
			uint8_t Reserved4 : 2;
			uint8_t State : 5;

			uint32_t xHCIReserved[4];
		} SlotContext;

		EnpointContext Endpoints[32];
	};

	enum class SlotState 
	{
		Disabled = 0,
		Default,
		Addressed,
		Configured,
	};

	struct StreamContext
	{
		uint64_t DequeueCycleState : 1;
		uint64_t StreamContextType : 3;
		uint64_t DequeuePointer;
		uint32_t StoppedEDTLA;
		uint32_t xHCIReserved;
	};

	struct InputContext
	{
		struct
		{
			uint32_t DropContextFlags;
			uint32_t AddContextFlags;
			
			uint32_t xHCIReserved[5];

			uint8_t ConfigurationValue;
			uint8_t InterfaceNumber;
			uint8_t AlternateSetting;
			uint8_t Reserved;
			
		} ControlContext;

		DeviceContext Context;
	};

	struct EventRingSegmentTable
	{
		uint64_t RingSegementAddress;
		uint16_t SegmentSize;
		uint16_t Reserved[3];
	};

	struct SupportedProtocolCapability
	{
		uint8_t CapabilityID;
		uint8_t NextPointer;
		uint8_t RevisionMinor;
		uint8_t RevisionMajor;
		uint8_t Name[4];
		uint8_t CompatiblePortOffset;
		uint8_t CompatiblePortCount;
		uint16_t ProtocolDefined : 12;
		uint16_t SpeedIDCount : 4;
		uint32_t SlotType : 4;
	};

	struct ProtocolSpeedID
	{
		uint16_t Value : 4;
		uint16_t Exponent : 2;
		uint16_t Type : 2;
		uint16_t FullDuplex : 1;

		uint16_t Mantissa;
	};
}

#pragma pack(pop)

class ExtensibleHCI
{
public:
	ExtensibleHCI(void);
	~ExtensibleHCI(void);
};

