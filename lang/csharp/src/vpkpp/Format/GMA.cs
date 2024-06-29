using System.Runtime.InteropServices;

namespace vpkpp.Format
{
    internal static unsafe partial class Extern
    {
        [DllImport("vpkppc")]
        public static extern void* vpkpp_gma_open([MarshalAs(UnmanagedType.LPStr)] string path);

        [DllImport("vpkppc")]
        public static extern void* vpkpp_gma_open_with_options([MarshalAs(UnmanagedType.LPStr)] string path, PackFileOptions options);
    }

    public class GMA : PackFile
    {
        private protected unsafe GMA(void* handle) : base(handle) {}

        public new static GMA? Open(string path)
        {
            unsafe
            {
                var handle = Extern.vpkpp_gma_open(path);
                return handle == null ? null : new GMA(handle);
            }
        }

        public new static GMA? Open(string path, PackFileOptions options)
        {
            unsafe
            {
                var handle = Extern.vpkpp_gma_open_with_options(path, options);
                return handle == null ? null : new GMA(handle);
            }
        }
    }
}
