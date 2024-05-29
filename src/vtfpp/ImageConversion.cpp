#include <vtfpp/ImageConversion.h>

#include <iterator>

#include <Compressonator.h>
#include <stb_image_write.h>

using namespace vtfpp;

namespace {

[[nodiscard]] constexpr CMP_FORMAT imageFormatToCompressonatorFormat(ImageFormat format) {
	switch (format) {
		using enum ImageFormat;
		case RGBA8888:
			return CMP_FORMAT_RGBA_8888;
		case ABGR8888:
			return CMP_FORMAT_ABGR_8888;
		case RGB888:
			return CMP_FORMAT_RGB_888;
		case BGR888:
			return CMP_FORMAT_BGR_888;
		case ARGB8888:
			return CMP_FORMAT_ARGB_8888;
		case BGRA8888:
			return CMP_FORMAT_BGRA_8888;
		case DXT1:
		case DXT1_ONE_BIT_ALPHA:
			return CMP_FORMAT_DXT1;
		case DXT3:
			return CMP_FORMAT_DXT3;
		case DXT5:
			return CMP_FORMAT_DXT5;
		case ATI2N:
			return CMP_FORMAT_ATI2N;
		case ATI1N:
			return CMP_FORMAT_ATI1N;
		case BC7:
			return CMP_FORMAT_BC7;
		case BC6H:
			return CMP_FORMAT_BC6H;
		default:
			break;
	}
	return CMP_FORMAT_Unknown;
}

[[nodiscard]] std::vector<std::byte> convertImageDataToRGBA8888(std::span<const std::byte> imageData, ImageFormat format) {
	std::vector<std::byte> newData;
	switch (format) {
		using enum ImageFormat;
		case RGBA8888:
		case UVWQ8888:
		case UVLX8888:
			newData = {imageData.begin(), imageData.end()};
			break;
		case ABGR8888:
			for (int i = 0; i < imageData.size(); i += 4) {
				newData.push_back(imageData[i + 3]);
				newData.push_back(imageData[i + 2]);
				newData.push_back(imageData[i + 1]);
				newData.push_back(imageData[i]);
			}
			break;
		case RGB888:
		case RGB888_BLUESCREEN:
			for (int i = 0; i < imageData.size(); i += 3) {
				newData.push_back(imageData[i]);
				newData.push_back(imageData[i + 1]);
				newData.push_back(imageData[i + 2]);
				newData.push_back(std::byte{0xff});
			}
			break;
		case BGR888:
		case BGR888_BLUESCREEN:
			for (int i = 0; i < imageData.size(); i += 3) {
				newData.push_back(imageData[i + 2]);
				newData.push_back(imageData[i + 1]);
				newData.push_back(imageData[i + 0]);
				newData.push_back(std::byte{0xff});
			}
			break;
		case RGB565:
			for (int i = 0; i < imageData.size(); i += 2) {
				auto pixel = *reinterpret_cast<const uint16_t*>(&imageData[i]);
				newData.push_back(static_cast<std::byte>(((((pixel >> 11) & 0x1f) * 527) + 23) >> 6));
				newData.push_back(static_cast<std::byte>(((((pixel >>  5) & 0x3f) * 259) + 33) >> 6));
				newData.push_back(static_cast<std::byte>(((( pixel        & 0x1f) * 527) + 23) >> 6));
				newData.push_back(std::byte{0xff});
			}
			break;
		case P8:
		case I8:
			for (int i = 0; i < imageData.size(); i += 1) {
				newData.push_back(imageData[i]);
				newData.push_back(imageData[i]);
				newData.push_back(imageData[i]);
				newData.push_back(std::byte{0xff});
			}
			break;
		case IA88:
			for (int i = 0; i < imageData.size(); i += 2) {
				newData.push_back(imageData[i]);
				newData.push_back(imageData[i]);
				newData.push_back(imageData[i]);
				newData.push_back(imageData[i + 1]);
			}
			break;
		case A8:
			for (int i = 0; i < imageData.size(); i += 1) {
				newData.push_back(std::byte{0});
				newData.push_back(std::byte{0});
				newData.push_back(std::byte{0});
				newData.push_back(imageData[i]);
			}
			break;
		case ARGB8888:
			for (int i = 0; i < imageData.size(); i += 4) {
				newData.push_back(imageData[i + 1]);
				newData.push_back(imageData[i + 2]);
				newData.push_back(imageData[i + 3]);
				newData.push_back(imageData[i]);
			}
			break;
		case BGRA8888:
		case BGRX8888:
			for (int i = 0; i < imageData.size(); i += 4) {
				newData.push_back(imageData[i + 2]);
				newData.push_back(imageData[i + 1]);
				newData.push_back(imageData[i]);
				newData.push_back(std::byte{0xff});
			}
			break;
		case BGR565:
			for (int i = 0; i < imageData.size(); i += 2) {
				auto pixel = *reinterpret_cast<const uint16_t*>(&imageData[i]);
				newData.push_back(static_cast<std::byte>(((( pixel        & 0x1f) * 527) + 23) >> 6));
				newData.push_back(static_cast<std::byte>(((((pixel >>  5) & 0x3f) * 259) + 33) >> 6));
				newData.push_back(static_cast<std::byte>(((((pixel >> 11) & 0x1f) * 527) + 23) >> 6));
				newData.push_back(std::byte{0xff});
			}
			break;
		case BGRA5551:
		case BGRX5551:
			for (int i = 0; i < imageData.size(); i += 2) {
				auto pixel = *reinterpret_cast<const uint16_t*>(&imageData[i]);
				newData.push_back(static_cast<std::byte>((((pixel >> 11) & 0x1f) * 255 + 15) / 31));
				newData.push_back(static_cast<std::byte>((((pixel >>  6) & 0x1f) * 255 + 15) / 31));
				newData.push_back(static_cast<std::byte>((((pixel >>  1) & 0x1f) * 255 + 15) / 31));
				newData.push_back(static_cast<std::byte>((  pixel        & 0x01) * 255));
			}
		case BGRA4444:
			for (int i = 0; i < imageData.size(); i += 2) {
				auto pixel = *reinterpret_cast<const uint16_t*>(&imageData[i]);
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
		case UV88:
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

[[nodiscard]] std::vector<std::byte> convertImageDataFromRGBA8888(std::span<const std::byte> imageData, ImageFormat format) {
	std::vector<std::byte> newData;
	switch (format) {
		using enum ImageFormat;
		case RGBA8888:
		case UVWQ8888:
		case UVLX8888:
			newData = {imageData.begin(), imageData.end()};
			break;
		case ABGR8888:
			for (int i = 0; i < imageData.size(); i += 4) {
				newData.push_back(imageData[i + 3]);
				newData.push_back(imageData[i + 2]);
				newData.push_back(imageData[i + 1]);
				newData.push_back(imageData[i]);
			}
			break;
		case RGB888:
		case RGB888_BLUESCREEN:
			for (int i = 0; i < imageData.size(); i += 4) {
				newData.push_back(imageData[i]);
				newData.push_back(imageData[i + 1]);
				newData.push_back(imageData[i + 2]);
			}
			break;
		case BGR888:
		case BGR888_BLUESCREEN:
			for (int i = 0; i < imageData.size(); i += 4) {
				newData.push_back(imageData[i + 2]);
				newData.push_back(imageData[i + 1]);
				newData.push_back(imageData[i + 0]);
			}
			break;
		case RGB565:
			for (int i = 0; i < imageData.size(); i += 4) {
				uint16_t rgb565 =
						(((static_cast<uint8_t>(imageData[i])     & 0xf8) << 8) +
						 ((static_cast<uint8_t>(imageData[i + 1]) & 0xfc) << 3) +
						  (static_cast<uint8_t>(imageData[i + 2])         >> 3));
				newData.push_back({});
				newData.push_back({});
				*reinterpret_cast<uint16_t*>(&newData[newData.size() - 2]) = rgb565;
			}
			break;
		case P8:
		case I8:
			for (int i = 0; i < imageData.size(); i += 4) {
				newData.push_back(imageData[i]);
			}
			break;
		case IA88:
			for (int i = 0; i < imageData.size(); i += 4) {
				newData.push_back(imageData[i]);
				newData.push_back(imageData[i + 3]);
			}
			break;
		case A8:
			for (int i = 0; i < imageData.size(); i += 4) {
				newData.push_back(imageData[i + 3]);
			}
			break;
		case ARGB8888:
			for (int i = 0; i < imageData.size(); i += 4) {
				newData.push_back(imageData[i + 3]);
				newData.push_back(imageData[i]);
				newData.push_back(imageData[i + 1]);
				newData.push_back(imageData[i + 2]);
			}
			break;
		case BGRA8888:
		case BGRX8888:
			for (int i = 0; i < imageData.size(); i += 4) {
				newData.push_back(imageData[i + 2]);
				newData.push_back(imageData[i + 1]);
				newData.push_back(imageData[i]);
				newData.push_back(imageData[i + 3]);
			}
			break;
		case BGR565:
			for (int i = 0; i < imageData.size(); i += 4) {
				uint16_t bgr565 =
						(((static_cast<uint8_t>(imageData[i + 2]) & 0xf8) << 8) +
						 ((static_cast<uint8_t>(imageData[i + 1]) & 0xfc) << 3) +
						  (static_cast<uint8_t>(imageData[i])             >> 3));
				newData.push_back({});
				newData.push_back({});
				*reinterpret_cast<uint16_t*>(&newData[newData.size() - 2]) = bgr565;
			}
			break;
		case BGRA5551:
		case BGRX5551:
			for (int i = 0; i < imageData.size(); i += 4) {
				uint32_t bgra5551 =
						(static_cast<uint16_t>(static_cast<uint8_t>(imageData[i])     * 31 / 255) << 11) |
						(static_cast<uint16_t>(static_cast<uint8_t>(imageData[i + 1]) * 31 / 255) <<  6) |
						(static_cast<uint16_t>(static_cast<uint8_t>(imageData[i + 2]) * 31 / 255) <<  1) |
						((static_cast<uint8_t>(imageData[i + 3]) > 0) ? 1 : 0);
				newData.push_back({});
				newData.push_back({});
				newData.push_back({});
				newData.push_back({});
				*reinterpret_cast<uint32_t*>(&newData[newData.size() - 4]) = bgra5551;
			}
		case BGRA4444:
			for (int i = 0; i < imageData.size(); i += 4) {
				auto r = static_cast<uint8_t>(imageData[i] >> 4);
				auto g = static_cast<uint8_t>(imageData[i + 1] >> 4);
				auto b = static_cast<uint8_t>(imageData[i + 2] >> 4);
				auto a = static_cast<uint8_t>(imageData[i + 3] >> 4);
				uint32_t bgra4444 = (b << 12) | (g << 8) | (r << 4) | a;
				newData.push_back({});
				newData.push_back({});
				newData.push_back({});
				newData.push_back({});
				*reinterpret_cast<uint32_t*>(&newData[newData.size() - 4]) = bgra4444;
			}
			break;
		case UV88:
			for (int i = 0; i < imageData.size(); i += 4) {
				newData.push_back(imageData[i]);
				newData.push_back(imageData[i + 1]);
			}
			break;
		default:
			break;
	}
	return newData;
}

[[nodiscard]] std::vector<std::byte> decodeImageDataToRGBA8888(std::span<const std::byte> imageData, ImageFormat format, uint16_t width, uint16_t height) {
	CMP_Texture srcTexture{};
	srcTexture.dwSize     = sizeof(srcTexture);
	srcTexture.dwWidth    = width;
	srcTexture.dwHeight   = height;
	srcTexture.dwPitch    = 0;
	srcTexture.format     = ::imageFormatToCompressonatorFormat(format);
	srcTexture.dwDataSize = imageData.size();

	srcTexture.pData = const_cast<CMP_BYTE*>(reinterpret_cast<const CMP_BYTE*>(imageData.data()));

	CMP_Texture destTexture{};
	destTexture.dwSize     = sizeof(destTexture);
	destTexture.dwWidth    = width;
	destTexture.dwHeight   = height;
	destTexture.dwPitch    = width * 4;
	destTexture.format     = ::imageFormatToCompressonatorFormat(ImageFormat::RGBA8888);
	destTexture.dwDataSize = CMP_CalculateBufferSize(&destTexture);

	std::vector<std::byte> destData;
	destData.resize(destTexture.dwDataSize);
	destTexture.pData = reinterpret_cast<CMP_BYTE*>(destData.data());

	CMP_CompressOptions options{};
	options.dwSize        = sizeof(options);
	options.bDXT1UseAlpha = false;
	options.dwnumThreads  = 0;

	if (CMP_ConvertTexture(&srcTexture, &destTexture, &options, nullptr) != CMP_OK) {
		return {};
	}
	return destData;
}

[[nodiscard]] std::vector<std::byte> encodeImageDataFromRGBA8888(std::span<const std::byte> imageData, ImageFormat format, uint16_t width, uint16_t height) {
	CMP_Texture srcTexture{};
	srcTexture.dwSize     = sizeof(srcTexture);
	srcTexture.dwWidth    = width;
	srcTexture.dwHeight   = height;
	srcTexture.dwPitch    = width * 4;
	srcTexture.format     = ::imageFormatToCompressonatorFormat(ImageFormat::RGBA8888);
	srcTexture.dwDataSize = imageData.size();

	srcTexture.pData = const_cast<CMP_BYTE*>(reinterpret_cast<const CMP_BYTE*>(imageData.data()));

	CMP_Texture destTexture{};
	destTexture.dwSize     = sizeof(destTexture);
	destTexture.dwWidth    = width;
	destTexture.dwHeight   = height;
	destTexture.dwPitch    = 0;
	destTexture.format     = ::imageFormatToCompressonatorFormat(format);
	destTexture.dwDataSize = CMP_CalculateBufferSize(&destTexture);

	std::vector<std::byte> destData;
	destData.resize(destTexture.dwDataSize);
	destTexture.pData = reinterpret_cast<CMP_BYTE*>(destData.data());

	CMP_CompressOptions options{};
	options.dwSize        = sizeof(options);
	options.bDXT1UseAlpha = false;
	options.dwnumThreads  = 0;

	if (CMP_ConvertTexture(&srcTexture, &destTexture, &options, nullptr) != CMP_OK) {
		return {};
	}
	return destData;
}

[[nodiscard]] std::vector<std::byte> convertImageDataToRGBA32323232F(std::span<const std::byte> imageData, ImageFormat format) {
	// todo: convert from a large format to rgba32323232f
	return {imageData.begin(), imageData.end()};
}

[[nodiscard]] std::vector<std::byte> convertImageDataFromRGBA32323232F(std::span<const std::byte> imageData, ImageFormat format) {
	// todo: convert from rgba32323232f to other large formats
	return {imageData.begin(), imageData.end()};
}

[[nodiscard]] std::vector<std::byte> convertImageDataFromRGBA8888ToRGBA32323232F(std::span<const std::byte> imageData) {
	// todo: convert from rgba8888 to rgba32323232f
	return {imageData.begin(), imageData.end()};
}

[[nodiscard]] std::vector<std::byte> convertImageDataFromRGBA32323232FToRGBA8888(std::span<const std::byte> imageData) {
	// todo: convert from rgba32323232f to rgba8888
	return {imageData.begin(), imageData.end()};
}

} // namespace

std::vector<std::byte> ImageConversion::convertImageDataToFormat(std::span<const std::byte> imageData, ImageFormat oldFormat, ImageFormat newFormat, uint16_t width, uint16_t height) {
	if (oldFormat == newFormat) {
		return {imageData.begin(), imageData.end()};
	}

	ImageFormat intermediaryFormat;
	std::vector<std::byte> intermediaryData;
	if (ImageFormatDetails::compressed(oldFormat)) {
		intermediaryFormat = ImageFormat::RGBA8888;
		intermediaryData = ::decodeImageDataToRGBA8888(imageData, oldFormat, width, height);
	} else if (ImageFormatDetails::large(oldFormat)) {
		intermediaryFormat = ImageFormat::RGBA32323232F;
		intermediaryData = ::convertImageDataToRGBA32323232F(imageData, oldFormat);
		if (!ImageFormatDetails::large(newFormat)) {
			intermediaryFormat = ImageFormat::RGBA8888;
			intermediaryData = ::convertImageDataFromRGBA32323232FToRGBA8888(imageData);
		}
	} else {
		intermediaryFormat = ImageFormat::RGBA8888;
		intermediaryData = ::convertImageDataToRGBA8888(imageData, oldFormat);
		if (ImageFormatDetails::large(newFormat)) {
			intermediaryFormat = ImageFormat::RGBA32323232F;
			intermediaryData = ::convertImageDataFromRGBA8888ToRGBA32323232F(imageData);
		}
	}

	if (intermediaryFormat == newFormat) {
		return intermediaryData;
	}
	if (intermediaryFormat == ImageFormat::RGBA8888) {
		if (ImageFormatDetails::compressed(newFormat)) {
			return ::encodeImageDataFromRGBA8888(intermediaryData, newFormat, width, height);
		}
		return ::convertImageDataFromRGBA8888(intermediaryData, newFormat);
	}
	return ::convertImageDataFromRGBA32323232F(intermediaryData, newFormat);
}

std::vector<std::byte> ImageConversion::convertImageDataToFile(std::span<const std::byte> imageData, ImageFormat format, uint16_t width, uint16_t height) {
	std::vector<std::byte> out;
	auto stbWriteFunc = [](void* out, void* data, int size) {
		std::copy(reinterpret_cast<std::byte*>(data), reinterpret_cast<std::byte*>(data) + size, std::back_inserter(*reinterpret_cast<std::vector<std::byte>*>(out)));
	};
	if (ImageFormatDetails::large(format)) {
		auto hdr = convertImageDataToFormat(imageData, format, ImageFormat::RGBA32323232F, width, height);
		// todo: is 4 correct?
		stbi_write_hdr_to_func(stbWriteFunc, &out, width, height, 4, reinterpret_cast<float*>(hdr.data()));
	} else {
		// This works for compressed formats too
		auto rgba = convertImageDataToFormat(imageData, format, ImageFormat::RGBA8888, width, height);
		stbi_write_png_to_func(stbWriteFunc, &out, width, height, 4, rgba.data(), 0);
	}
	return out;
}
