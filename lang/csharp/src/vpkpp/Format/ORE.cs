using System;
using System.Runtime.InteropServices;

namespace vpkpp.Format
{
    using EntryCallback = Action<string, Entry>;

    internal static unsafe partial class Extern
    {
        [DllImport("vpkppc")]
        public static extern void* vpkpp_ore_create([MarshalAs(UnmanagedType.LPStr)] string path);

        [DllImport("vpkppc")]
        public static extern void* vpkpp_ore_open([MarshalAs(UnmanagedType.LPStr)] string path, IntPtr callback);
    }

    public class ORE : PackFile
    {
        private protected unsafe ORE(void* handle) : base(handle) {}

        public static ORE? Create(string path)
        {
            unsafe
            {
                var handle = Extern.vpkpp_ore_create(path);
                return handle == null ? null : new ORE(handle);
            }
        }

        public new static ORE? Open(string path)
        {
            unsafe
            {
                var handle = Extern.vpkpp_ore_open(path, 0);
                return handle == null ? null : new ORE(handle);
            }
        }

        public new static ORE? Open(string path, EntryCallback callback)
        {
            unsafe
            {
                EntryCallbackNative callbackNative = (path, entry) =>
                {
                    callback(path, new Entry(entry, true));
                };
                var handle = Extern.vpkpp_ore_open(path, Marshal.GetFunctionPointerForDelegate(callbackNative));
                return handle == null ? null : new ORE(handle);
            }
        }
    }
}
