using System;
using System.Runtime.InteropServices;

namespace vpkpp.Format
{
    using EntryCallback = Action<string, Entry>;

    internal static unsafe partial class Extern
    {
        [DllImport("vpkppc")]
        public static extern void* vpkpp_zip_create([MarshalAs(UnmanagedType.LPStr)] string path);

        [DllImport("vpkppc")]
        public static extern void* vpkpp_zip_open([MarshalAs(UnmanagedType.LPStr)] string path, IntPtr callback);
    }

    public class ZIP : PackFile
    {
        private protected unsafe ZIP(void* handle) : base(handle) {}

        public static ZIP? Create(string path)
        {
            unsafe
            {
                var handle = Extern.vpkpp_zip_create(path);
                return handle == null ? null : new ZIP(handle);
            }
        }

        public new static ZIP? Open(string path)
        {
            unsafe
            {
                var handle = Extern.vpkpp_zip_open(path, 0);
                return handle == null ? null : new ZIP(handle);
            }
        }

        public new static ZIP? Open(string path, EntryCallback callback)
        {
            unsafe
            {
                EntryCallbackNative callbackNative = (path, entry) =>
                {
                    callback(path, new Entry(entry, true));
                };
                var handle = Extern.vpkpp_zip_open(path, Marshal.GetFunctionPointerForDelegate(callbackNative));
                return handle == null ? null : new ZIP(handle);
            }
        }
    }
}
