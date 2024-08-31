using System.Runtime.InteropServices;

namespace vpkpp
{
    [StructLayout(LayoutKind.Sequential)]
    public struct BakeOptions
    {
        public int gma_writeCRCs;

        public int vpk_generateMD5Entries;
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct EntryOptions
    {
        public int vpk_saveToDirectory;

        public uint vpk_preloadBytes;
    }
}
