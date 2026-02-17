#include <vtfppc/PPL.h>

#include <vtfpp/PPL.h>

#include <sourceppc/Helpers.h>

using namespace sourceppc;
using namespace vtfpp;

SOURCEPP_API vtfpp_ppl_handle_t vtfpp_ppl_create(uint32_t modelChecksum) {
	return new PPL{modelChecksum};
}

SOURCEPP_API vtfpp_ppl_handle_t vtfpp_ppl_create_with_options(uint32_t modelChecksum, vtfpp_image_format_e format, uint32_t version) {
	return new PPL{modelChecksum, convert::cast(format), version};
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

SOURCEPP_API void vtfpp_ppl_free(vtfpp_ppl_handle_t* handle) {
	SOURCEPP_EARLY_RETURN(handle);

	delete convert::handle<PPL>(*handle);
	*handle = nullptr;
}

SOURCEPP_API int vtfpp_ppl_is_valid(vtfpp_ppl_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, false);

	return convert::handle<PPL>(handle)->operator bool();
}

SOURCEPP_API uint32_t vtfpp_ppl_get_version(vtfpp_ppl_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, 0);

	return convert::handle<PPL>(handle)->getVersion();
}

SOURCEPP_API void vtfpp_ppl_set_version(vtfpp_ppl_handle_t handle, uint32_t version) {
	SOURCEPP_EARLY_RETURN(handle);

	return convert::handle<PPL>(handle)->setVersion(version);
}

SOURCEPP_API uint32_t vtfpp_ppl_get_model_checksum(vtfpp_ppl_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, 0);

	return convert::handle<PPL>(handle)->getModelChecksum();
}

SOURCEPP_API void vtfpp_ppl_set_model_checksum(vtfpp_ppl_handle_t handle, uint32_t modelChecksum) {
	SOURCEPP_EARLY_RETURN(handle);

	convert::handle<PPL>(handle)->setModelChecksum(modelChecksum);
}

SOURCEPP_API vtfpp_image_format_e vtfpp_ppl_get_format(vtfpp_ppl_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, VTFPP_IMAGE_FORMAT_EMPTY);

	return convert::cast(convert::handle<PPL>(handle)->getFormat());
}

SOURCEPP_API void vtfpp_ppl_set_format(vtfpp_ppl_handle_t handle, vtfpp_image_format_e format, float quality) {
	SOURCEPP_EARLY_RETURN(handle);

	convert::handle<PPL>(handle)->setFormat(convert::cast(format), quality);
}

SOURCEPP_API int vtfpp_ppl_has_image_for_lod(vtfpp_ppl_handle_t handle, uint32_t lod) {
	SOURCEPP_EARLY_RETURN_VAL(handle, false);

	return convert::handle<PPL>(handle)->hasImageForLOD(lod);
}

SOURCEPP_API sourcepp_buffer_t vtfpp_ppl_get_image_lods(vtfpp_ppl_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, SOURCEPP_BUFFER_INVALID);

	return convert::toBuffer(convert::handle<PPL>(handle)->getImageLODs());
}

SOURCEPP_API sourcepp_buffer_t vtfpp_ppl_get_image_raw(vtfpp_ppl_handle_t handle, uint32_t* width, uint32_t* height, uint32_t lod) {
	SOURCEPP_EARLY_RETURN_VAL(handle, SOURCEPP_BUFFER_INVALID);

	return convert::toBuffer(convert::handle<PPL>(handle)->getImageLODs());
}

SOURCEPP_API sourcepp_buffer_t vtfpp_ppl_get_image_as(vtfpp_ppl_handle_t handle, uint32_t* width, uint32_t* height, vtfpp_image_format_e format, uint32_t lod) {
	SOURCEPP_EARLY_RETURN_VAL(handle, SOURCEPP_BUFFER_INVALID);
	SOURCEPP_EARLY_RETURN_VAL(width, SOURCEPP_BUFFER_INVALID);
	SOURCEPP_EARLY_RETURN_VAL(height, SOURCEPP_BUFFER_INVALID);
	SOURCEPP_EARLY_RETURN_VAL(format != VTFPP_IMAGE_FORMAT_EMPTY, SOURCEPP_BUFFER_INVALID);

	auto image = convert::handle<PPL>(handle)->getImageAs(convert::cast(format), lod);
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

	auto image = convert::handle<PPL>(handle)->getImageAs(ImageFormat::RGB888, lod);
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

	return convert::handle<PPL>(handle)->setImage(imagePath, lod, quality);
}

SOURCEPP_API int vtfpp_ppl_set_image_from_file_with_options(vtfpp_ppl_handle_t handle, const char* imagePath, uint32_t resizedWidth, uint32_t resizedHeight, uint32_t lod, vtfpp_image_conversion_resize_filter_e filter, float quality) {
	SOURCEPP_EARLY_RETURN_VAL(handle, false);
	SOURCEPP_EARLY_RETURN_VAL(imagePath, false);
	SOURCEPP_EARLY_RETURN_VAL(resizedWidth, false);
	SOURCEPP_EARLY_RETURN_VAL(resizedHeight, false);

	return convert::handle<PPL>(handle)->setImage(imagePath, resizedWidth, resizedHeight, lod, convert::cast(filter), quality);
}

SOURCEPP_API int vtfpp_ppl_set_image_from_mem(vtfpp_ppl_handle_t handle, const unsigned char* buffer, size_t bufferLen, vtfpp_image_format_e format, uint32_t width, uint32_t height, uint32_t lod, float quality) {
	SOURCEPP_EARLY_RETURN_VAL(handle, false);
	SOURCEPP_EARLY_RETURN_VAL(buffer, false);
	SOURCEPP_EARLY_RETURN_VAL(bufferLen, false);
	SOURCEPP_EARLY_RETURN_VAL(format != VTFPP_IMAGE_FORMAT_EMPTY, false);
	SOURCEPP_EARLY_RETURN_VAL(width, false);
	SOURCEPP_EARLY_RETURN_VAL(height, false);

	return convert::handle<PPL>(handle)->setImage({reinterpret_cast<const std::byte*>(buffer), bufferLen}, convert::cast(format), width, height, lod, quality);
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

	return convert::handle<PPL>(handle)->setImage({reinterpret_cast<const std::byte*>(buffer), bufferLen}, convert::cast(format), width, height, resizedWidth, resizedHeight, lod, convert::cast(filter), quality);
}

SOURCEPP_API int vtfpp_ppl_save_image_to_file(vtfpp_ppl_handle_t handle, const char* imagePath, uint32_t lod, vtfpp_image_conversion_file_format_e fileFormat) {
	SOURCEPP_EARLY_RETURN_VAL(handle, false);
	SOURCEPP_EARLY_RETURN_VAL(imagePath, false);

	return convert::handle<PPL>(handle)->saveImageToFile(imagePath, lod, convert::cast(fileFormat));
}

SOURCEPP_API sourcepp_buffer_t vtfpp_ppl_bake(vtfpp_ppl_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, SOURCEPP_BUFFER_INVALID);

	return convert::toBuffer(convert::handle<PPL>(handle)->bake());
}

SOURCEPP_API int vtfpp_ppl_bake_to_file(vtfpp_ppl_handle_t handle, const char* pplPath) {
	SOURCEPP_EARLY_RETURN_VAL(handle, false);
	SOURCEPP_EARLY_RETURN_VAL(pplPath, false);

	return convert::handle<PPL>(handle)->bake(pplPath);
}
