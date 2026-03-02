using System;
using System.Collections.Generic;
using System.Linq;

namespace sourcepp.vcryptpp;

public static class VICE
{
	public static class KnownCodes {
		public const string DEFAULT = "x9Ke0BY7";
		public const string BLOODY_GOOD_TIME = "K4PeJwL7";
		public const string CONTAGION_WEAPONS = "fUk0fF69";
		public const string CONTAGION_SCRIPTS = "5!rrFz6p";
		public const string COUNTER_STRIKE_SOURCE = "d7NSuLq2";
		public const string COUNTER_STRIKE_GLOBAL_OFFENSIVE = COUNTER_STRIKE_SOURCE;
		public const string COUNTER_STRIKE_2 = COUNTER_STRIKE_GLOBAL_OFFENSIVE;
		public const string COUNTER_STRIKE_PROMOD = "H1aRQ0n1";
		public const string DAY_OF_DEFEAT_SOURCE = "Wl0u5B3F";
		public const string DYSTOPIA_1_2 = "pH3apO8w";
		public const string DYSTOPIA_1_3 = "G8stUh3F";
		public const string FORTRESS_FOREVER_PRE_GREENLIGHT = "saxEWr5v";
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
		public const string FAIRY_TALE_BUSTERS = TEAM_FORTRESS_2;
		public const string THE_SHIP = "eb3A4m79";
		public const string VALVE_TRACKER = "\rU\xf3\xd3\xad\x6\x57\x47";
		public const string ZOMBIE_PANIC_SOURCE = "5R0ni0pZ";

		public const string GPU_DEFAULT = "X8bU2qll";
		public const string GPU_ALIEN_SWARM = "sW9.JupP";
		public const string GPU_DOTA_2 = "dAIt1IL!";
		public const string GPU_HALF_LIFE_2_EPISODE_2 = "Xx81uBl)";
		public const string GPU_LEFT_4_DEAD_1 = "zp14Hi(]";
		public const string GPU_LEFT_4_DEAD_2 = GPU_LEFT_4_DEAD_1;
		public const string GPU_PORTAL_2 = "UrE66!Ap";
		public const string GPU_TEAM_FORTRESS_2 = TEAM_FORTRESS_2;
	}

	public static byte[] Decrypt(byte[] buffer, string code = KnownCodes.DEFAULT)
	{
		return new sourcepp.Buffer(DLL.vcryptpp_vice_decrypt(buffer, (ulong) buffer.LongLength, code)).Read();
	}

	public static byte[] Decrypt(IEnumerable<byte> buffer, string code = KnownCodes.DEFAULT)
	{
		return Decrypt(buffer.ToArray(), code);
	}

	public static byte[] Encrypt(byte[] buffer, string code)
	{
		return new sourcepp.Buffer(DLL.vcryptpp_vice_encrypt(buffer, (ulong) buffer.LongLength, code)).Read();
	}

	public static byte[] Encrypt(IEnumerable<byte> buffer, string code)
	{
		return Encrypt(buffer.ToArray(), code);
	}
}
