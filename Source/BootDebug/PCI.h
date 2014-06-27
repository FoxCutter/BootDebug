#include <stdint.h>
#pragma once

class PCI
{
	static const uint16_t ConfigPort = 0xCF8;
	static const uint16_t DataPort = 0xCFC;
public:
	PCI(void);
	~PCI(void);

	bool Initilize();
	bool EnumerateBus(uint8_t Bus);
	bool DumpDevice(uint32_t DeviceID);
	static uint32_t ReadRegister(uint32_t DeviceID, uint8_t Register);
	static void SetRegister(uint32_t DeviceID, uint8_t Register, uint32_t Value);

	uint32_t FindDeviceID(uint8_t Class, uint8_t Subclass, uint8_t ProgID);

private:
	static uint32_t BuildDeviceID(uint8_t Bus, uint8_t Device, uint8_t Function);
	static uint32_t BuildRegisterID(uint32_t DeviceID, uint8_t Register);
	static uint32_t BuildRegisterID(uint8_t Bus, uint8_t Device, uint8_t Function, uint8_t Register);


};

