#pragma once
#include <stdint.h>

#pragma pack(push, 1)
namespace Driver
{
	/*
	 * The basic flow for a driver
	 * 1 System finds new hardware,
	 * 2 Seaches the driver Catalog for matching drivers
	 * 3 Once found it will create the driver by calling the entry point in the Catalog Base
	 *   3a The function will create the driver object fill out the pointers. 
	 *   3b The function will create an object for the driver with a unique name (IE, USB2, IDE0, VGA0)
	 *   3c Will create a device for each known device on the driver (for non bus drivers, IE, for all the IDE devices on an IDE driver)
	 *   3d Register the interrupt callback functions
	 * 4 The system will create the devices requested via the OpenDevice function
	 *   4a The function will created the device object and fill out the data/pointers
	 *   4b The function will register a unique name for the device (HD0, CD2, HUB0)
	 *   4c The function will initialize the hardware

	 */
	// The Driver Catalog is just a way to collect all the known drivers so it can be searched and then create an instance of the driver
	
	class DriverObject;
	class DeviceObject;

	typedef uint32_t (* DriverFunction)(DriverObject *DriverObject);

	// 48 Bytes
	struct CatalogBase
	{
		uint8_t					Size;
		uint8_t					Type;
		uint16_t				Flags;

		char 					Name[32];
		DriverFunction			EntryPoint;

		CatalogBase *			Next;
		CatalogBase *			Prev;
	};

	enum DriverType : uint8_t
	{
		eDriverTypeUnknown		= 0x00,
		
		// Drivers that connect directly to hardware devices
		eDriverTypeSystem		= 0x10,
		eDriverTypePCI			= 0x11,
		eDriverTypeUSB			= 0x12,

		// File System driver
		eDriverTypeFileSystem   = 0x80,

		// Handles meta data on disk, such as partitions)
		eDriveTypeDiskManager   = 0x81, 
	};

	enum InformationFlags : uint16_t
	{
		eInformationFlagsNone =				0x0000,
		
		eInformationFlagCIDValid =		    0x0001, // In a Sys driver the CID is valid 
		eInformationVenderDevice =			0x0001, // The Vender/Device information is valid

		eInformationInterfaceDevice =		0x0002, // This can be used as a gerenic interface device if no exact match is found

		eInformationUSBVersion =			0x0004, // In a USB driver the Version is valid
		eInformationPCIRevisionID =			0x0004, // In a PCI driver the Revsion ID field is valid

		eInformationUSBProtocolSkip =		0x0008, // In a USB driver the Protocol field can be ignored if there are no other matches
		eInformationUSBInterfaceIDSkip =	0x0008, // In a PCI driver the Interface ID field can be ignored if there are no other matches
	};

	// 64 Bytes
	struct SystemBoardDriverCatalog
	{
		CatalogBase		Header; 
		char			HID[8];			// HID string as defined by ACPI.
		char			CID[8];			// Compatable HID
	};

	// 64 Bytes
	struct USBDriverCatalog
	{
		CatalogBase		Header; 
		uint16_t		VenderID;
		uint16_t		ProductID;

		uint8_t			DeviceClass;
		uint8_t			DeviceSubClass;
		uint8_t			DeviceProtocol;
		uint8_t			Reserved1; 

		uint16_t		DeviceVersion;
		uint16_t		Reserved2;

		uint32_t		Padding;
	};

	// 64 Bytes
	struct PCIDriverCatalog
	{
		CatalogBase		Header; 
		uint16_t		VenderID;
		uint16_t		DeviceID;

		uint8_t			Class;
		uint8_t			SubClass;
		uint8_t			InterfaceID;
		uint8_t			RevisionID;

		uint32_t		Padding[2];
	};

	class DriverCallbacks
	{
	public:
		virtual uint32_t UnloadDriver() = 0;
		virtual uint32_t OpenDevice() = 0;

		virtual uint32_t Status() = 0;
		virtual uint32_t IOControl() = 0;

		virtual uint32_t HandleOpened() = 0;
		virtual uint32_t HandleClosed() = 0;

		virtual uint32_t Read() = 0;
		virtual uint32_t Write() = 0;
		virtual uint32_t Flush() = 0;

		virtual uint32_t HandleGetMetaData() = 0;
		virtual uint32_t HandleSetMetaData() = 0;
	};
	
	class DriverObject
	{
	public:
		// List objects
		DriverObject *		Next;
		DriverObject *		Prev;
		
		// Pointer to devices owned by this driver
		DeviceObject *		Devices;

		// Pointer to the Catalog entry for this driver
		CatalogBase *		CatalogEntry;

		// How maybe devices are initially needed
		uint32_t			InitialDeviceCount;

		// Flags and state information
		uint32_t			Flags;

		// The object used to name this driver
		void *				NameObject;
		
		// Pointer to callback functions
		DriverCallbacks	*	Callbacks;

		// Allocates memory associted directly with this driver, there is only a limited supply (no more the 512) so use
		// It for state data and the driver object itself
		virtual void AllocateMemory();

		// Register the Inerrupt callback functions (Handler & Dispatch)
		virtual void RegisterInterruptFunctions();
		virtual void DeregisterInterruptFunctions();

		virtual void RegisterName();
		virtual void DeregisterName();

		// Creates a new device for this driver (will call back into the driver code) used to create known devices
		// For the driver. 
		virtual void CreateDevice(); 
	};

	enum DriverFlags : uint32_t
	{
		// The IO Control function is valid
		eDriverHasIOControl =		0x00000002,

		// The status function is vaild
		eDriverHasStatus =			0x00000001,
		
		// The handle Open Close functions are valid
		eDriverHasHandleSupport =	0x00000004,

		// The handle Read/write/flush functions are valid
		eDriverHasHandleReadWrite =	0x00000008,

		// The handle meta data functions are valid
		eDriverHasHandleMetaData =	0x00000010,
	};

	class DeviceCallbacks
	{
	public:
		virtual uint32_t CloseDevice() = 0;

		virtual uint32_t Status() = 0;
		virtual uint32_t IOControl() = 0;

		virtual uint32_t HandleOpened() = 0;
		virtual uint32_t HandleClosed() = 0;

		virtual uint32_t Read() = 0;
		virtual uint32_t Write() = 0;
		virtual uint32_t Flush() = 0;

		virtual uint32_t HandleGetMetaData() = 0;
		virtual uint32_t HandleSetMetaData() = 0;
	};

	class DeviceObject
	{
	public:
		DeviceObject *		Next;
		DeviceObject *		Prev;

		// Pointer to the parent driver 
		DriverObject *		Driver;

		// Flags and state information
		uint32_t			Flags;

		// The object used to name this Device
		void *				NameObject;

		// Pointer to callback functions
		DeviceCallbacks	*	Callbacks;

		// Allocates memory associted directly with this device, there is only a limited supply (no more the 512) so use
		// It for state data and the device object itself
		virtual void AllocateMemory();

		virtual void RegisterName();
		virtual void DeregisterName();
	};

}

#pragma pack(pop)
