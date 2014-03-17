using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;

namespace PEUtils
{
    public enum Error
    {
        None,
        InvalidFile,
    }

    public class PEFile
    {
        public Error LastError { get; set; }
        
        public IMAGE_DOS_HEADER _DOSHeader;
        public IMAGE_NT_HEADERS _NTHeader;
        public IMAGE_OPTIONAL_HEADER32 _OptionalHeader;

        public List<IMAGE_DATA_DIRECTORY> _DataDirectory;
        public List<IMAGE_SECTION_HEADER> _SectionHeaders;

        public PEFile()
        {
            LastError = Error.None;

            _DataDirectory = new List<IMAGE_DATA_DIRECTORY>();
            _SectionHeaders = new List<IMAGE_SECTION_HEADER>();
        }

        public bool ReadFile(BinaryReader FileData)
        {
            try
            {
                // Seek to the start of the stream.
                FileData.BaseStream.Position = 0;

                // We'll start with the DOS file header
                _DOSHeader = FileData.ReadObject<IMAGE_DOS_HEADER>();
                if (_DOSHeader.e_magic != 0x5A4D)
                {
                    // The magic number is bad, so whatever this is it's not a PE file
                    LastError = Error.InvalidFile;
                    return false;
                }

                // While we COULD go through and validate the DOS header to make sure it's correct, we're not going to bother in this case.
                // It's mostly academic anyways.

                // Jump to the NT Header
                FileData.BaseStream.Position = (long)_DOSHeader.e_lfanew;

                // Load up the PE signature and Header information
                _NTHeader = FileData.ReadObject<IMAGE_NT_HEADERS>();
                if (_NTHeader.Signature != 0x00004550)
                {
                    // And the signature is wrong or missing, so shut 'er down.
                    LastError = Error.InvalidFile;
                    return false;
                }

                // And read in the optional header
                _OptionalHeader = FileData.ReadObject<IMAGE_OPTIONAL_HEADER32>();

                if (_OptionalHeader.Magic != OptionSignature.NT_32Bit)
                {
                    // We only work with 32 bit headers at the moment
                    LastError = Error.InvalidFile;
                    return false;
                }
                
                // As well as the directory data (which is also part of the optional header)
                _DataDirectory.AddRange(FileData.ReadArray<IMAGE_DATA_DIRECTORY>((int)_OptionalHeader.NumberOfRvaAndSizes));
                
                // The lastly the section headers
                _SectionHeaders.AddRange(FileData.ReadArray<IMAGE_SECTION_HEADER>((int)_NTHeader.FileHeader.NumberOfSections));
            }
            catch
            {
                // If we get any sort of read error the file isn't valid.
                return false;
            }

            return true;
        }
    }
}
