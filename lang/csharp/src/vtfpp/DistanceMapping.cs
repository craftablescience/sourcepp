using System;

namespace sourcepp.vtfpp;

public static class DistanceMapping
{
	public enum Dither
	{
		NONE = 0,
		GRADIENT_TANGENT,
	}

	public enum Flags : uint
	{
		NONE           = 0,
		DISTANCEAA     = 1 << 0,
		EUCLIDEAN      = 1 << 1,
		SAMPLECENTERED = 1 << 2,
	}

	public static byte[] AlphaToDistance(ReadOnlySpan<byte> buffer, ImageFormat inFormat, ImageFormat outFormat, ushort width, ushort height, ushort resizeX, ushort resizeY, bool srgb, ref bool valveQuirks, float distanceSpread = 1.0f, float alphaThreshold = 0.04f, Flags flags = Flags.NONE, Dither dither = Dither.NONE, ImageConversion.ResizeFilter filter = ImageConversion.ResizeFilter.NICE, ImageConversion.ResizeEdge edge = ImageConversion.ResizeEdge.CLAMP)
	{
		var valveQuirksInt = Convert.ToInt32(valveQuirks);
		unsafe
		{
			var valveQuirksIntPtr = &valveQuirksInt;
			var output = new sourcepp.Buffer(DLL.vtfpp_distance_mapping_alpha_to_distance_ex(buffer, (ulong)buffer.Length, inFormat, outFormat, width, height, resizeX, resizeY, Convert.ToInt32(srgb), distanceSpread, alphaThreshold, flags, dither, filter, edge, valveQuirks ? (nint)valveQuirksIntPtr : 0)).Read<byte>();
			valveQuirks = Convert.ToBoolean(valveQuirksInt);
			return output;
		}
	}
}
