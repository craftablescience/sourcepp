#include <vtfppc/VTF.h>

#include <vtfpp/VTF.h>

#include <sourceppc/Convert.hpp>
#include <sourceppc/Helpers.h>
#include <vtfppc/Convert.hpp>

using namespace vtfpp;

SOURCEPP_API vtfpp_resource_type_e vtfpp_resource_get_type(vtfpp_resource_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, VTFPP_RESOURCE_TYPE_UNKNOWN);

	return Convert::resourceType(Convert::resource(handle)->type);
}

SOURCEPP_API vtfpp_resource_flags_e vtfpp_resource_get_flags(vtfpp_resource_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, VTFPP_RESOURCE_FLAG_NONE);

	return Convert::resourceFlags(Convert::resource(handle)->flags);
}

SOURCEPP_API const unsigned char* vtfpp_resource_get_data(vtfpp_resource_handle_t handle, size_t* dataLen) {
	SOURCEPP_EARLY_RETURN_VAL(handle, nullptr);
	SOURCEPP_EARLY_RETURN_VAL(dataLen, nullptr);

	const auto resourceData = Convert::resource(handle)->data;
	*dataLen = resourceData.size();
	return reinterpret_cast<const unsigned char*>(resourceData.data());
}

// REQUIRES MANUAL FREE: vtfpp_sht_close
SOURCEPP_API vtfpp_sht_handle_t vtfpp_resource_get_data_as_particle_sheet(vtfpp_resource_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, nullptr);

	return new SHT{Convert::resource(handle)->getDataAsParticleSheet()};
}

SOURCEPP_API uint32_t vtfpp_resource_get_data_as_crc(vtfpp_resource_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, 0);

	return Convert::resource(handle)->getDataAsCRC();
}

SOURCEPP_API uint32_t vtfpp_resource_get_data_as_extended_flags(vtfpp_resource_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, 0);

	return Convert::resource(handle)->getDataAsExtendedFlags();
}

SOURCEPP_API void vtfpp_resource_get_data_as_lod(vtfpp_resource_handle_t handle, uint8_t* u, uint8_t* v, uint8_t* u360, uint8_t* v360) {
	SOURCEPP_EARLY_RETURN(handle);

	const auto lodData = Convert::resource(handle)->getDataAsLODControlInfo();
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

// REQUIRES MANUAL FREE: sourcepp_string_free
SOURCEPP_API sourcepp_string_t vtfpp_resource_get_data_as_keyvalues_data(vtfpp_resource_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, SOURCEPP_STRING_INVALID);

	return Convert::toString(Convert::resource(handle)->getDataAsKeyValuesData());
}

SOURCEPP_API int16_t vtfpp_resource_get_data_as_aux_compression_level(vtfpp_resource_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, 0);

	return Convert::resource(handle)->getDataAsAuxCompressionLevel();
}

SOURCEPP_API vtfpp_compression_method_e vtfpp_resource_get_data_as_aux_compression_method(vtfpp_resource_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, VTFPP_COMPRESSION_METHOD_ZSTD);

	return Convert::compressionMethod(Convert::resource(handle)->getDataAsAuxCompressionMethod());
}

SOURCEPP_API uint32_t vtfpp_resource_get_data_as_aux_compression_length(vtfpp_resource_handle_t handle, uint8_t mip, uint8_t mipCount, uint16_t frame, uint16_t frameCount, uint16_t face, uint16_t faceCount) {
	SOURCEPP_EARLY_RETURN_VAL(handle, 0);

	return Convert::resource(handle)->getDataAsAuxCompressionLength(mip, mipCount, frame, frameCount, face, faceCount);
}

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

SOURCEPP_API int vtfpp_vtf_is_valid(vtfpp_vtf_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, false);

	return Convert::vtf(handle)->operator bool();
}

SOURCEPP_API int vtfpp_vtf_create_from_mem_and_bake(const unsigned char* imageData, size_t imageLen, vtfpp_image_format_e format, uint16_t width, uint16_t height, const char* vtfPath, vtfpp_vtf_creation_options_t options) {
	SOURCEPP_EARLY_RETURN_VAL(imageData, false);
	SOURCEPP_EARLY_RETURN_VAL(imageLen, false);
	SOURCEPP_EARLY_RETURN_VAL(vtfPath, false);

	return VTF::create({reinterpret_cast<const std::byte*>(imageData), imageLen}, Convert::imageFormat(format), width, height, vtfPath, Convert::vtfCreationOptions(options));
}

SOURCEPP_API int vtfpp_vtf_create_blank_and_bake(vtfpp_image_format_e format, uint16_t width, uint16_t height, const char* vtfPath, vtfpp_vtf_creation_options_t options) {
	SOURCEPP_EARLY_RETURN_VAL(vtfPath, false);

	return VTF::create(Convert::imageFormat(format), width, height, vtfPath, Convert::vtfCreationOptions(options));
}

SOURCEPP_API vtfpp_vtf_handle_t vtfpp_vtf_create_from_mem(const unsigned char* imageData, size_t imageLen, vtfpp_image_format_e format, uint16_t width, uint16_t height, vtfpp_vtf_creation_options_t options) {
	SOURCEPP_EARLY_RETURN_VAL(imageData, nullptr);
	SOURCEPP_EARLY_RETURN_VAL(imageLen, nullptr);

	return new VTF{VTF::create({reinterpret_cast<const std::byte*>(imageData), imageLen}, Convert::imageFormat(format), width, height, Convert::vtfCreationOptions(options))};
}

SOURCEPP_API vtfpp_vtf_handle_t vtfpp_vtf_create_blank(vtfpp_image_format_e format, uint16_t width, uint16_t height, vtfpp_vtf_creation_options_t options) {
	return new VTF{VTF::create(Convert::imageFormat(format), width, height, Convert::vtfCreationOptions(options))};
}

SOURCEPP_API int vtfpp_vtf_create_from_file_and_bake(const char* imagePath, const char* vtfPath, vtfpp_vtf_creation_options_t options) {
	SOURCEPP_EARLY_RETURN_VAL(imagePath, false);
	SOURCEPP_EARLY_RETURN_VAL(vtfPath, false);

	return VTF::create(imagePath, vtfPath, Convert::vtfCreationOptions(options));
}

SOURCEPP_API vtfpp_vtf_handle_t vtfpp_vtf_create_from_file(const char* imagePath, vtfpp_vtf_creation_options_t options) {
	SOURCEPP_EARLY_RETURN_VAL(imagePath, nullptr);

	return new VTF{VTF::create(imagePath, Convert::vtfCreationOptions(options))};
}

SOURCEPP_API vtfpp_vtf_platform_e vtfpp_vtf_get_platform(vtfpp_vtf_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, VTFPP_VTF_PLATFORM_UNKNOWN);

	return Convert::vtfPlatform(Convert::vtf(handle)->getPlatform());
}

SOURCEPP_API void vtfpp_vtf_set_platform(vtfpp_vtf_handle_t handle, vtfpp_vtf_platform_e platform) {
	SOURCEPP_EARLY_RETURN(handle);

	Convert::vtf(handle)->setPlatform(Convert::vtfPlatform(platform));
}

SOURCEPP_API uint32_t vtfpp_vtf_get_version(vtfpp_vtf_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, 0);

	return Convert::vtf(handle)->getVersion();
}

SOURCEPP_API void vtfpp_vtf_set_version(vtfpp_vtf_handle_t handle, uint32_t version) {
	SOURCEPP_EARLY_RETURN(handle);

	return Convert::vtf(handle)->setVersion(version);
}

SOURCEPP_API vtfpp_image_conversion_resize_method_e vtfpp_vtf_get_image_width_resize_method(vtfpp_vtf_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, VTFPP_IMAGE_CONVERSION_RESIZE_METHOD_NONE);

	return Convert::resizeMethod(Convert::vtf(handle)->getImageWidthResizeMethod());
}

SOURCEPP_API vtfpp_image_conversion_resize_method_e vtfpp_vtf_get_image_height_resize_method(vtfpp_vtf_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, VTFPP_IMAGE_CONVERSION_RESIZE_METHOD_NONE);

	return Convert::resizeMethod(Convert::vtf(handle)->getImageHeightResizeMethod());
}

SOURCEPP_API void vtfpp_vtf_set_image_resize_methods(vtfpp_vtf_handle_t handle, vtfpp_image_conversion_resize_method_e imageWidthResizeMethod, vtfpp_image_conversion_resize_method_e imageHeightResizeMethod) {
	SOURCEPP_EARLY_RETURN(handle);

	return Convert::vtf(handle)->setImageResizeMethods(Convert::resizeMethod(imageWidthResizeMethod), Convert::resizeMethod(imageHeightResizeMethod));
}

SOURCEPP_API void vtfpp_vtf_set_image_width_resize_method(vtfpp_vtf_handle_t handle, vtfpp_image_conversion_resize_method_e imageWidthResizeMethod) {
	SOURCEPP_EARLY_RETURN(handle);

	return Convert::vtf(handle)->setImageWidthResizeMethod(Convert::resizeMethod(imageWidthResizeMethod));
}

SOURCEPP_API void vtfpp_vtf_set_image_height_resize_method(vtfpp_vtf_handle_t handle, vtfpp_image_conversion_resize_method_e imageHeightResizeMethod) {
	SOURCEPP_EARLY_RETURN(handle);

	return Convert::vtf(handle)->setImageHeightResizeMethod(Convert::resizeMethod(imageHeightResizeMethod));
}

SOURCEPP_API uint16_t vtfpp_vtf_get_width(vtfpp_vtf_handle_t handle, uint8_t mip) {
	SOURCEPP_EARLY_RETURN_VAL(handle, 0);

	return Convert::vtf(handle)->getWidth(mip);
}

SOURCEPP_API uint16_t vtfpp_vtf_get_height(vtfpp_vtf_handle_t handle, uint8_t mip) {
	SOURCEPP_EARLY_RETURN_VAL(handle, 0);

	return Convert::vtf(handle)->getHeight(mip);
}

SOURCEPP_API void vtfpp_vtf_set_size(vtfpp_vtf_handle_t handle, uint16_t width, uint16_t height, vtfpp_image_conversion_resize_filter_e filter) {
	SOURCEPP_EARLY_RETURN(handle);

	Convert::vtf(handle)->setSize(width, height, Convert::resizeFilter(filter));
}

SOURCEPP_API uint32_t vtfpp_vtf_get_flags(vtfpp_vtf_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, 0);

	return Convert::vtf(handle)->getFlags();
}

SOURCEPP_API void vtfpp_vtf_set_flags(vtfpp_vtf_handle_t handle, uint32_t flags) {
	SOURCEPP_EARLY_RETURN(handle);

	Convert::vtf(handle)->setFlags(flags);
}

SOURCEPP_API void vtfpp_vtf_add_flags(vtfpp_vtf_handle_t handle, uint32_t flags) {
	SOURCEPP_EARLY_RETURN(handle);

	Convert::vtf(handle)->addFlags(flags);
}

SOURCEPP_API void vtfpp_vtf_remove_flags(vtfpp_vtf_handle_t handle, uint32_t flags) {
	SOURCEPP_EARLY_RETURN(handle);

	Convert::vtf(handle)->removeFlags(flags);
}

SOURCEPP_API int vtfpp_vtf_is_srgb(vtfpp_vtf_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, false);

	return Convert::vtf(handle)->isSRGB();
}

SOURCEPP_API void vtfpp_vtf_set_srgb(vtfpp_vtf_handle_t handle, int srgb) {
	SOURCEPP_EARLY_RETURN(handle);

	Convert::vtf(handle)->setSRGB(srgb);
}

SOURCEPP_API void vtfpp_vtf_compute_transparency_flags(vtfpp_vtf_handle_t handle) {
	SOURCEPP_EARLY_RETURN(handle);

	Convert::vtf(handle)->computeTransparencyFlags();
}

SOURCEPP_API vtfpp_image_format_e vtfpp_vtf_get_default_compressed_format(vtfpp_image_format_e inputFormat, uint32_t version, int isCubeMap) {
	return Convert::imageFormat(VTF::getDefaultCompressedFormat(Convert::imageFormat(inputFormat), version, isCubeMap));
}

SOURCEPP_API vtfpp_image_format_e vtfpp_vtf_get_format(vtfpp_vtf_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, VTFPP_IMAGE_FORMAT_EMPTY);

	return Convert::imageFormat(Convert::vtf(handle)->getFormat());
}

SOURCEPP_API void vtfpp_vtf_set_format(vtfpp_vtf_handle_t handle, vtfpp_image_format_e format, vtfpp_image_conversion_resize_filter_e filter) {
	SOURCEPP_EARLY_RETURN(handle);

	Convert::vtf(handle)->setFormat(Convert::imageFormat(format), Convert::resizeFilter(filter));
}

SOURCEPP_API uint8_t vtfpp_vtf_get_mip_count(vtfpp_vtf_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, 0);

	return Convert::vtf(handle)->getMipCount();
}

SOURCEPP_API int vtfpp_vtf_set_mip_count(vtfpp_vtf_handle_t handle, uint8_t mipCount) {
	SOURCEPP_EARLY_RETURN_VAL(handle, false);

	return Convert::vtf(handle)->setMipCount(mipCount);
}

SOURCEPP_API int vtfpp_vtf_set_recommended_mip_count(vtfpp_vtf_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, false);

	return Convert::vtf(handle)->setRecommendedMipCount();
}

SOURCEPP_API void vtfpp_vtf_compute_mips(vtfpp_vtf_handle_t handle, vtfpp_image_conversion_resize_filter_e filter) {
	SOURCEPP_EARLY_RETURN(handle);

	return Convert::vtf(handle)->computeMips(Convert::resizeFilter(filter));
}

SOURCEPP_API uint16_t vtfpp_vtf_get_frame_count(vtfpp_vtf_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, 0);

	return Convert::vtf(handle)->getFrameCount();
}

SOURCEPP_API int vtfpp_vtf_set_frame_count(vtfpp_vtf_handle_t handle, uint16_t frameCount) {
	SOURCEPP_EARLY_RETURN_VAL(handle, false);

	return Convert::vtf(handle)->setFrameCount(frameCount);
}

SOURCEPP_API uint8_t vtfpp_vtf_get_face_count(vtfpp_vtf_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, 0);

	return Convert::vtf(handle)->getFaceCount();
}

SOURCEPP_API int vtfpp_vtf_set_face_count(vtfpp_vtf_handle_t handle, int isCubeMap) {
	SOURCEPP_EARLY_RETURN_VAL(handle, false);

	return Convert::vtf(handle)->setFaceCount(isCubeMap);
}

SOURCEPP_API uint16_t vtfpp_vtf_get_slice_count(vtfpp_vtf_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, 0);

	return Convert::vtf(handle)->getSliceCount();
}

SOURCEPP_API int vtfpp_vtf_set_slice_count(vtfpp_vtf_handle_t handle, uint16_t sliceCount) {
	SOURCEPP_EARLY_RETURN_VAL(handle, false);

	return Convert::vtf(handle)->setSliceCount(sliceCount);
}

SOURCEPP_API int vtfpp_vtf_set_frame_face_and_slice_count(vtfpp_vtf_handle_t handle, uint16_t frameCount, int isCubeMap, uint16_t sliceCount) {
	SOURCEPP_EARLY_RETURN_VAL(handle, false);

	return Convert::vtf(handle)->setFrameFaceAndSliceCount(frameCount, isCubeMap, sliceCount);
}

SOURCEPP_API uint16_t vtfpp_vtf_get_start_frame(vtfpp_vtf_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, 0);

	return Convert::vtf(handle)->getStartFrame();
}

SOURCEPP_API void vtfpp_vtf_set_start_frame(vtfpp_vtf_handle_t handle, uint16_t startFrame) {
	SOURCEPP_EARLY_RETURN(handle);

	return Convert::vtf(handle)->setStartFrame(startFrame);
}

SOURCEPP_API void vtfpp_vtf_get_reflectivity(vtfpp_vtf_handle_t handle, float* r, float* g, float* b) {
	SOURCEPP_EARLY_RETURN(handle);
	SOURCEPP_EARLY_RETURN(r);
	SOURCEPP_EARLY_RETURN(g);
	SOURCEPP_EARLY_RETURN(b);

	const auto reflectivity = Convert::vtf(handle)->getReflectivity();
	*r = reflectivity[0];
	*g = reflectivity[1];
	*b = reflectivity[2];
}

SOURCEPP_API void vtfpp_vtf_set_reflectivity(vtfpp_vtf_handle_t handle, float r, float g, float b) {
	SOURCEPP_EARLY_RETURN(handle);

	Convert::vtf(handle)->setReflectivity({r, g, b});
}

SOURCEPP_API void vtfpp_vtf_compute_reflectivity(vtfpp_vtf_handle_t handle) {
	SOURCEPP_EARLY_RETURN(handle);

	Convert::vtf(handle)->computeReflectivity();
}

SOURCEPP_API float vtfpp_vtf_get_bumpmap_scale(vtfpp_vtf_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, 0.f);

	return Convert::vtf(handle)->getBumpMapScale();
}

SOURCEPP_API void vtfpp_vtf_set_bumpmap_scale(vtfpp_vtf_handle_t handle, float bumpMapScale) {
	SOURCEPP_EARLY_RETURN(handle);

	Convert::vtf(handle)->setBumpMapScale(bumpMapScale);
}

SOURCEPP_API vtfpp_image_format_e vtfpp_vtf_get_thumbnail_format(vtfpp_vtf_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, VTFPP_IMAGE_FORMAT_EMPTY);

	return Convert::imageFormat(Convert::vtf(handle)->getThumbnailFormat());
}

SOURCEPP_API uint8_t vtfpp_vtf_get_thumbnail_width(vtfpp_vtf_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, 0);

	return Convert::vtf(handle)->getThumbnailWidth();
}

SOURCEPP_API uint8_t vtfpp_vtf_get_thumbnail_height(vtfpp_vtf_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, 0);

	return Convert::vtf(handle)->getThumbnailHeight();
}

SOURCEPP_API uint32_t vtfpp_vtf_get_resources_count(vtfpp_vtf_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, 0);

	return Convert::vtf(handle)->getResources().size();
}

SOURCEPP_API vtfpp_resource_handle_t vtfpp_vtf_get_resource_at_index(vtfpp_vtf_handle_t handle, uint32_t index) {
	SOURCEPP_EARLY_RETURN_VAL(handle, nullptr);

	return &Convert::vtf(handle)->getResources().at(index);
}

SOURCEPP_API vtfpp_resource_handle_t vtfpp_vtf_get_resource_with_type(vtfpp_vtf_handle_t handle, vtfpp_resource_type_e type) {
	SOURCEPP_EARLY_RETURN_VAL(handle, nullptr);

	return Convert::vtf(handle)->getResource(Convert::resourceType(type));
}

SOURCEPP_API sourcepp_buffer_t vtfpp_vtf_get_particle_sheet_frame_data_raw(vtfpp_vtf_handle_t handle, uint16_t* width, uint16_t* height, uint32_t shtSequenceID, uint32_t shtFrame, uint8_t shtBounds, uint8_t mip, uint16_t frame, uint8_t face, uint16_t slice) {
	SOURCEPP_EARLY_RETURN_VAL(handle, SOURCEPP_BUFFER_INVALID);
	SOURCEPP_EARLY_RETURN_VAL(width, SOURCEPP_BUFFER_INVALID);
	SOURCEPP_EARLY_RETURN_VAL(height, SOURCEPP_BUFFER_INVALID);

	return Convert::toBuffer(Convert::vtf(handle)->getParticleSheetFrameDataRaw(*width, *height, shtSequenceID, shtFrame, shtBounds, mip, frame, face, slice));
}

SOURCEPP_API sourcepp_buffer_t vtfpp_vtf_get_particle_sheet_frame_data_as(vtfpp_vtf_handle_t handle, vtfpp_image_format_e format, uint16_t* width, uint16_t* height, uint32_t shtSequenceID, uint32_t shtFrame, uint8_t shtBounds, uint8_t mip, uint16_t frame, uint8_t face, uint16_t slice) {
	SOURCEPP_EARLY_RETURN_VAL(handle, SOURCEPP_BUFFER_INVALID);
	SOURCEPP_EARLY_RETURN_VAL(width, SOURCEPP_BUFFER_INVALID);
	SOURCEPP_EARLY_RETURN_VAL(height, SOURCEPP_BUFFER_INVALID);

	return Convert::toBuffer(Convert::vtf(handle)->getParticleSheetFrameDataAs(Convert::imageFormat(format), *width, *height, shtSequenceID, shtFrame, shtBounds, mip, frame, face, slice));
}

SOURCEPP_API sourcepp_buffer_t vtfpp_vtf_get_particle_sheet_frame_data_as_rgba8888(vtfpp_vtf_handle_t handle, uint16_t* width, uint16_t* height, uint32_t shtSequenceID, uint32_t shtFrame, uint8_t shtBounds, uint8_t mip, uint16_t frame, uint8_t face, uint16_t slice) {
	SOURCEPP_EARLY_RETURN_VAL(handle, SOURCEPP_BUFFER_INVALID);
	SOURCEPP_EARLY_RETURN_VAL(width, SOURCEPP_BUFFER_INVALID);
	SOURCEPP_EARLY_RETURN_VAL(height, SOURCEPP_BUFFER_INVALID);

	return Convert::toBuffer(Convert::vtf(handle)->getParticleSheetFrameDataAsRGBA8888(*width, *height, shtSequenceID, shtFrame, shtBounds, mip, frame, face, slice));
}

SOURCEPP_API void vtfpp_vtf_set_particle_sheet_resource(vtfpp_vtf_handle_t handle, vtfpp_sht_handle_t sht) {
	SOURCEPP_EARLY_RETURN(handle);

	Convert::vtf(handle)->setParticleSheetResource(*Convert::sht(sht));
}

SOURCEPP_API void vtfpp_vtf_remove_particle_sheet_resource(vtfpp_vtf_handle_t handle) {
	SOURCEPP_EARLY_RETURN(handle);

	Convert::vtf(handle)->removeParticleSheetResource();
}

SOURCEPP_API void vtfpp_vtf_set_crc_resource(vtfpp_vtf_handle_t handle, uint32_t value) {
	SOURCEPP_EARLY_RETURN(handle);

	Convert::vtf(handle)->setCRCResource(value);
}

SOURCEPP_API void vtfpp_vtf_remove_crc_resource(vtfpp_vtf_handle_t handle) {
	SOURCEPP_EARLY_RETURN(handle);

	Convert::vtf(handle)->removeCRCResource();
}

SOURCEPP_API void vtfpp_vtf_set_lod_resource(vtfpp_vtf_handle_t handle, uint8_t u, uint8_t v, uint8_t u360, uint8_t v360) {
	SOURCEPP_EARLY_RETURN(handle);

	Convert::vtf(handle)->setLODResource(u, v, u360, v360);
}

SOURCEPP_API void vtfpp_vtf_remove_lod_resource(vtfpp_vtf_handle_t handle) {
	SOURCEPP_EARLY_RETURN(handle);

	Convert::vtf(handle)->removeLODResource();
}

SOURCEPP_API void vtfpp_vtf_set_extended_flags_resource(vtfpp_vtf_handle_t handle, uint32_t value) {
	SOURCEPP_EARLY_RETURN(handle);

	Convert::vtf(handle)->setExtendedFlagsResource(value);
}

SOURCEPP_API void vtfpp_vtf_remove_extended_flags_resource(vtfpp_vtf_handle_t handle) {
	SOURCEPP_EARLY_RETURN(handle);

	Convert::vtf(handle)->removeExtendedFlagsResource();
}

SOURCEPP_API void vtfpp_vtf_set_keyvalues_data_resource(vtfpp_vtf_handle_t handle, const char* value) {
	SOURCEPP_EARLY_RETURN(handle);
	SOURCEPP_EARLY_RETURN(value);

	Convert::vtf(handle)->setKeyValuesDataResource(value);
}

SOURCEPP_API void vtfpp_vtf_remove_keyvalues_data_resource(vtfpp_vtf_handle_t handle) {
	SOURCEPP_EARLY_RETURN(handle);

	Convert::vtf(handle)->removeKeyValuesDataResource();
}

SOURCEPP_API int16_t vtfpp_vtf_get_compression_level(vtfpp_vtf_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, 0);

	return Convert::vtf(handle)->getCompressionLevel();
}

SOURCEPP_API void vtfpp_vtf_set_compression_level(vtfpp_vtf_handle_t handle, int16_t compressionLevel) {
	SOURCEPP_EARLY_RETURN(handle);

	Convert::vtf(handle)->setCompressionLevel(compressionLevel);
}

SOURCEPP_API vtfpp_compression_method_e vtfpp_vtf_get_compression_method(vtfpp_vtf_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, VTFPP_COMPRESSION_METHOD_ZSTD);

	return Convert::compressionMethod(Convert::vtf(handle)->getCompressionMethod());
}

SOURCEPP_API void vtfpp_vtf_set_compression_method(vtfpp_vtf_handle_t handle, vtfpp_compression_method_e compressionMethod) {
	SOURCEPP_EARLY_RETURN(handle);

	Convert::vtf(handle)->setCompressionMethod(Convert::compressionMethod(compressionMethod));
}

SOURCEPP_API int vtfpp_vtf_has_image_data(vtfpp_vtf_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, false);

	return Convert::vtf(handle)->hasImageData();
}

SOURCEPP_API const unsigned char* vtfpp_vtf_get_image_data_raw(vtfpp_vtf_handle_t handle, size_t* imageLen, uint8_t mip, uint16_t frame, uint8_t face, uint16_t slice) {
	SOURCEPP_EARLY_RETURN_VAL(handle, nullptr);
	SOURCEPP_EARLY_RETURN_VAL(imageLen, nullptr);

	const auto imageData = Convert::vtf(handle)->getImageDataRaw(mip, frame, face, slice);
	*imageLen = imageData.size();
	return reinterpret_cast<const unsigned char*>(imageData.data());
}

SOURCEPP_API sourcepp_buffer_t vtfpp_vtf_get_image_data_as(vtfpp_vtf_handle_t handle, vtfpp_image_format_e format, uint8_t mip, uint16_t frame, uint8_t face, uint16_t slice) {
	SOURCEPP_EARLY_RETURN_VAL(handle, SOURCEPP_BUFFER_INVALID);

	return Convert::toBuffer(Convert::vtf(handle)->getImageDataAs(Convert::imageFormat(format), mip, frame, face, slice));
}

SOURCEPP_API sourcepp_buffer_t vtfpp_vtf_get_image_data_as_rgba8888(vtfpp_vtf_handle_t handle, uint8_t mip, uint16_t frame, uint8_t face, uint16_t slice) {
	SOURCEPP_EARLY_RETURN_VAL(handle, SOURCEPP_BUFFER_INVALID);

	return Convert::toBuffer(Convert::vtf(handle)->getImageDataAsRGBA8888(mip, frame, face, slice));
}

SOURCEPP_API int vtfpp_vtf_set_image_from_mem(vtfpp_vtf_handle_t handle, const unsigned char* imageData, size_t imageLen, vtfpp_image_format_e format, uint16_t width, uint16_t height, vtfpp_image_conversion_resize_filter_e filter, uint8_t mip, uint16_t frame, uint8_t face, uint16_t slice) {
	SOURCEPP_EARLY_RETURN_VAL(handle, false);
	SOURCEPP_EARLY_RETURN_VAL(imageData, false);
	SOURCEPP_EARLY_RETURN_VAL(imageLen, false);

	return Convert::vtf(handle)->setImage({reinterpret_cast<const std::byte*>(imageData), imageLen}, Convert::imageFormat(format), width, height, Convert::resizeFilter(filter), mip, frame, face, slice);
}

SOURCEPP_API int vtfpp_vtf_set_image_from_file(vtfpp_vtf_handle_t handle, const char* imagePath, vtfpp_image_conversion_resize_filter_e filter, uint8_t mip, uint16_t frame, uint8_t face, uint16_t slice) {
	SOURCEPP_EARLY_RETURN_VAL(handle, false);
	SOURCEPP_EARLY_RETURN_VAL(imagePath, false);

	return Convert::vtf(handle)->setImage(imagePath, Convert::resizeFilter(filter), mip, frame, face, slice);
}

SOURCEPP_API int vtfpp_vtf_save_image_to_file(vtfpp_vtf_handle_t handle, const char* imagePath, uint8_t mip, uint16_t frame, uint8_t face, uint16_t slice, vtfpp_image_conversion_file_format_e fileFormat) {
	SOURCEPP_EARLY_RETURN_VAL(handle, false);
	SOURCEPP_EARLY_RETURN_VAL(imagePath, false);

	return Convert::vtf(handle)->saveImageToFile(imagePath, mip, frame, face, slice, Convert::fileFormat(fileFormat));
}

SOURCEPP_API int vtfpp_vtf_has_thumbnail_data(vtfpp_vtf_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, false);

	return Convert::vtf(handle)->hasThumbnailData();
}

SOURCEPP_API const unsigned char* vtfpp_vtf_get_thumbnail_data_raw(vtfpp_vtf_handle_t handle, size_t* imageLen) {
	SOURCEPP_EARLY_RETURN_VAL(handle, nullptr);
	SOURCEPP_EARLY_RETURN_VAL(imageLen, nullptr);

	const auto thumbnailData = Convert::vtf(handle)->getThumbnailDataRaw();
	*imageLen = thumbnailData.size();
	return reinterpret_cast<const unsigned char*>(thumbnailData.data());
}

SOURCEPP_API sourcepp_buffer_t vtfpp_vtf_get_thumbnail_data_as(vtfpp_vtf_handle_t handle, vtfpp_image_format_e format) {
	SOURCEPP_EARLY_RETURN_VAL(handle, SOURCEPP_BUFFER_INVALID);

	return Convert::toBuffer(Convert::vtf(handle)->getThumbnailDataAs(Convert::imageFormat(format)));
}

SOURCEPP_API sourcepp_buffer_t vtfpp_vtf_get_thumbnail_data_as_rgba8888(vtfpp_vtf_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, SOURCEPP_BUFFER_INVALID);

	return Convert::toBuffer(Convert::vtf(handle)->getThumbnailDataAsRGBA8888());
}

SOURCEPP_API void vtfpp_vtf_set_thumbnail(vtfpp_vtf_handle_t handle, const unsigned char* imageData, size_t imageLen, vtfpp_image_format_e format, uint16_t width, uint16_t height) {
	SOURCEPP_EARLY_RETURN(handle);
	SOURCEPP_EARLY_RETURN(imageData);
	SOURCEPP_EARLY_RETURN(imageLen);

	Convert::vtf(handle)->setThumbnail({reinterpret_cast<const std::byte*>(imageData), imageLen}, Convert::imageFormat(format), width, height);
}

SOURCEPP_API void vtfpp_vtf_compute_thumbnail(vtfpp_vtf_handle_t handle, vtfpp_image_conversion_resize_filter_e filter) {
	SOURCEPP_EARLY_RETURN(handle);

	Convert::vtf(handle)->computeThumbnail(Convert::resizeFilter(filter));
}

SOURCEPP_API void vtfpp_vtf_remove_thumbnail(vtfpp_vtf_handle_t handle) {
	SOURCEPP_EARLY_RETURN(handle);

	Convert::vtf(handle)->removeThumbnail();
}

SOURCEPP_API int vtfpp_vtf_save_thumbnail_to_file(vtfpp_vtf_handle_t handle, const char* imagePath, vtfpp_image_conversion_file_format_e fileFormat) {
	SOURCEPP_EARLY_RETURN_VAL(handle, false);
	SOURCEPP_EARLY_RETURN_VAL(imagePath, false);

	return Convert::vtf(handle)->saveThumbnailToFile(imagePath, Convert::fileFormat(fileFormat));
}

SOURCEPP_API sourcepp_buffer_t vtfpp_vtf_bake(vtfpp_vtf_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, SOURCEPP_BUFFER_INVALID);

	return Convert::toBuffer(Convert::vtf(handle)->bake());
}

SOURCEPP_API int vtfpp_vtf_bake_to_file(vtfpp_vtf_handle_t handle, const char* vtfPath) {
	SOURCEPP_EARLY_RETURN_VAL(handle, false);
	SOURCEPP_EARLY_RETURN_VAL(vtfPath, false);

	return Convert::vtf(handle)->bake(vtfPath);
}

SOURCEPP_API void vtfpp_vtf_close(vtfpp_vtf_handle_t* handle) {
	SOURCEPP_EARLY_RETURN(handle);

	delete Convert::vtf(*handle);
	*handle = nullptr;
}
