using System.Runtime.InteropServices;

namespace vpkpp.Format
{
    internal static unsafe partial class Extern
    {
        [DllImport("vpkppc")]
        public static extern void* vpkpp_grp_open([MarshalAs(UnmanagedType.LPStr)] string path);

        [DllImport("vpkppc")]
        public static extern void* vpkpp_grp_open_with_options([MarshalAs(UnmanagedType.LPStr)] string path, PackFileOptions options);
    }

    public class GRP : PackFile
    {
        private protected unsafe GRP(void* handle) : base(handle) {}

        public new static GRP? Open(string path)
        {
            unsafe
            {
                var handle = Extern.vpkpp_grp_open(path);
                return handle == null ? null : new GRP(handle);
            }
        }

        public new static GRP? Open(string path, PackFileOptions options)
        {
            unsafe
            {
                var handle = Extern.vpkpp_grp_open_with_options(path, options);
                return handle == null ? null : new GRP(handle);
            }
        }
    }
}
