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

        public const string CONTAGION_WEAPONS = "fUk0fF69";
        public const string CONTAGION_SCRIPTS = "5!rrFz6p";
        public const string COUNTER_STRIKE_SOURCE = "d7NSuLq2";
        public const string COUNTER_STRIKE_GLOBAL_OFFENSIVE = COUNTER_STRIKE_SOURCE;
        public const string COUNTER_STRIKE_2 = COUNTER_STRIKE_GLOBAL_OFFENSIVE;
        public const string COUNTER_STRIKE_PROMOD = "H1aRQ0n1";
        public const string DAY_OF_DEFEAT_SOURCE = "Wl0u5B3F";
        public const string DYSTOPIA_1_2 = "pH3apO8w";
        public const string DYSTOPIA_1_3 = "G8stUh3F";
        public const string GOLDEN_EYE_SOURCE = "Gr3naDes";
        public const string HALF_LIFE_2_CTF = "R1dj3axP";
        public const string HALF_LIFE_2_DM = DEFAULT;
        public const string INSURGENCY = "DrA5e3EB";
        public const string LEFT_4_DEAD_2 = "SDhfi878";
        public const string NO_MORE_ROOM_IN_HELL = "lREeeapA";
        public const string NUCLEAR_DAWN = "TA+*veh9";
        public const string TACTICAL_INTERVENTION = "71B4Dt1Z";
        public const string TEAM_FORTRESS_2 = "E2NcUkG2";
        public const string TEAM_FORTRESS_2_ITEMS = "A5fSXbf7";
        public const string THE_SHIP = "eb3A4m79";
        public const string ZOMBIE_PANIC_SOURCE = "5R0ni0pZ";
    }

    public static class VICE
    {
        public static byte[] Decrypt(byte[] buffer, string code = KnownCodes.DEFAULT)
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

        public static byte[] Decrypt(IEnumerable<byte> buffer, string code = KnownCodes.DEFAULT)
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
