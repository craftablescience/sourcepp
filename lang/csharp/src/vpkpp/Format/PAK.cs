using System;
using System.Runtime.InteropServices;

namespace vpkpp.Format
{
    using EntryCallback = Action<string, Entry>;

    internal static unsafe partial class Extern
    {
        [DllImport("vpkppc")]
        public static extern void* vpkpp_pak_create([MarshalAs(UnmanagedType.LPStr)] string path);

        [DllImport("vpkppc")]
        public static extern void* vpkpp_pak_open([MarshalAs(UnmanagedType.LPStr)] string path, IntPtr callback);
    }

    public class PAK : PackFile
    {
        private protected unsafe PAK(void* handle) : base(handle) {}

        public static PAK? Create(string path)
        {
            unsafe
            {
                var handle = Extern.vpkpp_pak_create(path);
                return handle == null ? null : new PAK(handle);
            }
        }

        public new static PAK? Open(string path)
        {
            unsafe
            {
                var handle = Extern.vpkpp_pak_open(path, 0);
                return handle == null ? null : new PAK(handle);
            }
        }

        public new static PAK? Open(string path, EntryCallback callback)
        {
            unsafe
            {
                EntryCallbackNative callbackNative = (path, entry) =>
                {
                    callback(path, new Entry(entry, true));
                };
                var handle = Extern.vpkpp_pak_open(path, Marshal.GetFunctionPointerForDelegate(callbackNative));
                return handle == null ? null : new PAK(handle);
            }
        }
    }
}
