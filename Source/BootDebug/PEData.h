#include <stdint.h>
#pragma once

#pragma pack(push, 1)

namespace PEFile
{
    // DOS .EXE header
    struct IMAGE_DOS_HEADER
    {
        uint16_t e_magic;                     // Magic number (0x5A4D)
        uint16_t e_cblp;                      // Bytes on last page of file
        uint16_t e_cp;                        // Pages in file
        uint16_t e_crlc;                      // Relocations
        uint16_t e_cparhdr;                   // Size of header in paragraphs
        uint16_t e_minalloc;                  // Minimum extra paragraphs needed
        uint16_t e_maxalloc;                  // Maximum extra paragraphs needed
        uint16_t e_ss;                        // Initial (relative) SS value
        uint16_t e_sp;                        // Initial SP value
        uint16_t e_csum;                      // Checksum
        uint16_t e_ip;                        // Initial IP value
        uint16_t e_cs;                        // Initial (relative) CS value
        uint16_t e_lfarlc;                    // File address of relocation table
        uint16_t e_ovno;                      // Overlay number
        uint16_t e_res[4];                    // Reserved words
        uint16_t e_oemid;                     // OEM identifier (for e_oeminfo)
        uint16_t e_oeminfo;                   // OEM information; e_oemid specific
        uint16_t e_res2[10];                  // Reserved words
        uint32_t e_lfanew;                    // File address of new exe header
    };

    enum ImageFileCharacteristics : uint16_t
    {
        RELOCS_STRIPPED             = 0x0001,  // Image Only: Relocation info stripped from file.
        EXECUTABLE_IMAGE            = 0x0002,  // Image Only: File is executable 
        LINE_NUMS_STRIPPED          = 0x0004,  // COFF Line nunbers stripped from file (depricated).
        LOCAL_SYMS_STRIPPED         = 0x0008,  // COFF Local symbols stripped from file (deprecated).
        AGGRESIVE_WS_TRIM           = 0x0010,  // Agressively trim working set (deprecated)
        LARGE_ADDRESS_AWARE         = 0x0020,  // App can handle >2gb addresses
        BYTES_REVERSED_LO           = 0x0080,  // Little Endian (deprecated)
        S32BIT_MACHINE              = 0x0100,  // 32 bit word machine.
        DEBUG_STRIPPED              = 0x0200,  // Debugging info stripped from file
        REMOVABLE_RUN_FROM_SWAP     = 0x0400,  // If Image is on removable media, copy and run from the swap file.
        NET_RUN_FROM_SWAP           = 0x0800,  // If Image is on Network, copy and run from the swap file.
        SYSTEM                      = 0x1000,  // System File.
        DLL                         = 0x2000,  // File is a DLL.
        UP_SYSTEM_ONLY              = 0x4000,  // File should only be run on a Uniprocessor machine
        BYTES_REVERSED_HI           = 0x8000,  // Big endian (deprecated)
    };

    enum class ImageFileMachine : uint16_t
    {
        UNKNOWN     = 0,
		AM33		= 0x01d3,		// Matsushita AM33
        AMD64       = 0x8664,       // x64
        ARM         = 0x01c0,       // ARM Little-Endian
        ARMNT       = 0x01c4,       // ARMv7 (or higher) Thumb mode only
		ARM64		= 0xaa64,		// ARMv8 in 64-bit mode
        EBC         = 0x0EBC,       // EFI Byte Code
		I386        = 0x014c,       // Intel 386 or later
        IA64        = 0x0200,       // Intel Itanium 
		M32R		= 0x9041,		// Mitsubishi M32R little endian
        MIPS16      = 0x0266,       // MIPS16
        MIPSFPU     = 0x0366,       // MIPS with FPU
        MIPSFPU16   = 0x0466,       // MIPS16 with FPU
        POWERPC     = 0x01F0,       // IBM PowerPC Little-Endian
        POWERPCFP   = 0x01f1,       // IBM PowerPC with floating point support
        R4000       = 0x0166,       // MIPS little-endian
		RISCV32		= 0x5032,		// RISC-V 32-bit address space 
		RISCV64		= 0x5064,		// RISC-V 64-bit address space 
		RISCV128	= 0x5128,		// RISC-V 128-bit address space 
        SH3         = 0x01a2,       // Hitachi SH3 little-endian
        SH3DSP      = 0x01a3,       // Hitachi SH3 DSP
        SH4         = 0x01a6,       // Hitachi SH4 little-endian
        SH5         = 0x01a8,       // Hitachi SH5
        THUMB       = 0x01c2,       // ARM Thumb/Thumb-2 Little-Endian
        WCEMIPSV2   = 0x0169,       // MIPS little-endian WCE v2
    };

    struct IMAGE_FILE_HEADER
    {
        ImageFileMachine Machine;
        uint16_t NumberOfSections;
        uint32_t TimeDateStamp;
        uint32_t PointerToSymbolTable;
        uint32_t NumberOfSymbols;
        uint16_t SizeOfOptionalHeader;
        ImageFileCharacteristics Characteristics;
    };

    enum class OptionSignature : uint16_t
    {
        NT_32Bit    = 0x010b,
        NT_64Bit    = 0x020b,
        ROM         = 0x0107,
    };

    enum class SubSystem : uint16_t
    {
        UNKNOWN                     = 0,    // Unknown subsystem.
        NATIVE                      = 1,    // Image doesn't require a subsystem.
        WINDOWS_GUI                 = 2,    // Image runs in the Windows GUI subsystem.
        WINDOWS_CUI                 = 3,    // Image runs in the Windows character subsystem.
        OS2_CUI                     = 5,    // image runs in the OS/2 character subsystem.
        POSIX_CUI                   = 7,    // image runs in the Posix character subsystem.
        NATIVE_WINDOWS              = 8,    // image is a native Win9x driver.
        WINDOWS_CE_GUI              = 9,    // Image runs in the Windows CE subsystem.
        EFI_APPLICATION             = 10,   // An Extensible Firmware Interface (EFI) application 
        EFI_BOOT_SERVICE_DRIVER     = 11,   // An EFI driver with boot services
        EFI_RUNTIME_DRIVER          = 12,   // An EFI driver with run-time services 
        EFI_ROM                     = 13,	// An EFI ROM image
        XBOX                        = 14,	// XBOX 
        WINDOWS_BOOT_APPLICATION    = 16,	// Windows boot application. 
    };

    enum DllCharacteristics : uint16_t
    {
        //PROCESS_INIT          = 0x0001,   // Reserved.
        //PROCESS_TERM          = 0x0002,   // Reserved.
        //THREAD_INIT           = 0x0004,   // Reserved.
        //THREAD_TERM           = 0x0008,   // Reserved.
        HIGH_ENTROPY_VA         = 0x0020,   // Image can handle a high entropy 64-bit virtual address space.
        DYNAMIC_BASE            = 0x0040,   // DLL can be relocated at load time. 
        FORCE_INTEGRITY         = 0x0080,   // Code Integrity checks are enforced. 
        NX_COMPAT               = 0x0100,   // Image is NX compatible
        NO_ISOLATION            = 0x0200,   // Image understands isolation and doesn't want it
        NO_SEH                  = 0x0400,   // Image does not use SEH.  No SE handler may reside in this image
        NO_BIND                 = 0x0800,   // Do not bind this image.
        APPCONTAINER            = 0x1000,   // Image should execute in an AppContainer
        WDM_DRIVER              = 0x2000,   // Driver uses WDM model
		GUARD_CF				= 0x4000,   // Image supports Control Flow Guard. 
        TERMINAL_SERVER_AWARE   = 0x8000,	// Terminal Server aware. 
    };
    
	struct IMAGE_OPTIONAL_HEADER32
    {
        //
        // Standard fields.
        //

        OptionSignature Magic;
        uint8_t MajorLinkerVersion;
        uint8_t MinorLinkerVersion;
        uint32_t SizeOfCode;
        uint32_t SizeOfInitializedData;
        uint32_t SizeOfUninitializedData;
        uint32_t AddressOfEntryPoint;
        uint32_t BaseOfCode;
        uint32_t BaseOfData;

        //
        // NT additional fields.
        //

        uint32_t ImageBase;
        uint32_t SectionAlignment;
        uint32_t FileAlignment;
        uint16_t MajorOperatingSystemVersion;
        uint16_t MinorOperatingSystemVersion;
        uint16_t MajorImageVersion;
        uint16_t MinorImageVersion;
        uint16_t MajorSubsystemVersion;
        uint16_t MinorSubsystemVersion;
        uint32_t Win32VersionValue;
        uint32_t SizeOfImage;
        uint32_t SizeOfHeaders;
        uint32_t CheckSum;
        SubSystem Subsystem;
        DllCharacteristics DllCharacteristics;
        uint32_t SizeOfStackReserve;
        uint32_t SizeOfStackCommit;
        uint32_t SizeOfHeapReserve;
        uint32_t SizeOfHeapCommit;
        uint32_t LoaderFlags;
        uint32_t NumberOfRvaAndSizes;
    };

    struct IMAGE_OPTIONAL_HEADER64
    {
        //
        // Standard fields.
        //

        OptionSignature Magic;
        uint8_t MajorLinkerVersion;
        uint8_t MinorLinkerVersion;
        uint32_t SizeOfCode;
        uint32_t SizeOfInitializedData;
        uint32_t SizeOfUninitializedData;
        uint32_t AddressOfEntryPoint;
        uint32_t BaseOfCode;

        //
        // NT additional fields.
        //

        uint64_t ImageBase;
        uint32_t SectionAlignment;
        uint32_t FileAlignment;
        uint16_t MajorOperatingSystemVersion;
        uint16_t MinorOperatingSystemVersion;
        uint16_t MajorImageVersion;
        uint16_t MinorImageVersion;
        uint16_t MajorSubsystemVersion;
        uint16_t MinorSubsystemVersion;
        uint32_t Win32VersionValue;
        uint32_t SizeOfImage;
        uint32_t SizeOfHeaders;
        uint32_t CheckSum;
        SubSystem Subsystem;
        DllCharacteristics DllCharacteristics;
        uint64_t SizeOfStackReserve;
        uint64_t SizeOfStackCommit;
        uint64_t SizeOfHeapReserve;
        uint64_t SizeOfHeapCommit;
        uint32_t LoaderFlags;
        uint32_t NumberOfRvaAndSizes;
    };

    struct IMAGE_NT_HEADERS 
    {
        uint32_t Signature;                        // should be 0x00004550
        IMAGE_FILE_HEADER FileHeader;
    };

	struct IMAGE_DATA_DIRECTORY
	{
		uint32_t VirtualAddress;
		uint32_t Size;
	};

	enum class DirectoryEntry
    {
        Export                  = 0,        // Export Table
        Import                  = 1,        // Import Table
        Resource                = 2,        // Resource Table
        Exception               = 3,        // Exception Table
        Certificate             = 4,        // Certificate Table
        BaseRelocation          = 5,        // Base Relocation Table
        Debug                   = 6,        // Debug Table
        Architecture            = 7,        // Architecture Specific Data (Reserverd)
        GlobalPointer           = 8,        // The RVA of the value to be stored in the global pointer register
        TLS                     = 9,        // TLS Directory
        LoadConfig              = 10,       // Load Configuration Table
        BoundImport             = 11,       // Bound Import Table
        ImportAddressTable      = 12,       // Import Address Table
        DelayLoadDescriptors    = 13,       // Delay Load Import Descriptors
        CLRRuntime				= 14,       // CLR Runtime descriptor
        Reserved                = 15
    };
    
    enum SectionCharacteristics : uint32_t
    {
        TYPE_NO_PAD                 = 0x00000008,   // The section should not be padded to the next boundary. (obsolete)

        CNT_CODE                    = 0x00000020,   // Section contains code.
        CNT_INITIALIZED_DATA        = 0x00000040,   // Section contains initialized data.
        CNT_UNINITIALIZED_DATA      = 0x00000080,   // Section contains uninitialized data.

        NK_OTHER                    = 0x00000100,   // Reserved
        NK_INFO                     = 0x00000200,   // Objects Only: Section contains comments or some other type of information.
        LNK_REMOVE                  = 0x00000800,   // Objects Only: Section contents will not become part of image.
        LNK_COMDAT                  = 0x00001000,   // Objects Only: Section contents comdat.
        GPREL                       = 0x00008000,   // Section content can be accessed relative to GP
        MEM_16BIT                   = 0x00020000,	// Reserved
        MEM_LOCKED                  = 0x00040000,	// Reserved
        MEM_PRELOAD                 = 0x00080000,	// Reserved

        ALIGN_1BYTES                = 0x00100000,   //
        ALIGN_2BYTES                = 0x00200000,   //
        ALIGN_4BYTES                = 0x00300000,   //
        ALIGN_8BYTES                = 0x00400000,   //
        ALIGN_16BYTES               = 0x00500000,   // Objects Only: Default alignment if no others are specified.
        ALIGN_32BYTES               = 0x00600000,   //
        ALIGN_64BYTES               = 0x00700000,   //
        ALIGN_128BYTES              = 0x00800000,   //
        ALIGN_256BYTES              = 0x00900000,   //
        ALIGN_512BYTES              = 0x00A00000,   //
        ALIGN_1024BYTES             = 0x00B00000,   //
        ALIGN_2048BYTES             = 0x00C00000,   //
        ALIGN_4096BYTES             = 0x00D00000,   //
        ALIGN_8192BYTES             = 0x00E00000,   //
        ALIGN_MASK                  = 0x00F00000,

        LNK_NRELOC_OVFL             = 0x01000000,   // Section contains extended relocations.
        MEM_DISCARDABLE             = 0x02000000,   // Section can be discarded.
        MEM_NOT_CACHED              = 0x04000000,   // Section is not cachable.
        MEM_NOT_PAGED               = 0x08000000,   // Section is not pageable.
        MEM_SHARED                  = 0x10000000,   // Section is shareable.
        MEM_EXECUTE                 = 0x20000000,   // Section is executable.
        MEM_READ                    = 0x40000000,   // Section is readable.
        MEM_WRITE                   = 0x80000000,   // Section is writeable.

        // TLS Characteristic Flags
        SCALE_INDEX                 = 0x00000001,   // Tls index is scaled
    };
	
    struct IMAGE_SECTION_HEADER
    {
        char Name[8];
        uint32_t VirtualSize;
        uint32_t VirtualAddress;
        uint32_t SizeOfRawData;
        uint32_t PointerToRawData;
        uint32_t PointerToRelocations;		// Zero for Image Files
        uint32_t PointerToLinenumbers;		// Zero for Image Files
        uint16_t NumberOfRelocations;		// Zero for Image Files
        uint16_t NumberOfLinenumbers;		// Zero for Image Files
        SectionCharacteristics Characteristics;
	};


	struct IMAGE_DELAY_LOAD_DIRECTORY
	{
		uint32_t Attributs;						// Zero
		uint32_t Name;							// RVA of DLL Name
		uint32_t ModuleHandle;					// RVA of Module Handle
		uint32_t DelayImportAddressTable;		// RVA of delay-load import address table
		uint32_t DelayImportNameTable;			// RVA of delay-load name table
		uint32_t BoundDelayImportAddressTable;	// RVA of bound delay-load import address table
		uint32_t BoundDelayImportNameTable;		// RVA of bound delay-load name table
		uint32_t TimeStamp;						// Timestamp of bound DLL 
	};


	struct IMAGE_EXPORT_DIRECTORY 
	{
		uint32_t ExportFlags;		// Reserved
		uint32_t TimeDateStamp;		// Timestamp the export was created
		uint16_t MajorVersion;		
		uint16_t MinorVersion;
		uint32_t Name;				// RVA of dll name
		uint32_t OrdinalBase;		// Starting Ordnial of exports
		uint32_t NumberOfAddress;	// Number of extries in Address table
		uint32_t NumberOfNames;		// Number of extries in Name/Ordinal tables
		uint32_t AddressTable;		// RVA of export address table
		uint32_t NameTable;			// RVA of export name table
		uint32_t OrdinalTable;		// RVA of Ordinal table
	};


	/*
	union IMAGE_EXPORT_ADDRESS_TABLE
	{
		uint32_t Export;			// RVA of exported Symbol
		uint32_t Forwarder;			// RVA to string located in the area pointed to by the export direcotry 
	};

	struct IMAGE_EXPORT_NAME_TABLE
	{
		uint32_t Name;				// RVA of function name string
	};

	struct IMAGE_EXPORT_ORDINAL_TABLE
	{
		uint16_t OldinalIndex;		// Index into the Address table, Pos + OrdinalBase in the Ordinal
	};
	*/


	struct IMAGE_IMPORT_ENTRY
	{
		uint32_t   LookupTable;				// RVA of the Import Lookup Table
		uint32_t   TimeDateStamp;           // Time/Date stamp of bound DLL
		uint32_t   ForwarderChain;          // Index to first forwarder reference
		uint32_t   Name;					// Name of the DLL
		uint32_t   ImportAddressTable;		// RVA to IAT (if bound this IAT has actual addresses)
	};


	union IMAGE_IMPORT_LOOKUP_TABLE
	{
		uint32_t  NameTable;		// RVA of a hint/name table entry
		uint32_t  OrdinalNumber;	// 16-bit Ordinal if high bit is set
	};

	struct IMAGE_HINT_NAME_TABLE
	{
		uint16_t	Hint;			// Index into the export name table of the linked DLL
		char	    Name[1];		// Name to import
	};

	struct IMAGE_BASE_RELOCATION 
	{
		uint32_t   PageVirtualAddress;
		uint32_t   SizeOfBlock;

		struct
		{
			uint16_t Offset : 12;
			uint16_t Type : 4;
		} TypeOffset[1];
	};


	enum class IMAGE_REL_BASED
	{
		// Absolute Address, skip
		ABSOLUTE	= 0,	
		
		// The high word of a 32-bit address
		HIGH		= 1,	
		
		// The low word of a 32-bit address
		LOW			= 2,
		
		// A 32-bit Address
		HIGHLOW		= 3,

		// Same as HIGH, Low will follow
		HIGHADJ		= 4,
		
		// A 64-bit address
		DIR64		= 10,
	};

	enum class DebugType : uint32_t
	{
		IMAGE_DEBUG_TYPE_UNKNOWN = 0,					// Unknown
		IMAGE_DEBUG_TYPE_COFF = 1,						// COFF debug information
		IMAGE_DEBUG_TYPE_CODEVIEW = 2,					// Codeview debug infomation (PDB)
		IMAGE_DEBUG_TYPE_FPO = 3,						// Frame pointer ommission data
		IMAGE_DEBUG_TYPE_MISC = 4,						// The location of DBG file. 
		IMAGE_DEBUG_TYPE_EXCEPTION = 5,					// A copy of .pdata section. 
		IMAGE_DEBUG_TYPE_FIXUP = 6,						// Reserverd
		IMAGE_DEBUG_TYPE_OMAP_TO_SRC = 7,				// The mapping from an RVA in image to an RVA in source image. 
		IMAGE_DEBUG_TYPE_OMAP_FROM_SRC = 8,				// The mapping from an RVA in source image to an RVA in image. 
		IMAGE_DEBUG_TYPE_BORLAND = 9,					// Borland
		IMAGE_DEBUG_TYPE_RESERVED10 = 10,				// Reserved
		IMAGE_DEBUG_TYPE_CLSID = 11,					// Reserved 
		IMAGE_DEBUG_TYPE_REPRO = 16,					// PE determinism or reproducibility. 
		IMAGE_DEBUG_TYPE_EX_DLLCHARACTERISTICS = 20,	// Extended DLL characteristics bits.
	};

	struct DEBUG_DIRECOTRY_ENTRY
	{
		uint32_t Characteristics;		// Reserved
		uint32_t DateTimeStamp;			// Date and Time the debug data was created
		uint16_t MajorVersion;			// Verion of Debug Data format
		uint16_t MinorVersion;			// Verion of Debug Data format
		DebugType Type;
		uint32_t SizeOfData;			// Size of the debug data
		uint32_t AddressOfRawData;		// RVA of the debug data
		uint32_t PointerToRawData;		// File Pointer to debug data

	};

	struct TLS_DIRECTORY
	{
		uint32_t RawDataStart;			// The starting address of the TLS template. 
		uint32_t RawDataEnd;			// The address of the last byte of the TLS, except for the zero fill.
		uint32_t AddressOfIndex;		// The location to receive the TLS index,
		uint32_t AddressOfCallbacks;	// The pointer to an array of TLS callback functions
		uint32_t SizeOfZeroFill;		// The size in bytes of the template, beyond the initialized data
		uint32_t Characteristics;		// The four bits [23:20] describe alignment info. Possible values are those defined as IMAGE_SCN_ALIGN_*
	};

}

#pragma pack(pop)
