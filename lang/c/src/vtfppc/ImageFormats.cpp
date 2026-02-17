#include <vtfppc/ImageFormats.h>

#include <vtfpp/ImageFormats.h>

#include <sourceppc/Helpers.h>

using namespace sourceppc;
using namespace vtfpp;

SOURCEPP_API int8_t vtfpp_image_format_details_red(vtfpp_image_format_e format) {
	return ImageFormatDetails::red(convert::cast(format));
}

SOURCEPP_API int8_t vtfpp_image_format_details_decompressed_red(vtfpp_image_format_e format) {
	return ImageFormatDetails::decompressedRed(convert::cast(format));
}

SOURCEPP_API int8_t vtfpp_image_format_details_green(vtfpp_image_format_e format) {
	return ImageFormatDetails::green(convert::cast(format));
}

SOURCEPP_API int8_t vtfpp_image_format_details_decompressed_green(vtfpp_image_format_e format) {
	return ImageFormatDetails::decompressedGreen(convert::cast(format));
}

SOURCEPP_API int8_t vtfpp_image_format_details_blue(vtfpp_image_format_e format) {
	return ImageFormatDetails::blue(convert::cast(format));
}

SOURCEPP_API int8_t vtfpp_image_format_details_decompressed_blue(vtfpp_image_format_e format) {
	return ImageFormatDetails::decompressedBlue(convert::cast(format));
}

SOURCEPP_API int8_t vtfpp_image_format_details_alpha(vtfpp_image_format_e format) {
	return ImageFormatDetails::alpha(convert::cast(format));
}

SOURCEPP_API int8_t vtfpp_image_format_details_decompressed_alpha(vtfpp_image_format_e format) {
	return ImageFormatDetails::decompressedAlpha(convert::cast(format));
}

SOURCEPP_API uint8_t vtfpp_image_format_details_bpp(vtfpp_image_format_e format) {
	return ImageFormatDetails::bpp(convert::cast(format));
}

SOURCEPP_API vtfpp_image_format_e vtfpp_image_format_details_container_format(vtfpp_image_format_e format) {
	return static_cast<vtfpp_image_format_e>(ImageFormatDetails::containerFormat(convert::cast(format)));
}

SOURCEPP_API int vtfpp_image_format_details_large(vtfpp_image_format_e format) {
	return ImageFormatDetails::large(convert::cast(format));
}

SOURCEPP_API int vtfpp_image_format_details_decimal(vtfpp_image_format_e format) {
	return ImageFormatDetails::decimal(convert::cast(format));
}

SOURCEPP_API int vtfpp_image_format_details_compressed(vtfpp_image_format_e format) {
	return ImageFormatDetails::compressed(convert::cast(format));
}

SOURCEPP_API int vtfpp_image_format_details_transparent(vtfpp_image_format_e format) {
	return ImageFormatDetails::transparent(convert::cast(format));
}

SOURCEPP_API int vtfpp_image_format_details_opaque(vtfpp_image_format_e format) {
	return ImageFormatDetails::opaque(convert::cast(format));
}

SOURCEPP_API int vtfpp_image_format_details_console(vtfpp_image_format_e format) {
	return ImageFormatDetails::console(convert::cast(format));
}

SOURCEPP_API uint16_t vtfpp_image_dimensions_get_mip_dim(uint8_t mip, uint16_t dim, int add_compressed_format_padding) {
	return ImageDimensions::getMipDim(mip, dim, add_compressed_format_padding);
}

SOURCEPP_API uint8_t vtfpp_image_dimensions_get_maximum_mip_count(uint16_t width, uint16_t height, uint16_t depth) {
	return ImageDimensions::getMaximumMipCount(width, height, depth);
}

SOURCEPP_API uint32_t vtfpp_image_format_details_get_data_length(vtfpp_image_format_e format, uint16_t width, uint16_t height, uint16_t depth) {
	return ImageFormatDetails::getDataLength(convert::cast(format), width, height, depth);
}

SOURCEPP_API uint32_t vtfpp_image_format_details_get_data_length_ex(vtfpp_image_format_e format, uint8_t mipCount, uint16_t frameCount, uint8_t faceCount, uint16_t width, uint16_t height, uint16_t depth) {
	return ImageFormatDetails::getDataLength(convert::cast(format), mipCount, frameCount, faceCount, width, height, depth);
}

SOURCEPP_API uint32_t vtfpp_image_format_details_get_data_length_xbox(int padded, vtfpp_image_format_e format, uint8_t mipCount, uint16_t frameCount, uint8_t faceCount, uint16_t width, uint16_t height, uint16_t depth) {
	return ImageFormatDetails::getDataLengthXBOX(padded, convert::cast(format), mipCount, frameCount, faceCount, width, height, depth);
}

SOURCEPP_API int vtfpp_image_format_details_get_data_position(uint32_t* offset, uint32_t* length, vtfpp_image_format_e format, uint8_t mip, uint8_t mipCount, uint16_t frame, uint16_t frameCount, uint8_t face, uint8_t faceCount, uint16_t width, uint16_t height, uint16_t slice, uint16_t depth) {
	SOURCEPP_EARLY_RETURN_VAL(offset, -1);
	SOURCEPP_EARLY_RETURN_VAL(length, -1);

	return ImageFormatDetails::getDataPosition(*offset, *length, convert::cast(format), mip, mipCount, frame, frameCount, face, faceCount, width, height, slice, depth);
}

SOURCEPP_API int vtfpp_image_format_details_get_data_position_xbox(uint32_t* offset, uint32_t* length, int padded, vtfpp_image_format_e format, uint8_t mip, uint8_t mipCount, uint16_t frame, uint16_t frameCount, uint8_t face, uint8_t faceCount, uint16_t width, uint16_t height, uint16_t slice, uint16_t depth) {
	SOURCEPP_EARLY_RETURN_VAL(offset, -1);
	SOURCEPP_EARLY_RETURN_VAL(length, -1);

	return ImageFormatDetails::getDataPositionXBOX(*offset, *length, padded, convert::cast(format), mip, mipCount, frame, frameCount, face, faceCount, width, height, slice, depth);
}
