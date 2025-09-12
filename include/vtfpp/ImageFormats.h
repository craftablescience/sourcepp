#pragma once

#include <sourcepp/Math.h>

namespace vtfpp {

enum class ImageFormat : int32_t {
	// region Universal Formats
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
	// endregion

	// region Alien Swarm & Beyond Formats
	RG1616F,
	RG3232F,
	RGBX8888,
	EMPTY,
	ATI2N,
	ATI1N,
	RGBA1010102,
	BGRA1010102,
	R16F,
	// endregion

	// region Console Formats
	CONSOLE_BGRX8888_LINEAR = 42,
	CONSOLE_RGBA8888_LINEAR,
	CONSOLE_ABGR8888_LINEAR,
	CONSOLE_ARGB8888_LINEAR,
	CONSOLE_BGRA8888_LINEAR,
	CONSOLE_RGB888_LINEAR,
	CONSOLE_BGR888_LINEAR,
	CONSOLE_BGRX5551_LINEAR,
	CONSOLE_I8_LINEAR,
	CONSOLE_RGBA16161616_LINEAR,
	CONSOLE_BGRX8888_LE,
	CONSOLE_BGRA8888_LE,
	// endregion

	// region Strata Source Formats
	R8 = 69,
	BC7,
	BC6H,
	// endregion
};

namespace ImageFormatDetails {

[[nodiscard]] constexpr int8_t red(ImageFormat format) {
	switch (format) {
		using enum ImageFormat;
		case R32F:
		case RG3232F:
		case RGB323232F:
		case RGBA32323232F:
			return 32;
		case R16F:
		case RG1616F:
		case RGBA16161616F:
		case RGBA16161616:
		case CONSOLE_RGBA16161616_LINEAR:
			return 16;
		case RGBA1010102:
		case BGRA1010102:
			return 10;
		case RGBA8888:
		case CONSOLE_RGBA8888_LINEAR:
		case ABGR8888:
		case CONSOLE_ABGR8888_LINEAR:
		case RGB888:
		case CONSOLE_RGB888_LINEAR:
		case BGR888:
		case CONSOLE_BGR888_LINEAR:
		case I8:
		case CONSOLE_I8_LINEAR:
		case IA88:
		case P8:
		case RGB888_BLUESCREEN:
		case BGR888_BLUESCREEN:
		case ARGB8888:
		case CONSOLE_ARGB8888_LINEAR:
		case BGRA8888:
		case CONSOLE_BGRA8888_LINEAR:
		case CONSOLE_BGRA8888_LE:
		case BGRX8888:
		case CONSOLE_BGRX8888_LINEAR:
		case CONSOLE_BGRX8888_LE:
		case UV88:
		case UVWQ8888:
		case UVLX8888:
		case RGBX8888:
		case R8:
			return 8;
		case RGB565:
		case BGR565:
		case BGRX5551:
		case CONSOLE_BGRX5551_LINEAR:
		case BGRA5551:
			return 5;
		case BGRA4444:
			return 4;
		case A8:
		case EMPTY:
			return 0;
		case DXT1:
		case DXT3:
		case DXT5:
		case DXT1_ONE_BIT_ALPHA:
		case ATI2N:
		case ATI1N:
		case BC7:
		case BC6H:
			return -1;
	}
	return 0;
}

[[nodiscard]] constexpr int8_t decompressedRed(ImageFormat format) {
	// This is merely for convenience, the true size may be different depending on the data
	switch (format) {
		using enum ImageFormat;
		case DXT1:
		case DXT1_ONE_BIT_ALPHA:
		case DXT3:
		case DXT5:
		case ATI2N:
		case ATI1N:
		case BC7:
			return 8;
		case BC6H:
			return 16;
		default:
			break;
	}
	return red(format);
}

[[nodiscard]] constexpr int8_t green(ImageFormat format) {
	switch (format) {
		using enum ImageFormat;
		case RG3232F:
		case RGB323232F:
		case RGBA32323232F:
			return 32;
		case RG1616F:
		case RGBA16161616F:
		case RGBA16161616:
		case CONSOLE_RGBA16161616_LINEAR:
			return 16;
		case RGBA1010102:
		case BGRA1010102:
			return 10;
		case RGBA8888:
		case CONSOLE_RGBA8888_LINEAR:
		case ABGR8888:
		case CONSOLE_ABGR8888_LINEAR:
		case RGB888:
		case CONSOLE_RGB888_LINEAR:
		case BGR888:
		case CONSOLE_BGR888_LINEAR:
		case RGB888_BLUESCREEN:
		case BGR888_BLUESCREEN:
		case ARGB8888:
		case CONSOLE_ARGB8888_LINEAR:
		case BGRA8888:
		case CONSOLE_BGRA8888_LINEAR:
		case CONSOLE_BGRA8888_LE:
		case BGRX8888:
		case CONSOLE_BGRX8888_LINEAR:
		case CONSOLE_BGRX8888_LE:
		case UV88:
		case UVWQ8888:
		case UVLX8888:
		case RGBX8888:
			return 8;
		case RGB565:
		case BGR565:
			return 6;
		case BGRX5551:
		case CONSOLE_BGRX5551_LINEAR:
		case BGRA5551:
			return 5;
		case BGRA4444:
			return 4;
		case I8:
		case CONSOLE_I8_LINEAR:
		case IA88:
		case P8:
		case R32F:
		case A8:
		case EMPTY:
		case R16F:
		case R8:
			return 0;
		case DXT1:
		case DXT3:
		case DXT5:
		case DXT1_ONE_BIT_ALPHA:
		case ATI2N:
		case ATI1N:
		case BC7:
		case BC6H:
			return -1;
	}
	return 0;
}

[[nodiscard]] constexpr int8_t decompressedGreen(ImageFormat format) {
	// This is merely for convenience, the true size may be different depending on the data
	switch (format) {
		using enum ImageFormat;
		case DXT1:
		case DXT1_ONE_BIT_ALPHA:
		case DXT3:
		case DXT5:
		case ATI2N:
		case ATI1N:
		case BC7:
			return 8;
		case BC6H:
			return 16;
		default:
			break;
	}
	return green(format);
}

[[nodiscard]] constexpr int8_t blue(ImageFormat format) {
	switch (format) {
		using enum ImageFormat;
		case RGB323232F:
		case RGBA32323232F:
			return 32;
		case RGBA16161616F:
		case RGBA16161616:
		case CONSOLE_RGBA16161616_LINEAR:
			return 16;
		case RGBA1010102:
		case BGRA1010102:
			return 10;
		case RGBA8888:
		case CONSOLE_RGBA8888_LINEAR:
		case ABGR8888:
		case CONSOLE_ABGR8888_LINEAR:
		case RGB888:
		case CONSOLE_RGB888_LINEAR:
		case BGR888:
		case CONSOLE_BGR888_LINEAR:
		case RGB888_BLUESCREEN:
		case BGR888_BLUESCREEN:
		case ARGB8888:
		case CONSOLE_ARGB8888_LINEAR:
		case BGRA8888:
		case CONSOLE_BGRA8888_LINEAR:
		case CONSOLE_BGRA8888_LE:
		case BGRX8888:
		case CONSOLE_BGRX8888_LINEAR:
		case CONSOLE_BGRX8888_LE:
		case UVWQ8888:
		case UVLX8888:
		case RGBX8888:
			return 8;
		case RGB565:
		case BGR565:
		case BGRX5551:
		case CONSOLE_BGRX5551_LINEAR:
		case BGRA5551:
			return 5;
		case BGRA4444:
			return 4;
		case I8:
		case CONSOLE_I8_LINEAR:
		case IA88:
		case P8:
		case UV88:
		case R32F:
		case A8:
		case EMPTY:
		case RG3232F:
		case RG1616F:
		case R16F:
		case R8:
			return 0;
		case DXT1:
		case DXT3:
		case DXT5:
		case DXT1_ONE_BIT_ALPHA:
		case ATI2N:
		case ATI1N:
		case BC7:
		case BC6H:
			return -1;
	}
	return 0;
}

[[nodiscard]] constexpr int8_t decompressedBlue(ImageFormat format) {
	// This is merely for convenience, the true size may be different depending on the data
	switch (format) {
		using enum ImageFormat;
		case DXT1:
		case DXT1_ONE_BIT_ALPHA:
		case DXT3:
		case DXT5:
		case ATI2N:
		case ATI1N:
		case BC7:
			return 8;
		case BC6H:
			return 16;
		default:
			break;
	}
	return blue(format);
}

[[nodiscard]] constexpr int8_t alpha(ImageFormat format) {
	switch (format) {
		using enum ImageFormat;
		case RGBA32323232F:
			return 32;
		case RGBA16161616F:
		case RGBA16161616:
		case CONSOLE_RGBA16161616_LINEAR:
			return 16;
		case RGBA8888:
		case CONSOLE_RGBA8888_LINEAR:
		case ABGR8888:
		case CONSOLE_ABGR8888_LINEAR:
		case IA88:
		case ARGB8888:
		case CONSOLE_ARGB8888_LINEAR:
		case BGRA8888:
		case CONSOLE_BGRA8888_LINEAR:
		case CONSOLE_BGRA8888_LE:
		case BGRX8888:
		case CONSOLE_BGRX8888_LINEAR:
		case CONSOLE_BGRX8888_LE:
		case UVWQ8888:
		case UVLX8888:
		case RGBX8888:
			return 8;
		case BGRA4444:
			return 4;
		case RGBA1010102:
		case BGRA1010102:
			return 2;
		case BGRX5551:
		case CONSOLE_BGRX5551_LINEAR:
		case BGRA5551:
			return 1;
		case RGB888:
		case CONSOLE_RGB888_LINEAR:
		case BGR888:
		case CONSOLE_BGR888_LINEAR:
		case P8:
		case I8:
		case CONSOLE_I8_LINEAR:
		case RGB888_BLUESCREEN:
		case BGR888_BLUESCREEN:
		case UV88:
		case RGB565:
		case BGR565:
		case R32F:
		case RGB323232F:
		case A8:
		case EMPTY:
		case RG3232F:
		case RG1616F:
		case R16F:
		case R8:
			return 0;
		case DXT1:
		case DXT3:
		case DXT5:
		case DXT1_ONE_BIT_ALPHA:
		case ATI2N:
		case ATI1N:
		case BC7:
		case BC6H:
			return -1;
	}
	return 0;
}

[[nodiscard]] constexpr int8_t decompressedAlpha(ImageFormat format) {
	// This is merely for convenience, the true size may be different depending on the data
	switch (format) {
		using enum ImageFormat;
		case DXT5:
		case BC7:
			return 8;
		case DXT3:
			return 4;
		case DXT1_ONE_BIT_ALPHA:
			return 1;
		case DXT1:
		case ATI2N:
		case ATI1N:
		case BC6H:
			return 0;
		default:
			break;
	}
	return alpha(format);
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
		case CONSOLE_RGBA16161616_LINEAR:
		case RG3232F:
			return 64;
		case RGBA8888:
		case CONSOLE_RGBA8888_LINEAR:
		case ABGR8888:
		case CONSOLE_ABGR8888_LINEAR:
		case ARGB8888:
		case CONSOLE_ARGB8888_LINEAR:
		case BGRA8888:
		case CONSOLE_BGRA8888_LINEAR:
		case CONSOLE_BGRA8888_LE:
		case BGRX8888:
		case CONSOLE_BGRX8888_LINEAR:
		case CONSOLE_BGRX8888_LE:
		case UVLX8888:
		case R32F:
		case UVWQ8888:
		case RGBX8888:
		case RGBA1010102:
		case BGRA1010102:
		case RG1616F:
			return 32;
		case RGB888:
		case CONSOLE_RGB888_LINEAR:
		case BGR888:
		case CONSOLE_BGR888_LINEAR:
		case RGB888_BLUESCREEN:
		case BGR888_BLUESCREEN:
			return 24;
		case RGB565:
		case BGR565:
		case IA88:
		case BGRX5551:
		case CONSOLE_BGRX5551_LINEAR:
		case BGRA4444:
		case BGRA5551:
		case UV88:
		case R16F:
			return 16;
		case I8:
		case CONSOLE_I8_LINEAR:
		case P8:
		case A8:
		case DXT3:
		case DXT5:
		case BC7:
		case BC6H:
		case ATI2N:
		case R8:
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

[[nodiscard]] constexpr ImageFormat containerFormat(ImageFormat format) {
	switch (format) {
		using enum ImageFormat;
		case R32F:
		case RG3232F:
		case RGB323232F:
		case R16F:
		case RG1616F:
		case RGBA16161616F:
		case RGBA32323232F:
		case BC6H:
			return RGBA32323232F;
		case RGBA16161616:
		case CONSOLE_RGBA16161616_LINEAR:
		case RGBA1010102:
		case BGRA1010102:
			return RGBA16161616;
		case RGBA8888:
		case CONSOLE_RGBA8888_LINEAR:
		case ABGR8888:
		case CONSOLE_ABGR8888_LINEAR:
		case RGB888:
		case CONSOLE_RGB888_LINEAR:
		case BGR888:
		case CONSOLE_BGR888_LINEAR:
		case RGB888_BLUESCREEN:
		case BGR888_BLUESCREEN:
		case ARGB8888:
		case CONSOLE_ARGB8888_LINEAR:
		case BGRA8888:
		case CONSOLE_BGRA8888_LINEAR:
		case CONSOLE_BGRA8888_LE:
		case BGRX8888:
		case CONSOLE_BGRX8888_LINEAR:
		case CONSOLE_BGRX8888_LE:
		case UVWQ8888:
		case UVLX8888:
		case RGB565:
		case BGR565:
		case BGRX5551:
		case CONSOLE_BGRX5551_LINEAR:
		case BGRA5551:
		case BGRA4444:
		case I8:
		case CONSOLE_I8_LINEAR:
		case IA88:
		case P8:
		case UV88:
		case A8:
		case DXT1:
		case DXT3:
		case DXT5:
		case DXT1_ONE_BIT_ALPHA:
		case ATI2N:
		case ATI1N:
		case RGBX8888:
		case R8:
		case BC7:
			return RGBA8888;
		case EMPTY:
			break;
	}
	return ImageFormat::EMPTY;
}

[[nodiscard]] constexpr bool large(ImageFormat format) {
	return containerFormat(format) != ImageFormat::RGBA8888 && containerFormat(format) != ImageFormat::EMPTY;
}

[[nodiscard]] constexpr bool decimal(ImageFormat format) {
	return containerFormat(format) == ImageFormat::RGBA32323232F;
}

[[nodiscard]] constexpr bool compressed(ImageFormat format) {
	return red(format) == -1;
}

[[nodiscard]] constexpr bool transparent(ImageFormat format) {
	const auto a = alpha(format);
	if (a < 0) {
		switch (format) {
			using enum ImageFormat;
			case DXT3:
			case DXT5:
			case DXT1_ONE_BIT_ALPHA:
			case ATI2N:
			case ATI1N:
			case BC7:
			case BC6H:
				return true;
			default:
				break;
		}
		return false;
	}
	switch (format) {
		using enum ImageFormat;
		case RGB888_BLUESCREEN:
		case BGR888_BLUESCREEN:
			return true;
		case BGRX8888:
		case CONSOLE_BGRX8888_LINEAR:
		case CONSOLE_BGRX8888_LE:
		case BGRX5551:
		case CONSOLE_BGRX5551_LINEAR:
		case UVLX8888:
		case RGBX8888:
			return false;
		default:
			break;
	}
	return a != 0;
}

[[nodiscard]] constexpr bool opaque(ImageFormat format) {
	return !transparent(format);
}

[[nodiscard]] constexpr bool console(ImageFormat format) {
	switch (format) {
		using enum ImageFormat;
		case CONSOLE_BGRX8888_LINEAR:
		case CONSOLE_RGBA8888_LINEAR:
		case CONSOLE_ABGR8888_LINEAR:
		case CONSOLE_ARGB8888_LINEAR:
		case CONSOLE_BGRA8888_LINEAR:
		case CONSOLE_RGB888_LINEAR:
		case CONSOLE_BGR888_LINEAR:
		case CONSOLE_BGRX5551_LINEAR:
		case CONSOLE_I8_LINEAR:
		case CONSOLE_RGBA16161616_LINEAR:
		case CONSOLE_BGRX8888_LE:
		case CONSOLE_BGRA8888_LE:
			return true;
		default:
			break;
	}
	return false;
}

} // namespace ImageFormatDetails

namespace ImageDimensions {

[[nodiscard]] constexpr uint32_t getMipDim(uint8_t mip, uint16_t dim) {
	for (int i = 0; i < mip; i++) {
		dim /= 2;
	}
	return dim;
}

[[nodiscard]] constexpr uint8_t getRecommendedMipCountForDims(ImageFormat format, uint16_t width, uint16_t height) {
	// Note that compressed formats need 4x4 pixel blocks
	if (sourcepp::math::isPowerOf2(width) && sourcepp::math::isPowerOf2(height)) {
		const auto log2 = std::bit_width(width > height ? height : width);
		if (!ImageFormatDetails::compressed(format)) {
			return log2;
		}
		// Eliminate 2x2, 1x1
		return log2 - 2 > 1 ? log2 - 2 : 1;
	}

	uint8_t maxMipCount = 1;
	if (ImageFormatDetails::compressed(format)) {
		while (width > 0 && height > 0 && width % 4 == 0 && height % 4 == 0) {
			width /= 2;
			height /= 2;
			maxMipCount++;
		}
	} else {
		while (width > 0 && height > 0 && width % 2 == 0 && height % 2 == 0) {
			width /= 2;
			height /= 2;
			maxMipCount++;
		}
	}
	return maxMipCount;
}

[[nodiscard]] constexpr uint8_t getActualMipCountForDimsOnConsole(uint16_t width, uint16_t height) {
	if (width == 0 || height == 0) {
		return 0;
	}
	uint8_t numMipLevels = 1;
	while (true) {
		if (width == 1 && height == 1) {
			break;
		}
		width >>= 1;
		if (width < 1) {
			width = 1;
		}
		height >>= 1;
		if (height < 1) {
			height = 1;
		}
		numMipLevels += 1;
	}
	return numMipLevels;
}

} // namespace ImageDimensions

namespace ImageFormatDetails {

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
			return ((((width + 3) / 4) < 1) ? 1 : ((width + 3) / 4)) * ((((height + 3) / 4) < 1) ? 1 : ((height + 3) / 4)) * sliceCount * bpp(format) * 2;
		default:
			break;
	}
	return width * height * sliceCount * (bpp(format) / 8);
}

[[nodiscard]] constexpr uint32_t getDataLength(ImageFormat format, uint8_t mipCount, uint16_t frameCount, uint8_t faceCount, uint16_t width, uint16_t height, uint16_t sliceCount = 1) {
	uint32_t length = 0;
	for (int mip = mipCount - 1; mip >= 0; mip--) {
		length += ImageFormatDetails::getDataLength(format, ImageDimensions::getMipDim(mip, width), ImageDimensions::getMipDim(mip, height), sliceCount) * frameCount * faceCount;
	}
	return length;
}

// XTF (PLATFORM_XBOX) has padding between frames to align each one to 512 bytes
[[nodiscard]] constexpr uint32_t getDataLengthXBOX(bool padded, ImageFormat format, uint8_t mipCount, uint16_t frameCount, uint8_t faceCount, uint16_t width, uint16_t height, uint16_t sliceCount = 1) {
	uint32_t length = 0;
	for (int j = 0; j < frameCount; j++) {
		for (int i = 0; i < mipCount; i++) {
			length += ImageFormatDetails::getDataLength(format, ImageDimensions::getMipDim(i, width), ImageDimensions::getMipDim(i, height), sliceCount) * faceCount;
		}
		if (padded && j + 1 != frameCount && length > 512) {
			length += sourcepp::math::paddingForAlignment(512, length);
		}
	}
	return length;
}

[[nodiscard]] constexpr bool getDataPosition(uint32_t& offset, uint32_t& length, ImageFormat format, uint8_t mip, uint8_t mipCount, uint16_t frame, uint16_t frameCount, uint8_t face, uint8_t faceCount, uint16_t width, uint16_t height, uint16_t slice = 0, uint16_t sliceCount = 1) {
	offset = 0;
	length = 0;
	for (int i = mipCount - 1; i >= 0; i--) {
		for (int j = 0; j < frameCount; j++) {
			for (int k = 0; k < faceCount; k++) {
				for (int l = 0; l < sliceCount; l++) {
					const auto imageSize = ImageFormatDetails::getDataLength(format, ImageDimensions::getMipDim(i, width), ImageDimensions::getMipDim(i, height));
					if (i == mip && j == frame && k == face && l == slice) {
						length = imageSize;
						return true;
					}
					offset += imageSize;
				}
			}
		}
	}
	return false;
}

// XTF (PLATFORM_XBOX) is more like DDS layout
[[nodiscard]] constexpr bool getDataPositionXbox(uint32_t& offset, uint32_t& length, bool padded, ImageFormat format, uint8_t mip, uint8_t mipCount, uint16_t frame, uint16_t frameCount, uint8_t face, uint8_t faceCount, uint16_t width, uint16_t height, uint16_t slice = 0, uint16_t sliceCount = 1) {
	offset = 0;
	length = 0;
	for (int j = 0; j < frameCount; j++) {
		for (int k = 0; k < faceCount; k++) {
			for (int i = 0; i < mipCount; i++) {
				for (int l = 0; l < sliceCount; l++) {
					const auto imageSize = ImageFormatDetails::getDataLength(format, ImageDimensions::getMipDim(i, width), ImageDimensions::getMipDim(i, height));
					if (i == mip && j == frame && k == face && l == slice) {
						length = imageSize;
						return true;
					}
					offset += imageSize;
				}
			}
		}
		if (padded && j + 1 != frameCount && offset > 512) {
			offset += sourcepp::math::paddingForAlignment(512, offset);
		}
	}
	return false;
}

} // namespace ImageFormatDetails

} // namespace vtfpp
