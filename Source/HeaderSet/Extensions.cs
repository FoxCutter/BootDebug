using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.InteropServices;

namespace PEUtils
{
    public static class BinaryReaderExtensions
    {
        public static T ReadObject<T>(this System.IO.BinaryReader DataStream)
        {

            byte[] RawData = DataStream.ReadBytes(System.Runtime.InteropServices.Marshal.SizeOf(typeof(T)));

            GCHandle PinnedData = GCHandle.Alloc(RawData, GCHandleType.Pinned);
            T Data = (T)System.Runtime.InteropServices.Marshal.PtrToStructure(PinnedData.AddrOfPinnedObject(), typeof(T));
            PinnedData.Free();

            return Data;
        }

        public static T[] ReadArray<T>(this System.IO.BinaryReader DataStream, int DataLength)
        {
            int RawLen = System.Runtime.InteropServices.Marshal.SizeOf(typeof(T));
            if (DataLength == 0)
                return null;

            T[] Data = new T[DataLength];
            byte[] RawData = DataStream.ReadBytes(RawLen * DataLength);

            GCHandle PinnedData = GCHandle.Alloc(RawData, GCHandleType.Pinned);

            // We have have convert each entry in the array one at a time.
            for (int x = 0, Pos = 0; x < DataLength; x++, Pos += RawLen)
            {
                Data[x] = (T)System.Runtime.InteropServices.Marshal.PtrToStructure(PinnedData.AddrOfPinnedObject() + Pos, typeof(T));
            }
            PinnedData.Free();

            return Data;
        }
    }
}
