using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.InteropServices;
using System.IO;
using System.Diagnostics;
using System.Text.RegularExpressions;

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
        Fifo = 7,
        Executable = 8,

        Root = 1024,
        RootDot,
        DotFile,
    }

    class FileData
    {
        public FileType Type = FileType.RegularFile;
        public bool Executable;

        public int DataLength;

        public string Name = "";
        public string FullPath = "";

        public int Padding = 0; // Padding to write before this block
        public int FilePos = 0; // The calculated location of where it should be in the file

        public FileData Link;
        public int DevNum;
        public List<FileData> Files = new List<FileData>();

        public int HeaderLength { get { return 16 + Program.AlignParagraph(Name.Length + 1); } }

        public FileData this [string Name]
        {
            get
            {
                if (String.IsNullOrWhiteSpace(Name))
                    return this;

                return Files.FirstOrDefault(e => e.Name == Name);
            }
        }
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
            Console.WriteLine("  -f File            Stores the provided in file in the root directory of the file system");
            Console.WriteLine("  -p Path File       Stores the provided in file under Path in the file system");
            Console.WriteLine("  -f FileList        FileList is new line list of files to store in the root of the file System");
            Console.WriteLine("  -P PathFileList    PathFileList is new line list paths and files to store in the file system (Path<NL>File<NL>)");
            Console.WriteLine("  -V                 Verbose Output");
        }

        bool Verbose = false;
        int Align = 16;
        string VolumeName = "";
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
                            VolumeName = args[x + 1];

                            if (VolumeName.Length > 15)
                                VolumeName = VolumeName.Substring(0, 15);

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
                        if (x + 1 == args.Length)
                        {
                            Console.WriteLine("  -F: Value missing");
                            return false;
                        }
                        else
                        {
                            if (!File.Exists(args[x + 1]))
                            {
                                Console.WriteLine("  -F: Input file missing {0}", args[x + 1]);
                                return false;

                            }

                            var FileList = File.ReadAllLines(args[x + 1]).Where(e => !String.IsNullOrWhiteSpace(e)).ToList();

                            foreach (string File in FileList)
                            {
                                if(!String.IsNullOrWhiteSpace(File))
                                    InputList.Add(Tuple.Create("\\", File));
                            }

                            x++;
                        }
                        break;

                    case "-p":
                        if (x + 2 == args.Length)
                        {
                            Console.WriteLine("  -p Value missing");
                            return false;
                        }
                        else
                        {
                            InputList.Add(Tuple.Create(args[x + 1], args[x + 2]));
                            x += 2;
                        }
                        break;

                    case "-P":
                        if (x + 1 == args.Length)
                        {
                            Console.WriteLine("  -P: Value missing");
                            return false;
                        }
                        else
                        {
                            if (!File.Exists(args[x + 1]))
                            {
                                Console.WriteLine("  -P: Input file missing {0}", args[x + 1]);
                                return false;
                            }

                            var FileList = File.ReadAllLines(args[x + 1]).Where(e => !String.IsNullOrWhiteSpace(e)).ToList();

                            for(int y = 0; y < FileList.Count; y += 2)
                            {
                                if (String.IsNullOrWhiteSpace(FileList[y]))
                                    continue;

                                if (y + 1 >= FileList.Count)
                                {
                                    Console.WriteLine("  -P: Directory without matching file in {0}", args[x + 1]);
                                    return false;
                                }

                                InputList.Add(Tuple.Create(FileList[y], FileList[y + 1]));
                            }

                            x++;
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
            
            if(VolumeName == "")
            {
                VolumeName = string.Format("ROMFS {0:X8}", (int)(DateTime.Now.ToBinary() >> 20));

                if (VolumeName.Length > 15)
                    VolumeName = VolumeName.Substring(0, 15);
            }

            if(InputFolder != "")
                Console.WriteLine("Source: {0}", InputFolder);

            Console.WriteLine("Output: {0}", OutputFileName);
            Console.WriteLine("Volume Name: {0}", VolumeName);


            if(Verbose)
            {
                Console.WriteLine("Alignment: {0}", Align);
            }



            // Create the root of the File System

            FileData Root = new FileData();
            Root.Type = FileType.Root;
            Root.Name = VolumeName;
            Root.FilePos = 0;
            Root.Padding = 0;
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

            //Process individal files
            foreach (var Entry in InputList)
            {
                if (Verbose)
                    Console.WriteLine("Storing: {1} in {0}", Entry.Item1, Entry.Item2);

                // Find Directory
                string[] Paths = Entry.Item1.Split(Path.DirectorySeparatorChar);

                FileData Node = Root;

                foreach (string Element in Paths)
                {
                    FileData NewEntry = Node[Element];

                    if (NewEntry == null)
                    {
                        NewEntry = CreateDirectory(Element, Node);
                    }


                    if(NewEntry.Type == FileType.HardLink || NewEntry.Type == FileType.DotFile || NewEntry.Type == FileType.RootDot)
                        Node = NewEntry.Link;
                    else
                        Node = NewEntry;

                    
                }

                ProcessFile(Path.GetFullPath(Entry.Item2), Node);
            }

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

                    while (OutputFile.BaseStream.Length % 1024 != 0)
                        OutputFile.Write((byte)0);

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
                if (Entry.Executable)
                    Console.Write(" Executable");

                if (Entry.Type == FileType.HardLink || Entry.Type == FileType.RootDot || Entry.Type == FileType.DotFile)
                {
                    Console.WriteLine(" => {0:X}) -> {1:X}", Entry.Link.Type == FileType.Root ? Entry.Link.Files[0].FilePos : Entry.Link.FilePos, Next);
                }
                else
                {
                    if (Entry.Type == FileType.Directory)
                    {
                        Console.WriteLine(" => {0:X}) -> {1:X}", Entry.Link.FilePos, Next);
                        DumpData(Entry, Depth + 1);
                    }
                    else
                    {
                        Console.WriteLine("): {0} -> {1:X}", Entry.DataLength, Next);
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
                if (Entry.Type != FileType.RegularFile && Entry.Type != FileType.SymbolicLink)
                {
                    // If we don't have any data, don't worry about alignment
                    Entry.Padding = 0;
                    Entry.FilePos = AlignParagraph(Offset + Length);
                    Entry.Padding += (Entry.FilePos - (Offset + Length));

                    Length += Entry.HeaderLength + Entry.Padding;
                }
                else
                {
                    Entry.FilePos = AlignData(Offset + Length) - Entry.HeaderLength;
                    while (Entry.FilePos < (Offset + Length))
                        Entry.FilePos += Align;

                    Entry.Padding += (Entry.FilePos - (Offset + Length));

                    Length += Entry.HeaderLength + Entry.DataLength + Entry.Padding;
                }

                if (Entry.Type == FileType.Directory)
                {
                    Length += SortAndAlignDirecotry(Entry, Offset + Length);

                    Entry.Link = Entry.Files[0];
                }
            }

            return Length;
        }


        public static int AlignParagraph(int Size)
        {
            if (Size % 16 == 0)
                return Size;

            return Size + 16 - (Size % 16);
        }

        public int AlignData(int Size)
        {
            if (Size % Align == 0)
                return Size;

            return Size + Align - (Size % Align);
        }

        void WriteFileBlock(BinaryWriter OutputFile, FileData Current, int Next)
        {
            if (Current.Padding != 0)
            {
                for (int x = 0; x < Current.Padding; x++)
                    OutputFile.Write((byte)0);
            }

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
                Console.Write("Sync Error, Expected {0} recived {1}", Header.FilePos, OutputFile.BaseStream.Position);

            if (Header.Type == FileType.Root)
            {
                OutputFile.Write(SwapEndian(MagicA));
                OutputFile.Write(SwapEndian(MagicB));
                OutputFile.Write(SwapEndian(AlignParagraph(Header.DataLength)));
                OutputFile.Write(SwapEndian(0));
                OutputFile.Write(Header.Name.ToArray());
                OutputFile.Write((byte)0);
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

                if (Header.Executable)
                    Next |= (int)FileType.Executable;

                Value += Next;

                if (Header.Type == FileType.BlockDevice || Header.Type == FileType.CharDevice || Header.Type == FileType.Fifo)
                {
                    Link = Header.DevNum;
                }
                else if (Header.Link == null)
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

            if(!Info.Exists)
            {
                Console.WriteLine("File dosn't exist: {0}", Info.FullName);
                return;
            }

            if (Info.Attributes.HasFlag(FileAttributes.ReparsePoint))
            {
                if (Verbose)
                    Console.WriteLine("Skipping system link {0}", Info.FullName);

                return;
            }
            else if (Info.Attributes.HasFlag(FileAttributes.Offline))
            {
                if (Verbose)
                    Console.WriteLine("Saving zero legth offline file {0}", Info.FullName);

                NewEntry.Name = Info.Name;
                NewEntry.Type = FileType.RegularFile;
                NewEntry.DataLength = 0;
            }
            else
            {
                // Can't handle super long files
                if (Info.Length > 0x80000000)
                {
                    Console.WriteLine("Skipping large file {0}", Info.FullName);

                    return;
                }

                NewEntry.Name = Info.Name;
                NewEntry.Type = FileType.RegularFile;
                NewEntry.DataLength = (int)Info.Length;  // Math.Min((int)Info.Length, Align/2); 
                NewEntry.FullPath = Info.FullName;

                if (Info.Name[0] == '@' && Info.Length == 0)
                {
                    // special file @name,[cpub],major,minor
                    Match NameMatch = Regex.Match(Info.Name, @"^@([\w\d_+-]*),(\w),(\d*),(\d*)$", RegexOptions.IgnoreCase);
                    if (NameMatch.Groups.Count == 5)
                    {
                        NewEntry.Name = NameMatch.Groups[1].Value;
                        short Major, Minor;

                        if (!short.TryParse(NameMatch.Groups[3].Value, out Major))
                        {
                            Console.WriteLine("Invalid speical device {0}", Info.FullName);
                            return;
                        }

                        if (!short.TryParse(NameMatch.Groups[4].Value, out Minor))
                        {
                            Console.WriteLine("Invalid speical device {0}", Info.FullName);
                            return;
                        }

                        NewEntry.DevNum = (Major << 16) + Minor;
                        switch (NameMatch.Groups[2].Value[0])
                        {
                            case 'c':
                            case 'u':
                                NewEntry.Type = FileType.CharDevice;
                                break;

                            case 'b':
                                NewEntry.Type = FileType.BlockDevice;
                                break;

                            case 'p':
                                NewEntry.Type = FileType.Fifo;
                                break;

                            default:
                                Console.WriteLine("Invalid speical device type {0} in {1}", NameMatch.Groups[2].Value[0], Info.FullName);

                                return;
                        }
                    }

                }
            }

            string ext = Path.GetExtension(Info.Name).ToLower();

            if (ext == ".exe" || ext == ".com" || ext == ".bat" || ext == ".cmd")
                NewEntry.Executable = true;

            Parent.Files.Add(NewEntry);
        }

        FileData CreateDirectory(string DirectoryName, FileData Parent)
        {
            FileData NewEntry = new FileData();
            NewEntry.Type = FileType.Directory;
            NewEntry.Name = DirectoryName;
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

            return NewEntry;
        }

        void ProcessDirectory(string DirectoryName, FileData Parent)
        {
            foreach (string FileName in Directory.EnumerateFileSystemEntries(DirectoryName))
            {
                FileInfo Info = new FileInfo(FileName);

                // Skip super hidden files
                if (Info.Attributes.HasFlag(FileAttributes.System) || Info.Attributes.HasFlag(FileAttributes.Hidden))
                    continue;

                if (Info.Attributes.HasFlag(FileAttributes.Directory))
                {
                    FileData NewEntry = CreateDirectory(Info.Name, Parent);
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
