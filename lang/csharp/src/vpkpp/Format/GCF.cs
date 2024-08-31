using System;
using System.Runtime.InteropServices;

namespace vpkpp.Format
{
    using EntryCallback = Action<string, Entry>;

    internal static unsafe partial class Extern
    {
        [DllImport("vpkppc")]
        public static extern void* vpkpp_gcf_open([MarshalAs(UnmanagedType.LPStr)] string path, IntPtr callback);
    }

    public class GCF : PackFile
    {
        private protected unsafe GCF(void* handle) : base(handle) {}

        public new static GCF? Open(string path)
        {
            unsafe
            {
                var handle = Extern.vpkpp_gcf_open(path, 0);
                return handle == null ? null : new GCF(handle);
            }
        }

        public new static GCF? Open(string path, EntryCallback callback)
        {
            unsafe
            {
                EntryCallbackNative callbackNative = (path, entry) =>
                {
                    callback(path, new Entry(entry, true));
                };
                var handle = Extern.vpkpp_gcf_open(path, Marshal.GetFunctionPointerForDelegate(callbackNative));
                return handle == null ? null : new GCF(handle);
            }
        }
    }
}
