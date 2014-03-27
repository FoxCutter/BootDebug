#include "PCIBIOS.h"
#include "Utility.h"
#include <stdio.h>
#include <string.h>
#include "LowLevel.h"

#pragma pack(push, 1)

namespace PCIBiosData
{
	const uint8_t BIOS32Sig[] = "_32_";

	struct BIOSServiceDirectory
	{
		uint8_t		Magic[4];
		uint32_t	EntryPoint;
		uint8_t		HeaderVersion;
		uint8_t		HeaderLength; // in Paragraphs
		uint8_t		Checksum;
		uint8_t		Reserved[5];
	};

	const uint32_t PCIBiosServiceID = 0x49435024; // $PCI

	const uint8_t PCI_FUNCTION_ID = 0xB1;
	const uint8_t PCI_BIOS_PRESENT = 0x01;
	const uint8_t FIND_PCI_DEVICE = 0x02;
	const uint8_t FIND_PCI_CLASS_CODE = 0x03;
	const uint8_t GENERATE_SPECIAL_CYCLE = 0x06;
	const uint8_t READ_CONFIG_BYTE = 0x08;
	const uint8_t READ_CONFIG_WORD = 0x09;
	const uint8_t READ_CONFIG_DWORD = 0x0A;
	const uint8_t WRITE_CONFIG_BYTE = 0x0B;
	const uint8_t WRITE_CONFIG_WORD = 0x0C;
	const uint8_t WRITE_CONFIG_DWORD = 0x0D;
	const uint8_t GET_IRC_ROUTING_OPTIONS = 0x0E;
	const uint8_t SET_PCI_IRQ = 0x0F;

};

#pragma pack(pop)

PCIBIOS::PCIBIOS(void)
{
}


PCIBIOS::~PCIBIOS(void)
{
}

bool PCIBIOS::Initilize()
{
	uint32_t BIOS32Address = SeachMemory(0xE0000, 0x20000, PCIBiosData::BIOS32Sig, 0x10);

	if(BIOS32Address == UINT32_MAX)
		return false;

	PCIBiosData::BIOSServiceDirectory * Bios32 = reinterpret_cast<PCIBiosData::BIOSServiceDirectory *>(BIOS32Address);

	if(!ValidateChecksum(Bios32, Bios32->HeaderLength * 0x10))
		return false;

	// Ask for the PCI bios
	Registers Reg;
	Reg.EAX = PCIBiosData::PCIBiosServiceID;
	Reg.EBX = 0;
	Reg.ECX = 0;
	Reg.EDX = 0;
	printf("A:%08X B:%08X C:%08X D:%08X\n", Reg.EAX, Reg.EBX, Reg.ECX, Reg.EDX);

	CallService(Bios32->EntryPoint, &Reg);
	
	printf("A:%02X B:%08X C:%08X D:%08X, F:%08X\n", Reg.AL, Reg.EBX, Reg.ECX, Reg.EDX, Reg.EFlags);

	if(Reg.AL != 0)
		return false;

	uint32_t BiosEntry = Reg.EBX + Reg.EDX;
	Reg.AH = 0xB1;
	Reg.AL = 1;
	Reg.EBX = 0;
	Reg.ECX = 0;
	Reg.EDX = 0;

	CallService(BiosEntry, &Reg);
	printf("A:%08X B:%08X C:%08X D:%08X, F:%08X\n", Reg.EAX, Reg.EBX, Reg.ECX, Reg.EDX, Reg.EFlags);

	return false;
}
