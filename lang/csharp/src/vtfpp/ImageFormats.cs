using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace vtfpp
{
	public enum ImageFormat
	{
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
		RG1616F,
		RG3232F,
		RGBX8888,
		EMPTY,
		ATI2N,
		ATI1N,
		RGBA1010102,
		BGRA1010102,
		R16F,

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

		R8 = 69,
		BC7,
		BC6H,
	}


	internal static unsafe partial class Extern
	{
		[LibraryImport("sourcepp_vtfppc", EntryPoint = "vtfpp_image_format_details_red")]
		public static partial sbyte ImageFormatDetailsRed(ImageFormat format);

		[LibraryImport("sourcepp_vtfppc", EntryPoint = "vtfpp_image_format_details_decompressed_red")]
		public static partial sbyte ImageFormatDetailsDecompressedRed(ImageFormat format);

		[LibraryImport("sourcepp_vtfppc", EntryPoint = "vtfpp_image_format_details_green")]
		public static partial sbyte ImageFormatDetailsGreen(ImageFormat format);

		[LibraryImport("sourcepp_vtfppc", EntryPoint = "vtfpp_image_format_details_decompressed_green")]
		public static partial sbyte ImageFormatDetailsDecompressedGreen(ImageFormat format);

		[LibraryImport("sourcepp_vtfppc", EntryPoint = "vtfpp_image_format_details_blue")]
		public static partial sbyte ImageFormatDetailsBlue(ImageFormat format);

		[LibraryImport("sourcepp_vtfppc", EntryPoint = "vtfpp_image_format_details_decompressed_blue")]
		public static partial sbyte ImageFormatDetailsDecompressedBlue(ImageFormat format);

		[LibraryImport("sourcepp_vtfppc", EntryPoint = "vtfpp_image_format_details_alpha")]
		public static partial sbyte ImageFormatDetailsAlpha(ImageFormat format);

		[LibraryImport("sourcepp_vtfppc", EntryPoint = "vtfpp_image_format_details_decompressed_alpha")]
		public static partial sbyte ImageFormatDetailsDecompressedAlpha(ImageFormat format);

		[LibraryImport("sourcepp_vtfppc", EntryPoint = "vtfpp_image_format_details_bpp")]
		public static partial byte ImageFormatDetailsBPP(ImageFormat format);

		[LibraryImport("sourcepp_vtfppc", EntryPoint = "vtfpp_image_format_details_container_format")]
		public static partial ImageFormat ImageFormatDetailsContainerFormat(ImageFormat format);

		[LibraryImport("sourcepp_vtfppc", EntryPoint = "vtfpp_image_format_details_large")]
		public static partial int ImageFormatDetailsLarge(ImageFormat format);

		[LibraryImport("sourcepp_vtfppc", EntryPoint = "vtfpp_image_format_details_decimal")]
		public static partial int ImageFormatDetailsDecimal(ImageFormat format);

		[LibraryImport("sourcepp_vtfppc", EntryPoint = "vtfpp_image_format_details_compressed")]
		public static partial int ImageFormatDetailsCompressed(ImageFormat format);

		[LibraryImport("sourcepp_vtfppc", EntryPoint = "vtfpp_image_format_details_transparent")]
		public static partial int ImageFormatDetailsTransparent(ImageFormat format);

		[LibraryImport("sourcepp_vtfppc", EntryPoint = "vtfpp_image_format_details_opaque")]
		public static partial int ImageFormatDetailsOpaque(ImageFormat format);

		[LibraryImport("sourcepp_vtfppc", EntryPoint = "vtfpp_image_dimensions_get_mip_dim")]
		public static partial uint ImageDimensionsGetMipDim(byte mip, ushort dim);

		[LibraryImport("sourcepp_vtfppc", EntryPoint = "vtfpp_image_dimensions_get_recommended_mip_count_for_dim")]
		public static partial byte ImageDimensionsGetRecommendedMipCountForDim(ImageFormat format, ushort width, ushort height);

		[LibraryImport("sourcepp_vtfppc", EntryPoint = "vtfpp_image_dimensions_get_actual_mip_count_for_dims_on_console")]
		public static partial byte ImageDimensionsGetActualMipCountForDimsOnConsole(ushort width, ushort height);

		[LibraryImport("sourcepp_vtfppc", EntryPoint = "vtfpp_image_format_details_get_data_length")]
		public static partial uint ImageFormatDetailsGetDataLength(ImageFormat format, ushort width, ushort height, ushort sliceCount);

		[LibraryImport("sourcepp_vtfppc", EntryPoint = "vtfpp_image_format_details_get_data_length_ex")]
		public static partial uint ImageFormatDetailsGetDataLengthEx(ImageFormat format, byte mipCount, ushort frameCount, byte faceCount, ushort width, ushort height, ushort sliceCount);

		[LibraryImport("sourcepp_vtfppc", EntryPoint = "vtfpp_image_format_details_get_data_position")]
		public static partial int ImageFormatGetDataPosition(uint* offset, uint* length, ImageFormat format, byte mip, byte mipCount, ushort frame, ushort frameCount, byte face, byte faceCount, ushort width, ushort height, ushort slice, ushort sliceCount);
	}

	public class ImageFormatDetails
	{
		public static sbyte Red(ImageFormat format)
		{
			unsafe
			{
				return Extern.ImageFormatDetailsRed(format);
			}
		}

		public static sbyte DecompressedRed(ImageFormat format)
		{
			unsafe
			{
				return Extern.ImageFormatDetailsDecompressedRed(format);
			}
		}

		public static sbyte Green(ImageFormat format)
		{
			unsafe
			{
				return Extern.ImageFormatDetailsGreen(format);
			}
		}

		public static sbyte DecompressedGreen(ImageFormat format)
		{
			unsafe
			{
				return Extern.ImageFormatDetailsDecompressedGreen(format);
			}
		}

		public static sbyte Blue(ImageFormat format)
		{
			unsafe
			{
				return Extern.ImageFormatDetailsBlue(format);
			}
		}

		public static sbyte DecompressedBlue(ImageFormat format)
		{
			unsafe
			{
				return Extern.ImageFormatDetailsDecompressedBlue(format);
			}
		}

		public static sbyte Alpha(ImageFormat format)
		{
			unsafe
			{
				return Extern.ImageFormatDetailsAlpha(format);
			}
		}

		public static sbyte DecompressedAlpha(ImageFormat format)
		{
			unsafe
			{
				return Extern.ImageFormatDetailsDecompressedAlpha(format);
			}
		}

		public static byte BPP(ImageFormat format)
		{
			unsafe
			{
				return Extern.ImageFormatDetailsBPP(format);
			}
		}

		public static ImageFormat ContainerFormat(ImageFormat format)
		{
			unsafe 
			{
				return Extern.ImageFormatDetailsContainerFormat(format);
			}
		}

		public static bool Large(ImageFormat format)
		{
			unsafe
			{
				return Convert.ToBoolean(Extern.ImageFormatDetailsLarge(format));
			}
		}

		public static bool Decimal(ImageFormat format)
		{
			unsafe
			{
				return Convert.ToBoolean(Extern.ImageFormatDetailsDecimal(format));
			}
		}

		public static bool Compressed(ImageFormat format)
		{
			unsafe
			{
				return Convert.ToBoolean(Extern.ImageFormatDetailsCompressed(format));
			}
		}

		public static bool Transparent(ImageFormat format)
		{
			unsafe
			{
				return Convert.ToBoolean(Extern.ImageFormatDetailsTransparent(format));
			}
		}

		public static bool Opaque(ImageFormat format)
		{
			unsafe
			{
				return Convert.ToBoolean(Extern.ImageFormatDetailsOpaque(format));
			}
		}

		public static uint GetDataLength(ImageFormat format, ushort width, ushort height, ushort sliceCount)
		{
			unsafe
			{
				return Extern.ImageFormatDetailsGetDataLength(format, width, height, sliceCount);
			}
		}

		public static uint GetDataLength(ImageFormat format, byte mipCount, ushort frameCount, byte faceCount, ushort width, ushort height, ushort sliceCount)
		{
			unsafe
			{
				return Extern.ImageFormatDetailsGetDataLengthEx(format, mipCount, frameCount, faceCount, width, height, sliceCount);
			}
		}

		public static bool GetDataPosition(ref uint offset, ref uint length, ImageFormat format, byte mip, byte mipCount, ushort frame, byte face, byte faceCount, ushort frameCount, ushort width, ushort height, ushort slice = 0, ushort sliceCount = 1)
		{
			unsafe
			{
				/// @todo Is there a better way for this?
				fixed (uint* offsetPtr = &offset)
				{
					fixed (uint* lengthPtr = &length)
					{
						return Convert.ToBoolean(Extern.ImageFormatGetDataPosition(offsetPtr, lengthPtr, format, mip, mipCount, frame, frameCount, face, faceCount, width, height, slice, sliceCount));
					}
				}
			}
		}
	}

	class ImageDimensions
	{
		public static uint GetMipDim(byte mip, ushort dim)
		{
			unsafe
			{
				return Extern.ImageDimensionsGetMipDim(mip, dim);
			}
		}

		public static byte GetRecommendedMipCountForDim(ImageFormat format, ushort width, ushort height)
		{
			unsafe
			{
				return Extern.ImageDimensionsGetRecommendedMipCountForDim(format, width, height);
			}
		}

		public static byte GetActualMipCountForDimsOnConsole(ushort width, ushort height)
		{
			unsafe
			{
				return Extern.ImageDimensionsGetActualMipCountForDimsOnConsole(width, height);
			}
		}

	}
}
