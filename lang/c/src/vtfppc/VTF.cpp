// ReSharper disable CppDFALocalValueEscapesFunction
// ReSharper disable CppPassValueParameterByConstReference

#include <vtfppc/VTF.h>

#include <vtfpp/VTF.h>

#include <sourceppc/Helpers.h>

using namespace sourceppc;
using namespace vtfpp;

const uint32_t VTFPP_VTF_SIGNATURE = VTF_SIGNATURE;
const uint32_t VTFPP_XTF_SIGNATURE = XTF_SIGNATURE;
const uint32_t VTFPP_VTFX_SIGNATURE = VTFX_SIGNATURE;
const uint32_t VTFPP_VTF3_SIGNATURE = VTF3_SIGNATURE;

SOURCEPP_API vtfpp_resource_type_e vtfpp_resource_get_type(vtfpp_resource_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, VTFPP_RESOURCE_TYPE_UNKNOWN);

	return convert::cast(convert::handle<const Resource>(handle)->type);
}

SOURCEPP_API vtfpp_resource_flags_e vtfpp_resource_get_flags(vtfpp_resource_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, VTFPP_RESOURCE_FLAG_NONE);

	return convert::cast(convert::handle<const Resource>(handle)->flags);
}

SOURCEPP_API unsigned char* vtfpp_resource_get_data(vtfpp_resource_handle_t handle, size_t* dataLen) {
	SOURCEPP_EARLY_RETURN_VAL(handle, nullptr);
	SOURCEPP_EARLY_RETURN_VAL(dataLen, nullptr);

	const auto resourceData = convert::handle<const Resource>(handle)->data;
	*dataLen = resourceData.size();
	return reinterpret_cast<unsigned char*>(resourceData.data());
}

SOURCEPP_API sourcepp_buffer_t vtfpp_resource_get_data_as_palette(vtfpp_resource_handle_t handle, uint16_t frame) {
	SOURCEPP_EARLY_RETURN_VAL(handle, SOURCEPP_BUFFER_INVALID);

	return convert::toBuffer(convert::handle<const Resource>(handle)->getDataAsPalette(frame));
}

SOURCEPP_API vtfpp_sht_handle_t vtfpp_resource_get_data_as_particle_sheet(vtfpp_resource_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, nullptr);

	return new SHT{convert::handle<const Resource>(handle)->getDataAsParticleSheet()};
}

SOURCEPP_API uint32_t vtfpp_resource_get_data_as_crc(vtfpp_resource_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, 0);

	return convert::handle<const Resource>(handle)->getDataAsCRC();
}

SOURCEPP_API uint32_t vtfpp_resource_get_data_as_extended_flags(vtfpp_resource_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, 0);

	return convert::handle<const Resource>(handle)->getDataAsExtendedFlags();
}

SOURCEPP_API void vtfpp_resource_get_data_as_lod_control_info(vtfpp_resource_handle_t handle, uint8_t* u, uint8_t* v, uint8_t* u360, uint8_t* v360) {
	SOURCEPP_EARLY_RETURN(handle);

	const auto lodData = convert::handle<const Resource>(handle)->getDataAsLODControlInfo();
	if (u) {
		*u = std::get<0>(lodData);
	}
	if (v) {
		*v = std::get<1>(lodData);
	}
	if (u360) {
		*u360 = std::get<2>(lodData);
	}
	if (v360) {
		*v360 = std::get<3>(lodData);
	}
}

SOURCEPP_API sourcepp_string_t vtfpp_resource_get_data_as_keyvalues_data(vtfpp_resource_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, SOURCEPP_STRING_INVALID);

	return convert::toString(convert::handle<const Resource>(handle)->getDataAsKeyValuesData());
}

SOURCEPP_API vtfpp_hot_handle_t vtfpp_resource_get_data_as_hotspot_data(vtfpp_resource_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, nullptr);

	return new HOT{convert::handle<const Resource>(handle)->getDataAsHotspotData()};
}

SOURCEPP_API int16_t vtfpp_resource_get_data_as_aux_compression_level(vtfpp_resource_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, 0);

	return convert::handle<const Resource>(handle)->getDataAsAuxCompressionLevel();
}

SOURCEPP_API vtfpp_compression_method_e vtfpp_resource_get_data_as_aux_compression_method(vtfpp_resource_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, VTFPP_COMPRESSION_METHOD_ZSTD);

	return convert::cast(convert::handle<const Resource>(handle)->getDataAsAuxCompressionMethod());
}

SOURCEPP_API uint32_t vtfpp_resource_get_data_as_aux_compression_length(vtfpp_resource_handle_t handle, uint8_t mip, uint8_t mipCount, uint16_t frame, uint16_t frameCount, uint16_t face, uint16_t faceCount) {
	SOURCEPP_EARLY_RETURN_VAL(handle, 0);

	return convert::handle<const Resource>(handle)->getDataAsAuxCompressionLength(mip, mipCount, frame, frameCount, face, faceCount);
}

const vtfpp_image_format_e VTFPP_VTF_FORMAT_UNCHANGED = convert::cast(VTF::FORMAT_UNCHANGED);
const vtfpp_image_format_e VTFPP_VTF_FORMAT_DEFAULT   = convert::cast(VTF::FORMAT_DEFAULT);

SOURCEPP_API vtfpp_vtf_handle_t vtfpp_vtf_create_empty() {
	return new VTF{};
}

SOURCEPP_API vtfpp_vtf_handle_t vtfpp_vtf_open_from_mem(const unsigned char* vtfData, size_t vtfLen, int parseHeaderOnly) {
	SOURCEPP_EARLY_RETURN_VAL(vtfData, nullptr);
	SOURCEPP_EARLY_RETURN_VAL(vtfLen, nullptr);

	return new VTF{{reinterpret_cast<const std::byte*>(vtfData), vtfLen}, static_cast<bool>(parseHeaderOnly)};
}

SOURCEPP_API vtfpp_vtf_handle_t vtfpp_vtf_open_from_file(const char* vtfPath, int parseHeaderOnly) {
	SOURCEPP_EARLY_RETURN_VAL(vtfPath, nullptr);

	return new VTF{vtfPath, static_cast<bool>(parseHeaderOnly)};
}

SOURCEPP_API void vtfpp_vtf_free(vtfpp_vtf_handle_t* handle) {
	SOURCEPP_EARLY_RETURN(handle);

	delete convert::handle<VTF>(*handle);
	*handle = nullptr;
}

SOURCEPP_API int vtfpp_vtf_is_valid(vtfpp_vtf_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, false);

	return convert::handle<VTF>(handle)->operator bool();
}

SOURCEPP_API int vtfpp_vtf_create_from_mem_and_bake(const unsigned char* imageData, size_t imageLen, vtfpp_image_format_e format, uint16_t width, uint16_t height, const char* vtfPath, vtfpp_vtf_creation_options_t options) {
	SOURCEPP_EARLY_RETURN_VAL(imageData, false);
	SOURCEPP_EARLY_RETURN_VAL(imageLen, false);
	SOURCEPP_EARLY_RETURN_VAL(vtfPath, false);

	return VTF::create({reinterpret_cast<const std::byte*>(imageData), imageLen}, convert::cast(format), width, height, vtfPath, convert::cast(options));
}

SOURCEPP_API int vtfpp_vtf_create_blank_and_bake(vtfpp_image_format_e format, uint16_t width, uint16_t height, const char* vtfPath, vtfpp_vtf_creation_options_t options) {
	SOURCEPP_EARLY_RETURN_VAL(vtfPath, false);

	return VTF::create(convert::cast(format), width, height, vtfPath, convert::cast(options));
}

SOURCEPP_API vtfpp_vtf_handle_t vtfpp_vtf_create_from_mem(const unsigned char* imageData, size_t imageLen, vtfpp_image_format_e format, uint16_t width, uint16_t height, vtfpp_vtf_creation_options_t options) {
	SOURCEPP_EARLY_RETURN_VAL(imageData, nullptr);
	SOURCEPP_EARLY_RETURN_VAL(imageLen, nullptr);

	return new VTF{VTF::create({reinterpret_cast<const std::byte*>(imageData), imageLen}, convert::cast(format), width, height, convert::cast(options))};
}

SOURCEPP_API vtfpp_vtf_handle_t vtfpp_vtf_create_blank(vtfpp_image_format_e format, uint16_t width, uint16_t height, vtfpp_vtf_creation_options_t options) {
	return new VTF{VTF::create(convert::cast(format), width, height, convert::cast(options))};
}

SOURCEPP_API int vtfpp_vtf_create_from_file_and_bake(const char* imagePath, const char* vtfPath, vtfpp_vtf_creation_options_t options) {
	SOURCEPP_EARLY_RETURN_VAL(imagePath, false);
	SOURCEPP_EARLY_RETURN_VAL(vtfPath, false);

	return VTF::create(imagePath, vtfPath, convert::cast(options));
}

SOURCEPP_API vtfpp_vtf_handle_t vtfpp_vtf_create_from_file(const char* imagePath, vtfpp_vtf_creation_options_t options) {
	SOURCEPP_EARLY_RETURN_VAL(imagePath, nullptr);

	return new VTF{VTF::create(imagePath, convert::cast(options))};
}

SOURCEPP_API vtfpp_vtf_platform_e vtfpp_vtf_get_platform(vtfpp_vtf_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, VTFPP_VTF_PLATFORM_UNKNOWN);

	return convert::cast(convert::handle<VTF>(handle)->getPlatform());
}

SOURCEPP_API void vtfpp_vtf_set_platform(vtfpp_vtf_handle_t handle, vtfpp_vtf_platform_e platform) {
	SOURCEPP_EARLY_RETURN(handle);

	convert::handle<VTF>(handle)->setPlatform(convert::cast(platform));
}

SOURCEPP_API uint32_t vtfpp_vtf_get_version(vtfpp_vtf_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, 0);

	return convert::handle<VTF>(handle)->getVersion();
}

SOURCEPP_API void vtfpp_vtf_set_version(vtfpp_vtf_handle_t handle, uint32_t version) {
	SOURCEPP_EARLY_RETURN(handle);

	return convert::handle<VTF>(handle)->setVersion(version);
}

SOURCEPP_API vtfpp_image_conversion_resize_method_e vtfpp_vtf_get_image_width_resize_method(vtfpp_vtf_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, VTFPP_IMAGE_CONVERSION_RESIZE_METHOD_NONE);

	return convert::cast(convert::handle<VTF>(handle)->getImageWidthResizeMethod());
}

SOURCEPP_API vtfpp_image_conversion_resize_method_e vtfpp_vtf_get_image_height_resize_method(vtfpp_vtf_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, VTFPP_IMAGE_CONVERSION_RESIZE_METHOD_NONE);

	return convert::cast(convert::handle<VTF>(handle)->getImageHeightResizeMethod());
}

SOURCEPP_API void vtfpp_vtf_set_image_resize_methods(vtfpp_vtf_handle_t handle, vtfpp_image_conversion_resize_method_e imageWidthResizeMethod, vtfpp_image_conversion_resize_method_e imageHeightResizeMethod) {
	SOURCEPP_EARLY_RETURN(handle);

	return convert::handle<VTF>(handle)->setImageResizeMethods(convert::cast(imageWidthResizeMethod), convert::cast(imageHeightResizeMethod));
}

SOURCEPP_API void vtfpp_vtf_set_image_width_resize_method(vtfpp_vtf_handle_t handle, vtfpp_image_conversion_resize_method_e imageWidthResizeMethod) {
	SOURCEPP_EARLY_RETURN(handle);

	return convert::handle<VTF>(handle)->setImageWidthResizeMethod(convert::cast(imageWidthResizeMethod));
}

SOURCEPP_API void vtfpp_vtf_set_image_height_resize_method(vtfpp_vtf_handle_t handle, vtfpp_image_conversion_resize_method_e imageHeightResizeMethod) {
	SOURCEPP_EARLY_RETURN(handle);

	return convert::handle<VTF>(handle)->setImageHeightResizeMethod(convert::cast(imageHeightResizeMethod));
}

SOURCEPP_API uint16_t vtfpp_vtf_get_width(vtfpp_vtf_handle_t handle, uint8_t mip) {
	SOURCEPP_EARLY_RETURN_VAL(handle, 0);

	return convert::handle<VTF>(handle)->getWidth(mip);
}

SOURCEPP_API uint16_t vtfpp_vtf_get_padded_width(vtfpp_vtf_handle_t handle, uint8_t mip) {
	SOURCEPP_EARLY_RETURN_VAL(handle, 0);

	return convert::handle<VTF>(handle)->getPaddedWidth(mip);
}

SOURCEPP_API uint16_t vtfpp_vtf_get_height(vtfpp_vtf_handle_t handle, uint8_t mip) {
	SOURCEPP_EARLY_RETURN_VAL(handle, 0);

	return convert::handle<VTF>(handle)->getHeight(mip);
}

SOURCEPP_API uint16_t vtfpp_vtf_get_padded_height(vtfpp_vtf_handle_t handle, uint8_t mip) {
	SOURCEPP_EARLY_RETURN_VAL(handle, 0);

	return convert::handle<VTF>(handle)->getPaddedHeight(mip);
}

SOURCEPP_API void vtfpp_vtf_set_size(vtfpp_vtf_handle_t handle, uint16_t width, uint16_t height, vtfpp_image_conversion_resize_filter_e filter) {
	SOURCEPP_EARLY_RETURN(handle);

	convert::handle<VTF>(handle)->setSize(width, height, convert::cast(filter));
}

SOURCEPP_API uint32_t vtfpp_vtf_get_flags(vtfpp_vtf_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, 0);

	return convert::handle<VTF>(handle)->getFlags();
}

SOURCEPP_API void vtfpp_vtf_set_flags(vtfpp_vtf_handle_t handle, uint32_t flags) {
	SOURCEPP_EARLY_RETURN(handle);

	convert::handle<VTF>(handle)->setFlags(flags);
}

SOURCEPP_API void vtfpp_vtf_add_flags(vtfpp_vtf_handle_t handle, uint32_t flags) {
	SOURCEPP_EARLY_RETURN(handle);

	convert::handle<VTF>(handle)->addFlags(flags);
}

SOURCEPP_API void vtfpp_vtf_remove_flags(vtfpp_vtf_handle_t handle, uint32_t flags) {
	SOURCEPP_EARLY_RETURN(handle);

	convert::handle<VTF>(handle)->removeFlags(flags);
}

SOURCEPP_API int vtfpp_vtf_is_srgb(vtfpp_vtf_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, false);

	return convert::handle<VTF>(handle)->isSRGB();
}

SOURCEPP_API void vtfpp_vtf_set_srgb(vtfpp_vtf_handle_t handle, int srgb) {
	SOURCEPP_EARLY_RETURN(handle);

	convert::handle<VTF>(handle)->setSRGB(srgb);
}

SOURCEPP_API void vtfpp_vtf_compute_transparency_flags(vtfpp_vtf_handle_t handle) {
	SOURCEPP_EARLY_RETURN(handle);

	convert::handle<VTF>(handle)->computeTransparencyFlags();
}

SOURCEPP_API vtfpp_image_format_e vtfpp_vtf_get_default_compressed_format(vtfpp_image_format_e inputFormat, uint32_t version, int isCubeMap) {
	return convert::cast(VTF::getDefaultCompressedFormat(convert::cast(inputFormat), version, isCubeMap));
}

SOURCEPP_API vtfpp_image_format_e vtfpp_vtf_get_format(vtfpp_vtf_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, VTFPP_IMAGE_FORMAT_EMPTY);

	return convert::cast(convert::handle<VTF>(handle)->getFormat());
}

SOURCEPP_API void vtfpp_vtf_set_format(vtfpp_vtf_handle_t handle, vtfpp_image_format_e format, vtfpp_image_conversion_resize_filter_e filter, float quality) {
	SOURCEPP_EARLY_RETURN(handle);

	convert::handle<VTF>(handle)->setFormat(convert::cast(format), convert::cast(filter), quality);
}

SOURCEPP_API uint8_t vtfpp_vtf_get_mip_count(vtfpp_vtf_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, 0);

	return convert::handle<VTF>(handle)->getMipCount();
}

SOURCEPP_API int vtfpp_vtf_set_mip_count(vtfpp_vtf_handle_t handle, uint8_t mipCount) {
	SOURCEPP_EARLY_RETURN_VAL(handle, false);

	return convert::handle<VTF>(handle)->setMipCount(mipCount);
}

SOURCEPP_API int vtfpp_vtf_set_recommended_mip_count(vtfpp_vtf_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, false);

	return convert::handle<VTF>(handle)->setRecommendedMipCount();
}

SOURCEPP_API void vtfpp_vtf_compute_mips(vtfpp_vtf_handle_t handle, vtfpp_image_conversion_resize_filter_e filter) {
	SOURCEPP_EARLY_RETURN(handle);

	return convert::handle<VTF>(handle)->computeMips(convert::cast(filter));
}

SOURCEPP_API uint16_t vtfpp_vtf_get_frame_count(vtfpp_vtf_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, 0);

	return convert::handle<VTF>(handle)->getFrameCount();
}

SOURCEPP_API int vtfpp_vtf_set_frame_count(vtfpp_vtf_handle_t handle, uint16_t frameCount) {
	SOURCEPP_EARLY_RETURN_VAL(handle, false);

	return convert::handle<VTF>(handle)->setFrameCount(frameCount);
}

SOURCEPP_API uint8_t vtfpp_vtf_get_face_count(vtfpp_vtf_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, 0);

	return convert::handle<VTF>(handle)->getFaceCount();
}

SOURCEPP_API int vtfpp_vtf_set_face_count(vtfpp_vtf_handle_t handle, int isCubeMap) {
	SOURCEPP_EARLY_RETURN_VAL(handle, false);

	return convert::handle<VTF>(handle)->setFaceCount(isCubeMap);
}

SOURCEPP_API uint16_t vtfpp_vtf_get_depth(vtfpp_vtf_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, 0);

	return convert::handle<VTF>(handle)->getDepth();
}

SOURCEPP_API int vtfpp_vtf_set_depth(vtfpp_vtf_handle_t handle, uint16_t depth) {
	SOURCEPP_EARLY_RETURN_VAL(handle, false);

	return convert::handle<VTF>(handle)->setDepth(depth);
}

SOURCEPP_API int vtfpp_vtf_set_frame_face_and_depth(vtfpp_vtf_handle_t handle, uint16_t frameCount, int isCubeMap, uint16_t depth) {
	SOURCEPP_EARLY_RETURN_VAL(handle, false);

	return convert::handle<VTF>(handle)->setFrameFaceAndDepth(frameCount, isCubeMap, depth);
}

SOURCEPP_API uint16_t vtfpp_vtf_get_start_frame(vtfpp_vtf_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, 0);

	return convert::handle<VTF>(handle)->getStartFrame();
}

SOURCEPP_API void vtfpp_vtf_set_start_frame(vtfpp_vtf_handle_t handle, uint16_t startFrame) {
	SOURCEPP_EARLY_RETURN(handle);

	return convert::handle<VTF>(handle)->setStartFrame(startFrame);
}

SOURCEPP_API void vtfpp_vtf_get_reflectivity(vtfpp_vtf_handle_t handle, float* r, float* g, float* b) {
	SOURCEPP_EARLY_RETURN(handle);
	SOURCEPP_EARLY_RETURN(r);
	SOURCEPP_EARLY_RETURN(g);
	SOURCEPP_EARLY_RETURN(b);

	const auto reflectivity = convert::handle<VTF>(handle)->getReflectivity();
	*r = reflectivity[0];
	*g = reflectivity[1];
	*b = reflectivity[2];
}

SOURCEPP_API void vtfpp_vtf_set_reflectivity(vtfpp_vtf_handle_t handle, float r, float g, float b) {
	SOURCEPP_EARLY_RETURN(handle);

	convert::handle<VTF>(handle)->setReflectivity({r, g, b});
}

SOURCEPP_API void vtfpp_vtf_compute_reflectivity(vtfpp_vtf_handle_t handle) {
	SOURCEPP_EARLY_RETURN(handle);

	convert::handle<VTF>(handle)->computeReflectivity();
}

SOURCEPP_API float vtfpp_vtf_get_bumpmap_scale(vtfpp_vtf_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, 0.f);

	return convert::handle<VTF>(handle)->getBumpMapScale();
}

SOURCEPP_API void vtfpp_vtf_set_bumpmap_scale(vtfpp_vtf_handle_t handle, float bumpMapScale) {
	SOURCEPP_EARLY_RETURN(handle);

	convert::handle<VTF>(handle)->setBumpMapScale(bumpMapScale);
}

SOURCEPP_API vtfpp_image_format_e vtfpp_vtf_get_thumbnail_format(vtfpp_vtf_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, VTFPP_IMAGE_FORMAT_EMPTY);

	return convert::cast(convert::handle<VTF>(handle)->getThumbnailFormat());
}

SOURCEPP_API uint8_t vtfpp_vtf_get_thumbnail_width(vtfpp_vtf_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, 0);

	return convert::handle<VTF>(handle)->getThumbnailWidth();
}

SOURCEPP_API uint8_t vtfpp_vtf_get_thumbnail_height(vtfpp_vtf_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, 0);

	return convert::handle<VTF>(handle)->getThumbnailHeight();
}

SOURCEPP_API uint8_t vtfpp_vtf_get_fallback_width(vtfpp_vtf_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, 0);

	return convert::handle<VTF>(handle)->getFallbackWidth();
}

SOURCEPP_API uint8_t vtfpp_vtf_get_fallback_height(vtfpp_vtf_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, 0);

	return convert::handle<VTF>(handle)->getFallbackHeight();
}

SOURCEPP_API uint8_t vtfpp_vtf_get_fallback_mip_count(vtfpp_vtf_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, 0);

	return convert::handle<VTF>(handle)->getFallbackMipCount();
}

SOURCEPP_API uint32_t vtfpp_vtf_get_resources_count(vtfpp_vtf_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, 0);

	return convert::handle<VTF>(handle)->getResources().size();
}

SOURCEPP_API vtfpp_resource_handle_t vtfpp_vtf_get_resource_at_index(vtfpp_vtf_handle_t handle, uint32_t index) {
	SOURCEPP_EARLY_RETURN_VAL(handle, nullptr);

	return &convert::handle<VTF>(handle)->getResources().at(index);
}

SOURCEPP_API vtfpp_resource_handle_t vtfpp_vtf_get_resource_with_type(vtfpp_vtf_handle_t handle, vtfpp_resource_type_e type) {
	SOURCEPP_EARLY_RETURN_VAL(handle, nullptr);

	return convert::handle<VTF>(handle)->getResource(convert::cast(type));
}

SOURCEPP_API sourcepp_buffer_t vtfpp_vtf_get_palette_resource_frame(vtfpp_vtf_handle_t handle, uint16_t frame) {
	SOURCEPP_EARLY_RETURN_VAL(handle, SOURCEPP_BUFFER_INVALID);

	return convert::toBuffer(convert::handle<VTF>(handle)->getPaletteResourceFrame(frame));
}

SOURCEPP_API sourcepp_buffer_t vtfpp_vtf_get_particle_sheet_frame_data_raw(vtfpp_vtf_handle_t handle, uint16_t* width, uint16_t* height, uint32_t shtSequenceID, uint32_t shtFrame, uint8_t shtBounds, uint8_t mip, uint16_t frame, uint8_t face, uint16_t slice) {
	SOURCEPP_EARLY_RETURN_VAL(handle, SOURCEPP_BUFFER_INVALID);
	SOURCEPP_EARLY_RETURN_VAL(width, SOURCEPP_BUFFER_INVALID);
	SOURCEPP_EARLY_RETURN_VAL(height, SOURCEPP_BUFFER_INVALID);

	return convert::toBuffer(convert::handle<VTF>(handle)->getParticleSheetFrameDataRaw(*width, *height, shtSequenceID, shtFrame, shtBounds, mip, frame, face, slice));
}

SOURCEPP_API sourcepp_buffer_t vtfpp_vtf_get_particle_sheet_frame_data_as(vtfpp_vtf_handle_t handle, vtfpp_image_format_e format, uint16_t* width, uint16_t* height, uint32_t shtSequenceID, uint32_t shtFrame, uint8_t shtBounds, uint8_t mip, uint16_t frame, uint8_t face, uint16_t slice) {
	SOURCEPP_EARLY_RETURN_VAL(handle, SOURCEPP_BUFFER_INVALID);
	SOURCEPP_EARLY_RETURN_VAL(width, SOURCEPP_BUFFER_INVALID);
	SOURCEPP_EARLY_RETURN_VAL(height, SOURCEPP_BUFFER_INVALID);

	return convert::toBuffer(convert::handle<VTF>(handle)->getParticleSheetFrameDataAs(convert::cast(format), *width, *height, shtSequenceID, shtFrame, shtBounds, mip, frame, face, slice));
}

SOURCEPP_API sourcepp_buffer_t vtfpp_vtf_get_particle_sheet_frame_data_as_rgba8888(vtfpp_vtf_handle_t handle, uint16_t* width, uint16_t* height, uint32_t shtSequenceID, uint32_t shtFrame, uint8_t shtBounds, uint8_t mip, uint16_t frame, uint8_t face, uint16_t slice) {
	SOURCEPP_EARLY_RETURN_VAL(handle, SOURCEPP_BUFFER_INVALID);
	SOURCEPP_EARLY_RETURN_VAL(width, SOURCEPP_BUFFER_INVALID);
	SOURCEPP_EARLY_RETURN_VAL(height, SOURCEPP_BUFFER_INVALID);

	return convert::toBuffer(convert::handle<VTF>(handle)->getParticleSheetFrameDataAsRGBA8888(*width, *height, shtSequenceID, shtFrame, shtBounds, mip, frame, face, slice));
}

SOURCEPP_API void vtfpp_vtf_set_particle_sheet_resource(vtfpp_vtf_handle_t handle, vtfpp_sht_handle_t sht) {
	SOURCEPP_EARLY_RETURN(handle);
	SOURCEPP_EARLY_RETURN(sht);

	convert::handle<VTF>(handle)->setParticleSheetResource(*convert::handle<SHT>(sht));
}

SOURCEPP_API void vtfpp_vtf_remove_particle_sheet_resource(vtfpp_vtf_handle_t handle) {
	SOURCEPP_EARLY_RETURN(handle);

	convert::handle<VTF>(handle)->removeParticleSheetResource();
}

SOURCEPP_API void vtfpp_vtf_set_crc_resource(vtfpp_vtf_handle_t handle, uint32_t value) {
	SOURCEPP_EARLY_RETURN(handle);

	convert::handle<VTF>(handle)->setCRCResource(value);
}

SOURCEPP_API void vtfpp_vtf_remove_crc_resource(vtfpp_vtf_handle_t handle) {
	SOURCEPP_EARLY_RETURN(handle);

	convert::handle<VTF>(handle)->removeCRCResource();
}

SOURCEPP_API void vtfpp_vtf_set_lod_resource(vtfpp_vtf_handle_t handle, uint8_t u, uint8_t v, uint8_t u360, uint8_t v360) {
	SOURCEPP_EARLY_RETURN(handle);

	convert::handle<VTF>(handle)->setLODResource(u, v, u360, v360);
}

SOURCEPP_API void vtfpp_vtf_remove_lod_resource(vtfpp_vtf_handle_t handle) {
	SOURCEPP_EARLY_RETURN(handle);

	convert::handle<VTF>(handle)->removeLODResource();
}

SOURCEPP_API void vtfpp_vtf_set_extended_flags_resource(vtfpp_vtf_handle_t handle, uint32_t value) {
	SOURCEPP_EARLY_RETURN(handle);

	convert::handle<VTF>(handle)->setExtendedFlagsResource(value);
}

SOURCEPP_API void vtfpp_vtf_remove_extended_flags_resource(vtfpp_vtf_handle_t handle) {
	SOURCEPP_EARLY_RETURN(handle);

	convert::handle<VTF>(handle)->removeExtendedFlagsResource();
}

SOURCEPP_API void vtfpp_vtf_set_keyvalues_data_resource(vtfpp_vtf_handle_t handle, const char* value) {
	SOURCEPP_EARLY_RETURN(handle);
	SOURCEPP_EARLY_RETURN(value);

	convert::handle<VTF>(handle)->setKeyValuesDataResource(value);
}

SOURCEPP_API void vtfpp_vtf_remove_keyvalues_data_resource(vtfpp_vtf_handle_t handle) {
	SOURCEPP_EARLY_RETURN(handle);

	convert::handle<VTF>(handle)->removeKeyValuesDataResource();
}

SOURCEPP_API void vtfpp_vtf_set_hotspot_data_resource(vtfpp_vtf_handle_t handle, vtfpp_hot_handle_t hot) {
	SOURCEPP_EARLY_RETURN(handle);
	SOURCEPP_EARLY_RETURN(hot);

	convert::handle<VTF>(handle)->setHotspotDataResource(*convert::handle<HOT>(hot));
}

SOURCEPP_API void vtfpp_vtf_remove_hotspot_data_resource(vtfpp_vtf_handle_t handle) {
	SOURCEPP_EARLY_RETURN(handle);

	convert::handle<VTF>(handle)->removeHotspotDataResource();
}

SOURCEPP_API int16_t vtfpp_vtf_get_compression_level(vtfpp_vtf_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, 0);

	return convert::handle<VTF>(handle)->getCompressionLevel();
}

SOURCEPP_API void vtfpp_vtf_set_compression_level(vtfpp_vtf_handle_t handle, int16_t compressionLevel) {
	SOURCEPP_EARLY_RETURN(handle);

	convert::handle<VTF>(handle)->setCompressionLevel(compressionLevel);
}

SOURCEPP_API vtfpp_compression_method_e vtfpp_vtf_get_compression_method(vtfpp_vtf_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, VTFPP_COMPRESSION_METHOD_ZSTD);

	return convert::cast(convert::handle<VTF>(handle)->getCompressionMethod());
}

SOURCEPP_API void vtfpp_vtf_set_compression_method(vtfpp_vtf_handle_t handle, vtfpp_compression_method_e compressionMethod) {
	SOURCEPP_EARLY_RETURN(handle);

	convert::handle<VTF>(handle)->setCompressionMethod(convert::cast(compressionMethod));
}

SOURCEPP_API int vtfpp_vtf_has_image_data(vtfpp_vtf_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, false);

	return convert::handle<VTF>(handle)->hasImageData();
}

SOURCEPP_API const unsigned char* vtfpp_vtf_get_image_data_raw(vtfpp_vtf_handle_t handle, size_t* imageLen, uint8_t mip, uint16_t frame, uint8_t face, uint16_t slice) {
	SOURCEPP_EARLY_RETURN_VAL(handle, nullptr);
	SOURCEPP_EARLY_RETURN_VAL(imageLen, nullptr);

	const auto imageData = convert::handle<VTF>(handle)->getImageDataRaw(mip, frame, face, slice);
	*imageLen = imageData.size();
	return reinterpret_cast<const unsigned char*>(imageData.data());
}

SOURCEPP_API sourcepp_buffer_t vtfpp_vtf_get_image_data_as(vtfpp_vtf_handle_t handle, vtfpp_image_format_e format, uint8_t mip, uint16_t frame, uint8_t face, uint16_t slice) {
	SOURCEPP_EARLY_RETURN_VAL(handle, SOURCEPP_BUFFER_INVALID);

	return convert::toBuffer(convert::handle<VTF>(handle)->getImageDataAs(convert::cast(format), mip, frame, face, slice));
}

SOURCEPP_API sourcepp_buffer_t vtfpp_vtf_get_image_data_as_rgba8888(vtfpp_vtf_handle_t handle, uint8_t mip, uint16_t frame, uint8_t face, uint16_t slice) {
	SOURCEPP_EARLY_RETURN_VAL(handle, SOURCEPP_BUFFER_INVALID);

	return convert::toBuffer(convert::handle<VTF>(handle)->getImageDataAsRGBA8888(mip, frame, face, slice));
}

SOURCEPP_API int vtfpp_vtf_set_image_from_mem(vtfpp_vtf_handle_t handle, const unsigned char* imageData, size_t imageLen, vtfpp_image_format_e format, uint16_t width, uint16_t height, vtfpp_image_conversion_resize_filter_e filter, uint8_t mip, uint16_t frame, uint8_t face, uint16_t slice, float quality) {
	SOURCEPP_EARLY_RETURN_VAL(handle, false);
	SOURCEPP_EARLY_RETURN_VAL(imageData, false);
	SOURCEPP_EARLY_RETURN_VAL(imageLen, false);

	return convert::handle<VTF>(handle)->setImage({reinterpret_cast<const std::byte*>(imageData), imageLen}, convert::cast(format), width, height, convert::cast(filter), mip, frame, face, slice, quality);
}

SOURCEPP_API int vtfpp_vtf_set_image_from_file(vtfpp_vtf_handle_t handle, const char* imagePath, vtfpp_image_conversion_resize_filter_e filter, uint8_t mip, uint16_t frame, uint8_t face, uint16_t slice, float quality) {
	SOURCEPP_EARLY_RETURN_VAL(handle, false);
	SOURCEPP_EARLY_RETURN_VAL(imagePath, false);

	return convert::handle<VTF>(handle)->setImage(imagePath, convert::cast(filter), mip, frame, face, slice, quality);
}

SOURCEPP_API int vtfpp_vtf_save_image_to_file(vtfpp_vtf_handle_t handle, const char* imagePath, uint8_t mip, uint16_t frame, uint8_t face, uint16_t slice, vtfpp_image_conversion_file_format_e fileFormat) {
	SOURCEPP_EARLY_RETURN_VAL(handle, false);
	SOURCEPP_EARLY_RETURN_VAL(imagePath, false);

	return convert::handle<VTF>(handle)->saveImageToFile(imagePath, mip, frame, face, slice, convert::cast(fileFormat));
}

SOURCEPP_API int vtfpp_vtf_has_thumbnail_data(vtfpp_vtf_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, false);

	return convert::handle<VTF>(handle)->hasThumbnailData();
}

SOURCEPP_API const unsigned char* vtfpp_vtf_get_thumbnail_data_raw(vtfpp_vtf_handle_t handle, size_t* imageLen) {
	SOURCEPP_EARLY_RETURN_VAL(handle, nullptr);
	SOURCEPP_EARLY_RETURN_VAL(imageLen, nullptr);

	const auto thumbnailData = convert::handle<VTF>(handle)->getThumbnailDataRaw();
	*imageLen = thumbnailData.size();
	return reinterpret_cast<const unsigned char*>(thumbnailData.data());
}

SOURCEPP_API sourcepp_buffer_t vtfpp_vtf_get_thumbnail_data_as(vtfpp_vtf_handle_t handle, vtfpp_image_format_e format) {
	SOURCEPP_EARLY_RETURN_VAL(handle, SOURCEPP_BUFFER_INVALID);

	return convert::toBuffer(convert::handle<VTF>(handle)->getThumbnailDataAs(convert::cast(format)));
}

SOURCEPP_API sourcepp_buffer_t vtfpp_vtf_get_thumbnail_data_as_rgba8888(vtfpp_vtf_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, SOURCEPP_BUFFER_INVALID);

	return convert::toBuffer(convert::handle<VTF>(handle)->getThumbnailDataAsRGBA8888());
}

SOURCEPP_API void vtfpp_vtf_set_thumbnail_from_mem(vtfpp_vtf_handle_t handle, const unsigned char* imageData, size_t imageLen, vtfpp_image_format_e format, uint16_t width, uint16_t height, float quality) {
	SOURCEPP_EARLY_RETURN(handle);
	SOURCEPP_EARLY_RETURN(imageData);
	SOURCEPP_EARLY_RETURN(imageLen);

	convert::handle<VTF>(handle)->setThumbnail({reinterpret_cast<const std::byte*>(imageData), imageLen}, convert::cast(format), width, height, quality);
}

SOURCEPP_API void vtfpp_vtf_set_thumbnail_from_file(vtfpp_vtf_handle_t handle, const char* imagePath, float quality) {
	SOURCEPP_EARLY_RETURN(handle);
	SOURCEPP_EARLY_RETURN(imagePath);

	convert::handle<VTF>(handle)->setThumbnail(imagePath, quality);
}

SOURCEPP_API void vtfpp_vtf_compute_thumbnail(vtfpp_vtf_handle_t handle, vtfpp_image_conversion_resize_filter_e filter, float quality) {
	SOURCEPP_EARLY_RETURN(handle);

	convert::handle<VTF>(handle)->computeThumbnail(convert::cast(filter), quality);
}

SOURCEPP_API void vtfpp_vtf_remove_thumbnail(vtfpp_vtf_handle_t handle) {
	SOURCEPP_EARLY_RETURN(handle);

	convert::handle<VTF>(handle)->removeThumbnail();
}

SOURCEPP_API int vtfpp_vtf_save_thumbnail_to_file(vtfpp_vtf_handle_t handle, const char* imagePath, vtfpp_image_conversion_file_format_e fileFormat) {
	SOURCEPP_EARLY_RETURN_VAL(handle, false);
	SOURCEPP_EARLY_RETURN_VAL(imagePath, false);

	return convert::handle<VTF>(handle)->saveThumbnailToFile(imagePath, convert::cast(fileFormat));
}

SOURCEPP_API int vtfpp_vtf_has_fallback_data(vtfpp_vtf_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, false);

	return convert::handle<VTF>(handle)->hasFallbackData();
}

SOURCEPP_API const unsigned char* vtfpp_vtf_get_fallback_data_raw(vtfpp_vtf_handle_t handle, size_t* imageLen, uint8_t mip, uint16_t frame, uint8_t face) {
	SOURCEPP_EARLY_RETURN_VAL(handle, nullptr);
	SOURCEPP_EARLY_RETURN_VAL(imageLen, nullptr);

	const auto fallbackData = convert::handle<VTF>(handle)->getFallbackDataRaw(mip, frame, face);
	*imageLen = fallbackData.size();
	return reinterpret_cast<const unsigned char*>(fallbackData.data());
}

SOURCEPP_API sourcepp_buffer_t vtfpp_vtf_get_fallback_data_as(vtfpp_vtf_handle_t handle, vtfpp_image_format_e format, uint8_t mip, uint16_t frame, uint8_t face) {
	SOURCEPP_EARLY_RETURN_VAL(handle, SOURCEPP_BUFFER_INVALID);

	return convert::toBuffer(convert::handle<VTF>(handle)->getFallbackDataAs(convert::cast(format), mip, frame, face));
}

SOURCEPP_API sourcepp_buffer_t vtfpp_vtf_get_fallback_data_as_rgba8888(vtfpp_vtf_handle_t handle, uint8_t mip, uint16_t frame, uint8_t face) {
	SOURCEPP_EARLY_RETURN_VAL(handle, SOURCEPP_BUFFER_INVALID);

	return convert::toBuffer(convert::handle<VTF>(handle)->getFallbackDataAsRGBA8888(mip, frame, face));
}

SOURCEPP_API void vtfpp_vtf_compute_fallback(vtfpp_vtf_handle_t handle, vtfpp_image_conversion_resize_filter_e filter) {
	SOURCEPP_EARLY_RETURN(handle);

	convert::handle<VTF>(handle)->computeFallback(convert::cast(filter));
}

SOURCEPP_API void vtfpp_vtf_remove_fallback(vtfpp_vtf_handle_t handle) {
	SOURCEPP_EARLY_RETURN(handle);

	convert::handle<VTF>(handle)->removeFallback();
}

SOURCEPP_API int vtfpp_vtf_save_fallback_to_file(vtfpp_vtf_handle_t handle, const char* imagePath, uint8_t mip, uint16_t frame, uint8_t face, vtfpp_image_conversion_file_format_e fileFormat) {
	SOURCEPP_EARLY_RETURN_VAL(handle, false);
	SOURCEPP_EARLY_RETURN_VAL(imagePath, false);

	return convert::handle<VTF>(handle)->saveFallbackToFile(imagePath, mip, frame, face, convert::cast(fileFormat));
}

SOURCEPP_API uint8_t vtfpp_vtf_get_console_mip_scale(vtfpp_vtf_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, 0);

	return convert::handle<VTF>(handle)->getConsoleMipScale();
}

SOURCEPP_API void vtfpp_vtf_set_console_mip_scale(vtfpp_vtf_handle_t handle, uint8_t consoleMipScale) {
	SOURCEPP_EARLY_RETURN(handle);

	convert::handle<VTF>(handle)->setConsoleMipScale(consoleMipScale);
}

SOURCEPP_API uint64_t vtfpp_vtf_estimate_bake_size(vtfpp_vtf_handle_t handle, int* isExact) {
	SOURCEPP_EARLY_RETURN_VAL(handle, 0);

	bool exactBool;
	const auto estimate = convert::handle<VTF>(handle)->estimateBakeSize(exactBool);
	if (isExact) {
		*isExact = exactBool;
	}
	return estimate;
}

SOURCEPP_API sourcepp_buffer_t vtfpp_vtf_bake(vtfpp_vtf_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, SOURCEPP_BUFFER_INVALID);

	return convert::toBuffer(convert::handle<VTF>(handle)->bake());
}

SOURCEPP_API int vtfpp_vtf_bake_to_file(vtfpp_vtf_handle_t handle, const char* vtfPath) {
	SOURCEPP_EARLY_RETURN_VAL(handle, false);
	SOURCEPP_EARLY_RETURN_VAL(vtfPath, false);

	return convert::handle<VTF>(handle)->bake(vtfPath);
}
