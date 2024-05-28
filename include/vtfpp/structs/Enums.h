#pragma once

#include <sourcepp/math/Integer.h>

namespace vtfpp {

enum class VTFFlag : int32_t {
	NONE                                    = 0,
	POINT_SAMPLE                            = 1 <<  0,
	TRILINEAR                               = 1 <<  1,
	CLAMP_S                                 = 1 <<  2,
	CLAMP_T                                 = 1 <<  3,
	ANISOTROPIC                             = 1 <<  4,
	HINT_DXT5                               = 1 <<  5,
	SRGB                                    = 1 <<  6,
	NO_COMPRESS                             =    SRGB, // Internal to vtex, removed
	NORMAL                                  = 1 <<  7,
	NO_MIP                                  = 1 <<  8,
	NO_LOD                                  = 1 <<  9,
	MIN_MIP                                 = 1 << 10,
	PROCEDURAL                              = 1 << 11,
	ONE_BIT_ALPHA                           = 1 << 12, // Added at VTF creation time
	MULTI_BIT_ALPHA                         = 1 << 13, // Added at VTF creation time
	ENVMAP                                  = 1 << 14,
	RENDERTARGET                            = 1 << 15,
	DEPTH_RENDERTARGET                      = 1 << 16,
	NO_DEBUG_OVERRIDE                       = 1 << 17,
	SINGLE_COPY                             = 1 << 18,
	ONE_OVER_MIP_LEVEL_IN_ALPHA             = 1 << 19, // Internal to vtex, removed
	PREMULTIPLY_COLOR_BY_ONE_OVER_MIP_LEVEL = 1 << 20, // Internal to vtex, removed
	NORMAL_TO_DUDV                          = 1 << 21, // Internal to vtex, removed
	ALPHA_TEST_MIP_GENERATION               = 1 << 22,
	NO_DEPTH_BUFFER                         = 1 << 23,
	NICE_FILTERED                           = 1 << 24, // Internal to vtex, removed
	CLAMP_U                                 = 1 << 25,
	VERTEX_TEXTURE                          = 1 << 26,
	SSBUMP                                  = 1 << 27,
	UNFILTERABLE_OK                         = 1 << 28, // Removed
	BORDER                                  = 1 << 29,
	SPECVAR_RED                             = 1 << 30, // Removed
	SPECVAR_ALPHA                           = 1 << 31, // Removed
};

enum class ImageFormat : int32_t {
	RGBA8888 = 0,
	ABGR8888,
	RGB888,
	BGR888,
	RGB565,
	I8,
	IA88,
	P8,
	A8,
	RGB888_BLUESCREEN,
	BGR888_BLUESCREEN,
	ARGB8888,
	BGRA8888,
	DXT1,
	DXT3,
	DXT5,
	BGRX8888,
	BGR565,
	BGRX5551,
	BGRA4444,
	DXT1_ONE_BIT_ALPHA,
	BGRA5551,
	UV88,
	UVWQ8888,
	RGBA16161616F,
	RGBA16161616,
	UVLX8888,
	R32F,
	RGB323232F,
	RGBA32323232F,

	EMPTY = 33,
	ATI2N,
	ATI1N,

	BC7 = 70,
	BC6H,
};

namespace ImageFormatDetails {

uint8_t bpp(ImageFormat format);

uint32_t dataLength(ImageFormat format, uint32_t width, uint32_t height, uint32_t depth = 1);

} // namespace ImageFormatDetails

enum class ResourceType : uint8_t {
	THUMBNAIL_DATA      = '\x01', // \x01\0\0
	IMAGE_DATA          = '\x30', // \x30\0\0
	PARTICLE_SHEET_DATA = '\x10', // \x10\0\0
	CRC                 = 'C',    // CRC
	LOD_CONTROL_INFO    = 'L',    // LOD
	EXTENDED_FLAGS      = 'T',    // TSO
	KEYVALUES_DATA      = 'K',    // KVD
	AUX_COMPRESSION     = 'A',    // AXC
};

enum class ResourceFlag : uint8_t {
	NONE    = 0,
	NO_DATA = 1 << 1,
};

} // namespace vtfpp
