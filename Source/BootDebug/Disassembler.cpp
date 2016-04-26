#include "Disassembler.h"
#include "OpcodeTables.h"

#include <stdio.h>

bool IsPrefix(ASMParseTypes ParseType)
{
	return  ParseType == ParsePrefix || 
			ParseType == ParseSegmentPrefix || 
			ParseType == ParseAdSizePrefix || 
			ParseType == ParseOpSizePrefix;
}

bool HasNoParams(ASMParseTypes ParseType)
{
	return  ParseType == ParseNoParam || 
			ParseType == ParseNoParamByte;
}

bool HasModRM(ASMParseTypes ParseType)
{
	return	ParseType == ParseModRMByte || 
			ParseType == ParseModRM || 
			ParseType == ParseImmediateByte || 
			ParseType == ParseImmediate || 
			ParseType == ParseOpImmediateByte;
}

bool HasImmediate(ASMParseTypes ParseType)
{
	return	ParseType == ParseOnlyImmediateByte || 
			ParseType == ParseOnlyImmediateWord || 
			ParseType == ParseOnlyImmediate || 
			ParseType == ParseModRMPlusByte ||
			ParseType == ParseModRMPlus ||
			ParseType == ParseImmediateByte || 
			ParseType == ParseOpImmediateByte ||
			ParseType == ParseImmediate || 
			ParseType == ParseOnlyFarAddress;
}

bool HasOnlyImmediate(ASMParseTypes ParseType)
{
	return	ParseType == ParseOnlyImmediateByte || 
			ParseType == ParseOnlyImmediateWord || 
			ParseType == ParseOnlyImmediate || 
			ParseType == ParseOnlyFarAddress;
}

bool IsByteAddressSize(ASMParseTypes ParseType)
{
	return	ParseType == ParseNoParamByte ||
			ParseType == ParseModRMPlusByte ||
			ParseType == ParseModRMByte || 
			ParseType == ParseOnlyImmediateByte || 
			ParseType == ParseImmediateByte;
}

bool IsDynamicAddressSize(ASMParseTypes ParseType)
{
	return	ParseType == ParseNoParam ||
			ParseType == ParseModRMPlus ||
			ParseType == ParseModRM || 
			ParseType == ParseOnlyImmediate || 
			ParseType == ParseImmediate;
}

// Maps to the ASMParamTypes entries starting form ParamRegDirect.
char * RegName[] = 
{
	"", 
	"AL",
	"BL",
	"CL",
	"DL",
	"AH",
	"BH",
	"CH",
	"DH",

	"#AX",
	"#BX",
	"#CX",
	"#DX",
	"#SP",
	"#BP",
	"#SI",
	"#DI",

	"R8!",
	"R9!",
	"R10!",
	"R11!",
	"R12!",
	"R13!",
	"R14!",
	"R15!",

	"ES",
	"CS",
	"SS",
	"DS",
	"FS",
	"GS",
};

char * ModRM16[8] = 
{ 
	"BX+SI",
	"BX+DI",
	"BP+SI",
	"BP+DI",
	"SI",
	"DI",
	"BP",
	"BX",
};

struct Disassembler::OpcodeData
{
	// The full raw code of the opcode
	uint8_t Code[16];
	uint8_t Length;
	
	char * Name;

	uint8_t ParamCount;
	ASMParamTypes ParamsFlags[3];
	uint32_t Params[3];
	uint32_t Displacement;

	uint32_t SegmentPrefix;
	uint32_t Prefixs[6];
	uint32_t PrefixCount;

	// The size of the Operator we are using (this changes the opcode)
	uint32_t OpSize; 
	
	// The address size we are using (this changes how the ModR/M + SIB bytes are treated)
	uint32_t AddressSize;
	
	// The size of the data being pointed to (and which bank of registers to use)
	uint32_t PointerSize;
};

Disassembler::Disassembler(void)
{
}


Disassembler::~Disassembler(void)
{
}


uint32_t Disassembler::Disassamble(intptr_t *Address, uint32_t Length, uint32_t OpSize, uint32_t AddressSize)
{
	uint8_t *Data = reinterpret_cast<uint8_t *>(Address);
	
	printf("Address: %08X\n", Data);

	uint32_t Count = 0;
	bool REX = false;
	if(OpSize == 64)
		REX = true;

	while(Count < Length)
	{		
		// Fetch next Opcode
		OpcodeData Opcode = ReadOpcode(Data, OpSize, AddressSize, REX);
		
		// print the opcode
		PrintOpcode(Data, Opcode);
		
		Count += Opcode.Length;
		Data += Opcode.Length;
	};

	return Count;
}

void PrintOffset(int32_t Value, uint32_t Size)
{
	//if(Size == 8)
	//{
	//	if(Value & 0x80)
	//		printf("-%02X", -Value);
	//	else
	//		printf("+%02X", Value);
	//}
	//else if(Size == 16)
	//{
	//	if(Value & 0x8000)
	//		printf("-%04X", -Value);
	//	else
	//		printf("+%04X", Value);
	//}
	//else if(Size == 32)
	{
		if(Value & 0x80000000)
			printf("-0%Xh", -Value);
		else
			printf("+0%Xh", Value);
	}
}

void PrintReg(uint32_t Reg, uint32_t OpSize)
{
	if(Reg <= ParamRegDirect)
		return;

	char *Name = RegName[Reg - ParamRegDirect];

	while(*Name != 0)
	{
		if(*Name == '#')
		{
			if(OpSize == 32)
				printf("E", *Name);		

			else if(OpSize == 64)
				printf("R", *Name);		
		}
		else if(*Name == '!')
		{
			if(OpSize == 8)
				printf("L", *Name);		

			else if(OpSize == 16)
				printf("W", *Name);		

			else if(OpSize == 32)
				printf("D", *Name);		
		}
		else
		{
			printf("%c", *Name);
		}

		Name++;
	};
}

uint32_t ConvertReg(uint32_t Reg, uint32_t AdSize)
{
	switch(Reg)
	{
		case 0:
			return (AdSize == 8) ? RegAL : RegAX;

		case 1:
			return (AdSize == 8) ? RegCL : RegCX;

		case 2:
			return (AdSize == 8) ? RegDL : RegDX;

		case 3:
			return (AdSize == 8) ? RegBL : RegBX;

		case 4:
			return (AdSize == 8) ? RegAH : RegSP;

		case 5:
			return (AdSize == 8) ? RegCH : RegBP;

		case 6:
			return (AdSize == 8) ? RegDH : RegSI;

		case 7:
			return (AdSize == 8) ? RegBH : RegDI;


		case 8:
			return Reg8r;

		case 9:
			return Reg9r;

		case 10:
			return Reg10r;

		case 11:
			return Reg11r;

		case 12:
			return Reg12r;

		case 13:
			return Reg13r;

		case 14:
			return Reg14r;

		case 15:
			return Reg15r;
	}

	return 0;
}


void PrintModRM(uint32_t ModData, int32_t Displacement, uint32_t PointerSize, uint32_t AdSize)
{
	bool RexB = (ModData & 0x0100) == 0x0100;
	bool RexX = (ModData & 0x0200) == 0x0200;
	bool RexR = (ModData & 0x0400) == 0x0400;
	bool RexW = (ModData & 0x0800) == 0x0800;
							
	uint8_t Mod = (ModData & 0xC0) >> 6;
	uint8_t RM  = (ModData & 0x07);

	RM  += RexB << 3;

	bool SIB = (AdSize != 16 && Mod != 3 && RM == 4);
	uint32_t DisplacmentSize = 0;

	if(Mod != 3)
	{
		switch(PointerSize)
		{
			case 8:
				printf("byte ");
				break;

			case 16:
				printf("word ");
				break;

			case 32:
				printf("dword ");
				break;

			case 64:
				printf("qword ");
				break;
		}
		
		printf("[");
	}

	if(!SIB)
	{
		if(AdSize != 16)
		{
			if(Mod == 0 && RM == 5)
				PrintOffset(Displacement, 32);
			
			else
			{
				PrintReg(ConvertReg(RM, AdSize), PointerSize);

				if(Mod == 1)
					DisplacmentSize = 8;

				if(Mod == 2)
					DisplacmentSize = 32;
			}
		}
		else
		{
			if(Mod == 0 && RM == 6)
				PrintOffset(Displacement, 16);

			else
			{
				if(Mod != 3)
					printf("%s", ModRM16[RM]);
				else
					PrintReg(ConvertReg(RM, AdSize), PointerSize);

				if(Mod == 1)
					DisplacmentSize = 8;

				if(Mod == 2)
					DisplacmentSize = 16;
			}
		}
	}
	else
	{
		uint8_t Scale = (ModData & 0xC000) >> 14;
		uint8_t Index = (ModData & 0x3800) >> 11;
		uint8_t Base  = (ModData & 0x0700) >> 8;
		
		Index += RexX << 3;
		Base  += RexB << 3;

		if(!(Mod == 0 && Base == 5))
		{
			PrintReg(ConvertReg(Base, AdSize), AdSize);
			printf(".");
		}
		else
		{
			DisplacmentSize = 4;
		}

		if(Index != 4)
		{
			printf("+");
			PrintReg(ConvertReg(Index, AdSize), AdSize);

			switch(Scale)
			{
			case 1:
				printf("*2");
				break;

			case 2:
				printf("*4");
				break;

			case 3:
				printf("*8");
				break;
			}
		}

		if(Mod == 1)
			DisplacmentSize = 1;

		else if(Mod == 2)
			DisplacmentSize = 4;
	}

	if(DisplacmentSize != 0)
		PrintOffset(Displacement, DisplacmentSize);

	if(Mod != 3)
		printf("]");

}

void Disassembler::PrintOpcode(uint8_t *Address, OpcodeData &Data)
{
	printf("%08X ", Address);
	for(int x = 0; x < 8; x++)
	{
		if(x < Data.Length)
			printf("%02X ", Data.Code[x]);
		else
			printf("   ");
	}

	for(unsigned int x = 0; x < Data.PrefixCount; x++)
	{
		printf("%s\t ", SingleByteOpCodes[Data.Prefixs[x]].Name);
	}
	
	if(Data.Name[0] == 0x00)
		printf("--- \t ", Data.Name[0]);
	else if(Data.Name[0] < 0x20)
		printf("-%02X-\t ", Data.Name[0]);
	else
		printf("%s\t ", Data.Name);

	for(int x = 0; x < Data.ParamCount; x++)
	{
		if(x != 0)
			printf(", ");
		
		switch(Data.ParamsFlags[x])
		{
			case ParamOne:
				printf("1");
				break;

			case ParamRegDirect:
				PrintReg(Data.Params[x], Data.PointerSize);		
				break;

			case ParamRegSeg:
				PrintReg(SegES + Data.Params[x], Data.OpSize);		
				break;

			case ParamModRM:				
				PrintModRM(Data.Params[x], Data.Displacement, Data.PointerSize, Data.AddressSize);
				break;

			case ParamRegOpSize:				
				PrintReg(ConvertReg(Data.Params[x], Data.OpSize), Data.OpSize);
				break;

			case ParamReg:				
				PrintReg(ConvertReg(Data.Params[x], Data.PointerSize), Data.PointerSize);
				break;

			case ParamImmediate:
				printf("0%Xh", Data.Params[x]);
				break;

			case ParamFarAddress:
				printf("FAR %X:%X", Data.SegmentPrefix, Data.Params[x]);
				
				if(Data.AddressSize == 16)
					printf(" (%X)", (Data.SegmentPrefix << 4) + Data.Params[x]);

				break;

			case ParamAddress:
				printf("[0%Xh]", Data.Params[x]);
				break;

			case ParamOffset:
				PrintOffset(Data.Params[x], Data.AddressSize);

				if(Data.AddressSize != 16)
					printf(" (%X)", Address + Data.Length + Data.Params[x]);
				else
					printf(" (%X)", (uint32_t)(Address + Data.Length + Data.Params[x]) & 0xFFFFF);
				break;
		};		
	}

	printf("\n");
}

Disassembler::OpcodeData Disassembler::ReadOpcode(uint8_t *Address, uint32_t OpSize, uint32_t AddressSize, bool REX)
{
	OpcodeData Ret;
	Ret.Length = 0;
	Ret.OpSize = OpSize;
	Ret.AddressSize = AddressSize;
	Ret.SegmentPrefix = 0;
	Ret.PrefixCount = 0;
	Ret.Displacement = 0;
	Ret.ParamCount = 0;

	// State list
	// 00: Parse Prefixes
	// 01: Parse Opcode
	// 02: Parse ModR/M
	// 03: Parse SIB
	// 04: Parse Displacement
	// 05: Parse Immediate
	// FF: Parsing Complete
	int State = 0;

	// 0: One Byte Opcodes
	// 1: Two Byte Opcodes
	int CurrentTable = 0;

	uint32_t SIBModRM = 0;
	uint16_t Reg = 0;
	uint32_t Immediate = 0;
	uint8_t ImmediateSize = 0;
	

	uint8_t Mod;
	uint8_t RM;
	uint8_t Scale;
	uint8_t Index;
	uint8_t Base;

	bool REXPrefix = false;
	bool RexB = false;
	bool RexX = false;
	bool RexR = false;
	bool RexW = false;


	const ASMTableEntry *OpEntry = nullptr;
	const ASMTableEntry *GroupBlock = nullptr;
	const ASMTableEntry *GroupEntry = nullptr;
	uint8_t GroupIndex = 0;

	do
	{
		uint8_t data = Address[Ret.Length];
		Ret.Code[Ret.Length] = data;
		Ret.Length++;
		
		switch(State)
		{
			case 0: // Parse Prefix
			case 1: // Parse Opcode
				{
					if(REX)
					{
						if(data >= 0x40 && data <= 0x4f)
						{
							REXPrefix = true;

							RexB = (data & 0x01) == 0x01;
							RexX = (data & 0x02) == 0x02;
							RexR = (data & 0x04) == 0x04;
							RexW = (data & 0x08) == 0x08;

							if(RexW)
								Ret.OpSize = 64;
							else
								Ret.OpSize = 32;

							SIBModRM += data << 16;

							continue;
						}
					}

					if(CurrentTable == 0)
						OpEntry = &SingleByteOpCodes[data];
					
					else if (CurrentTable == 1)
						OpEntry = &TwoByteOpCodes[data];
						
					Ret.Name = OpEntry->Name;

					// Check for an Opcode Group
					if(OpEntry->Name[0] != 0 && OpEntry->Name[0] < 0x20)
					{
						GroupIndex = OpEntry->Name[0] - 1;
						GroupBlock = OpcodeGroups[GroupIndex];						
					}

					if(OpEntry->ParseType == ParseTwoBytePrefix)
					{
						CurrentTable = 1;
					}
					else if(IsPrefix(OpEntry->ParseType))
					{
						State = 1;
						switch(OpEntry->ParseType)
						{
							case ParseOpSizePrefix:
								if(Ret.OpSize == 32)
									Ret.OpSize = 16;
								else
									Ret.OpSize = 32;
								break;
							
							case ParseAdSizePrefix:
								if(Ret.AddressSize == 32)
									Ret.AddressSize = 16;
								else
									Ret.AddressSize = 32;
								break;

							case ParseSegmentPrefix:
								Ret.SegmentPrefix = OpEntry->Params[0];
								break;

							case ParsePrefix:
								Ret.Prefixs[Ret.ParamCount] = data;
								Ret.PrefixCount++;
								break;
						}
					}
					else if(HasNoParams(OpEntry->ParseType))
					{						
						State = 0xFF;
					}
					else if(HasOnlyImmediate(OpEntry->ParseType))
					{
						if(REX && !RexW)
							Ret.OpSize = 32;

						State = 5;
					}
					else 
					{						
						if(REX && !RexW)
							Ret.OpSize = 32;

						State = 2;
					}

					if(IsByteAddressSize(OpEntry->ParseType))
						Ret.PointerSize = 8;
					else
						Ret.PointerSize = Ret.OpSize;
				}
				break;

			case 2: // Parse ModR/M
				Mod = (data & 0xC0) >> 6;
				Reg = (data & 0x38) >> 3;
				RM  = (data & 0x07);
				
				if(REXPrefix)
				{
					Reg += RexR << 3;
					RM  += RexB << 3;
				}
				
				SIBModRM = data & 0xC7;

				/*
				if(Ret.AddressSize == 32 || Ret.AddressSize == 64)
				{
					if(Mod == 0)
					{
						if(RM == 4)
						{
							State = 3;	// SIB
						}
						else if(RM == 5)
						{
							ImmediateSize = 4;
							State = 4;	// Displacement
						}
					}
					if(Mod == 1)
					{
						ImmediateSize = 1;
						if(RM == 4)
							State = 3;	// SIB

						else
							State = 4;	// Displacement
					}
					else if(Mod == 2)
					{
						ImmediateSize = 4;

						if(RM == 4)
							State = 3; // SIB

						else
							State = 4; // Displacement
					}
				}
				else
				{
					if(Mod == 0)
					{
						if(RM == 6)
						{
							ImmediateSize = 2;
							State = 4;	// Displacement
						}
					}
					if(Mod == 1)
					{
						ImmediateSize = 1;
						State = 4;	// Displacement
					}
					else if(Mod == 2)
					{
						ImmediateSize = 2;
						State = 4;	// Displacement
					}
				}
				*/

				if(Mod == 0)
				{
					if(Ret.AddressSize == 16 && RM == 6)
					{
						ImmediateSize = 2;
						State = 4;	// Displacement
					}
					else if(Ret.AddressSize != 16 && RM == 5)
					{
						ImmediateSize = 4;
						State = 4;	// Displacement
					}
				}
				else if (Mod == 1)
				{
					ImmediateSize = 1;
					State = 4; // Displacement
				}
				else if (Mod == 2)
				{
					ImmediateSize = (Ret.AddressSize == 16 ? 2 : 4);
					State = 4; // Displacement
				}

				if(Ret.AddressSize != 16 && Mod != 3 && RM == 4)
				{
					State = 3;	// SIB
				}
				
				if(State == 2)				
					State = 5;	// Parse Immediate

				if(GroupBlock != nullptr)
					GroupEntry = &GroupBlock[Reg];

				break;

			case 3: // Parse SIB
				Scale = (data & 0xC0) >> 6;
				Index = (data & 0x38) >> 3;
				Base  = (data & 0x07);
				
				if(REXPrefix)
				{
					Index += RexX << 3;
					Base  += RexB << 3;
				}

				ImmediateSize = 0;
				if(Mod == 0 && Base == 5)
					ImmediateSize = 4;

				else if(Mod == 1)
					ImmediateSize = 1;

				else if(Mod == 2)
					ImmediateSize = 4;
				
				SIBModRM += data << 8;
				
				if(ImmediateSize != 0)
					State = 4; // Parse Displacement
				else
					State = 5;	// Parse Immediate
				break;

			case 4: // Parse Displacement
				Ret.Displacement = 0;
				Ret.Displacement = data;

				if(ImmediateSize > 4)
					ImmediateSize = 4;

				for(int x = 1; x < ImmediateSize; x++)
				{
					data = Address[Ret.Length];
					Ret.Code[Ret.Length] = data;
					Ret.Length++;

					Ret.Displacement += data << (8 * x);
				}
				
				if(data & 0x80)
				{
					for(int x = 0; x < 4 - ImmediateSize; x++)
					{
						Ret.Displacement |= 0xFF << (8 * (ImmediateSize + x));
					}
				}


				State = 5;	// Parse Immediate
				break;				

			case 5: // Parse Immediate
				if(OpEntry->ParseType == ParseImmediate || OpEntry->ParseType == ParseOnlyImmediate || OpEntry->ParseType == ParseModRMPlus || OpEntry->ParseType == ParseOnlyFarAddress)
				{
					if(OpEntry->Params[0] == ParamAddress || OpEntry->Params[1] == ParamAddress)
						ImmediateSize = Ret.AddressSize / 8;
					else
						ImmediateSize = Ret.OpSize / 8;
				}
				else if (OpEntry->ParseType == ParseOnlyImmediateWord)
				{
					ImmediateSize = 2;
				}
				else if (OpEntry->ParseType == ParseImmediateByte || OpEntry->ParseType == ParseOnlyImmediateByte || OpEntry->ParseType == ParseOpImmediateByte || OpEntry->ParseType == ParseModRMPlusByte)
				{
					// Byte pointer to an address
					if(OpEntry->Params[0] == ParamAddress || OpEntry->Params[1] == ParamAddress)
						ImmediateSize = Ret.AddressSize / 8;
					else
						ImmediateSize = 1;
				}
				else
				{
					State = 0xFF;
					Ret.Length--;
					break;
				}

				if(ImmediateSize > 4)
					ImmediateSize = 4;
				
				Immediate = 0;
				Immediate = data;

				for(int x = 1; x < ImmediateSize; x++)
				{
					data = Address[Ret.Length];
					Ret.Code[Ret.Length] = data;
					Ret.Length++;

					Immediate += data << (8 * x);
				}
				
				if(OpEntry->ParseType == ParseOnlyFarAddress)
				{
					Ret.SegmentPrefix = 0;

					for(int x = 0; x < 2; x++)
					{
						data = Address[Ret.Length];
						Ret.Code[Ret.Length] = data;
						Ret.Length++;

						Ret.SegmentPrefix += data << (8 * x);
					}
				}

				State = 0xFF;

				break;				
		}
	} while(State != 0xFF && Ret.Length != 16);

	if(State != 0xFF)
		return Ret;
	
	if(GroupEntry != nullptr && GroupEntry->Name != nullptr)
	{		
		Ret.Name = GroupEntry->Name;

		if(	GroupEntry->ParseType != ParseParamsFromParent)
			OpEntry = GroupEntry;
	}

	Ret.ParamCount = OpEntry->ParamCount;

	for(int x = 0; x < OpEntry->ParamCount; x++)
	{
		Ret.ParamsFlags[x] = OpEntry->Params[x];
		if(Ret.ParamsFlags[x] == ParamModRM)
			Ret.Params[x] = SIBModRM;

		if(Ret.ParamsFlags[x] == ParamReg || Ret.ParamsFlags[x] == ParamRegSeg || Ret.ParamsFlags[x] == ParamRegControl || Ret.ParamsFlags[x] == ParamRegDebug || Ret.ParamsFlags[x] == ParamRegOpSize)
			Ret.Params[x] = Reg;

		if(Ret.ParamsFlags[x] == ParamImmediate || Ret.ParamsFlags[x] == ParamOffset || Ret.ParamsFlags[x] == ParamFarAddress || Ret.ParamsFlags[x] == ParamAddress)
			Ret.Params[x] = Immediate;

		if(Ret.ParamsFlags[x] > ParamRegDirect)
		{
			Ret.Params[x] = Ret.ParamsFlags[x];
			Ret.ParamsFlags[x] = ParamRegDirect;
		}
	}
	
	return Ret;
}