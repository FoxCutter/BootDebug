#include <stdint.h>
#pragma once
#pragma pack(push, 1)

namespace USBData
{
	struct USBDeviceRequest
	{
		union
		{
			uint8_t RequestType;
			struct
			{
				uint8_t Recipient : 5;
				uint8_t Type : 2;
				uint8_t Direction : 1; // False: Host to Device, True: Device to Host
			};
		};

		uint8_t Request;
		uint16_t Value;
		uint16_t Index;
		uint16_t Length;
	};

	enum USBDeviceRequestEnums
	{
		TypeStandard		= 0,
		TypeClass			= 1,
		TypeVender			= 2,
		TypeReserved		= 3,

		RecipientDevice		= 0,
		RecipientInterface	= 1,
		RecipientEndpoint	= 2,
		RecipientOther		= 3
	};

	enum USBDeviceRequestIDs
	{
		GetStatus			= 0,
		ClearFeature		= 1,
		SetFeature			= 3,
		SetAddress			= 5,
		GetDescriptor		= 6,
		SetDescriptor		= 7,
		GetConfiguration	= 8,
		SetConfiguration	= 9,
		GetInterface		= 10,
		SetInterface		= 11,
		SynchFrame			= 12,
	};

	enum DescriptorTypes
	{
		Device					= 1,
		Configuration			= 2,
		String					= 3,
		Interface				= 4,
		Endpoint				= 5,
		DeviceQualifier			= 6,	// USB 2
		OtherSpeedConfiguration = 7,	// USB 2
		IntefacePower			= 8,
		OTG						= 9,
		Debug					= 10,
		InterfaceAssociation	= 11,	// USB 3
		BOS						= 15,	// USB 3
		DeviceCapability		= 16,	// USB 3
		SuperspeedEndpoint		= 48,	// USB 3
		SuperspeedIsoEndpoint	= 49,	// USB 3
	};

	enum USBDeviceFeatures
	{
		// Device Features
		DeviceRemoteWakeup		= 1,
		TestMode				= 2,	// USB 2
		
		// Interface Features

		// Endpoint Features
		EndpointHalt			= 0,
	};

	struct DeviceDescriptor
	{
		uint8_t		Length;
		uint8_t		DescriptorType;

		uint16_t	USBVersion;

		uint8_t		DeviceClass;
		uint8_t		DeviceSubClass;
		uint8_t		DeviceProtocol;

		uint8_t		MaxPacketSize0;

		uint16_t	VenderID;
		uint16_t	ProductID;
		uint16_t	DeviceVersion;

		uint8_t		Manufacturer;
		uint8_t		Product;
		uint8_t		SerialNumber;

		uint8_t		NumConfigurations;
	};

	struct ConfigurationDescriptor
	{
		uint8_t		Length;
		uint8_t		DescriptorType;

		uint16_t	TotalLength;
		uint8_t		NumInterfaces;
		uint8_t		ConfigurationValue;
		uint8_t		Configuration;

		uint8_t		Reserved : 5;
		uint8_t		RemoteWakeup : 1;
		uint8_t		SelfPowered : 1;

		uint8_t		MaxPower;
	};

	struct StringDescriptor
	{
		uint8_t		Length;
		uint8_t		DescriptorType;
		
		uint16_t	Value[1];
	};

	struct InterfaceDescriptor
	{
		uint8_t		Length;
		uint8_t		DescriptorType;

		uint8_t		InterfaceNumber;
		uint8_t		AlternateSetting;
		uint8_t		NumEndpoints;

		uint8_t		InterfaceClass;
		uint8_t		InterfaceSubClass;
		uint8_t		InterfaceProtical;

		uint8_t		Interface;
	};

	struct EndpointDescriptor
	{
		uint8_t		Length;
		uint8_t		DescriptorType;

		uint8_t		EndpointAddress : 4;
		uint8_t		Reserved : 3;
		uint8_t		Direction : 1;

		uint8_t		TransferType : 2;
		uint8_t		SynchronizationType : 2;	// USB 2
		uint8_t		UsageType : 2;				// USB 2

		uint16_t	MaxPacketSize;
		uint8_t		Interval;
	};

	enum EndpointAttributes
	{
		ControlTranferType			= 0,
		IsochronousTranferType		= 1,
		BulkTranferType				= 2,
		InterruptTranferType		= 3,

		// USB 2
		NoSynchronization			= 0,
		Asynchronous				= 1,
		AdaptiveSynchronization		= 2,
		Synchronous					= 3,
		
		// USB 2
		DataEndpoint				= 0,
		FeedbackEndpoint			= 1,
		ImplicitFeedbackEndpoint	= 2,

		// USB 3
		PeriodicEndpoint			= 0,
		NotificationEndpoint		= 1,
	};

	// USB 2
	struct DeviceQualifierDescriptor
	{
		uint8_t		Length;
		uint8_t		DescriptorType;

		uint16_t	USBVersion;

		uint8_t		DeviceClass;
		uint8_t		DeviceSubClass;
		uint8_t		DeviceProtocol;

		uint8_t		MaxPacketSize0;

		uint8_t		NumConfigurations;

		uint8_t		Reserved;
	};

	// USB 3
	struct BDODescriptor
	{
		uint8_t		Length;
		uint8_t		DescriptorType;

		uint16_t	TotalLength;		
		uint8_t		NumDeviceCaps;
	};

	struct DeviceCapDescriptor
	{
		uint8_t		Length;
		uint8_t		DescriptorType;

		uint8_t		DeviceCapabilityType;		
		uint8_t		Data[1];
	};

	enum DeviceCapabilities
	{
		WirelessUSB = 0x01,
		USB20,
		Superspeed,
		ContainerID,
		Platfrom,
		PowerDevilery,
		BatteryInfo,
		PDConsumerPort,
		PDProviderPort,
		SuperspeedPlus,
		PrecisionTimeMeasurement,
		WirelessUSBExt,
	};

	struct InterfaceAssociationDescriptor
	{
		uint8_t		Length;
		uint8_t		DescriptorType;

		uint8_t		FirstInterface;
		uint8_t		InterfaceCount;

		uint8_t		FunctionClass;
		uint8_t		FunctionSubClass;
		uint8_t		FunctionProtical;

		uint8_t		Function;
	};

	struct SuperspeedEndpointDescriptor
	{
		uint8_t		Length;
		uint8_t		DescriptorType;

		uint8_t		MaxBurst;
		uint8_t		Attributes;
		uint16_t	BytesPerInterval;
	};
}

namespace USBHub
{
	struct HubDescriptor
	{
		uint8_t		Length;
		uint8_t		DescriptorType;

		uint8_t		DownStreamPorts;

		uint16_t	PowerSwitchingMode : 1;
		uint16_t	NoPowerSwitching : 1;
		uint16_t	DeviceType : 1;
		uint16_t	OverCurrentProtectionMode : 1;
		uint16_t	NoOverCurrentProtection : 1;
		uint16_t	TT_ThinkTime : 2;				// USB 2
		uint16_t	PortIndicatorsSupported : 1;	// USB 2

		uint8_t		PowerOnToGood;
		uint8_t		MaxHubCurrent;

		// Both the DeviceRemovable & PortPowerControlMask are variable length,
		uint8_t		DeviceRemovablePowerControlMask[1];
	};

	enum USBHubThinkTime
	{
		ThinkTime8FS	= 0,
		ThinkTime16FS	= 1,
		ThinkTime24FS	= 2,
		ThinkTime32FS	= 3
	};

	enum USBDeviceRequestIDs
	{
		GetState			= 2,		// USB 1 only
		ClearTTBuffer		= 8,		// USB 2 only
		ResetTT				= 9,		// USB 2 only
		GetTTState			= 10,		// USB 2 only
		StopTT				= 11,		// USB 2 only
		SetHubDepth			= 12,		// USB 3
		GetPortErrorCount	= 13,		// USB 3
	};

	enum DescriptorTypes
	{
		Hub					= 0x29,
		SuperspeedHub		= 0x2A,
	};

	enum USBDeviceFeatures
	{
		// Hub Features
		ClearHubLocalPower		= 0,
		ClearHubOverCurrent		= 1,

		// Port Features
		PortConnection			= 0,
		PortEnabled				= 1,	// USB 1 & 2
		PortSuspended			= 2,	// USB 1 & 2
		PortOverCurrent			= 3,
		PortReset				= 4,
		PortLinkState			= 5,	// USB 3
		PortPower				= 8,
		PortLowSpeed			= 9,	// USB 1 & 2
		
		ClearPortConnection		= 16,
		ClearPortEnable			= 17,
		ClearPortSuspended		= 18,
		ClearPortOverCurrent	= 19,
		ClearPortReset			= 20,

		PortTest				= 21,	// USB 2
		PortIndicator			= 22,	// USB 2

		PortU1Timeout			= 23,	// USB 3
		PortU2Timeout			= 24,	// USB 3

		ClearPortLinkState		= 25,	// USB 3
		ClearPortConfigError	= 26,	// USB 3

		PortRemoteWakeMask		= 27,	// USB 3
		BHPortReset				= 28,	// USB 3
		ClearBPPortReset		= 29,	// USB 3
		ForceLinkPMAccept		= 30,	// USB 3
	};

	struct SuperspeedHubDescriptor
	{
		uint8_t		Length;
		uint8_t		DescriptorType;

		uint8_t		DownStreamPorts;

		uint16_t	PowerSwitchingMode : 1;
		uint16_t	NoPowerSwitching : 1;
		uint16_t	DeviceType : 1;
		uint16_t	OverCurrentProtectionMode : 1;
		uint16_t	NoOverCurrentProtection : 1;

		uint8_t		PowerOnToGood;
		uint8_t		MaxHubCurrent;
		
		uint8_t		PacketHeaderDecodeLatency;
		uint16_t	HubDelay;

		// Both the DeviceRemovable is variable length,
		uint8_t		DeviceRemovable[1];
	};


	struct HubStatusAndChange
	{
		union
		{
			uint32_t StatusAndChanged;
			struct
			{
				struct
				{
					uint16_t LocalPowerSource : 1;
					uint16_t OverCurrentIndicator : 1;
				} Status;

				struct 
				{
					uint16_t LocalPowerStatusChange : 1;
					uint16_t OverCurrentIndicatorChange : 1;
				} Changed;
			};
		};
	};

	struct PortStatusAndChange
	{
		union
		{
			uint32_t StatusAndChanged;
			struct
			{
				struct 
				{
					uint16_t Connection : 1;
					uint16_t Enabled : 1;
					uint16_t Suspended : 1;					// USB 1 & 2
					uint16_t OverCurrentIndicator : 1;
					uint16_t Reset : 1;
					uint16_t Reserved : 3;
					uint16_t PortPower : 1;
					uint16_t LowSpeedDeviceAttached : 1;
					uint16_t HighSpeedDeviceAttached : 1;	// USB 2
					uint16_t PortTestMode : 1;				// USB 2
					uint16_t PortIndicatorControl : 1;		// USB 2
				} Status;

				struct
				{
					uint16_t ConnectionChange : 1;
					uint16_t EnabledChange : 1;				// USB 1 & 2
					uint16_t SuspendedChange : 1;			// USB 1 & 2
					uint16_t OverCurrentIndicatorChange : 1;
					uint16_t ResetChange : 1;
					uint16_t DHResetChanged : 1;			// USB 3
					uint16_t PortLinkStatusChanged : 1;		// USB 3
					uint16_t PortConfigError : 1;			// USB 3
				} Changed;
			};
		};
	};

	struct PortExtendedStatusAndChange
	{
		union
		{
			uint32_t StatusAndChanged;
			struct
			{
				struct 
				{
					uint16_t Connection : 1;
					uint16_t Enabled : 1;
					uint16_t Suspended : 1;					// USB 1 & 2
					uint16_t OverCurrentIndicator : 1;
					uint16_t Reset : 1;
					uint16_t PortLinkState : 4;				// USB 3
					uint16_t PortPower : 1;
					uint16_t PortSpeed : 3;					// USB 3
				} Status;

				struct
				{
					uint16_t ConnectionChange : 1;
					uint16_t EnabledChange : 1;				// USB 1 & 2
					uint16_t SuspendedChange : 1;			// USB 1 & 2
					uint16_t OverCurrentIndicatorChange : 1;
					uint16_t ResetChange : 1;
					uint16_t DHResetChanged : 1;			// USB 3
					uint16_t PortLinkStatusChanged : 1;		// USB 3
					uint16_t PortConfigError : 1;			// USB 3
				} Changed;
			};
			
			uint32_t RxSublinkSpeedID : 4;
			uint32_t TxSublinkSpeedID : 4;
			uint32_t RxLaneCount: 4;
			uint32_t TxLaneCount: 4;
		};
	};

}

#pragma pack(pop)
