#include "KernalLib.h"
#include "LowLevel.h"
#include "Thread.h"

struct v86ModeData
{
	uint32_t State;
	bool	 VirtualIF;
	uint32_t ReturnIP;
	uint32_t ReturnSP;
};


uint8_t ReadByte(FullPointer RealModeAddress)
{
	uint8_t * Memory = reinterpret_cast<uint8_t *>((RealModeAddress.Selector << 4) + RealModeAddress.Address);
	return *Memory;
}

void WriteByte(FullPointer RealModeAddress, uint8_t Data)
{
	uint8_t * Memory = reinterpret_cast<uint8_t *>((RealModeAddress.Selector << 4) + RealModeAddress.Address);
	*Memory = Data;
}

uint16_t ReadWord(FullPointer RealModeAddress)
{
	uint16_t * Memory = reinterpret_cast<uint16_t *>((RealModeAddress.Selector << 4) + RealModeAddress.Address);
	return *Memory;
}

void WriteWord(FullPointer RealModeAddress, uint16_t Data)
{
	uint16_t * Memory = reinterpret_cast<uint16_t *>((RealModeAddress.Selector << 4) + RealModeAddress.Address);
	*Memory = Data;
}

uint16_t PopStack(FullPointer &StackPointer)
{
	uint16_t * Memory = reinterpret_cast<uint16_t *>((StackPointer.Selector << 4) + StackPointer.Address);
	
	StackPointer.Address += sizeof(uint16_t);
	
	return *Memory;
}

void PushStack(FullPointer &StackPointer, uint16_t Data)
{
	StackPointer.Address -= sizeof(uint16_t);

	uint16_t * Memory = reinterpret_cast<uint16_t *>((StackPointer.Selector << 4) + StackPointer.Address);

	*Memory = Data;
}

void v86ModeTrap(volatile InterruptContext * Context, uintptr_t * Data)
{
/*
	// Check for GPF fault in V86 mode.
	if(Context->InterruptNumber != 0x0d && (Context->Origin.EFlags & EFlags::Virtual8086Mode) == 0)
		return;
	
	ThreadInformation *CurrentThread = reinterpret_cast<ThreadInformation *>(ReadFS(8));
	//v86ModeData * CurrentModeData = CurrentThread->v86Data;
	
	//KernalPrintf(" %08X %08X %08X\n", Context, Data, &Context);
	//PrintContext(Context);

	bool Address32 = false;
	bool Opcode32 = false;
			
	bool Done = false;
	while(!Done)
	{
		switch(ReadByte(Context->Origin.Return))
		{
			// OP Size Prefix
			case 0x66:
				Opcode32 = true;
				break;

			// Address size Prefix
			case 0x67:
				Address32 = true;
				break;				

			//// REPNE
			//case 0xF3:
			//	break;

			//// REP
			//case 0xf4:
			//	break;
			//
			//case 0xF0:	// LOCK
			//case 0x26:	// ES
			//case 0x2E:	// CS
			//case 0x36:	// SS
			//case 0x3E:	// DS
			//case 0x64:	// FS
			//case 0x65:	// GS
			//	break;
			//					
			//case 0xCC:	// INT 3
			//case 0xCD:	// Int Immediate Byte
			//case 0xCE:	// INTO (int 4)
			//	break;

			//// IRET
			//case 0xCF:
			//	break;

			//// Pushf
			//case 0x9C:
			//	break;

			//// PopF
			//case 0x9D:
			//	break;

			// CLI
			case 0xFA:
				CurrentModeData->VirtualIF = false;
				break;

			// STI
			case 0xFB:
				CurrentModeData->VirtualIF = true;
				break;

			//// IN Immediate Byte
			//case 0xE4:
			//	break;

			//// IN Immediate Byte
			//case 0xE5:
			//	break;

			//// IN No Param
			//case 0xEC:	
			//	break;
			//
			//// IN No Param
			//case 0xED:
			//	break;
			//
			//// OUT Immediate Byte
			//case 0xE6:
			//	break;

			//// OUT Immediate Byte
			//case 0xE7:
			//	break;

			//// OUT No Param
			//case 0xEE:
			//	break;

			//// OUT No Param
			//case 0xEF:
			//	break;

			//// INS No Param
			//case 0x6C: 
			//	break;

			//// INS No Param
			//case 0x6D:
			//	break;

			//// OUTS No Param
			//case 0x6E:
			//	break;

			//// OUTS No Param
			//case 0x6F:
			//	break;

			default:
				KernalPrintf("\nGPF Fault %08X-%02X at: %04X:%04X (%08X) in Virtual Monitor\n", Context->ErrorCode, ReadByte(Context->Origin.Return), Context->Origin.Return.Selector, Context->Origin.Return.Address, (Context->Origin.Return.Selector << 4) + Context->Origin.Return.Address );
				//KernalPrintf(" v86 Stack: %04X:%04X (%08X) %08X, %08X, %08X\n", Context->Origin.Stack.Selector, Context->Origin.Stack.Address, &Stack[0], Stack[-2], Stack[-4], Stack[-6]);
				Done = true;
				break;
		}

		Context->Origin.Return.Address++;
	}

	// Return from the v86 mode
	//Context->Origin.EFlags = (Context->Origin.EFlags & ~EFlags::Virtual8086Mode) | 0x3000 | EFlags::InterruptEnable;
	//Context->Origin.Stack.Selector = Stack[-3];
	//Context->Origin.Stack.Address = Stack[-4];

	//Context->Origin.Return.Selector = Stack[-5];
	//Context->Origin.Return.Address = Stack[-6];

	//Context->Segments.DS = Stack[-3];
	*/

}