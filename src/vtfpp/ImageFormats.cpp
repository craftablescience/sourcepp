#include <vtfpp/ImageFormats.h>

#include <cmath>
#include <utility>

using namespace vtfpp;

namespace {

uint32_t getMipDim(uint8_t mip, uint16_t dim) {
	return static_cast<uint32_t>(std::ceil(static_cast<float>(dim) / static_cast<float>(1 << mip)));
}

} // namespace

uint32_t ImageFormatDetails::getDataLength(ImageFormat format, uint16_t width, uint16_t height, uint16_t sliceCount) {
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

uint32_t ImageFormatDetails::getDataLength(ImageFormat format, uint8_t mipCount, uint16_t frameCount, uint16_t faceCount, uint16_t width, uint16_t height, uint16_t sliceCount) {
	uint32_t length = 0;
	for (int mip = mipCount - 1; mip >= 0; mip--) {
		length += ImageFormatDetails::getDataLength(format, ::getMipDim(mip, width), ::getMipDim(mip, height), sliceCount) * frameCount * faceCount;
	}
	return length;
}

bool ImageFormatDetails::getDataPosition(uint32_t& offset, uint32_t& length, vtfpp::ImageFormat format, uint8_t mip, uint8_t mipCount, uint16_t frame, uint16_t frameCount, uint16_t face, uint16_t faceCount, uint16_t width, uint16_t height, uint16_t slice, uint16_t sliceCount) {
	offset = 0;
	length = 0;
	for (int i = mipCount - 1; i >= 0; i--) {
		for (int j = 0; j < frameCount; j++) {
			for (int k = 0; k < faceCount; k++) {
				for (int l = 0; l < sliceCount; l++) {
					auto imageSize = ImageFormatDetails::getDataLength(format, ::getMipDim(i, width), ::getMipDim(i, height));
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
