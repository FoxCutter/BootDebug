#include "DriverBase.h"

#pragma comment(linker, "/section:.DRIVER,RW")
#pragma comment(linker, "/merge:.DRIVER=.data")

using namespace Driver;

#pragma data_seg(".DRIVER$!")
static CatalogBase	__DVR_a = {
	sizeof(CatalogBase), eDriverTypeUnknown, eInformationFlagsNone, "", nullptr, nullptr, nullptr
};
#pragma data_seg()

#pragma data_seg(".DRIVER$~")
static CatalogBase	__DVR_z = {
	sizeof(CatalogBase), eDriverTypeUnknown, eInformationFlagsNone, "", nullptr, nullptr, nullptr
};
#pragma data_seg()




#pragma data_seg(".DRIVER$OHCI")
static PCIDriverCatalog OHCIDriverCatalog = { 
		{ 
				sizeof(PCIDriverCatalog),			// Size
				eDriverTypePCI,						// Byte Type
				eInformationInterfaceDevice,		// We are an PCI interface Driver
				"USB1-OHCI",						// Name
				nullptr,							// Function
				nullptr, nullptr,					// Links.
		}, 

		0xFFFF, 0xFFFF,								// Vender:Device ID
		0x0C, 0x03, 0x10,							// Class, Subclass, IntefaceID
		0xFF										// Revision ID (unused)
	};
#pragma data_seg(".DRIVER$PS2K")
static SystemBoardDriverCatalog PS2KDriverCatalog = { 
		{ 
				sizeof(SystemBoardDriverCatalog),	// Size
				eDriverTypeSystem,						// Byte Type
				eInformationFlagsNone,				// Nothing Speical
				"PS/2 Keyboard",					// Name
				nullptr,							// Function
				nullptr, nullptr,					// Links.
		}, 
		"PNP0303", ""
	};
#pragma data_seg(".DRIVER$PS2M")
static SystemBoardDriverCatalog PS2MDriverCatalog = { 
		{ 
				sizeof(SystemBoardDriverCatalog),	// Size
				eDriverTypeSystem,					// Byte Type
				eInformationFlagsNone,				// We are an PCI interface Driver
				"PS/2 Mouse",						// Name
				nullptr,							// Function
				nullptr, nullptr,					// Links.
		}, 
		"PNP0F13", ""
	};

#pragma data_seg()
