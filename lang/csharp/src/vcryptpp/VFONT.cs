using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;

namespace vcryptpp
{
    internal static unsafe partial class Extern
    {
		internal static unsafe partial class VFONT
		{
			[LibraryImport("sourcepp_vcryptppc", EntryPoint = "vcryptpp_vfont_encrypt")]
			public static partial sourcepp.Buffer Encrypt(byte* buffer, ulong bufferLen, byte saltLen);

			[LibraryImport("sourcepp_vcryptppc", EntryPoint = "vcryptpp_vfont_decrypt")]
			public static partial sourcepp.Buffer Decrypt(byte* buffer, ulong bufferLen);
		}
    }

    public static class VFONT
    {
        public static byte[] Encrypt(byte[] buffer, byte saltLen)
        {
            unsafe
            {
                fixed (byte* bufferPtr = buffer)
                {
                    var ret = Extern.VFONT.Encrypt(bufferPtr, (ulong) buffer.LongLength, saltLen);
                    return sourcepp.BufferUtils.ConvertToArrayAndDelete(ref ret);
                }
            }
        }

        public static byte[] Encrypt(IEnumerable<byte> buffer, byte saltLen)
        {
            unsafe
            {
                var data = buffer.ToArray();
                fixed (byte* bufferPtr = data)
                {
                    var ret = Extern.VFONT.Encrypt(bufferPtr, (ulong) data.LongLength, saltLen);
                    return sourcepp.BufferUtils.ConvertToArrayAndDelete(ref ret);
                }
            }
        }

        public static byte[] Decrypt(byte[] buffer)
        {
            unsafe
            {
                fixed (byte* bufferPtr = buffer)
                {
                    var ret = Extern.VFONT.Decrypt(bufferPtr, (ulong) buffer.LongLength);
                    return sourcepp.BufferUtils.ConvertToArrayAndDelete(ref ret);
                }
            }
        }

        public static byte[] Decrypt(IEnumerable<byte> buffer)
        {
            unsafe
            {
                var data = buffer.ToArray();
                fixed (byte* bufferPtr = data)
                {
                    var ret = Extern.VFONT.Decrypt(bufferPtr, (ulong) data.LongLength);
                    return sourcepp.BufferUtils.ConvertToArrayAndDelete(ref ret);
                }
            }
        }
    }
}
