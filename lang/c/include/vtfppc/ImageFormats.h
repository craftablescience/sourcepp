#pragma once

#include <sourceppc/API.h>
#include <sourceppc/Buffer.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
	VTFPP_IMAGE_FORMAT_RGBA8888 = 0,
	VTFPP_IMAGE_FORMAT_ABGR8888,
	VTFPP_IMAGE_FORMAT_RGB888,
	VTFPP_IMAGE_FORMAT_BGR888,
	VTFPP_IMAGE_FORMAT_RGB565,
	VTFPP_IMAGE_FORMAT_I8,
	VTFPP_IMAGE_FORMAT_IA88,
	VTFPP_IMAGE_FORMAT_P8,
	VTFPP_IMAGE_FORMAT_A8,
	VTFPP_IMAGE_FORMAT_RGB888_BLUESCREEN,
	VTFPP_IMAGE_FORMAT_BGR888_BLUESCREEN,
	VTFPP_IMAGE_FORMAT_ARGB8888,
	VTFPP_IMAGE_FORMAT_BGRA8888,
	VTFPP_IMAGE_FORMAT_DXT1,
	VTFPP_IMAGE_FORMAT_DXT3,
	VTFPP_IMAGE_FORMAT_DXT5,
	VTFPP_IMAGE_FORMAT_BGRX8888,
	VTFPP_IMAGE_FORMAT_BGR565,
	VTFPP_IMAGE_FORMAT_BGRX5551,
	VTFPP_IMAGE_FORMAT_BGRA4444,
	VTFPP_IMAGE_FORMAT_DXT1_ONE_BIT_ALPHA,
	VTFPP_IMAGE_FORMAT_BGRA5551,
	VTFPP_IMAGE_FORMAT_UV88,
	VTFPP_IMAGE_FORMAT_UVWQ8888,
	VTFPP_IMAGE_FORMAT_RGBA16161616F,
	VTFPP_IMAGE_FORMAT_RGBA16161616,
	VTFPP_IMAGE_FORMAT_UVLX8888,
	VTFPP_IMAGE_FORMAT_R32F,
	VTFPP_IMAGE_FORMAT_RGB323232F,
	VTFPP_IMAGE_FORMAT_RGBA32323232F,
	VTFPP_IMAGE_FORMAT_RG1616F,
	VTFPP_IMAGE_FORMAT_RG3232F,
	VTFPP_IMAGE_FORMAT_RGBX8888,
	VTFPP_IMAGE_FORMAT_EMPTY,
	VTFPP_IMAGE_FORMAT_ATI2N,
	VTFPP_IMAGE_FORMAT_ATI1N,
	VTFPP_IMAGE_FORMAT_RGBA1010102,
	VTFPP_IMAGE_FORMAT_BGRA1010102,
	VTFPP_IMAGE_FORMAT_R16F,

	VTFPP_IMAGE_FORMAT_CONSOLE_BGRX8888_LINEAR = 42,
	VTFPP_IMAGE_FORMAT_CONSOLE_RGBA8888_LINEAR,
	VTFPP_IMAGE_FORMAT_CONSOLE_ABGR8888_LINEAR,
	VTFPP_IMAGE_FORMAT_CONSOLE_ARGB8888_LINEAR,
	VTFPP_IMAGE_FORMAT_CONSOLE_BGRA8888_LINEAR,
	VTFPP_IMAGE_FORMAT_CONSOLE_RGB888_LINEAR,
	VTFPP_IMAGE_FORMAT_CONSOLE_BGR888_LINEAR,
	VTFPP_IMAGE_FORMAT_CONSOLE_BGRX5551_LINEAR,
	VTFPP_IMAGE_FORMAT_CONSOLE_I8_LINEAR,
	VTFPP_IMAGE_FORMAT_CONSOLE_RGBA16161616_LINEAR,
	VTFPP_IMAGE_FORMAT_CONSOLE_BGRX8888_LE,
	VTFPP_IMAGE_FORMAT_CONSOLE_BGRA8888_LE,

	VTFPP_IMAGE_FORMAT_R8 = 69,
	VTFPP_IMAGE_FORMAT_BC7,
	VTFPP_IMAGE_FORMAT_BC6H,
} vtfpp_image_format_e;

#ifdef __cplusplus
} // extern "C"
#endif

SOURCEPP_API int8_t vtfpp_image_format_details_red(vtfpp_image_format_e format);

SOURCEPP_API int8_t vtfpp_image_format_details_decompressed_red(vtfpp_image_format_e format);

SOURCEPP_API int8_t vtfpp_image_format_details_green(vtfpp_image_format_e format);

SOURCEPP_API int8_t vtfpp_image_format_details_decompressed_green(vtfpp_image_format_e format);

SOURCEPP_API int8_t vtfpp_image_format_details_blue(vtfpp_image_format_e format);

SOURCEPP_API int8_t vtfpp_image_format_details_decompressed_blue(vtfpp_image_format_e format);

SOURCEPP_API int8_t vtfpp_image_format_details_alpha(vtfpp_image_format_e format);

SOURCEPP_API int8_t vtfpp_image_format_details_decompressed_alpha(vtfpp_image_format_e format);

SOURCEPP_API int8_t vtfpp_image_format_details_bpp(vtfpp_image_format_e format);

SOURCEPP_API vtfpp_image_format_e vtfpp_image_format_details_container_format(vtfpp_image_format_e format);

SOURCEPP_API int vtfpp_image_format_details_large(vtfpp_image_format_e format);

SOURCEPP_API int vtfpp_image_format_details_decimal(vtfpp_image_format_e format);

SOURCEPP_API int vtfpp_image_format_details_compressed(vtfpp_image_format_e format);

SOURCEPP_API int vtfpp_image_format_details_transparent(vtfpp_image_format_e format);

SOURCEPP_API int vtfpp_image_format_details_opaque(vtfpp_image_format_e format);

SOURCEPP_API int vtfpp_image_format_details_console(vtfpp_image_format_e format);

SOURCEPP_API uint32_t vtfpp_image_dimensions_get_mip_dim(uint8_t mip, uint16_t dim);

SOURCEPP_API uint8_t vtfpp_image_dimensions_get_recommended_mip_count_for_dim(vtfpp_image_format_e format, uint16_t width, uint16_t height);

SOURCEPP_API uint8_t vtfpp_image_dimensions_get_actual_mip_count_for_dims_on_console(uint16_t width, uint16_t height, uint16_t depth);

SOURCEPP_API uint32_t vtfpp_image_format_details_get_data_length(vtfpp_image_format_e format, uint16_t width, uint16_t height, uint16_t depth);

SOURCEPP_API uint32_t vtfpp_image_format_details_get_data_length_ex(vtfpp_image_format_e format, uint8_t mipCount, uint16_t frameCount, uint8_t faceCount, uint16_t width, uint16_t height, uint16_t depth);

SOURCEPP_API int vtfpp_image_format_details_get_data_position(uint32_t* offset, uint32_t* length, vtfpp_image_format_e format, uint8_t mip, uint8_t mipCount, uint16_t frame, uint16_t frameCount, uint8_t face, uint8_t faceCount, uint16_t width, uint16_t height, uint16_t slice, uint16_t depth);
