using System;
using System.Runtime.InteropServices;

namespace vtfpp
{
	public enum CompressionMethod
	{
		DEFLATE = 8,
		ZSTD = 93,
		CONSOLE_LZMA = 0x360,
	}

	public enum ResourceType
	{
		UNKNOWN,
		THUMBNAIL_DATA,
		IMAGE_DATA,
		PARTICLE_SHEET_DATA,
		CRC,
		LOD_CONTROL_INFO,
		EXTENDED_FLAGS,
		KEYVALUES_DATA,
		AUX_COMPRESSION,
	}

	[Flags]
	public enum ResourceFlags
	{
		NONE = 0,
		LOCAL_DATA = 1 << 1,
	}

	[Flags]
	public enum VTFFlags
	{
		NONE = 0,
		POINT_SAMPLE = 1 << 0,
		TRILINEAR = 1 << 1,
		CLAMP_S = 1 << 2,
		CLAMP_T = 1 << 3,
		ANISOTROPIC = 1 << 4,
		HINT_DXT5 = 1 << 5,
		PWL_CORRECTED = 1 << 6,
		NORMAL = 1 << 7,
		NO_MIP = 1 << 8,
		NO_LOD = 1 << 9,
		LOAD_ALL_MIPS = 1 << 10,
		PROCEDURAL = 1 << 11,
		ONE_BIT_ALPHA = 1 << 12,
		MULTI_BIT_ALPHA = 1 << 13,
		ENVMAP = 1 << 14,
		RENDERTARGET = 1 << 15,
		DEPTH_RENDERTARGET = 1 << 16,
		NO_DEBUG_OVERRIDE = 1 << 17,
		SINGLE_COPY = 1 << 18,
		SRGB = 1 << 19,
		DEFAULT_POOL = 1 << 20,
		COMBINED = 1 << 21,
		ASYNC_DOWNLOAD = 1 << 22,
		NO_DEPTH_BUFFER = 1 << 23,
		SKIP_INITIAL_DOWNLOAD = 1 << 24,
		CLAMP_U = 1 << 25,
		VERTEX_TEXTURE = 1 << 26,
		XBOX_PRESWIZZLED = 1 << 26,
		SSBUMP = 1 << 27,
		XBOX_CACHEABLE = 1 << 27,
		LOAD_MOST_MIPS = 1 << 28,
		BORDER = 1 << 29,
		YCOCG = 1 << 30,
		ASYNC_SKIP_INITIAL_LOW_RES = 1 << 31,
	}

	public enum VTFPlatform
	{
		UNKNOWN = 0x000,
		PC = 0x001,
		PS3_PORTAL2 = 0x003,
		PS3_ORANGEBOX = 0x333,
		X360 = 0x360,
	}

	internal static unsafe partial class Extern
	{
		[LibraryImport("sourcepp_vtfppc", EntryPoint = "vtfpp_vtf_open_from_mem")]
		public static partial void* VTFOpenFromMemory(byte* buffer, ulong bufferLen, int parseHeaderOnly);

		[LibraryImport("sourcepp_vtfppc", EntryPoint = "vtfpp_vtf_open_from_file")]
		public static partial void* VTFOpenFromFile([MarshalAs(UnmanagedType.LPStr)] string vtfPath);

		[LibraryImport("sourcepp_vtfppc", EntryPoint = "vtfpp_vtf_get_image_data_as_rgba8888")]
		public static partial sourcepp.Buffer VTFGetImageDataAsRGBA8888(void* handle, byte mip, ushort frame, byte face, ushort slice);

		[LibraryImport("sourcepp_vtfppc", EntryPoint = "vtfpp_vtf_get_width")]
		public static partial ushort VTFGetWidth(void* handle, byte mip);

		[LibraryImport("sourcepp_vtfppc", EntryPoint = "vtfpp_vtf_get_height")]
		public static partial ushort VTFGetHeight(void* handle, byte mip);

		[LibraryImport("sourcepp_vtfppc", EntryPoint = "vtfpp_vtf_close")]
		public static partial void VTFClose(void** handle);

	}

	public class VTF
	{
		private protected unsafe VTF(void* handle)
		{
			Handle = handle;
		}

		~VTF()
		{
			unsafe
			{
				fixed (void** handlePtr = &Handle)
				{
					Extern.VTFClose(handlePtr);
				}
			}
		}

		public static VTF? OpenFromMemory(byte[] buffer, int parseHeaderOnly)
		{
			unsafe
			{
				fixed (byte* bufferPtr = buffer)
				{
					var handle = Extern.VTFOpenFromMemory(bufferPtr, (ulong)buffer.LongLength, parseHeaderOnly);
					return handle == null ? null : new VTF(handle);
				}
			}
		}

		public static VTF? OpenFromFile(string path)
		{
			unsafe
			{
				var handle = Extern.VTFOpenFromFile(path);
				return handle == null ? null : new VTF(handle);
			}
		}

		public byte[]? GetImageDataAsRGBA8888(byte mip, ushort frame, byte face, ushort slice)
		{
			unsafe
			{
				var buffer = Extern.VTFGetImageDataAsRGBA8888(Handle, mip, frame, face, slice);
				return buffer.size < 0 ? null : sourcepp.BufferUtils.ConvertToArrayAndDelete(ref buffer);
			}
		}

		public ushort GetWidth(byte mip = 0)
		{
			unsafe
			{
				return Extern.VTFGetWidth(Handle, mip);
			}
		}

		public ushort GetHeight(byte mip = 0)
		{
			unsafe
			{
				return Extern.VTFGetHeight(Handle, mip);
			}
		}

		private protected readonly unsafe void* Handle;
	}
}
