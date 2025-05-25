#pragma once

#include <sourceppc/API.h>
#include <sourceppc/Buffer.h>

#include "ImageFormats.h"

#ifdef __cplusplus
extern "C" {
#endif

// Skipping vtfpp::ImagePixel, not super useful

typedef enum {
	VTFPP_IMAGE_CONVERSION_FILE_FORMAT_DEFAULT,
	VTFPP_IMAGE_CONVERSION_FILE_FORMAT_PNG,
	VTFPP_IMAGE_CONVERSION_FILE_FORMAT_JPG,
	VTFPP_IMAGE_CONVERSION_FILE_FORMAT_BMP,
	VTFPP_IMAGE_CONVERSION_FILE_FORMAT_TGA,
	VTFPP_IMAGE_CONVERSION_FILE_FORMAT_QOI,
	VTFPP_IMAGE_CONVERSION_FILE_FORMAT_HDR,
	VTFPP_IMAGE_CONVERSION_FILE_FORMAT_EXR,
} vtfpp_image_conversion_file_format_e;

typedef enum {
	VTFPP_IMAGE_CONVERSION_RESIZE_EDGE_CLAMP,
	VTFPP_IMAGE_CONVERSION_RESIZE_EDGE_REFLECT,
	VTFPP_IMAGE_CONVERSION_RESIZE_EDGE_WRAP,
	VTFPP_IMAGE_CONVERSION_RESIZE_EDGE_ZERO,
} vtfpp_image_conversion_resize_edge_e;

typedef enum {
	VTFPP_IMAGE_CONVERSION_RESIZE_FILTER_DEFAULT = 0,
	VTFPP_IMAGE_CONVERSION_RESIZE_FILTER_BOX,
	VTFPP_IMAGE_CONVERSION_RESIZE_FILTER_BILINEAR,
	VTFPP_IMAGE_CONVERSION_RESIZE_FILTER_CUBIC_BSPLINE,
	VTFPP_IMAGE_CONVERSION_RESIZE_FILTER_CATMULL_ROM,
	VTFPP_IMAGE_CONVERSION_RESIZE_FILTER_MITCHELL,
	VTFPP_IMAGE_CONVERSION_RESIZE_FILTER_POINT_SAMPLE,
	VTFPP_IMAGE_CONVERSION_RESIZE_FILTER_KAISER = 100,
} vtfpp_image_conversion_resize_filter_e;

typedef enum {
	VTFPP_IMAGE_CONVERSION_RESIZE_METHOD_NONE,
	VTFPP_IMAGE_CONVERSION_RESIZE_METHOD_POWER_OF_TWO_BIGGER,
	VTFPP_IMAGE_CONVERSION_RESIZE_METHOD_POWER_OF_TWO_SMALLER,
	VTFPP_IMAGE_CONVERSION_RESIZE_METHOD_POWER_OF_TWO_NEAREST,
} vtfpp_image_conversion_resize_method_e;

#ifdef __cplusplus
} // extern "C"
#endif

// REQUIRES MANUAL FREE: sourcepp_buffer_free
SOURCEPP_API sourcepp_buffer_t vtfpp_image_conversion_convert_image_data_to_format(const unsigned char* buffer, size_t bufferLen, vtfpp_image_format_e oldFormat, vtfpp_image_format_e newFormat, uint16_t width, uint16_t height);

// REQUIRES MANUAL FREE: sourcepp_buffer_free
SOURCEPP_API sourcepp_buffer_t vtfpp_convert_several_image_data_to_format(const unsigned char* buffer, size_t bufferLen, vtfpp_image_format_e oldFormat, vtfpp_image_format_e newFormat, uint8_t mipCount, uint16_t frameCount, uint8_t faceCount, uint16_t width, uint16_t height, uint16_t sliceCount);

// REQUIRES MANUAL FREE: sourcepp_buffer_free
SOURCEPP_API sourcepp_buffer_t vtfpp_image_conversion_convert_hdri_to_cubemap(const unsigned char* buffer, size_t bufferLen, vtfpp_image_format_e format, uint16_t width, uint16_t height);

// REQUIRES MANUAL FREE: sourcepp_buffer_free
SOURCEPP_API sourcepp_buffer_t vtfpp_image_conversion_convert_hdri_to_cubemap_ex(const unsigned char* buffer, size_t bufferLen, vtfpp_image_format_e format, uint16_t width, uint16_t height, uint16_t resolution, int bilinear);

SOURCEPP_API vtfpp_image_conversion_file_format_e vtfpp_image_conversion_get_default_file_format_for_image_format(vtfpp_image_format_e format);

// REQUIRES MANUAL FREE: sourcepp_buffer_free
SOURCEPP_API sourcepp_buffer_t vtfpp_image_conversion_convert_image_data_to_file(const unsigned char* buffer, size_t bufferLen, vtfpp_image_format_e format, uint16_t width, uint16_t height, vtfpp_image_conversion_file_format_e fileFormat);

// REQUIRES MANUAL FREE: sourcepp_buffer_free
SOURCEPP_API sourcepp_buffer_t vtfpp_image_conversion_convert_file_to_image_data(const unsigned char* buffer, size_t bufferLen, vtfpp_image_format_e* format, int* width, int* height, int* frameCount);

SOURCEPP_API uint16_t vtfpp_image_conversion_get_resized_dim(uint16_t n, vtfpp_image_conversion_resize_method_e method);

SOURCEPP_API void vtfpp_image_conversion_set_resized_dims(uint16_t* width, vtfpp_image_conversion_resize_method_e widthResize, uint16_t* height, vtfpp_image_conversion_resize_method_e heightResize);

// REQUIRES MANUAL FREE: sourcepp_buffer_free
SOURCEPP_API sourcepp_buffer_t vtfpp_image_conversion_resize_image_data(const unsigned char* buffer, size_t bufferLen, vtfpp_image_format_e format, uint16_t width, uint16_t newWidth, uint16_t height, uint16_t newHeight, int srgb, vtfpp_image_conversion_resize_filter_e filter, vtfpp_image_conversion_resize_edge_e edge);

// REQUIRES MANUAL FREE: sourcepp_buffer_free
SOURCEPP_API sourcepp_buffer_t vtfpp_image_conversion_resize_image_data_strict(const unsigned char* buffer, size_t bufferLen, vtfpp_image_format_e format, uint16_t width, uint16_t newWidth, uint16_t* widthOut, vtfpp_image_conversion_resize_method_e widthResize, uint16_t height, uint16_t newHeight, uint16_t* heightOut, vtfpp_image_conversion_resize_method_e heightResize, int srgb, vtfpp_image_conversion_resize_filter_e filter, vtfpp_image_conversion_resize_edge_e edge);

// REQUIRES MANUAL FREE: sourcepp_buffer_free
SOURCEPP_API sourcepp_buffer_t vtfpp_image_conversion_crop_image_data(const unsigned char* buffer, size_t bufferLen, vtfpp_image_format_e format, uint16_t width, uint16_t newWidth, uint16_t xOffset, uint16_t height, uint16_t newHeight, uint16_t yOffset);

// REQUIRES MANUAL FREE: sourcepp_buffer_free
SOURCEPP_API sourcepp_buffer_t vtfpp_image_conversion_gamma_correct_image_data(const unsigned char* buffer, size_t bufferLen, vtfpp_image_format_e format, uint16_t width, uint16_t height, float gamma);

// REQUIRES MANUAL FREE: sourcepp_buffer_free
SOURCEPP_API sourcepp_buffer_t vtfpp_image_conversion_invert_green_channel_for_image_data(const unsigned char* buffer, size_t bufferLen, vtfpp_image_format_e format, uint16_t width, uint16_t height);
