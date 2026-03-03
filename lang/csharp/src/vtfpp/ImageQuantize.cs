using System;

namespace sourcepp.vtfpp;

public static class ImageQuantize
{
	// ReSharper disable once InconsistentNaming
	public static byte[] ConvertP8ImageDataToBGRA8888(ReadOnlySpan<byte> paletteData, ReadOnlySpan<byte> imageData)
	{
		return new sourcepp.Buffer(DLL.vtfpp_image_quantize_convert_p8_image_data_to_bgra8888(paletteData, (ulong) paletteData.Length, imageData, (ulong) imageData.Length)).Read<byte>();
	}
}
