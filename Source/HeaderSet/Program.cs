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
            
            using (BinaryReader FileData = new BinaryReader(File.Open(args[0], FileMode.Open, FileAccess.ReadWrite)))
            {
                PEUtils.PEFile WildFile = new PEUtils.PEFile();
                if (!WildFile.ReadFile(FileData))
                {
                    Console.WriteLine("ERROR: Invalid PE file.");
                    return -1;
                }
                
                // We need to sanity check a few things here
                //if (WildFile._OptionalHeader.ImageBase != 0x100000)
                //{
                //    Console.WriteLine("ERROR: PE file doesn't start at the right base.");
                //    return -1;
                //}

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

                // Readjust everything to the image base
                Load_Address += WildFile._OptionalHeader.ImageBase;
                Load_End_Address += WildFile._OptionalHeader.ImageBase;
                BSS_End_Address += WildFile._OptionalHeader.ImageBase;

                uint Header_Address = WildFile._OptionalHeader.ImageBase;

                Console.WriteLine("Offset Information ");
                Console.WriteLine("   Header Address:         0x{0}", Header_Address.ToString("X08"));
                Console.WriteLine("   Load Address:           0x{0}", Load_Address.ToString("X08"));
                Console.WriteLine("   Load End Address:       0x{0}", Load_End_Address.ToString("X08"));
                Console.WriteLine("   BSS End Address:        0x{0}", BSS_End_Address.ToString("X08"));

                Console.WriteLine();
                
                // Okay, lets find the headers.
                byte[] Data = new byte[8192];

                // Starting with the Multiboot v1 header
                FileData.BaseStream.Position = 0;
                FileData.Read(Data, 0, 8192);

                byte[] Header = new byte[4];
                byte[] Value = new byte[4] { 0x02, 0xB0, 0xAD, 0x1B };

                for (uint x = 0; x < Data.Length; x += 4)
                {
                    Array.Copy(Data, x, Header, 0, 4);

                    if (Header.SequenceEqual(Value))
                    {
                        Console.WriteLine("Found MultiBoot Header at  0x{0}", x.ToString("X08"));
                        
                        Header_Address = WildFile._OptionalHeader.ImageBase + x;
                        FileData.BaseStream.Position = x;
                        FileData.ReadUInt32();  // Magic
                        uint Flags = FileData.ReadUInt32();  // Flags
                        uint Chceksum = FileData.ReadUInt32();  // Checksum

                        if ((Flags & 0x00010000) != 0x00010000)
                        {
                            Console.WriteLine(" Address fields aren't in the header");
                            continue;
                        }

                        // This is all we care about
                        using (BinaryWriter WriteData = new BinaryWriter(FileData.BaseStream, Encoding.Default, true))
                        {
                            WriteData.BaseStream.Position = FileData.BaseStream.Position;
                            WriteData.Write(Header_Address);
                            WriteData.Write(Load_Address);
                            WriteData.Write(Load_End_Address);
                            WriteData.Write(BSS_End_Address);
                        }                    
                    }
                    
                    //if(Data[x])
                    
                    //0x1BADB002
                }

                Data = new byte[32768];

                // Starting with the Multiboot v2 header
                FileData.BaseStream.Position = 0;
                FileData.Read(Data, 0, 32768);

                Value = new byte[4] { 0xD6, 0x50, 0x52, 0xE8 };

                for (uint x = 0; x < Data.Length; x += 8)
                {
                    Array.Copy(Data, x, Header, 0, 4);

                    if (Header.SequenceEqual(Value))
                    {
                        Console.WriteLine("Found MultiBoot2 Header at 0x{0}", x.ToString("X08"));

                        Header_Address = WildFile._OptionalHeader.ImageBase + x;

                        FileData.BaseStream.Position = x;
                        FileData.ReadUInt32();  // Magic
                        FileData.ReadUInt32();  // Arch
                        uint Leng = FileData.ReadUInt32();  // Header Length
                        uint Checksum = FileData.ReadUInt32();  // Checksum

                        // Read in the tags
                        while (true)
                        {
                            if (FileData.BaseStream.Position > FileData.BaseStream.Length)
                            {
                                Console.WriteLine("ERROR: MulitBoot2 Header is Invalid");
                                break;
                            }
                            
                            while (FileData.BaseStream.Position % 8 != 0)
                                FileData.BaseStream.Position++;

                            long TagBase = FileData.BaseStream.Position;

                            ushort Type = FileData.ReadUInt16();    // Type
                            FileData.ReadUInt16();                  // Flags
                            uint Size = FileData.ReadUInt32();      // Size

                            if (Type == 2)
                            {
                                // This is the one we care about.
                                using (BinaryWriter WriteData = new BinaryWriter(FileData.BaseStream, Encoding.Default, true))
                                {
                                    WriteData.BaseStream.Position = FileData.BaseStream.Position;
                                    WriteData.Write(Header_Address);
                                    WriteData.Write(Load_Address);
                                    WriteData.Write(Load_End_Address);
                                    WriteData.Write(BSS_End_Address);
                                }
                            }
                            else if (Type == 0)
                                break;

                            FileData.BaseStream.Position = TagBase + Size;
                        }
                        
                        break;
                    }
                }

            }

            return 0;
        }
    }
}
