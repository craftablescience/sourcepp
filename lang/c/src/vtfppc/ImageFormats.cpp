#include <vtfppc/ImageFormats.h>

#include <vtfpp/ImageFormats.h>

#include <sourceppc/Helpers.h>

using namespace vtfpp;

SOURCEPP_API int8_t vtfpp_image_format_details_red(vtfpp_image_format_e format) {
	return ImageFormatDetails::red(static_cast<ImageFormat>(format));
}

SOURCEPP_API int8_t vtfpp_image_format_details_decompressed_red(vtfpp_image_format_e format) {
	return ImageFormatDetails::decompressedRed(static_cast<ImageFormat>(format));
}

SOURCEPP_API int8_t vtfpp_image_format_details_green(vtfpp_image_format_e format) {
	return ImageFormatDetails::green(static_cast<ImageFormat>(format));
}

SOURCEPP_API int8_t vtfpp_image_format_details_decompressed_green(vtfpp_image_format_e format) {
	return ImageFormatDetails::decompressedGreen(static_cast<ImageFormat>(format));
}

SOURCEPP_API int8_t vtfpp_image_format_details_blue(vtfpp_image_format_e format) {
	return ImageFormatDetails::blue(static_cast<ImageFormat>(format));
}

SOURCEPP_API int8_t vtfpp_image_format_details_decompressed_blue(vtfpp_image_format_e format) {
	return ImageFormatDetails::decompressedBlue(static_cast<ImageFormat>(format));
}

SOURCEPP_API int8_t vtfpp_image_format_details_alpha(vtfpp_image_format_e format) {
	return ImageFormatDetails::alpha(static_cast<ImageFormat>(format));
}

SOURCEPP_API int8_t vtfpp_image_format_details_decompressed_alpha(vtfpp_image_format_e format) {
	return ImageFormatDetails::decompressedAlpha(static_cast<ImageFormat>(format));
}

SOURCEPP_API int8_t vtfpp_image_format_details_bpp(vtfpp_image_format_e format) {
	return ImageFormatDetails::bpp(static_cast<ImageFormat>(format));
}

SOURCEPP_API vtfpp_image_format_e vtfpp_image_format_details_container_format(vtfpp_image_format_e format) {
	return static_cast<vtfpp_image_format_e>(ImageFormatDetails::containerFormat(static_cast<ImageFormat>(format)));
}

SOURCEPP_API int vtfpp_image_format_details_large(vtfpp_image_format_e format) {
	return ImageFormatDetails::large(static_cast<ImageFormat>(format));
}

SOURCEPP_API int vtfpp_image_format_details_decimal(vtfpp_image_format_e format) {
	return ImageFormatDetails::decimal(static_cast<ImageFormat>(format));
}

SOURCEPP_API int vtfpp_image_format_details_compressed(vtfpp_image_format_e format) {
	return ImageFormatDetails::compressed(static_cast<ImageFormat>(format));
}

SOURCEPP_API int vtfpp_image_format_details_transparent(vtfpp_image_format_e format) {
	return ImageFormatDetails::transparent(static_cast<ImageFormat>(format));
}

SOURCEPP_API int vtfpp_image_format_details_opaque(vtfpp_image_format_e format) {
	return ImageFormatDetails::opaque(static_cast<ImageFormat>(format));
}

SOURCEPP_API int vtfpp_image_format_details_console(vtfpp_image_format_e format) {
	return ImageFormatDetails::console(static_cast<ImageFormat>(format));
}

SOURCEPP_API uint32_t vtfpp_image_dimensions_get_mip_dim(uint8_t mip, uint16_t dim) {
	return ImageDimensions::getMipDim(mip, dim);
}

SOURCEPP_API uint8_t vtfpp_image_dimensions_get_recommended_mip_count_for_dim(vtfpp_image_format_e format, uint16_t width, uint16_t height) {
	return ImageDimensions::getRecommendedMipCountForDims(static_cast<ImageFormat>(format), width, height);
}

SOURCEPP_API uint8_t vtfpp_image_dimensions_get_actual_mip_count_for_dims_on_console(uint16_t width, uint16_t height, uint16_t depth) {
	return ImageDimensions::getActualMipCountForDimsOnConsole(width, height, depth);
}

SOURCEPP_API uint32_t vtfpp_image_format_details_get_data_length(vtfpp_image_format_e format, uint16_t width, uint16_t height, uint16_t depth) {
	return ImageFormatDetails::getDataLength(static_cast<ImageFormat>(format), width, height, depth);
}

SOURCEPP_API uint32_t vtfpp_image_format_details_get_data_length_ex(vtfpp_image_format_e format, uint8_t mipCount, uint16_t frameCount, uint8_t faceCount, uint16_t width, uint16_t height, uint16_t depth) {
	return ImageFormatDetails::getDataLength(static_cast<ImageFormat>(format), mipCount, frameCount, faceCount, width, height, depth);
}

SOURCEPP_API int vtfpp_image_format_details_get_data_position(uint32_t* offset, uint32_t* length, vtfpp_image_format_e format, uint8_t mip, uint8_t mipCount, uint16_t frame, uint16_t frameCount, uint8_t face, uint8_t faceCount, uint16_t width, uint16_t height, uint16_t slice, uint16_t depth) {
	SOURCEPP_EARLY_RETURN_VAL(offset, -1);
	SOURCEPP_EARLY_RETURN_VAL(length, -1);

	return ImageFormatDetails::getDataPosition(*offset, *length, static_cast<ImageFormat>(format), mip, mipCount, frame, frameCount, face, faceCount, width, height, slice, depth);
}
