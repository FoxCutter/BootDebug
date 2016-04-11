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
        RELOCS_STRIPPED             = 0x0001,  // Relocation info stripped from file.
        EXECUTABLE_IMAGE            = 0x0002,  // File is executable  (i.e. no unresolved externel references).
        LINE_NUMS_STRIPPED          = 0x0004,  // Line nunbers stripped from file.
        LOCAL_SYMS_STRIPPED         = 0x0008,  // Local symbols stripped from file.
        AGGRESIVE_WS_TRIM           = 0x0010,  // Agressively trim working set
        LARGE_ADDRESS_AWARE         = 0x0020,  // App can handle >2gb addresses
        BYTES_REVERSED_LO           = 0x0080,  // Bytes of machine word are reversed.
        S32BIT_MACHINE              = 0x0100,  // 32 bit word machine.
        DEBUG_STRIPPED              = 0x0200,  // Debugging info stripped from file in .DBG file
        REMOVABLE_RUN_FROM_SWAP     = 0x0400,  // If Image is on removable media, copy and run from the swap file.
        NET_RUN_FROM_SWAP           = 0x0800,  // If Image is on Net, copy and run from the swap file.
        SYSTEM                      = 0x1000,  // System File.
        DLL                         = 0x2000,  // File is a DLL.
        UP_SYSTEM_ONLY              = 0x4000,  // File should only be run on a UP machine
        BYTES_REVERSED_HI           = 0x8000,  // Bytes of machine word are reversed.
    };

    enum class ImageFileMachine : uint16_t
    {
        UNKNOWN     = 0,
        I386        = 0x014c,       // Intel 386.
        R3000       = 0x0162,       // MIPS little-endian, 0x160 big-endian
        R4000       = 0x0166,       // MIPS little-endian
        R10000      = 0x0168,       // MIPS little-endian
        WCEMIPSV2   = 0x0169,       // MIPS little-endian WCE v2
        ALPHA       = 0x0184,       // Alpha_AXP
        SH3         = 0x01a2,       // SH3 little-endian
        SH3DSP      = 0x01a3,       
        SH3E        = 0x01a4,       // SH3E little-endian
        SH4         = 0x01a6,       // SH4 little-endian
        SH5         = 0x01a8,       // SH5
        ARM         = 0x01c0,       // ARM Little-Endian
        THUMB       = 0x01c2,       // ARM Thumb/Thumb-2 Little-Endian
        ARMNT       = 0x01c4,       // ARM Thumb-2 Little-Endian
        AM33        = 0x01d3,       
        POWERPC     = 0x01F0,       // IBM PowerPC Little-Endian
        POWERPCFP   = 0x01f1,       
        IA64        = 0x0200,       // Intel 64
        MIPS16      = 0x0266,       // MIPS
        ALPHA64     = 0x0284,       // ALPHA64
        MIPSFPU     = 0x0366,       // MIPS
        MIPSFPU16   = 0x0466,       // MIPS
        AXP64       = ALPHA64,
        TRICORE     = 0x0520,       // Infineon
        CEF         = 0x0CEF,       
        EBC         = 0x0EBC,       // EFI Byte Code
        AMD64       = 0x8664,       // AMD64 (K8)
        M32R        = 0x9041,       // M32R little-endian
        CEE         = 0xC0EE,   
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

    struct IMAGE_DATA_DIRECTORY
    {
       uint32_t VirtualAddress;
       uint32_t Size;
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
        EFI_APPLICATION             = 10,   //
        EFI_BOOT_SERVICE_DRIVER     = 11,   //
        EFI_RUNTIME_DRIVER          = 12,   //
        EFI_ROM                     = 13,
        XBOX                        = 14,
        WINDOWS_BOOT_APPLICATION    = 16,
    };

    enum DllCharacteristics : uint16_t
    {
        //PROCESS_INIT          = 0x0001,   // Reserved.
        //PROCESS_TERM          = 0x0002,   // Reserved.
        //THREAD_INIT           = 0x0004,   // Reserved.
        //THREAD_TERM           = 0x0008,   // Reserved.
        HIGH_ENTROPY_VA         = 0x0020,   // Image can handle a high entropy 64-bit virtual address space.
        DYNAMIC_BASE            = 0x0040,   // DLL can move.
        FORCE_INTEGRITY         = 0x0080,   // Code Integrity Image
        NX_COMPAT               = 0x0100,   // Image is NX compatible
        NO_ISOLATION            = 0x0200,   // Image understands isolation and doesn't want it
        NO_SEH                  = 0x0400,   // Image does not use SEH.  No SE handler may reside in this image
        NO_BIND                 = 0x0800,   // Do not bind this image.
        APPCONTAINER            = 0x1000,   // Image should execute in an AppContainer
        WDM_DRIVER              = 0x2000,   // Driver uses WDM model
        //                      = 0x4000,   // Reserved.
        TERMINAL_SERVER_AWARE   = 0x8000,
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

		IMAGE_DATA_DIRECTORY DataDirectory[1];
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

		IMAGE_DATA_DIRECTORY DataDirectory[1];

    };

    struct IMAGE_NT_HEADERS 
    {
        uint32_t Signature;                        // should be 0x00004550
        IMAGE_FILE_HEADER FileHeader;
    };

    enum class DirectoryEntry
    {
        Export                  = 0,        // Export Directory
        Import                  = 1,        // Import Directory
        Resource                = 2,        // Resource Directory
        Exception               = 3,        // Exception Directory
        Security                = 4,        // Security Directory
        BaseRelocation          = 5,        // Base Relocation Table
        Debug                   = 6,        // Debug Directory
        //Copyright             = 7,        // (X86 usage)
        Architecture            = 7,        // Architecture Specific Data
        GlobalPointer           = 8,        // RVA of GP
        TLS                     = 9,        // TLS Directory
        LoadConfig              = 10,       // Load Configuration Directory
        BoundImport             = 11,       // Bound Import Directory in headers
        ImportAddressTable      = 12,       // Import Address Table
        DelayLoadDescriptors    = 13,       // Delay Load Import Descriptors
        ComDescriptors          = 14,       // COM Runtime descriptor
        Reserved                = 15
    };
    
    enum SectionCharacteristics : uint32_t
    {
        //TYPE_REG                  = 0x00000000,   // Reserved.
        //TYPE_DSECT                = 0x00000001,   // Reserved.
        //TYPE_NOLOAD               = 0x00000002,   // Reserved.
        //TYPE_GROUP                = 0x00000004,   // Reserved.
        TYPE_NO_PAD                 = 0x00000008,   // Reserved.
        //TYPE_COPY                 = 0x00000010,   // Reserved.

        CNT_CODE                    = 0x00000020,   // Section contains code.
        CNT_INITIALIZED_DATA        = 0x00000040,   // Section contains initialized data.
        CNT_UNINITIALIZED_DATA      = 0x00000080,   // Section contains uninitialized data.

        NK_OTHER                    = 0x00000100,   // Reserved.
        NK_INFO                     = 0x00000200,   // Section contains comments or some other type of information.
        //TYPE_OVER                 = 0x00000400,   // Reserved.
        LNK_REMOVE                  = 0x00000800,   // Section contents will not become part of image.
        LNK_COMDAT                  = 0x00001000,   // Section contents comdat.
        //                          = 0x00002000,   // Reserved.
        //MEM_PROTECTED             = 0x00004000,   // Obsolete
        NO_DEFER_SPEC_EXC           = 0x00004000,   // Reset speculative exceptions handling bits in the TLB entries for this section.
        GPREL                       = 0x00008000,   // Section content can be accessed relative to GP
        MEM_FARDATA                 = 0x00008000,   
        //MEM_SYSHEAP               = 0x00010000,   // Obsolete
        MEM_PURGEABLE               = 0x00020000,   
        MEM_16BIT                   = 0x00020000,
        MEM_LOCKED                  = 0x00040000,
        MEM_PRELOAD                 = 0x00080000,

        ALIGN_1BYTES                = 0x00100000,   //
        ALIGN_2BYTES                = 0x00200000,   //
        ALIGN_4BYTES                = 0x00300000,   //
        ALIGN_8BYTES                = 0x00400000,   //
        ALIGN_16BYTES               = 0x00500000,   // Default alignment if no others are specified.
        ALIGN_32BYTES               = 0x00600000,   //
        ALIGN_64BYTES               = 0x00700000,   //
        ALIGN_128BYTES              = 0x00800000,   //
        ALIGN_256BYTES              = 0x00900000,   //
        ALIGN_512BYTES              = 0x00A00000,   //
        ALIGN_1024BYTES             = 0x00B00000,   //
        ALIGN_2048BYTES             = 0x00C00000,   //
        ALIGN_4096BYTES             = 0x00D00000,   //
        ALIGN_8192BYTES             = 0x00E00000,   //
        //                          = 0x00F00000,   // Obsolete
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
        uint32_t PointerToRelocations;
        uint32_t PointerToLinenumbers;
        uint16_t NumberOfRelocations;
        uint16_t NumberOfLinenumbers;
        SectionCharacteristics Characteristics;
	};

	struct IMAGE_EXPORT_DIRECTORY 
	{
		uint32_t Characteristics;
		uint32_t TimeDateStamp;
		uint16_t MajorVersion;
		uint16_t MinorVersion;
		uint32_t Name;
		uint32_t Base;
		uint32_t NumberOfFunctions;
		uint32_t NumberOfNames;
		uint32_t AddressOfFunctions;
		uint32_t AddressOfNames;
		uint32_t AddressOfNameOrdinal;
	};

	struct IMAGE_IMPORT_DESCRIPTOR 
	{
		union 
		{
			uint32_t   Characteristics;            // 0 for terminating null import descriptor
			uint32_t   OriginalFirstThunk;         // RVA to original unbound IAT (PIMAGE_THUNK_DATA)
		};
		uint32_t   TimeDateStamp;                  // 0 if not bound,
		uint32_t   ForwarderChain;                 // -1 if no forwarders
		uint32_t   Name;
		uint32_t   FirstThunk;                     // RVA to IAT (if bound this IAT has actual addresses)
	};

	struct IMAGE_THUNK_DATA 
	{
		union 
		{
			uint32_t ForwarderString;      // PBYTE 
			uint32_t Function;             // PDWORD
			uint32_t Ordinal;
			uint32_t AddressOfData;        // PIMAGE_IMPORT_BY_NAME
		};
	};

	struct IMAGE_IMPORT_BY_NAME 
	{
		uint16_t	Hint;
		char	    Name[1];
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
}

#pragma pack(pop)
