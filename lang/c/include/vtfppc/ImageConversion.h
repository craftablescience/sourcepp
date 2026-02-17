#pragma once

#include <sourceppc/API.h>
#include <sourceppc/Buffer.h>

#include "ImageFormats.h"

// Skipping vtfpp::ImagePixel, not super useful

SOURCEPP_EXTERNVAR const float VTFPP_IMAGE_CONVERSION_DEFAULT_COMPRESSED_QUALITY;

SOURCEPP_API sourcepp_buffer_t vtfpp_image_conversion_convert_image_data_to_format(const unsigned char* buffer, size_t bufferLen, vtfpp_image_format_e oldFormat, vtfpp_image_format_e newFormat, uint16_t width, uint16_t height, float quality); // REQUIRES MANUAL FREE: sourcepp_buffer_free
SOURCEPP_API sourcepp_buffer_t vtfpp_convert_several_image_data_to_format(const unsigned char* buffer, size_t bufferLen, vtfpp_image_format_e oldFormat, vtfpp_image_format_e newFormat, uint8_t mipCount, uint16_t frameCount, uint8_t faceCount, uint16_t width, uint16_t height, uint16_t depth, float quality); // REQUIRES MANUAL FREE: sourcepp_buffer_free
SOURCEPP_API sourcepp_buffer_t vtfpp_image_conversion_convert_hdri_to_cubemap(const unsigned char* buffer, size_t bufferLen, vtfpp_image_format_e format, uint16_t width, uint16_t height); // REQUIRES MANUAL FREE: sourcepp_buffer_free
SOURCEPP_API sourcepp_buffer_t vtfpp_image_conversion_convert_hdri_to_cubemap_ex(const unsigned char* buffer, size_t bufferLen, vtfpp_image_format_e format, uint16_t width, uint16_t height, uint16_t resolution, int bilinear); // REQUIRES MANUAL FREE: sourcepp_buffer_free

SOURCEPP_EXTERN typedef enum {
	VTFPP_IMAGE_CONVERSION_FILE_FORMAT_DEFAULT = 0,
	VTFPP_IMAGE_CONVERSION_FILE_FORMAT_PNG     = 1,
	VTFPP_IMAGE_CONVERSION_FILE_FORMAT_JPG     = 2,
	VTFPP_IMAGE_CONVERSION_FILE_FORMAT_BMP     = 3,
	VTFPP_IMAGE_CONVERSION_FILE_FORMAT_TGA     = 4,
#ifdef VTFPP_SUPPORT_WEBP
	VTFPP_IMAGE_CONVERSION_FILE_FORMAT_WEBP    = 5,
#endif
#ifdef VTFPP_SUPPORT_QOI
	VTFPP_IMAGE_CONVERSION_FILE_FORMAT_QOI     = 6,
#endif
	VTFPP_IMAGE_CONVERSION_FILE_FORMAT_HDR     = 7,
#ifdef VTFPP_SUPPORT_EXR
	VTFPP_IMAGE_CONVERSION_FILE_FORMAT_EXR     = 8,
#endif
} vtfpp_image_conversion_file_format_e;

SOURCEPP_API vtfpp_image_conversion_file_format_e vtfpp_image_conversion_get_default_file_format_for_image_format(vtfpp_image_format_e format);
SOURCEPP_API sourcepp_buffer_t vtfpp_image_conversion_convert_image_data_to_file(const unsigned char* buffer, size_t bufferLen, vtfpp_image_format_e format, uint16_t width, uint16_t height, vtfpp_image_conversion_file_format_e fileFormat); // REQUIRES MANUAL FREE: sourcepp_buffer_free
SOURCEPP_API sourcepp_buffer_t vtfpp_image_conversion_convert_file_to_image_data(const unsigned char* buffer, size_t bufferLen, vtfpp_image_format_e* format, int* width, int* height, int* frameCount); // REQUIRES MANUAL FREE: sourcepp_buffer_free

SOURCEPP_EXTERN typedef enum {
	VTFPP_IMAGE_CONVERSION_RESIZE_EDGE_CLAMP = 0,
	VTFPP_IMAGE_CONVERSION_RESIZE_EDGE_REFLECT,
	VTFPP_IMAGE_CONVERSION_RESIZE_EDGE_WRAP,
	VTFPP_IMAGE_CONVERSION_RESIZE_EDGE_ZERO,
} vtfpp_image_conversion_resize_edge_e;

SOURCEPP_EXTERN typedef enum {
	VTFPP_IMAGE_CONVERSION_RESIZE_FILTER_DEFAULT = 0,
	VTFPP_IMAGE_CONVERSION_RESIZE_FILTER_BOX,
	VTFPP_IMAGE_CONVERSION_RESIZE_FILTER_BILINEAR,
	VTFPP_IMAGE_CONVERSION_RESIZE_FILTER_CUBIC_BSPLINE,
	VTFPP_IMAGE_CONVERSION_RESIZE_FILTER_CATMULL_ROM,
	VTFPP_IMAGE_CONVERSION_RESIZE_FILTER_MITCHELL,
	VTFPP_IMAGE_CONVERSION_RESIZE_FILTER_POINT_SAMPLE,
	VTFPP_IMAGE_CONVERSION_RESIZE_FILTER_KAISER = 100,
	VTFPP_IMAGE_CONVERSION_RESIZE_FILTER_NICE = 101,
} vtfpp_image_conversion_resize_filter_e;

SOURCEPP_EXTERN typedef enum {
	VTFPP_IMAGE_CONVERSION_RESIZE_METHOD_NONE,
	VTFPP_IMAGE_CONVERSION_RESIZE_METHOD_POWER_OF_TWO_BIGGER,
	VTFPP_IMAGE_CONVERSION_RESIZE_METHOD_POWER_OF_TWO_SMALLER,
	VTFPP_IMAGE_CONVERSION_RESIZE_METHOD_POWER_OF_TWO_NEAREST,
} vtfpp_image_conversion_resize_method_e;

SOURCEPP_API uint16_t vtfpp_image_conversion_get_resized_dim(uint16_t n, vtfpp_image_conversion_resize_method_e method);
SOURCEPP_API void vtfpp_image_conversion_set_resized_dims(uint16_t* width, vtfpp_image_conversion_resize_method_e widthResize, uint16_t* height, vtfpp_image_conversion_resize_method_e heightResize);
SOURCEPP_API sourcepp_buffer_t vtfpp_image_conversion_resize_image_data(const unsigned char* buffer, size_t bufferLen, vtfpp_image_format_e format, uint16_t width, uint16_t newWidth, uint16_t height, uint16_t newHeight, int srgb, vtfpp_image_conversion_resize_filter_e filter, vtfpp_image_conversion_resize_edge_e edge); // REQUIRES MANUAL FREE: sourcepp_buffer_free
SOURCEPP_API sourcepp_buffer_t vtfpp_image_conversion_resize_image_data_strict(const unsigned char* buffer, size_t bufferLen, vtfpp_image_format_e format, uint16_t width, uint16_t newWidth, uint16_t* widthOut, vtfpp_image_conversion_resize_method_e widthResize, uint16_t height, uint16_t newHeight, uint16_t* heightOut, vtfpp_image_conversion_resize_method_e heightResize, int srgb, vtfpp_image_conversion_resize_filter_e filter, vtfpp_image_conversion_resize_edge_e edge); // REQUIRES MANUAL FREE: sourcepp_buffer_free
SOURCEPP_API sourcepp_buffer_t vtfpp_image_conversion_crop_image_data(const unsigned char* buffer, size_t bufferLen, vtfpp_image_format_e format, uint16_t width, uint16_t newWidth, uint16_t xOffset, uint16_t height, uint16_t newHeight, uint16_t yOffset); // REQUIRES MANUAL FREE: sourcepp_buffer_free
SOURCEPP_API sourcepp_buffer_t vtfpp_image_conversion_gamma_correct_image_data(const unsigned char* buffer, size_t bufferLen, vtfpp_image_format_e format, uint16_t width, uint16_t height, float gamma); // REQUIRES MANUAL FREE: sourcepp_buffer_free
SOURCEPP_API sourcepp_buffer_t vtfpp_image_conversion_invert_green_channel_for_image_data(const unsigned char* buffer, size_t bufferLen, vtfpp_image_format_e format, uint16_t width, uint16_t height); // REQUIRES MANUAL FREE: sourcepp_buffer_free

// Skipping vtfpp::ImageConversion::extractChannelFromImageData, requires ImagePixel types

// C++ conversion routines
#ifdef __cplusplus

#include <vtfpp/ImageConversion.h>

namespace sourceppc::convert {

inline vtfpp::ImageConversion::FileFormat cast(vtfpp_image_conversion_file_format_e value) {
	switch (value) {
		case VTFPP_IMAGE_CONVERSION_FILE_FORMAT_DEFAULT: return vtfpp::ImageConversion::FileFormat::DEFAULT;
		case VTFPP_IMAGE_CONVERSION_FILE_FORMAT_PNG:     return vtfpp::ImageConversion::FileFormat::PNG;
		case VTFPP_IMAGE_CONVERSION_FILE_FORMAT_JPG:     return vtfpp::ImageConversion::FileFormat::JPG;
		case VTFPP_IMAGE_CONVERSION_FILE_FORMAT_BMP:     return vtfpp::ImageConversion::FileFormat::BMP;
		case VTFPP_IMAGE_CONVERSION_FILE_FORMAT_TGA:     return vtfpp::ImageConversion::FileFormat::TGA;
#ifdef VTFPP_SUPPORT_WEBP
		case VTFPP_IMAGE_CONVERSION_FILE_FORMAT_WEBP:    return vtfpp::ImageConversion::FileFormat::WEBP;
#endif
#ifdef VTFPP_SUPPORT_QOI
		case VTFPP_IMAGE_CONVERSION_FILE_FORMAT_QOI:     return vtfpp::ImageConversion::FileFormat::QOI;
#endif
		case VTFPP_IMAGE_CONVERSION_FILE_FORMAT_HDR:     return vtfpp::ImageConversion::FileFormat::HDR;
#ifdef VTFPP_SUPPORT_EXR
		case VTFPP_IMAGE_CONVERSION_FILE_FORMAT_EXR:     return vtfpp::ImageConversion::FileFormat::EXR;
#endif
	}
	return vtfpp::ImageConversion::FileFormat::DEFAULT;
}

inline vtfpp_image_conversion_file_format_e cast(vtfpp::ImageConversion::FileFormat value) {
	switch (value) {
		case vtfpp::ImageConversion::FileFormat::DEFAULT: return VTFPP_IMAGE_CONVERSION_FILE_FORMAT_DEFAULT;
		case vtfpp::ImageConversion::FileFormat::PNG:     return VTFPP_IMAGE_CONVERSION_FILE_FORMAT_PNG;
		case vtfpp::ImageConversion::FileFormat::JPG:     return VTFPP_IMAGE_CONVERSION_FILE_FORMAT_JPG;
		case vtfpp::ImageConversion::FileFormat::BMP:     return VTFPP_IMAGE_CONVERSION_FILE_FORMAT_BMP;
		case vtfpp::ImageConversion::FileFormat::TGA:     return VTFPP_IMAGE_CONVERSION_FILE_FORMAT_TGA;
#ifdef VTFPP_SUPPORT_WEBP
		case vtfpp::ImageConversion::FileFormat::WEBP:    return VTFPP_IMAGE_CONVERSION_FILE_FORMAT_WEBP;
#endif
#ifdef VTFPP_SUPPORT_QOI
		case vtfpp::ImageConversion::FileFormat::QOI:     return VTFPP_IMAGE_CONVERSION_FILE_FORMAT_QOI;
#endif
		case vtfpp::ImageConversion::FileFormat::HDR:     return VTFPP_IMAGE_CONVERSION_FILE_FORMAT_HDR;
#ifdef VTFPP_SUPPORT_EXR
		case vtfpp::ImageConversion::FileFormat::EXR:     return VTFPP_IMAGE_CONVERSION_FILE_FORMAT_EXR;
#endif
	}
	return VTFPP_IMAGE_CONVERSION_FILE_FORMAT_DEFAULT;
}

inline vtfpp::ImageConversion::ResizeEdge cast(vtfpp_image_conversion_resize_edge_e value) {
	switch (value) {
		case VTFPP_IMAGE_CONVERSION_RESIZE_EDGE_CLAMP:   return vtfpp::ImageConversion::ResizeEdge::CLAMP;
		case VTFPP_IMAGE_CONVERSION_RESIZE_EDGE_REFLECT: return vtfpp::ImageConversion::ResizeEdge::REFLECT;
		case VTFPP_IMAGE_CONVERSION_RESIZE_EDGE_WRAP:    return vtfpp::ImageConversion::ResizeEdge::WRAP;
		case VTFPP_IMAGE_CONVERSION_RESIZE_EDGE_ZERO:    return vtfpp::ImageConversion::ResizeEdge::ZERO;
	}
	return vtfpp::ImageConversion::ResizeEdge::CLAMP;
}

inline vtfpp_image_conversion_resize_edge_e cast(vtfpp::ImageConversion::ResizeEdge value) {
	switch (value) {
		case vtfpp::ImageConversion::ResizeEdge::CLAMP:   return VTFPP_IMAGE_CONVERSION_RESIZE_EDGE_CLAMP;
		case vtfpp::ImageConversion::ResizeEdge::REFLECT: return VTFPP_IMAGE_CONVERSION_RESIZE_EDGE_REFLECT;
		case vtfpp::ImageConversion::ResizeEdge::WRAP:    return VTFPP_IMAGE_CONVERSION_RESIZE_EDGE_WRAP;
		case vtfpp::ImageConversion::ResizeEdge::ZERO:    return VTFPP_IMAGE_CONVERSION_RESIZE_EDGE_ZERO;
	}
	return VTFPP_IMAGE_CONVERSION_RESIZE_EDGE_CLAMP;
}

inline vtfpp::ImageConversion::ResizeFilter cast(vtfpp_image_conversion_resize_filter_e value) {
	switch (value) {
		case VTFPP_IMAGE_CONVERSION_RESIZE_FILTER_DEFAULT:       return vtfpp::ImageConversion::ResizeFilter::DEFAULT;
		case VTFPP_IMAGE_CONVERSION_RESIZE_FILTER_BOX:           return vtfpp::ImageConversion::ResizeFilter::BOX;
		case VTFPP_IMAGE_CONVERSION_RESIZE_FILTER_BILINEAR:      return vtfpp::ImageConversion::ResizeFilter::BILINEAR;
		case VTFPP_IMAGE_CONVERSION_RESIZE_FILTER_CUBIC_BSPLINE: return vtfpp::ImageConversion::ResizeFilter::CUBIC_BSPLINE;
		case VTFPP_IMAGE_CONVERSION_RESIZE_FILTER_CATMULL_ROM:   return vtfpp::ImageConversion::ResizeFilter::CATMULL_ROM;
		case VTFPP_IMAGE_CONVERSION_RESIZE_FILTER_MITCHELL:      return vtfpp::ImageConversion::ResizeFilter::MITCHELL;
		case VTFPP_IMAGE_CONVERSION_RESIZE_FILTER_POINT_SAMPLE:  return vtfpp::ImageConversion::ResizeFilter::POINT_SAMPLE;
		case VTFPP_IMAGE_CONVERSION_RESIZE_FILTER_KAISER:        return vtfpp::ImageConversion::ResizeFilter::KAISER;
		case VTFPP_IMAGE_CONVERSION_RESIZE_FILTER_NICE:	         return vtfpp::ImageConversion::ResizeFilter::NICE;
	}
	return vtfpp::ImageConversion::ResizeFilter::DEFAULT;
}

inline vtfpp_image_conversion_resize_filter_e cast(vtfpp::ImageConversion::ResizeFilter value) {
	switch (value) {
		case vtfpp::ImageConversion::ResizeFilter::DEFAULT:       return VTFPP_IMAGE_CONVERSION_RESIZE_FILTER_DEFAULT;
		case vtfpp::ImageConversion::ResizeFilter::BOX:           return VTFPP_IMAGE_CONVERSION_RESIZE_FILTER_BOX;
		case vtfpp::ImageConversion::ResizeFilter::BILINEAR:      return VTFPP_IMAGE_CONVERSION_RESIZE_FILTER_BILINEAR;
		case vtfpp::ImageConversion::ResizeFilter::CUBIC_BSPLINE: return VTFPP_IMAGE_CONVERSION_RESIZE_FILTER_CUBIC_BSPLINE;
		case vtfpp::ImageConversion::ResizeFilter::CATMULL_ROM:   return VTFPP_IMAGE_CONVERSION_RESIZE_FILTER_CATMULL_ROM;
		case vtfpp::ImageConversion::ResizeFilter::MITCHELL:      return VTFPP_IMAGE_CONVERSION_RESIZE_FILTER_MITCHELL;
		case vtfpp::ImageConversion::ResizeFilter::POINT_SAMPLE:  return VTFPP_IMAGE_CONVERSION_RESIZE_FILTER_POINT_SAMPLE;
		case vtfpp::ImageConversion::ResizeFilter::KAISER:        return VTFPP_IMAGE_CONVERSION_RESIZE_FILTER_KAISER;
		case vtfpp::ImageConversion::ResizeFilter::NICE:          return VTFPP_IMAGE_CONVERSION_RESIZE_FILTER_NICE;
	}
	return VTFPP_IMAGE_CONVERSION_RESIZE_FILTER_DEFAULT;
}

inline vtfpp::ImageConversion::ResizeMethod cast(vtfpp_image_conversion_resize_method_e value) {
	switch (value) {
		case VTFPP_IMAGE_CONVERSION_RESIZE_METHOD_NONE:                 return vtfpp::ImageConversion::ResizeMethod::NONE;
		case VTFPP_IMAGE_CONVERSION_RESIZE_METHOD_POWER_OF_TWO_BIGGER:  return vtfpp::ImageConversion::ResizeMethod::POWER_OF_TWO_BIGGER;
		case VTFPP_IMAGE_CONVERSION_RESIZE_METHOD_POWER_OF_TWO_SMALLER: return vtfpp::ImageConversion::ResizeMethod::POWER_OF_TWO_SMALLER;
		case VTFPP_IMAGE_CONVERSION_RESIZE_METHOD_POWER_OF_TWO_NEAREST: return vtfpp::ImageConversion::ResizeMethod::POWER_OF_TWO_NEAREST;
	}
	return vtfpp::ImageConversion::ResizeMethod::NONE;
}

inline vtfpp_image_conversion_resize_method_e cast(vtfpp::ImageConversion::ResizeMethod value) {
	switch (value) {
		case vtfpp::ImageConversion::ResizeMethod::NONE:                 return VTFPP_IMAGE_CONVERSION_RESIZE_METHOD_NONE;
		case vtfpp::ImageConversion::ResizeMethod::POWER_OF_TWO_BIGGER:  return VTFPP_IMAGE_CONVERSION_RESIZE_METHOD_POWER_OF_TWO_BIGGER;
		case vtfpp::ImageConversion::ResizeMethod::POWER_OF_TWO_SMALLER: return VTFPP_IMAGE_CONVERSION_RESIZE_METHOD_POWER_OF_TWO_SMALLER;
		case vtfpp::ImageConversion::ResizeMethod::POWER_OF_TWO_NEAREST: return VTFPP_IMAGE_CONVERSION_RESIZE_METHOD_POWER_OF_TWO_NEAREST;
	}
	return VTFPP_IMAGE_CONVERSION_RESIZE_METHOD_NONE;
}

} // namespace sourceppc::convert

#endif
