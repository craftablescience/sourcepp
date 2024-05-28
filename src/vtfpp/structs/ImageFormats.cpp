#include <vtfpp/structs/ImageFormats.h>

#include <algorithm>

#include <vtfpp/detail/Utility.h>

using namespace vtfpp;
using namespace vtfpp::detail;

uint8_t ImageFormatDetails::bpp(ImageFormat format) {
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

uint32_t ImageFormatDetails::dataLength(ImageFormat format, uint16_t width, uint16_t height, uint16_t sliceCount) {
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
	return width * height * sliceCount * bpp(format) / 8;
}

uint32_t ImageFormatDetails::dataLength(ImageFormat format, uint8_t mipCount, uint16_t frameCount, uint16_t faceCount, uint16_t width, uint16_t height, uint16_t sliceCount) {
	uint32_t length = 0;
	for (int mip = mipCount - 1; mip >= 0; mip--) {
		length += ImageFormatDetails::dataLength(format, ::getMipDim(mip, width), ::getMipDim(mip, height), sliceCount) * frameCount * faceCount;
	}
	return length;
}
