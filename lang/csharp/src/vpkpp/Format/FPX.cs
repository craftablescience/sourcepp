using System;
using System.Runtime.InteropServices;

namespace vpkpp.Format
{
    using EntryCallback = Action<string, Entry>;

    internal static unsafe partial class Extern
    {
        [DllImport("vpkppc")]
        public static extern void* vpkpp_fpx_create([MarshalAs(UnmanagedType.LPStr)] string path);

        [DllImport("vpkppc")]
        public static extern void* vpkpp_fpx_open([MarshalAs(UnmanagedType.LPStr)] string path, IntPtr callback);
    }

    public class FPX : PackFile
    {
        private protected unsafe FPX(void* handle) : base(handle) {}

        public static FPX? Create(string path)
        {
            unsafe
            {
                var handle = Extern.vpkpp_fpx_create(path);
                return handle == null ? null : new FPX(handle);
            }
        }

        public new static FPX? Open(string path)
        {
            unsafe
            {
                var handle = Extern.vpkpp_fpx_open(path, 0);
                return handle == null ? null : new FPX(handle);
            }
        }

        public new static FPX? Open(string path, EntryCallback callback)
        {
            unsafe
            {
                EntryCallbackNative callbackNative = (path, entry) =>
                {
                    callback(path, new Entry(entry, true));
                };
                var handle = Extern.vpkpp_fpx_open(path, Marshal.GetFunctionPointerForDelegate(callbackNative));
                return handle == null ? null : new FPX(handle);
            }
        }
    }
}
