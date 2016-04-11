#include <stdint.h>
#include "PCI.h"
#include "ObjectCallback.h"

#pragma once
class IDE : public ObjectCallback
{
	enum DriveType
	{
		NoDrive = 0,
		ATADrive,
		ATAPIDrive,
	};
	
	struct DeviceInfo
	{
		// Base Port for the IDE inteface
		uint16_t Base;
		
		// Alternate Status Port
		uint16_t Status;

		// Base for the Bus Master
		uint16_t BusMaster;
		
		// The Last value of the Driver Register
		uint8_t LastDrive;

		DriveType Device0Type;
		DriveType Device1Type;
	};
	
	uint32_t m_DeviceID;
	bool m_PCIBusMaster;

	DeviceInfo m_Primary;
	DeviceInfo m_Secondary;

	__declspec(align(32)) 
	uint16_t m_DevID[4][0x100];

public:
	IDE(void);
	~IDE(void);
	
	void IDE::Setup(PCI &Bus);

	void DisplayObject(char * Command, char *Param);

private:
	bool SendCommand(DeviceInfo &Device, bool Slave, uint32_t SectorNumber, uint8_t SectorCount, uint8_t Command);
	bool ReadPIO(DeviceInfo &Device, void *Address, uint32_t WordCount);
	bool ReadDMA(DeviceInfo &Device, void *Address, uint32_t ByteCount);

	bool WritePIO(DeviceInfo &Device, void *Address, uint32_t WordCount);
	bool WriteDMA(DeviceInfo &Device, void *Address, uint32_t ByteCount);
};

