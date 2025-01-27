using System;
using System.Runtime.InteropServices;

namespace vpkpp
{
    internal static unsafe partial class Extern
    {
		internal static unsafe partial class Entry
		{
			[LibraryImport("sourcepp_vpkppc", EntryPoint = "vpkpp_entry_get_flags")]
			public static partial uint GetFlags(void* handle);

			[LibraryImport("sourcepp_vpkppc", EntryPoint = "vpkpp_entry_get_archive_index")]
			public static partial uint GetArchiveIndex(void* handle);

			[LibraryImport("sourcepp_vpkppc", EntryPoint = "vpkpp_entry_get_length")]
			public static partial ulong GetLength(void* handle);

			[LibraryImport("sourcepp_vpkppc", EntryPoint = "vpkpp_entry_get_compressed_length")]
			public static partial ulong GetCompressedLength(void* handle);

			[LibraryImport("sourcepp_vpkppc", EntryPoint = "vpkpp_entry_get_offset")]
			public static partial ulong GetOffset(void* handle);

			[LibraryImport("sourcepp_vpkppc", EntryPoint = "vpkpp_entry_get_extra_data")]
			public static partial sourcepp.Buffer GetExtraData(void* handle);

			[LibraryImport("sourcepp_vpkppc", EntryPoint = "vpkpp_entry_get_crc32")]
			public static partial uint GetCRC32(void* handle);

			[LibraryImport("sourcepp_vpkppc", EntryPoint = "vpkpp_entry_is_unbaked")]
			public static partial int IsUnbaked(void* handle);

			[LibraryImport("sourcepp_vpkppc", EntryPoint = "vpkpp_entry_free")]
			public static partial void Free(void** handle);
		}
    }

    public class Entry
    {
        internal unsafe Entry(void* handle, bool managed)
        {
            Handle = handle;
            _managed = managed;
        }

        ~Entry()
        {
            if (!_managed)
            {
                unsafe
                {
                    fixed (void** handlePtr = &Handle)
                    {
                        Extern.Entry.Free(handlePtr);
                    }
                }
            }
        }

        public uint Flags
        {
            get
            {
                unsafe
                {
                    return Extern.Entry.GetFlags(Handle);
                }
            }
        }

        public uint ArchiveIndex
        {
            get
            {
                unsafe
                {
                    return Extern.Entry.GetArchiveIndex(Handle);
                }
            }
        }

        public ulong Length
        {
            get
            {
                unsafe
                {
                    return Extern.Entry.GetLength(Handle);
                }
            }
        }

        public ulong CompressedLength
        {
            get
            {
                unsafe
                {
                    return Extern.Entry.GetCompressedLength(Handle);
                }
            }
        }

        public ulong Offset
        {
            get
            {
                unsafe
                {
                    return Extern.Entry.GetOffset(Handle);
                }
            }
        }

        public byte[] ExtraData
        {
            get
            {
                unsafe
                {
                    var buffer = Extern.Entry.GetExtraData(Handle);
                    return sourcepp.BufferUtils.ConvertToArrayAndDelete(ref buffer);
                }
            }
        }

        public uint CRC32
        {
            get
            {
                unsafe
                {
                    return Extern.Entry.GetCRC32(Handle);
                }
            }
        }

        public bool Unbaked
        {
            get
            {
                unsafe
                {
                    return Convert.ToBoolean(Extern.Entry.IsUnbaked(Handle));
                }
            }
        }

        internal unsafe void* Handle;

        private readonly bool _managed;
    }
}
