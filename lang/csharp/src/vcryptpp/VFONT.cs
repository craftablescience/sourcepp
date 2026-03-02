using System.Collections.Generic;
using System.Linq;

namespace sourcepp.vcryptpp;

public static class VFONT
{
	public static byte[] Encrypt(byte[] buffer, byte saltLen = 2)
	{
		return new sourcepp.Buffer(DLL.vcryptpp_vfont_encrypt(buffer, (ulong) buffer.LongLength, saltLen)).Read<byte>();
	}

	public static byte[] Encrypt(IEnumerable<byte> buffer, byte saltLen = 2)
	{
		return Encrypt(buffer.ToArray(), saltLen);
	}

	public static byte[] Decrypt(byte[] buffer)
	{
		return new sourcepp.Buffer(DLL.vcryptpp_vfont_decrypt(buffer, (ulong) buffer.LongLength)).Read<byte>();
	}

	public static byte[] Decrypt(IEnumerable<byte> buffer)
	{
		return Decrypt(buffer.ToArray());
	}
}
