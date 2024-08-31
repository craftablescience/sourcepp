using System;
using System.Runtime.InteropServices;

namespace vpkpp.Format
{
    using EntryCallback = Action<string, Entry>;

    internal static unsafe partial class Extern
    {
        [DllImport("vpkppc")]
        public static extern void* vpkpp_vpk_vtmb_create([MarshalAs(UnmanagedType.LPStr)] string path);

        [DllImport("vpkppc")]
        public static extern void* vpkpp_vpk_vtmb_open([MarshalAs(UnmanagedType.LPStr)] string path, IntPtr callback);
    }

    public class VPK_VTMB : PackFile
    {
        private protected unsafe VPK_VTMB(void* handle) : base(handle) {}

        public static VPK_VTMB? Create(string path)
        {
            unsafe
            {
                var handle = Extern.vpkpp_vpk_vtmb_create(path);
                return handle == null ? null : new VPK_VTMB(handle);
            }
        }

        public new static VPK_VTMB? Open(string path)
        {
            unsafe
            {
                var handle = Extern.vpkpp_vpk_vtmb_open(path, 0);
                return handle == null ? null : new VPK_VTMB(handle);
            }
        }

        public new static VPK_VTMB? Open(string path, EntryCallback callback)
        {
            unsafe
            {
                EntryCallbackNative callbackNative = (path, entry) =>
                {
                    callback(path, new Entry(entry, true));
                };
                var handle = Extern.vpkpp_vpk_vtmb_open(path, Marshal.GetFunctionPointerForDelegate(callbackNative));
                return handle == null ? null : new VPK_VTMB(handle);
            }
        }
    }
}
