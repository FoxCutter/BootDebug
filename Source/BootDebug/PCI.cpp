#include "PCI.h"
#include <stdio.h>
#include <string.h>
#include "LowLevel.h"
#include "Utility.h"
#include "ObjectManager.h"
#include "KernalLib.h"

PCI::PCI(void)
{
}


PCI::~PCI(void)
{
}

bool PCI::Initilize()
{
	ObjectManager::Current()->AddObject("PCI", 3, this);
	
	//for(int x = 0; x < 256; x++)
	//	EnumerateBus(x);

	return true;
}

bool PCI::EnumerateBus(uint8_t Bus)
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
			KernalPrintf("PCI %06X(%02X:%02X:%02X)", CurrentDeviceID, Bus, CurrentDevice, CurrentFunction);
			KernalPrintf(" Dev %04X:%04X", Val & 0xFFFF, (Val & 0xFFFF0000) >> 16);
			
			Val = ReadRegister(CurrentDeviceID, 0x08);
			KernalPrintf(", C %02X, S %02X, Pn  %02X", (Val & 0xFF000000) >> 24, (Val & 0x00FF0000) >> 16, (Val & 0x0000FF00) >> 8);
			
			Val = ReadRegister(CurrentDeviceID, 0x2C);
			KernalPrintf(", SV %04X, SI %04X", Val & 0xFFFF, (Val & 0xFFFF0000) >> 16);

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
				Val = (Val & 0x00FF0000) >> 16;

				KernalPrintf(", SB %02X", Val);
			}
			KernalPrintf("\n");
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
		KernalPrintf("PCI Device %08X is not present\n", DeviceID);
		return false;
	}

	KernalPrintf("PCI Device ID: %08X\n", DeviceID);
	KernalPrintf("  Vender ID %04X, Device ID %04X\n", Val & 0xFFFF, (Val & 0xFFFF0000) >> 16);

	Val = ReadRegister(DeviceID, 4);
	KernalPrintf("  Status %04X, Command %04X\n", (Val & 0xFFFF0000) >> 16, Val & 0xFFFF);
	
	Val = ReadRegister(DeviceID, 8);
	KernalPrintf("  Class %02X, Sub Class %02X, Prog IF %02X, Revision ID %02X\n", (Val & 0xFF000000) >> 24, (Val & 0x00FF0000) >> 16, (Val & 0x0000FF00) >> 8, Val & 0x000000FF);
	
	Val = ReadRegister(DeviceID, 0x0C);
	KernalPrintf("  BIST %02X, Header Type %02X, Latency Timer %02X, Cache Line Size %02X\n", (Val & 0xFF000000) >> 24, (Val & 0x00FF0000) >> 16, (Val & 0x0000FF00) >> 8, Val & 0x000000FF);

	Val = (Val & 0x00FF0000) >> 16;

	if((Val & 0x7F) == 0x00)
	{	
		Val = ReadRegister(DeviceID, 0x10);
		KernalPrintf("  Base Address 0 %08X\n", Val);

		Val = ReadRegister(DeviceID, 0x14);
		KernalPrintf("  Base Address 1 %08X\n", Val);

		Val = ReadRegister(DeviceID, 0x18);
		KernalPrintf("  Base Address 2 %08X\n", Val);

		Val = ReadRegister(DeviceID, 0x1C);
		KernalPrintf("  Base Address 3 %08X\n", Val);

		Val = ReadRegister(DeviceID, 0x20);
		KernalPrintf("  Base Address 4 %08X\n", Val);

		Val = ReadRegister(DeviceID, 0x24);
		KernalPrintf("  Base Address 5 %08X\n", Val);

		Val = ReadRegister(DeviceID, 0x28);
		KernalPrintf("  Cardbus Pointer %08X\n", Val);

		Val = ReadRegister(DeviceID, 0x2C);
		KernalPrintf("  Subsystem ID %04X, Subsystem Vender ID %04X\n", (Val & 0xFFFF0000) >> 16, Val & 0xFFFF);

		Val = ReadRegister(DeviceID, 0x30);
		KernalPrintf("  Expansion Rom Base %08X\n", Val);

		Val = ReadRegister(DeviceID, 0x34);
		uint8_t CapRegister = Val & 0x000000FF;
		KernalPrintf("  Reserved %06X, Capabilities Pointer %02X\n",  (Val & 0xFFFFFF00) >> 8, CapRegister);

		while(CapRegister != 00)
		{
			Val = ReadRegister(DeviceID, CapRegister);
			KernalPrintf("    ID: %02X, Register: %02X\n",  Val & 0x000000FF, CapRegister);
			
			CapRegister = (Val & 0x0000FF00) >> 8;
		};
		
		//Val = ReadRegister(DeviceID, 0x38);
		//KernalPrintf("  Reserved %08X\n", Val);

		Val = ReadRegister(DeviceID, 0x3C);
		KernalPrintf("  Max latency %02X, Min Grant %02X, Interrupt PIN %02X, Interrupt Line %02X\n", (Val & 0xFF000000) >> 24, (Val & 0x00FF0000) >> 16, (Val & 0x0000FF00) >> 8, Val & 0x000000FF);
	}
	else
	{
		Val = ReadRegister(DeviceID, 0x10);
		KernalPrintf("  Base Address 0 %08X\n", Val);

		Val = ReadRegister(DeviceID, 0x14);
		KernalPrintf("  Base Address 1 %08X\n", Val);

		Val = ReadRegister(DeviceID, 0x18);
		KernalPrintf("  Sec Latency Time %02X, Sub Bus %02X, Secondary Bus %02X, Primary Bus %02X\n", (Val & 0xFF000000) >> 24, (Val & 0x00FF0000) >> 16, (Val & 0x0000FF00) >> 8, Val & 0x000000FF);

		Val = ReadRegister(DeviceID, 0x1C);
		KernalPrintf("  Secondary Status %04X, I/O Limit %02X, I/O Base %02X\n", (Val & 0xFFFF0000) >> 16, (Val & 0x0000FF00) >> 8, Val & 0x000000FF);

		Val = ReadRegister(DeviceID, 0x20);
		KernalPrintf("  Memory Limit %04X, Memory Base %04X\n", (Val & 0xFFFF0000) >> 16, Val & 0xFFFF);

		Val = ReadRegister(DeviceID, 0x24);
		KernalPrintf("  Prefetch Memory Limit %04X, Prefetch Memory Base %04X\n", (Val & 0xFFFF0000) >> 16, Val & 0xFFFF);

		Val = ReadRegister(DeviceID, 0x28);
		KernalPrintf("  Prefetchable Base Upper 32 Bits %08X\n", Val);

		Val = ReadRegister(DeviceID, 0x2C);
		KernalPrintf("  Prefetchable Limit Upper 32 Bits %08X\n", Val);

		Val = ReadRegister(DeviceID, 0x30);
		KernalPrintf("  I/O Limit Upper 16 Bits  %04X, I/O Base Upper 16 Bits %04X\n", (Val & 0xFFFF0000) >> 16, Val & 0xFFFF);

		Val = ReadRegister(DeviceID, 0x34);
		KernalPrintf("  Reserved %06X, Capabilities Pointer %02X\n",  (Val & 0xFFFFFF00) >> 8, Val & 0x000000FF);

		Val = ReadRegister(DeviceID, 0x38);
		KernalPrintf("  Expansion Rom Base %08X\n", Val);

		Val = ReadRegister(DeviceID, 0x3C);
		KernalPrintf("  Bridge Control %04X, Interrupt PIN %02X, Interrupt Line %02X\n", (Val & 0xFFFF0000) >> 16, (Val & 0x0000FF00) >> 8, Val & 0x000000FF);
	}
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
			if((Val & 0xFF000000) >> 24 == Class && (Val & 0x00FF0000) >> 16 == Subclass && (Val & 0x0000FF00) >> 8 == ProgID)
				return CurrentDeviceID;			

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

void PCI::DisplayObject(char * Command)
{
	char *Input = Command;
	char *CurrentData = NextToken(Input);

	if(CurrentData == nullptr)
	{
		for(int x = 0; x < 256; x++)
			EnumerateBus(x);
		
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