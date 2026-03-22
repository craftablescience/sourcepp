using System;
using System.Runtime.InteropServices;

namespace sourcepp.vtfpp;

public static class ImageConversion
{
	public const float DEFAULT_COMPRESSED_QUALITY = -1;

	public static byte[] ConvertImageDataToFormat(ReadOnlySpan<byte> buffer, ImageFormat oldFormat, ImageFormat newFormat, ushort width, ushort height, float quality = DEFAULT_COMPRESSED_QUALITY)
	{
		return new sourcepp.Buffer(DLL.vtfpp_image_conversion_convert_image_data_to_format(buffer, (ulong) buffer.Length, oldFormat, newFormat, width, height, quality)).Read<byte>();
	}
	
	public static byte[] ConvertSeveralImageDataToFormat(ReadOnlySpan<byte> buffer, ImageFormat oldFormat, ImageFormat newFormat, byte mipCount, ushort frameCount, byte faceCount, ushort width, ushort height, ushort depth, float quality = DEFAULT_COMPRESSED_QUALITY)
	{
		return new sourcepp.Buffer(DLL.vtfpp_image_conversion_convert_several_image_data_to_format(buffer, (ulong) buffer.Length, oldFormat, newFormat, mipCount, frameCount, faceCount, width, height, depth, quality)).Read<byte>();
	}

	public static byte[] ConvertHdriToCubemap(ReadOnlySpan<byte> buffer, ImageFormat format, ushort width, ushort height, ushort resolution = 0, bool bilinear = true)
	{
		return new sourcepp.Buffer(DLL.vtfpp_image_conversion_convert_hdri_to_cubemap_ex(buffer, (ulong) buffer.Length, format, width, height, resolution, Convert.ToInt32(bilinear))).Read<byte>();
	}
	
	public enum FileFormat {
		DEFAULT = 0,
		PNG     = 1,
		JPG     = 2,
		BMP     = 3,
		TGA     = 4,
		WEBP    = 5,
		QOI     = 6,
		HDR     = 7,
		EXR     = 8,
	}

	public static FileFormat GetDefaultFileFormatForImageFormat(ImageFormat format)
	{
		return DLL.vtfpp_image_conversion_get_default_file_format_for_image_format(format);
	}

	public static byte[] ConvertImageDataToFile(ReadOnlySpan<byte> buffer, ImageFormat format, ushort width, ushort height, FileFormat fileFormat = FileFormat.DEFAULT)
	{
		return new sourcepp.Buffer(DLL.vtfpp_image_conversion_convert_image_data_to_file(buffer, (ulong) buffer.Length, format, width, height, fileFormat)).Read<byte>();
	}

	public static byte[] ConvertImageDataToFile(ReadOnlySpan<byte> buffer, ref ImageFormat format, ref int width, ref int height, ref int frameCount)
	{
		return new sourcepp.Buffer(DLL.vtfpp_image_conversion_convert_file_to_image_data(buffer, (ulong) buffer.Length, ref format, ref width, ref height, ref frameCount)).Read<byte>();
	}

	public enum ResizeEdge {
		CLAMP = 0,
		REFLECT,
		WRAP,
		ZERO,
	}

	public enum ResizeFilter {
		DEFAULT = 0,
		BOX,
		BILINEAR,
		CUBIC_BSPLINE,
		CATMULL_ROM,
		MITCHELL,
		POINT_SAMPLE,
		KAISER = 100,
		NICE = 101,
	}

	public enum ResizeMethod {
		NONE,
		POWER_OF_TWO_BIGGER,
		POWER_OF_TWO_SMALLER,
		POWER_OF_TWO_NEAREST,
	}

	[StructLayout(LayoutKind.Sequential)]
	public struct ResizeBounds
	{
		public ushort ResizeMinWidth = 0;
		public ushort ResizeMaxWidth = 0;
		public ushort ResizeMinHeight = 0;
		public ushort ResizeMaxHeight = 0;

		public ResizeBounds()
		{
		}

		public ResizeBounds(ushort size)
		{
			ResizeMinWidth = size;
			ResizeMaxWidth = size;
			ResizeMinHeight = size;
			ResizeMaxHeight = size;
		}

		public ResizeBounds(ushort width, ushort height)
		{
			ResizeMinWidth = width;
			ResizeMaxWidth = width;
			ResizeMinHeight = height;
			ResizeMaxHeight = height;
		}

		public ResizeBounds(ushort minWidth, ushort maxWidth, ushort minHeight, ushort maxHeight)
		{
			ResizeMinWidth = minWidth;
			ResizeMaxWidth = maxWidth;
			ResizeMinHeight = minHeight;
			ResizeMaxHeight = maxHeight;
		}
	}

	public static ushort GetResizedDim(ushort n, ResizeMethod method)
	{
		return DLL.vtfpp_image_conversion_get_resized_dim(n, method);
	}

	public static void SetResizedDims(ref ushort width, ResizeMethod widthResize, ref ushort height, ResizeMethod heightResize)
	{
		DLL.vtfpp_image_conversion_set_resized_dims(ref width, widthResize, ref height, heightResize);
	}

	public static byte[] ResizeImageData(ReadOnlySpan<byte> buffer, ImageFormat format, ushort width, ushort newWidth, ushort height, ushort newHeight, bool srgb, ResizeFilter filter, ResizeEdge edge = ResizeEdge.CLAMP)
	{
		return new sourcepp.Buffer(DLL.vtfpp_image_conversion_resize_image_data(buffer, (ulong) buffer.Length, format, width, newWidth, height, newHeight, Convert.ToInt32(srgb), filter, edge)).Read<byte>();
	}
	
	public static byte[] ResizeImageDataStrict(ReadOnlySpan<byte> buffer, ImageFormat format, ushort width, ushort newWidth, ref ushort widthOut, ResizeMethod widthResize, ushort height, ushort newHeight, ref ushort heightOut, ResizeMethod heightResize, bool srgb, ResizeFilter filter, ResizeEdge edge = ResizeEdge.CLAMP)
	{
		return new sourcepp.Buffer(DLL.vtfpp_image_conversion_resize_image_data_strict(buffer, (ulong) buffer.Length, format, width, newWidth, ref widthOut, widthResize, height, newHeight, ref heightOut, heightResize, Convert.ToInt32(srgb), filter, edge)).Read<byte>();
	}

	public static byte[] CropImageData(ReadOnlySpan<byte> buffer, ImageFormat format, ushort width, ushort newWidth, ushort xOffset, ushort height, ushort newHeight, ushort yOffset)
	{
		return new sourcepp.Buffer(DLL.vtfpp_image_conversion_crop_image_data(buffer, (ulong) buffer.Length, format, width, newWidth, xOffset, height, newHeight, yOffset)).Read<byte>();
	}

	public static void GammaCorrectImageData(Span<byte> buffer, ImageFormat format, ushort width, ushort height, float gamma)
	{
		DLL.vtfpp_image_conversion_gamma_correct_image_data(buffer, (ulong) buffer.Length, format, width, height, gamma);
	}

	public static void InvertGreenChannelForImageData(Span<byte> buffer, ImageFormat format, ushort width, ushort height)
	{
		DLL.vtfpp_image_conversion_invert_green_channel_for_image_data(buffer, (ulong) buffer.Length, format, width, height);
	}
}
