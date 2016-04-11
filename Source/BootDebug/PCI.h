#include <stdint.h>
#include "ObjectCallback.h"
#pragma once

struct HardwareTree; 

class PCI : ObjectCallback
{
	static const uint16_t ConfigPort = 0xCF8;
	static const uint16_t DataPort = 0xCFC;
public:
	PCI(void);
	~PCI(void);

	bool Initilize(HardwareTree * PCIRoot);
	bool DumpBus(uint8_t Bus);
	bool DumpDevice(uint32_t DeviceID);
	static uint32_t ReadRegister(uint32_t DeviceID, uint8_t Register);
	static void SetRegister(uint32_t DeviceID, uint8_t Register, uint32_t Value);

	static uint32_t ReadRegister(uint32_t DeviceID, uint8_t Register, uint8_t Size);
	static void SetRegister(uint32_t DeviceID, uint8_t Register, uint32_t Value, uint8_t Size);

	uint32_t FindDeviceID(uint8_t Class, uint8_t Subclass, uint8_t ProgID = 0);

	static uint32_t BuildDeviceID(uint8_t Bus, uint8_t Device, uint8_t Function);
	static uint32_t BuildRegisterID(uint32_t DeviceID, uint8_t Register);
	static uint32_t BuildRegisterID(uint8_t Bus, uint8_t Device, uint8_t Function, uint8_t Register);

private:
	virtual void DisplayObject(char * Command, char *Param);

	bool EnumerateBus(uint8_t Bus, HardwareTree * Root);
	bool DumpDeviceMemory(uint32_t DeviceID);
};

