#pragma once

#include <sourceppc/API.h>
#include <sourceppc/Buffer.h>
#include <sourceppc/String.h>

#include "HOT.h"
#include "ImageConversion.h"
#include "SHT.h"

SOURCEPP_EXTERNVAR const uint32_t VTFPP_VTF_SIGNATURE;
SOURCEPP_EXTERNVAR const uint32_t VTFPP_XTF_SIGNATURE;
SOURCEPP_EXTERNVAR const uint32_t VTFPP_VTFX_SIGNATURE;
SOURCEPP_EXTERNVAR const uint32_t VTFPP_VTF3_SIGNATURE;

SOURCEPP_EXTERN typedef enum {
	VTFPP_COMPRESSION_METHOD_DEFLATE = 8,
	VTFPP_COMPRESSION_METHOD_ZSTD = 93,
	VTFPP_COMPRESSION_METHOD_CONSOLE_LZMA = 0x360,
} vtfpp_compression_method_e;

SOURCEPP_EXTERN typedef enum {
	VTFPP_RESOURCE_TYPE_UNKNOWN             = 0,
	VTFPP_RESOURCE_TYPE_THUMBNAIL_DATA      = 1,
	VTFPP_RESOURCE_TYPE_PALETTE_DATA        = 2,
	VTFPP_RESOURCE_TYPE_FALLBACK_DATA       = 3,
	VTFPP_RESOURCE_TYPE_PARTICLE_SHEET_DATA = 16,
	VTFPP_RESOURCE_TYPE_HOTSPOT_DATA        = 43,
	VTFPP_RESOURCE_TYPE_IMAGE_DATA          = 48,
	VTFPP_RESOURCE_TYPE_EXTENDED_FLAGS      = 3167060,
	VTFPP_RESOURCE_TYPE_CRC                 = 4411971,
	VTFPP_RESOURCE_TYPE_AUX_COMPRESSION     = 4413505,
	VTFPP_RESOURCE_TYPE_LOD_CONTROL_INFO    = 4476748,
	VTFPP_RESOURCE_TYPE_KEYVALUES_DATA      = 4478539,
} vtfpp_resource_type_e;

SOURCEPP_EXTERN typedef enum {
	VTFPP_RESOURCE_FLAG_NONE       = 0,
	VTFPP_RESOURCE_FLAG_LOCAL_DATA = 1 << 1,
} vtfpp_resource_flags_e;

typedef const void* vtfpp_resource_handle_t;

SOURCEPP_API vtfpp_resource_type_e vtfpp_resource_get_type(vtfpp_resource_handle_t handle);
SOURCEPP_API vtfpp_resource_flags_e vtfpp_resource_get_flags(vtfpp_resource_handle_t handle);
SOURCEPP_API unsigned char* vtfpp_resource_get_data(vtfpp_resource_handle_t handle, size_t* dataLen);
SOURCEPP_API sourcepp_buffer_t vtfpp_resource_get_data_as_palette(vtfpp_resource_handle_t handle, uint16_t frame); // REQUIRES MANUAL FREE: sourcepp_buffer_free
SOURCEPP_API vtfpp_sht_handle_t vtfpp_resource_get_data_as_particle_sheet(vtfpp_resource_handle_t handle); // REQUIRES MANUAL FREE: vtfpp_sht_free
SOURCEPP_API uint32_t vtfpp_resource_get_data_as_crc(vtfpp_resource_handle_t handle);
SOURCEPP_API uint32_t vtfpp_resource_get_data_as_extended_flags(vtfpp_resource_handle_t handle);
SOURCEPP_API void vtfpp_resource_get_data_as_lod_control_info(vtfpp_resource_handle_t handle, uint8_t* u, uint8_t* v, uint8_t* u360, uint8_t* v360);
SOURCEPP_API sourcepp_string_t vtfpp_resource_get_data_as_keyvalues_data(vtfpp_resource_handle_t handle); // REQUIRES MANUAL FREE: sourcepp_string_free
SOURCEPP_API vtfpp_hot_handle_t vtfpp_resource_get_data_as_hotspot_data(vtfpp_resource_handle_t handle); // REQUIRES MANUAL FREE: vtfpp_hot_free
SOURCEPP_API int16_t vtfpp_resource_get_data_as_aux_compression_level(vtfpp_resource_handle_t handle);
SOURCEPP_API vtfpp_compression_method_e vtfpp_resource_get_data_as_aux_compression_method(vtfpp_resource_handle_t handle);
SOURCEPP_API uint32_t vtfpp_resource_get_data_as_aux_compression_length(vtfpp_resource_handle_t handle, uint8_t mip, uint8_t mipCount, uint16_t frame, uint16_t frameCount, uint16_t face, uint16_t faceCount);

SOURCEPP_EXTERN typedef enum {
	VTFPP_VTF_FLAG_V0_POINT_SAMPLE     = 1u <<  0,
	VTFPP_VTF_FLAG_V0_TRILINEAR        = 1u <<  1,
	VTFPP_VTF_FLAG_V0_CLAMP_S          = 1u <<  2,
	VTFPP_VTF_FLAG_V0_CLAMP_T          = 1u <<  3,
	VTFPP_VTF_FLAG_V0_ANISOTROPIC      = 1u <<  4,
	VTFPP_VTF_FLAG_V0_VTEX_HINT_DXT5   = 1u <<  5,
	VTFPP_VTF_FLAG_V0_VTEX_NO_COMPRESS = 1u <<  6,
	VTFPP_VTF_FLAG_V0_NORMAL           = 1u <<  7,
	VTFPP_VTF_FLAG_V0_NO_MIP           = 1u <<  8,
	VTFPP_VTF_FLAG_V0_NO_LOD           = 1u <<  9,
	VTFPP_VTF_FLAG_V0_LOAD_SMALL_MIPS  = 1u << 10,
	VTFPP_VTF_FLAG_V0_PROCEDURAL       = 1u << 11,
	VTFPP_VTF_FLAG_V0_ONE_BIT_ALPHA    = 1u << 12,
	VTFPP_VTF_FLAG_V0_MULTI_BIT_ALPHA  = 1u << 13,
	VTFPP_VTF_FLAG_V0_ENVMAP           = 1u << 14,
	VTFPP_VTF_FLAG_MASK_V0 = VTFPP_VTF_FLAG_V0_POINT_SAMPLE | VTFPP_VTF_FLAG_V0_TRILINEAR | VTFPP_VTF_FLAG_V0_CLAMP_S | VTFPP_VTF_FLAG_V0_CLAMP_T | VTFPP_VTF_FLAG_V0_ANISOTROPIC | VTFPP_VTF_FLAG_V0_VTEX_HINT_DXT5 | VTFPP_VTF_FLAG_V0_VTEX_NO_COMPRESS | VTFPP_VTF_FLAG_V0_NORMAL | VTFPP_VTF_FLAG_V0_NO_MIP | VTFPP_VTF_FLAG_V0_NO_LOD | VTFPP_VTF_FLAG_V0_LOAD_SMALL_MIPS | VTFPP_VTF_FLAG_V0_PROCEDURAL | VTFPP_VTF_FLAG_V0_ONE_BIT_ALPHA | VTFPP_VTF_FLAG_V0_MULTI_BIT_ALPHA | VTFPP_VTF_FLAG_V0_ENVMAP,
	VTFPP_VTF_FLAG_MASK_V0_VTEX = VTFPP_VTF_FLAG_V0_VTEX_HINT_DXT5 | VTFPP_VTF_FLAG_V0_VTEX_NO_COMPRESS,

	VTFPP_VTF_FLAG_V1_RENDERTARGET                                 = 1u << 15,
	VTFPP_VTF_FLAG_V1_DEPTH_RENDERTARGET                           = 1u << 16,
	VTFPP_VTF_FLAG_V1_NO_DEBUG_OVERRIDE                            = 1u << 17,
	VTFPP_VTF_FLAG_V1_SINGLE_COPY                                  = 1u << 18,
	VTFPP_VTF_FLAG_V1_VTEX_ONE_OVER_MIP_LEVEL_IN_ALPHA             = 1u << 19,
	VTFPP_VTF_FLAG_V1_VTEX_PREMULTIPLY_COLOR_BY_ONE_OVER_MIP_LEVEL = 1u << 20,
	VTFPP_VTF_FLAG_V1_VTEX_CONVERT_NORMAL_TO_DUDV                  = 1u << 21,
	VTFPP_VTF_FLAG_MASK_V1 = VTFPP_VTF_FLAG_V1_RENDERTARGET | VTFPP_VTF_FLAG_V1_DEPTH_RENDERTARGET | VTFPP_VTF_FLAG_V1_NO_DEBUG_OVERRIDE | VTFPP_VTF_FLAG_V1_SINGLE_COPY | VTFPP_VTF_FLAG_V1_VTEX_ONE_OVER_MIP_LEVEL_IN_ALPHA | VTFPP_VTF_FLAG_V1_VTEX_PREMULTIPLY_COLOR_BY_ONE_OVER_MIP_LEVEL | VTFPP_VTF_FLAG_V1_VTEX_CONVERT_NORMAL_TO_DUDV,
	VTFPP_VTF_FLAG_MASK_V1_VTEX = VTFPP_VTF_FLAG_V1_VTEX_ONE_OVER_MIP_LEVEL_IN_ALPHA | VTFPP_VTF_FLAG_V1_VTEX_PREMULTIPLY_COLOR_BY_ONE_OVER_MIP_LEVEL | VTFPP_VTF_FLAG_V1_VTEX_CONVERT_NORMAL_TO_DUDV,

	VTFPP_VTF_FLAG_V2_VTEX_ALPHA_TEST_MIP_GENERATION = 1u << 22, // Internal to vtex
	VTFPP_VTF_FLAG_V2_NO_DEPTH_BUFFER                = 1u << 23,
	VTFPP_VTF_FLAG_V2_VTEX_NICE_FILTERED             = 1u << 24, // Internal to vtex
	VTFPP_VTF_FLAG_V2_CLAMP_U                        = 1u << 25,
	VTFPP_VTF_FLAG_MASK_V2 = VTFPP_VTF_FLAG_V2_VTEX_ALPHA_TEST_MIP_GENERATION | VTFPP_VTF_FLAG_V2_NO_DEPTH_BUFFER | VTFPP_VTF_FLAG_V2_VTEX_NICE_FILTERED | VTFPP_VTF_FLAG_V2_CLAMP_U,
	VTFPP_VTF_FLAG_MASK_V2_VTEX = VTFPP_VTF_FLAG_V2_VTEX_ALPHA_TEST_MIP_GENERATION | VTFPP_VTF_FLAG_V2_VTEX_NICE_FILTERED,

	VTFPP_VTF_FLAG_XBOX_VTEX_PRESWIZZLED = 1u << 26,
	VTFPP_VTF_FLAG_XBOX_CACHEABLE        = 1u << 27,
	VTFPP_VTF_FLAG_XBOX_UNFILTERABLE_OK  = 1u << 28,
	VTFPP_VTF_FLAG_MASK_XBOX = VTFPP_VTF_FLAG_XBOX_VTEX_PRESWIZZLED | VTFPP_VTF_FLAG_XBOX_CACHEABLE | VTFPP_VTF_FLAG_XBOX_UNFILTERABLE_OK,
	VTFPP_VTF_FLAG_MASK_XBOX_VTEX = VTFPP_VTF_FLAG_XBOX_VTEX_PRESWIZZLED,

	VTFPP_VTF_FLAG_V3_LOAD_ALL_MIPS  = 1u << 10,
	VTFPP_VTF_FLAG_V3_VERTEX_TEXTURE = 1u << 26,
	VTFPP_VTF_FLAG_V3_SSBUMP         = 1u << 27,
	VTFPP_VTF_FLAG_V3_BORDER         = 1u << 29,
	VTFPP_VTF_FLAG_MASK_V3 = VTFPP_VTF_FLAG_V3_LOAD_ALL_MIPS | VTFPP_VTF_FLAG_V3_VERTEX_TEXTURE | VTFPP_VTF_FLAG_V3_SSBUMP | VTFPP_VTF_FLAG_V3_BORDER,

	VTFPP_VTF_FLAG_V4_SRGB = 1u << 6,
	VTFPP_VTF_FLAG_MASK_V4 = VTFPP_VTF_FLAG_V4_SRGB,

	VTFPP_VTF_FLAG_V4_TF2_STAGING_MEMORY    = 1u << 19,
	VTFPP_VTF_FLAG_V4_TF2_IMMEDIATE_CLEANUP = 1u << 20,
	VTFPP_VTF_FLAG_V4_TF2_IGNORE_PICMIP     = 1u << 21,
	VTFPP_VTF_FLAG_V4_TF2_STREAMABLE_COARSE = 1u << 30,
	VTFPP_VTF_FLAG_V4_TF2_STREAMABLE_FINE   = 1u << 31,
	VTFPP_VTF_FLAG_MASK_V4_TF2 = VTFPP_VTF_FLAG_V4_TF2_STAGING_MEMORY | VTFPP_VTF_FLAG_V4_TF2_IMMEDIATE_CLEANUP | VTFPP_VTF_FLAG_V4_TF2_IGNORE_PICMIP | VTFPP_VTF_FLAG_V4_TF2_STREAMABLE_COARSE | VTFPP_VTF_FLAG_V4_TF2_STREAMABLE_FINE,

	VTFPP_VTF_FLAG_V5_PWL_CORRECTED  = 1u <<  6,
	VTFPP_VTF_FLAG_V5_SRGB           = 1u << 19,
	VTFPP_VTF_FLAG_V5_DEFAULT_POOL   = 1u << 20,
	VTFPP_VTF_FLAG_V5_LOAD_MOST_MIPS = 1u << 28,
	VTFPP_VTF_FLAG_MASK_V5 = VTFPP_VTF_FLAG_V5_PWL_CORRECTED | VTFPP_VTF_FLAG_V5_SRGB | VTFPP_VTF_FLAG_V5_DEFAULT_POOL | VTFPP_VTF_FLAG_V5_LOAD_MOST_MIPS,

	VTFPP_VTF_FLAG_V5_CSGO_COMBINED                   = 1u << 21,
	VTFPP_VTF_FLAG_V5_CSGO_ASYNC_DOWNLOAD             = 1u << 22,
	VTFPP_VTF_FLAG_V5_CSGO_SKIP_INITIAL_DOWNLOAD      = 1u << 24,
	VTFPP_VTF_FLAG_V5_CSGO_YCOCG                      = 1u << 30,
	VTFPP_VTF_FLAG_V5_CSGO_ASYNC_SKIP_INITIAL_LOW_RES = 1u << 31,
	VTFPP_VTF_FLAG_MASK_V5_CSGO = VTFPP_VTF_FLAG_V5_CSGO_COMBINED | VTFPP_VTF_FLAG_V5_CSGO_ASYNC_DOWNLOAD | VTFPP_VTF_FLAG_V5_CSGO_SKIP_INITIAL_DOWNLOAD | VTFPP_VTF_FLAG_V5_CSGO_YCOCG | VTFPP_VTF_FLAG_V5_CSGO_ASYNC_SKIP_INITIAL_LOW_RES,

	VTFPP_VTF_FLAG_MASK_INTERNAL = VTFPP_VTF_FLAG_V0_NO_MIP | VTFPP_VTF_FLAG_V0_ENVMAP,
} vtfpp_vtf_flags_e;

SOURCEPP_EXTERN typedef enum {
	VTFPP_VTF_PLATFORM_UNKNOWN       = 0x000,
	VTFPP_VTF_PLATFORM_PC            = 0x007,
	VTFPP_VTF_PLATFORM_XBOX          = 0x005,
	VTFPP_VTF_PLATFORM_X360          = 0x360,
	VTFPP_VTF_PLATFORM_PS3_ORANGEBOX = 0x333,
	VTFPP_VTF_PLATFORM_PS3_PORTAL2   = 0x334,
} vtfpp_vtf_platform_e;

SOURCEPP_EXTERNVAR const vtfpp_image_format_e VTFPP_VTF_FORMAT_UNCHANGED;
SOURCEPP_EXTERNVAR const vtfpp_image_format_e VTFPP_VTF_FORMAT_DEFAULT;

SOURCEPP_EXTERN typedef struct {
	uint32_t version;
	vtfpp_image_format_e outputFormat;
	float compressedFormatQuality;
	vtfpp_image_conversion_resize_method_e widthResizeMethod;
	vtfpp_image_conversion_resize_method_e heightResizeMethod;
	vtfpp_image_conversion_resize_filter_e filter;
	uint32_t flags;
	uint16_t initialFrameCount;
	uint16_t startFrame;
	int isCubeMap;
	uint16_t initialDepth;
	int computeTransparencyFlags;
	int computeMips;
	int computeThumbnail;
	int computeReflectivity;
	vtfpp_vtf_platform_e platform;
	int16_t compressionLevel;
	vtfpp_compression_method_e compressionMethod;
	float bumpMapScale;
	float gammaCorrection;
	int invertGreenChannel;
	uint8_t consoleMipScale;
} vtfpp_vtf_creation_options_t;

#define VTFPP_VTF_CREATION_OPTIONS_DEFAULT (SOURCEPP_CAST_CTOR(vtfpp_vtf_creation_options_t) { \
	.version = 4, \
	.outputFormat = VTFPP_VTF_FORMAT_DEFAULT, \
	.compressedFormatQuality = VTFPP_IMAGE_CONVERSION_DEFAULT_COMPRESSED_QUALITY, \
	.widthResizeMethod = VTFPP_IMAGE_CONVERSION_RESIZE_METHOD_POWER_OF_TWO_BIGGER, \
	.heightResizeMethod = VTFPP_IMAGE_CONVERSION_RESIZE_METHOD_POWER_OF_TWO_BIGGER, \
    .filter = VTFPP_IMAGE_CONVERSION_RESIZE_FILTER_DEFAULT, \
    .flags = 0, \
    .initialFrameCount = 1, \
    .startFrame = 0, \
    .isCubeMap = 0, \
	.hasSphereMap = 0, \
	.initialDepth = 1, \
	.computeTransparencyFlags = 1, \
	.computeMips = 1, \
	.computeThumbnail = 1, \
	.computeReflectivity = 1, \
	.platform = VTFPP_VTF_PLATFORM_PC, \
	.compressionLevel = -1, \
	.compressionMethod = VTFPP_COMPRESSION_METHOD_ZSTD, \
	.bumpMapScale = 1.f, \
	.gammaCorrection = 1.f, \
	.invertGreenChannel = 0, \
	.consoleMipScale = 0, \
})

typedef void* vtfpp_vtf_handle_t;

SOURCEPP_API vtfpp_vtf_handle_t vtfpp_vtf_create_empty(); // REQUIRES MANUAL FREE: vtfpp_vtf_close
SOURCEPP_API vtfpp_vtf_handle_t vtfpp_vtf_open_from_mem(const unsigned char* vtfData, size_t vtfLen, int parseHeaderOnly); // REQUIRES MANUAL FREE: vtfpp_vtf_close
SOURCEPP_API vtfpp_vtf_handle_t vtfpp_vtf_open_from_file(const char* vtfPath, int parseHeaderOnly); // REQUIRES MANUAL FREE: vtfpp_vtf_close
SOURCEPP_API void vtfpp_vtf_close(vtfpp_vtf_handle_t* handle);
SOURCEPP_API int vtfpp_vtf_is_valid(vtfpp_vtf_handle_t handle);
SOURCEPP_API int vtfpp_vtf_create_from_mem_and_bake(const unsigned char* imageData, size_t imageLen, vtfpp_image_format_e format, uint16_t width, uint16_t height, const char* vtfPath, vtfpp_vtf_creation_options_t options);
SOURCEPP_API int vtfpp_vtf_create_blank_and_bake(vtfpp_image_format_e format, uint16_t width, uint16_t height, const char* vtfPath, vtfpp_vtf_creation_options_t options);
SOURCEPP_API vtfpp_vtf_handle_t vtfpp_vtf_create_from_mem(const unsigned char* imageData, size_t imageLen, vtfpp_image_format_e format, uint16_t width, uint16_t height, vtfpp_vtf_creation_options_t options); // REQUIRES MANUAL FREE: vtfpp_vtf_close
SOURCEPP_API vtfpp_vtf_handle_t vtfpp_vtf_create_blank(vtfpp_image_format_e format, uint16_t width, uint16_t height, vtfpp_vtf_creation_options_t options); // REQUIRES MANUAL FREE: vtfpp_vtf_close
SOURCEPP_API int vtfpp_vtf_create_from_file_and_bake(const char* imagePath, const char* vtfPath, vtfpp_vtf_creation_options_t options);
SOURCEPP_API vtfpp_vtf_handle_t vtfpp_vtf_create_from_file(const char* imagePath, vtfpp_vtf_creation_options_t options); // REQUIRES MANUAL FREE: vtfpp_vtf_close
SOURCEPP_API vtfpp_vtf_platform_e vtfpp_vtf_get_platform(vtfpp_vtf_handle_t handle);
SOURCEPP_API void vtfpp_vtf_set_platform(vtfpp_vtf_handle_t handle, vtfpp_vtf_platform_e platform);
SOURCEPP_API uint32_t vtfpp_vtf_get_version(vtfpp_vtf_handle_t handle);
SOURCEPP_API void vtfpp_vtf_set_version(vtfpp_vtf_handle_t handle, uint32_t version);
SOURCEPP_API vtfpp_image_conversion_resize_method_e vtfpp_vtf_get_image_width_resize_method(vtfpp_vtf_handle_t handle);
SOURCEPP_API vtfpp_image_conversion_resize_method_e vtfpp_vtf_get_image_height_resize_method(vtfpp_vtf_handle_t handle);
SOURCEPP_API void vtfpp_vtf_set_image_resize_methods(vtfpp_vtf_handle_t handle, vtfpp_image_conversion_resize_method_e imageWidthResizeMethod, vtfpp_image_conversion_resize_method_e imageHeightResizeMethod);
SOURCEPP_API void vtfpp_vtf_set_image_width_resize_method(vtfpp_vtf_handle_t handle, vtfpp_image_conversion_resize_method_e imageWidthResizeMethod);
SOURCEPP_API void vtfpp_vtf_set_image_height_resize_method(vtfpp_vtf_handle_t handle, vtfpp_image_conversion_resize_method_e imageHeightResizeMethod);
SOURCEPP_API uint16_t vtfpp_vtf_get_width(vtfpp_vtf_handle_t handle, uint8_t mip);
SOURCEPP_API uint16_t vtfpp_vtf_get_padded_width(vtfpp_vtf_handle_t handle, uint8_t mip);
SOURCEPP_API uint16_t vtfpp_vtf_get_height(vtfpp_vtf_handle_t handle, uint8_t mip);
SOURCEPP_API uint16_t vtfpp_vtf_get_padded_height(vtfpp_vtf_handle_t handle, uint8_t mip);
SOURCEPP_API void vtfpp_vtf_set_size(vtfpp_vtf_handle_t handle, uint16_t width, uint16_t height, vtfpp_image_conversion_resize_filter_e filter);
SOURCEPP_API uint32_t vtfpp_vtf_get_flags(vtfpp_vtf_handle_t handle);
SOURCEPP_API void vtfpp_vtf_set_flags(vtfpp_vtf_handle_t handle, uint32_t flags);
SOURCEPP_API void vtfpp_vtf_add_flags(vtfpp_vtf_handle_t handle, uint32_t flags);
SOURCEPP_API void vtfpp_vtf_remove_flags(vtfpp_vtf_handle_t handle, uint32_t flags);
SOURCEPP_API int vtfpp_vtf_is_srgb(vtfpp_vtf_handle_t handle);
SOURCEPP_API void vtfpp_vtf_set_srgb(vtfpp_vtf_handle_t handle, int srgb);
SOURCEPP_API void vtfpp_vtf_compute_transparency_flags(vtfpp_vtf_handle_t handle);
SOURCEPP_API vtfpp_image_format_e vtfpp_vtf_get_default_compressed_format(vtfpp_image_format_e inputFormat, uint32_t version, int isCubeMap);
SOURCEPP_API vtfpp_image_format_e vtfpp_vtf_get_format(vtfpp_vtf_handle_t handle);
SOURCEPP_API void vtfpp_vtf_set_format(vtfpp_vtf_handle_t handle, vtfpp_image_format_e format, vtfpp_image_conversion_resize_filter_e filter, float quality);
SOURCEPP_API uint8_t vtfpp_vtf_get_mip_count(vtfpp_vtf_handle_t handle);
SOURCEPP_API int vtfpp_vtf_set_mip_count(vtfpp_vtf_handle_t handle, uint8_t mipCount);
SOURCEPP_API int vtfpp_vtf_set_recommended_mip_count(vtfpp_vtf_handle_t handle);
SOURCEPP_API void vtfpp_vtf_compute_mips(vtfpp_vtf_handle_t handle, vtfpp_image_conversion_resize_filter_e filter);
SOURCEPP_API uint16_t vtfpp_vtf_get_frame_count(vtfpp_vtf_handle_t handle);
SOURCEPP_API int vtfpp_vtf_set_frame_count(vtfpp_vtf_handle_t handle, uint16_t frameCount);
SOURCEPP_API uint8_t vtfpp_vtf_get_face_count(vtfpp_vtf_handle_t handle);
SOURCEPP_API int vtfpp_vtf_set_face_count(vtfpp_vtf_handle_t handle, int isCubeMap);
SOURCEPP_API uint16_t vtfpp_vtf_get_depth(vtfpp_vtf_handle_t handle);
SOURCEPP_API int vtfpp_vtf_set_depth(vtfpp_vtf_handle_t handle, uint16_t depth);
SOURCEPP_API int vtfpp_vtf_set_frame_face_and_depth(vtfpp_vtf_handle_t handle, uint16_t frameCount, int isCubeMap, uint16_t depth);
SOURCEPP_API uint16_t vtfpp_vtf_get_start_frame(vtfpp_vtf_handle_t handle);
SOURCEPP_API void vtfpp_vtf_set_start_frame(vtfpp_vtf_handle_t handle, uint16_t startFrame);
SOURCEPP_API void vtfpp_vtf_get_reflectivity(vtfpp_vtf_handle_t handle, float* r, float* g, float* b);
SOURCEPP_API void vtfpp_vtf_set_reflectivity(vtfpp_vtf_handle_t handle, float r, float g, float b);
SOURCEPP_API void vtfpp_vtf_compute_reflectivity(vtfpp_vtf_handle_t handle);
SOURCEPP_API float vtfpp_vtf_get_bumpmap_scale(vtfpp_vtf_handle_t handle);
SOURCEPP_API void vtfpp_vtf_set_bumpmap_scale(vtfpp_vtf_handle_t handle, float bumpMapScale);
SOURCEPP_API vtfpp_image_format_e vtfpp_vtf_get_thumbnail_format(vtfpp_vtf_handle_t handle);
SOURCEPP_API uint8_t vtfpp_vtf_get_thumbnail_width(vtfpp_vtf_handle_t handle);
SOURCEPP_API uint8_t vtfpp_vtf_get_thumbnail_height(vtfpp_vtf_handle_t handle);
SOURCEPP_API uint8_t vtfpp_vtf_get_fallback_width(vtfpp_vtf_handle_t handle);
SOURCEPP_API uint8_t vtfpp_vtf_get_fallback_height(vtfpp_vtf_handle_t handle);
SOURCEPP_API uint8_t vtfpp_vtf_get_fallback_mip_count(vtfpp_vtf_handle_t handle);
SOURCEPP_API uint32_t vtfpp_vtf_get_resources_count(vtfpp_vtf_handle_t handle);
SOURCEPP_API vtfpp_resource_handle_t vtfpp_vtf_get_resource_at_index(vtfpp_vtf_handle_t handle, uint32_t index);
SOURCEPP_API vtfpp_resource_handle_t vtfpp_vtf_get_resource_with_type(vtfpp_vtf_handle_t handle, vtfpp_resource_type_e type);
SOURCEPP_API sourcepp_buffer_t vtfpp_vtf_get_palette_resource_frame(vtfpp_vtf_handle_t handle, uint16_t frame); // REQUIRES MANUAL FREE: sourcepp_buffer_free
SOURCEPP_API sourcepp_buffer_t vtfpp_vtf_get_particle_sheet_frame_data_raw(vtfpp_vtf_handle_t handle, uint16_t* width, uint16_t* height, uint32_t shtSequenceID, uint32_t shtFrame, uint8_t shtBounds, uint8_t mip, uint16_t frame, uint8_t face, uint16_t slice); // REQUIRES MANUAL FREE: sourcepp_buffer_free
SOURCEPP_API sourcepp_buffer_t vtfpp_vtf_get_particle_sheet_frame_data_as(vtfpp_vtf_handle_t handle, vtfpp_image_format_e format, uint16_t* width, uint16_t* height, uint32_t shtSequenceID, uint32_t shtFrame, uint8_t shtBounds, uint8_t mip, uint16_t frame, uint8_t face, uint16_t slice); // REQUIRES MANUAL FREE: sourcepp_buffer_free
SOURCEPP_API sourcepp_buffer_t vtfpp_vtf_get_particle_sheet_frame_data_as_rgba8888(vtfpp_vtf_handle_t handle, uint16_t* width, uint16_t* height, uint32_t shtSequenceID, uint32_t shtFrame, uint8_t shtBounds, uint8_t mip, uint16_t frame, uint8_t face, uint16_t slice); // REQUIRES MANUAL FREE: sourcepp_buffer_free
SOURCEPP_API void vtfpp_vtf_set_particle_sheet_resource(vtfpp_vtf_handle_t handle, vtfpp_sht_handle_t sht);
SOURCEPP_API void vtfpp_vtf_remove_particle_sheet_resource(vtfpp_vtf_handle_t handle);
SOURCEPP_API void vtfpp_vtf_set_crc_resource(vtfpp_vtf_handle_t handle, uint32_t value);
SOURCEPP_API void vtfpp_vtf_remove_crc_resource(vtfpp_vtf_handle_t handle);
SOURCEPP_API void vtfpp_vtf_set_lod_resource(vtfpp_vtf_handle_t handle, uint8_t u, uint8_t v, uint8_t u360, uint8_t v360);
SOURCEPP_API void vtfpp_vtf_remove_lod_resource(vtfpp_vtf_handle_t handle);
SOURCEPP_API void vtfpp_vtf_set_extended_flags_resource(vtfpp_vtf_handle_t handle, uint32_t value);
SOURCEPP_API void vtfpp_vtf_remove_extended_flags_resource(vtfpp_vtf_handle_t handle);
SOURCEPP_API void vtfpp_vtf_set_keyvalues_data_resource(vtfpp_vtf_handle_t handle, const char* value);
SOURCEPP_API void vtfpp_vtf_remove_keyvalues_data_resource(vtfpp_vtf_handle_t handle);
SOURCEPP_API void vtfpp_vtf_set_hotspot_data_resource(vtfpp_vtf_handle_t handle, vtfpp_hot_handle_t hot);
SOURCEPP_API void vtfpp_vtf_remove_hotspot_data_resource(vtfpp_vtf_handle_t handle);
SOURCEPP_API int16_t vtfpp_vtf_get_compression_level(vtfpp_vtf_handle_t handle);
SOURCEPP_API void vtfpp_vtf_set_compression_level(vtfpp_vtf_handle_t handle, int16_t compressionLevel);
SOURCEPP_API vtfpp_compression_method_e vtfpp_vtf_get_compression_method(vtfpp_vtf_handle_t handle);
SOURCEPP_API void vtfpp_vtf_set_compression_method(vtfpp_vtf_handle_t handle, vtfpp_compression_method_e compressionMethod);
SOURCEPP_API int vtfpp_vtf_has_image_data(vtfpp_vtf_handle_t handle);
SOURCEPP_API const unsigned char* vtfpp_vtf_get_image_data_raw(vtfpp_vtf_handle_t handle, size_t* imageLen, uint8_t mip, uint16_t frame, uint8_t face, uint16_t slice);
SOURCEPP_API sourcepp_buffer_t vtfpp_vtf_get_image_data_as(vtfpp_vtf_handle_t handle, vtfpp_image_format_e format, uint8_t mip, uint16_t frame, uint8_t face, uint16_t slice); // REQUIRES MANUAL FREE: sourcepp_buffer_free
SOURCEPP_API sourcepp_buffer_t vtfpp_vtf_get_image_data_as_rgba8888(vtfpp_vtf_handle_t handle, uint8_t mip, uint16_t frame, uint8_t face, uint16_t slice); // REQUIRES MANUAL FREE: sourcepp_buffer_free
SOURCEPP_API int vtfpp_vtf_set_image_from_mem(vtfpp_vtf_handle_t handle, const unsigned char* imageData, size_t imageLen, vtfpp_image_format_e format, uint16_t width, uint16_t height, vtfpp_image_conversion_resize_filter_e filter, uint8_t mip, uint16_t frame, uint8_t face, uint16_t slice, float quality);
SOURCEPP_API int vtfpp_vtf_set_image_from_file(vtfpp_vtf_handle_t handle, const char* imagePath, vtfpp_image_conversion_resize_filter_e filter, uint8_t mip, uint16_t frame, uint8_t face, uint16_t slice, float quality);
SOURCEPP_API int vtfpp_vtf_save_image_to_file(vtfpp_vtf_handle_t handle, const char* imagePath, uint8_t mip, uint16_t frame, uint8_t face, uint16_t slice, vtfpp_image_conversion_file_format_e fileFormat);
SOURCEPP_API int vtfpp_vtf_has_thumbnail_data(vtfpp_vtf_handle_t handle);
SOURCEPP_API const unsigned char* vtfpp_vtf_get_thumbnail_data_raw(vtfpp_vtf_handle_t handle, size_t* imageLen);
SOURCEPP_API sourcepp_buffer_t vtfpp_vtf_get_thumbnail_data_as(vtfpp_vtf_handle_t handle, vtfpp_image_format_e format); // REQUIRES MANUAL FREE: sourcepp_buffer_free
SOURCEPP_API sourcepp_buffer_t vtfpp_vtf_get_thumbnail_data_as_rgba8888(vtfpp_vtf_handle_t handle); // REQUIRES MANUAL FREE: sourcepp_buffer_free
SOURCEPP_API void vtfpp_vtf_set_thumbnail_from_mem(vtfpp_vtf_handle_t handle, const unsigned char* imageData, size_t imageLen, vtfpp_image_format_e format, uint16_t width, uint16_t height, float quality);
SOURCEPP_API void vtfpp_vtf_set_thumbnail_from_file(vtfpp_vtf_handle_t handle, const char* imagePath, float quality);
SOURCEPP_API void vtfpp_vtf_compute_thumbnail(vtfpp_vtf_handle_t handle, vtfpp_image_conversion_resize_filter_e filter, float quality);
SOURCEPP_API void vtfpp_vtf_remove_thumbnail(vtfpp_vtf_handle_t handle);
SOURCEPP_API int vtfpp_vtf_save_thumbnail_to_file(vtfpp_vtf_handle_t handle, const char* imagePath, vtfpp_image_conversion_file_format_e fileFormat);
SOURCEPP_API int vtfpp_vtf_has_fallback_data(vtfpp_vtf_handle_t handle);
SOURCEPP_API const unsigned char* vtfpp_vtf_get_fallback_data_raw(vtfpp_vtf_handle_t handle, size_t* imageLen, uint8_t mip, uint16_t frame, uint8_t face);
SOURCEPP_API sourcepp_buffer_t vtfpp_vtf_get_fallback_data_as(vtfpp_vtf_handle_t handle, vtfpp_image_format_e format, uint8_t mip, uint16_t frame, uint8_t face); // REQUIRES MANUAL FREE: sourcepp_buffer_free
SOURCEPP_API sourcepp_buffer_t vtfpp_vtf_get_fallback_data_as_rgba8888(vtfpp_vtf_handle_t handle, uint8_t mip, uint16_t frame, uint8_t face); // REQUIRES MANUAL FREE: sourcepp_buffer_free
SOURCEPP_API void vtfpp_vtf_compute_fallback(vtfpp_vtf_handle_t handle, vtfpp_image_conversion_resize_filter_e filter);
SOURCEPP_API void vtfpp_vtf_remove_fallback(vtfpp_vtf_handle_t handle);
SOURCEPP_API int vtfpp_vtf_save_fallback_to_file(vtfpp_vtf_handle_t handle, const char* imagePath, uint8_t mip, uint16_t frame, uint8_t face, vtfpp_image_conversion_file_format_e fileFormat);
SOURCEPP_API uint8_t vtfpp_vtf_get_console_mip_scale(vtfpp_vtf_handle_t handle);
SOURCEPP_API void vtfpp_vtf_set_console_mip_scale(vtfpp_vtf_handle_t handle, uint8_t consoleMipScale);
SOURCEPP_API uint64_t vtfpp_vtf_estimate_bake_size(vtfpp_vtf_handle_t handle, int* isExact);
SOURCEPP_API sourcepp_buffer_t vtfpp_vtf_bake(vtfpp_vtf_handle_t handle); // REQUIRES MANUAL FREE: sourcepp_buffer_free
SOURCEPP_API int vtfpp_vtf_bake_to_file(vtfpp_vtf_handle_t handle, const char* vtfPath);

// C++ conversion routines
#ifdef __cplusplus

#include <vtfpp/VTF.h>

namespace sourceppc::convert {

inline vtfpp::CompressionMethod cast(vtfpp_compression_method_e value) {
	switch (value) {
		case VTFPP_COMPRESSION_METHOD_DEFLATE:      return vtfpp::CompressionMethod::DEFLATE;
		case VTFPP_COMPRESSION_METHOD_ZSTD:         return vtfpp::CompressionMethod::ZSTD;
		case VTFPP_COMPRESSION_METHOD_CONSOLE_LZMA: return vtfpp::CompressionMethod::CONSOLE_LZMA;
	}
	return vtfpp::CompressionMethod::ZSTD;
}

inline vtfpp_compression_method_e cast(vtfpp::CompressionMethod value) {
	switch (value) {
		case vtfpp::CompressionMethod::DEFLATE:      return VTFPP_COMPRESSION_METHOD_DEFLATE;
		case vtfpp::CompressionMethod::ZSTD:         return VTFPP_COMPRESSION_METHOD_ZSTD;
		case vtfpp::CompressionMethod::CONSOLE_LZMA: return VTFPP_COMPRESSION_METHOD_CONSOLE_LZMA;
	}
	return VTFPP_COMPRESSION_METHOD_ZSTD;
}

inline vtfpp::Resource::Type cast(vtfpp_resource_type_e value) {
	switch (value) {
		case VTFPP_RESOURCE_TYPE_UNKNOWN:             return vtfpp::Resource::TYPE_UNKNOWN;
		case VTFPP_RESOURCE_TYPE_THUMBNAIL_DATA:      return vtfpp::Resource::TYPE_THUMBNAIL_DATA;
		case VTFPP_RESOURCE_TYPE_PALETTE_DATA:        return vtfpp::Resource::TYPE_PALETTE_DATA;
		case VTFPP_RESOURCE_TYPE_FALLBACK_DATA:       return vtfpp::Resource::TYPE_FALLBACK_DATA;
		case VTFPP_RESOURCE_TYPE_PARTICLE_SHEET_DATA: return vtfpp::Resource::TYPE_PARTICLE_SHEET_DATA;
		case VTFPP_RESOURCE_TYPE_HOTSPOT_DATA:        return vtfpp::Resource::TYPE_HOTSPOT_DATA;
		case VTFPP_RESOURCE_TYPE_IMAGE_DATA:          return vtfpp::Resource::TYPE_IMAGE_DATA;
		case VTFPP_RESOURCE_TYPE_EXTENDED_FLAGS:      return vtfpp::Resource::TYPE_EXTENDED_FLAGS;
		case VTFPP_RESOURCE_TYPE_CRC:                 return vtfpp::Resource::TYPE_CRC;
		case VTFPP_RESOURCE_TYPE_AUX_COMPRESSION:     return vtfpp::Resource::TYPE_AUX_COMPRESSION;
		case VTFPP_RESOURCE_TYPE_LOD_CONTROL_INFO:    return vtfpp::Resource::TYPE_LOD_CONTROL_INFO;
		case VTFPP_RESOURCE_TYPE_KEYVALUES_DATA:      return vtfpp::Resource::TYPE_KEYVALUES_DATA;
	}
	return vtfpp::Resource::TYPE_UNKNOWN;
}

inline vtfpp_resource_type_e cast(vtfpp::Resource::Type value) {
	switch (value) {
		case vtfpp::Resource::TYPE_UNKNOWN:             return VTFPP_RESOURCE_TYPE_UNKNOWN;
		case vtfpp::Resource::TYPE_THUMBNAIL_DATA:      return VTFPP_RESOURCE_TYPE_THUMBNAIL_DATA;
		case vtfpp::Resource::TYPE_PALETTE_DATA:        return VTFPP_RESOURCE_TYPE_PALETTE_DATA;
		case vtfpp::Resource::TYPE_FALLBACK_DATA:       return VTFPP_RESOURCE_TYPE_FALLBACK_DATA;
		case vtfpp::Resource::TYPE_PARTICLE_SHEET_DATA: return VTFPP_RESOURCE_TYPE_PARTICLE_SHEET_DATA;
		case vtfpp::Resource::TYPE_HOTSPOT_DATA:        return VTFPP_RESOURCE_TYPE_HOTSPOT_DATA;
		case vtfpp::Resource::TYPE_IMAGE_DATA:          return VTFPP_RESOURCE_TYPE_IMAGE_DATA;
		case vtfpp::Resource::TYPE_EXTENDED_FLAGS:      return VTFPP_RESOURCE_TYPE_EXTENDED_FLAGS;
		case vtfpp::Resource::TYPE_CRC:                 return VTFPP_RESOURCE_TYPE_CRC;
		case vtfpp::Resource::TYPE_AUX_COMPRESSION:     return VTFPP_RESOURCE_TYPE_AUX_COMPRESSION;
		case vtfpp::Resource::TYPE_LOD_CONTROL_INFO:    return VTFPP_RESOURCE_TYPE_LOD_CONTROL_INFO;
		case vtfpp::Resource::TYPE_KEYVALUES_DATA:      return VTFPP_RESOURCE_TYPE_KEYVALUES_DATA;
	}
	return VTFPP_RESOURCE_TYPE_UNKNOWN;
}

inline vtfpp::Resource::Flags cast(vtfpp_resource_flags_e flags) {
	return static_cast<vtfpp::Resource::Flags>(flags);
}

inline vtfpp_resource_flags_e cast(vtfpp::Resource::Flags flags) {
	return static_cast<vtfpp_resource_flags_e>(flags);
}

inline vtfpp::VTF::Flags cast(vtfpp_vtf_flags_e flags) {
	return static_cast<vtfpp::VTF::Flags>(flags);
}

inline vtfpp_vtf_flags_e cast(vtfpp::VTF::Flags flags) {
	return static_cast<vtfpp_vtf_flags_e>(flags);
}

inline vtfpp::VTF::Platform cast(vtfpp_vtf_platform_e value) {
	switch (value) {
		case VTFPP_VTF_PLATFORM_UNKNOWN:       return vtfpp::VTF::PLATFORM_UNKNOWN;
		case VTFPP_VTF_PLATFORM_PC:            return vtfpp::VTF::PLATFORM_PC;
		case VTFPP_VTF_PLATFORM_XBOX:          return vtfpp::VTF::PLATFORM_XBOX;
		case VTFPP_VTF_PLATFORM_X360:          return vtfpp::VTF::PLATFORM_X360;
		case VTFPP_VTF_PLATFORM_PS3_ORANGEBOX: return vtfpp::VTF::PLATFORM_PS3_ORANGEBOX;
		case VTFPP_VTF_PLATFORM_PS3_PORTAL2:   return vtfpp::VTF::PLATFORM_PS3_PORTAL2;
	}
	return vtfpp::VTF::PLATFORM_UNKNOWN;
}

inline vtfpp_vtf_platform_e cast(vtfpp::VTF::Platform value) {
	switch (value) {
		case vtfpp::VTF::PLATFORM_UNKNOWN:       return VTFPP_VTF_PLATFORM_UNKNOWN;
		case vtfpp::VTF::PLATFORM_PC:            return VTFPP_VTF_PLATFORM_PC;
		case vtfpp::VTF::PLATFORM_XBOX:          return VTFPP_VTF_PLATFORM_XBOX;
		case vtfpp::VTF::PLATFORM_X360:          return VTFPP_VTF_PLATFORM_X360;
		case vtfpp::VTF::PLATFORM_PS3_ORANGEBOX: return VTFPP_VTF_PLATFORM_PS3_ORANGEBOX;
		case vtfpp::VTF::PLATFORM_PS3_PORTAL2:   return VTFPP_VTF_PLATFORM_PS3_PORTAL2;
	}
	return VTFPP_VTF_PLATFORM_UNKNOWN;
}

inline vtfpp::VTF::CreationOptions cast(const vtfpp_vtf_creation_options_t& value) {
	return {
		.version                  = value.version,
		.outputFormat             = cast(value.outputFormat),
		.compressedFormatQuality  = value.compressedFormatQuality,
		.widthResizeMethod        = cast(value.widthResizeMethod),
		.heightResizeMethod       = cast(value.heightResizeMethod),
		.filter                   = cast(value.filter),
		.flags                    = value.flags,
		.initialFrameCount        = value.initialFrameCount,
		.startFrame               = value.startFrame,
		.isCubeMap                = static_cast<bool>(value.isCubeMap),
		.initialDepth             = value.initialDepth,
		.computeTransparencyFlags = static_cast<bool>(value.computeTransparencyFlags),
		.computeMips              = static_cast<bool>(value.computeMips),
		.computeThumbnail         = static_cast<bool>(value.computeThumbnail),
		.computeReflectivity      = static_cast<bool>(value.computeReflectivity),
		.platform                 = cast(value.platform),
		.compressionLevel         = value.compressionLevel,
		.compressionMethod        = cast(value.compressionMethod),
		.bumpMapScale             = value.bumpMapScale,
		.gammaCorrection          = value.gammaCorrection,
		.invertGreenChannel       = static_cast<bool>(value.invertGreenChannel),
		.consoleMipScale          = value.consoleMipScale,
	};
}

inline vtfpp_vtf_creation_options_t cast(const vtfpp::VTF::CreationOptions& value) {
	return {
		.version                  = value.version,
		.outputFormat             = cast(value.outputFormat),
		.compressedFormatQuality  = value.compressedFormatQuality,
		.widthResizeMethod        = cast(value.widthResizeMethod),
		.heightResizeMethod       = cast(value.heightResizeMethod),
		.filter                   = cast(value.filter),
		.flags                    = value.flags,
		.initialFrameCount        = value.initialFrameCount,
		.startFrame               = value.startFrame,
		.isCubeMap                = value.isCubeMap,
		.initialDepth             = value.initialDepth,
		.computeTransparencyFlags = value.computeTransparencyFlags,
		.computeMips              = value.computeMips,
		.computeThumbnail         = value.computeThumbnail,
		.computeReflectivity      = value.computeReflectivity,
		.platform                 = cast(value.platform),
		.compressionLevel         = value.compressionLevel,
		.compressionMethod        = cast(value.compressionMethod),
		.bumpMapScale             = value.bumpMapScale,
		.gammaCorrection          = value.gammaCorrection,
		.invertGreenChannel       = value.invertGreenChannel,
		.consoleMipScale          = value.consoleMipScale,
	};
}

} // namespace sourceppc::convert

#endif
