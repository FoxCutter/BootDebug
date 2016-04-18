#include "PCI.h"
#include <stdio.h>
#include <string.h>
#include "LowLevel.h"
#include "Utility.h"
#include "ObjectManager.h"
#include "KernalLib.h"
#include "HardwareTree.h"
#include "CoreComplex.h"

#include <algorithm>

struct PCINameEntry
{
	uint32_t Code;
	char * Name;
};

PCINameEntry DeviceNameList[] = 
{
	// Class 0
	{0x000100, "VGA-Compatible Device"},

	// Class 1
	{0x010000, "SCSI Bus Controller"},
	{0x010100, "IDE Controller"},
	{0x010200, "Floppy Disk Controller"},
	{0x010300, "IPI Bus Controller"},
	{0x010400, "RAID Controller"},
	{0x010500, "ATA Controller"},
	{0x010600, "Serial ATA"},
	{0x010700, "Serial Attached SCSI"},
	{0x018000, "Other Mass Storage"},

	// Class 2
	{0x020000, "Ethernet Controller"},
	{0x020100, "Token Ring Controller"},
	{0x020200, "FDDI Controller"},
	{0x020300, "ATM Controller"},
	{0x020400, "ISDN Controller"},
	{0x020500, "WorldFip Controller"},
	{0x020600, "PICMG 2.14 Multi Computing"},
	{0x028000, "Other Network Controller"},

	// Class 3
	{0x030000, "VGA-Compatible Controller"},
	{0x030001, "8512-Compatible Controller"},
	{0x030100, "XGA Controller"},
	{0x030200, "3D Controller (Not VGA-Compatible)"},
	{0x038000, "Other Display Controller "},

	// Class 4
	{0x040000, "Video Device"},
	{0x040100, "Audio Device"},
	{0x040200, "Computer Telephony Device"},
	{0x040400, "High Definition Audio Device"},
	{0x048000, "Other Multimedia Controller"},

	// Class 5
	{0x050000, "RAM Controller"},
	{0x050100, "Flash Controller"},
	{0x058000, "Other Memeory Controller"},

	// Class 6
	{0x060000, "Host Bridge"},
	{0x060100, "ISA Bridge"},
	{0x060200, "EISA Bridge"},
	{0x060300, "MCA Bridge"},
	{0x060400, "PCI-to-PCI Bridge"},
	{0x060500, "PCMCIA Bridge"},
	{0x060600, "NuBus Bridge"},
	{0x060700, "CardBus Bridge"},
	{0x060800, "RACEway Bridge"},
	{0x060940, "PCI-to-PCI Bridge"},
	{0x060A00, "InfiniBrand-to-PCI Host Bridge "},	
	{0x068000, "Other Bridge Device"},

	// Class 7
	{0x070000, "Generic XT-Compatible Serial Controller"},
	{0x070100, "Parallel Port"},
	{0x070101, "Bi-Directional Parallel Port"},
	{0x070102, "ECP 1.X Compliant Parallel Port"},
	{0x070103, "IEEE 1284 Controller"},
	{0x0701FE, "IEEE 1284 Target Device"},
	{0x070200, "Multiport Serial Controller"},
	{0x070300, "Generic Modem"},
	{0x070400, "IEEE 488.1/2 (GPIB) Controller"},
	{0x070500, "Smart Card"},
	{0x078000, "Other Communications Device"},

	// Class 8
	{0x080000, "Generic 8259 PIC"},
	{0x080001, "ISA PIC"},
	{0x080002, "EISA PIC"},
	{0x080010, "I/O APIC Interrupt Controller"},
	{0x080020, "I/O(x) APIC Interrupt Controller"},
	{0x080100, "Generic 8237 DMA Controller"},
	{0x080101, "ISA DMA Controller"},
	{0x080102, "EISA DMA Controller"},
	{0x080200, "Generic 8254 System Timer"},
	{0x080201, "ISA System Timer"},
	{0x080202, "EISA System Timer"},
	{0x080300, "Generic RTC Controller"},
	{0x080301, "ISA RTC Controller"},
	{0x080400, "Generic PCI Hot-Plug Controller"},
	{0x088000, "Other System Peripheral"},
	
	// Class 9
	{0x090000, "Keyboard Controller"},
	{0x090100, "Digitizer"},
	{0x090200, "Mouse Controller"},
	{0x090300, "Scanner Controller"},
	{0x090400, "Gameport Controller"},
	{0x098000, "Other Input Controller"},

	// Class A
	{0x0A0000, "Generic Docking Station"},
	{0x0A8000, "Other Docking Station"},

	// Class B
	{0x0B0000, "386 Processor"},
	{0x0B0100, "486 Processor"},
	{0x0B0200, "Pentium Processor"},
	{0x0B1000, "Alpha Processor"},
	{0x0B2000, "PowerPC Processor"},
	{0x0B3000, "MIPS Processor"},
	{0x0B4000, "Co-Processor"},
	
	// Class C
	{0x0C0000, "IEEE 1394 Controller (FireWire)"},
	{0x0C0010, "IEEE 1394 Controller (OpenHCI)"},
	{0x0C0100, "ACCESS.bus"},
	{0x0C0200, "SSA"},
	{0x0C0300, "USB1 UHCI Controller"},
	{0x0C0310, "USB1 OHCI Controller"},
	{0x0C0320, "USB2 EHCI Controller"},
	{0x0C0330, "USB3 XHCI Controller"},
	{0x0C0380, "USB Controller"},
	{0x0C03FE, "USB (Not Host Controller)"},
	{0x0C0400, "Fibre Channel"},
	{0x0C0500, "SMBus"},
	{0x0C0600, "InfiniBand"},
	{0x0C0700, "IPMI SMIC Interface"},
	{0x0C0800, "SERCOS Interface Standard (IEC 61491)"},
	{0x0C0900, "CANbus"},
	
	// Class D
	{0x0D0000, "iRDA Compatible Controller"},
	{0x0D0100, "Consumer IR Controller"},
	{0x0D1000, "RF Controller"},
	{0x0D1100, "Bluetooth Controller"},
	{0x0D1200, "Broadband Controller"},
	{0x0D2000, "Ethernet Controller (802.11a)"},
	{0x0D2100, "Ethernet Controller (802.11b)"},
	{0x0D8000, "Other Wireless Controller"},
	
	// Class E
	{0x0E0000, "Intelligent I/O Controllers"},
	
	// Class F
	{0x0F0100, "TV Controller"},
	{0x0F0200, "Audio Controller"},
	{0x0F0300, "Voice Controller"},
	{0x0F0400, "Data Controller"},
	
	// Class 10
	{0x100000, "Network and Computing Encrpytion/Decryption"},
	{0x101000, "Entertainment Encryption/Decryption"},
	{0x108000, "Other Encryption/Decryption"},
	
	// Class 11
	{0x110000, "DPIO Modules"},
	{0x110100, "Performance Counters"},
	{0x111000, "Communications Syncrhonization Plus Time and Frequency Test/Measurment"},
	{0x112000, "Management Card"},
	{0x118000, "Other Data Acquisition/Signal Processing Controller"},
};



PCINameEntry * FindPCIClassName(uint32_t ClassCode)
{
	// Start with hte full classes code
	PCINameEntry * Entry = std::find_if(std::begin(DeviceNameList), std::end(DeviceNameList), [ClassCode](PCINameEntry &Entry){return Entry.Code == ClassCode;});
	if(Entry != std::end(DeviceNameList))
		return Entry;

	// Remove the interface ID from the class code to get a more generic answer.
	ClassCode = ClassCode & 0xFFFF00;
	Entry = std::find_if(std::begin(DeviceNameList), std::end(DeviceNameList), [ClassCode](PCINameEntry &Entry){return Entry.Code == ClassCode;});

	if(Entry != std::end(DeviceNameList))
		return Entry;

	return nullptr;
}

PCI::PCI(void)
{
}


PCI::~PCI(void)
{
}

bool PCI::Initilize(HardwareTree * PCIRoot)
{
	ObjectManager::Current()->AddObject("PCI", 3, this);
	
	for(int x = 0; x < 256; x++)
		EnumerateBus(x, PCIRoot);

	return true;
}

bool PCI::EnumerateBus(uint8_t Bus, HardwareTree * Root)
{
	uint8_t CurrentDevice = 0;
	uint8_t CurrentFunction = 0;

	uint32_t CurrentDeviceID = 0;

	bool AllFunctions = false;
	//KernalPrintf("Bus %u\n", Bus);
	while(true)
	{
		if(CurrentFunction == 0x08)
		{
			AllFunctions = false;
			CurrentDevice ++;
			CurrentFunction = 0;
		}
		
		if(CurrentDevice == 0x20)
			break;

		CurrentDeviceID = BuildDeviceID(Bus, CurrentDevice, CurrentFunction);
		uint32_t Val = ReadRegister(CurrentDeviceID, 0);
		if(Val == 0xFFFFFFFF)
		{
			if(AllFunctions)
			{
				CurrentFunction++;
			}
			else
			{
				CurrentDevice ++;
				CurrentFunction = 0;
			}
		}
		else
		{
			char ID[16];
			char Name[32];
			uint16_t VenderID = Val & 0xFFFF; 
			uint16_t DeviceID = (Val & 0xFFFF0000) >> 16;

			
			KernalSprintf(ID, 16, "PCI_%04X:%04X", VenderID, DeviceID);

			Val = ReadRegister(CurrentDeviceID, 0x08);
			KernalSprintf(Name, 32, "%06X_%06X", CurrentDeviceID, (Val & 0xFFFFFF00) >> 8);

			CoreComplexObj::GetComplex()->HardwareComplex.Add(ID, Name, Root);
			
			Val = ReadRegister(CurrentDeviceID, 0x0C);			
			Val = (Val & 0x00FF0000) >> 16;

			if(Val & 0x80 || AllFunctions == true)
			{
				CurrentFunction ++;
				AllFunctions = true;
			}
			else
			{
				CurrentDevice ++;
				CurrentFunction = 0;
			}
		}
	} 

	return false;
}

bool PCI::DumpBus(uint8_t Bus)
{
	uint8_t CurrentDevice = 0;
	uint8_t CurrentFunction = 0;

	uint32_t CurrentDeviceID = 0;

	bool AllFunctions = false;

	while(true)
	{
		if(CurrentFunction == 0x08)
		{
			AllFunctions = false;
			CurrentDevice ++;
			CurrentFunction = 0;
		}
		
		if(CurrentDevice == 0x20)
			break;

		CurrentDeviceID = BuildDeviceID(Bus, CurrentDevice, CurrentFunction);
		uint32_t Val = ReadRegister(CurrentDeviceID, 0);
		if(Val == 0xFFFFFFFF)
		{
			if(AllFunctions)
			{
				CurrentFunction++;
			}
			else
			{
				CurrentDevice ++;
				CurrentFunction = 0;
			}
		}
		else
		{
			KernalPrintf(" %06X (%02X:%02X:%02X)", CurrentDeviceID, Bus, CurrentDevice, CurrentFunction);
			KernalPrintf(" Dev %04X:%04X", Val & 0xFFFF, (Val & 0xFFFF0000) >> 16);
			
			Val = ReadRegister(CurrentDeviceID, 0x08);
			KernalPrintf(", C %02X-%02X-%02X", (Val & 0xFF000000) >> 24, (Val & 0x00FF0000) >> 16, (Val & 0x0000FF00) >> 8);
			
			//Val = ReadRegister(CurrentDeviceID, 0x2C);
			//KernalPrintf(", Sub %04X:%04X", Val & 0xFFFF, (Val & 0xFFFF0000) >> 16);

			// Get the class code
			Val = ReadRegister(CurrentDeviceID, 0x08) >> 8;
			PCINameEntry * Entry = FindPCIClassName(Val);

			if(Entry != nullptr)
				KernalPrintf(" %.25s", Entry->Name);


			Val = ReadRegister(CurrentDeviceID, 0x0C);			
			Val = (Val & 0x00FF0000) >> 16;

			if(Val & 0x80 || AllFunctions == true)
			{
				CurrentFunction ++;
				AllFunctions = true;
			}
			else
			{
				CurrentDevice ++;
				CurrentFunction = 0;
			}

			if((Val & 0x7F) == 0x01)
			{
				Val = ReadRegister(CurrentDeviceID, 0x18);
				uint8_t SecondaryBus = (Val & 0x0000FF00) >> 8;
				uint8_t SubBus =       (Val & 0x00FF0000) >> 16;

				KernalPrintf(", SB %02X", SecondaryBus);
				if(SecondaryBus != SubBus)
					KernalPrintf("-%02X", SubBus);
			}

			KernalPrintf("\n");
		}
	} 

	return false;
}

void PrintPCIAddress(uint32_t Address)
{
	if(Address == 0)
	{
		KernalPrintf("Unused");
	}
	else if(Address & 0x00000001)
	{
		KernalPrintf("I/O %04X", Address & 0xFFFFFFFC);
	}
	else
	{
		KernalPrintf("Memory %08X", Address & 0xFFFFFFF0);

		if(Address & 0x00000008)
			KernalPrintf(", Prefetch");
	}
}


bool PCI::DumpDevice(uint32_t DeviceID)
{
	DeviceID = DeviceID & 0xFFFFFF00;

	uint32_t Val = ReadRegister(DeviceID, 0);

	if(Val == 0xFFFFFFFF)
	{
		KernalPrintf("PCI Device %08X is not present\n", DeviceID);
		return false;
	}

	bool CapabilitiesList = false;

	KernalPrintf("PCI Device ID: %08X\n", DeviceID);
	KernalPrintf("  Vender ID %04X, Device ID %04X\n", Val & 0xFFFF, (Val & 0xFFFF0000) >> 16);

	Val = ReadRegister(DeviceID, 4);
	KernalPrintf("  Status %04X, Command %04X\n", (Val & 0xFFFF0000) >> 16, Val & 0xFFFF);
	
	if(Val & 0x00100000)
		CapabilitiesList = true;

	Val = ReadRegister(DeviceID, 8);
	KernalPrintf("  Class %02X, Sub Class %02X, Prog IF %02X, Revision ID %02X\n", (Val & 0xFF000000) >> 24, (Val & 0x00FF0000) >> 16, (Val & 0x0000FF00) >> 8, Val & 0x000000FF);
	// Get the class code
	Val = Val >> 8;
	PCINameEntry * Entry = FindPCIClassName(Val);

	if(Entry != nullptr)
		KernalPrintf("    %s\n", Entry->Name);
	
	Val = ReadRegister(DeviceID, 0x0C);
	KernalPrintf("  BIST %02X, Header Type %02X, Latency Timer %02X, Cache Line Size %02X\n", (Val & 0xFF000000) >> 24, (Val & 0x00FF0000) >> 16, (Val & 0x0000FF00) >> 8, Val & 0x000000FF);

	Val = (Val & 0x00FF0000) >> 16;

	if((Val & 0x7F) == 0x00)
	{	
		Val = ReadRegister(DeviceID, 0x10);
		KernalPrintf("  BAR0: ");
		PrintPCIAddress(Val);
		KernalPrintf("\n");

		Val = ReadRegister(DeviceID, 0x14);
		KernalPrintf("  BAR1: ");
		PrintPCIAddress(Val);
		KernalPrintf("\n");

		Val = ReadRegister(DeviceID, 0x18);
		KernalPrintf("  BAR2: ");
		PrintPCIAddress(Val);
		KernalPrintf("\n");

		Val = ReadRegister(DeviceID, 0x1C);
		KernalPrintf("  BAR3: ");
		PrintPCIAddress(Val);
		KernalPrintf("\n");

		Val = ReadRegister(DeviceID, 0x20);
		KernalPrintf("  BAR4: ");
		PrintPCIAddress(Val);
		KernalPrintf("\n");

		Val = ReadRegister(DeviceID, 0x24);
		KernalPrintf("  BAR5: ");
		PrintPCIAddress(Val);
		KernalPrintf("\n");

		Val = ReadRegister(DeviceID, 0x28);
		KernalPrintf("  Cardbus Pointer %08X\n", Val);

		Val = ReadRegister(DeviceID, 0x2C);
		KernalPrintf("  Subsystem ID %04X, Subsystem Vender ID %04X\n", (Val & 0xFFFF0000) >> 16, Val & 0xFFFF);

		Val = ReadRegister(DeviceID, 0x30);
		KernalPrintf("  Expansion Rom Base %08X\n", Val);

		if(CapabilitiesList)
		{
			Val = ReadRegister(DeviceID, 0x34);
			uint8_t CapRegister = Val & 0x000000FF;
			KernalPrintf("  Capabilities Pointer %02X\n", CapRegister);

			while(CapRegister != 00)
			{
				Val = ReadRegister(DeviceID, CapRegister);
				KernalPrintf("    ID: %02X, Register: %02X\n",  Val & 0x000000FF, CapRegister);
			
				CapRegister = (Val & 0x0000FF00) >> 8;
			};		
		}

		Val = ReadRegister(DeviceID, 0x3C);
		KernalPrintf("  Max latency %02X, Min Grant %02X, Interrupt PIN %02X, Interrupt Line %02X\n", (Val & 0xFF000000) >> 24, (Val & 0x00FF0000) >> 16, (Val & 0x0000FF00) >> 8, Val & 0x000000FF);
	}
	else
	{
		Val = ReadRegister(DeviceID, 0x10);
		KernalPrintf("  BAR0: ");
		PrintPCIAddress(Val);
		KernalPrintf("\n");

		Val = ReadRegister(DeviceID, 0x14);
		KernalPrintf("  BAR1: ");
		PrintPCIAddress(Val);
		KernalPrintf("\n");

		Val = ReadRegister(DeviceID, 0x18);
		KernalPrintf("  Sec Latency Time %02X, Sub Bus %02X, Secondary Bus %02X, Primary Bus %02X\n", (Val & 0xFF000000) >> 24, (Val & 0x00FF0000) >> 16, (Val & 0x0000FF00) >> 8, Val & 0x000000FF);

		uint8_t IOBase, IOLimit;
		
		Val = ReadRegister(DeviceID, 0x1C);
		IOLimit = (Val & 0x0000FF00) >> 8;
		IOBase = Val & 0x000000FF;
		
		//KernalPrintf("  Secondary Status %04X, I/O Limit %02X, I/O Base %02X\n", (Val & 0xFFFF0000) >> 16, (Val & 0x0000FF00) >> 8, Val & 0x000000FF);
		KernalPrintf("  Secondary Status %04X\n", (Val & 0xFFFF0000) >> 16);

		Val = ReadRegister(DeviceID, 0x20);
		KernalPrintf("  Memory Limit %04X, Memory Base %04X\n", (Val & 0xFFFF0000) >> 16, Val & 0xFFFF);

		uint16_t Base, Limit;
		
		Val = ReadRegister(DeviceID, 0x24);
		Limit = (Val & 0xFFFF0000) >> 16;
		Base = Val & 0xFFFF;

		Val = ReadRegister(DeviceID, 0x28);
		KernalPrintf("  Prefetchable Memory Base %08X:%04X, ", Val, Base);

		Val = ReadRegister(DeviceID, 0x2C);
		KernalPrintf("Limit %08X:%04X\n", Val, Limit);

		Val = ReadRegister(DeviceID, 0x30);
		KernalPrintf("  I/O Base %04X:%02X, I/O Limit %04X:%02X\n", Val & 0xFFFF, IOBase, (Val & 0xFFFF0000) >> 16, IOLimit);

		if(CapabilitiesList)
		{
			Val = ReadRegister(DeviceID, 0x34);
			uint8_t CapRegister = Val & 0x000000FF;
			KernalPrintf("  Capabilities Pointer %02X\n", CapRegister);

			while(CapRegister != 00)
			{
				Val = ReadRegister(DeviceID, CapRegister);
				KernalPrintf("    ID: %02X, Register: %02X\n",  Val & 0x000000FF, CapRegister);
			
				CapRegister = (Val & 0x0000FF00) >> 8;
			};
		}

		Val = ReadRegister(DeviceID, 0x38);
		KernalPrintf("  Expansion Rom Base %08X\n", Val);

		Val = ReadRegister(DeviceID, 0x3C);
		KernalPrintf("  Bridge Control %04X, Interrupt PIN %02X, Interrupt Line %02X\n", (Val & 0xFFFF0000) >> 16, (Val & 0x0000FF00) >> 8, Val & 0x000000FF);
	}
	return true;
}

bool PCI::DumpDeviceMemory(uint32_t DeviceID)
{
	DeviceID = DeviceID & 0xFFFFFF00;

	
	union 
	{
		uint32_t Val;
		unsigned char Data[4];
	};

	Val = ReadRegister(DeviceID, 0);

	char Buffer[17];
	Buffer[16] = 0;

	if(Val == 0xFFFFFFFF)
	{
		KernalPrintf("PCI Device %08X is not present\n", DeviceID);
		return false;
	}

	int Pos = 0;
	for(int x = 0; x < 0x100; x += 4, Pos += 4)
	{
		if(x % 0x10 == 0)
		{
			if(x != 0)
			{			
				KernalPrintf("   %s\n", Buffer);
			}

			KernalPrintf("      %02X:", x);
			Pos = 0;
		}
		
		if(x % 0x10 != 0 && x % 0x08 == 0)
			KernalPrintf("-");
		else
			KernalPrintf(" ");

		Val = ReadRegister(DeviceID, x);

		KernalPrintf("%02X %02X %02X %02X", Data[0], Data[1], Data[2], Data[3]);
		if(Data[0] < ' ' || Data[0] > 127)
			Buffer[Pos + 0] = '.';
		else
			Buffer[Pos + 0] = Data[0];

		if(Data[1] < ' ' || Data[1] > 127)
			Buffer[Pos + 1] = '.';
		else
			Buffer[Pos + 1] = Data[1];

		if(Data[2] < ' ' || Data[2] > 127)
			Buffer[Pos + 2] = '.';
		else
			Buffer[Pos + 2] = Data[2];

		if(Data[3] < ' ' || Data[3] > 127)
			Buffer[Pos + 3] = '.';
		else
			Buffer[Pos + 3] = Data[3];
	}

	KernalPrintf("   %s\n", Buffer);

	return true;
}

uint32_t PCI::ReadRegister(uint32_t DeviceID, uint8_t Register)
{
	OutPortd(ConfigPort, BuildRegisterID(DeviceID, Register));
	return InPortd(DataPort);
}

uint32_t PCI::ReadRegister(uint32_t DeviceID, uint8_t Register, uint8_t Size)
{	
	// Work out which register to read, the offset and the mask
	uint32_t RegisterID = BuildRegisterID(DeviceID, Register);
	uint32_t Shift = 0;
	uint32_t Mask = 0;

	// For the moment we are going to assume that we have proper alignment for our values.
	switch(Size)
	{
		case 1:
			Mask = 0xFF;
			Shift = (Register & 0x03) * 8;
			break;

		case 2:
			Mask = 0xFFFF;
			Shift = (Register & 0x02) * 8;

			if(Register & 0x01)
				KernalPanic(KernalCode::GeneralError, "Unaligned PCI access");
			break;

		case 4:
			Mask = 0xFFFFFFFF;
			Shift = 0;

			if(Register & 0x03)
				KernalPanic(KernalCode::GeneralError, "Unaligned PCI access");
			break;
	}
	
	OutPortd(ConfigPort, RegisterID);
	uint32_t Value = InPortd(DataPort);

	return (Value >> Shift) & Mask;
}

void PCI::SetRegister(uint32_t DeviceID, uint8_t Register, uint32_t Value)
{
	OutPortd(ConfigPort, BuildRegisterID(DeviceID, Register));
	OutPortd(DataPort, Value);
}

void PCI::SetRegister(uint32_t DeviceID, uint8_t Register, uint32_t Value, uint8_t Size)
{
	// Work out which register to read, the offset and the mask
	uint32_t RegisterID = BuildRegisterID(DeviceID, Register);
	uint32_t Shift = 0;
	uint32_t Mask = 0;

	// For the moment we are going to assume that we have proper alignment for our values.
	switch(Size)
	{
		case 1:
			Mask = 0xFF;
			Shift = (Register & 0x03) * 8;
			break;

		case 2:
			Mask = 0xFFFF;
			Shift = (Register & 0x02) * 8;

			if(Register & 0x01)
				KernalPanic(KernalCode::GeneralError, "Unaligned PCI access");
			break;

		case 4:
			Mask = 0xFFFFFFFF;
			Shift = 0;

			if(Register & 0x03)
				KernalPanic(KernalCode::GeneralError, "Unaligned PCI access");
			break;
	}
	
	OutPortd(ConfigPort, RegisterID);
	uint32_t Data = InPortd(DataPort);

	// Mask off the values we want to keep
	Data = Data & ~(Mask << Shift);

	// Set the new value
	Data = Data | ((Value & Mask) << Shift);

	OutPortd(DataPort, Data);
}

uint32_t PCI::FindDeviceID(uint8_t Class, uint8_t Subclass, uint8_t ProgID)
{
	uint8_t CurrentBus = 0;
	uint8_t CurrentDevice = 0;
	uint8_t CurrentFunction = 0;

	uint32_t CurrentDeviceID = 0;

	bool AllFunctions = false;
	while(true)
	{
		if(CurrentFunction == 0x08)
		{
			AllFunctions = false;
			CurrentDevice ++;
			CurrentFunction = 0;
		}
		
		if(CurrentDevice == 0x20)
		{
			if(CurrentBus == 0xFF)
				break;

			CurrentBus++;
			CurrentDevice = 0;
			AllFunctions = false;
			CurrentFunction = 0;
		}

		CurrentDeviceID = BuildDeviceID(CurrentBus, CurrentDevice, CurrentFunction);
		uint32_t Val = ReadRegister(CurrentDeviceID, 0);
		if(Val == 0xFFFFFFFF)
		{
			if(AllFunctions)
			{
				CurrentFunction++;
			}
			else
			{
				CurrentDevice ++;
				CurrentFunction = 0;
			}
		}
		else
		{
			Val = ReadRegister(CurrentDeviceID, 0x08);
			if((Val & 0xFF000000) >> 24 == Class && (Val & 0x00FF0000) >> 16 == Subclass)
			{
				// Ignore the Prog ID if it's 0.
				if(ProgID == 0)
					return CurrentDeviceID;
				
				else if((Val & 0x0000FF00) >> 8 == ProgID)
					return CurrentDeviceID;
			}

			Val = ReadRegister(CurrentDeviceID, 0x0C);			
			Val = (Val & 0x00FF0000) >> 16;

			if(Val & 0x80 || AllFunctions == true)
			{
				CurrentFunction ++;
				AllFunctions = true;
			}
			else
			{
				CurrentDevice ++;
				CurrentFunction = 0;
			}
		}
	} 

	return 0xFFFFFFFF;
}


uint32_t PCI::BuildDeviceID(uint8_t Bus, uint8_t Device, uint8_t Function)
{
	uint32_t Ret = 0;
	
	Ret |= Bus << 16;
	Ret |= (Device  & 0x1F) << 11;
	Ret |= (Function & 0x07) << 8;
	return Ret;
}

uint32_t PCI::BuildRegisterID(uint32_t DeviceID, uint8_t Register)
{
	uint32_t Ret = DeviceID;

	Ret |= 0x80000000;	// Enable Flag
	Ret |= (Register & 0xFC);	// Bottom two bytes are always 0
	
	return Ret;
}

uint32_t PCI::BuildRegisterID(uint8_t Bus, uint8_t Device, uint8_t Function, uint8_t Register)
{
	return BuildRegisterID(BuildDeviceID(Bus, Device, Function), Register);
}

void PCI::DisplayObject(char * Command, char *Param)
{
	char *Input = Param;
	char *CurrentData = NextToken(Input);

	if(CurrentData == nullptr)
	{
		for(int x = 0; x < 256; x++)
			DumpBus(x);
		
		return;
	}

	uint32_t DeviceID = 0;
	if(!ParseHex(CurrentData, DeviceID))
	{
		KernalPrintf(" Invalid Device [%s]\n", CurrentData);
		return;
	}

	CurrentData = NextToken(Input);

	if(CurrentData == nullptr)
	{
		DumpDevice(DeviceID);
		return;
	}
						
	if(CurrentData[0] == '!')
	{
		DumpDeviceMemory(DeviceID);
		return;
	}
	
	uint32_t Register = 0;
	if(!ParseHex(CurrentData, Register))
	{
		KernalPrintf(" Invalid Register [%s]\n", CurrentData);
		return;
	}

	CurrentData = NextToken(Input);

	if(CurrentData == nullptr)
	{
		uint32_t Value = ReadRegister(DeviceID, Register);
		KernalPrintf("%02X: %08X\n", Register & 0xFC, Value);
		return;
	}

	uint32_t Value = 0;
	if(!ParseHex(CurrentData, Value))
	{
		KernalPrintf(" Invalid Value [%s]\n", CurrentData);
		return;
	}

	SetRegister(DeviceID, Register, Value);
}