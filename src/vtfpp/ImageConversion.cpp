#include <vtfpp/ImageConversion.h>

#include <iterator>

#include <stb_image_write.h>

using namespace vtfpp;

namespace {

std::vector<std::byte> convertImageDataToRGBA8888(std::span<const std::byte> imageData, ImageFormat format) {
	std::vector<std::byte> newData;
	switch (format) {
		case ImageFormat::RGBA8888:
		case ImageFormat::UVWQ8888:
		case ImageFormat::UVLX8888:
			newData = {imageData.begin(), imageData.end()};
			break;
		case ImageFormat::ABGR8888:
			for (int i = 0; i < imageData.size(); i += 4) {
				newData.push_back(imageData[i + 3]);
				newData.push_back(imageData[i + 2]);
				newData.push_back(imageData[i + 1]);
				newData.push_back(imageData[i]);
			}
			break;
		case ImageFormat::RGB888:
		case ImageFormat::RGB888_BLUESCREEN:
			for (int i = 0; i < imageData.size(); i += 3) {
				newData.push_back(imageData[i]);
				newData.push_back(imageData[i + 1]);
				newData.push_back(imageData[i + 2]);
				newData.push_back(std::byte{0xff});
			}
			break;
		case ImageFormat::BGR888:
		case ImageFormat::BGR888_BLUESCREEN:
			for (int i = 0; i < imageData.size(); i += 3) {
				newData.push_back(imageData[i + 2]);
				newData.push_back(imageData[i + 1]);
				newData.push_back(imageData[i + 0]);
				newData.push_back(std::byte{0xff});
			}
			break;
		case ImageFormat::RGB565:
			for (int i = 0; i < imageData.size(); i += 2) {
				uint16_t pixel = *reinterpret_cast<const uint16_t*>(&imageData[i]);
				newData.push_back(static_cast<std::byte>(((((pixel >> 11) & 0x1f) * 527) + 23) >> 6));
				newData.push_back(static_cast<std::byte>(((((pixel >>  5) & 0x3f) * 259) + 33) >> 6));
				newData.push_back(static_cast<std::byte>(((( pixel        & 0x1f) * 527) + 23) >> 6));
				newData.push_back(std::byte{0xff});
			}
			break;
		case ImageFormat::P8:
		case ImageFormat::I8:
			for (int i = 0; i < imageData.size(); i += 1) {
				newData.push_back(imageData[i]);
				newData.push_back(imageData[i]);
				newData.push_back(imageData[i]);
				newData.push_back(std::byte{0xff});
			}
			break;
		case ImageFormat::IA88:
			for (int i = 0; i < imageData.size(); i += 2) {
				newData.push_back(imageData[i]);
				newData.push_back(imageData[i]);
				newData.push_back(imageData[i]);
				newData.push_back(imageData[i + 1]);
			}
			break;
		case ImageFormat::A8:
			for (int i = 0; i < imageData.size(); i += 1) {
				newData.push_back(std::byte{0});
				newData.push_back(std::byte{0});
				newData.push_back(std::byte{0});
				newData.push_back(imageData[i]);
			}
			break;
		case ImageFormat::ARGB8888:
			for (int i = 0; i < imageData.size(); i += 4) {
				newData.push_back(imageData[i + 3]);
				newData.push_back(imageData[i]);
				newData.push_back(imageData[i + 1]);
				newData.push_back(imageData[i + 2]);
			}
			break;
		case ImageFormat::BGRA8888:
		case ImageFormat::BGRX8888:
			for (int i = 0; i < imageData.size(); i += 4) {
				newData.push_back(imageData[i + 2]);
				newData.push_back(imageData[i + 1]);
				newData.push_back(imageData[i]);
				newData.push_back(imageData[i + 3]);
			}
			break;
		case ImageFormat::BGR565:
			for (int i = 0; i < imageData.size(); i += 2) {
				uint16_t pixel = *reinterpret_cast<const uint16_t*>(&imageData[i]);
				newData.push_back(static_cast<std::byte>(((( pixel        & 0x1f) * 527) + 23) >> 6));
				newData.push_back(static_cast<std::byte>(((((pixel >>  5) & 0x3f) * 259) + 33) >> 6));
				newData.push_back(static_cast<std::byte>(((((pixel >> 11) & 0x1f) * 527) + 23) >> 6));
				newData.push_back(std::byte{0xff});
			}
			break;
		case ImageFormat::BGRA5551:
		case ImageFormat::BGRX5551:
			for (int i = 0; i < imageData.size(); i += 2) {
				uint16_t pixel = *reinterpret_cast<const uint16_t*>(&imageData[i]);
				newData.push_back(static_cast<std::byte>((((pixel >> 11) & 0x1f) * 255 + 15) / 31));
				newData.push_back(static_cast<std::byte>((((pixel >>  6) & 0x1f) * 255 + 15) / 31));
				newData.push_back(static_cast<std::byte>((((pixel >>  1) & 0x1f) * 255 + 15) / 31));
				newData.push_back(static_cast<std::byte>((  pixel        & 0x01) * 255));
			}
		case ImageFormat::BGRA4444:
			for (int i = 0; i < imageData.size(); i += 2) {
				uint16_t pixel = *reinterpret_cast<const uint16_t*>(&imageData[i]);
				uint8_t b = (pixel & 0xf000) >> 12;
				uint8_t g = (pixel & 0x0f00) >> 8;
				uint8_t r = (pixel & 0x00f0) >> 4;
				uint8_t a = (pixel & 0x000f);
				newData.push_back(static_cast<std::byte>((r << 4) | r));
				newData.push_back(static_cast<std::byte>((g << 4) | g));
				newData.push_back(static_cast<std::byte>((b << 4) | b));
				newData.push_back(static_cast<std::byte>((a << 4) | a));
			}
			break;
		case ImageFormat::UV88:
			for (int i = 0; i < imageData.size(); i += 2) {
				newData.push_back(imageData[i]);
				newData.push_back(imageData[i + 1]);
				newData.push_back(std::byte{0});
				newData.push_back(std::byte{0xff});
			}
			break;
		default:
			break;
	}
	return newData;
}

std::vector<std::byte> convertImageDataFromRGBA8888(std::span<const std::byte> imageData, ImageFormat format) {
	// todo: convert from rgba8888 to other int formats
	return {imageData.begin(), imageData.end()};
}

std::vector<std::byte> convertImageDataToRGBA32323232F(std::span<const std::byte> imageData, ImageFormat format) {
	// todo: convert from a floating point format to rgba32323232f
	return {imageData.begin(), imageData.end()};
}

std::vector<std::byte> convertImageDataFromRGBA32323232F(std::span<const std::byte> imageData, ImageFormat format) {
	// todo: convert from rgba32323232f to other formats
	return {imageData.begin(), imageData.end()};
}

} // namespace

std::vector<std::byte> ImageConversion::convertImageDataToFormat(std::span<const std::byte> imageData, ImageFormat oldFormat, ImageFormat newFormat, uint16_t, uint16_t) {
	if (oldFormat == newFormat) {
		return {imageData.begin(), imageData.end()};
	}

	ImageFormat intermediaryFormat;
	std::vector<std::byte> intermediaryData;
	if (ImageFormatDetails::compressed(oldFormat)) {
		intermediaryFormat = ImageFormat::RGBA8888;
		// todo: decompress
	} else {
		if (ImageFormatDetails::red(oldFormat) > 8 || ImageFormatDetails::bpp(oldFormat) > 32) {
			intermediaryFormat = ImageFormat::RGBA32323232F;
			intermediaryData = ::convertImageDataToRGBA32323232F(imageData, oldFormat);
		} else {
			intermediaryFormat = ImageFormat::RGBA8888;
			intermediaryData = ::convertImageDataToRGBA8888(imageData, oldFormat);
		}
	}

	if (intermediaryFormat == newFormat) {
		return intermediaryData;
	}
	if (intermediaryFormat == ImageFormat::RGBA8888) {
		return ::convertImageDataFromRGBA8888(intermediaryData, newFormat);
	}
	return ::convertImageDataFromRGBA32323232F(intermediaryData, newFormat);
}

std::vector<std::byte> ImageConversion::convertImageDataToFile(std::span<const std::byte> imageData, ImageFormat format, uint16_t width, uint16_t height) {
	std::vector<std::byte> out;
	auto stbWriteFunc = [](void* out, void* data, int size) {
		std::copy(reinterpret_cast<std::byte*>(data), reinterpret_cast<std::byte*>(data) + size, std::back_inserter(*reinterpret_cast<std::vector<std::byte>*>(out)));
	};
	if (ImageFormatDetails::bpp(format) <= 32) {
		auto rgba = convertImageDataToFormat(imageData, format, ImageFormat::RGBA8888, width, height);
#if 0
		stbi_write_png("test.png", width, height, 4, rgba.data(), 0);
#else
		stbi_write_png_to_func(stbWriteFunc, &out, width, height, 4, rgba.data(), 0);
#endif
	} else {
		auto hdr = convertImageDataToFormat(imageData, format, ImageFormat::RGBA32323232F, width, height);
		stbi_write_hdr_to_func(stbWriteFunc, &out, width, height, 4, reinterpret_cast<float*>(hdr.data()));
	}
	return out;
}
