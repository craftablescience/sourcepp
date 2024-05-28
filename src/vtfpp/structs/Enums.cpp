#include <vtfpp/structs/Enums.h>

using namespace vtfpp;

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

uint32_t ImageFormatDetails::dataLength(ImageFormat format, uint32_t width, uint32_t height, uint32_t depth) {
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
			if (width < 4 && width > 0) {
				width = 4;
			}
			if (height < 4 && height > 0) {
				height = 4;
			}
			return (bpp(format) * 2) * ((width + 3) / 4) * ((height + 3) / 4) * depth;
		default:
			break;
	}
	return bpp(format) * width * height * depth;
}
