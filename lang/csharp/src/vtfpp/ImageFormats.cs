// ReSharper disable InconsistentNaming

using System;

namespace sourcepp.vtfpp;

public enum ImageFormat {
	#region Universal Formats
	RGBA8888 = 0,
	ABGR8888,
	RGB888,
	BGR888,
	RGB565,
	I8,
	IA88,
	P8,
	A8,
	RGB888_BLUESCREEN,
	BGR888_BLUESCREEN,
	ARGB8888,
	BGRA8888,
	DXT1,
	DXT3,
	DXT5,
	BGRX8888,
	BGR565,
	BGRX5551,
	BGRA4444,
	DXT1_ONE_BIT_ALPHA,
	BGRA5551,
	UV88,
	UVWQ8888,
	RGBA16161616F,
	RGBA16161616,
	UVLX8888,
	R32F,
	RGB323232F,
	RGBA32323232F,
	#endregion

	#region Alien Swarm & Beyond Formats
	RG1616F,
	RG3232F,
	RGBX8888,
	EMPTY,
	ATI2N,
	ATI1N,
	RGBA1010102,
	BGRA1010102,
	R16F,
	#endregion

	#region Console Formats
	CONSOLE_BGRX8888_LINEAR = 42,
	CONSOLE_RGBA8888_LINEAR,
	CONSOLE_ABGR8888_LINEAR,
	CONSOLE_ARGB8888_LINEAR,
	CONSOLE_BGRA8888_LINEAR,
	CONSOLE_RGB888_LINEAR,
	CONSOLE_BGR888_LINEAR,
	CONSOLE_BGRX5551_LINEAR,
	CONSOLE_I8_LINEAR,
	CONSOLE_RGBA16161616_LINEAR,
	CONSOLE_BGRX8888_LE,
	CONSOLE_BGRA8888_LE,
	#endregion

	#region Titanfall 2 Formats
	TFALL2_BC6H = 66,
	TFALL2_BC7 = 67,
	#endregion

	#region Strata Source Formats
	STRATA_R8 = 69,
	STRATA_BC7,
	STRATA_BC6H,
	#endregion
}

public static partial class ImageFormatDetails
{
	public static sbyte Red(ImageFormat format)
	{
		return DLL.vtfpp_image_format_details_red(format);
	}

	public static sbyte DecompressedRed(ImageFormat format)
	{
		return DLL.vtfpp_image_format_details_decompressed_red(format);
	}

	public static sbyte Green(ImageFormat format)
	{
		return DLL.vtfpp_image_format_details_green(format);
	}

	public static sbyte DecompressedGreen(ImageFormat format)
	{
		return DLL.vtfpp_image_format_details_decompressed_green(format);
	}

	public static sbyte Blue(ImageFormat format)
	{
		return DLL.vtfpp_image_format_details_blue(format);
	}

	public static sbyte DecompressedBlue(ImageFormat format)
	{
		return DLL.vtfpp_image_format_details_decompressed_blue(format);
	}

	public static sbyte Alpha(ImageFormat format)
	{
		return DLL.vtfpp_image_format_details_alpha(format);
	}

	public static sbyte DecompressedAlpha(ImageFormat format)
	{
		return DLL.vtfpp_image_format_details_decompressed_alpha(format);
	}

	public static byte Bpp(ImageFormat format)
	{
		return DLL.vtfpp_image_format_details_bpp(format);
	}

	public static ImageFormat ContainerFormat(ImageFormat format)
	{
		return DLL.vtfpp_image_format_details_container_format(format);
	}

	public static bool Large(ImageFormat format)
	{
		return Convert.ToBoolean(DLL.vtfpp_image_format_details_large(format));
	}

	public static bool Decimal(ImageFormat format)
	{
		return Convert.ToBoolean(DLL.vtfpp_image_format_details_decimal(format));
	}

	public static bool Compressed(ImageFormat format)
	{
		return Convert.ToBoolean(DLL.vtfpp_image_format_details_compressed(format));
	}

	public static bool Transparent(ImageFormat format)
	{
		return Convert.ToBoolean(DLL.vtfpp_image_format_details_transparent(format));
	}

	public static bool Opaque(ImageFormat format)
	{
		return Convert.ToBoolean(DLL.vtfpp_image_format_details_opaque(format));
	}

	public static bool Console(ImageFormat format)
	{
		return Convert.ToBoolean(DLL.vtfpp_image_format_details_console(format));
	}
}

public static class ImageDimensions
{
	public static ushort GetMipDim(byte mip, ushort dim, bool addCompressedFormatPadding = false)
	{
		return DLL.vtfpp_image_dimensions_get_mip_dim(mip, dim, Convert.ToInt32(addCompressedFormatPadding));
	}

	public static byte GetMaximumMipCount(ushort width, ushort height, ushort depth = 1)
	{
		return DLL.vtfpp_image_dimensions_get_maximum_mip_count(width, height, depth);
	}
}

public static partial class ImageFormatDetails
{
	public static uint GetDataLength(ImageFormat format, ushort width, ushort height, ushort depth = 1)
	{
		return DLL.vtfpp_image_format_details_get_data_length(format, width, height, depth);
	}

	public static uint GetDataLength(ImageFormat format, byte mipCount, ushort frameCount, byte faceCount, ushort width, ushort height, ushort depth = 1)
	{
		return DLL.vtfpp_image_format_details_get_data_length_ex(format, mipCount, frameCount, faceCount, width, height, depth);
	}

	public static uint GetDataLengthXbox(bool padded, ImageFormat format, byte mipCount, ushort frameCount, byte faceCount, ushort width, ushort height, ushort depth = 1)
	{
		return DLL.vtfpp_image_format_details_get_data_length_xbox(Convert.ToInt32(padded), format, mipCount, frameCount, faceCount, width, height, depth);
	}

	public static int GetDataPosition(ref uint offset, ref uint length, ImageFormat format, byte mip, byte mipCount, ushort frame, ushort frameCount, byte face, byte faceCount, ushort width, ushort height, ushort slice = 0, ushort depth = 1)
	{
		return DLL.vtfpp_image_format_details_get_data_position(ref offset, ref length, format, mip, mipCount, frame, frameCount, face, faceCount, width, height, slice, depth);
	}

	public static int GetDataPositionXbox(ref uint offset, ref uint length, int padded, ImageFormat format, byte mip, byte mipCount, ushort frame, ushort frameCount, byte face, byte faceCount, ushort width, ushort height, ushort slice = 0, ushort depth = 1)
	{
		return DLL.vtfpp_image_format_details_get_data_position_xbox(ref offset, ref length, padded, format, mip, mipCount, frame, frameCount, face, faceCount, width, height, slice, depth);
	}
}
