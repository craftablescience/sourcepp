using System;
using System.Diagnostics.CodeAnalysis;
using System.Runtime.InteropServices;

namespace sourcepp.vtfpp;

public enum CompressionMethod {
	DEFLATE = 8,
	ZSTD = 93,
	CONSOLE_LZMA = 0x360,
}

public sealed class Resource : sourcepp.ManagedNativeHandle
{
	public enum Type
	{
		UNKNOWN             = 0,
		THUMBNAIL_DATA      = 1,
		PALETTE_DATA        = 2,
		FALLBACK_DATA       = 3,
		PARTICLE_SHEET_DATA = 16,
		HOTSPOT_DATA        = 43,
		IMAGE_DATA          = 48,
		EXTENDED_FLAGS      = 3167060,
		CRC                 = 4411971,
		AUX_COMPRESSION     = 4413505,
		LOD_CONTROL_INFO    = 4476748,
		KEYVALUES_DATA      = 4478539,
	}

	[Flags]
	public enum Flags
	{
		NONE       = 0,
		LOCAL_DATA = 1 << 1,
	}

	internal Resource(nint handle) : base(handle, null, false)
	{
	}

	public Type ResourceType
	{
		get
		{
			ThrowIfDisposed();
			return DLL.vtfpp_resource_get_type(Handle);
		}
	}

	public Flags ResourceFlags
	{
		get
		{
			ThrowIfDisposed();
			return DLL.vtfpp_resource_get_flags(Handle);
		}
	}

	public Span<byte> Data
	{
		get
		{
			ThrowIfDisposed();
			unsafe
			{
				ulong dataLen = 0;
				var dataPtr = DLL.vtfpp_resource_get_data(Handle, ref dataLen);
				return new Span<byte>(dataPtr, (int) dataLen);
			}
		}
	}

	public byte[] GetDataAsPalette(ushort frame)
	{
		ThrowIfDisposed();
		return new sourcepp.Buffer(DLL.vtfpp_resource_get_data_as_palette(Handle, frame)).Read<byte>();
	}

	public SHT GetDataAsParticleSheet()
	{
		ThrowIfDisposed();
		return new SHT(DLL.vtfpp_resource_get_data_as_particle_sheet(Handle));
	}

	public uint GetDataAsCrc()
	{
		ThrowIfDisposed();
		return DLL.vtfpp_resource_get_data_as_crc(Handle);
	}

	public uint GetDataAsExtendedFlags()
	{
		ThrowIfDisposed();
		return DLL.vtfpp_resource_get_data_as_extended_flags(Handle);
	}

	public (byte, byte, byte, byte) GetDataAsLodControlInfo()
	{
		ThrowIfDisposed();
		byte u = 0, v = 0, u360 = 0, v360 = 0;
		DLL.vtfpp_resource_get_data_as_lod_control_info(Handle, ref u, ref v, ref u360, ref v360);
		return (u, v, u360, v360);
	}

	public string GetDataAsKeyValuesData()
	{
		ThrowIfDisposed();
		return new sourcepp.String(DLL.vtfpp_resource_get_data_as_keyvalues_data(Handle)).Read();
	}

	public HOT GetDataAsHotspotData()
	{
		ThrowIfDisposed();
		return new HOT(DLL.vtfpp_resource_get_data_as_hotspot_data(Handle));
	}

	public short GetDataAsAuxCompressionLevel()
	{
		ThrowIfDisposed();
		return DLL.vtfpp_resource_get_data_as_aux_compression_level(Handle);
	}
	
	public CompressionMethod GetDataAsAuxCompressionMethod()
	{
		ThrowIfDisposed();
		return DLL.vtfpp_resource_get_data_as_aux_compression_method(Handle);
	}
	
	public uint GetDataAsAuxCompressionLength(byte mip, byte mipCount, ushort frame, ushort frameCount, ushort face, ushort faceCount)
	{
		ThrowIfDisposed();
		return DLL.vtfpp_resource_get_data_as_aux_compression_length(Handle, mip, mipCount, frame, frameCount, face, faceCount);
	}
}

public sealed class VTF : sourcepp.ManagedNativeHandle
{
	[Flags]
	[SuppressMessage("Design", "CA1069:Enums values should not be duplicated")]
	public enum Flags : uint
	{
		V0_POINT_SAMPLE     = 1u <<  0,
		V0_TRILINEAR        = 1u <<  1,
		V0_CLAMP_S          = 1u <<  2,
		V0_CLAMP_T          = 1u <<  3,
		V0_ANISOTROPIC      = 1u <<  4,
		V0_VTEX_HINT_DXT5   = 1u <<  5,
		V0_VTEX_NO_COMPRESS = 1u <<  6,
		V0_NORMAL           = 1u <<  7,
		V0_NO_MIP           = 1u <<  8,
		V0_NO_LOD           = 1u <<  9,
		V0_LOAD_SMALL_MIPS  = 1u << 10,
		V0_PROCEDURAL       = 1u << 11,
		V0_ONE_BIT_ALPHA    = 1u << 12,
		V0_MULTI_BIT_ALPHA  = 1u << 13,
		V0_ENVMAP           = 1u << 14,
		MASK_V0 = V0_POINT_SAMPLE |V0_TRILINEAR | V0_CLAMP_S | V0_CLAMP_T | V0_ANISOTROPIC | V0_VTEX_HINT_DXT5 | V0_VTEX_NO_COMPRESS | V0_NORMAL | V0_NO_MIP | V0_NO_LOD | V0_LOAD_SMALL_MIPS | V0_PROCEDURAL | V0_ONE_BIT_ALPHA | V0_MULTI_BIT_ALPHA | V0_ENVMAP,
		MASK_V0_VTEX = V0_VTEX_HINT_DXT5 | V0_VTEX_NO_COMPRESS,

		V1_RENDERTARGET                                 = 1u << 15,
		V1_DEPTH_RENDERTARGET                           = 1u << 16,
		V1_NO_DEBUG_OVERRIDE                            = 1u << 17,
		V1_SINGLE_COPY                                  = 1u << 18,
		V1_VTEX_ONE_OVER_MIP_LEVEL_IN_ALPHA             = 1u << 19,
		V1_VTEX_PREMULTIPLY_COLOR_BY_ONE_OVER_MIP_LEVEL = 1u << 20,
		V1_VTEX_CONVERT_NORMAL_TO_DUDV                  = 1u << 21,
		MASK_V1 = V1_RENDERTARGET | V1_DEPTH_RENDERTARGET | V1_NO_DEBUG_OVERRIDE | V1_SINGLE_COPY | V1_VTEX_ONE_OVER_MIP_LEVEL_IN_ALPHA | V1_VTEX_PREMULTIPLY_COLOR_BY_ONE_OVER_MIP_LEVEL | V1_VTEX_CONVERT_NORMAL_TO_DUDV,
		MASK_V1_VTEX = V1_VTEX_ONE_OVER_MIP_LEVEL_IN_ALPHA | V1_VTEX_PREMULTIPLY_COLOR_BY_ONE_OVER_MIP_LEVEL | V1_VTEX_CONVERT_NORMAL_TO_DUDV,

		V2_VTEX_ALPHA_TEST_MIP_GENERATION = 1u << 22, // Internal to vtex
		V2_NO_DEPTH_BUFFER                = 1u << 23,
		V2_VTEX_NICE_FILTERED             = 1u << 24, // Internal to vtex
		V2_CLAMP_U                        = 1u << 25,
		MASK_V2 = V2_VTEX_ALPHA_TEST_MIP_GENERATION | V2_NO_DEPTH_BUFFER | V2_VTEX_NICE_FILTERED | V2_CLAMP_U,
		MASK_V2_VTEX = V2_VTEX_ALPHA_TEST_MIP_GENERATION | V2_VTEX_NICE_FILTERED,

		XBOX_VTEX_PRESWIZZLED = 1u << 26,
		XBOX_CACHEABLE        = 1u << 27,
		XBOX_UNFILTERABLE_OK  = 1u << 28,
		MASK_XBOX = XBOX_VTEX_PRESWIZZLED | XBOX_CACHEABLE | XBOX_UNFILTERABLE_OK,
		MASK_XBOX_VTEX = XBOX_VTEX_PRESWIZZLED,

		V3_LOAD_ALL_MIPS  = 1u << 10,
		V3_VERTEX_TEXTURE = 1u << 26,
		V3_SSBUMP         = 1u << 27,
		V3_BORDER         = 1u << 29,
		MASK_V3 = V3_LOAD_ALL_MIPS | V3_VERTEX_TEXTURE | V3_SSBUMP | V3_BORDER,

		V4_SRGB = 1u << 6,
		MASK_V4 = V4_SRGB,

		V4_TF2_STAGING_MEMORY    = 1u << 19,
		V4_TF2_IMMEDIATE_CLEANUP = 1u << 20,
		V4_TF2_IGNORE_PICMIP     = 1u << 21,
		V4_TF2_STREAMABLE_COARSE = 1u << 30,
		V4_TF2_STREAMABLE_FINE   = 1u << 31,
		MASK_V4_TF2 = V4_TF2_STAGING_MEMORY | V4_TF2_IMMEDIATE_CLEANUP | V4_TF2_IGNORE_PICMIP | V4_TF2_STREAMABLE_COARSE | V4_TF2_STREAMABLE_FINE,

		V5_PWL_CORRECTED  = 1u <<  6,
		V5_SRGB           = 1u << 19,
		V5_DEFAULT_POOL   = 1u << 20,
		V5_LOAD_MOST_MIPS = 1u << 28,
		MASK_V5 = V5_PWL_CORRECTED | V5_SRGB | V5_DEFAULT_POOL | V5_LOAD_MOST_MIPS,

		V5_CSGO_COMBINED                   = 1u << 21,
		V5_CSGO_ASYNC_DOWNLOAD             = 1u << 22,
		V5_CSGO_SKIP_INITIAL_DOWNLOAD      = 1u << 24,
		V5_CSGO_YCOCG                      = 1u << 30,
		V5_CSGO_ASYNC_SKIP_INITIAL_LOW_RES = 1u << 31,
		MASK_V5_CSGO = V5_CSGO_COMBINED | V5_CSGO_ASYNC_DOWNLOAD | V5_CSGO_SKIP_INITIAL_DOWNLOAD | V5_CSGO_YCOCG | V5_CSGO_ASYNC_SKIP_INITIAL_LOW_RES,

		V6_IGNORE_PICMIP = 1u << 18,
		MASK_V6 = V6_IGNORE_PICMIP,

		MASK_INTERNAL = V0_NO_MIP | V0_ENVMAP,
	}

	public enum Platform
	{
		UNKNOWN       = 0x000,
		PC            = 0x007,
		XBOX          = 0x005,
		X360          = 0x360,
		PS3_ORANGEBOX = 0x333,
		PS3_PORTAL2   = 0x334,
	}
	
	public static readonly ImageFormat FormatUnchanged = (ImageFormat)(-2);
	public static readonly ImageFormat FormatDefault   = (ImageFormat)(-1);

	[StructLayout(LayoutKind.Sequential, Pack = 1)]
	public struct CreationOptions()
	{
		public uint Version = 4;
		public ImageFormat OutputFormat = FormatDefault;
		public float CompressedFormatQuality = ImageConversion.DEFAULT_COMPRESSED_QUALITY;
		public ImageConversion.ResizeMethod WidthResizeMethod = ImageConversion.ResizeMethod.POWER_OF_TWO_BIGGER;
		public ImageConversion.ResizeMethod HeightResizeMethod = ImageConversion.ResizeMethod.POWER_OF_TWO_BIGGER;
		public ImageConversion.ResizeFilter Filter = ImageConversion.ResizeFilter.DEFAULT;
		public Flags VTFFlags = 0;
		public ushort InitialFrameCount = 1;
		public ushort StartFrame = 0;
		public int IsCubeMap = 0;
		public ushort InitialDepth = 1;
		public int ComputeTransparencyFlags = 1;
		public int ComputeMips = 1;
		public int ComputeThumbnail = 1;
		public int ComputeReflectivity = 1;
		public Platform Platform = Platform.PC;
		public short CompressionLevel = -1;
		public CompressionMethod CompressionMethod = CompressionMethod.ZSTD;
		public float BumpMapScale = 1;
		public float GammaCorrection = 1;
		public int InvertGreenChannel = 0;
		public byte ConsoleMipScale = 0;
	}

	internal VTF(nint handle, bool managed = true) : base(handle, DLL.vtfpp_vtf_close, managed)
	{
	}

	public VTF() : this(DLL.vtfpp_vtf_create_empty())
	{
	}

	public VTF(ReadOnlySpan<byte> vtfData, bool parseHeaderOnly = false) : this(DLL.vtfpp_vtf_open_from_mem(vtfData, (ulong) vtfData.Length, Convert.ToInt32(parseHeaderOnly)))
	{
	}
	
	public VTF(string vtfPath, bool parseHeaderOnly = false) : this(DLL.vtfpp_vtf_open_from_file(vtfPath, Convert.ToInt32(parseHeaderOnly)))
	{
	}
	
	public static implicit operator bool(VTF vtf)
	{
		vtf.ThrowIfDisposed();
		return Convert.ToBoolean(DLL.vtfpp_vtf_is_valid(vtf.Handle));
	}

	public static bool Create(ReadOnlySpan<byte> imageData, ImageFormat format, ushort width, ushort height, string vtfPath, CreationOptions options)
	{
		return Convert.ToBoolean(DLL.vtfpp_vtf_create_from_mem_and_bake(imageData, (ulong)imageData.Length, format, width, height, vtfPath, options));
	}

	public static bool Create(ImageFormat format, ushort width, ushort height, string vtfPath, CreationOptions options)
	{
		return Convert.ToBoolean(DLL.vtfpp_vtf_create_blank_and_bake(format, width, height, vtfPath, options));
	}

	public static VTF? Create(ReadOnlySpan<byte> imageData, ImageFormat format, ushort width, ushort height, CreationOptions options)
	{
		var handle = DLL.vtfpp_vtf_create_from_mem(imageData, (ulong)imageData.Length, format, width, height, options);
		return handle == nint.Zero ? null : new VTF(handle);
	}

	public static VTF? Create(ImageFormat format, ushort width, ushort height, CreationOptions options)
	{
		var handle = DLL.vtfpp_vtf_create_blank(format, width, height, options);
		return handle == nint.Zero ? null : new VTF(handle);
	}

	public static bool Create(string imagePath, string vtfPath, CreationOptions options)
	{
		return Convert.ToBoolean(DLL.vtfpp_vtf_create_from_file_and_bake(imagePath, vtfPath, options));
	}

	public static VTF? Create(string imagePath, CreationOptions options)
	{
		var handle = DLL.vtfpp_vtf_create_from_file(imagePath, options);
		return handle == nint.Zero ? null : new VTF(handle);
	}

	public Platform VtfPlatform
	{
		get
		{
			ThrowIfDisposed();
			return DLL.vtfpp_vtf_get_platform(Handle);
		}
		set
		{
			ThrowIfDisposed();
			DLL.vtfpp_vtf_set_platform(Handle, value);
		}
	}

	public uint Version
	{
		get
		{
			ThrowIfDisposed();
			return DLL.vtfpp_vtf_get_version(Handle);
		}
		set
		{
			ThrowIfDisposed();
			DLL.vtfpp_vtf_set_version(Handle, value);
		}
	}

	public ImageConversion.ResizeMethod ImageWidthResizeMethod
	{
		get
		{
			ThrowIfDisposed();
			return DLL.vtfpp_vtf_get_image_width_resize_method(Handle);
		}
		set
		{
			ThrowIfDisposed();
			DLL.vtfpp_vtf_set_image_width_resize_method(Handle, value);
		}
	}
	
	public ImageConversion.ResizeMethod ImageHeightResizeMethod
	{
		get
		{
			ThrowIfDisposed();
			return DLL.vtfpp_vtf_get_image_height_resize_method(Handle);
		}
		set
		{
			ThrowIfDisposed();
			DLL.vtfpp_vtf_set_image_height_resize_method(Handle, value);
		}
	}
	
	public (ImageConversion.ResizeMethod, ImageConversion.ResizeMethod) ImageResizeMethods
	{
		get
		{
			ThrowIfDisposed();
			return (DLL.vtfpp_vtf_get_image_width_resize_method(Handle), DLL.vtfpp_vtf_get_image_height_resize_method(Handle));
		}
		set
		{
			ThrowIfDisposed();
			DLL.vtfpp_vtf_set_image_resize_methods(Handle, value.Item1, value.Item2);
		}
	}

	public ushort Width(byte mip = 0)
	{
		ThrowIfDisposed();
		return DLL.vtfpp_vtf_get_width(Handle, mip);
	}

	public ushort PaddedWidth(byte mip = 0)
	{
		ThrowIfDisposed();
		return DLL.vtfpp_vtf_get_padded_width(Handle, mip);
	}
	
	public ushort Height(byte mip = 0)
	{
		ThrowIfDisposed();
		return DLL.vtfpp_vtf_get_height(Handle, mip);
	}

	public ushort PaddedHeight(byte mip = 0)
	{
		ThrowIfDisposed();
		return DLL.vtfpp_vtf_get_padded_height(Handle, mip);
	}

	public void SetSize(ushort width, ushort height, ImageConversion.ResizeFilter filter)
	{
		ThrowIfDisposed();
		DLL.vtfpp_vtf_set_size(Handle, width, height, filter);
	}

	public Flags VtfFlags
	{
		get
		{
			ThrowIfDisposed();
			return (Flags) DLL.vtfpp_vtf_get_flags(Handle);
		}
		set
		{
			ThrowIfDisposed();
			DLL.vtfpp_vtf_set_flags(Handle, (uint) value);
		}
	}

	public void AddFlags(Flags flags)
	{
		ThrowIfDisposed();
		DLL.vtfpp_vtf_add_flags(Handle, (uint) flags);
	}
	
	public void RemoveFlags(Flags flags)
	{
		ThrowIfDisposed();
		DLL.vtfpp_vtf_remove_flags(Handle, (uint) flags);
	}

	public bool IsSrgb
	{
		get
		{
			ThrowIfDisposed();
			return Convert.ToBoolean(DLL.vtfpp_vtf_is_srgb(Handle));
		}
		set
		{
			ThrowIfDisposed();
			DLL.vtfpp_vtf_set_srgb(Handle, Convert.ToInt32(value));
		}
	}

	public void ComputeTransparencyFlags()
	{
		ThrowIfDisposed();
		DLL.vtfpp_vtf_compute_transparency_flags(Handle);
	}

	public static ImageFormat DefaultCompressedFormat(ImageFormat format, uint version, bool isCubeMap)
	{
		return DLL.vtfpp_vtf_get_default_compressed_format(format, version, Convert.ToInt32(isCubeMap));
	}

	public ImageFormat GetFormat()
	{
		ThrowIfDisposed();
		return DLL.vtfpp_vtf_get_format(Handle);
	}

	public void SetFormat(ImageFormat format, ImageConversion.ResizeFilter filter = ImageConversion.ResizeFilter.DEFAULT, float quality = ImageConversion.DEFAULT_COMPRESSED_QUALITY)
	{
		ThrowIfDisposed();
		DLL.vtfpp_vtf_set_format(Handle, format, filter, quality);
	}

	public byte GetMipCount()
	{
		ThrowIfDisposed();
		return DLL.vtfpp_vtf_get_mip_count(Handle);
	}

	public bool SetMipCount(byte mipCount)
	{
		ThrowIfDisposed();
		return Convert.ToBoolean(DLL.vtfpp_vtf_set_mip_count(Handle, mipCount));
	}

	public bool SetRecommendedMipCount()
	{
		ThrowIfDisposed();
		return Convert.ToBoolean(DLL.vtfpp_vtf_set_recommended_mip_count(Handle));
	}

	public void ComputeMips(ImageConversion.ResizeFilter filter = ImageConversion.ResizeFilter.DEFAULT)
	{
		ThrowIfDisposed();
		DLL.vtfpp_vtf_compute_mips(Handle, filter);
	}

	public ushort GetFrameCount()
	{
		ThrowIfDisposed();
		return DLL.vtfpp_vtf_get_frame_count(Handle);
	}

	public bool SetFrameCount(ushort frameCount)
	{
		ThrowIfDisposed();
		return Convert.ToBoolean(DLL.vtfpp_vtf_set_frame_count(Handle, frameCount));
	}

	public byte GetFaceCount()
	{
		ThrowIfDisposed();
		return DLL.vtfpp_vtf_get_face_count(Handle);
	}

	public bool SetFaceCount(bool isCubeMap)
	{
		ThrowIfDisposed();
		return Convert.ToBoolean(DLL.vtfpp_vtf_set_face_count(Handle, Convert.ToInt32(isCubeMap)));
	}

	public ushort GetDepth(byte mip = 0)
	{
		ThrowIfDisposed();
		return DLL.vtfpp_vtf_get_depth(Handle, mip);
	}

	public bool SetDepth(ushort depth)
	{
		ThrowIfDisposed();
		return Convert.ToBoolean(DLL.vtfpp_vtf_set_depth(Handle, depth));
	}

	public bool SetFrameFaceAndDepth(ushort frameCount, bool isCubeMap, ushort depth = 1)
	{
		ThrowIfDisposed();
		return Convert.ToBoolean(DLL.vtfpp_vtf_set_frame_face_and_depth(Handle, frameCount, Convert.ToInt32(isCubeMap), depth));
	}

	public ushort StartFrame
	{
		get
		{
			ThrowIfDisposed();
			return DLL.vtfpp_vtf_get_start_frame(Handle);
		}
		set
		{
			ThrowIfDisposed();
			DLL.vtfpp_vtf_set_start_frame(Handle, value);
		}
	}

	public (float, float, float) Reflectivity
	{
		get
		{
			ThrowIfDisposed();
			float r = 0, g = 0, b = 0;
			DLL.vtfpp_vtf_get_reflectivity(Handle, ref r, ref g, ref b);
			return (r, g, b);
		}
		set
		{
			ThrowIfDisposed();
			DLL.vtfpp_vtf_set_reflectivity(Handle, value.Item1, value.Item2, value.Item3);
		}
	}

	public void ComputeReflectivity()
	{
		ThrowIfDisposed();
		DLL.vtfpp_vtf_compute_reflectivity(Handle);
	}

	public float BumpMapScale
	{
		get
		{
			ThrowIfDisposed();
			return DLL.vtfpp_vtf_get_bumpmap_scale(Handle);
		}
		set
		{
			ThrowIfDisposed();
			DLL.vtfpp_vtf_set_bumpmap_scale(Handle, value);
		}
	}

	public ImageFormat GetThumbnailFormat()
	{
		return DLL.vtfpp_vtf_get_thumbnail_format(Handle);
	}

	public byte GetThumbnailWidth()
	{
		return DLL.vtfpp_vtf_get_thumbnail_width(Handle);
	}

	public byte GetThumbnailHeight()
	{
		return DLL.vtfpp_vtf_get_thumbnail_height(Handle);
	}
	
	public byte GetFallbackWidth()
	{
		return DLL.vtfpp_vtf_get_fallback_width(Handle);
	}

	public byte GetFallbackHeight()
	{
		return DLL.vtfpp_vtf_get_fallback_height(Handle);
	}
	
	public byte GetFallbackMipCount()
	{
		return DLL.vtfpp_vtf_get_fallback_mip_count(Handle);
	}

	public Resource[] Resources
	{
		get
		{
			ThrowIfDisposed();
			var resources = new Resource[DLL.vtfpp_vtf_get_resources_count(Handle)];
			for (var i = 0u; i < resources.Length; i++)
			{
				resources[i] = new Resource(DLL.vtfpp_vtf_get_resource_at_index(Handle, i));
			}
			return resources;
		}
	}

	public Resource? GetResourceWithType(Resource.Type type)
	{
		ThrowIfDisposed();
		var handle = DLL.vtfpp_vtf_get_resource_with_type(Handle, type);
		return handle == nint.Zero ? null : new Resource(handle);
	}

	public byte[] GetPaletteResourceFrame(ushort frame = 0)
	{
		ThrowIfDisposed();
		return new sourcepp.Buffer(DLL.vtfpp_vtf_get_palette_resource_frame(Handle, frame)).Read<byte>();
	}

	public byte[] GetParticleSheetFrameDataRaw(ref ushort width, ref ushort height, uint shtSequenceId, uint shtFrame, byte shtBounds = 0, byte mip = 0, ushort frame = 0, byte face = 0, ushort slice = 0)
	{
		ThrowIfDisposed();
		return new sourcepp.Buffer(DLL.vtfpp_vtf_get_particle_sheet_frame_data_raw(Handle, ref width, ref height, shtSequenceId, shtFrame, shtBounds, mip, frame, face, slice)).Read<byte>();
	}
	
	public byte[] GetParticleSheetFrameDataAs(ImageFormat format, ref ushort width, ref ushort height, uint shtSequenceId, uint shtFrame, byte shtBounds = 0, byte mip = 0, ushort frame = 0, byte face = 0, ushort slice = 0)
	{
		ThrowIfDisposed();
		return new sourcepp.Buffer(DLL.vtfpp_vtf_get_particle_sheet_frame_data_as(Handle, format, ref width, ref height, shtSequenceId, shtFrame, shtBounds, mip, frame, face, slice)).Read<byte>();
	}
	
	// ReSharper disable once InconsistentNaming
	public byte[] GetParticleSheetFrameDataAsRGBA8888(ref ushort width, ref ushort height, uint shtSequenceId, uint shtFrame, byte shtBounds = 0, byte mip = 0, ushort frame = 0, byte face = 0, ushort slice = 0)
	{
		ThrowIfDisposed();
		return new sourcepp.Buffer(DLL.vtfpp_vtf_get_particle_sheet_frame_data_as_rgba8888(Handle, ref width, ref height, shtSequenceId, shtFrame, shtBounds, mip, frame, face, slice)).Read<byte>();
	}

	public void SetParticleSheetResource(SHT sht)
	{
		ThrowIfDisposed();
		DLL.vtfpp_vtf_set_particle_sheet_resource(Handle, sht.Handle);
	}

	public void RemoveParticleSheetResource()
	{
		ThrowIfDisposed();
		DLL.vtfpp_vtf_remove_particle_sheet_resource(Handle);
	}

	public void SetCrcResource(uint value)
	{
		ThrowIfDisposed();
		DLL.vtfpp_vtf_set_crc_resource(Handle, value);
	}

	public void RemoveCrcResource()
	{
		ThrowIfDisposed();
		DLL.vtfpp_vtf_remove_crc_resource(Handle);
	}

	public void SetLodResource(byte u, byte v, byte u360 = 0, byte v360 = 0)
	{
		ThrowIfDisposed();
		DLL.vtfpp_vtf_set_lod_resource(Handle, u, v, u360, v360);
	}
	
	public void RemoveLodResource()
	{
		ThrowIfDisposed();
		DLL.vtfpp_vtf_remove_lod_resource(Handle);
	}
	
	public void SetExtendedFlagsResource(uint value)
	{
		ThrowIfDisposed();
		DLL.vtfpp_vtf_set_extended_flags_resource(Handle, value);
	}

	public void RemoveExtendedFlagsResource()
	{
		ThrowIfDisposed();
		DLL.vtfpp_vtf_remove_extended_flags_resource(Handle);
	}

	public void SetKeyValuesDataResource(string value)
	{
		ThrowIfDisposed();
		DLL.vtfpp_vtf_set_keyvalues_data_resource(Handle, value);
	}

	public void RemoveKeyValuesDataResource()
	{
		ThrowIfDisposed();
		DLL.vtfpp_vtf_remove_keyvalues_data_resource(Handle);
	}
	
	public void SetHotspotDataResource(HOT hot)
	{
		ThrowIfDisposed();
		DLL.vtfpp_vtf_set_hotspot_data_resource(Handle, hot.Handle);
	}

	public void RemoveHotspotDataResource()
	{
		ThrowIfDisposed();
		DLL.vtfpp_vtf_remove_hotspot_data_resource(Handle);
	}

	public short CompressionLevel
	{
		get
		{
			ThrowIfDisposed();
			return DLL.vtfpp_vtf_get_compression_level(Handle);
		}
		set
		{
			ThrowIfDisposed();
			DLL.vtfpp_vtf_set_compression_level(Handle, value);
		}
	}

	public CompressionMethod CompressionMethod
	{
		get
		{
			ThrowIfDisposed();
			return DLL.vtfpp_vtf_get_compression_method(Handle);
		}
		set
		{
			ThrowIfDisposed();
			DLL.vtfpp_vtf_set_compression_method(Handle, value);
		}
	}

	public bool HasImageData
	{
		get
		{
			ThrowIfDisposed();
			return Convert.ToBoolean(DLL.vtfpp_vtf_has_image_data(Handle));
		}
	}

	public Span<byte> GetImageDataRaw(byte mip = 0, ushort frame = 0, byte face = 0, ushort slice = 0)
	{
		ThrowIfDisposed();
		unsafe
		{
			ulong imageLen = 0;
			var imagePtr = DLL.vtfpp_vtf_get_image_data_raw(Handle, ref imageLen, mip, frame, face, slice);
			return new Span<byte>(imagePtr, (int) imageLen);
		}
	}

	public byte[] GetImageDataAs(ImageFormat format, byte mip = 0, ushort frame = 0, byte face = 0, ushort slice = 0)
	{
		ThrowIfDisposed();
		return new sourcepp.Buffer(DLL.vtfpp_vtf_get_image_data_as(Handle, format, mip, frame, face, slice)).Read<byte>();
	}
	
	// ReSharper disable once InconsistentNaming
	public byte[] GetImageDataAsRGBA8888(byte mip = 0, ushort frame = 0, byte face = 0, ushort slice = 0)
	{
		ThrowIfDisposed();
		return new sourcepp.Buffer(DLL.vtfpp_vtf_get_image_data_as_rgba8888(Handle, mip, frame, face, slice)).Read<byte>();
	}

	public bool SetImage(ReadOnlySpan<byte> imageData, ImageFormat format, ushort width, ushort height, ImageConversion.ResizeFilter filter = ImageConversion.ResizeFilter.DEFAULT, byte mip = 0, ushort frame = 0, byte face = 0, ushort slice = 0, float quality = ImageConversion.DEFAULT_COMPRESSED_QUALITY)
	{
		ThrowIfDisposed();
		return Convert.ToBoolean(DLL.vtfpp_vtf_set_image_from_mem(Handle, imageData, (ulong) imageData.Length, format, width, height, filter, mip, frame, face, slice, quality));
	}
	
	public bool SetImage(string imagePath, ImageConversion.ResizeFilter filter = ImageConversion.ResizeFilter.DEFAULT, byte mip = 0, ushort frame = 0, byte face = 0, ushort slice = 0, float quality = ImageConversion.DEFAULT_COMPRESSED_QUALITY)
	{
		ThrowIfDisposed();
		return Convert.ToBoolean(DLL.vtfpp_vtf_set_image_from_file(Handle, imagePath, filter, mip, frame, face, slice, quality));
	}

	public bool SaveImageToFile(string imagePath, byte mip = 0, ushort frame = 0, byte face = 0, ushort slice = 0, ImageConversion.FileFormat fileFormat = ImageConversion.FileFormat.DEFAULT)
	{
		ThrowIfDisposed();
		return Convert.ToBoolean(DLL.vtfpp_vtf_save_image_to_file(Handle, imagePath, mip, frame, face, slice, fileFormat));
	}
	
	public bool HasThumbnailData
	{
		get
		{
			ThrowIfDisposed();
			return Convert.ToBoolean(DLL.vtfpp_vtf_has_thumbnail_data(Handle));
		}
	}

	public Span<byte> GetThumbnailDataRaw()
	{
		ThrowIfDisposed();
		unsafe
		{
			ulong imageLen = 0;
			var imagePtr = DLL.vtfpp_vtf_get_thumbnail_data_raw(Handle, ref imageLen);
			return new Span<byte>(imagePtr, (int) imageLen);
		}
	}

	public byte[] GetThumbnailDataAs(ImageFormat format)
	{
		ThrowIfDisposed();
		return new sourcepp.Buffer(DLL.vtfpp_vtf_get_thumbnail_data_as(Handle, format)).Read<byte>();
	}
	
	// ReSharper disable once InconsistentNaming
	public byte[] GetThumbnailDataAsRGBA8888()
	{
		ThrowIfDisposed();
		return new sourcepp.Buffer(DLL.vtfpp_vtf_get_thumbnail_data_as_rgba8888(Handle)).Read<byte>();
	}

	public void SetThumbnail(ReadOnlySpan<byte> imageData, ImageFormat format, ushort width, ushort height, float quality = ImageConversion.DEFAULT_COMPRESSED_QUALITY)
	{
		ThrowIfDisposed();
		DLL.vtfpp_vtf_set_thumbnail_from_mem(Handle, imageData, (ulong) imageData.Length, format, width, height, quality);
	}
	
	public void SetThumbnail(string imagePath, float quality = ImageConversion.DEFAULT_COMPRESSED_QUALITY)
	{
		ThrowIfDisposed();
		DLL.vtfpp_vtf_set_thumbnail_from_file(Handle, imagePath, quality);
	}

	public void ComputeThumbnail(ImageConversion.ResizeFilter filter, float quality = ImageConversion.DEFAULT_COMPRESSED_QUALITY)
	{
		ThrowIfDisposed();
		DLL.vtfpp_vtf_compute_thumbnail(Handle, filter, quality);
	}

	public void RemoveThumbnail()
	{
		ThrowIfDisposed();
		DLL.vtfpp_vtf_remove_thumbnail(Handle);
	}

	public bool SaveThumbnailToFile(string imagePath, ImageConversion.FileFormat fileFormat = ImageConversion.FileFormat.DEFAULT)
	{
		ThrowIfDisposed();
		return Convert.ToBoolean(DLL.vtfpp_vtf_save_thumbnail_to_file(Handle, imagePath, fileFormat));
	}
	
	public bool HasFallbackData
	{
		get
		{
			ThrowIfDisposed();
			return Convert.ToBoolean(DLL.vtfpp_vtf_has_fallback_data(Handle));
		}
	}
	
	public Span<byte> GetFallbackDataRaw(byte mip = 0, ushort frame = 0, byte face = 0)
	{
		ThrowIfDisposed();
		unsafe
		{
			ulong imageLen = 0;
			var imagePtr = DLL.vtfpp_vtf_get_fallback_data_raw(Handle, ref imageLen, mip, frame, face);
			return new Span<byte>(imagePtr, (int) imageLen);
		}
	}
	
	public byte[] GetFallbackDataAs(ImageFormat format, byte mip = 0, ushort frame = 0, byte face = 0)
	{
		ThrowIfDisposed();
		return new sourcepp.Buffer(DLL.vtfpp_vtf_get_fallback_data_as(Handle, format, mip, frame, face)).Read<byte>();
	}
	
	// ReSharper disable once InconsistentNaming
	public byte[] GetFallbackDataAsRGBA8888(byte mip = 0, ushort frame = 0, byte face = 0)
	{
		ThrowIfDisposed();
		return new sourcepp.Buffer(DLL.vtfpp_vtf_get_fallback_data_as_rgba8888(Handle, mip, frame, face)).Read<byte>();
	}
	
	public void ComputeFallback(ImageConversion.ResizeFilter filter)
	{
		ThrowIfDisposed();
		DLL.vtfpp_vtf_compute_fallback(Handle, filter);
	}

	public void RemoveFallback()
	{
		ThrowIfDisposed();
		DLL.vtfpp_vtf_remove_fallback(Handle);
	}
	
	public bool SaveFallbackToFile(string imagePath, byte mip = 0, ushort frame = 0, byte face = 0, ImageConversion.FileFormat fileFormat = ImageConversion.FileFormat.DEFAULT)
	{
		ThrowIfDisposed();
		return Convert.ToBoolean(DLL.vtfpp_vtf_save_fallback_to_file(Handle, imagePath, mip, frame, face, fileFormat));
	}

	public byte ConsoleMipScale
	{
		get
		{
			ThrowIfDisposed();
			return DLL.vtfpp_vtf_get_console_mip_scale(Handle);
		}
		set
		{
			ThrowIfDisposed();
			DLL.vtfpp_vtf_set_console_mip_scale(Handle, value);
		}
	}

	public ulong EstimateBakeSize(ref bool isExact)
	{
		var isExactInt = 0;
		var estimate = DLL.vtfpp_vtf_estimate_bake_size(Handle, ref isExactInt);
		isExact = Convert.ToBoolean(isExactInt);
		return estimate;
	}
	
	public byte[] Bake()
	{
		ThrowIfDisposed();
		return new sourcepp.Buffer(DLL.vtfpp_vtf_bake(Handle)).Read<byte>();
	}

	public bool Bake(string vtfPath)
	{
		ThrowIfDisposed();
		return Convert.ToBoolean(DLL.vtfpp_vtf_bake_to_file(Handle, vtfPath));
	}
}
