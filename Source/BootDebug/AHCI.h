#include <stdint.h>
#include "PCI.h"
#include "ObjectCallback.h"
#pragma once

namespace AHCIData
{
	struct GenericHostControl;
	struct PortRegister;
}

class AHCI : public ObjectCallback
{
	uint32_t m_DeviceID;
	AHCIData::GenericHostControl * m_HostControl;
	AHCIData::PortRegister * m_Ports;

	void DisplayObject(uint32_t ArgCount, char *ArgData[]);

public:
	AHCI(void);
	~AHCI(void);

	void Setup(PCI &Bus);
};

