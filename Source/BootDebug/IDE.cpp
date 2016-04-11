#include "IDE.h"
#include "ObjectManager.h"
#include "KernalLib.h"
#include "LowLevel.h"
#include "Utility.h"
#include <string.h>

IDE::IDE(void)
{
	m_DeviceID = 0xFFFFFFFF;
	m_PCIBusMaster = false;

	m_Primary.Base = 0x1F0;
	m_Primary.Status = 0x3F6;
	m_Primary.BusMaster = 0;
	m_Primary.LastDrive = 0;
	m_Primary.Device0Type = NoDrive;
	m_Primary.Device1Type = NoDrive;

	m_Secondary.Base = 0x170;
	m_Secondary.Status = 0x376;
	m_Secondary.BusMaster = 0;
	m_Secondary.LastDrive = 0;
	m_Secondary.Device0Type = NoDrive;
	m_Secondary.Device1Type = NoDrive;
}


IDE::~IDE(void)
{
}

void IDE::DisplayObject(char * Command, char *Param)
{
	if(_strnicmp("IDE", Command, 3) == 0)
	{
		KernalPrintf(" PCI Device ID %08X\n", m_DeviceID);

		KernalPrintf(" Primary\n");
		KernalPrintf("  Base: %04X, Status: %04X\n", m_Primary.Base, m_Primary.Status);
		if(m_PCIBusMaster)
			KernalPrintf("  Busmaster: %04X\n", m_Primary.BusMaster);
		else
			KernalPrintf(" No Bus Master Present\n");

		KernalPrintf(" Secondary\n");
		KernalPrintf("  Base: %04X, Status: %04X\n", m_Secondary.Base, m_Secondary.Status);
		if(m_PCIBusMaster)
			KernalPrintf("  Busmaster: %04X\n", m_Secondary.BusMaster);
		else
			KernalPrintf(" No Bus Master Present\n");
	}
	else if(_strnicmp("HD", Command, 2) == 0 || _strnicmp("CD", Command, 2) == 0)
	{
		int DriveNum = Command[2] - '0'; 

		if(DriveNum < 0 || DriveNum > 4)
		{
			KernalPrintf(" Unknown Hard disk %s\n", Command);
			return;
		}
		
		if(m_DevID[DriveNum][0] & 0x80)
			KernalPrintf(" ATAPI Device");
		else 
			KernalPrintf(" ATA Device");

		//if(m_DevID[DriveNum][0] & 0x40)
		//	KernalPrintf(" - Removable Media");
		KernalPrintf("\n");
		
		KernalPrintf("  Data: %08X\n", &m_DevID[DriveNum]);

		KernalPrintf("  Model Number: ");
		for(int x = 27; x < 47; x++)
		{
			KernalPrintf("%c%c", m_DevID[DriveNum][x] >> 8, m_DevID[DriveNum][x]);
		}
		KernalPrintf("\n");

		KernalPrintf("  Serial Number: ");
		for(int x = 10; x < 20; x++)
		{
			KernalPrintf("%c%c", m_DevID[DriveNum][x] >> 8, m_DevID[DriveNum][x]);
		}
		KernalPrintf("\n");

		KernalPrintf("  Firmware Revision: ");
		for(int x = 23; x < 28; x++)
		{
			KernalPrintf("%c%c", m_DevID[DriveNum][x] >> 8, m_DevID[DriveNum][x]);
		}

		KernalPrintf("\n");
		if(!(m_DevID[DriveNum][0] & 0x80))
		{
			uint64_t Sectors = m_DevID[DriveNum][61] << 16 | m_DevID[DriveNum][60];
			
			KernalPrintf("  Total Sectors: %08X\n", Sectors);
			KernalPrintf("  Disk Size: %llu Megs\n", (Sectors * 512) / 0x100000);

		}
	}
	
}

void IDE::Setup(PCI &Bus)
{
	// First off, lets just see if the PCI bus knows about an IDE device.
	m_DeviceID = Bus.FindDeviceID(0x01, 0x01);
	if(m_DeviceID != 0xFFFFFFFF)
	{
		// We have a device ID, so lets see what else we have
		uint8_t ProgID = (Bus.ReadRegister(m_DeviceID, 0x08) & 0x0000FF00) >> 8;
		if(ProgID & 0x80)
		{
			m_PCIBusMaster = true;
			m_Primary.BusMaster = (Bus.ReadRegister(m_DeviceID, 0x20)) & 0xFFFE;
			m_Secondary.BusMaster = m_Primary.BusMaster + 8;
		}

		if(ProgID & 0x01)
		{
			m_Primary.Base = (Bus.ReadRegister(m_DeviceID, 0x10)) & 0xFFFE;
			m_Primary.Status = (Bus.ReadRegister(m_DeviceID, 0x14)) & 0xFFFE;
		}

		if(ProgID & 0x04)
		{
			m_Secondary.Base = (Bus.ReadRegister(m_DeviceID, 0x18)) & 0xFFFE;
			m_Secondary.Status = (Bus.ReadRegister(m_DeviceID, 0x1C)) & 0xFFFE;
		}

		// Make sure it's allowed to be a bus master
		if(m_PCIBusMaster == true)
		{
			uint32_t Temp = Bus.ReadRegister(m_DeviceID, 0x04);
			Temp |= 0x04;
			Bus.SetRegister(m_DeviceID, 0x04, Temp);
		}
	}

	// Okay, time to ID the drive that exist, there are some extra steps for this one, so lets go.
	
	if(SendCommand(m_Primary, false, 0, 0, 0xEC))
	{
		// if it's 0 there is no drive
		if(InPortb(m_Primary.Status) != 0)
		{
			ReadPIO(m_Primary, &m_DevID[0], 0x100);
			m_Primary.Device0Type = ATADrive;
		}
	}
	else
	{
		if(SendCommand(m_Primary, false, 0, 0, 0xA1))
		{
			ReadPIO(m_Primary, &m_DevID[0], 0x100);
			m_Primary.Device0Type = ATAPIDrive;
		}
	}


	if(SendCommand(m_Primary, true, 0, 0, 0xEC))
	{
		// if it's 0 there is no drive
		if(InPortb(m_Primary.Status) != 0)
		{
			ReadPIO(m_Primary, &m_DevID[1], 0x100);
			m_Primary.Device1Type = ATADrive;
		}
	}
	else
	{
		if(SendCommand(m_Primary, true, 0, 0, 0xA1))
		{
			ReadPIO(m_Primary, &m_DevID[1], 0x100);
			m_Primary.Device1Type = ATAPIDrive;
		}
	}

	if(SendCommand(m_Secondary, false, 0, 0, 0xEC))
	{
		// if it's 0 there is no drive
		if(InPortb(m_Secondary.Status) != 0)
		{
			ReadPIO(m_Secondary, &m_DevID[2], 0x100);
			m_Secondary.Device0Type = ATADrive;
		}
	}
	else
	{
		if(SendCommand(m_Secondary, false, 0, 0, 0xA1))
		{
			ReadPIO(m_Secondary, &m_DevID[2], 0x100);
			m_Secondary.Device0Type = ATAPIDrive;
		}
	}

	if(SendCommand(m_Secondary, true, 0, 0, 0xEC))
	{
		// if it's 0 there is no drive
		if(InPortb(m_Secondary.Status) != 0)
		{
			ReadPIO(m_Secondary, &m_DevID[3], 0x100);
			m_Secondary.Device1Type = ATADrive;
		}
	}
	else
	{
		if(SendCommand(m_Secondary, true, 0, 0, 0xA1))
		{
			ReadPIO(m_Secondary, &m_DevID[3], 0x100);
			m_Secondary.Device1Type = ATAPIDrive;
		}
	}

	//OutPortb(m_Primary.Status, 0x02);
	//SendCommand(m_Primary, false, 0, 1, 0xC8); // Read DMA
	//ReadDMA(m_Primary, reinterpret_cast<void *>(0x10000300), 0x200);

	ObjectManager::Current()->AddObject("IDE", 3, this);
	
	if(m_Primary.Device0Type == ATADrive)
		ObjectManager::Current()->AddObject("HD0", 3, this);
	else if(m_Primary.Device0Type == ATAPIDrive)
		ObjectManager::Current()->AddObject("CD0", 3, this);

	if(m_Primary.Device1Type == ATADrive)
		ObjectManager::Current()->AddObject("HD1", 3, this);
	else if(m_Primary.Device1Type == ATAPIDrive)
		ObjectManager::Current()->AddObject("CD1", 3, this);

	if(m_Secondary.Device0Type == ATADrive)
		ObjectManager::Current()->AddObject("HD2", 3, this);
	else if(m_Secondary.Device0Type == ATAPIDrive)
		ObjectManager::Current()->AddObject("CD2", 3, this);

	if(m_Secondary.Device1Type == ATADrive)
		ObjectManager::Current()->AddObject("HD3", 3, this);
	else if(m_Secondary.Device1Type == ATAPIDrive)
		ObjectManager::Current()->AddObject("CD3", 3, this);
}

bool IDE::SendCommand(DeviceInfo &Device, bool Slave, uint32_t SectorNumber, uint8_t SectorCount, uint8_t Command)
{
	// Select the drive to use
	if(Slave)
		OutPortb(Device.Base + 6, (0xF0 | (SectorNumber & 0x0F00000) >> 24));
	else
		OutPortb(Device.Base + 6, (0xE0 | (SectorNumber & 0x0F00000) >> 24));

	// Kill some time
	InPortb(Device.Status);
	InPortb(Device.Status);
	InPortb(Device.Status);
	InPortb(Device.Status);

	OutPortb(Device.Base + 2, SectorCount);
	OutPortb(Device.Base + 3, SectorNumber & 0x000000FF);
	OutPortb(Device.Base + 4, (SectorNumber & 0x0000FF00) >> 8);
	OutPortb(Device.Base + 5, (SectorNumber & 0x00FF0000) >> 16);

	OutPortb(Device.Base + 7, Command);

	// Kill some time
	InPortb(Device.Status);
	InPortb(Device.Status);
	InPortb(Device.Status);
	InPortb(Device.Status);

	return !(InPortb(Device.Status) & 0x01);
}

bool IDE::ReadPIO(DeviceInfo &Device, void *Address, uint32_t WordCount)
{
	uint16_t * Data = reinterpret_cast<uint16_t *>(Address);

	while((InPortb(Device.Status) & 0x08) == 0x00);

	// Slow read time.
	for(uint32_t x = 0; x < WordCount; x++)
	{
		*Data = InPortw(Device.Base);
		Data++;
	}

	return !(InPortb(Device.Status) & 0x01);
}

bool IDE::ReadDMA(DeviceInfo &Device, void *Address, uint32_t ByteCount)
{
	uint32_t *PRD = reinterpret_cast<uint32_t *>(0x70000000);
	
	while((InPortb(Device.Status) & 0x08) == 0x00);

	KernalPrintf("%02X\n", InPortb(Device.Status));

	if(m_PCIBusMaster == false)
	{
		return ReadPIO(Device, Address, ByteCount / 2);
	}
	
	// Turn on the read flag
	OutPortb(Device.BusMaster, 0x08);
	OutPortb(Device.BusMaster + 2, InPortb(Device.BusMaster + 2) | 0x07);

	PRD[0] = reinterpret_cast<uint32_t>(Address);
	PRD[1] = 0x80000000 | ByteCount;

	// Set the address of the PRD.
	OutPortd(Device.BusMaster + 4, reinterpret_cast<uint32_t>(PRD));
	
	
	// Start the busmaster
	OutPortb(Device.BusMaster, 0x09);
	
	//while((InPortb(Device.BusMaster + 2) & 0x01) == 0x01);

	KernalPrintf("%08X, %08X\n", PRD[0], PRD[1]);

	//OutPortb(Device.BusMaster, 0x00);

	KernalPrintf("%02X\n", InPortb(Device.Status));
	
	return !(InPortb(Device.Status) & 0x01);
}