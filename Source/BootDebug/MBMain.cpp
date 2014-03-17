#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "RawTerminal.h"
#include "RawMemory.h"
#include "LowLevel.h"
#include "InterruptControler.h"

#include "GDT.h"
#include "IDT.h"

#include "..\StdLib\initterm.h"

#include "MultiBoot.h"
#include "MultiBoot2.h"

//void PrintMemory(void *Address, int Length);
//
//typedef void (*InterruptCallbackPtr)(InterruptContext * Context);
//
//InterruptCallbackPtr InterruptCallback[256]; 
//
//struct BootInfo
//{
//	uint32_t Magic;
//	uint32_t Addr;
//};
//
//extern "C"  void HandleInterrupt(InterruptContext * Context)
//{
//	//printf("Int %02X!\n", Context->InterruptNumber);
//	
//	// Interrupt
//	if(InterruptCallback[Context->InterruptNumber] != 0)
//	{
//		InterruptCallback[Context->InterruptNumber](Context);
//	}	
//
//	//__asm hlt
//	
//	return;
//}
//
//void SystemTrap(InterruptContext * Context)
//{
//	if(Context->InterruptNumber == 0x0d)
//	{
//		printf("GPF Fault at: %08X\n", Context->SourceEIP);
//	}
//	else
//	{	
//		printf("System Error: %02X (%08X)\n", Context->InterruptNumber, Context->ErrorCode);
//	};
//}
//
//unsigned char kbdus[128] =
//{
//    0,  27, '1', '2', '3', '4', '5', '6', '7', '8',	/* 9 */
//  '9', '0', '-', '=', '\b',	/* Backspace */
//  '\t',			/* Tab */
//  'q', 'w', 'e', 'r',	/* 19 */
//  't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',	/* Enter key */
//    0,			/* 29   - Control */
//  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',	/* 39 */
// '\'', '`',   0,		/* Left shift */
// '\\', 'z', 'x', 'c', 'v', 'b', 'n',			/* 49 */
//  'm', ',', '.', '/',   0,				/* Right shift */
//  '*',
//    0,	/* Alt */
//  ' ',	/* Space bar */
//    0,	/* Caps lock */
//    0,	/* 59 - F1 key ... > */
//    0,   0,   0,   0,   0,   0,   0,   0,
//    0,	/* < ... F10 */
//    0,	/* 69 - Num lock*/
//    0,	/* Scroll Lock */
//    0,	/* Home key */
//    0,	/* Up Arrow */
//    0,	/* Page Up */
//  '-',
//    0,	/* Left Arrow */
//    0,
//    0,	/* Right Arrow */
//  '+',
//    0,	/* 79 - End key*/
//    0,	/* Down Arrow */
//    0,	/* Page Down */
//    0,	/* Insert Key */
//    0,	/* Delete Key */
//    0,   0,   0,
//    0,	/* F11 Key */
//    0,	/* F12 Key */
//    0,	/* All other keys are undefined */
//};		
//
//InterruptControler m_InterruptControler;
//
//
//void IRQInterrupt(InterruptContext * Context)
//{
//	//printf("IRQ Called: %02X\n", Context->InterruptNumber - 0x20);
//
//	// Keyboard
//	if(Context->InterruptNumber == 0x021)
//	{
//		unsigned char scancode = InPortb(0x60);		
//		printf("%02X ", scancode);
//		//if(scancode & 0x80)
//		//{
//		//	//printf("!%c", kbdus[scancode & 0x7F]);
//		//}
//		//else
//		//{
//		//	printf("%02X ", scancode);
//		//	//printf("%c", kbdus[scancode]);
//		//}
//	}
//    
//	
//	m_InterruptControler.ClearIRQ(m_InterruptControler.MapIntToIRQ(Context->InterruptNumber));
//
//	
//	if (Context->InterruptNumber - 0x20 >= 8)
//    {
//		OutPortb(0xA0, 0x20);
//    }
//
//	OutPortb(0x20, 0x20);
//}
//
//GDT::GDTEntry GDTTabe[10];
//GDT::GDTPtr GDTPtr;
//
//void BuildGDTEntry(GDT::GDTEntry *Entry, uint32_t Base, uint32_t Limit, uint16_t Attributes, uint8_t Type, uint8_t DPL)
//{
//	Entry->Attributes = Attributes;
//	
//	Entry->BaseLow  = (Base & 0x0000FFFF);
//	Entry->BaseMid  = (Base & 0x00FF0000) >> 16;
//	Entry->BaseHigh = (Base & 0xFF000000) >> 24;
//
//	if(Limit > 0xFFFFF)
//	{
//		Entry->Attributes |= GDT::Granularity4k;
//		Limit = Limit / 0x1000;
//	}
//
//	Entry->LimitLow    = (Limit & 0x0000FFFF);
//	Entry->Attributes |= (Limit & 0x000F0000) >> 8;
//
//	Entry->Attributes |= (Type & GDT::TypeMask);
//	Entry->Attributes |= (DPL << 4);
//
//	if(Limit != 0)
//		 Entry->Attributes |= GDT::NonSystemFlag;
//}
//
//void BuildIDTEntry(IDT::IDTEntry *Entry, uint16_t Segment, uint32_t Offset, uint8_t Attributes, uint8_t Type, uint8_t DPL)
//{
//	Entry->Attributes = Attributes;
//	
//	Entry->OffsetLow  = (Offset & 0x0000FFFF);
//	Entry->OffsetHigh = (Offset & 0xFFFF0000) >> 16;
//	Entry->Segment = Segment;
//
//	Entry->Attributes |= (Type & IDT::TypeMask);
//	Entry->Attributes |= (DPL << 4);
//}
//
//extern "C" void FlushGDT(uint32_t CodeSeg, uint32_t DataSeg);
//
//void BuildGDT()
//{
//	BuildGDTEntry(&GDTTabe[0], 0, 0, 0, 0, 0);
//	BuildGDTEntry(&GDTTabe[1], 0, 0xFFFFFFFF, GDT::Present | GDT::Operand32Bit, GDT::CodeSegment, 0);
//	BuildGDTEntry(&GDTTabe[2], 0, 0xFFFFFFFF, GDT::Present | GDT::Operand32Bit, GDT::DataSegment, 0);
//
//	GDTPtr.Address = (uint32_t)&GDTTabe;
//	GDTPtr.Limit = (sizeof(GDT::GDTEntry) * 3) - 1;
//	
//	ASM_LGDT(GDTPtr);
//	FlushGDT(0x08, 0x10);
//}
//
//extern "C" uint32_t IntTable[];
//
//
//IDT::IDTEntry IDTTabe[256];
//IDT::IDTPtr IDTPtr;
//
//void BuildIDT()
//{
//	memset(IDTTabe, 0, sizeof(IDT::IDTEntry) * 256);
//	
//	int Pos = 0;
//	while(IntTable[Pos] != 0)
//	{
//		BuildIDTEntry(&IDTTabe[Pos], 0x08, IntTable[Pos], IDT::Present, IDT::InteruptGate_32Bit, 0);
//		Pos++;
//	}
//
//	IDTPtr.Address = (uint32_t)&IDTTabe;
//	IDTPtr.Limit = (sizeof(IDT::IDTEntry) * Pos) - 1;
//
//	ASM_LIDT(IDTPtr);
//
//	// Set up the handlers for System errors
//	for(int x = 0; x < 0x20; x++)
//	{
//		InterruptCallback[x] = SystemTrap;
//	}
//
//	// Set up the handlers for IRQs
//	for(int x = 0x20; x < 0x30; x++)
//	{
//		InterruptCallback[x] = IRQInterrupt;
//	}
//}
//
//void DumpGDT(GDT::GDTEntry *Entry)
//{
//	unsigned int Base = 0;
//	unsigned int Limit = 0;
//
//	Base = Entry->BaseLow;
//	Base |= Entry->BaseMid << 16;
//	Base |= Entry->BaseHigh << 24;
//
//	Limit = Entry->LimitLow;
//	Limit |= (Entry->Attributes & GDT::LimitHighMask) << 8;
//
//	if(Entry->Attributes & GDT::Granularity4k)
//	{
//		Limit *= 0x1000;
//		Limit += 0x0FFF;
//	}
//
//	printf("Base:       %08X\n", Base, Entry->BaseLow, Entry->BaseMid, Entry->BaseHigh);
//	printf("Limit:      %08X\n", Limit, Entry->LimitLow, (Entry->Attributes & GDT::LimitHighMask) >> 8);
//	printf("DPL:        ");
//	//printf("Type:       ");
//
//	switch(Entry->Attributes & GDT::DPLMask)
//	{
//		case GDT::DPL0:
//			printf("0\n");
//			break;
//
//		case GDT::DPL1:
//			printf("1\n");
//			break;
//
//		case GDT::DPL2:
//			printf("2\n");
//			break;
//
//		case GDT::DPL3:
//			printf("3\n");
//			break;
//	}
//
//	printf("Attributes: ");
//
//	if(Entry->Attributes & GDT::Accessed)
//		printf("Accessed ");
//
//	if(Entry->Attributes & GDT::ReadWrite)
//		printf("R/W ");
//
//	if(Entry->Attributes & GDT::DirectionConforming)
//		printf("D/C ");
//
//	if(Entry->Attributes & GDT::Executable)
//		printf("Executable ");
//
//	if(Entry->Attributes & GDT::NonSystemFlag)
//		printf("Non-System ");
//
//	if(Entry->Attributes & GDT::Present)
//		printf("Present ");
//
//	if(Entry->Attributes & GDT::Operand32Bit)
//		printf("32Bit ");
//
//	if(Entry->Attributes & GDT::Granularity4k)
//		printf("4k ");
//
//	printf("\n", Entry->Attributes);
//}
//
//
//void MBMain(BootInfo * MBInfo) //unsigned long magic, unsigned long addr)
//{
//	_initterm();
//
//	RawTerminal TestTerm;
//	CurrentTerminal = &TestTerm;
//
//	printf("MultiBoot Magic: %08X, Address: %08X\n", MBInfo->Magic, MBInfo->Addr);
//
//	MulitBoot::Boot_Header * BootHeader = (MulitBoot::Boot_Header *)MBInfo->Addr;
//
//	printf(" Boot Flags: %08X\n", BootHeader->Flags);
//
//	if((BootHeader->Flags & MulitBoot::MemoryInfo) == MulitBoot::MemoryInfo)
//	{
//		printf(" Basic Memory: %08X, %08x\n", BootHeader->Memory_Lower, BootHeader->Memory_Upper);
//	}
//	if((BootHeader->Flags & MulitBoot::BootDeviceInfo) == MulitBoot::BootDeviceInfo)
//	{
//		printf(" Boot Device:  %08X\n", BootHeader->BootDevice);
//	}
//	if((BootHeader->Flags & MulitBoot::CommandLineInfo) == MulitBoot::CommandLineInfo)
//	{
//		printf(" Command Line: %s\n", BootHeader->CommandLine);
//	}
//	if((BootHeader->Flags & MulitBoot::ModuleInfo) == MulitBoot::ModuleInfo)
//	{
//		printf(" Mods\n");
//	}
//	if((BootHeader->Flags & MulitBoot::AOutSymbols) == MulitBoot::AOutSymbols)
//	{
//		printf(" a.out\n");
//	}
//	if((BootHeader->Flags & MulitBoot::ELFSymbols) == MulitBoot::ELFSymbols)
//	{
//		printf(" ELF\n");
//	}
//	if((BootHeader->Flags & MulitBoot::MemoryMapInfo) == MulitBoot::MemoryMapInfo)
//	{
//		printf(" Memory Map: %08X, %08X\n", BootHeader->MemMap_Address, BootHeader->MemMap_Length);
//				
//		intptr_t Base = 0;
//
//		while(Base < (intptr_t) BootHeader->MemMap_Length)
//		{
//			MulitBoot::MemoryMapEntry* MapEntry = (MulitBoot::MemoryMapEntry *)(BootHeader->MemMap_Address + Base);
//
//			char *Type = "Unknown";
//			switch(MapEntry->Type)
//			{
//				case 1:
//					Type = "Available";
//					break;
//				case 2:
//					Type = "Reserved";
//					break;
//				case 3:
//					Type = "ACPI";
//					break;
//				case 4:
//					Type = "NVS";
//					break;
//				case 5:
//					Type = "Bad";
//					break;
//			}
//
//			printf("  Address %08X-%08X, Length %08X-%08X, Type %s\n", MapEntry->BaseAddressHigh, MapEntry->BaseAddressLow, MapEntry->LengthHigh, MapEntry->LengthLow, Type);
//			Base += MapEntry->Size + 4;
//		}
//	}
//	if((BootHeader->Flags & MulitBoot::DriveInfo) == MulitBoot::DriveInfo)
//	{
//		printf(" Drive Info\n");
//	}
//	if((BootHeader->Flags & MulitBoot::ConfigTable) == MulitBoot::ConfigTable)
//	{
//		printf(" Config Table\n");
//	}
//	if((BootHeader->Flags & MulitBoot::BootLoaderName) == MulitBoot::BootLoaderName)
//	{
//		printf(" Boot Loader Name: %s\n", BootHeader->BootLoaderName);
//	}
//	if((BootHeader->Flags & MulitBoot::ApmTable) == MulitBoot::ApmTable)
//	{
//		MulitBoot::Boot_APMTAble * Entry = (MulitBoot::Boot_APMTAble *)BootHeader->APMTable;
//		
//		printf(" APM Table: V: %04X, CS: %04X, CL: %04X, CO: %08X\n", Entry->Version, Entry->CodeSeg, Entry->CodeSegLength, Entry->Offset);
//	}
//	if((BootHeader->Flags & MulitBoot::VBEInfo) == MulitBoot::VBEInfo)
//	{
//		printf(" VBE: Mode %04X, Seg: %04X, Offset: %04X, Length %04X\n", BootHeader->VBE_Mode, BootHeader->VBE_IntefaceSegment, BootHeader->VBE_IntefaceOffset, BootHeader->VBE_IntefaceLength);
//	}
//	if((BootHeader->Flags & MulitBoot::FrameBufferInfo) == MulitBoot::FrameBufferInfo)
//	{
//		printf(" Frame Buffer Info\n");
//		printf("  Addr:%08X, P:%04X, W:%04X, H:%04X BPP:%02X\n", (unsigned int)BootHeader->FrameBuffer_Address, BootHeader->FrameBuffer_Pitch, BootHeader->FrameBuffer_Width, BootHeader->FrameBuffer_Height, BootHeader->FrameBuffer_BPP);
//		printf("  Type: %02X\n", BootHeader->FrameBuffer_Type);
//	}
//
//	BuildGDT();
//	BuildIDT();
//
//	m_InterruptControler.RemapIRQBase(0x20);
//
//
//	//irq_remap();
//
//	ASM_STI;
//
//    //int divisor = 1193180 /1000;       /* Calculate our divisor */
//    //OutPortb(0x43, 0x36);             /* Set our command byte 0x36 */
//    //OutPortb(0x40, divisor & 0xFF);   /* Set low byte of divisor */
//    //OutPortb(0x40, divisor >> 8);     /* Set high byte of divisor */
//
//	//int t = 0;
//
//	//int m = 1/t;
//
//
//
//	GDT::GDTPtr GDTInfo;
//	
//	__asm sidt [GDTInfo]
//
//	PrintMemory((void *)GDTInfo.Address, 32);
//
//	for(;;);
//
//	//for(int x = 0; x < GDTInfo.Limit; )
//	//{
//	//	GDT::GDTEntry *Entry = (GDT::GDTEntry *)(GDTInfo.Address + x);
//	//	
//	//	printf("%04X\n", x);
//	//	DumpGDT(Entry);
//
//	//	x += sizeof(GDT::GDTEntry);
//	//}
//
//	//uint16_t TempSeg;
//	//__asm mov [TempSeg], cs
//	//
//	//printf("CS: %04X\n", TempSeg);
//	//__asm mov [TempSeg], ds
//
//	//printf("DS: %04X\n", TempSeg);
//	
//	//printf("GDT Limit: %04X Base %08X\n", GDTInfo.limit, GDTInfo.base);
//
//	//PrintMemory((void *)GDTInfo.Address, GDTInfo.Limit + 1);
//
//	//__asm sidt [GDTInfo]
//
//	//printf("IDT Limit: %04X Base %08X\n", GDTInfo.limit, GDTInfo.base);
//
//	//PrintMemory((void *)GDTInfo.base, GDTInfo.limit);
//}
//
//void MB2Main(BootInfo * MBInfo) //unsigned long magic, unsigned long addr)
//{
//	_initterm();
//
//	RawTerminal TestTerm;
//	CurrentTerminal = &TestTerm;
//
//	MulitBoot2::Boot_Header * BootHeader = (MulitBoot2::Boot_Header *)MBInfo->Addr;
//
//	printf("MultiBoot2 Magic: %08X, Address: %08X\n", MBInfo->Magic, MBInfo->Addr);
//	printf("Header Size: %04X\n", BootHeader->Size);
//
//	unsigned char *Pos = (unsigned char *)MBInfo->Addr;
//	unsigned char *End = (unsigned char *)MBInfo->Addr + BootHeader->Size;
//	Pos += sizeof(MulitBoot2::Boot_Header);
//
//	while(Pos < End)
//	{
//        while ((int)Pos % 8 != 0)
//            Pos++;
//
//		MulitBoot2::Boot_Entry * CurrentHeader = (MulitBoot2::Boot_Entry *)Pos;
//
//		switch(CurrentHeader->Type)
//		{
//			case MulitBoot2::CommandLine:
//			{
//				MulitBoot2::Boot_CommandLine * Entry = (MulitBoot2::Boot_CommandLine *)Pos;
//				printf(" Command Line: %s\n", Entry->String);
//				break;
//			}
//
//			case MulitBoot2::BootLoaderName:
//			{
//				MulitBoot2::Boot_BootLoaderName * Entry = (MulitBoot2::Boot_BootLoaderName *)Pos;
//				printf(" Boot Loader: %s\n", Entry->String);
//				break;
//			}
//
//			case MulitBoot2::Modules:
//			{
//				MulitBoot2::Boot_Module * Entry = (MulitBoot2::Boot_Module *)Pos;
//				printf(" Module: %s\n", Entry->String);
//				break;
//			}
//
//			case MulitBoot2::BasicMemory:
//			{
//				MulitBoot2::Boot_BasicMemory* Entry = (MulitBoot2::Boot_BasicMemory *)Pos;
//				printf(" Basic Memory: %08X, %08x\n", Entry->Lower, Entry->Upper);
//				break;
//			}
//
//			case MulitBoot2::BIOSBootDevice:
//			{
//				MulitBoot2::Boot_BIOSBootDevice* Entry = (MulitBoot2::Boot_BIOSBootDevice *)Pos;
//				printf(" Boot Dev: %04X/%04x/%04x\n", Entry->BiosDev, Entry->Partition, Entry->SubPartition);
//				break;
//			}
//
//			case MulitBoot2::MemoryMap:
//			{
//				MulitBoot2::Boot_MemoryMap* Entry = (MulitBoot2::Boot_MemoryMap *)Pos;
//				printf(" Memory Map\n");
//				
//				intptr_t Base = sizeof(MulitBoot2::Boot_MemoryMap);
//				
//				while(Base < (intptr_t) Entry->Size)
//				{
//					MulitBoot2::MemoryMapEntry* MapEntry = (MulitBoot2::MemoryMapEntry *)(Pos + Base);
//
//					char *Type = "Unknown";
//					switch(MapEntry->Type)
//					{
//						case 1:
//							Type = "Available";
//							break;
//						case 2:
//							Type = "Reserved";
//							break;
//						case 3:
//							Type = "ACPI";
//							break;
//						case 4:
//							Type = "NVS";
//							break;
//						case 5:
//							Type = "Bad";
//							break;
//					}
//
//					printf("  Address %08X-%08X, Length %08X-%08X, Type %s\n", MapEntry->BaseAddressHigh, MapEntry->BaseAddressLow, MapEntry->LengthHigh, MapEntry->LengthLow, Type);
//					//PrintMemory((unsigned char *)MapEntry, Entry->Entry_Size);
//					Base += Entry->Entry_Size;
//				}
//				
//				break;
//			}
//
//			case MulitBoot2::VBEInfo:
//			{
//				MulitBoot2::Boot_VBEInfo* Entry = (MulitBoot2::Boot_VBEInfo *)Pos;
//				printf(" VBEInfo: Mode %04X, Seg: %04X, Offset: %04X, Length %04X\n", Entry->Mode, Entry->IntefaceSegment, Entry->IntefaceOffset, Entry->IntefaceLength);
//				break;
//			}
//
//			case MulitBoot2::FrameBufferInfo:
//			{
//				MulitBoot2::Boot_FrameBufferInfo* Entry = (MulitBoot2::Boot_FrameBufferInfo *)Pos;
//				printf(" Frame Buffer Info\n");
//				printf("  Addr:%08X, P:%04X, W:%04X, H:%04X BPP:%02X\n", (unsigned int)Entry->Address, Entry->Pitch, Entry->Width, Entry->Height, Entry->BPP);
//				printf("  Type: %02X\n", Entry->FrameBufferType);
//				break;
//			}
//
//			case MulitBoot2::ELFSymbols:
//			{
//				MulitBoot2::Boot_ELFSymbols* Entry = (MulitBoot2::Boot_ELFSymbols *)Pos;
//				printf(" ELF Symbols: %02X\n", Entry->Num);
//				break;
//			}
//
//			case MulitBoot2::APMTable:
//			{
//				MulitBoot2::Boot_APMTAble* Entry = (MulitBoot2::Boot_APMTAble *)Pos;
//				printf(" APM Table: V: %04X, CS: %04X, CL: %04X, CO: %08X\n", Entry->Version, Entry->CodeSeg, Entry->CodeSegLength, Entry->Offset);
//				break;
//			}
//			case MulitBoot2::BootDataEnd:
//				break;
//
//			default:
//				printf(" Entry Type: %02X Size: %04x\n", CurrentHeader->Type, CurrentHeader->Size);
//				PrintMemory(Pos, CurrentHeader->Size);
//				
//		}
//		
//		Pos += CurrentHeader->Size;
//	}
//
//	//gdt_ptr GDTInfo;
//
//	//__asm sgdt [GDTInfo]
//
//	//printf("GDT Limit: %04X Base %08X\n", GDTInfo.limit, GDTInfo.base);
//
//	//PrintMemory((void *)GDTInfo.base, GDTInfo.limit);
//
//	//__asm sidt [GDTInfo]
//
//	//printf("IDT Limit: %04X Base %08X\n", GDTInfo.limit, GDTInfo.base);
//
//	//PrintMemory((void *)GDTInfo.base, GDTInfo.limit);
//}
//
//void main(int argc, char *argv[]);
//
//extern "C" void MultiBootMain(BootInfo * MBInfo)
//{
//	// At this point we are officially alive, but we're still a long ways away from being up and running.
//
//	// Step 1: Set our GDT
//	BuildGDT();
//
//	// Step 2: Set our IDT
//	BuildIDT();
//
//	// Set up the handlers for System errors
//	for(int x = 0; x < 0x20; x++)
//	{
//		InterruptCallback[x] = SystemTrap;
//	}
//
//	// Step 3: Remap IRQs
//	m_InterruptControler.RemapIRQBase(0x20);
//
//	// Set up the handlers for IRQs
//	for(int x = 0x20; x < 0x30; x++)
//	{
//		InterruptCallback[x] = IRQInterrupt;
//	}
//	
//	ASM_STI;
//
//	// Step 5: Set up the execution Environment
//	RawTerminal TestTerm;
//	CurrentTerminal = &TestTerm;
//
//	RawMemory MemorySystem;
//	MemoryMgr = &MemorySystem;
//
//	//_atexit_init();
//	_initterm();
//	
//	// Step 6: Start the full kernel
//	char * CommandLine = nullptr;
//
//	//if((void *)MBInfo->Addr != nullptr)
//	//{
//	//	if(MBInfo->Magic == MulitBoot::BootMagic)
//	//	{
//	//	}
//	//	else if(MBInfo->Magic == MulitBoot2::BootMagic)
//	//	{
//	//	}
//	//}
//
//	//_ConvertCommandLineToArgcArgv(
//
//	//main(0, nullptr);
//
//	//printf("BYe!");
//	
//	for(;;);
//
//	//printf("Woops!");
//}
