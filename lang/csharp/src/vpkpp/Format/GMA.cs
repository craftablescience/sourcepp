using System;
using System.Runtime.InteropServices;

namespace vpkpp.Format
{
    using EntryCallback = Action<string, Entry>;

    internal static unsafe partial class Extern
    {
        [DllImport("vpkppc")]
        public static extern void* vpkpp_gma_open([MarshalAs(UnmanagedType.LPStr)] string path, IntPtr callback);
    }

    public class GMA : PackFile
    {
        private protected unsafe GMA(void* handle) : base(handle) {}

        public new static GMA? Open(string path)
        {
            unsafe
            {
                var handle = Extern.vpkpp_gma_open(path, 0);
                return handle == null ? null : new GMA(handle);
            }
        }

        public new static GMA? Open(string path, EntryCallback callback)
        {
            unsafe
            {
                EntryCallbackNative callbackNative = (path, entry) =>
                {
                    callback(path, new Entry(entry, true));
                };
                var handle = Extern.vpkpp_gma_open(path, Marshal.GetFunctionPointerForDelegate(callbackNative));
                return handle == null ? null : new GMA(handle);
            }
        }
    }
}
