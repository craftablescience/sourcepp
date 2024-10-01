using System;
using System.Runtime.InteropServices;

namespace vpkpp.Format
{
    using EntryCallback = Action<string, Entry>;

    internal static unsafe partial class Extern
    {
        [DllImport("vpkppc")]
        public static extern void* vpkpp_wad3_create([MarshalAs(UnmanagedType.LPStr)] string path);

        [DllImport("vpkppc")]
        public static extern void* vpkpp_wad3_open([MarshalAs(UnmanagedType.LPStr)] string path, IntPtr callback);
    }

    public class WAD3 : PackFile
    {
        private protected unsafe WAD3(void* handle) : base(handle) {}

        public static WAD3? Create(string path)
        {
            unsafe
            {
                var handle = Extern.vpkpp_wad3_create(path);
                return handle == null ? null : new WAD3(handle);
            }
        }

        public new static WAD3? Open(string path)
        {
            unsafe
            {
                var handle = Extern.vpkpp_wad3_open(path, 0);
                return handle == null ? null : new WAD3(handle);
            }
        }

        public new static WAD3? Open(string path, EntryCallback callback)
        {
            unsafe
            {
                EntryCallbackNative callbackNative = (path, entry) =>
                {
                    callback(path, new Entry(entry, true));
                };
                var handle = Extern.vpkpp_wad3_open(path, Marshal.GetFunctionPointerForDelegate(callbackNative));
                return handle == null ? null : new WAD3(handle);
            }
        }
    }
}
