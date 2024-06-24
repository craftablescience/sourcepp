using System.Runtime.InteropServices;

namespace vpkpp.Format
{
    internal static unsafe partial class Extern
    {
        [DllImport("vpkppc")]
        public static extern void* vpkpp_bsp_open([MarshalAs(UnmanagedType.LPStr)] string path);

        [DllImport("vpkppc")]
        public static extern void* vpkpp_bsp_open_with_options([MarshalAs(UnmanagedType.LPStr)] string path, PackFileOptions options);
    }

    public class BSP : PackFile
    {
        private protected unsafe BSP(void* handle) : base(handle) {}

        public new static BSP? Open(string path)
        {
            unsafe
            {
                var handle = Extern.vpkpp_bsp_open(path);
                return handle == null ? null : new BSP(handle);
            }
        }

        public new static BSP? Open(string path, PackFileOptions options)
        {
            unsafe
            {
                var handle = Extern.vpkpp_bsp_open_with_options(path, options);
                return handle == null ? null : new BSP(handle);
            }
        }
    }
}
