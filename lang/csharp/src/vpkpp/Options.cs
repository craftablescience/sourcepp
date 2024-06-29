using System.Runtime.InteropServices;

namespace vpkpp
{
    [StructLayout(LayoutKind.Sequential)]
    public struct PackFileOptions
    {
        public byte gma_writeCRCs;

        public uint vpk_version;

        public uint vpk_preferredChunkSize;

        public byte vpk_generateMD5Entries;

        public ushort zip_compressionMethod;
    }
}
