using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;

namespace DiskTools
{
    struct PathInfo
    {
        public int Partition;
        public string Folder;
        public string File;

        public string FilePath;
    }
    
    class Program
    {
        enum CommandType
        {
            Info,
            Add,
            Copy,
            Dir,
            None
        }

        static void Usage()
        {
            Console.WriteLine("");
            Console.WriteLine("Usage:");
            Console.WriteLine(" DiskTools COMMAND File [Options]");
            Console.WriteLine("  COMMAND: The command to run");
            Console.WriteLine("  Fie: The path to the file to work on");
            Console.WriteLine("");
            Console.WriteLine("COMMAND List");
            Console.WriteLine("  INFO File: Returns info on the file.");
            Console.WriteLine("  ADD File FilePath DestPath: Adds a file to the image.");
            Console.WriteLine("  COPY File SrcFile [DestPath]: Copies the file out of the image.");
            Console.WriteLine("  DIR File SrcPath: List all the files in the folder");
            Console.WriteLine();
            Console.WriteLine("All Paths into the virtual disk need to be prefaced by the partition ID");
            Console.WriteLine(" IE (hd1)\\boot\\");
        }

        PathInfo ParseImagePath(string InputPath)
        {
            PathInfo Ret;

            Ret.Partition = -1;
            Ret.Folder = @"\";
            Ret.File = "";
            Ret.FilePath = @"\";

            // First off lets find the partition information
            if (InputPath.StartsWith("(hd", StringComparison.CurrentCultureIgnoreCase))
            {
                int End = InputPath.IndexOf(')');
                string Temp = InputPath.Substring(3, End - 3);
                if (!int.TryParse(Temp, out Ret.Partition))
                {
                    Console.WriteLine("ERROR: Unknown Partition in path {0}", InputPath);
                    return Ret;
                }

                // Get the rest of the string
                Ret.FilePath = InputPath.Substring(End + 1);
            }
            else
            {
                Ret.FilePath = InputPath;
            }

            Ret.Folder = Path.GetDirectoryName(Ret.FilePath);
            Ret.File = Path.GetFileName(Ret.FilePath);

            if (Ret.Folder == null)
                Ret.Folder = @"\";

            return Ret;
        }

        bool PrintDirectory(DiscUtils.DiscDirectoryInfo DirInfo, string Pattern, bool Recurse)
        {
            if(!DirInfo.Exists)
            {
                Console.WriteLine("Unknown Folder {0}", DirInfo.FullName);
                return false;
            }

            List<DiscUtils.DiscFileSystemInfo> FolderList = new List<DiscUtils.DiscFileSystemInfo>( DirInfo.GetDirectories(Pattern));
            FolderList.AddRange(DirInfo.GetFiles(Pattern));
            FolderList.Sort((LHS, RHS) =>
            {
                return LHS.Name.CompareTo(RHS.Name);
            });

            Console.WriteLine(); 
            Console.WriteLine(" Directory of {0}", DirInfo.FullName);
            Console.WriteLine();
            foreach (var Entry in FolderList)
            {
                string Date = Entry.LastWriteTime.ToString("MM/dd/yyyy");
                string Time = Entry.LastWriteTime.ToString("hh:mm tt");
                string Type = "";
                string Size = "";

                if ((Entry.Attributes & FileAttributes.Directory) == FileAttributes.Directory)
                {
                    Type = "<DIR>";
                }
                else
                {
                    DiscUtils.DiscFileInfo FileInfo = (DiscUtils.DiscFileInfo)Entry;
                    Size = FileInfo.Length.ToString("0,0");
                }

                Console.WriteLine("{0}  {1} {2,8} {3,8} {4}", Date, Time, Type, Size, Entry.Name);
            }

            if (Recurse)
            {
                foreach (var Entry in FolderList)
                {
                    if ((Entry.Attributes & FileAttributes.Directory) == FileAttributes.Directory)
                    {
                        if (!PrintDirectory((DiscUtils.DiscDirectoryInfo)Entry, Pattern, true))
                            return false;
                    }
                }
            }

            return true;
        }

        int ExecuteCommand(string[] args)
        {
            CommandType Command = CommandType.None;
            System.IO.FileAccess Access = FileAccess.Read;

            if (args.Length >= 2)
            {
                if (args[0].Equals("info", StringComparison.InvariantCultureIgnoreCase))
                {
                    Command = CommandType.Info;
                }
                else if (args[0].Equals("add", StringComparison.InvariantCultureIgnoreCase))
                {
                    Command = CommandType.Add;
                    Access = FileAccess.ReadWrite;
                }
                else if (args[0].Equals("copy", StringComparison.InvariantCultureIgnoreCase))
                {
                    Command = CommandType.Copy;
                }
                else if (args[0].Equals("dir", StringComparison.InvariantCultureIgnoreCase))
                {
                    Command = CommandType.Dir;
                }
                else
                {
                    Console.WriteLine("Unknown Command: {0}", args[0]);
                }
            }

            if (Command == CommandType.None)
            {
                Usage();
                return 0;
            }

            DiscUtils.VirtualDisk CurrentDisk;

            try
            {
                CurrentDisk = DiscUtils.VirtualDisk.OpenDisk(args[1], Access);
            }
            catch (Exception e)
            {
                Console.WriteLine("ERROR: Can not open disk {0} ({1})", args[1], e.Message);
                return -1;
            }
            
            if (Command == CommandType.Info)
            {
                Console.WriteLine("Disk       {0}", Path.GetFileName(args[1]));
                Console.WriteLine("Type       {0}", CurrentDisk.DiskTypeInfo.Name);
                Console.WriteLine("Capacity   {0} bytes", CurrentDisk.Capacity.ToString("0,0"));
                Console.WriteLine("Block Size {0} bytes", CurrentDisk.BlockSize);
                Console.WriteLine("Signature  {0}", CurrentDisk.Signature.ToString("X"));
                Console.WriteLine("Sectors    {0}", CurrentDisk.Geometry.TotalSectors.ToString("0,0"));

                if (CurrentDisk.DiskTypeInfo.CanBeHardDisk == true)
                {
                    if (CurrentDisk.Partitions == null)
                    {
                        Console.WriteLine("Partitions 0");
                        return 0;
                    }

                    Console.WriteLine("Partitions {0}", CurrentDisk.Partitions.Count);

                    for (int x = 0; x < CurrentDisk.Partitions.Count; x++)
                    {
                        long TotalSize = CurrentDisk.Partitions[x].SectorCount * CurrentDisk.BlockSize;

                        Console.WriteLine();
                        Console.WriteLine("Partition ID  (hd{0})", x);
                        Console.WriteLine("Type          {0} [0x{1}]", CurrentDisk.Partitions[x].TypeAsString, CurrentDisk.Partitions[x].BiosType.ToString("X2"));
                        Console.WriteLine("First Sector  {0}", CurrentDisk.Partitions[x].FirstSector.ToString("0,0"));
                        Console.WriteLine("Last Sector   {0}", CurrentDisk.Partitions[x].LastSector.ToString("0,0"));
                        Console.WriteLine("Total Sectors {0}", CurrentDisk.Partitions[x].SectorCount.ToString("0,0"));
                        Console.WriteLine("Size          {0} bytes", TotalSize.ToString("0,0"));

                        DiscUtils.FileSystemInfo[] FileSystemInfo = DiscUtils.FileSystemManager.DetectDefaultFileSystems(CurrentDisk.Partitions[x].Open());
                        foreach (var Info in FileSystemInfo)
                        {
                            Console.WriteLine("Name          {0}", Info.Name);
                            Console.WriteLine("Description   {0}", Info.Description);
                        }

                    }
                }
                else
                {
                    DiscUtils.FileSystemInfo[] FileSystemInfo = DiscUtils.FileSystemManager.DetectDefaultFileSystems(CurrentDisk.Content);
                    foreach (var Info in FileSystemInfo)
                    {
                        Console.WriteLine();
                        Console.WriteLine("Partition ID  (hd0)");
                        Console.WriteLine("Name          {0}", Info.Name);
                        Console.WriteLine("Description   {0}", Info.Description);                        
                    }
                }
            }

            else if (Command == CommandType.Dir)
            {
                PathInfo PI;

                PI.Partition = -1;
                PI.FilePath = "";
                PI.File = "";
                PI.Folder = "";

                if (args.Length > 2)
                    PI = ParseImagePath(args[2]);

                if (string.IsNullOrEmpty(PI.File))
                    PI.File = "*.*";
                
                DiscUtils.FileSystemInfo[] FileSystemInfo = null;

                if (CurrentDisk.DiskTypeInfo.CanBeHardDisk)
                {
                    if (CurrentDisk.Partitions == null || CurrentDisk.Partitions.Count <= PI.Partition)
                    {
                        Console.WriteLine("ERROR: Unknown Partition {0}", args[2]);
                        return -1;
                    }

                    FileSystemInfo = DiscUtils.FileSystemManager.DetectDefaultFileSystems(CurrentDisk.Partitions[PI.Partition].Open());

                    foreach (var Info in FileSystemInfo)
                    {
                        DiscUtils.DiscFileSystem FileSystem = Info.Open(CurrentDisk.Partitions[PI.Partition].Open());
                        var DirInfo = FileSystem.GetDirectoryInfo(PI.Folder);

                        PrintDirectory(DirInfo, PI.File, false);
                    }
                }
                else
                {
                    FileSystemInfo = DiscUtils.FileSystemManager.DetectDefaultFileSystems(CurrentDisk.Content);

                    foreach (var Info in FileSystemInfo)
                    {
                        DiscUtils.DiscFileSystem FileSystem = Info.Open(CurrentDisk.Content);
                        var DirInfo = FileSystem.GetDirectoryInfo(PI.Folder);

                        PrintDirectory(DirInfo, PI.File, false);
                    }
                }


            }

            else if (Command == CommandType.Copy)
            {
                PathInfo PI = default(PathInfo);
                PI.Partition = -1;
                if (args.Length > 2)
                    PI = ParseImagePath(args[2]);

                if (CurrentDisk.Partitions == null || CurrentDisk.Partitions.Count <= PI.Partition || PI.Partition == -1)
                {
                    Console.WriteLine("ERROR: Unknown Partition {0}", args[2]);
                    return -1;
                }

                DiscUtils.FileSystemInfo[] FileSystemInfo = DiscUtils.FileSystemManager.DetectDefaultFileSystems(CurrentDisk.Partitions[PI.Partition].Open());

                foreach (var Info in FileSystemInfo)
                {
                    DiscUtils.DiscFileSystem FileSystem = Info.Open(CurrentDisk.Partitions[PI.Partition].Open());
                    if (PI.FilePath == null || !FileSystem.FileExists(PI.FilePath))
                    {
                        Console.WriteLine("ERROR: Unknown File {0}", PI.FilePath);
                        return -1;
                    }

                    StreamReader FileData = new StreamReader(FileSystem.OpenFile(PI.FilePath, FileMode.Open));

                    if (args.Length <= 3)
                    {
                        // Copy to con
                        string Data = FileData.ReadToEnd();
                        Console.Write(Data);
                    }
                    else
                    {
                        Console.WriteLine("Copying {0} to {1}", PI.FilePath, args[3]);
                        
                        using (StreamWriter OutData = new StreamWriter(File.Open(args[3], FileMode.Create)))
                        {
                            FileData.BaseStream.CopyTo(OutData.BaseStream);
                        }                        
                    }
                }

            }
            else if (Command == CommandType.Add)
            {
                PathInfo PI = default(PathInfo);
                PI.Partition = -1;
                if (args.Length > 3)
                    PI = ParseImagePath(args[3]);

                if (CurrentDisk.Partitions == null || CurrentDisk.Partitions.Count <= PI.Partition || PI.Partition == -1)
                {
                    Console.WriteLine("ERROR: Unknown Partition {0}", args[3]);
                    return -1;
                }

                if (!File.Exists(args[2]))
                {
                    Console.WriteLine("ERROR: Missing Input File {0}", args[2]);
                    return -1;
                }
                                
                DiscUtils.FileSystemInfo[] FileSystemInfo = DiscUtils.FileSystemManager.DetectDefaultFileSystems(CurrentDisk.Partitions[PI.Partition].Open());

                foreach (var Info in FileSystemInfo)
                {
                    DiscUtils.DiscFileSystem FileSystem = Info.Open(CurrentDisk.Partitions[PI.Partition].Open());
                    string folder = System.IO.Path.GetDirectoryName(PI.FilePath);

                    if (!FileSystem.CanWrite)
                    {
                        Console.WriteLine("ERROR: Can not write to file system {0}", FileSystem.FriendlyName);
                        return -1;
                    }

                    using (StreamReader FileData = new StreamReader(File.Open(args[2], FileMode.Open)))
                    {
                        using (StreamWriter OutData = new StreamWriter(FileSystem.OpenFile(PI.FilePath, FileMode.Create)))
                        {
                            FileData.BaseStream.CopyTo(OutData.BaseStream);
                        }
                    }
                }

            }            
            return 0;
        }

        static int Main(string[] args)
        {
            Program MyApp = new Program();
            return MyApp.ExecuteCommand(args);
        }

    }
}
