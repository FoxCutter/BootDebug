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
		OtherSpeedConfiguration = 7		// USB 2
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

		uint8_t		EndpointAddress : 3;
		uint8_t		Reserved : 4;
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
		ClearTTBuffer		= 8,		// USB 2
		ResetTT				= 9,		// USB 2
		GetTTState			= 10,		// USB 2
		StopTT				= 11,		// USB 2
	};

	enum DescriptorTypes
	{
		Hub					= 0x29
	};

	enum USBDeviceFeatures
	{
		// Hub Features
		ClearHubLocalPower		= 0,
		ClearHubOverCurrent		= 1,

		// Port Features
		PortConnection			= 0,
		PortEnabled				= 1,
		PortSuspended			= 2,
		PortOverCurrent			= 3,
		PortReset				= 4,
		PortPower				= 8,
		PortLowSpeed			= 9,
		
		ClearPortConnection		= 16,
		ClearPortEnable			= 17,
		ClearPortSuspended		= 18,
		ClearPortOverCurrent	= 19,
		ClearPortReset			= 20,
		PortTest				= 21,	// USB 2
		PortIndicator			= 22	// USB 2
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
					uint16_t Suspended : 1;
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
					uint16_t EnabledChange : 1;
					uint16_t SuspendedChange : 1;
					uint16_t OverCurrentIndicatorChange : 1;
					uint16_t ResetChange : 1;
				} Changed;
			};
		};
	};
}

#pragma pack(pop)
