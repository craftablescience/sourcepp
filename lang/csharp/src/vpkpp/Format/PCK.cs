using System;
using System.Runtime.InteropServices;

namespace vpkpp.Format
{
    using EntryCallback = Action<string, Entry>;

    internal static unsafe partial class Extern
    {
        [DllImport("vpkppc")]
        public static extern void* vpkpp_pck_create([MarshalAs(UnmanagedType.LPStr)] string path);

        [DllImport("vpkppc")]
        public static extern void* vpkpp_pck_create_with_options([MarshalAs(UnmanagedType.LPStr)] string path, uint version, uint godotMajorVersion, uint godotMinorVersion, uint godotPatchVersion);

        [DllImport("vpkppc")]
        public static extern void* vpkpp_pck_open([MarshalAs(UnmanagedType.LPStr)] string path, IntPtr callback);
    }

    public class PCK : PackFile
    {
        private protected unsafe PCK(void* handle) : base(handle) {}

        public static PCK? Create(string path)
        {
            unsafe
            {
                var handle = Extern.vpkpp_pck_create(path);
                return handle == null ? null : new PCK(handle);
            }
        }

        public static PCK? Create(string path, uint version, uint godotMajorVersion, uint godotMinorVersion, uint godotPatchVersion)
        {
            unsafe
            {
                var handle = Extern.vpkpp_pck_create_with_options(path, version, godotMajorVersion, godotMinorVersion, godotPatchVersion);
                return handle == null ? null : new PCK(handle);
            }
        }

        public new static PCK? Open(string path)
        {
            unsafe
            {
                var handle = Extern.vpkpp_pck_open(path, 0);
                return handle == null ? null : new PCK(handle);
            }
        }

        public new static PCK? Open(string path, EntryCallback callback)
        {
            unsafe
            {
                EntryCallbackNative callbackNative = (path, entry) =>
                {
                    callback(path, new Entry(entry, true));
                };
                var handle = Extern.vpkpp_pck_open(path, Marshal.GetFunctionPointerForDelegate(callbackNative));
                return handle == null ? null : new PCK(handle);
            }
        }
    }
}
