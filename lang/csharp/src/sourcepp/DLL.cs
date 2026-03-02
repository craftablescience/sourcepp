using System.Runtime.InteropServices;

namespace sourcepp.sourcepp;

internal static partial class DLL
{
	private const string Name = "sourcepp_sourceppc";

	[StructLayout(LayoutKind.Sequential)]
	internal unsafe struct Buffer
	{
		public sbyte* Data;
		public long Size;
	}

	[LibraryImport(Name)]
	public static partial Buffer sourcepp_buffer_new(ulong size);

	[LibraryImport(Name)]
	public static partial void sourcepp_buffer_free(ref Buffer buf);

	[StructLayout(LayoutKind.Sequential)]
	internal struct String
	{
		public nint Data;
		public long Size;
	}

	[LibraryImport(Name)]
	public static partial String sourcepp_string_new(ulong size);

	[LibraryImport(Name)]
	public static partial void sourcepp_string_free(ref String str);

	[StructLayout(LayoutKind.Sequential)]
	internal struct StringArray
	{
		public unsafe nint* Data;
		public long Size;
	}

	[LibraryImport(Name)]
	public static partial String sourcepp_string_array_new(ulong size);

	[LibraryImport(Name)]
	public static partial void sourcepp_string_array_free(ref StringArray arr);
}
