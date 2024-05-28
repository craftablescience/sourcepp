#pragma once

#include <sourcepp/math/Integer.h>

namespace vtfpp {

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

[[nodiscard]] bool compressed(ImageFormat format);

[[nodiscard]] int8_t red(ImageFormat format);

[[nodiscard]] int8_t green(ImageFormat format);

[[nodiscard]] int8_t blue(ImageFormat format);

[[nodiscard]] int8_t alpha(ImageFormat format);

[[nodiscard]] uint8_t bpp(ImageFormat format);

[[nodiscard]] uint32_t getDataLength(ImageFormat format, uint16_t width, uint16_t height, uint16_t sliceCount = 1);

[[nodiscard]] uint32_t getDataLength(ImageFormat format, uint8_t mipCount, uint16_t frameCount, uint16_t faceCount, uint16_t width, uint16_t height, uint16_t sliceCount = 1);

[[nodiscard]] bool getDataPosition(uint32_t& offset, uint32_t& length, ImageFormat format, uint8_t mip, uint8_t mipCount,  uint16_t frame, uint16_t frameCount, uint16_t face, uint16_t faceCount, uint16_t width, uint16_t height, uint16_t slice = 0, uint16_t sliceCount = 1);

} // namespace ImageFormatDetails

} // namespace vtfpp
