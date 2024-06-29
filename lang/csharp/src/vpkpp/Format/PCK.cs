using System.Runtime.InteropServices;

namespace vpkpp.Format
{
    internal static unsafe partial class Extern
    {
        [DllImport("vpkppc")]
        public static extern void* vpkpp_pck_open([MarshalAs(UnmanagedType.LPStr)] string path);

        [DllImport("vpkppc")]
        public static extern void* vpkpp_pck_open_with_options([MarshalAs(UnmanagedType.LPStr)] string path, PackFileOptions options);
    }

    public class PCK : PackFile
    {
        private protected unsafe PCK(void* handle) : base(handle) {}

        public new static PCK? Open(string path)
        {
            unsafe
            {
                var handle = Extern.vpkpp_pck_open(path);
                return handle == null ? null : new PCK(handle);
            }
        }

        public new static PCK? Open(string path, PackFileOptions options)
        {
            unsafe
            {
                var handle = Extern.vpkpp_pck_open_with_options(path, options);
                return handle == null ? null : new PCK(handle);
            }
        }
    }
}
