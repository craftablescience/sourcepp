using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;

namespace vcryptpp
{
    internal static unsafe partial class Extern
    {
        [DllImport("vcryptppc")]
        public static extern Buffer vcryptpp_vfont_decrypt(byte* buffer, ulong bufferLen);
    }

    public static class VFONT
    {
        public static byte[] Decrypt(byte[] buffer)
        {
            unsafe
            {
                fixed (byte* bufferPtr = buffer)
                {
                    Buffer ret = Extern.vcryptpp_vfont_decrypt(bufferPtr, (ulong) buffer.LongLength);
                    return BufferUtils.ConvertToArrayAndDelete(ref ret);
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
                    Buffer ret = Extern.vcryptpp_vfont_decrypt(bufferPtr, (ulong) data.LongLength);
                    return BufferUtils.ConvertToArrayAndDelete(ref ret);
                }
            }
        }
    }
}
