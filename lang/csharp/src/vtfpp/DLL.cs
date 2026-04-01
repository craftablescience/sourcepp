using System;
using System.Runtime.InteropServices;

namespace sourcepp.vtfpp;

internal static partial class DLL
{
	private const string Name = "sourcepp_vtfppc";

	[LibraryImport(Name)]
	public static partial nint vtfpp_hot_create();

	[LibraryImport(Name)]
	public static partial nint vtfpp_hot_open_from_mem(ReadOnlySpan<byte> hotData, ulong hotLen);

	[LibraryImport(Name, StringMarshalling = StringMarshalling.Utf8)]
	public static partial nint vtfpp_hot_open_from_file(string hotPath);

	[LibraryImport(Name)]
	public static partial void vtfpp_hot_close(ref nint handle);

	[LibraryImport(Name)]
	public static partial int vtfpp_hot_is_valid(nint handle);

	[LibraryImport(Name)]
	public static partial byte vtfpp_hot_get_version(nint handle);

	[LibraryImport(Name)]
	public static partial void vtfpp_hot_set_version(nint handle, byte version);

	[LibraryImport(Name)]
	public static partial byte vtfpp_hot_get_flags(nint handle);

	[LibraryImport(Name)]
	public static partial void vtfpp_hot_set_flags(nint handle, byte flags);

	[LibraryImport(Name)]
	public static partial ushort vtfpp_hot_get_rect_count(nint handle);

	[LibraryImport(Name)]
	public static partial HOT.Rect vtfpp_hot_get_rect_at_index(nint handle, ushort index);

	[LibraryImport(Name)]
	public static partial void vtfpp_hot_set_rect_at_index(nint handle, ushort index, HOT.Rect rect);

	[LibraryImport(Name)]
	public static partial void vtfpp_hot_add_rect(nint handle, HOT.Rect rect);

	[LibraryImport(Name)]
	public static partial void vtfpp_hot_remove_rect_at_index(nint handle, ushort index);

	[LibraryImport(Name)]
	public static partial sourcepp.DLL.Buffer vtfpp_hot_bake(nint handle);

	[LibraryImport(Name, StringMarshalling = StringMarshalling.Utf8)]
	public static partial int vtfpp_hot_bake_to_file(nint handle, string hotPath);

	[LibraryImport(Name)]
	public static partial sourcepp.DLL.Buffer vtfpp_image_conversion_convert_image_data_to_format(ReadOnlySpan<byte> buffer, ulong bufferLen, ImageFormat oldFormat, ImageFormat newFormat, ushort width, ushort height, float quality);

	[LibraryImport(Name)]
	public static partial sourcepp.DLL.Buffer vtfpp_image_conversion_convert_several_image_data_to_format(ReadOnlySpan<byte> buffer, ulong bufferLen, ImageFormat oldFormat, ImageFormat newFormat, byte mipCount, ushort frameCount, byte faceCount, ushort width, ushort height, ushort depth, float quality);

	[LibraryImport(Name)]
	public static partial sourcepp.DLL.Buffer vtfpp_image_conversion_convert_hdri_to_cubemap(ReadOnlySpan<byte> buffer, ulong bufferLen, ImageFormat format, ushort width, ushort height, ushort resolution, int bilinear);

	[LibraryImport(Name)]
	public static partial sourcepp.DLL.Buffer vtfpp_image_conversion_compress_bgra8888_hdr(ReadOnlySpan<byte> buffer, ulong bufferLen, float overbrightFactor);

	[LibraryImport(Name)]
	public static partial sourcepp.DLL.Buffer vtfpp_image_conversion_decompress_bgra8888_hdr(ReadOnlySpan<byte> buffer, ulong bufferLen, float overbrightFactor);

	[LibraryImport(Name)]
	public static partial sourcepp.DLL.Buffer vtfpp_image_conversion_compress_rgba16161616_hdr(ReadOnlySpan<byte> buffer, ulong bufferLen, int flipExponentAndSignificand);

	[LibraryImport(Name)]
	public static partial sourcepp.DLL.Buffer vtfpp_image_conversion_decompress_rgba16161616_hdr(ReadOnlySpan<byte> buffer, ulong bufferLen, int flipExponentAndSignificand);
	
	[LibraryImport(Name)]
	public static partial ImageConversion.FileFormat vtfpp_image_conversion_get_default_file_format_for_image_format(ImageFormat format);

	[LibraryImport(Name)]
	public static partial sourcepp.DLL.Buffer vtfpp_image_conversion_convert_image_data_to_file(ReadOnlySpan<byte> buffer, ulong bufferLen, ImageFormat format, ushort width, ushort height, ImageConversion.FileFormat fileFormat);

	[LibraryImport(Name)]
	public static partial sourcepp.DLL.Buffer vtfpp_image_conversion_convert_file_to_image_data(ReadOnlySpan<byte> buffer, ulong bufferLen, ref ImageFormat format, ref int width, ref int height, ref int frameCount);

	[LibraryImport(Name)]
	public static partial ushort vtfpp_image_conversion_get_resized_dim(ushort n, ImageConversion.ResizeMethod method);

	[LibraryImport(Name)]
	public static partial void vtfpp_image_conversion_set_resized_dims(ref ushort width, ImageConversion.ResizeMethod widthResize, ref ushort height, ImageConversion.ResizeMethod heightResize);

	[LibraryImport(Name)]
	public static partial sourcepp.DLL.Buffer vtfpp_image_conversion_resize_image_data(ReadOnlySpan<byte> buffer, ulong bufferLen, ImageFormat format, ushort width, ushort newWidth, ushort height, ushort newHeight, int srgb, ImageConversion.ResizeFilter filter, ImageConversion.ResizeEdge edge);

	[LibraryImport(Name)]
	public static partial sourcepp.DLL.Buffer vtfpp_image_conversion_resize_image_data_strict(ReadOnlySpan<byte> buffer, ulong bufferLen, ImageFormat format, ushort width, ushort newWidth, ref ushort widthOut, ImageConversion.ResizeMethod widthResize, ushort height, ushort newHeight, ref ushort heightOut, ImageConversion.ResizeMethod heightResize, int srgb, ImageConversion.ResizeFilter filter, ImageConversion.ResizeEdge edge);

	[LibraryImport(Name)]
	public static partial sourcepp.DLL.Buffer vtfpp_image_conversion_crop_image_data(ReadOnlySpan<byte> buffer, ulong bufferLen, ImageFormat format, ushort width, ushort newWidth, ushort xOffset, ushort height, ushort newHeight, ushort yOffset);

	[LibraryImport(Name)]
	public static partial void vtfpp_image_conversion_gamma_correct_image_data(Span<byte> buffer, ulong bufferLen, ImageFormat format, ushort width, ushort height, float gamma);

	[LibraryImport(Name)]
	public static partial void vtfpp_image_conversion_invert_green_channel_for_image_data(Span<byte> buffer, ulong bufferLen, ImageFormat format, ushort width, ushort height);

	[LibraryImport(Name)]
   	public static partial void vtfpp_image_conversion_hable_tonemap_image_data(Span<byte> buffer, ulong bufferLen, ImageFormat format, ushort width, ushort height);

	[LibraryImport(Name)]
	public static partial sbyte vtfpp_image_format_details_red(ImageFormat format);

	[LibraryImport(Name)]
	public static partial sbyte vtfpp_image_format_details_decompressed_red(ImageFormat format);

	[LibraryImport(Name)]
	public static partial sbyte vtfpp_image_format_details_green(ImageFormat format);

	[LibraryImport(Name)]
	public static partial sbyte vtfpp_image_format_details_decompressed_green(ImageFormat format);

	[LibraryImport(Name)]
	public static partial sbyte vtfpp_image_format_details_blue(ImageFormat format);

	[LibraryImport(Name)]
	public static partial sbyte vtfpp_image_format_details_decompressed_blue(ImageFormat format);

	[LibraryImport(Name)]
	public static partial sbyte vtfpp_image_format_details_alpha(ImageFormat format);

	[LibraryImport(Name)]
	public static partial sbyte vtfpp_image_format_details_decompressed_alpha(ImageFormat format);

	[LibraryImport(Name)]
	public static partial byte vtfpp_image_format_details_bpp(ImageFormat format);

	[LibraryImport(Name)]
	public static partial ImageFormat vtfpp_image_format_details_container_format(ImageFormat format);

	[LibraryImport(Name)]
	public static partial int vtfpp_image_format_details_large(ImageFormat format);

	[LibraryImport(Name)]
	public static partial int vtfpp_image_format_details_decimal(ImageFormat format);

	[LibraryImport(Name)]
	public static partial int vtfpp_image_format_details_compressed(ImageFormat format);

	[LibraryImport(Name)]
	public static partial int vtfpp_image_format_details_compressed_hdr(ImageFormat format);

	[LibraryImport(Name)]
	public static partial int vtfpp_image_format_details_transparent(ImageFormat format);

	[LibraryImport(Name)]
	public static partial int vtfpp_image_format_details_opaque(ImageFormat format);

	[LibraryImport(Name)]
	public static partial int vtfpp_image_format_details_console(ImageFormat format);

	[LibraryImport(Name)]
	public static partial int vtfpp_image_format_details_titanfall(ImageFormat format);

	[LibraryImport(Name)]
	public static partial int vtfpp_image_format_details_strata(ImageFormat format);

	[LibraryImport(Name)]
	public static partial ushort vtfpp_image_dimensions_get_mip_dim(byte mip, ushort dim, int addCompressedFormatPadding);

	[LibraryImport(Name)]
	public static partial byte vtfpp_image_dimensions_get_maximum_mip_count(ushort width, ushort height, ushort depth);

	[LibraryImport(Name)]
	public static partial uint vtfpp_image_format_details_get_data_length(ImageFormat format, ushort width, ushort height, ushort depth);

	[LibraryImport(Name)]
	public static partial uint vtfpp_image_format_details_get_data_length_ex(ImageFormat format, byte mipCount, ushort frameCount, byte faceCount, ushort width, ushort height, ushort depth);

	[LibraryImport(Name)]
	public static partial uint vtfpp_image_format_details_get_data_length_xbox(int padded, ImageFormat format, byte mipCount, ushort frameCount, byte faceCount, ushort width, ushort height, ushort depth);

	[LibraryImport(Name)]
	public static partial int vtfpp_image_format_details_get_data_position(ref uint offset, ref uint length, ImageFormat format, byte mip, byte mipCount, ushort frame, ushort frameCount, byte face, byte faceCount, ushort width, ushort height, ushort slice, ushort depth);

	[LibraryImport(Name)]
	public static partial int vtfpp_image_format_details_get_data_position_xbox(ref uint offset, ref uint length, int padded, ImageFormat format, byte mip, byte mipCount, ushort frame, ushort frameCount, byte face, byte faceCount, ushort width, ushort height, ushort slice, ushort depth);

	[LibraryImport(Name)]
	public static partial sourcepp.DLL.Buffer vtfpp_image_quantize_convert_p8_image_data_to_bgra8888(ReadOnlySpan<byte> paletteData, ulong paletteDataLen, ReadOnlySpan<byte> imageData, ulong imageDataLen);

	[LibraryImport(Name)]
	public static partial nint vtfpp_ppl_create(uint modelChecksum);

	[LibraryImport(Name)]
	public static partial nint vtfpp_ppl_create_with_options(uint modelChecksum, ImageFormat format, uint version);

	[LibraryImport(Name)]
	public static partial nint vtfpp_ppl_open_from_mem(ReadOnlySpan<byte> buffer, ulong bufferLen);

	[LibraryImport(Name, StringMarshalling = StringMarshalling.Utf8)]
	public static partial nint vtfpp_ppl_open_from_file(string pplPath);

	[LibraryImport(Name)]
	public static partial void vtfpp_ppl_close(ref nint handle);

	[LibraryImport(Name)]
	public static partial int vtfpp_ppl_is_valid(nint handle);

	[LibraryImport(Name)]
	public static partial uint vtfpp_ppl_get_version(nint handle);

	[LibraryImport(Name)]
	public static partial void vtfpp_ppl_set_version(nint handle, uint version);

	[LibraryImport(Name)]
	public static partial uint vtfpp_ppl_get_model_checksum(nint handle);

	[LibraryImport(Name)]
	public static partial void vtfpp_ppl_set_model_checksum(nint handle, uint modelChecksum);

	[LibraryImport(Name)]
	public static partial ImageFormat vtfpp_ppl_get_format(nint handle);

	[LibraryImport(Name)]
	public static partial void vtfpp_ppl_set_format(nint handle, ImageFormat format, float quality);

	[LibraryImport(Name)]
	public static partial int vtfpp_ppl_has_image_for_lod(nint handle, uint lod);

	[LibraryImport(Name)]
	public static partial sourcepp.DLL.Buffer vtfpp_ppl_get_image_lods(nint handle);

	[LibraryImport(Name)]
	public static unsafe partial byte* vtfpp_ppl_get_image_raw(nint handle, ref ulong imageLen, ref uint width, ref uint height, uint lod);

	[LibraryImport(Name)]
	public static partial sourcepp.DLL.Buffer vtfpp_ppl_get_image_as(nint handle, ref uint width, ref uint height, ImageFormat format, uint lod);

	[LibraryImport(Name)]
	public static partial sourcepp.DLL.Buffer vtfpp_ppl_get_image_as_rgb888(nint handle, ref uint width, ref uint height, uint lod);

	[LibraryImport(Name, StringMarshalling = StringMarshalling.Utf8)]
	public static partial int vtfpp_ppl_set_image_from_file(nint handle, string imagePath, uint lod, float quality);

	[LibraryImport(Name, StringMarshalling = StringMarshalling.Utf8)]
	public static partial int vtfpp_ppl_set_image_from_file_with_options(nint handle, string imagePath, uint resizedWidth, uint resizedHeight, uint lod, ImageConversion.ResizeFilter filter, float quality);

	[LibraryImport(Name)]
	public static partial int vtfpp_ppl_set_image_from_mem(nint handle, ReadOnlySpan<byte> buffer, ulong bufferLen, ImageFormat format, uint width, uint height, uint lod, float quality);

	[LibraryImport(Name)]
	public static partial int vtfpp_ppl_set_image_from_mem_with_options(nint handle, ReadOnlySpan<byte> buffer, ulong bufferLen, ImageFormat format, uint width, uint height, uint resizedWidth, uint resizedHeight, uint lod, ImageConversion.ResizeFilter filter, float quality);

	[LibraryImport(Name)]
	public static partial int vtfpp_ppl_remove_image(nint handle, uint lod);

	[LibraryImport(Name, StringMarshalling = StringMarshalling.Utf8)]
	public static partial int vtfpp_ppl_save_image_to_file(nint handle, string imagePath, uint lod, ImageConversion.FileFormat fileFormat);

	[LibraryImport(Name)]
	public static partial sourcepp.DLL.Buffer vtfpp_ppl_bake(nint handle);

	[LibraryImport(Name, StringMarshalling = StringMarshalling.Utf8)]
	public static partial int vtfpp_ppl_bake_to_file(nint handle, string pplPath);

	[LibraryImport(Name)]
	public static partial nint vtfpp_psframes_open_from_mem(ReadOnlySpan<byte> buffer, ulong bufferLen);

	[LibraryImport(Name, StringMarshalling = StringMarshalling.Utf8)]
	public static partial nint vtfpp_psframes_open_from_file(string psframesPath);

	[LibraryImport(Name)]
	public static partial void vtfpp_psframes_close(ref nint handle);

	[LibraryImport(Name)]
	public static partial uint vtfpp_psframes_get_frame_count(nint handle);

	[LibraryImport(Name)]
	public static partial uint vtfpp_psframes_get_fps(nint handle);

	[LibraryImport(Name)]
	public static partial ushort vtfpp_psframes_get_width(nint handle);

	[LibraryImport(Name)]
	public static partial ushort vtfpp_psframes_get_height(nint handle);

	[LibraryImport(Name)]
	public static partial sourcepp.DLL.Buffer vtfpp_psframes_get_image_data_as(nint handle, ImageFormat newFormat, uint frame);

	[LibraryImport(Name)]
	public static partial sourcepp.DLL.Buffer vtfpp_psframes_get_image_data_as_bgr888(nint handle, uint frame);

	[LibraryImport(Name)]
	public static partial nint vtfpp_sht_create();

	[LibraryImport(Name)]
	public static partial nint vtfpp_sht_open_from_mem(ReadOnlySpan<byte> shtData, ulong shtLen);

	[LibraryImport(Name, StringMarshalling = StringMarshalling.Utf8)]
	public static partial nint vtfpp_sht_open_from_file(string shtPath);

	[LibraryImport(Name)]
	public static partial void vtfpp_sht_close(ref nint handle);

	[LibraryImport(Name)]
	public static partial int vtfpp_sht_is_valid(nint handle);

	[LibraryImport(Name)]
	public static partial uint vtfpp_sht_get_version(nint handle);

	[LibraryImport(Name)]
	public static partial void vtfpp_sht_set_version(nint handle, uint version);

	[LibraryImport(Name)]
	public static partial uint vtfpp_sht_get_sequences_count(nint handle);

	[LibraryImport(Name)]
	public static partial SHT.Sequence vtfpp_sht_get_sequence_at_index(nint handle, uint index);

	[LibraryImport(Name)]
	public static partial SHT.Sequence vtfpp_sht_get_sequence_with_id(nint handle, uint id);

	[LibraryImport(Name)]
	public static partial void vtfpp_sht_set_sequence_at_index(nint handle, uint index, SHT.Sequence sequence);

	[LibraryImport(Name)]
	public static partial void vtfpp_sht_set_sequence_with_id(nint handle, uint id, SHT.Sequence sequence);

	[LibraryImport(Name)]
	public static partial void vtfpp_sht_add_sequence(nint handle, SHT.Sequence sequence);

	[LibraryImport(Name)]
	public static partial void vtfpp_sht_remove_sequence_at_index(nint handle, uint index);

	[LibraryImport(Name)]
	public static partial void vtfpp_sht_remove_sequence_with_id(nint handle, uint id);

	[LibraryImport(Name)]
	public static partial SHT.Sequence.Frame vtfpp_sht_get_frame(nint handle, uint sequenceId, uint index);

	[LibraryImport(Name)]
	public static partial void vtfpp_sht_set_frame(nint handle, uint sequenceId, uint index, SHT.Sequence.Frame frame);

	[LibraryImport(Name)]
	public static partial void vtfpp_sht_add_frame(nint handle, uint sequenceId, SHT.Sequence.Frame frame);

	[LibraryImport(Name)]
	public static partial void vtfpp_sht_remove_frame(nint handle, uint sequenceId, uint index);

	[LibraryImport(Name)]
	public static partial byte vtfpp_sht_get_frame_bounds_count(nint handle);

	[LibraryImport(Name)]
	public static partial sourcepp.DLL.Buffer vtfpp_sht_bake(nint handle);

	[LibraryImport(Name, StringMarshalling = StringMarshalling.Utf8)]
	public static partial int vtfpp_sht_bake_to_file(nint handle, string shtPath);

	[LibraryImport(Name)]
	public static partial nint vtfpp_ttx_create(nint vtf);

	[LibraryImport(Name)]
	public static partial nint vtfpp_ttx_open_from_mem(ReadOnlySpan<byte> tthData, ulong tthLen, ReadOnlySpan<byte> ttzData, ulong ttzLen);

	[LibraryImport(Name, StringMarshalling = StringMarshalling.Utf8)]
	public static partial nint vtfpp_ttx_open_from_file(string tthPath, string ttzPath);

	[LibraryImport(Name)]
	public static partial void vtfpp_ttx_close(ref nint handle);

	[LibraryImport(Name)]
	public static partial int vtfpp_ttx_is_valid(nint handle);

	[LibraryImport(Name)]
	public static partial byte vtfpp_ttx_get_major_version(nint handle);

	[LibraryImport(Name)]
	public static partial byte vtfpp_ttx_get_minor_version(nint handle);

	[LibraryImport(Name)]
	public static partial void vtfpp_ttx_set_version(nint handle, byte majorVersion, byte minorVersion);

	[LibraryImport(Name)]
	public static partial void vtfpp_ttx_set_major_version(nint handle, byte majorVersion);

	[LibraryImport(Name)]
	public static partial void vtfpp_ttx_set_minor_version(nint handle, byte minorVersion);

	[LibraryImport(Name)]
	public static partial byte vtfpp_ttx_get_aspect_ratio_type(nint handle);

	[LibraryImport(Name)]
	public static partial void vtfpp_ttx_set_aspect_ratio_type(nint handle, byte aspectRatioType);

	[LibraryImport(Name)]
	public static partial sourcepp.DLL.Buffer vtfpp_ttx_get_mip_flags(nint handle);

	[LibraryImport(Name)]
	public static partial void vtfpp_ttx_set_mip_flags(nint handle, ReadOnlySpan<ulong> mipFlags, ulong mipFlagsLen);

	[LibraryImport(Name)]
	public static partial nint vtfpp_ttx_get_vtf(nint handle);

	[LibraryImport(Name)]
	public static partial void vtfpp_ttx_set_vtf(nint handle, nint vtf);

	[LibraryImport(Name)]
	public static partial short vtfpp_ttx_get_compression_level(nint handle);

	[LibraryImport(Name)]
	public static partial void vtfpp_ttx_set_compression_level(nint handle, short compressionLevel);

	[LibraryImport(Name)]
	public static partial sourcepp.DLL.Buffer vtfpp_ttx_bake(nint handle, ref ulong tthLen, ref ulong ttzLen);

	[LibraryImport(Name, StringMarshalling = StringMarshalling.Utf8)]
	public static partial int vtfpp_ttx_bake_to_file(nint handle, string tthPath, string ttzPath);

	[LibraryImport(Name)]
	public static partial nint vtfpp_vbf_open_from_mem(ReadOnlySpan<byte> buffer, ulong bufferLen);

	[LibraryImport(Name, StringMarshalling = StringMarshalling.Utf8)]
	public static partial nint vtfpp_vbf_open_from_file(string vbfPath);

	[LibraryImport(Name)]
	public static partial void vtfpp_vbf_close(ref nint handle);

	[LibraryImport(Name)]
	public static partial int vtfpp_vbf_is_valid(nint handle);

	[LibraryImport(Name)]
	public static partial ushort vtfpp_vbf_get_page_width(nint handle);

	[LibraryImport(Name)]
	public static partial ushort vtfpp_vbf_get_page_height(nint handle);

	[LibraryImport(Name)]
	public static partial ushort vtfpp_vbf_get_max_glyph_width(nint handle);

	[LibraryImport(Name)]
	public static partial ushort vtfpp_vbf_get_max_glyph_height(nint handle);

	[LibraryImport(Name)]
	public static partial VBF.Flags vtfpp_vbf_get_flags(nint handle);

	[LibraryImport(Name)]
	public static partial ushort vtfpp_vbf_get_ascent(nint handle);

	[LibraryImport(Name)]
	public static partial VBF.Glyph vtfpp_vbf_get_glyph(nint handle, byte index);

	[LibraryImport(Name)]
	public static partial Resource.Type vtfpp_resource_get_type(nint handle);

	[LibraryImport(Name)]
	public static partial Resource.Flags vtfpp_resource_get_flags(nint handle);

	[LibraryImport(Name)]
	public static unsafe partial byte* vtfpp_resource_get_data(nint handle, ref ulong dataLen);

	[LibraryImport(Name)]
	public static partial sourcepp.DLL.Buffer vtfpp_resource_get_data_as_palette(nint handle, ushort frame);

	[LibraryImport(Name)]
	public static partial nint vtfpp_resource_get_data_as_particle_sheet(nint handle);

	[LibraryImport(Name)]
	public static partial uint vtfpp_resource_get_data_as_crc(nint handle);

	[LibraryImport(Name)]
	public static partial uint vtfpp_resource_get_data_as_extended_flags(nint handle);

	[LibraryImport(Name)]
	public static partial void vtfpp_resource_get_data_as_lod_control_info(nint handle, ref byte u, ref byte v, ref byte u360, ref byte v360);

	[LibraryImport(Name)]
	public static partial sourcepp.DLL.String vtfpp_resource_get_data_as_keyvalues_data(nint handle);

	[LibraryImport(Name)]
	public static partial nint vtfpp_resource_get_data_as_hotspot_data(nint handle);

	[LibraryImport(Name)]
	public static partial short vtfpp_resource_get_data_as_aux_compression_level(nint handle);

	[LibraryImport(Name)]
	public static partial CompressionMethod vtfpp_resource_get_data_as_aux_compression_method(nint handle);

	[LibraryImport(Name)]
	public static partial uint vtfpp_resource_get_data_as_aux_compression_length(nint handle, byte mip, byte mipCount, ushort frame, ushort frameCount, ushort face, ushort faceCount);

	[LibraryImport(Name)]
	public static partial nint vtfpp_vtf_create_empty();

	[LibraryImport(Name)]
	public static partial nint vtfpp_vtf_open_from_mem(ReadOnlySpan<byte> vtfData, ulong vtfLen, int parseHeaderOnly, int hdr);

	[LibraryImport(Name, StringMarshalling = StringMarshalling.Utf8)]
	public static partial nint vtfpp_vtf_open_from_file(string vtfPath, int parseHeaderOnly);

	[LibraryImport(Name)]
	public static partial void vtfpp_vtf_close(ref nint handle);

	[LibraryImport(Name)]
	public static partial int vtfpp_vtf_is_valid(nint handle);

	[LibraryImport(Name, StringMarshalling = StringMarshalling.Utf8)]
	public static partial int vtfpp_vtf_create_from_mem_and_bake(ReadOnlySpan<byte> imageData, ulong imageLen, ImageFormat format, ushort width, ushort height, string vtfPath, VTF.CreationOptions options);

	[LibraryImport(Name, StringMarshalling = StringMarshalling.Utf8)]
	public static partial int vtfpp_vtf_create_blank_and_bake(ImageFormat format, ushort width, ushort height, string vtfPath, VTF.CreationOptions options);

	[LibraryImport(Name)]
	public static partial nint vtfpp_vtf_create_from_mem(ReadOnlySpan<byte> imageData, ulong imageLen, ImageFormat format, ushort width, ushort height, VTF.CreationOptions options);

	[LibraryImport(Name)]
	public static partial nint vtfpp_vtf_create_blank(ImageFormat format, ushort width, ushort height, VTF.CreationOptions options);

	[LibraryImport(Name, StringMarshalling = StringMarshalling.Utf8)]
	public static partial int vtfpp_vtf_create_from_file_and_bake(string imagePath, string vtfPath, VTF.CreationOptions options);

	[LibraryImport(Name, StringMarshalling = StringMarshalling.Utf8)]
	public static partial nint vtfpp_vtf_create_from_file(string imagePath, VTF.CreationOptions options);

	[LibraryImport(Name)]
	public static partial VTF.Platform vtfpp_vtf_get_platform(nint handle);

	[LibraryImport(Name)]
	public static partial void vtfpp_vtf_set_platform(nint handle, VTF.Platform platform);

	[LibraryImport(Name)]
	public static partial uint vtfpp_vtf_get_version(nint handle);

	[LibraryImport(Name)]
	public static partial void vtfpp_vtf_set_version(nint handle, uint version);

	[LibraryImport(Name)]
	public static partial ImageConversion.ResizeMethod vtfpp_vtf_get_image_width_resize_method(nint handle);

	[LibraryImport(Name)]
	public static partial ImageConversion.ResizeMethod vtfpp_vtf_get_image_height_resize_method(nint handle);

	[LibraryImport(Name)]
	public static partial void vtfpp_vtf_set_image_resize_methods(nint handle, ImageConversion.ResizeMethod imageWidthResizeMethod, ImageConversion.ResizeMethod imageHeightResizeMethod);

	[LibraryImport(Name)]
	public static partial void vtfpp_vtf_set_image_width_resize_method(nint handle, ImageConversion.ResizeMethod imageWidthResizeMethod);

	[LibraryImport(Name)]
	public static partial void vtfpp_vtf_set_image_height_resize_method(nint handle, ImageConversion.ResizeMethod imageHeightResizeMethod);

	[LibraryImport(Name)]
	public static partial ushort vtfpp_vtf_get_width(nint handle, byte mip);

	[LibraryImport(Name)]
	public static partial ushort vtfpp_vtf_get_padded_width(nint handle, byte mip);

	[LibraryImport(Name)]
	public static partial ushort vtfpp_vtf_get_height(nint handle, byte mip);

	[LibraryImport(Name)]
	public static partial ushort vtfpp_vtf_get_padded_height(nint handle, byte mip);

	[LibraryImport(Name)]
	public static partial void vtfpp_vtf_set_size(nint handle, ushort width, ushort height, ImageConversion.ResizeFilter filter);

	[LibraryImport(Name)]
	public static partial uint vtfpp_vtf_get_flags(nint handle);

	[LibraryImport(Name)]
	public static partial void vtfpp_vtf_set_flags(nint handle, uint flags);

	[LibraryImport(Name)]
	public static partial void vtfpp_vtf_add_flags(nint handle, uint flags);

	[LibraryImport(Name)]
	public static partial void vtfpp_vtf_remove_flags(nint handle, uint flags);

	[LibraryImport(Name)]
	public static partial int vtfpp_vtf_is_srgb(nint handle);

	[LibraryImport(Name)]
	public static partial void vtfpp_vtf_set_srgb(nint handle, int srgb);

	[LibraryImport(Name)]
	public static partial void vtfpp_vtf_compute_transparency_flags(nint handle);

	[LibraryImport(Name)]
	public static partial ImageFormat vtfpp_vtf_get_default_compressed_format(ImageFormat inputFormat, uint version, int isCubeMap);

	[LibraryImport(Name)]
	public static partial ImageFormat vtfpp_vtf_get_format(nint handle);

	[LibraryImport(Name)]
	public static partial void vtfpp_vtf_set_format(nint handle, ImageFormat format, ImageConversion.ResizeFilter filter, float quality);

	[LibraryImport(Name)]
	public static partial byte vtfpp_vtf_get_mip_count(nint handle);

	[LibraryImport(Name)]
	public static partial int vtfpp_vtf_set_mip_count(nint handle, byte mipCount);

	[LibraryImport(Name)]
	public static partial int vtfpp_vtf_set_recommended_mip_count(nint handle);

	[LibraryImport(Name)]
	public static partial void vtfpp_vtf_compute_mips(nint handle, ImageConversion.ResizeFilter filter);

	[LibraryImport(Name)]
	public static partial ushort vtfpp_vtf_get_frame_count(nint handle);

	[LibraryImport(Name)]
	public static partial int vtfpp_vtf_set_frame_count(nint handle, ushort frameCount);

	[LibraryImport(Name)]
	public static partial byte vtfpp_vtf_get_face_count(nint handle);

	[LibraryImport(Name)]
	public static partial int vtfpp_vtf_set_face_count(nint handle, int isCubeMap);

	[LibraryImport(Name)]
	public static partial ushort vtfpp_vtf_get_depth(nint handle, byte mip);

	[LibraryImport(Name)]
	public static partial int vtfpp_vtf_set_depth(nint handle, ushort depth);

	[LibraryImport(Name)]
	public static partial int vtfpp_vtf_set_frame_face_and_depth(nint handle, ushort frameCount, int isCubeMap, ushort depth);

	[LibraryImport(Name)]
	public static partial ushort vtfpp_vtf_get_start_frame(nint handle);

	[LibraryImport(Name)]
	public static partial void vtfpp_vtf_set_start_frame(nint handle, ushort startFrame);

	[LibraryImport(Name)]
	public static partial void vtfpp_vtf_get_reflectivity(nint handle, ref float r, ref float g, ref float b);

	[LibraryImport(Name)]
	public static partial void vtfpp_vtf_set_reflectivity(nint handle, float r, float g, float b);

	[LibraryImport(Name)]
	public static partial void vtfpp_vtf_compute_reflectivity(nint handle);

	[LibraryImport(Name)]
	public static partial float vtfpp_vtf_get_bumpmap_scale(nint handle);

	[LibraryImport(Name)]
	public static partial void vtfpp_vtf_set_bumpmap_scale(nint handle, float bumpMapScale);

	[LibraryImport(Name)]
	public static partial ImageFormat vtfpp_vtf_get_thumbnail_format(nint handle);

	[LibraryImport(Name)]
	public static partial byte vtfpp_vtf_get_thumbnail_width(nint handle);

	[LibraryImport(Name)]
	public static partial byte vtfpp_vtf_get_thumbnail_height(nint handle);

	[LibraryImport(Name)]
	public static partial byte vtfpp_vtf_get_fallback_width(nint handle, byte mip);

	[LibraryImport(Name)]
   	public static partial byte vtfpp_vtf_get_padded_fallback_width(nint handle, byte mip);

	[LibraryImport(Name)]
	public static partial byte vtfpp_vtf_get_fallback_height(nint handle, byte mip);

	[LibraryImport(Name)]
	public static partial byte vtfpp_vtf_get_padded_fallback_height(nint handle, byte mip);

	[LibraryImport(Name)]
	public static partial byte vtfpp_vtf_get_fallback_mip_count(nint handle);

	[LibraryImport(Name)]
    public static partial byte vtfpp_vtf_has_native_resource_support(nint handle);

	[LibraryImport(Name)]
	public static partial uint vtfpp_vtf_get_resources_count(nint handle);

	[LibraryImport(Name)]
	public static partial nint vtfpp_vtf_get_resource_at_index(nint handle, uint index);

	[LibraryImport(Name)]
	public static partial nint vtfpp_vtf_get_resource_with_type(nint handle, Resource.Type type);

	[LibraryImport(Name)]
	public static partial sourcepp.DLL.Buffer vtfpp_vtf_get_palette_resource_frame(nint handle, ushort frame);

	[LibraryImport(Name)]
	public static partial sourcepp.DLL.Buffer vtfpp_vtf_get_particle_sheet_frame_data_raw(nint handle, ref ushort width, ref ushort height, uint shtSequenceId, uint shtFrame, byte shtBounds, byte mip, ushort frame, byte face, ushort slice);

	[LibraryImport(Name)]
	public static partial sourcepp.DLL.Buffer vtfpp_vtf_get_particle_sheet_frame_data_as(nint handle, ImageFormat format, ref ushort width, ref ushort height, uint shtSequenceId, uint shtFrame, byte shtBounds, byte mip, ushort frame, byte face, ushort slice);

	[LibraryImport(Name)]
	public static partial sourcepp.DLL.Buffer vtfpp_vtf_get_particle_sheet_frame_data_as_rgba8888(nint handle, ref ushort width, ref ushort height, uint shtSequenceId, uint shtFrame, byte shtBounds, byte mip, ushort frame, byte face, ushort slice);

	[LibraryImport(Name)]
	public static partial void vtfpp_vtf_set_particle_sheet_resource(nint handle, nint sht);

	[LibraryImport(Name)]
	public static partial void vtfpp_vtf_remove_particle_sheet_resource(nint handle);

	[LibraryImport(Name)]
	public static partial void vtfpp_vtf_set_crc_resource(nint handle, uint value);

	[LibraryImport(Name)]
	public static partial void vtfpp_vtf_remove_crc_resource(nint handle);

	[LibraryImport(Name)]
	public static partial void vtfpp_vtf_set_lod_resource(nint handle, byte u, byte v, byte u360, byte v360);

	[LibraryImport(Name)]
	public static partial void vtfpp_vtf_remove_lod_resource(nint handle);

	[LibraryImport(Name)]
	public static partial void vtfpp_vtf_set_extended_flags_resource(nint handle, uint value);

	[LibraryImport(Name)]
	public static partial void vtfpp_vtf_remove_extended_flags_resource(nint handle);

	[LibraryImport(Name, StringMarshalling = StringMarshalling.Utf8)]
	public static partial void vtfpp_vtf_set_keyvalues_data_resource(nint handle, string value);

	[LibraryImport(Name)]
	public static partial void vtfpp_vtf_remove_keyvalues_data_resource(nint handle);

	[LibraryImport(Name)]
	public static partial void vtfpp_vtf_set_hotspot_data_resource(nint handle, nint hot);

	[LibraryImport(Name)]
	public static partial void vtfpp_vtf_remove_hotspot_data_resource(nint handle);

	[LibraryImport(Name)]
	public static partial short vtfpp_vtf_get_compression_level(nint handle);

	[LibraryImport(Name)]
	public static partial void vtfpp_vtf_set_compression_level(nint handle, short compressionLevel);

	[LibraryImport(Name)]
	public static partial CompressionMethod vtfpp_vtf_get_compression_method(nint handle);

	[LibraryImport(Name)]
	public static partial void vtfpp_vtf_set_compression_method(nint handle, CompressionMethod compressionMethod);

	[LibraryImport(Name)]
	public static partial int vtfpp_vtf_has_image_data(nint handle);

	[LibraryImport(Name)]
	public static unsafe partial byte* vtfpp_vtf_get_image_data_raw(nint handle, ref ulong imageLen, byte mip, ushort frame, byte face, ushort slice);

	[LibraryImport(Name)]
	public static partial sourcepp.DLL.Buffer vtfpp_vtf_get_image_data_as(nint handle, ImageFormat format, byte mip, ushort frame, byte face, ushort slice);

	[LibraryImport(Name)]
	public static partial sourcepp.DLL.Buffer vtfpp_vtf_get_image_data_as_rgba8888(nint handle, byte mip, ushort frame, byte face, ushort slice);

	[LibraryImport(Name)]
	public static partial int vtfpp_vtf_set_image_from_mem(nint handle, ReadOnlySpan<byte> imageData, ulong imageLen, ImageFormat format, ushort width, ushort height, ImageConversion.ResizeFilter filter, byte mip, ushort frame, byte face, ushort slice, float quality);

	[LibraryImport(Name, StringMarshalling = StringMarshalling.Utf8)]
	public static partial int vtfpp_vtf_set_image_from_file(nint handle, string imagePath, ImageConversion.ResizeFilter filter, byte mip, ushort frame, byte face, ushort slice, float quality, ImageConversion.ResizeBounds resizeBounds);

	[LibraryImport(Name, StringMarshalling = StringMarshalling.Utf8)]
	public static partial int vtfpp_vtf_save_image_to_file(nint handle, string imagePath, byte mip, ushort frame, byte face, ushort slice, ImageConversion.FileFormat fileFormat);

	[LibraryImport(Name)]
	public static partial int vtfpp_vtf_has_thumbnail_data(nint handle);

	[LibraryImport(Name)]
	public static unsafe partial byte* vtfpp_vtf_get_thumbnail_data_raw(nint handle, ref ulong imageLen);

	[LibraryImport(Name)]
	public static partial sourcepp.DLL.Buffer vtfpp_vtf_get_thumbnail_data_as(nint handle, ImageFormat format);

	[LibraryImport(Name)]
	public static partial sourcepp.DLL.Buffer vtfpp_vtf_get_thumbnail_data_as_rgba8888(nint handle);

	[LibraryImport(Name)]
	public static partial void vtfpp_vtf_set_thumbnail_from_mem(nint handle, ReadOnlySpan<byte> imageData, ulong imageLen, ImageFormat format, ushort width, ushort height, float quality);

	[LibraryImport(Name, StringMarshalling = StringMarshalling.Utf8)]
	public static partial void vtfpp_vtf_set_thumbnail_from_file(nint handle, string imagePath, float quality);

	[LibraryImport(Name)]
	public static partial void vtfpp_vtf_compute_thumbnail(nint handle, ImageConversion.ResizeFilter filter, float quality);

	[LibraryImport(Name)]
	public static partial void vtfpp_vtf_remove_thumbnail(nint handle);

	[LibraryImport(Name, StringMarshalling = StringMarshalling.Utf8)]
	public static partial int vtfpp_vtf_save_thumbnail_to_file(nint handle, string imagePath, ImageConversion.FileFormat fileFormat);

	[LibraryImport(Name)]
	public static partial int vtfpp_vtf_has_fallback_data(nint handle);

	[LibraryImport(Name)]
	public static unsafe partial byte* vtfpp_vtf_get_fallback_data_raw(nint handle, ref ulong imageLen, byte mip, ushort frame, byte face);

	[LibraryImport(Name)]
	public static partial sourcepp.DLL.Buffer vtfpp_vtf_get_fallback_data_as(nint handle, ImageFormat format, byte mip, ushort frame, byte face);

	[LibraryImport(Name)]
	public static partial sourcepp.DLL.Buffer vtfpp_vtf_get_fallback_data_as_rgba8888(nint handle, byte mip, ushort frame, byte face);

	[LibraryImport(Name)]
	public static partial void vtfpp_vtf_compute_fallback(nint handle, ImageConversion.ResizeFilter filter);

	[LibraryImport(Name)]
	public static partial void vtfpp_vtf_remove_fallback(nint handle);

	[LibraryImport(Name, StringMarshalling = StringMarshalling.Utf8)]
	public static partial int vtfpp_vtf_save_fallback_to_file(nint handle, string imagePath, byte mip, ushort frame, byte face, ImageConversion.FileFormat fileFormat);

	[LibraryImport(Name)]
	public static partial byte vtfpp_vtf_get_console_mip_scale(nint handle);

	[LibraryImport(Name)]
	public static partial void vtfpp_vtf_set_console_mip_scale(nint handle, byte consoleMipScale);

	[LibraryImport(Name)]
	public static partial ulong vtfpp_vtf_estimate_bake_size(nint handle, ref int isExact);

	[LibraryImport(Name)]
	public static partial sourcepp.DLL.Buffer vtfpp_vtf_bake(nint handle);

	[LibraryImport(Name, StringMarshalling = StringMarshalling.Utf8)]
	public static partial int vtfpp_vtf_bake_to_file(nint handle, string vtfPath);
}
