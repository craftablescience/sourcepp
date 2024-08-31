using System;
using System.Runtime.InteropServices;

namespace vpkpp.Format
{
    using EntryCallback = Action<string, Entry>;

    internal static unsafe partial class Extern
    {
        [DllImport("vpkppc")]
        public static extern void* vpkpp_bsp_open([MarshalAs(UnmanagedType.LPStr)] string path, IntPtr callback);
    }

    public class BSP : PackFile
    {
        private protected unsafe BSP(void* handle) : base(handle) {}

        public new static BSP? Open(string path)
        {
            unsafe
            {
                var handle = Extern.vpkpp_bsp_open(path, 0);
                return handle == null ? null : new BSP(handle);
            }
        }

        public new static BSP? Open(string path, EntryCallback callback)
        {
            unsafe
            {
                EntryCallbackNative callbackNative = (path, entry) =>
                {
                    callback(path, new Entry(entry, true));
                };
                var handle = Extern.vpkpp_bsp_open(path, Marshal.GetFunctionPointerForDelegate(callbackNative));
                return handle == null ? null : new BSP(handle);
            }
        }
    }
}
