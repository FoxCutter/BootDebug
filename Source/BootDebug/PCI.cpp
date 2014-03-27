#include "PCI.h"
#include <stdio.h>
#include <string.h>
#include "LowLevel.h"

PCI::PCI(void)
{
}


PCI::~PCI(void)
{
}

bool PCI::Initilize()
{
	EnumerateBus(0);
	return true;
}

bool PCI::EnumerateBus(uint8_t Bus)
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
			printf("PCI(%08X)", CurrentDeviceID);
			printf(": Vender %04X, Dev %04X", Val & 0xFFFF, (Val & 0xFFFF0000) >> 16);
			
			Val = ReadRegister(CurrentDeviceID, 0x08);
			printf(", Class %02X, Sub %02X, Prog %02X", (Val & 0xFF000000) >> 24, (Val & 0x00FF0000) >> 16, (Val & 0x0000FF00) >> 8);
			
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
			printf("\n");
		}
	} 

	return false;
}

bool PCI::DumpDevice(uint32_t DeviceID)
{
	DeviceID = DeviceID & 0xFFFFFF00;

	uint32_t Val = ReadRegister(DeviceID, 0);

	if(Val == 0xFFFFFFFF)
	{
		printf("PCI Device %08X is not present\n", DeviceID);
		return false;
	}

	printf("PCI Device ID: %08X\n", DeviceID);
	printf("  Vender ID %04X, Device ID %04X\n", Val & 0xFFFF, (Val & 0xFFFF0000) >> 16);

	Val = ReadRegister(DeviceID, 4);
	printf("  Status %04X, Command %04X\n", (Val & 0xFFFF0000) >> 16, Val & 0xFFFF);
	
	Val = ReadRegister(DeviceID, 8);
	printf("  Class %02X, Sub Class %02X, Prog IF %02X, Revision ID %02X\n", (Val & 0xFF000000) >> 24, (Val & 0x00FF0000) >> 16, (Val & 0x0000FF00) >> 8, Val & 0x000000FF);
	
	Val = ReadRegister(DeviceID, 0x0C);
	printf("  BIST %02X, Header Type %02X, Latency Timer %02X, Cache Line Size %02X\n", (Val & 0xFF000000) >> 24, (Val & 0x00FF0000) >> 16, (Val & 0x0000FF00) >> 8, Val & 0x000000FF);

	Val = ReadRegister(DeviceID, 0x10);
	printf("  Base Address 0 %08X\n", Val);

	Val = ReadRegister(DeviceID, 0x14);
	printf("  Base Address 1 %08X\n", Val);

	Val = ReadRegister(DeviceID, 0x18);
	printf("  Base Address 2 %08X\n", Val);

	Val = ReadRegister(DeviceID, 0x1C);
	printf("  Base Address 3 %08X\n", Val);

	Val = ReadRegister(DeviceID, 0x20);
	printf("  Base Address 4 %08X\n", Val);

	Val = ReadRegister(DeviceID, 0x24);
	printf("  Base Address 5 %08X\n", Val);

	Val = ReadRegister(DeviceID, 0x28);
	printf("  Cardbus Pointer %08X\n", Val);

	Val = ReadRegister(DeviceID, 0x2C);
	printf("  Subsystem ID %04X, Subsystem Vender ID %04X\n", (Val & 0xFFFF0000) >> 16, Val & 0xFFFF);

	Val = ReadRegister(DeviceID, 0x30);
	printf("  Expansion Rom Base %08X\n", Val);

	Val = ReadRegister(DeviceID, 0x34);
	printf("  Reserved %06X, Capabilities Pointer %02X\n",  (Val & 0xFFFFFF00) >> 8, Val & 0x000000FF);

	Val = ReadRegister(DeviceID, 0x38);
	printf("  Reserved %08X\n", Val);

	Val = ReadRegister(DeviceID, 0x3C);
	printf("  Max latency %02X, Min Grant %02X, Interrupt PIN %02X, Interrupt Line %02X\n", (Val & 0xFF000000) >> 24, (Val & 0x00FF0000) >> 16, (Val & 0x0000FF00) >> 8, Val & 0x000000FF);

	return true;
}

uint32_t PCI::ReadRegister(uint32_t DeviceID, uint8_t Register)
{
	OutPortd(ConfigPort, BuildRegisterID(DeviceID, Register));
	return InPortd(DataPort);
}

void PCI::SetRegister(uint32_t DeviceID, uint8_t Register, uint32_t Value)
{
	OutPortd(ConfigPort, BuildRegisterID(DeviceID, Register));
	OutPortd(DataPort, Value);
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