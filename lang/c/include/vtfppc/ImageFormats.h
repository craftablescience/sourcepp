#pragma once

#include <sourceppc/API.h>

SOURCEPP_EXTERN typedef enum {
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

	VTFPP_IMAGE_FORMAT_TFALL2_BC6H = 66,
	VTFPP_IMAGE_FORMAT_TFALL2_BC7,

	VTFPP_IMAGE_FORMAT_STRATA_R8 = 69,
	VTFPP_IMAGE_FORMAT_STRATA_BC7,
	VTFPP_IMAGE_FORMAT_STRATA_BC6H,
} vtfpp_image_format_e;

SOURCEPP_API int8_t vtfpp_image_format_details_red(vtfpp_image_format_e format);
SOURCEPP_API int8_t vtfpp_image_format_details_decompressed_red(vtfpp_image_format_e format);
SOURCEPP_API int8_t vtfpp_image_format_details_green(vtfpp_image_format_e format);
SOURCEPP_API int8_t vtfpp_image_format_details_decompressed_green(vtfpp_image_format_e format);
SOURCEPP_API int8_t vtfpp_image_format_details_blue(vtfpp_image_format_e format);
SOURCEPP_API int8_t vtfpp_image_format_details_decompressed_blue(vtfpp_image_format_e format);
SOURCEPP_API int8_t vtfpp_image_format_details_alpha(vtfpp_image_format_e format);
SOURCEPP_API int8_t vtfpp_image_format_details_decompressed_alpha(vtfpp_image_format_e format);
SOURCEPP_API uint8_t vtfpp_image_format_details_bpp(vtfpp_image_format_e format);
SOURCEPP_API vtfpp_image_format_e vtfpp_image_format_details_container_format(vtfpp_image_format_e format);
SOURCEPP_API int vtfpp_image_format_details_large(vtfpp_image_format_e format);
SOURCEPP_API int vtfpp_image_format_details_decimal(vtfpp_image_format_e format);
SOURCEPP_API int vtfpp_image_format_details_compressed(vtfpp_image_format_e format);
SOURCEPP_API int vtfpp_image_format_details_transparent(vtfpp_image_format_e format);
SOURCEPP_API int vtfpp_image_format_details_opaque(vtfpp_image_format_e format);
SOURCEPP_API int vtfpp_image_format_details_console(vtfpp_image_format_e format);
SOURCEPP_API uint16_t vtfpp_image_dimensions_get_mip_dim(uint8_t mip, uint16_t dim, int add_compressed_format_padding);

// Skipping vtfpp::ImageDimensions::getMipDims

SOURCEPP_API uint8_t vtfpp_image_dimensions_get_maximum_mip_count(uint16_t width, uint16_t height, uint16_t depth);
SOURCEPP_API uint32_t vtfpp_image_format_details_get_data_length(vtfpp_image_format_e format, uint16_t width, uint16_t height, uint16_t depth);
SOURCEPP_API uint32_t vtfpp_image_format_details_get_data_length_ex(vtfpp_image_format_e format, uint8_t mipCount, uint16_t frameCount, uint8_t faceCount, uint16_t width, uint16_t height, uint16_t depth);
SOURCEPP_API uint32_t vtfpp_image_format_details_get_data_length_xbox(int padded, vtfpp_image_format_e format, uint8_t mipCount, uint16_t frameCount, uint8_t faceCount, uint16_t width, uint16_t height, uint16_t depth);
SOURCEPP_API int vtfpp_image_format_details_get_data_position(uint32_t* offset, uint32_t* length, vtfpp_image_format_e format, uint8_t mip, uint8_t mipCount, uint16_t frame, uint16_t frameCount, uint8_t face, uint8_t faceCount, uint16_t width, uint16_t height, uint16_t slice, uint16_t depth);
SOURCEPP_API int vtfpp_image_format_details_get_data_position_xbox(uint32_t* offset, uint32_t* length, int padded, vtfpp_image_format_e format, uint8_t mip, uint8_t mipCount, uint16_t frame, uint16_t frameCount, uint8_t face, uint8_t faceCount, uint16_t width, uint16_t height, uint16_t slice, uint16_t depth);

// C++ conversion routines
#ifdef __cplusplus

#include <vtfpp/ImageFormats.h>

namespace sourceppc::convert {

inline vtfpp::ImageFormat cast(vtfpp_image_format_e value) {
	switch (value) {
		case VTFPP_IMAGE_FORMAT_RGBA8888:                    return vtfpp::ImageFormat::RGBA8888;
		case VTFPP_IMAGE_FORMAT_ABGR8888:                    return vtfpp::ImageFormat::ABGR8888;
		case VTFPP_IMAGE_FORMAT_RGB888:                      return vtfpp::ImageFormat::RGB888;
		case VTFPP_IMAGE_FORMAT_BGR888:                      return vtfpp::ImageFormat::BGR888;
		case VTFPP_IMAGE_FORMAT_RGB565:                      return vtfpp::ImageFormat::RGB565;
		case VTFPP_IMAGE_FORMAT_I8:                          return vtfpp::ImageFormat::I8;
		case VTFPP_IMAGE_FORMAT_IA88:                        return vtfpp::ImageFormat::IA88;
		case VTFPP_IMAGE_FORMAT_P8:                          return vtfpp::ImageFormat::P8;
		case VTFPP_IMAGE_FORMAT_A8:                          return vtfpp::ImageFormat::A8;
		case VTFPP_IMAGE_FORMAT_RGB888_BLUESCREEN:           return vtfpp::ImageFormat::RGB888_BLUESCREEN;
		case VTFPP_IMAGE_FORMAT_BGR888_BLUESCREEN:           return vtfpp::ImageFormat::BGR888_BLUESCREEN;
		case VTFPP_IMAGE_FORMAT_ARGB8888:                    return vtfpp::ImageFormat::ARGB8888;
		case VTFPP_IMAGE_FORMAT_BGRA8888:                    return vtfpp::ImageFormat::BGRA8888;
		case VTFPP_IMAGE_FORMAT_DXT1:                        return vtfpp::ImageFormat::DXT1;
		case VTFPP_IMAGE_FORMAT_DXT3:                        return vtfpp::ImageFormat::DXT3;
		case VTFPP_IMAGE_FORMAT_DXT5:                        return vtfpp::ImageFormat::DXT5;
		case VTFPP_IMAGE_FORMAT_BGRX8888:                    return vtfpp::ImageFormat::BGRX8888;
		case VTFPP_IMAGE_FORMAT_BGR565:                      return vtfpp::ImageFormat::BGR565;
		case VTFPP_IMAGE_FORMAT_BGRX5551:                    return vtfpp::ImageFormat::BGRX5551;
		case VTFPP_IMAGE_FORMAT_BGRA4444:                    return vtfpp::ImageFormat::BGRA4444;
		case VTFPP_IMAGE_FORMAT_DXT1_ONE_BIT_ALPHA:          return vtfpp::ImageFormat::DXT1_ONE_BIT_ALPHA;
		case VTFPP_IMAGE_FORMAT_BGRA5551:                    return vtfpp::ImageFormat::BGRA5551;
		case VTFPP_IMAGE_FORMAT_UV88:                        return vtfpp::ImageFormat::UV88;
		case VTFPP_IMAGE_FORMAT_UVWQ8888:                    return vtfpp::ImageFormat::UVWQ8888;
		case VTFPP_IMAGE_FORMAT_RGBA16161616F:               return vtfpp::ImageFormat::RGBA16161616F;
		case VTFPP_IMAGE_FORMAT_RGBA16161616:                return vtfpp::ImageFormat::RGBA16161616;
		case VTFPP_IMAGE_FORMAT_UVLX8888:                    return vtfpp::ImageFormat::UVLX8888;
		case VTFPP_IMAGE_FORMAT_R32F:                        return vtfpp::ImageFormat::R32F;
		case VTFPP_IMAGE_FORMAT_RGB323232F:                  return vtfpp::ImageFormat::RGB323232F;
		case VTFPP_IMAGE_FORMAT_RGBA32323232F:               return vtfpp::ImageFormat::RGBA32323232F;
		case VTFPP_IMAGE_FORMAT_RG1616F:                     return vtfpp::ImageFormat::RG1616F;
		case VTFPP_IMAGE_FORMAT_RG3232F:                     return vtfpp::ImageFormat::RG3232F;
		case VTFPP_IMAGE_FORMAT_RGBX8888:                    return vtfpp::ImageFormat::RGBX8888;
		case VTFPP_IMAGE_FORMAT_EMPTY:                       return vtfpp::ImageFormat::EMPTY;
		case VTFPP_IMAGE_FORMAT_ATI2N:                       return vtfpp::ImageFormat::ATI2N;
		case VTFPP_IMAGE_FORMAT_ATI1N:                       return vtfpp::ImageFormat::ATI1N;
		case VTFPP_IMAGE_FORMAT_RGBA1010102:                 return vtfpp::ImageFormat::RGBA1010102;
		case VTFPP_IMAGE_FORMAT_BGRA1010102:                 return vtfpp::ImageFormat::BGRA1010102;
		case VTFPP_IMAGE_FORMAT_R16F:                        return vtfpp::ImageFormat::R16F;
		case VTFPP_IMAGE_FORMAT_CONSOLE_BGRX8888_LINEAR:     return vtfpp::ImageFormat::CONSOLE_BGRX8888_LINEAR;
		case VTFPP_IMAGE_FORMAT_CONSOLE_RGBA8888_LINEAR:     return vtfpp::ImageFormat::CONSOLE_RGBA8888_LINEAR;
		case VTFPP_IMAGE_FORMAT_CONSOLE_ABGR8888_LINEAR:     return vtfpp::ImageFormat::CONSOLE_ABGR8888_LINEAR;
		case VTFPP_IMAGE_FORMAT_CONSOLE_ARGB8888_LINEAR:     return vtfpp::ImageFormat::CONSOLE_ARGB8888_LINEAR;
		case VTFPP_IMAGE_FORMAT_CONSOLE_BGRA8888_LINEAR:     return vtfpp::ImageFormat::CONSOLE_BGRA8888_LINEAR;
		case VTFPP_IMAGE_FORMAT_CONSOLE_RGB888_LINEAR:       return vtfpp::ImageFormat::CONSOLE_RGB888_LINEAR;
		case VTFPP_IMAGE_FORMAT_CONSOLE_BGR888_LINEAR:       return vtfpp::ImageFormat::CONSOLE_BGR888_LINEAR;
		case VTFPP_IMAGE_FORMAT_CONSOLE_BGRX5551_LINEAR:     return vtfpp::ImageFormat::CONSOLE_BGRX5551_LINEAR;
		case VTFPP_IMAGE_FORMAT_CONSOLE_I8_LINEAR:           return vtfpp::ImageFormat::CONSOLE_I8_LINEAR;
		case VTFPP_IMAGE_FORMAT_CONSOLE_RGBA16161616_LINEAR: return vtfpp::ImageFormat::CONSOLE_RGBA16161616_LINEAR;
		case VTFPP_IMAGE_FORMAT_CONSOLE_BGRX8888_LE:         return vtfpp::ImageFormat::CONSOLE_BGRX8888_LE;
		case VTFPP_IMAGE_FORMAT_CONSOLE_BGRA8888_LE:         return vtfpp::ImageFormat::CONSOLE_BGRA8888_LE;
		case VTFPP_IMAGE_FORMAT_TFALL2_BC6H:                 return vtfpp::ImageFormat::TFALL2_BC6H;
		case VTFPP_IMAGE_FORMAT_TFALL2_BC7:                  return vtfpp::ImageFormat::TFALL2_BC7;
		case VTFPP_IMAGE_FORMAT_STRATA_R8:                   return vtfpp::ImageFormat::STRATA_R8;
		case VTFPP_IMAGE_FORMAT_STRATA_BC7:                  return vtfpp::ImageFormat::STRATA_BC7;
		case VTFPP_IMAGE_FORMAT_STRATA_BC6H:                 return vtfpp::ImageFormat::STRATA_BC6H;
	}
	return vtfpp::ImageFormat::RGBA8888;
}

inline vtfpp_image_format_e cast(vtfpp::ImageFormat value) {
	switch (value) {
		case vtfpp::ImageFormat::RGBA8888:                    return VTFPP_IMAGE_FORMAT_RGBA8888;
		case vtfpp::ImageFormat::ABGR8888:                    return VTFPP_IMAGE_FORMAT_ABGR8888;
		case vtfpp::ImageFormat::RGB888:                      return VTFPP_IMAGE_FORMAT_RGB888;
		case vtfpp::ImageFormat::BGR888:                      return VTFPP_IMAGE_FORMAT_BGR888;
		case vtfpp::ImageFormat::RGB565:                      return VTFPP_IMAGE_FORMAT_RGB565;
		case vtfpp::ImageFormat::I8:                          return VTFPP_IMAGE_FORMAT_I8;
		case vtfpp::ImageFormat::IA88:                        return VTFPP_IMAGE_FORMAT_IA88;
		case vtfpp::ImageFormat::P8:                          return VTFPP_IMAGE_FORMAT_P8;
		case vtfpp::ImageFormat::A8:                          return VTFPP_IMAGE_FORMAT_A8;
		case vtfpp::ImageFormat::RGB888_BLUESCREEN:           return VTFPP_IMAGE_FORMAT_RGB888_BLUESCREEN;
		case vtfpp::ImageFormat::BGR888_BLUESCREEN:           return VTFPP_IMAGE_FORMAT_BGR888_BLUESCREEN;
		case vtfpp::ImageFormat::ARGB8888:                    return VTFPP_IMAGE_FORMAT_ARGB8888;
		case vtfpp::ImageFormat::BGRA8888:                    return VTFPP_IMAGE_FORMAT_BGRA8888;
		case vtfpp::ImageFormat::DXT1:                        return VTFPP_IMAGE_FORMAT_DXT1;
		case vtfpp::ImageFormat::DXT3:                        return VTFPP_IMAGE_FORMAT_DXT3;
		case vtfpp::ImageFormat::DXT5:                        return VTFPP_IMAGE_FORMAT_DXT5;
		case vtfpp::ImageFormat::BGRX8888:                    return VTFPP_IMAGE_FORMAT_BGRX8888;
		case vtfpp::ImageFormat::BGR565:                      return VTFPP_IMAGE_FORMAT_BGR565;
		case vtfpp::ImageFormat::BGRX5551:                    return VTFPP_IMAGE_FORMAT_BGRX5551;
		case vtfpp::ImageFormat::BGRA4444:                    return VTFPP_IMAGE_FORMAT_BGRA4444;
		case vtfpp::ImageFormat::DXT1_ONE_BIT_ALPHA:          return VTFPP_IMAGE_FORMAT_DXT1_ONE_BIT_ALPHA;
		case vtfpp::ImageFormat::BGRA5551:                    return VTFPP_IMAGE_FORMAT_BGRA5551;
		case vtfpp::ImageFormat::UV88:                        return VTFPP_IMAGE_FORMAT_UV88;
		case vtfpp::ImageFormat::UVWQ8888:                    return VTFPP_IMAGE_FORMAT_UVWQ8888;
		case vtfpp::ImageFormat::RGBA16161616F:               return VTFPP_IMAGE_FORMAT_RGBA16161616F;
		case vtfpp::ImageFormat::RGBA16161616:                return VTFPP_IMAGE_FORMAT_RGBA16161616;
		case vtfpp::ImageFormat::UVLX8888:                    return VTFPP_IMAGE_FORMAT_UVLX8888;
		case vtfpp::ImageFormat::R32F:                        return VTFPP_IMAGE_FORMAT_R32F;
		case vtfpp::ImageFormat::RGB323232F:                  return VTFPP_IMAGE_FORMAT_RGB323232F;
		case vtfpp::ImageFormat::RGBA32323232F:               return VTFPP_IMAGE_FORMAT_RGBA32323232F;
		case vtfpp::ImageFormat::RG1616F:                     return VTFPP_IMAGE_FORMAT_RG1616F;
		case vtfpp::ImageFormat::RG3232F:                     return VTFPP_IMAGE_FORMAT_RG3232F;
		case vtfpp::ImageFormat::RGBX8888:                    return VTFPP_IMAGE_FORMAT_RGBX8888;
		case vtfpp::ImageFormat::EMPTY:                       return VTFPP_IMAGE_FORMAT_EMPTY;
		case vtfpp::ImageFormat::ATI2N:                       return VTFPP_IMAGE_FORMAT_ATI2N;
		case vtfpp::ImageFormat::ATI1N:                       return VTFPP_IMAGE_FORMAT_ATI1N;
		case vtfpp::ImageFormat::RGBA1010102:                 return VTFPP_IMAGE_FORMAT_RGBA1010102;
		case vtfpp::ImageFormat::BGRA1010102:                 return VTFPP_IMAGE_FORMAT_BGRA1010102;
		case vtfpp::ImageFormat::R16F:                        return VTFPP_IMAGE_FORMAT_R16F;
		case vtfpp::ImageFormat::CONSOLE_BGRX8888_LINEAR:     return VTFPP_IMAGE_FORMAT_CONSOLE_BGRX8888_LINEAR;
		case vtfpp::ImageFormat::CONSOLE_RGBA8888_LINEAR:     return VTFPP_IMAGE_FORMAT_CONSOLE_RGBA8888_LINEAR;
		case vtfpp::ImageFormat::CONSOLE_ABGR8888_LINEAR:     return VTFPP_IMAGE_FORMAT_CONSOLE_ABGR8888_LINEAR;
		case vtfpp::ImageFormat::CONSOLE_ARGB8888_LINEAR:     return VTFPP_IMAGE_FORMAT_CONSOLE_ARGB8888_LINEAR;
		case vtfpp::ImageFormat::CONSOLE_BGRA8888_LINEAR:     return VTFPP_IMAGE_FORMAT_CONSOLE_BGRA8888_LINEAR;
		case vtfpp::ImageFormat::CONSOLE_RGB888_LINEAR:       return VTFPP_IMAGE_FORMAT_CONSOLE_RGB888_LINEAR;
		case vtfpp::ImageFormat::CONSOLE_BGR888_LINEAR:       return VTFPP_IMAGE_FORMAT_CONSOLE_BGR888_LINEAR;
		case vtfpp::ImageFormat::CONSOLE_BGRX5551_LINEAR:     return VTFPP_IMAGE_FORMAT_CONSOLE_BGRX5551_LINEAR;
		case vtfpp::ImageFormat::CONSOLE_I8_LINEAR:           return VTFPP_IMAGE_FORMAT_CONSOLE_I8_LINEAR;
		case vtfpp::ImageFormat::CONSOLE_RGBA16161616_LINEAR: return VTFPP_IMAGE_FORMAT_CONSOLE_RGBA16161616_LINEAR;
		case vtfpp::ImageFormat::CONSOLE_BGRX8888_LE:         return VTFPP_IMAGE_FORMAT_CONSOLE_BGRX8888_LE;
		case vtfpp::ImageFormat::CONSOLE_BGRA8888_LE:         return VTFPP_IMAGE_FORMAT_CONSOLE_BGRA8888_LE;
		case vtfpp::ImageFormat::TFALL2_BC6H:                 return VTFPP_IMAGE_FORMAT_TFALL2_BC6H;
		case vtfpp::ImageFormat::TFALL2_BC7:                  return VTFPP_IMAGE_FORMAT_TFALL2_BC7;
		case vtfpp::ImageFormat::STRATA_R8:                   return VTFPP_IMAGE_FORMAT_STRATA_R8;
		case vtfpp::ImageFormat::STRATA_BC7:                  return VTFPP_IMAGE_FORMAT_STRATA_BC7;
		case vtfpp::ImageFormat::STRATA_BC6H:                 return VTFPP_IMAGE_FORMAT_STRATA_BC6H;
	}
	return VTFPP_IMAGE_FORMAT_RGBA8888;
}

} // namespace sourceppc::convert

#endif
