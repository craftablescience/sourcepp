using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;

namespace vcryptpp
{
    internal static unsafe partial class Extern
    {
		internal static unsafe partial class VFONT
		{
			[LibraryImport("vcryptppc", EntryPoint = "vcryptpp_vfont_decrypt")]
			public static partial sourcepp.Buffer Decrypt(byte* buffer, ulong bufferLen);
		}
    }

    public static class VFONT
    {
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
