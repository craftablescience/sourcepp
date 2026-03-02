using System;
using System.Runtime.InteropServices;

namespace sourcepp.vcryptpp;

internal static partial class DLL
{
	private const string Name = "sourcepp_vcryptppc";

	[LibraryImport(Name)]
	public static partial sourcepp.DLL.Buffer vcryptpp_vfont_encrypt(ReadOnlySpan<byte> buffer, ulong bufferLen, byte saltLen);

	[LibraryImport(Name)]
	public static partial sourcepp.DLL.Buffer vcryptpp_vfont_decrypt(ReadOnlySpan<byte> buffer, ulong bufferLen);

	[LibraryImport(Name, StringMarshalling = StringMarshalling.Utf8)]
	public static partial sourcepp.DLL.Buffer vcryptpp_vice_decrypt(ReadOnlySpan<byte> buffer, ulong bufferLen, string code);

	[LibraryImport(Name, StringMarshalling = StringMarshalling.Utf8)]
	public static partial sourcepp.DLL.Buffer vcryptpp_vice_encrypt(ReadOnlySpan<byte> buffer, ulong bufferLen, string code);
}
