using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.InteropServices;
using System.IO;
using System.Diagnostics;

namespace GenRomFS
{
    enum FileType : UInt32
    {
        HardLink = 0,
        Directory = 1,
        RegularFile = 2,
        SymbolicLink = 3,
        BlockDevice = 4,
        CharDevice = 5,
        Socket = 6,
        Fifi = 7,
        Executable = 8,

        Root = 1024,
        RootDot,
        DotFile,
    }

    class FileData
    {
        public static int ToParagraph(int Size)
        {
            if (Size % 16 == 0)
                return Size;

            return Size + 16 - (Size % 16);
        }

        public FileType Type = FileType.RegularFile;

        public int DataLength;

        public string Name = "";
        public string FullPath = "";

        public int FilePos = 0;

        public FileData Link;
        public List<FileData> Files = new List<FileData>();

        public int HeaderLength { get { return 16 + ToParagraph(Name.Length + 1); } }

    }

    class Program
    {
        public static int MagicA = 0x2d726f6d;
        public static int MagicB = 0x3166732d;

        static void Main(string[] args)
        {
            Program p = new Program();

            p.Run(args);

        }

        void Usage()
        {
            Console.WriteLine("Usage:");
            Console.WriteLine(" GenRomFS [Options] File");
            Console.WriteLine("  Create a RomFS filesystem.");
            Console.WriteLine("");
            Console.WriteLine("  File               The path to the RomFS file to create");
            Console.WriteLine("  -d Directory       Build the file system from the Directory");
            Console.WriteLine("  -v VolumeName      The name of the Volume (Default 'ROMFS'");
            Console.WriteLine("  -a Align           Align files to Byte Boundry (Default 16)");
            //Console.WriteLine("  -f File            Stores the provided in file in the root directory of the file system");
            //Console.WriteLine("  -f @FileList       FileList is new line list of files to store in the root of the file System");
            //Console.WriteLine("  -F Path File       Stores the provided in file under Path the file system");
            //Console.WriteLine("  -F Path @FileList  FileList is new line list of files to store under Path of the file System");
            Console.WriteLine("  -V                 Verobuse Output");
        }

        bool Verbose = false;
        int Align = 16;
        string VolumeName = "ROMFS";
        string InputFolder = "";
        string OutputFileName = "";
        List<Tuple<string, string>> InputList = new List<Tuple<string, string>>();

        bool ParseCommandLine(string[] args)
        {
            if (args.Length == 0)
            {
                Usage();
                return false;
            }


            for (int x = 0; x < args.Length; x++)
            {
                switch (args[x])
                {
                    case "-d":
                        if (x + 1 == args.Length)
                        {
                            Console.WriteLine("  -d: Value missing");
                            return false;
                        }
                        else
                        {
                            InputFolder = args[x + 1];
                            x++;
                        }
                        break;

                    case "-v":
                        if (x + 1 == args.Length)
                        {
                            Console.WriteLine("  -v: Value missing");
                            return false;
                        }
                        else
                        {
                            VolumeName = args[x + 1].Substring(0, Math.Min(args[x + 1].Length, 15));
                            x++;
                        }
                        break;

                    case "-a":
                        if (x + 1 == args.Length)
                        {
                            Console.WriteLine("  -a: Value missing");
                            return false;
                        }
                        else
                        {
                            if (!int.TryParse(args[x + 1], out Align))
                            {
                                Console.WriteLine("  -a: Invalide alignment {0}", args[x + 1]);
                                return false;
                            }

                            if (Align < 16)
                            {
                                Console.WriteLine("  -a: Invalide alignment {0}, must be at least 16", args[x + 1]);
                                return false;
                            }

                            if ((Align & (Align - 1)) != 0)
                            {
                                Console.WriteLine("  -a: Invalide alignment {0}, must be a power of two", args[x + 1]);
                                return false;
                            }
                            x++;
                        }
                        break;

                    case "-f":
                        if (x + 1 == args.Length)
                        {
                            Console.WriteLine("  -f: Value missing");
                            return false;
                        }
                        else
                        {
                            InputList.Add(Tuple.Create("\\", args[x + 1]));
                            x++;
                        }
                        break;

                    case "-F":
                        if (x + 2 == args.Length)
                        {
                            Console.WriteLine("  -f: Value missing");
                            return false;
                        }
                        else
                        {
                            InputList.Add(Tuple.Create(args[x + 1], args[x + 2]));
                            x += 2;
                        }
                        break;

                    case "-h":
                    case "-?":
                    case "--usage":
                        Usage();
                        return false;

                    case "-V":
                        Verbose = true;
                        break;

                    default:
                        if (args[x][0] == '-')
                        {
                            Console.WriteLine("Unknown Switch {0}", args[x]);
                            return false;
                        }
                        else
                        {
                            OutputFileName = args[x];
                        }
                        break;
                }

            }

            return true;
        }


        void Run(string[] args)
        {
            if (!ParseCommandLine(args))
                return;

            if(OutputFileName == "")
            {
                Console.WriteLine("ERROR: Output file missing");
                return;
            }

            if(InputFolder != "")
                Console.WriteLine("Source: {0}", InputFolder);
            Console.WriteLine("Output: {0}", OutputFileName);
            Console.WriteLine("Volume Name: {0}", VolumeName);


            if(Verbose)
            {
                Console.WriteLine("Alignment {0}", Align);

                foreach(var Entry in InputList)
                {
                    Console.WriteLine("{1} in {0}", Entry.Item1, Entry.Item2);
                }
            }



            // Create the root of the File System

            FileData Root = new FileData();
            Root.Type = FileType.Root;
            Root.Name = VolumeName;
            Root.FilePos = 0;
            Root.DataLength = Root.HeaderLength;

            {
                FileData Entry = new FileData();
                Entry.Name = ".";
                Entry.Type = FileType.RootDot; // FileType.Directory | FileType.Executable;
                Entry.Link = Entry;

                Root.Files.Add(Entry);
            }


            {
                FileData Entry = new FileData();
                Entry.Name = "..";
                Entry.Type = FileType.DotFile;
                Entry.Link = Root;

                Root.Files.Add(Entry);
            }


            // Process the direcotry, if we have one
            if (InputFolder != "")
                ProcessDirectory(InputFolder, Root);


            // Process individal files

            // Sort and Align all the entries
            Root.DataLength += SortAndAlignDirecotry(Root, Root.DataLength);


            if (Verbose)
            {
                Console.WriteLine("{0} ({1}): {2:X}", Root.Name, Root.Type.ToString(), Root.DataLength);
                DumpData(Root);
            }


            // Write output file
            using (var stream = File.Open(OutputFileName, FileMode.Create, FileAccess.ReadWrite))
            {
                using (BinaryWriter OutputFile = new BinaryWriter(stream, Encoding.ASCII))
                {
                    WriteFileHeader(OutputFile, Root, Root.HeaderLength);
                    WriteData(OutputFile, Root);

                    //while (OutputFile.BaseStream.Length % 1024 != 0)
                    //    OutputFile.Write(0);

                    int Value = 0;

                    using (BinaryReader InputFile = new BinaryReader(stream, Encoding.ASCII))
                    {
                        InputFile.BaseStream.Position = 0;
                        List<Byte> Data = InputFile.ReadBytes(512).ToList();

                        byte[] ByteData = Data.ToArray();

                        for (int x = 0; x < Data.Count; x += 4)
                        {
                            Value += SwapEndian(BitConverter.ToInt32(ByteData, x));
                        }

                        OutputFile.BaseStream.Position = 12;
                        OutputFile.Write(SwapEndian(-Value));
                    }
                }

            }
        }

        public void DumpData(FileData Current, int Depth = 1)
        {
            for(int x = 0; x < Current.Files.Count; x++)
            {
                FileData Entry = Current.Files[x];
                int Next = x + 1 < Current.Files.Count ? Current.Files[x + 1].FilePos : 0;

                for (int y = 0; y < Depth; y++)
                    Console.Write(' ');

                Console.Write("{0:X}: {1} ({2}", Entry.FilePos, Entry.Name, Entry.Type.ToString());

                //Console.Write("{2:X}: {0} ({1}) -> {3:X}", Entry.Name, Entry.Type.ToString(), Entry.FilePos, x + 1 < Current.Files.Count ? Current.Files[x + 1].FilePos : 0);

                if (Entry.Type == FileType.HardLink || Entry.Type == FileType.RootDot || Entry.Type == FileType.DotFile)
                {
                    Console.WriteLine(" => {0:X}) -> {1:X}", Entry.Link.Type == FileType.Root ? Entry.Link.Files[0].FilePos : Entry.Link.FilePos, Next);
                }
                else
                {
                    Console.WriteLine("): {0} -> {1:X}", Entry.DataLength, Next);

                    if (Entry.Type == FileType.Directory)
                    {
                        DumpData(Entry, Depth + 1);
                    }
                }

            }

        }

        int SortAndAlignDirecotry(FileData Parent, int Offset)
        {
            var Files = Parent.Files.Where(e => e.Type != FileType.DotFile && e.Type != FileType.RootDot).OrderBy(e => e.Name);

            // Pull out . and .. to make sure they will always be at the start.
            Parent.Files = Parent.Files.Where(e => e.Type == FileType.DotFile || e.Type == FileType.RootDot).OrderBy(e => e.Name).ToList();

            // The add the sorted results
            Parent.Files.AddRange(Files);

            int Length = 0;

            foreach (FileData Entry in Parent.Files)
            {
                Entry.FilePos = Offset + Length;

                if (Entry.Type == FileType.RootDot)
                {
                    // The file header isn't padded, so the first entry will always be right after it, no matter what the alignment is,
                    // Because of that Offset may not be aligned
                    Length = AlignData(Entry.HeaderLength) - Offset;
                }
                else
                {
                    Length += AlignData(Entry.HeaderLength + Entry.DataLength);
                }

                if (Entry.Type == FileType.Directory)
                {
                    Length += SortAndAlignDirecotry(Entry, Offset + Length);

                    Entry.Link = Entry.Files[0];
                }
            }

            return Length;
        }


        public int AlignData(int Size)
        {
            if (Size % Align == 0)
                return Size;

            return Size + Align - (Size % Align);
        }

        void WriteFileBlock(BinaryWriter OutputFile, FileData Current, int Next)
        {
            // Write the header
            WriteFileHeader(OutputFile, Current, Next);

            // Write the contents of the file
            if (Current.Type == FileType.RegularFile)
            {
                using (BinaryReader Input = new BinaryReader(File.OpenRead(Current.FullPath)))
                {
                    OutputFile.Write(Input.ReadBytes(Current.DataLength));
                }
            }

            // Pad it out to be aligned
            if ((OutputFile.BaseStream.Position % Align) != 0)
            {
                long Count = Align - (OutputFile.BaseStream.Position % Align);
                while (Count != 0)
                {
                    OutputFile.Write((byte)0);

                    Count--;
                }
            }

        }

        void WriteData(BinaryWriter OutputFile, FileData Base)
        {
            for(int x = 0; x < Base.Files.Count; x++)
            {
                WriteFileBlock(OutputFile, Base.Files[x], x + 1 < Base.Files.Count ? Base.Files[x + 1].FilePos : 0);

                if (Base.Files[x].Type == FileType.Directory)
                {
                    WriteData(OutputFile, Base.Files[x]);
                }
            }
        }

        int SwapEndian(int value)
        {
            Byte[] ret = BitConverter.GetBytes(value);

            Array.Reverse(ret);

            return BitConverter.ToInt32(ret, 0);
        }

        void WriteFileHeader(BinaryWriter OutputFile, FileData Header, int Next)
        {
            if (Header.FilePos != (int)OutputFile.BaseStream.Position)
                Console.Write("!");

            if (Header.Type == FileType.Root)
            {
                OutputFile.Write(SwapEndian(MagicA));
                OutputFile.Write(SwapEndian(MagicB));
                OutputFile.Write(SwapEndian(Header.DataLength));
                OutputFile.Write(SwapEndian(0));
                OutputFile.Write(Header.Name.ToArray());
            }
            else
            {
                int Value = 0;
                int Link = 0;
                int Length = 0;

                if (Header.Type == FileType.RootDot)
                    Next = Next | (int)(FileType.Directory | FileType.Executable);

                else if (Header.Type == FileType.DotFile)
                    Next = Next | (int)(FileType.HardLink);

                else
                    Next = Next | (int)Header.Type;

                Value += Next;

                if (Header.Link == null)
                {
                    Link = 0;
                }
                else
                {
                    if (Header.Link.Type == FileType.Root)
                        Link = Header.Link.Files[0].FilePos;
                    else
                        Link = Header.Link.FilePos;
                }

                Value += Link;

                List<byte> Name = Encoding.ASCII.GetBytes(Header.Name).ToList();
                Name.Add(0); // Add the null

                while (Name.Count % 4 != 0)
                {
                    Name.Add(0);
                }

                for(int x = 0; x < Name.Count; x += 4)
                {
                    Value += SwapEndian(BitConverter.ToInt32(Name.ToArray(), x));
                }


                if (Header.Type == FileType.RegularFile)
                    Length = Header.DataLength;

                Value += Length;

                OutputFile.Write(SwapEndian(Next));
                OutputFile.Write(SwapEndian(Link));
                OutputFile.Write(SwapEndian(Length));
                OutputFile.Write(SwapEndian(-Value));
                OutputFile.Write(Name.ToArray());
            }

            // Pad it out to be aligned
            if ((OutputFile.BaseStream.Position % 16) != 0)
            {
                long Count = 16 - (OutputFile.BaseStream.Position % 16);
                while (Count != 0)
                {
                    OutputFile.Write((byte)0);

                    Count--;
                }
            }

        }

        void ProcessFile(string FileName, FileData Parent)
        {
            FileInfo Info = new FileInfo(FileName);
            FileData NewEntry = new FileData();

            if (Info.Attributes.HasFlag(FileAttributes.ReparsePoint))
            {
                if (Verbose)
                    Console.WriteLine("Skipping system link {0}", Info.FullName);

                return;
            }
            else if (Info.Attributes.HasFlag(FileAttributes.Offline))
            {
                NewEntry.Name = Info.Name;
                NewEntry.Type = FileType.RegularFile;
                NewEntry.DataLength = 0;
            }
            else
            {
                // Can't handle super long files
                if (Info.Length > 0x80000000)
                {
                    if (Verbose)
                        Console.WriteLine("Skipping large file {0}", Info.FullName);

                    return;
                }

                NewEntry.Name = Info.Name;
                NewEntry.Type = FileType.RegularFile;
                NewEntry.DataLength = (int)Info.Length;  // Math.Min((int)Info.Length, Align/2); 
                NewEntry.FullPath = Info.FullName;
            }

            Parent.Files.Add(NewEntry);
        }

        void ProcessDirectory(string DirectoryName, FileData Parent)
        {
            foreach (string FileName in Directory.EnumerateFileSystemEntries(DirectoryName))
            {
                FileInfo Info = new FileInfo(FileName);

                // Skip super hidden files
                if (Info.Attributes.HasFlag(FileAttributes.System) || Info.Attributes.HasFlag(FileAttributes.Hidden))
                    continue;

                FileData NewEntry = new FileData();
                if (Info.Attributes.HasFlag(FileAttributes.Directory))
                {
                    NewEntry.Type = FileType.Directory;
                    NewEntry.Name = Info.Name;
                    Parent.Files.Add(NewEntry);

                    {
                        FileData Entry = new FileData();
                        Entry.Name = ".";
                        Entry.Type = FileType.DotFile;
                        Entry.Link = NewEntry;  // Link to the entry for the new directory

                        NewEntry.Files.Add(Entry);
                        NewEntry.Link = Entry; // Link to the first file in the directory 
                    }

                    {
                        FileData Entry = new FileData();
                        Entry.Name = "..";
                        Entry.Type = FileType.DotFile;
                        Entry.Link = Parent;      // Link to the entry of the parent Direcotry

                        NewEntry.Files.Add(Entry);
                    }

                    ProcessDirectory(FileName, NewEntry);
                }
                else
                {
                    ProcessFile(FileName, Parent);
                }
            }
        }
    }
}
