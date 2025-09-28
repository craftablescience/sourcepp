using System.Runtime.InteropServices;

namespace vpkpp
{
	public enum EntryCompressionType : short
	{
		NO_OVERRIDE = -1,
		NO_COMPRESS = 0,
		DEFLATE = 8,
		BZIP2 = 12,
		LZMA = 14,
		ZSTD = 93,
		XZ = 95,
	}

    [StructLayout(LayoutKind.Sequential)]
    public struct BakeOptions
    {
		public EntryCompressionType zip_compressionTypeOverride;
		public short zip_compressionStrength;
		public byte gma_writeCRCs;
		public byte vpk_generateMD5Entries;
	}

    [StructLayout(LayoutKind.Sequential)]
    public struct EntryOptions
    {
		public EntryCompressionType zip_compressionType;
		public short zip_compressionStrength;
		public ushort vpk_preloadBytes;
		public byte vpk_saveToDirectory;
	}
}
