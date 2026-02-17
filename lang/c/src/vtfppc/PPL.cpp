#include <vtfppc/PPL.h>

#include <vtfpp/PPL.h>

#include <sourceppc/Helpers.h>
#include <vtfppc/Convert.hpp>

using namespace sourceppc;
using namespace vtfpp;

SOURCEPP_API vtfpp_ppl_handle_t vtfpp_ppl_create(uint32_t modelChecksum) {
	return new PPL{modelChecksum};
}

SOURCEPP_API vtfpp_ppl_handle_t vtfpp_ppl_create_with_options(uint32_t modelChecksum, vtfpp_image_format_e format, uint32_t version) {
	return new PPL{modelChecksum, static_cast<ImageFormat>(format), version};
}

SOURCEPP_API vtfpp_ppl_handle_t vtfpp_ppl_open_from_mem(const unsigned char* buffer, size_t bufferLen) {
	SOURCEPP_EARLY_RETURN_VAL(buffer, nullptr);
	SOURCEPP_EARLY_RETURN_VAL(bufferLen, nullptr);

	return new PPL{{reinterpret_cast<const std::byte*>(buffer), bufferLen}};
}

SOURCEPP_API vtfpp_ppl_handle_t vtfpp_ppl_open_from_file(const char* pplPath) {
	SOURCEPP_EARLY_RETURN_VAL(pplPath, nullptr);

	return new PPL{pplPath};
}

SOURCEPP_API int vtfpp_ppl_is_valid(vtfpp_ppl_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, false);

	return Convert::ppl(handle)->operator bool();
}

SOURCEPP_API uint32_t vtfpp_ppl_get_version(vtfpp_ppl_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, 0);

	return Convert::ppl(handle)->getVersion();
}

SOURCEPP_API void vtfpp_ppl_set_version(vtfpp_ppl_handle_t handle, uint32_t version) {
	SOURCEPP_EARLY_RETURN(handle);

	return Convert::ppl(handle)->setVersion(version);
}

SOURCEPP_API uint32_t vtfpp_ppl_get_model_checksum(vtfpp_ppl_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, 0);

	return Convert::ppl(handle)->getModelChecksum();
}

SOURCEPP_API void vtfpp_ppl_set_model_checksum(vtfpp_ppl_handle_t handle, uint32_t modelChecksum) {
	SOURCEPP_EARLY_RETURN(handle);

	Convert::ppl(handle)->setModelChecksum(modelChecksum);
}

SOURCEPP_API vtfpp_image_format_e vtfpp_ppl_get_format(vtfpp_ppl_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, VTFPP_IMAGE_FORMAT_EMPTY);

	return static_cast<vtfpp_image_format_e>(Convert::ppl(handle)->getFormat());
}

SOURCEPP_API void vtfpp_ppl_set_format(vtfpp_ppl_handle_t handle, vtfpp_image_format_e format, float quality) {
	SOURCEPP_EARLY_RETURN(handle);

	Convert::ppl(handle)->setFormat(static_cast<ImageFormat>(format), quality);
}

SOURCEPP_API int vtfpp_ppl_has_image_for_lod(vtfpp_ppl_handle_t handle, uint32_t lod) {
	SOURCEPP_EARLY_RETURN_VAL(handle, false);

	return Convert::ppl(handle)->hasImageForLOD(lod);
}

SOURCEPP_API sourcepp_buffer_t vtfpp_ppl_get_image_lods(vtfpp_ppl_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, SOURCEPP_BUFFER_INVALID);

	return convert::toBuffer(Convert::ppl(handle)->getImageLODs());
}

SOURCEPP_API sourcepp_buffer_t vtfpp_ppl_get_image_raw(vtfpp_ppl_handle_t handle, uint32_t* width, uint32_t* height, uint32_t lod) {
	SOURCEPP_EARLY_RETURN_VAL(handle, SOURCEPP_BUFFER_INVALID);

	return convert::toBuffer(Convert::ppl(handle)->getImageLODs());
}

SOURCEPP_API sourcepp_buffer_t vtfpp_ppl_get_image_as(vtfpp_ppl_handle_t handle, uint32_t* width, uint32_t* height, vtfpp_image_format_e format, uint32_t lod) {
	SOURCEPP_EARLY_RETURN_VAL(handle, SOURCEPP_BUFFER_INVALID);
	SOURCEPP_EARLY_RETURN_VAL(width, SOURCEPP_BUFFER_INVALID);
	SOURCEPP_EARLY_RETURN_VAL(height, SOURCEPP_BUFFER_INVALID);
	SOURCEPP_EARLY_RETURN_VAL(format != VTFPP_IMAGE_FORMAT_EMPTY, SOURCEPP_BUFFER_INVALID);

	auto image = Convert::ppl(handle)->getImageAs(static_cast<ImageFormat>(format), lod);
	if (!image) {
		return SOURCEPP_BUFFER_INVALID;
	}
	*width = image->width;
	*height = image->height;
	return convert::toBuffer(image->data);
}

SOURCEPP_API sourcepp_buffer_t vtfpp_ppl_get_image_as_rgb888(vtfpp_ppl_handle_t handle, uint32_t* width, uint32_t* height, uint32_t lod) {
	SOURCEPP_EARLY_RETURN_VAL(handle, SOURCEPP_BUFFER_INVALID);
	SOURCEPP_EARLY_RETURN_VAL(width, SOURCEPP_BUFFER_INVALID);
	SOURCEPP_EARLY_RETURN_VAL(height, SOURCEPP_BUFFER_INVALID);

	auto image = Convert::ppl(handle)->getImageAs(ImageFormat::RGB888, lod);
	if (!image) {
		return SOURCEPP_BUFFER_INVALID;
	}
	*width = image->width;
	*height = image->height;
	return convert::toBuffer(image->data);
}

SOURCEPP_API int vtfpp_ppl_set_image_from_file(vtfpp_ppl_handle_t handle, const char* imagePath, uint32_t lod, float quality) {
	SOURCEPP_EARLY_RETURN_VAL(handle, false);
	SOURCEPP_EARLY_RETURN_VAL(imagePath, false);

	return Convert::ppl(handle)->setImage(imagePath, lod, quality);
}

SOURCEPP_API int vtfpp_ppl_set_image_from_file_with_options(vtfpp_ppl_handle_t handle, const char* imagePath, uint32_t resizedWidth, uint32_t resizedHeight, uint32_t lod, vtfpp_image_conversion_resize_filter_e filter, float quality) {
	SOURCEPP_EARLY_RETURN_VAL(handle, false);
	SOURCEPP_EARLY_RETURN_VAL(imagePath, false);
	SOURCEPP_EARLY_RETURN_VAL(resizedWidth, false);
	SOURCEPP_EARLY_RETURN_VAL(resizedHeight, false);

	return Convert::ppl(handle)->setImage(imagePath, resizedWidth, resizedHeight, lod, static_cast<ImageConversion::ResizeFilter>(filter), quality);
}

SOURCEPP_API int vtfpp_ppl_set_image_from_mem(vtfpp_ppl_handle_t handle, const unsigned char* buffer, size_t bufferLen, vtfpp_image_format_e format, uint32_t width, uint32_t height, uint32_t lod, float quality) {
	SOURCEPP_EARLY_RETURN_VAL(handle, false);
	SOURCEPP_EARLY_RETURN_VAL(buffer, false);
	SOURCEPP_EARLY_RETURN_VAL(bufferLen, false);
	SOURCEPP_EARLY_RETURN_VAL(format != VTFPP_IMAGE_FORMAT_EMPTY, false);
	SOURCEPP_EARLY_RETURN_VAL(width, false);
	SOURCEPP_EARLY_RETURN_VAL(height, false);

	return Convert::ppl(handle)->setImage({reinterpret_cast<const std::byte*>(buffer), bufferLen}, static_cast<ImageFormat>(format), width, height, lod, quality);
}

SOURCEPP_API int vtfpp_ppl_set_image_from_mem_with_options(vtfpp_ppl_handle_t handle, const unsigned char* buffer, size_t bufferLen, vtfpp_image_format_e format, uint32_t width, uint32_t height, uint32_t resizedWidth, uint32_t resizedHeight, uint32_t lod, vtfpp_image_conversion_resize_filter_e filter, float quality) {
	SOURCEPP_EARLY_RETURN_VAL(handle, false);
	SOURCEPP_EARLY_RETURN_VAL(buffer, false);
	SOURCEPP_EARLY_RETURN_VAL(bufferLen, false);
	SOURCEPP_EARLY_RETURN_VAL(format != VTFPP_IMAGE_FORMAT_EMPTY, false);
	SOURCEPP_EARLY_RETURN_VAL(width, false);
	SOURCEPP_EARLY_RETURN_VAL(height, false);
	SOURCEPP_EARLY_RETURN_VAL(resizedWidth, false);
	SOURCEPP_EARLY_RETURN_VAL(resizedHeight, false);

	return Convert::ppl(handle)->setImage({reinterpret_cast<const std::byte*>(buffer), bufferLen}, static_cast<ImageFormat>(format), width, height, resizedWidth, resizedHeight, lod, static_cast<ImageConversion::ResizeFilter>(filter), quality);
}

SOURCEPP_API sourcepp_buffer_t vtfpp_ppl_save_image_to_file(vtfpp_ppl_handle_t handle, const char* imagePath, uint32_t lod, vtfpp_image_conversion_file_format_e fileFormat) {
	SOURCEPP_EARLY_RETURN_VAL(handle, SOURCEPP_BUFFER_INVALID);
	SOURCEPP_EARLY_RETURN_VAL(imagePath, SOURCEPP_BUFFER_INVALID);

	return convert::toBuffer(Convert::ppl(handle)->saveImageToFile(lod, static_cast<ImageConversion::FileFormat>(fileFormat)));
}

SOURCEPP_API sourcepp_buffer_t vtfpp_ppl_bake(vtfpp_ppl_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, SOURCEPP_BUFFER_INVALID);

	return convert::toBuffer(Convert::ppl(handle)->bake());
}

SOURCEPP_API int vtfpp_ppl_bake_to_file(vtfpp_ppl_handle_t handle, const char* pplPath) {
	SOURCEPP_EARLY_RETURN_VAL(handle, false);
	SOURCEPP_EARLY_RETURN_VAL(pplPath, false);

	return Convert::ppl(handle)->bake(pplPath);
}

SOURCEPP_API void vtfpp_ppl_close(vtfpp_ppl_handle_t* handle) {
	SOURCEPP_EARLY_RETURN(handle);

	delete Convert::ppl(*handle);
	*handle = nullptr;
}
