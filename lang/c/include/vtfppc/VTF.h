#pragma once

#include <sourceppc/API.h>
#include <sourceppc/Buffer.h>
#include <sourceppc/String.h>

#include "HOT.h"
#include "ImageConversion.h"
#include "SHT.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
	VTFPP_COMPRESSION_METHOD_DEFLATE = 8,
	VTFPP_COMPRESSION_METHOD_ZSTD = 93,
	VTFPP_COMPRESSION_METHOD_CONSOLE_LZMA = 0x360,
} vtfpp_compression_method_e;

typedef enum {
	VTFPP_RESOURCE_TYPE_UNKNOWN,
	VTFPP_RESOURCE_TYPE_THUMBNAIL_DATA,
	VTFPP_RESOURCE_TYPE_PARTICLE_SHEET_DATA,
	VTFPP_RESOURCE_TYPE_HOTSPOT_DATA,
	VTFPP_RESOURCE_TYPE_IMAGE_DATA,
	VTFPP_RESOURCE_TYPE_EXTENDED_FLAGS,
	VTFPP_RESOURCE_TYPE_CRC,
	VTFPP_RESOURCE_TYPE_AUX_COMPRESSION,
	VTFPP_RESOURCE_TYPE_LOD_CONTROL_INFO,
	VTFPP_RESOURCE_TYPE_KEYVALUES_DATA,
} vtfpp_resource_type_e;

typedef enum {
	VTFPP_RESOURCE_FLAG_NONE       = 0,
	VTFPP_RESOURCE_FLAG_LOCAL_DATA = 1 << 1,
} vtfpp_resource_flags_e;

typedef const void* vtfpp_resource_handle_t;

typedef enum {
	VTFPP_VTF_FLAG_POINT_SAMPLE                  = 1 <<  0,
	VTFPP_VTF_FLAG_TRILINEAR                     = 1 <<  1,
	VTFPP_VTF_FLAG_CLAMP_S                       = 1 <<  2,
	VTFPP_VTF_FLAG_CLAMP_T                       = 1 <<  3,
	VTFPP_VTF_FLAG_ANISOTROPIC                   = 1 <<  4,
	VTFPP_VTF_FLAG_HINT_DXT5                     = 1 <<  5,
	VTFPP_VTF_FLAG_NORMAL                        = 1 <<  7,
	VTFPP_VTF_FLAG_NO_MIP                        = 1 <<  8, // Controlled by mip count
	VTFPP_VTF_FLAG_NO_LOD                        = 1 <<  9,
	VTFPP_VTF_FLAG_MIN_MIP                       = 1 << 10,
	VTFPP_VTF_FLAG_PROCEDURAL                    = 1 << 11,
	VTFPP_VTF_FLAG_ONE_BIT_ALPHA                 = 1 << 12,
	VTFPP_VTF_FLAG_MULTI_BIT_ALPHA               = 1 << 13,
	VTFPP_VTF_FLAG_ENVMAP                        = 1 << 14, // Controlled by face count
	VTFPP_VTF_FLAG_RENDERTARGET                  = 1 << 15,
	VTFPP_VTF_FLAG_DEPTH_RENDERTARGET            = 1 << 16,
	VTFPP_VTF_FLAG_NO_DEBUG_OVERRIDE             = 1 << 17,
	VTFPP_VTF_FLAG_SINGLE_COPY                   = 1 << 18,
} vtfpp_vtf_flags_v0_e;
#define VTFPP_VTF_FLAGS_MASK_V0 (VTFPP_VTF_FLAG_POINT_SAMPLE | VTFPP_VTF_FLAG_TRILINEAR | VTFPP_VTF_FLAG_CLAMP_S | VTFPP_VTF_FLAG_CLAMP_T | VTFPP_VTF_FLAG_ANISOTROPIC | VTFPP_VTF_FLAG_HINT_DXT5 | VTFPP_VTF_FLAG_NORMAL | VTFPP_VTF_FLAG_NO_MIP | VTFPP_VTF_FLAG_NO_LOD | VTFPP_VTF_FLAG_MIN_MIP | VTFPP_VTF_FLAG_PROCEDURAL | VTFPP_VTF_FLAG_ONE_BIT_ALPHA | VTFPP_VTF_FLAG_MULTI_BIT_ALPHA | VTFPP_VTF_FLAG_ENVMAP | VTFPP_VTF_FLAG_RENDERTARGET | FLAG_DEPTH_RENDERTARGET | VTFPP_VTF_FLAG_NO_DEBUG_OVERRIDE | VTFPP_VTF_FLAG_SINGLE_COPY)

typedef enum {
	VTFPP_VTF_FLAG_V2_NO_DEPTH_BUFFER            = 1 << 23,
	VTFPP_VTF_FLAG_V2_CLAMP_U                    = 1 << 25,
} vtfpp_vtf_flags_v2_e;
#define VTFPP_VTF_FLAGS_MASK_V2 (VTFPP_VTF_FLAG_V2_NO_DEPTH_BUFFER | VTFPP_VTF_FLAG_V2_CLAMP_U)

enum FlagsV3 : uint32_t {
	VTFPP_VTF_FLAG_V3_LOAD_ALL_MIPS              = 1 << 10,
	VTFPP_VTF_FLAG_V3_VERTEX_TEXTURE             = 1 << 26,
	VTFPP_VTF_FLAG_V3_SSBUMP                     = 1 << 27,
	VTFPP_VTF_FLAG_V3_BORDER                     = 1 << 29,
};
#define VTFPP_VTF_FLAGS_MASK_V3 (VTFPP_VTF_FLAG_V3_LOAD_ALL_MIPS | VTFPP_VTF_FLAG_V3_VERTEX_TEXTURE | VTFPP_VTF_FLAG_V3_SSBUMP | VTFPP_VTF_FLAG_V3_BORDER)

typedef enum {
	FLAG_V4_SRGB                       = 1 <<  6,
} vtfpp_vtf_flags_v4_e;
#define VTFPP_VTF_FLAGS_MASK_V4 (VTFPP_VTF_FLAG_V4_SRGB)

typedef enum {
	VTFPP_VTF_FLAG_V4_TF2_STAGING_MEMORY         = 1 << 19,
	VTFPP_VTF_FLAG_V4_TF2_IMMEDIATE_CLEANUP      = 1 << 20,
	VTFPP_VTF_FLAG_V4_TF2_IGNORE_PICMIP          = 1 << 21,
	VTFPP_VTF_FLAG_V4_TF2_STREAMABLE_COARSE      = 1 << 30,
	VTFPP_VTF_FLAG_V4_TF2_STREAMABLE_FINE        = 1 << 31,
} vtfpp_vtf_flags_v4_tf2_e;
#define VTFPP_VTF_FLAGS_MASK_V4_TF2 (VTFPP_VTF_FLAG_V4_TF2_STAGING_MEMORY | VTFPP_VTF_FLAG_V4_TF2_IMMEDIATE_CLEANUP | VTFPP_VTF_FLAG_V4_TF2_IGNORE_PICMIP | VTFPP_VTF_FLAG_V4_TF2_STREAMABLE_COARSE | VTFPP_VTF_FLAG_V4_TF2_STREAMABLE_FINE)

typedef enum {
	VTFPP_VTF_FLAG_V5_PWL_CORRECTED              = 1 <<  6,
	VTFPP_VTF_FLAG_V5_SRGB                       = 1 << 19,
	VTFPP_VTF_FLAG_V5_DEFAULT_POOL               = 1 << 20,
	VTFPP_VTF_FLAG_V5_LOAD_MOST_MIPS             = 1 << 28,
} vtfpp_vtf_flags_v5_e;
#define VTFPP_VTF_FLAGS_MASK_V5 (VTFPP_VTF_FLAG_V5_PWL_CORRECTED | VTFPP_VTF_FLAG_V5_SRGB | VTFPP_VTF_FLAG_V5_DEFAULT_POOL | VTFPP_VTF_FLAG_V5_LOAD_MOST_MIPS)

typedef enum {
	VTFPP_VTF_FLAG_V5_CSGO_COMBINED                   = 1 << 21,
	VTFPP_VTF_FLAG_V5_CSGO_ASYNC_DOWNLOAD             = 1 << 22,
	VTFPP_VTF_FLAG_V5_CSGO_SKIP_INITIAL_DOWNLOAD      = 1 << 24,
	VTFPP_VTF_FLAG_V5_CSGO_YCOCG                      = 1 << 30,
	VTFPP_VTF_FLAG_V5_CSGO_ASYNC_SKIP_INITIAL_LOW_RES = 1 << 31,
} vtfpp_vtf_flags_v5_csgo_e;
#define VTFPP_VTF_FLAGS_MASK_V5_CSGO (VTFPP_VTF_FLAG_V5_CSGO_COMBINED | VTFPP_VTF_FLAG_V5_CSGO_ASYNC_DOWNLOAD | VTFPP_VTF_FLAG_V5_CSGO_SKIP_INITIAL_DOWNLOAD | VTFPP_VTF_FLAG_V5_CSGO_YCOCG | VTFPP_VTF_FLAG_V5_CSGO_ASYNC_SKIP_INITIAL_LOW_RES)

#define VTFPP_VTF_FLAGS_MASK_INTERNAL (VTFPP_VTF_FLAG_NO_MIP | VTFPP_VTF_FLAG_ENVMAP)

typedef enum {
	VTFPP_VTF_PLATFORM_UNKNOWN       = 0x000,
	VTFPP_VTF_PLATFORM_PC            = 0x001,
	VTFPP_VTF_PLATFORM_X360          = 0x360,
	VTFPP_VTF_PLATFORM_PS3_ORANGEBOX = 0x333,
	VTFPP_VTF_PLATFORM_PS3_PORTAL2   = 0x003,
} vtfpp_vtf_platform_e;

#define VTFPP_VTF_FORMAT_UNCHANGED ((vtfpp_image_format_e) -2)
#define VTFPP_VTF_FORMAT_DEFAULT   ((vtfpp_image_format_e) -1)

typedef struct {
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
})

typedef void* vtfpp_vtf_handle_t;

#ifdef __cplusplus
} // extern "C"
#endif

SOURCEPP_API vtfpp_resource_type_e vtfpp_resource_get_type(vtfpp_resource_handle_t handle);

SOURCEPP_API vtfpp_resource_flags_e vtfpp_resource_get_flags(vtfpp_resource_handle_t handle);

SOURCEPP_API const unsigned char* vtfpp_resource_get_data(vtfpp_resource_handle_t handle, size_t* dataLen);

// REQUIRES MANUAL FREE: vtfpp_sht_close
SOURCEPP_API vtfpp_sht_handle_t vtfpp_resource_get_data_as_particle_sheet(vtfpp_resource_handle_t handle);

SOURCEPP_API uint32_t vtfpp_resource_get_data_as_crc(vtfpp_resource_handle_t handle);

SOURCEPP_API uint32_t vtfpp_resource_get_data_as_extended_flags(vtfpp_resource_handle_t handle);

SOURCEPP_API void vtfpp_resource_get_data_as_lod(vtfpp_resource_handle_t handle, uint8_t* u, uint8_t* v, uint8_t* u360, uint8_t* v360);

// REQUIRES MANUAL FREE: sourcepp_string_free
SOURCEPP_API sourcepp_string_t vtfpp_resource_get_data_as_keyvalues_data(vtfpp_resource_handle_t handle);

// REQUIRES MANUAL FREE: vtfpp_hot_close
SOURCEPP_API vtfpp_hot_handle_t vtfpp_resource_get_data_as_hotspot_data(vtfpp_resource_handle_t handle);

SOURCEPP_API int16_t vtfpp_resource_get_data_as_aux_compression_level(vtfpp_resource_handle_t handle);

SOURCEPP_API vtfpp_compression_method_e vtfpp_resource_get_data_as_aux_compression_method(vtfpp_resource_handle_t handle);

SOURCEPP_API uint32_t vtfpp_resource_get_data_as_aux_compression_length(vtfpp_resource_handle_t handle, uint8_t mip, uint8_t mipCount, uint16_t frame, uint16_t frameCount, uint16_t face, uint16_t faceCount);

// REQUIRES MANUAL FREE: vtfpp_vtf_close
SOURCEPP_API vtfpp_vtf_handle_t vtfpp_vtf_create_empty();

// REQUIRES MANUAL FREE: vtfpp_vtf_close
SOURCEPP_API vtfpp_vtf_handle_t vtfpp_vtf_open_from_mem(const unsigned char* vtfData, size_t vtfLen, int parseHeaderOnly);

// REQUIRES MANUAL FREE: vtfpp_vtf_close
SOURCEPP_API vtfpp_vtf_handle_t vtfpp_vtf_open_from_file(const char* vtfPath, int parseHeaderOnly);

SOURCEPP_API int vtfpp_vtf_is_valid(vtfpp_vtf_handle_t handle);

SOURCEPP_API int vtfpp_vtf_create_from_mem_and_bake(const unsigned char* imageData, size_t imageLen, vtfpp_image_format_e format, uint16_t width, uint16_t height, const char* vtfPath, vtfpp_vtf_creation_options_t options);

SOURCEPP_API int vtfpp_vtf_create_blank_and_bake(vtfpp_image_format_e format, uint16_t width, uint16_t height, const char* vtfPath, vtfpp_vtf_creation_options_t options);

// REQUIRES MANUAL FREE: vtfpp_vtf_close
SOURCEPP_API vtfpp_vtf_handle_t vtfpp_vtf_create_from_mem(const unsigned char* imageData, size_t imageLen, vtfpp_image_format_e format, uint16_t width, uint16_t height, vtfpp_vtf_creation_options_t options);

// REQUIRES MANUAL FREE: vtfpp_vtf_close
SOURCEPP_API vtfpp_vtf_handle_t vtfpp_vtf_create_blank(vtfpp_image_format_e format, uint16_t width, uint16_t height, vtfpp_vtf_creation_options_t options);

SOURCEPP_API int vtfpp_vtf_create_from_file_and_bake(const char* imagePath, const char* vtfPath, vtfpp_vtf_creation_options_t options);

// REQUIRES MANUAL FREE: vtfpp_vtf_close
SOURCEPP_API vtfpp_vtf_handle_t vtfpp_vtf_create_from_file(const char* imagePath, vtfpp_vtf_creation_options_t options);

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

SOURCEPP_API uint16_t vtfpp_vtf_get_height(vtfpp_vtf_handle_t handle, uint8_t mip);

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

SOURCEPP_API uint32_t vtfpp_vtf_get_resources_count(vtfpp_vtf_handle_t handle);

SOURCEPP_API vtfpp_resource_handle_t vtfpp_vtf_get_resource_at_index(vtfpp_vtf_handle_t handle, uint32_t index);

SOURCEPP_API vtfpp_resource_handle_t vtfpp_vtf_get_resource_with_type(vtfpp_vtf_handle_t handle, vtfpp_resource_type_e type);

// REQUIRES MANUAL FREE: sourcepp_buffer_free
SOURCEPP_API sourcepp_buffer_t vtfpp_vtf_get_particle_sheet_frame_data_raw(vtfpp_vtf_handle_t handle, uint16_t* width, uint16_t* height, uint32_t shtSequenceID, uint32_t shtFrame, uint8_t shtBounds, uint8_t mip, uint16_t frame, uint8_t face, uint16_t slice);

// REQUIRES MANUAL FREE: sourcepp_buffer_free
SOURCEPP_API sourcepp_buffer_t vtfpp_vtf_get_particle_sheet_frame_data_as(vtfpp_vtf_handle_t handle, vtfpp_image_format_e format, uint16_t* width, uint16_t* height, uint32_t shtSequenceID, uint32_t shtFrame, uint8_t shtBounds, uint8_t mip, uint16_t frame, uint8_t face, uint16_t slice);

// REQUIRES MANUAL FREE: sourcepp_buffer_free
SOURCEPP_API sourcepp_buffer_t vtfpp_vtf_get_particle_sheet_frame_data_as_rgba8888(vtfpp_vtf_handle_t handle, uint16_t* width, uint16_t* height, uint32_t shtSequenceID, uint32_t shtFrame, uint8_t shtBounds, uint8_t mip, uint16_t frame, uint8_t face, uint16_t slice);

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

// REQUIRES MANUAL FREE: sourcepp_buffer_free
SOURCEPP_API sourcepp_buffer_t vtfpp_vtf_get_image_data_as(vtfpp_vtf_handle_t handle, vtfpp_image_format_e format, uint8_t mip, uint16_t frame, uint8_t face, uint16_t slice);

// REQUIRES MANUAL FREE: sourcepp_buffer_free
SOURCEPP_API sourcepp_buffer_t vtfpp_vtf_get_image_data_as_rgba8888(vtfpp_vtf_handle_t handle, uint8_t mip, uint16_t frame, uint8_t face, uint16_t slice);

SOURCEPP_API int vtfpp_vtf_set_image_from_mem(vtfpp_vtf_handle_t handle, const unsigned char* imageData, size_t imageLen, vtfpp_image_format_e format, uint16_t width, uint16_t height, vtfpp_image_conversion_resize_filter_e filter, uint8_t mip, uint16_t frame, uint8_t face, uint16_t slice, float quality);

SOURCEPP_API int vtfpp_vtf_set_image_from_file(vtfpp_vtf_handle_t handle, const char* imagePath, vtfpp_image_conversion_resize_filter_e filter, uint8_t mip, uint16_t frame, uint8_t face, uint16_t slice, float quality);

SOURCEPP_API int vtfpp_vtf_save_image_to_file(vtfpp_vtf_handle_t handle, const char* imagePath, uint8_t mip, uint16_t frame, uint8_t face, uint16_t slice, vtfpp_image_conversion_file_format_e fileFormat);

SOURCEPP_API int vtfpp_vtf_has_thumbnail_data(vtfpp_vtf_handle_t handle);

SOURCEPP_API const unsigned char* vtfpp_vtf_get_thumbnail_data_raw(vtfpp_vtf_handle_t handle, size_t* imageLen);

// REQUIRES MANUAL FREE: sourcepp_buffer_free
SOURCEPP_API sourcepp_buffer_t vtfpp_vtf_get_thumbnail_data_as(vtfpp_vtf_handle_t handle, vtfpp_image_format_e format);

// REQUIRES MANUAL FREE: sourcepp_buffer_free
SOURCEPP_API sourcepp_buffer_t vtfpp_vtf_get_thumbnail_data_as_rgba8888(vtfpp_vtf_handle_t handle);

SOURCEPP_API void vtfpp_vtf_set_thumbnail(vtfpp_vtf_handle_t handle, const unsigned char* imageData, size_t imageLen, vtfpp_image_format_e format, uint16_t width, uint16_t height, float quality);

SOURCEPP_API void vtfpp_vtf_compute_thumbnail(vtfpp_vtf_handle_t handle, vtfpp_image_conversion_resize_filter_e filter, float quality);

SOURCEPP_API void vtfpp_vtf_remove_thumbnail(vtfpp_vtf_handle_t handle);

SOURCEPP_API int vtfpp_vtf_save_thumbnail_to_file(vtfpp_vtf_handle_t handle, const char* imagePath, vtfpp_image_conversion_file_format_e fileFormat);

// REQUIRES MANUAL FREE: sourcepp_buffer_free
SOURCEPP_API sourcepp_buffer_t vtfpp_vtf_bake(vtfpp_vtf_handle_t handle);

SOURCEPP_API int vtfpp_vtf_bake_to_file(vtfpp_vtf_handle_t handle, const char* vtfPath);

SOURCEPP_API void vtfpp_vtf_close(vtfpp_vtf_handle_t* handle);
