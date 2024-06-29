using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;

namespace vicepp
{
    internal static unsafe partial class Extern
    {
        [DllImport("viceppc")]
        public static extern Buffer vicepp_decrypt(byte* buffer, ulong bufferLen, [MarshalAs(UnmanagedType.LPStr)] string code);

        [DllImport("viceppc")]
        public static extern Buffer vicepp_encrypt(byte* buffer, ulong bufferLen, [MarshalAs(UnmanagedType.LPStr)] string code);
    }

    public static class KnownCodes {
        public const string DEFAULT = "x9Ke0BY7";
        public const string HL2DM = DEFAULT;
        public const string CSS = "d7NSuLq2";
        public const string CSGO = CSS;
        public const string DODS = "Wl0u5B3F";
        public const string TF2 = "E2NcUkG2";
        public const string TF2_ITEMS = "A5fSXbf7";
        public const string L4D2 = "SDhfi878";
        public const string THE_SHIP = "eb3A4m79";
        public const string CONTAGION_WEAPONS = "fUk0fF69";
        public const string CONTAGION_SCRIPTS = "5!rrFz6p";
        public const string ZPS = "5R0ni0pZ";
    }

    public static class VICE
    {
        public static byte[] Decrypt(byte[] buffer, string code)
        {
            unsafe
            {
                fixed (byte* bufferPtr = buffer)
                {
                    Buffer ret = Extern.vicepp_decrypt(bufferPtr, (ulong) buffer.LongLength, code);
                    return BufferUtils.ConvertToArrayAndDelete(ref ret);
                }
            }
        }

        public static byte[] Decrypt(IEnumerable<byte> buffer, string code)
        {
            unsafe
            {
                var data = buffer.ToArray();
                fixed (byte* bufferPtr = data)
                {
                    Buffer ret = Extern.vicepp_decrypt(bufferPtr, (ulong) data.LongLength, code);
                    return BufferUtils.ConvertToArrayAndDelete(ref ret);
                }
            }
        }

        public static byte[] Encrypt(byte[] buffer, string code)
        {
            unsafe
            {
                fixed (byte* bufferPtr = buffer)
                {
                    Buffer ret = Extern.vicepp_encrypt(bufferPtr, (ulong) buffer.LongLength, code);
                    return BufferUtils.ConvertToArrayAndDelete(ref ret);
                }
            }
        }

        public static byte[] Encrypt(IEnumerable<byte> buffer, string code)
        {
            unsafe
            {
                var data = buffer.ToArray();
                fixed (byte* bufferPtr = data)
                {
                    Buffer ret = Extern.vicepp_encrypt(bufferPtr, (ulong) data.LongLength, code);
                    return BufferUtils.ConvertToArrayAndDelete(ref ret);
                }
            }
        }
    }
}
