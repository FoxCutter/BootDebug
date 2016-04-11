#include "AHCI.h"
#include "KernalLib.h"
#include "ObjectManager.h"
#include "Utility.h"

#pragma pack(push, 1)

namespace AHCIData
{
	struct GenericHostControl
	{
		struct
		{
			uint32_t	NumberOfPorts : 5;
			uint32_t	eSATA : 1;
			uint32_t	EnclosureManagement : 1;
			uint32_t	CCC : 1;

			uint32_t	CommandSlots : 5;
			uint32_t	PartialStateCapable : 1;
			uint32_t	SulmberStateCapable : 1;
			uint32_t	PIOMultipleDRQBlock : 1;

			uint32_t	FISBasedSwitching : 1;
			uint32_t	PortMultiplier : 1;
			uint32_t	AHCIOnly: 1;
			uint32_t	Reserved : 1;
			uint32_t	InterfaceSpeed : 4;

			uint32_t	CommandListOverride : 1;
			uint32_t	ActivityLED : 1;
			uint32_t	ALPM : 1;
			uint32_t	StaggeredSpinup : 1;
			uint32_t	MechanicalPresenceSwitch : 1;
			uint32_t	SNotificationRegister : 1;
			uint32_t	NativeCommandQueuing : 1;
			uint32_t	Suports64BitAddressing : 1;
		} HostCapabilites;

		struct
		{
			uint32_t	HBAReset : 1;
			uint32_t	InterruptEnable : 1;
			uint32_t	MSIRevertSingleMessage : 1;
			uint32_t	Reserved : 28;
			uint32_t	AHCIEnable : 1;

		} GlobalHostControl;

		uint32_t	InterruptStatus;
		uint32_t	PortsImplemented;
		uint32_t	Version;
		uint32_t	CCC_Control;
		uint32_t	CCC_Ports;
		uint32_t	EM_Location;
		uint32_t	EM_Control;

		struct
		{
			uint32_t	BIOSHandoff : 1;
			uint32_t	NVMHCI : 1;
			uint32_t	AutomaticPartialToSlumber : 1;
			uint32_t	DeviceSleep : 1;
			uint32_t	ADSM : 1;
			uint32_t	SleepFromSlumberOnly : 1;
		} HostCapabilitesExtended;

		uint32_t	HandoffControl;		
	};

	struct PortRegister
	{
		uint64_t CommandListBaseAddress;
		uint64_t FISBaseAddress;
		uint32_t InterruptStatus;
		uint32_t InterruptEnable;
		struct {
			uint32_t	Start : 1;
			uint32_t	SpinUpDevice : 1;
			uint32_t	PowerOnDevice : 1;
			uint32_t	CommandListOverride : 1;
			uint32_t	FISRecive : 1;
			uint32_t	Reserved1 : 3;

			uint32_t	CurrentCommandSlot : 5;
			uint32_t	MechanicalPresenceSwitchState : 1;
			uint32_t	FISReciveRunning : 1;
			uint32_t	CommandListRunning : 1;

			uint32_t	ColdPresenceState : 1;
			uint32_t	PortMultiplier : 1;
			uint32_t	HotPlugCapable : 1;
			uint32_t	MechanicalPresenceSwitch : 1;
			uint32_t	ColdPresenceDetection : 1;
			uint32_t	ExternalPort : 1;
			uint32_t	FISSwitchingCapable : 1;
			uint32_t	APSTE : 1;

			uint32_t	ATAPI : 1;
			uint32_t	DriveLEDonATAPIEnable: 1;
			uint32_t	ALPME : 1;
			uint32_t	AggressSlumber : 1;
			uint32_t	InterfaceCommunicationControl : 4;
		} CommandAndStatus;

		uint32_t Reserved;
		struct { 
			uint32_t	Status : 8;
			uint32_t	Error : 8;
		} TaskFile;
		uint32_t Signature;

		uint32_t SerialATAStatus;
		uint32_t SerialATAControl;
		uint32_t SerialATAError;
		uint32_t SerialATAActive;
		uint32_t CommandIssue;
		uint32_t SerialATANotification;
		uint32_t FISSwitichingControl;
		uint32_t DeviceSleep;
		uint32_t Reserved2[10];
		uint32_t VenderSpecific[4];
	};

	struct CommandHeader
	{
		uint16_t Flags;
		uint16_t DescriptorTableLength;
		uint32_t ByteCount;
		uint64_t CommandTableAddress;
		uint32_t Reserved[4];
	};

	struct PhysicalRegionDescriptior
	{
		uint64_t BaseAddress;
		uint32_t Reserved;
		uint32_t ByteCount;
	};

	struct CommandTable
	{
		uint8_t CommandFIS[0x40];
		uint8_t CommandATAPI[0x10];
		uint8_t Filler[0x30];

		PhysicalRegionDescriptior PRDT[1];
	};


	struct RecivedFIS
	{
		uint8_t DMASetup[0x20];
		uint8_t PIOSetup[0x20];
		uint8_t D2HRegister[0x18];
		uint8_t SetDeviceBits[0x8];
		uint8_t Unknown[0x40];
		uint8_t Reserved[0x60];
	};

	enum FISType
	{
		FIS_Register_H2D = 0x27,		// 0010 0111
		FIS_Register_D2H = 0x34,		// 0011 0100
		FIS_DMA_Active_D2H = 0x39,		// 0011 1001
		FIS_DMA_Setup_Bi = 0x41,		// 0100 0001
		FIS_Data_Bi = 0x46,				// 0100 0110
		FIS_BIST_Activ_Bi = 0x58,		// 0101 1000
		FIS_PIO_Setup_D2H = 0x5F,		// 0101 1111
		FIS_Set_DeviceBits_D2H = 0xA1,	// 1010 0001
	};
	
	struct FIS_Register
	{
		uint8_t Type;

		uint8_t PMPort : 4;
		uint8_t ResFlags : 2;
		uint8_t Interrupt_D2H : 1;
		uint8_t CommandOrControl_H2D : 1;

		uint8_t CommandStatus;
		uint8_t FeaturesError;
		uint32_t LBA : 24;
		uint32_t Device : 8;
		uint32_t LBAHigh : 24;
		uint32_t FeaturesHigh : 8;
		uint16_t Count;
		uint8_t ICC;
		uint8_t Control;
		uint32_t Reserved;
	};

	struct FIS_SetDeviceBits
	{
		uint8_t Type;

		uint8_t PMPort : 4;
		uint8_t ResFlags : 2;
		uint8_t Interrupt : 1;
		uint8_t Notification : 1;

		uint8_t StatusLow : 3;
		uint8_t Res2 : 1;
		uint8_t StatusHigh : 3;
		uint8_t Res3 : 1;
		uint8_t Error;

		uint32_t ProtocolSpecific;
	};

	struct FIS_DMA_Active
	{
		uint8_t Type;

		uint8_t PMPort : 4;
		uint8_t ResFlags : 3;
	};

	struct FIS_DMA_Setup
	{
		uint8_t Type;

		uint8_t PMPort : 4;
		uint8_t ResFlags : 1;
		uint8_t Direction : 1;
		uint8_t Interrupt : 1;
		uint8_t AutoActivate : 1;

		uint16_t Reserved1;
		uint64_t DMABufferIdentifier;
		uint32_t Reserved2;
		uint32_t DMABufferOffset;
		uint32_t DMATransferCount;
		uint32_t Reserved3;
	};

	struct FIS_BIST
	{
		uint8_t Type;

		uint8_t PMPort : 4;
		uint8_t ResFlags : 4;

		uint8_t Pattern;
		uint8_t Reserved;
		uint32_t Data1;
		uint32_t Data2;
	};

	struct FIS_PIO_Setup
	{
		uint8_t Type;

		uint8_t PMPort : 4;
		uint8_t ResFlags : 1;
		uint8_t Direction : 1;
		uint8_t Interrupt: 1;
		uint8_t ResFlags2 : 1;

		uint8_t Status;
		uint8_t Error;
		
		uint32_t LBA : 24;
		uint32_t Device : 8;
		
		uint32_t LBAHigh : 24;
		uint32_t FeaturesHigh : 8;
		
		uint16_t Count;
		uint8_t Reserved;
		uint8_t E_Status;
		uint16_t TransferCount;
		uint16_t Reserved2;
	};

	struct FIS_Data
	{
		uint8_t Type;

		uint8_t PMPort : 4;
		uint8_t ResFlags : 4;

		uint16_t Reserved;
		
		uint32_t Data[1];
	};

}

#pragma pack(pop)


AHCI::AHCI(void)
{
	m_DeviceID = 0x00000000;
	m_HostControl = nullptr;
	m_Ports = nullptr;

}


AHCI::~AHCI(void)
{
}

void AHCI::Setup(PCI &Bus)
{
	ObjectManager::Current()->AddObject("SATA", 4, this);

	// First off, lets just see if the PCI bus knows about an IDE device.
	m_DeviceID = Bus.FindDeviceID(0x01, 0x06, 0x01);
	if(m_DeviceID == 0xFFFFFFFF)
		return;

	m_HostControl = reinterpret_cast<AHCIData::GenericHostControl *>(Bus.ReadRegister(m_DeviceID, 0x24) & 0xFFFFFFF0);
	m_Ports = reinterpret_cast<AHCIData::PortRegister *>((uint8_t *)m_HostControl + 0x100);

	char name[4] = "HD3";
	char CDname[4] = "CD3";

	for(size_t x = 0; x <= m_HostControl->HostCapabilites.NumberOfPorts; x++)
	{
		name[2] ++;
		CDname[2] ++;
		if((m_HostControl->PortsImplemented & (1 << x)) != 0)
		{
			if((m_Ports[x].SerialATAStatus & 0x07) != 0)
			{
				if(m_Ports[x].Signature == 0xEB140101)
					ObjectManager::Current()->AddObject(CDname, 3, this);
				else 
					ObjectManager::Current()->AddObject(name, 3, this);
			}
		}
	}

}

void AHCI::DisplayObject(char * Command, char *Param)
{
	KernalPrintf(" SATA Device ID %08X\n", m_DeviceID);
	KernalPrintf(" Register Base %08X, Port Control: %08X\n", m_HostControl, m_Ports);

	KernalPrintf(" Ports: %02X, %08X\n", m_HostControl->HostCapabilites.NumberOfPorts + 1, m_HostControl->PortsImplemented);
	KernalPrintf(" Command Slots: %02X\n", m_HostControl->HostCapabilites.CommandSlots + 1);

	for(size_t x = 0; x <= m_HostControl->HostCapabilites.NumberOfPorts; x++)
	{
		KernalPrintf("  Port %02X\n", x);
		KernalPrintf("   Command List: "); 
		PrintLongAddress(m_Ports[x].CommandListBaseAddress);
		KernalPrintf(", FIS: ");
		PrintLongAddress(m_Ports[x].FISBaseAddress);
		KernalPrintf("\n");

		KernalPrintf("   Command & Satus: %08X, Task File: %08X\n", m_Ports[x].CommandAndStatus, m_Ports[x].TaskFile); 
		KernalPrintf("   Signatue: %08X\n", m_Ports[x].Signature);
		KernalPrintf("   ATA Staus: %08X, Control: %08X, Error %08X, Active %08X\n", m_Ports[x].SerialATAStatus, m_Ports[x].SerialATAControl, m_Ports[x].SerialATAError, m_Ports[x].SerialATAActive);
		if(m_HostControl->HostCapabilites.SNotificationRegister)
			KernalPrintf("   ATA Notification %08X\n", m_Ports[x].SerialATANotification);
	}
}

