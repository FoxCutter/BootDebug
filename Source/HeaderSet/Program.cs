using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.InteropServices;
using System.IO;

namespace HeaderSet
{
    class Program
    {
        static bool UpdateMultiboot1(BinaryReader InputFile, uint Load_Address, uint Offset = 0, uint Load_End_Address = 0, uint BSS_End_Address = 0)
        {
            // Okay, lets find the headers.
            byte[] Data = new byte[8192];

            // Starting with the Multiboot v1 header
            long Base = InputFile.BaseStream.Position;
            InputFile.Read(Data, 0, 8192);

            byte[] Header = new byte[4];
            byte[] Value = new byte[4] { 0x02, 0xB0, 0xAD, 0x1B };

            uint Header_Address = Load_Address;

            for (uint x = 0; x < Data.Length; x += 4)
            {
                Array.Copy(Data, x, Header, 0, 4);

                if (Header.SequenceEqual(Value))
                {
                    Console.WriteLine("Found MultiBoot Header at 0x{0}", x.ToString("X08"));

                    uint Base_Address = Load_Address - Offset;
                    Load_End_Address += Offset;
                    BSS_End_Address += Offset;
                    Header_Address -= Offset;

                    Header_Address += x;
                    InputFile.BaseStream.Position = Base + x;
                    InputFile.ReadUInt32();  // Magic
                    uint Flags = InputFile.ReadUInt32();  // Flags
                    uint Chceksum = InputFile.ReadUInt32();  // Checksum

                    Console.WriteLine("Offset Information ");
                    Console.WriteLine("   Offset to Image:        0x{0}", Offset.ToString("X08"));
                    Console.WriteLine("   File Load Address:      0x{0}", Base_Address.ToString("X08"));
                    Console.WriteLine("   Image Base Address:     0x{0}", Load_Address.ToString("X08"));
                    Console.WriteLine("   Header Load Address:    0x{0}", Header_Address.ToString("X08"));
                    Console.WriteLine("   Load End Address:       0x{0}", Load_End_Address.ToString("X08"));
                    Console.WriteLine("   BSS End Address:        0x{0}", BSS_End_Address.ToString("X08"));
                    Console.WriteLine();


                    if ((Flags & 0x00010000) != 0x00010000)
                    {
                        Console.WriteLine(" Address fields aren't in the header");
                        continue;
                    }

                    // This is all we care about
                    using (BinaryWriter WriteData = new BinaryWriter(InputFile.BaseStream, Encoding.Default, true))
                    {
                        WriteData.BaseStream.Position = InputFile.BaseStream.Position;
                        WriteData.Write(Header_Address);
                        WriteData.Write(Load_Address);
                        WriteData.Write(Load_End_Address);
                        WriteData.Write(BSS_End_Address);
                    }

                    return true;
                }

                //if(Data[x])

                //0x1BADB002
            }

            return false;
        }

        static bool UpdateMultiboot2(BinaryReader InputFile, uint Load_Address, uint Offset = 0, uint Load_End_Address = 0, uint BSS_End_Address = 0)
        {
            byte[] Data = Data = new byte[32768];

            // Starting with the Multiboot v2 header
            long Base = InputFile.BaseStream.Position;
            InputFile.Read(Data, 0, 32768);

            byte[] Header = new byte[4];
            byte[] Value = new byte[4] { 0xD6, 0x50, 0x52, 0xE8 };

            uint Header_Address = Load_Address;

            for (uint x = 0; x < Data.Length; x += 8)
            {
                Array.Copy(Data, x, Header, 0, 4);

                if (Header.SequenceEqual(Value))
                {
                    Console.WriteLine("Found MultiBoot2 Header at 0x{0}", x.ToString("X08"));

                    uint Base_Address = Load_Address - Offset;
                    Load_End_Address += Offset;
                    BSS_End_Address += Offset;
                    Header_Address -= Offset;

                    Header_Address += x;
                    InputFile.BaseStream.Position = Base + x;
                    InputFile.ReadUInt32();  // Magic
                    InputFile.ReadUInt32();  // Arch
                    uint Leng = InputFile.ReadUInt32();  // Header Length
                    uint Checksum = InputFile.ReadUInt32();  // Checksum

                    Console.WriteLine("Offset Information ");
                    Console.WriteLine("   Offset to Image:        0x{0}", Offset.ToString("X08"));
                    Console.WriteLine("   File Load Address:      0x{0}", Base_Address.ToString("X08"));
                    Console.WriteLine("   Image Base Address:     0x{0}", Load_Address.ToString("X08"));
                    Console.WriteLine("   Header Load Address:    0x{0}", Header_Address.ToString("X08"));
                    Console.WriteLine("   Load End Address:       0x{0}", Load_End_Address.ToString("X08"));
                    Console.WriteLine("   BSS End Address:        0x{0}", BSS_End_Address.ToString("X08"));
                    Console.WriteLine();


                    // Read in the tags
                    while (true)
                    {
                        if (InputFile.BaseStream.Position > InputFile.BaseStream.Length)
                        {
                            Console.WriteLine("ERROR: MulitBoot2 Header is Invalid");
                            break;
                        }

                        while (InputFile.BaseStream.Position % 8 != 0)
                            InputFile.BaseStream.Position++;

                        long TagBase = InputFile.BaseStream.Position;

                        ushort Type = InputFile.ReadUInt16();    // Type
                        InputFile.ReadUInt16();                  // Flags
                        uint Size = InputFile.ReadUInt32();      // Size

                        if (Type == 2)
                        {
                            // This is the one we care about.
                            using (BinaryWriter WriteData = new BinaryWriter(InputFile.BaseStream, Encoding.Default, true))
                            {
                                WriteData.BaseStream.Position = InputFile.BaseStream.Position;
                                WriteData.Write(Header_Address);
                                WriteData.Write(Load_Address);
                                WriteData.Write(Load_End_Address);
                                WriteData.Write(BSS_End_Address);
                            }

                            return true;
                        }
                        else if (Type == 0)
                            break;

                        InputFile.BaseStream.Position = TagBase + Size;
                    }

                    break;
                }
            }
            
            return false;
        }

        static int Main(string[] args)
        {
            if (args.Length == 0 || !File.Exists(args[0]))
            {
                Console.WriteLine("USAGE:");
                Console.WriteLine("  HeaderSet.exe \"Path to PE file\"");
                Console.WriteLine("");
                Console.WriteLine("  This tool will process the PE file and set both the MultiBoot header");
                Console.WriteLine(" and the MultiBoot2 header to the correct information for the File.");
                Console.WriteLine(" It will NOT set the entry point to the entry point in the PE header.");

                return -1;
            }

            bool ExtractFile = false;

            using (BinaryReader FileData = new BinaryReader(File.Open(args[0], FileMode.Open, FileAccess.ReadWrite)))
            {
                PEUtils.PEFile WildFile = new PEUtils.PEFile();
                if (!WildFile.ReadFile(FileData))
                {
                    Console.WriteLine("Input not a PE file, searching to see if it's a container.");

                    if (!WildFile.SearchFile(FileData))
                    {
                        Console.WriteLine("ERROR: Invalid PE file.");
                        return -1;
                    }

                    Console.WriteLine("PE file found at offset {0:X8}", WildFile.Offset);
                    Console.WriteLine();
                }


                if (WildFile._OptionalHeader.FileAlignment != WildFile._OptionalHeader.SectionAlignment)
                {
                    Console.WriteLine("ERROR: File Alignment and Section Alignment do not match.");
                    return -1;
                }

                // First off, lets work out the header values.
                uint Load_Address = 0;
                uint Load_End_Address = 0;
                uint BSS_End_Address = 0;

                foreach (var Entry in WildFile._SectionHeaders)
                {
                    if ((Entry.Characteristics & PEUtils.SectionCharacteristics.CNT_CODE) == PEUtils.SectionCharacteristics.CNT_CODE)
                    {
                        //if (Entry.VirtualAddress < Load_Address )
                        //    Load_Address = Entry.VirtualAddress;
                    }
                    else if ((Entry.Characteristics & PEUtils.SectionCharacteristics.CNT_INITIALIZED_DATA) == PEUtils.SectionCharacteristics.CNT_INITIALIZED_DATA)
                    {
                        if (Entry.VirtualAddress + Entry.VirtualSize > Load_End_Address)
                            Load_End_Address = Entry.VirtualAddress + Entry.VirtualSize;
                    }
                    else if ((Entry.Characteristics & PEUtils.SectionCharacteristics.CNT_UNINITIALIZED_DATA) == PEUtils.SectionCharacteristics.CNT_UNINITIALIZED_DATA)
                    {
                        if (Entry.VirtualAddress + Entry.VirtualSize > BSS_End_Address)
                            BSS_End_Address = Entry.VirtualAddress + Entry.VirtualSize;
                    }
                }

                if (BSS_End_Address < Load_End_Address)
                    BSS_End_Address = Load_End_Address;

                if (!ExtractFile)
                {
                    Load_End_Address = (uint)FileData.BaseStream.Length;
                    BSS_End_Address = (uint)FileData.BaseStream.Length;
                }

                // Readjust everything to the image base
                Load_Address += WildFile._OptionalHeader.ImageBase;
                Load_End_Address += WildFile._OptionalHeader.ImageBase;
                BSS_End_Address += WildFile._OptionalHeader.ImageBase;

                uint Header_Address = WildFile._OptionalHeader.ImageBase;

                //Console.WriteLine("Offset Information ");
                //Console.WriteLine("   Header Address:         0x{0}", Header_Address.ToString("X08"));
                //Console.WriteLine("   Load Address:           0x{0}", Load_Address.ToString("X08"));
                //Console.WriteLine("   Load End Address:       0x{0}", Load_End_Address.ToString("X08"));
                //Console.WriteLine("   BSS End Address:        0x{0}", BSS_End_Address.ToString("X08"));

                //Console.WriteLine();

                FileData.BaseStream.Position = WildFile.Offset;
                UpdateMultiboot1(FileData, WildFile._OptionalHeader.ImageBase, ExtractFile ? 0 : WildFile.Offset, Load_End_Address, BSS_End_Address);
                FileData.BaseStream.Position = WildFile.Offset;
                UpdateMultiboot2(FileData, WildFile._OptionalHeader.ImageBase, ExtractFile ? 0 : WildFile.Offset, Load_End_Address, BSS_End_Address);
            }

            return 0;
        }
    }
}
