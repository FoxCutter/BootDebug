#include <stdint.h>
#include "USBData.h"
#include "RawMemory.h"
#pragma once

namespace OpenHCIData 
{
	struct HCRegisters;
}

class InterruptControler;
struct InterruptContext;
struct PipeData;
struct TransferData;
enum PipeState;

class USBHubInterface
{
	virtual uint8_t NumberOfPorts() = 0;
	virtual uint8_t PowerOnToGood() = 0;
	virtual uint8_t MaxCurrent() = 0;
	virtual bool IsDeviceRemovable(uint8_t PortNumber) = 0;
	
	// Returns a bitmap of what had change. Bit 0 is the hub, and the rest of the bits are the ports.
	virtual uint32_t StatusChangeBitmap() = 0;

	virtual USBHub::HubStatusAndChange GetHubStatusAndChanged() = 0;
	virtual void ClearHubChanged(USBHub::HubStatusAndChange &Flags) = 0;

	virtual USBHub::PortStatusAndChange GetPortStatusAndChanged(uint8_t PortNumber) = 0;
	virtual void ClearPortChanged(uint8_t PortNumber, USBHub::PortStatusAndChange &Flags) = 0;
};

typedef bool (*OpenHCICallbackPtr)(uint32_t Handle, uintptr_t * Data);

class OpenHCI : public USBHubInterface
{
	RawMemory Allocator;

	uint32_t m_DeviceID;
	uint8_t m_IRQ;
	InterruptControler *m_IntControler;
	volatile OpenHCIData::HCRegisters * Registers;

	PipeData * FreePipeHead;	
	PipeData * InterruptEndpoints[6];
	TransferData * FreeTransferDataHead;

	PipeData * DefaultHandle;
	PipeData * IsochronusPipeHead;

	OpenHCICallbackPtr RootHubChangeCallback;
	uintptr_t *RootHubChangeData;

	void FillInterruptTable(PipeData *Table, int Length, PipeData *LinkTable, int LinkTableLength);

	void AddEndpoint(PipeData *Base, PipeData *EndPoint);
	void EnableEndpoint(PipeData *EndPoint);
	void DisableEndpoint(PipeData *EndPoint, PipeState NewState);
	void FreeEndpoint(PipeData *EndPoint);
	void ClearEndpointData(PipeData *EndPoint);

	void SpinLockEndpoint(PipeData *EndPoint);

	PipeData *GetEndpoint();
	TransferData *GetTransferData();

	void FreeTransferData(TransferData *Data);

	bool IsPeriodicEndpoint(PipeData *EndPoint);

	bool FetchString(uint32_t Handle, uint8_t ID, uint16_t Language);

	void BuildTransferData(PipeData * DataHandle, void *Data, size_t Length, int Direction, int DataToggle = 0, bool MarkLastBlock = true);
	void AddTerminalTransferData(PipeData * DataHandle);

public:
	enum PipeType
	{
		BulkPipe,
		ControlPipe,

		InterruptPipe,
		IsochronusPipe,

		ReservedPipe = -1,
	};

	struct PipeInformation
	{
		uint8_t Address;
		uint8_t Endpoint;

		PipeType Type;

		uint16_t MaxPacketSize;

		bool LowSpeedDevice;	
		bool ReadFromDevice;

		// The frequency to query an interrupt pipe
		uint8_t Frequencey;

		// Function to call when an Interrupt or Isochronus transfer completes.
		OpenHCICallbackPtr CompletionCallback;
		uintptr_t *CallbackData;

		PipeInformation()
		{
			Address = 0;
			Endpoint = 0;
			Type = BulkPipe;
			MaxPacketSize = 0x08;
			LowSpeedDevice = false;
			ReadFromDevice = false;
			Frequencey = 0xFF;
			CompletionCallback = nullptr;
			CallbackData = nullptr;
		}
	};

	void Interrupt(InterruptContext * Context);
	
	OpenHCI(void);
	~OpenHCI(void);

	bool StartUp(uint32_t DeviceID, InterruptControler *IntControler);

	const static uint32_t DefaultDeviceHandle = 0x00000000;

	uint32_t OpenPipe(PipeInformation &Pipe);
	bool AdjustMaxPacketSize(uint32_t Handle, uint16_t MaxPacketSize);
	void ClosePipe(uint32_t Handle);
	bool ReadPipe(uint32_t Handle, void *Data, size_t Length, size_t &Read, bool Block = true);
	bool WritePipe(uint32_t Handle, void *Data, size_t Length, bool Block = true);
	bool DeviceRequest(uint32_t Handle, USBData::USBDeviceRequest &Request, void *Buffer);
	uint32_t GetPipeStatus(uint32_t Handle);
	void ClearPipeError(uint32_t Handle);
	void WaitOnPipe(uint32_t Handle);
	bool PipeCompleted(uint32_t Handle);
	uint32_t GetByteCount(uint32_t Handle);
	
	//bool DisableInterruptPipe(uint32_t Handle);
	//bool ResetInterruptPipe(uint32_t Handle, OpenHCICallbackPtr CompletionCallback, uintptr_t *CallbackData Data);

	void Dump();


	// Root hub info
	uint8_t NumberOfPorts();
	uint8_t PowerOnToGood();
	uint8_t MaxCurrent();
	bool IsDeviceRemovable(uint8_t PortNumber);
	
	// Returns a bitmap of what had change. Bit 0 is the hub, and the rest of the bits are the ports.
	uint32_t StatusChangeBitmap();

	USBHub::HubStatusAndChange GetHubStatusAndChanged();
	void ClearHubChanged(USBHub::HubStatusAndChange &Flags);

	USBHub::PortStatusAndChange GetPortStatusAndChanged(uint8_t PortNumber);
	void ClearPortChanged(uint8_t PortNumber, USBHub::PortStatusAndChange &Flags);

	void SetRootHubChangeInterrupt(OpenHCICallbackPtr CallBack, uintptr_t *CallbackData);
};

