#pragma once

#include <utility>

#include <sourcepp/math/Integer.h>

namespace vtfpp {

namespace ImageDimensions {

[[nodiscard]] constexpr uint32_t getMipDim(uint8_t mip, uint16_t dim) {
	// https://stackoverflow.com/a/31962570
	constexpr auto ceil = [](float num) {
		return (static_cast<float>(static_cast<uint32_t>(num)) == num) ? static_cast<uint32_t>(num) : static_cast<uint32_t>(num) + ((num > 0) ? 1 : 0);
	};
	return ceil(static_cast<float>(dim) / static_cast<float>(1 << mip));
}

} // namespace Image

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

[[nodiscard]] constexpr int8_t red(ImageFormat format) {
	switch (format) {
		case ImageFormat::R32F:
		case ImageFormat::RGB323232F:
		case ImageFormat::RGBA32323232F:
			return 32;
		case ImageFormat::RGBA16161616F:
		case ImageFormat::RGBA16161616:
			return 16;
		case ImageFormat::RGBA8888:
		case ImageFormat::ABGR8888:
		case ImageFormat::RGB888:
		case ImageFormat::BGR888:
		case ImageFormat::I8:
		case ImageFormat::IA88:
		case ImageFormat::P8:
		case ImageFormat::RGB888_BLUESCREEN:
		case ImageFormat::BGR888_BLUESCREEN:
		case ImageFormat::ARGB8888:
		case ImageFormat::BGRA8888:
		case ImageFormat::BGRX8888:
		case ImageFormat::UV88:
		case ImageFormat::UVWQ8888:
		case ImageFormat::UVLX8888:
			return 8;
		case ImageFormat::RGB565:
		case ImageFormat::BGR565:
		case ImageFormat::BGRX5551:
		case ImageFormat::BGRA5551:
			return 5;
		case ImageFormat::BGRA4444:
			return 4;
		case ImageFormat::A8:
		case ImageFormat::EMPTY:
			return 0;
		case ImageFormat::DXT1:
		case ImageFormat::DXT3:
		case ImageFormat::DXT5:
		case ImageFormat::DXT1_ONE_BIT_ALPHA:
		case ImageFormat::ATI2N:
		case ImageFormat::ATI1N:
		case ImageFormat::BC7:
		case ImageFormat::BC6H:
			return -1;
	}
	return 0;
}

[[nodiscard]] constexpr int8_t green(ImageFormat format) {
	switch (format) {
		case ImageFormat::RGB323232F:
		case ImageFormat::RGBA32323232F:
			return 32;
		case ImageFormat::RGBA16161616F:
		case ImageFormat::RGBA16161616:
			return 16;
		case ImageFormat::RGBA8888:
		case ImageFormat::ABGR8888:
		case ImageFormat::RGB888:
		case ImageFormat::BGR888:
		case ImageFormat::RGB888_BLUESCREEN:
		case ImageFormat::BGR888_BLUESCREEN:
		case ImageFormat::ARGB8888:
		case ImageFormat::BGRA8888:
		case ImageFormat::BGRX8888:
		case ImageFormat::UV88:
		case ImageFormat::UVWQ8888:
		case ImageFormat::UVLX8888:
			return 8;
		case ImageFormat::RGB565:
		case ImageFormat::BGR565:
			return 6;
		case ImageFormat::BGRX5551:
		case ImageFormat::BGRA5551:
			return 5;
		case ImageFormat::BGRA4444:
			return 4;
		case ImageFormat::I8:
		case ImageFormat::IA88:
		case ImageFormat::P8:
		case ImageFormat::R32F:
		case ImageFormat::A8:
		case ImageFormat::EMPTY:
			return 0;
		case ImageFormat::DXT1:
		case ImageFormat::DXT3:
		case ImageFormat::DXT5:
		case ImageFormat::DXT1_ONE_BIT_ALPHA:
		case ImageFormat::ATI2N:
		case ImageFormat::ATI1N:
		case ImageFormat::BC7:
		case ImageFormat::BC6H:
			return -1;
	}
	return 0;
}

[[nodiscard]] constexpr int8_t blue(ImageFormat format) {
	switch (format) {
		case ImageFormat::RGB323232F:
		case ImageFormat::RGBA32323232F:
			return 32;
		case ImageFormat::RGBA16161616F:
		case ImageFormat::RGBA16161616:
			return 16;
		case ImageFormat::RGBA8888:
		case ImageFormat::ABGR8888:
		case ImageFormat::RGB888:
		case ImageFormat::BGR888:
		case ImageFormat::RGB888_BLUESCREEN:
		case ImageFormat::BGR888_BLUESCREEN:
		case ImageFormat::ARGB8888:
		case ImageFormat::BGRA8888:
		case ImageFormat::BGRX8888:
		case ImageFormat::UVWQ8888:
		case ImageFormat::UVLX8888:
			return 8;
		case ImageFormat::RGB565:
		case ImageFormat::BGR565:
		case ImageFormat::BGRX5551:
		case ImageFormat::BGRA5551:
			return 5;
		case ImageFormat::BGRA4444:
			return 4;
		case ImageFormat::I8:
		case ImageFormat::IA88:
		case ImageFormat::P8:
		case ImageFormat::UV88:
		case ImageFormat::R32F:
		case ImageFormat::A8:
		case ImageFormat::EMPTY:
			return 0;
		case ImageFormat::DXT1:
		case ImageFormat::DXT3:
		case ImageFormat::DXT5:
		case ImageFormat::DXT1_ONE_BIT_ALPHA:
		case ImageFormat::ATI2N:
		case ImageFormat::ATI1N:
		case ImageFormat::BC7:
		case ImageFormat::BC6H:
			return -1;
	}
	return 0;
}

[[nodiscard]] constexpr int8_t alpha(ImageFormat format) {
	switch (format) {
		case ImageFormat::RGBA32323232F:
			return 32;
		case ImageFormat::RGBA16161616F:
		case ImageFormat::RGBA16161616:
			return 16;
		case ImageFormat::RGBA8888:
		case ImageFormat::ABGR8888:
		case ImageFormat::IA88:
		case ImageFormat::ARGB8888:
		case ImageFormat::BGRA8888:
		case ImageFormat::BGRX8888:
		case ImageFormat::UVWQ8888:
		case ImageFormat::UVLX8888:
			return 8;
		case ImageFormat::BGRA4444:
			return 4;
		case ImageFormat::BGRX5551:
		case ImageFormat::BGRA5551:
			return 1;
		case ImageFormat::RGB888:
		case ImageFormat::BGR888:
		case ImageFormat::P8:
		case ImageFormat::I8:
		case ImageFormat::RGB888_BLUESCREEN:
		case ImageFormat::BGR888_BLUESCREEN:
		case ImageFormat::UV88:
		case ImageFormat::RGB565:
		case ImageFormat::BGR565:
		case ImageFormat::R32F:
		case ImageFormat::RGB323232F:
		case ImageFormat::A8:
		case ImageFormat::EMPTY:
			return 0;
		case ImageFormat::DXT1:
		case ImageFormat::DXT3:
		case ImageFormat::DXT5:
		case ImageFormat::DXT1_ONE_BIT_ALPHA:
		case ImageFormat::ATI2N:
		case ImageFormat::ATI1N:
		case ImageFormat::BC7:
		case ImageFormat::BC6H:
			return -1;
	}
	return 0;
}

[[nodiscard]] constexpr uint8_t bpp(ImageFormat format) {
	switch (format) {
		using enum ImageFormat;
		case RGBA32323232F:
			return 128;
		case RGB323232F:
			return 96;
		case RGBA16161616F:
		case RGBA16161616:
			return 64;
		case RGBA8888:
		case ABGR8888:
		case ARGB8888:
		case BGRA8888:
		case BGRX8888:
		case UVLX8888:
		case R32F:
		case UVWQ8888:
			return 32;
		case RGB888:
		case BGR888:
		case RGB888_BLUESCREEN:
		case BGR888_BLUESCREEN:
		case BGR565:
			return 24;
		case RGB565:
		case IA88:
		case BGRX5551:
		case BGRA4444:
		case BGRA5551:
		case UV88:
			return 16;
		case I8:
		case P8:
		case A8:
		case DXT3:
		case DXT5:
		case BC7:
		case BC6H:
		case ATI2N:
			return 8;
		case ATI1N:
		case DXT1:
		case DXT1_ONE_BIT_ALPHA:
			return 4;
		case EMPTY:
			return 0;
	}
	return 0;
}

[[nodiscard]] constexpr bool compressed(ImageFormat format) {
	return red(format) == -1;
}

[[nodiscard]] constexpr bool large(ImageFormat format) {
	return red(format) > 8 || bpp(format) > 32;
}

[[nodiscard]] constexpr uint32_t getDataLength(ImageFormat format, uint16_t width, uint16_t height, uint16_t sliceCount = 1) {
	switch(format) {
		using enum ImageFormat;
		case DXT3:
		case DXT5:
		case ATI2N:
		case BC7:
		case BC6H:
		case ATI1N:
		case DXT1:
		case DXT1_ONE_BIT_ALPHA:
			return std::max((width + 3) / 4, 1) * std::max((height + 3) / 4, 1) * sliceCount * bpp(format) * 2;
		default:
			break;
	}
	uint32_t out = width * height * sliceCount * bpp(format) / 8;
	return out;
}

[[nodiscard]] constexpr uint32_t getDataLength(ImageFormat format, uint8_t mipCount, uint16_t frameCount, uint16_t faceCount, uint16_t width, uint16_t height, uint16_t sliceCount = 1) {
	uint32_t length = 0;
	for (int mip = mipCount - 1; mip >= 0; mip--) {
		length += ImageFormatDetails::getDataLength(format, ImageDimensions::getMipDim(mip, width), ImageDimensions::getMipDim(mip, height), sliceCount) * frameCount * faceCount;
	}
	return length;
}

[[nodiscard]] constexpr bool getDataPosition(uint32_t& offset, uint32_t& length, ImageFormat format, uint8_t mip, uint8_t mipCount,  uint16_t frame, uint16_t frameCount, uint16_t face, uint16_t faceCount, uint16_t width, uint16_t height, uint16_t slice = 0, uint16_t sliceCount = 1) {
	offset = 0;
	length = 0;
	for (int i = mipCount - 1; i >= 0; i--) {
		for (int j = 0; j < frameCount; j++) {
			for (int k = 0; k < faceCount; k++) {
				for (int l = 0; l < sliceCount; l++) {
					auto imageSize = ImageFormatDetails::getDataLength(format, ImageDimensions::getMipDim(i, width), ImageDimensions::getMipDim(i, height));
					if (i == mip && j == frame && k == face && l == slice) {
						length = imageSize;
						return true;
					} else {
						offset += imageSize;
					}
				}
			}
		}
	}
	return false;
}

} // namespace ImageFormatDetails

} // namespace vtfpp
