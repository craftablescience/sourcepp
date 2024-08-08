using System.Runtime.InteropServices;

namespace vpkpp.Format
{
    internal static unsafe partial class Extern
    {
        [DllImport("vpkppc")]
        public static extern void* vpkpp_vpk_vtmb_open([MarshalAs(UnmanagedType.LPStr)] string path);

        [DllImport("vpkppc")]
        public static extern void* vpkpp_vpk_vtmb_open_with_options([MarshalAs(UnmanagedType.LPStr)] string path, PackFileOptions options);
    }

    public class VPK_VTMB : PackFile
    {
        private protected unsafe VPK_VTMB(void* handle) : base(handle) {}

        public new static VPK_VTMB? Open(string path)
        {
            unsafe
            {
                var handle = Extern.vpkpp_vpk_vtmb_open(path);
                return handle == null ? null : new VPK_VTMB(handle);
            }
        }

        public new static VPK_VTMB? Open(string path, PackFileOptions options)
        {
            unsafe
            {
                var handle = Extern.vpkpp_vpk_vtmb_open_with_options(path, options);
                return handle == null ? null : new VPK_VTMB(handle);
            }
        }
    }
}
