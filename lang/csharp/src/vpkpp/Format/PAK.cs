using System.Runtime.InteropServices;

namespace vpkpp.Format
{
    internal static unsafe partial class Extern
    {
        [DllImport("vpkppc")]
        public static extern void* vpkpp_pak_open([MarshalAs(UnmanagedType.LPStr)] string path);

        [DllImport("vpkppc")]
        public static extern void* vpkpp_pak_open_with_options([MarshalAs(UnmanagedType.LPStr)] string path, PackFileOptions options);
    }

    public class PAK : PackFile
    {
        private protected unsafe PAK(void* handle) : base(handle) {}

        public new static PAK? Open(string path)
        {
            unsafe
            {
                var handle = Extern.vpkpp_pak_open(path);
                return handle == null ? null : new PAK(handle);
            }
        }

        public new static PAK? Open(string path, PackFileOptions options)
        {
            unsafe
            {
                var handle = Extern.vpkpp_pak_open_with_options(path, options);
                return handle == null ? null : new PAK(handle);
            }
        }
    }
}
