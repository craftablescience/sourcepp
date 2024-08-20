using System;
using System.Runtime.InteropServices;

namespace vpkpp
{
    internal static unsafe partial class Extern
    {
        [DllImport("vpkppc")]
        public static extern uint vpkpp_entry_get_flags(void* handle);

        [DllImport("vpkppc")]
        public static extern uint vpkpp_entry_get_archive_index(void* handle);

        [DllImport("vpkppc")]
        public static extern ulong vpkpp_entry_get_length(void* handle);

        [DllImport("vpkppc")]
        public static extern ulong vpkpp_entry_get_compressed_length(void* handle);

        [DllImport("vpkppc")]
        public static extern ulong vpkpp_entry_get_offset(void* handle);

        [DllImport("vpkppc")]
        public static extern Buffer vpkpp_entry_get_extra_data(void* handle);

        [DllImport("vpkppc")]
        public static extern uint vpkpp_entry_get_crc32(void* handle);

        [DllImport("vpkppc")]
        public static extern int vpkpp_entry_is_unbaked(void* handle);

        [DllImport("vpkppc")]
        public static extern void vpkpp_entry_free(void** handle);
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
                        Extern.vpkpp_entry_free(handlePtr);
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
                    return Extern.vpkpp_entry_get_flags(Handle);
                }
            }
        }

        public uint ArchiveIndex
        {
            get
            {
                unsafe
                {
                    return Extern.vpkpp_entry_get_archive_index(Handle);
                }
            }
        }

        public ulong Length
        {
            get
            {
                unsafe
                {
                    return Extern.vpkpp_entry_get_length(Handle);
                }
            }
        }

        public ulong CompressedLength
        {
            get
            {
                unsafe
                {
                    return Extern.vpkpp_entry_get_compressed_length(Handle);
                }
            }
        }

        public ulong Offset
        {
            get
            {
                unsafe
                {
                    return Extern.vpkpp_entry_get_offset(Handle);
                }
            }
        }

        public byte[] ExtraData
        {
            get
            {
                unsafe
                {
                    var buffer = Extern.vpkpp_entry_get_extra_data(Handle);
                    return BufferUtils.ConvertToArrayAndDelete(ref buffer);
                }
            }
        }

        public uint CRC32
        {
            get
            {
                unsafe
                {
                    return Extern.vpkpp_entry_get_crc32(Handle);
                }
            }
        }

        public bool Unbaked
        {
            get
            {
                unsafe
                {
                    return Convert.ToBoolean(Extern.vpkpp_entry_is_unbaked(Handle));
                }
            }
        }

        internal unsafe void* Handle;

        private readonly bool _managed;
    }
}
