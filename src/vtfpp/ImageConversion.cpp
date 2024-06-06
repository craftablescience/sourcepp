#include <vtfpp/ImageConversion.h>

#include <algorithm>
#include <iterator>

#include <Compressonator.h>
#include <sourcepp/math/Float.h>
#include <stb_image_write.h>

using namespace sourcepp;
using namespace vtfpp;

namespace {

[[nodiscard]] constexpr CMP_FORMAT imageFormatToCompressonatorFormat(ImageFormat format) {
	switch (format) {
		using enum ImageFormat;
		case RGBA8888:
		case UVWQ8888:
			return CMP_FORMAT_RGBA_8888;
		case ABGR8888:
			return CMP_FORMAT_ABGR_8888;
		case RGB888:
			return CMP_FORMAT_RGB_888;
		case BGR888:
			return CMP_FORMAT_BGR_888;
		case I8:
		case P8:
			return CMP_FORMAT_R_8;
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
		case UV88:
			return CMP_FORMAT_RG_8;
		case RGBA16161616F:
			return CMP_FORMAT_RGBA_16F;
		case RGBA16161616:
			return CMP_FORMAT_RGBA_16;
		case R32F:
			return CMP_FORMAT_R_32F;
		case RGB323232F:
			return CMP_FORMAT_RGB_32F;
		case RGBA32323232F:
			return CMP_FORMAT_RGBA_32F;
		case ATI2N:
			return CMP_FORMAT_ATI2N;
		case ATI1N:
			return CMP_FORMAT_ATI1N;
		case BC7:
			return CMP_FORMAT_BC7;
		case BC6H:
			return CMP_FORMAT_BC6H;
		case RGB565:
		case IA88:
		case A8:
		case RGB888_BLUESCREEN:
		case BGR888_BLUESCREEN:
		case BGRX8888:
		case BGR565:
		case BGRX5551:
		case BGRA4444:
		case BGRA5551:
		case UVLX8888:
		case EMPTY:
			return CMP_FORMAT_Unknown;
	}
	return CMP_FORMAT_Unknown;
}

[[nodiscard]] std::vector<std::byte> convertImageDataUsingCompressonator(std::span<const std::byte> imageData, ImageFormat oldFormat, ImageFormat newFormat, uint16_t width, uint16_t height) {
	CMP_Texture srcTexture{};
	srcTexture.dwSize     = sizeof(srcTexture);
	srcTexture.dwWidth    = width;
	srcTexture.dwHeight   = height;
	srcTexture.dwPitch    = ImageFormatDetails::compressed(newFormat) ? 0 : width * (ImageFormatDetails::bpp(newFormat) / 8);
	srcTexture.format     = ::imageFormatToCompressonatorFormat(oldFormat);
	srcTexture.dwDataSize = imageData.size();

	srcTexture.pData = const_cast<CMP_BYTE*>(reinterpret_cast<const CMP_BYTE*>(imageData.data()));

	CMP_Texture destTexture{};
	destTexture.dwSize     = sizeof(destTexture);
	destTexture.dwWidth    = width;
	destTexture.dwHeight   = height;
	destTexture.dwPitch    = ImageFormatDetails::compressed(newFormat) ? 0 : width * (ImageFormatDetails::bpp(newFormat) / 8);
	destTexture.format     = ::imageFormatToCompressonatorFormat(newFormat);
	destTexture.dwDataSize = CMP_CalculateBufferSize(&destTexture);

	std::vector<std::byte> destData;
	destData.resize(destTexture.dwDataSize);
	destTexture.pData = reinterpret_cast<CMP_BYTE*>(destData.data());

	CMP_CompressOptions options{};
	options.dwSize        = sizeof(options);
	options.bDXT1UseAlpha = oldFormat == ImageFormat::DXT1_ONE_BIT_ALPHA || newFormat == ImageFormat::DXT1_ONE_BIT_ALPHA;
	options.dwnumThreads  = 0;

	if (CMP_ConvertTexture(&srcTexture, &destTexture, &options, nullptr) != CMP_OK) {
		return {};
	}
	return destData;
}

[[nodiscard]] std::vector<std::byte> convertImageDataToRGBA8888(std::span<const std::byte> imageData, ImageFormat format) {
	std::vector<std::byte> newData;
	newData.reserve(imageData.size() / (ImageFormatDetails::bpp(format) / 8) * (ImageFormatDetails::bpp(ImageFormat::RGBA8888) / 8));
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
			for (int i = 0; i < imageData.size(); i += 3) {
				newData.push_back(imageData[i]);
				newData.push_back(imageData[i + 1]);
				newData.push_back(imageData[i + 2]);
				newData.push_back(std::byte{0xff});
			}
			break;
		case RGB888_BLUESCREEN:
			for (int i = 0; i < imageData.size(); i += 3) {
				newData.push_back(imageData[i]);
				newData.push_back(imageData[i + 1]);
				newData.push_back(imageData[i + 2]);
				if (static_cast<uint8_t>(imageData[i]) == 0 && static_cast<uint8_t>(imageData[i + 1]) == 0 && static_cast<uint8_t>(imageData[i + 2]) == 0xff) {
					newData.push_back(std::byte{0});
				} else {
					newData.push_back(std::byte{0xff});
				}
			}
			break;
		case BGR888:
			for (int i = 0; i < imageData.size(); i += 3) {
				newData.push_back(imageData[i + 2]);
				newData.push_back(imageData[i + 1]);
				newData.push_back(imageData[i]);
				newData.push_back(std::byte{0xff});
			}
			break;
		case BGR888_BLUESCREEN:
			for (int i = 0; i < imageData.size(); i += 3) {
				newData.push_back(imageData[i + 2]);
				newData.push_back(imageData[i + 1]);
				newData.push_back(imageData[i]);
				if (static_cast<uint8_t>(imageData[i + 2]) == 0 && static_cast<uint8_t>(imageData[i + 1]) == 0 && static_cast<uint8_t>(imageData[i]) == 0xff) {
					newData.push_back(std::byte{0});
				} else {
					newData.push_back(std::byte{0xff});
				}
			}
			break;
		case RGB565:
			for (int i = 0; i < imageData.size(); i += 2) {
				auto pixel = *reinterpret_cast<const uint16_t*>(&imageData[i]);
				newData.push_back(static_cast<std::byte>(((( pixel        & 0x1f) * 527) + 23) >> 6));
				newData.push_back(static_cast<std::byte>(((((pixel >>  5) & 0x3f) * 259) + 33) >> 6));
				newData.push_back(static_cast<std::byte>(((((pixel >> 11) & 0x1f) * 527) + 23) >> 6));
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
				newData.push_back(imageData[i + 3]);
				newData.push_back(imageData[i]);
				newData.push_back(imageData[i + 1]);
				newData.push_back(imageData[i + 2]);
			}
			break;
		case BGRA8888:
			for (int i = 0; i < imageData.size(); i += 4) {
				newData.push_back(imageData[i + 2]);
				newData.push_back(imageData[i + 1]);
				newData.push_back(imageData[i]);
				newData.push_back(imageData[i + 3]);
			}
			break;
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
				newData.push_back(static_cast<std::byte>(((((pixel >> 11) & 0x1f) * 527) + 23) >> 6));
				newData.push_back(static_cast<std::byte>(((((pixel >>  5) & 0x3f) * 259) + 33) >> 6));
				newData.push_back(static_cast<std::byte>(((( pixel        & 0x1f) * 527) + 23) >> 6));
				newData.push_back(std::byte{0xff});
			}
			break;
		case BGRA5551:
			for (int i = 0; i < imageData.size(); i += 2) {
				auto pixel = *reinterpret_cast<const uint16_t*>(&imageData[i]);
				newData.push_back(static_cast<std::byte>((((pixel & 0x7c00) >> 10) * 255 + 15) / 31));
				newData.push_back(static_cast<std::byte>((((pixel & 0x03e0) >>  5) * 255 + 15) / 31));
				newData.push_back(static_cast<std::byte>((( pixel & 0x001f       ) * 255 + 15) / 31));
				newData.push_back(static_cast<std::byte>((((pixel & 0x8000) >> 15) * 255)));
			}
			break;
		case BGRX5551:
			for (int i = 0; i < imageData.size(); i += 2) {
				auto pixel = *reinterpret_cast<const uint16_t*>(&imageData[i]);
				newData.push_back(static_cast<std::byte>((((pixel & 0x7c00) >> 10) * 255 + 15) / 31));
				newData.push_back(static_cast<std::byte>((((pixel & 0x03e0) >>  5) * 255 + 15) / 31));
				newData.push_back(static_cast<std::byte>((( pixel & 0x001f       ) * 255 + 15) / 31));
				newData.push_back(std::byte{255});
			}
			break;
		case BGRA4444:
			for (int i = 0; i < imageData.size(); i += 2) {
				newData.push_back(static_cast<std::byte>(((static_cast<uint16_t>(imageData[i + 1]) & 0x0f) * 255 + 7) / 15));
				newData.push_back(static_cast<std::byte>(((static_cast<uint16_t>(imageData[i])       >> 4) * 255 + 7) / 15));
				newData.push_back(static_cast<std::byte>(((static_cast<uint16_t>(imageData[i])     & 0x0f) * 255 + 7) / 15));
				newData.push_back(static_cast<std::byte>(((static_cast<uint16_t>(imageData[i + 1])   >> 4) * 255 + 7) / 15));
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
	newData.reserve(imageData.size() / (ImageFormatDetails::bpp(ImageFormat::RGBA8888) / 8) * (ImageFormatDetails::bpp(format) / 8));
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
			for (int i = 0; i < imageData.size(); i += 4) {
				newData.push_back(imageData[i]);
				newData.push_back(imageData[i + 1]);
				newData.push_back(imageData[i + 2]);
			}
			break;
		case RGB888_BLUESCREEN:
			for (int i = 0; i < imageData.size(); i += 4) {
				if (static_cast<uint8_t>(imageData[i + 3]) == 0) {
					newData.push_back(std::byte{0});
					newData.push_back(std::byte{0});
					newData.push_back(std::byte{0xff});
				} else {
					newData.push_back(imageData[i]);
					newData.push_back(imageData[i + 1]);
					newData.push_back(imageData[i + 2]);
				}
			}
			break;
		case BGR888:
			for (int i = 0; i < imageData.size(); i += 4) {
				newData.push_back(imageData[i + 2]);
				newData.push_back(imageData[i + 1]);
				newData.push_back(imageData[i]);
			}
			break;
		case BGR888_BLUESCREEN:
			for (int i = 0; i < imageData.size(); i += 4) {
				if (static_cast<uint8_t>(imageData[i + 3]) == 0) {
					newData.push_back(std::byte{0xff});
					newData.push_back(std::byte{0});
					newData.push_back(std::byte{0});
				} else {
					newData.push_back(imageData[i + 2]);
					newData.push_back(imageData[i + 1]);
					newData.push_back(imageData[i]);
				}
			}
			break;
		case RGB565:
			for (int i = 0; i < imageData.size(); i += 4) {
				uint16_t rgb565 = ((static_cast<uint8_t>(imageData[i + 2]) >> 3) << 11) | ((static_cast<uint8_t>(imageData[i + 1]) >> 2) << 5) | (static_cast<uint8_t>(imageData[i]) >> 3);
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
				newData.push_back(imageData[i + 1]);
				newData.push_back(imageData[i + 2]);
				newData.push_back(imageData[i + 3]);
				newData.push_back(imageData[i]);
			}
			break;
		case BGRA8888:
			for (int i = 0; i < imageData.size(); i += 4) {
				newData.push_back(imageData[i + 2]);
				newData.push_back(imageData[i + 1]);
				newData.push_back(imageData[i]);
				newData.push_back(imageData[i + 3]);
			}
			break;
		case BGRX8888:
			for (int i = 0; i < imageData.size(); i += 4) {
				newData.push_back(imageData[i + 2]);
				newData.push_back(imageData[i + 1]);
				newData.push_back(imageData[i]);
				newData.push_back(std::byte{0});
			}
			break;
		case BGR565:
			for (int i = 0; i < imageData.size(); i += 4) {
				uint16_t bgr565 = ((static_cast<uint8_t>(imageData[i]) >> 3) << 11) | ((static_cast<uint8_t>(imageData[i + 1]) >> 2) << 5) | (static_cast<uint8_t>(imageData[i + 2]) >> 3);
				newData.push_back({});
				newData.push_back({});
				*reinterpret_cast<uint16_t*>(&newData[newData.size() - 2]) = bgr565;
			}
			break;
		case BGRA5551:
			for (int i = 0; i < imageData.size(); i += 4) {
				uint16_t bgra5551 = ((static_cast<uint8_t>(imageData[i + 3]) > 0 ? uint16_t{1} : uint16_t{0}) << 15) | ((static_cast<uint16_t>(imageData[i]) >> 3) << 10) | ((static_cast<uint16_t>(imageData[i + 1]) >> 3) << 5) | ((static_cast<uint16_t>(imageData[i + 2]) >> 3));
				newData.push_back({});
				newData.push_back({});
				*reinterpret_cast<uint16_t*>(&newData[newData.size() - 2]) = bgra5551;
			}
			break;
		case BGRX5551:
			for (int i = 0; i < imageData.size(); i += 4) {
				uint16_t bgra5551 = ((static_cast<uint16_t>(imageData[i]) >> 3) << 10) | ((static_cast<uint16_t>(imageData[i + 1]) >> 3) << 5) | ((static_cast<uint16_t>(imageData[i + 2]) >> 3));
				newData.push_back({});
				newData.push_back({});
				*reinterpret_cast<uint16_t*>(&newData[newData.size() - 2]) = bgra5551;
			}
			break;
		case BGRA4444:
			for (int i = 0; i < imageData.size(); i += 4) {
				auto r = static_cast<uint8_t>(imageData[i] >> 4);
				auto g = static_cast<uint8_t>(imageData[i + 1] >> 4);
				auto b = static_cast<uint8_t>(imageData[i + 2] >> 4);
				auto a = static_cast<uint8_t>(imageData[i + 3] >> 4);
				uint16_t bgra4444 = (a << 12) | (r << 8) | (g << 4) | b;
				newData.push_back({});
				newData.push_back({});
				*reinterpret_cast<uint16_t*>(&newData[newData.size() - 2]) = bgra4444;
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
	return ::convertImageDataUsingCompressonator(imageData, format, ImageFormat::RGBA8888, width, height);
}

[[nodiscard]] std::vector<std::byte> encodeImageDataFromRGBA8888(std::span<const std::byte> imageData, ImageFormat format, uint16_t width, uint16_t height) {
	return ::convertImageDataUsingCompressonator(imageData, ImageFormat::RGBA8888, format, width, height);
}

[[nodiscard]] std::vector<std::byte> convertImageDataToRGBA32323232F(std::span<const std::byte> imageData, ImageFormat format) {
	std::vector<std::byte> newData;
	newData.reserve(imageData.size() / (ImageFormatDetails::bpp(format) / 8) * (ImageFormatDetails::bpp(ImageFormat::RGBA32323232F) / 8));
	switch (format) {
		using enum ImageFormat;
		case RGBA32323232F:
			newData = {imageData.begin(), imageData.end()};
			break;
		case RGB323232F:
			for (int i = 0; i < imageData.size(); i += 12) {
				for (int j = 0; j < 16; j++) {
					newData.push_back({});
				}
				*reinterpret_cast<float*>(&newData[newData.size() - 16]) = *reinterpret_cast<const float*>(&imageData[i]);
				*reinterpret_cast<float*>(&newData[newData.size() - 12]) = *reinterpret_cast<const float*>(&imageData[i + 4]);
				*reinterpret_cast<float*>(&newData[newData.size() -  8]) = *reinterpret_cast<const float*>(&imageData[i + 8]);
				*reinterpret_cast<float*>(&newData[newData.size() -  4]) = 1.f;
			}
			break;
		case R32F:
			for (int i = 0; i < imageData.size(); i += 4) {
				for (int j = 0; j < 16; j++) {
					newData.push_back({});
				}
				*reinterpret_cast<float*>(&newData[newData.size() - 16]) = *reinterpret_cast<const float*>(&imageData[i]);
				*reinterpret_cast<float*>(&newData[newData.size() - 12]) = 0.f;
				*reinterpret_cast<float*>(&newData[newData.size() -  8]) = 0.f;
				*reinterpret_cast<float*>(&newData[newData.size() -  4]) = 1.f;
			}
			break;
		case RGBA16161616F:
			for (int i = 0; i < imageData.size(); i += 8) {
				for (int j = 0; j < 16; j++) {
					newData.push_back({});
				}
				*reinterpret_cast<float*>(&newData[newData.size() - 16]) = *math::FloatCompressed16{*reinterpret_cast<const uint16_t*>(&imageData[i])};
				*reinterpret_cast<float*>(&newData[newData.size() - 12]) = *math::FloatCompressed16{*reinterpret_cast<const uint16_t*>(&imageData[i + 2])};
				*reinterpret_cast<float*>(&newData[newData.size() -  8]) = *math::FloatCompressed16{*reinterpret_cast<const uint16_t*>(&imageData[i + 4])};
				*reinterpret_cast<float*>(&newData[newData.size() -  4]) = *math::FloatCompressed16{*reinterpret_cast<const uint16_t*>(&imageData[i + 6])};
			}
			break;
		case RGBA16161616:
			for (int i = 0; i < imageData.size(); i += 8) {
				for (int j = 0; j < 16; j++) {
					newData.push_back({});
				}
				*reinterpret_cast<float*>(&newData[newData.size() - 16]) = static_cast<float>(*reinterpret_cast<const uint16_t*>(&imageData[i])) / 65535.f;
				*reinterpret_cast<float*>(&newData[newData.size() - 12]) = static_cast<float>(*reinterpret_cast<const uint16_t*>(&imageData[i + 2])) / 65535.f;
				*reinterpret_cast<float*>(&newData[newData.size() -  8]) = static_cast<float>(*reinterpret_cast<const uint16_t*>(&imageData[i + 4])) / 65535.f;
				*reinterpret_cast<float*>(&newData[newData.size() -  4]) = static_cast<float>(*reinterpret_cast<const uint16_t*>(&imageData[i + 6])) / 65535.f;
			}
			break;
		default:
			break;
	}
	return newData;
}

[[nodiscard]] std::vector<std::byte> convertImageDataFromRGBA32323232F(std::span<const std::byte> imageData, ImageFormat format) {
	std::vector<std::byte> newData;
	newData.reserve(imageData.size() / (ImageFormatDetails::bpp(ImageFormat::RGBA32323232F) / 8) * (ImageFormatDetails::bpp(format) / 8));
	switch (format) {
		using enum ImageFormat;
		case RGBA32323232F:
			newData = {imageData.begin(), imageData.end()};
			break;
		case RGB323232F:
			for (int i = 0; i < imageData.size(); i += 16) {
				for (int j = 0; j < 12; j++) {
					newData.push_back({});
				}
				*reinterpret_cast<float*>(&newData[newData.size() - 12]) = *reinterpret_cast<const float*>(&imageData[i]);
				*reinterpret_cast<float*>(&newData[newData.size() -  8]) = *reinterpret_cast<const float*>(&imageData[i + 4]);
				*reinterpret_cast<float*>(&newData[newData.size() -  4]) = *reinterpret_cast<const float*>(&imageData[i + 8]);
			}
			break;
		case R32F:
			for (int i = 0; i < imageData.size(); i += 16) {
				for (int j = 0; j < 4; j++) {
					newData.push_back({});
				}
				*reinterpret_cast<float*>(&newData[newData.size() - 4]) = *reinterpret_cast<const float*>(&imageData[i]);
			}
			break;
		case RGBA16161616F:
			for (int i = 0; i < imageData.size(); i += 16) {
				for (int j = 0; j < 8; j++) {
					newData.push_back({});
				}
				*reinterpret_cast<uint16_t*>(&newData[newData.size() - 8]) = math::FloatCompressed16{*reinterpret_cast<const uint16_t*>(&imageData[i])}.toFloat16();
				*reinterpret_cast<uint16_t*>(&newData[newData.size() - 6]) = math::FloatCompressed16{*reinterpret_cast<const uint16_t*>(&imageData[i + 4])}.toFloat16();
				*reinterpret_cast<uint16_t*>(&newData[newData.size() - 4]) = math::FloatCompressed16{*reinterpret_cast<const uint16_t*>(&imageData[i + 8])}.toFloat16();
				*reinterpret_cast<uint16_t*>(&newData[newData.size() - 2]) = math::FloatCompressed16{*reinterpret_cast<const uint16_t*>(&imageData[i + 12])}.toFloat16();
			}
			break;
		case RGBA16161616:
			for (int i = 0; i < imageData.size(); i += 16) {
				for (int j = 0; j < 8; j++) {
					newData.push_back({});
				}
				*reinterpret_cast<uint16_t*>(&newData[newData.size() - 8]) = static_cast<uint16_t>(*reinterpret_cast<const float*>(&imageData[i]) * 65535.f);
				*reinterpret_cast<uint16_t*>(&newData[newData.size() - 6]) = static_cast<uint16_t>(*reinterpret_cast<const float*>(&imageData[i + 4]) * 65535.f);
				*reinterpret_cast<uint16_t*>(&newData[newData.size() - 4]) = static_cast<uint16_t>(*reinterpret_cast<const float*>(&imageData[i + 8]) * 65535.f);
				*reinterpret_cast<uint16_t*>(&newData[newData.size() - 2]) = static_cast<uint16_t>(*reinterpret_cast<const float*>(&imageData[i + 12]) * 65535.f);
			}
			break;
		default:
			break;
	}
	return newData;
}

[[nodiscard]] std::vector<std::byte> convertImageDataFromRGBA8888ToRGBA32323232F(std::span<const std::byte> imageData) {
	std::vector<std::byte> newData;
	newData.reserve(imageData.size() / (ImageFormatDetails::bpp(ImageFormat::RGBA8888) / 8) * (ImageFormatDetails::bpp(ImageFormat::RGBA32323232F) / 8));
	for (int i = 0; i < imageData.size(); i += 4) {
		for (int j = 0; j < 16; j++) {
			newData.push_back({});
		}
		*reinterpret_cast<float*>(&newData[newData.size() - 16]) = static_cast<float>(*reinterpret_cast<const uint16_t*>(&imageData[i])) / 255.f;
		*reinterpret_cast<float*>(&newData[newData.size() - 12]) = static_cast<float>(*reinterpret_cast<const uint16_t*>(&imageData[i + 1])) / 255.f;
		*reinterpret_cast<float*>(&newData[newData.size() -  8]) = static_cast<float>(*reinterpret_cast<const uint16_t*>(&imageData[i + 2])) / 255.f;
		*reinterpret_cast<float*>(&newData[newData.size() -  4]) = static_cast<float>(*reinterpret_cast<const uint16_t*>(&imageData[i + 3])) / 255.f;
	}
	return newData;
}

[[nodiscard]] std::vector<std::byte> convertImageDataFromRGBA32323232FToRGBA8888(std::span<const std::byte> imageData) {
	std::vector<std::byte> newData;
	newData.reserve(imageData.size() / (ImageFormatDetails::bpp(ImageFormat::RGBA32323232F) / 8) * (ImageFormatDetails::bpp(ImageFormat::RGBA8888) / 8));
	for (int i = 0; i < imageData.size(); i += 16) {
		newData.push_back(static_cast<std::byte>(std::clamp(*reinterpret_cast<const float*>(&imageData[i]) * 255, 0.f, 255.f)));
		newData.push_back(static_cast<std::byte>(std::clamp(*reinterpret_cast<const float*>(&imageData[i + 4]) * 255, 0.f, 255.f)));
		newData.push_back(static_cast<std::byte>(std::clamp(*reinterpret_cast<const float*>(&imageData[i + 8]) * 255, 0.f, 255.f)));
		newData.push_back(static_cast<std::byte>(std::clamp(*reinterpret_cast<const float*>(&imageData[i + 12]) * 255, 0.f, 255.f)));
	}
	return newData;
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
			intermediaryData = ::convertImageDataFromRGBA32323232FToRGBA8888(intermediaryData);
		}
	} else {
		intermediaryFormat = ImageFormat::RGBA8888;
		intermediaryData = ::convertImageDataToRGBA8888(imageData, oldFormat);
		if (ImageFormatDetails::large(newFormat)) {
			intermediaryFormat = ImageFormat::RGBA32323232F;
			intermediaryData = ::convertImageDataFromRGBA8888ToRGBA32323232F(intermediaryData);
		}
	}

	if (intermediaryFormat == newFormat) {
		return intermediaryData;
	}

	std::vector<std::byte> finalData;
	if (intermediaryFormat == ImageFormat::RGBA8888) {
		if (ImageFormatDetails::compressed(newFormat)) {
			finalData = ::encodeImageDataFromRGBA8888(intermediaryData, newFormat, width, height);
		} else {
			finalData = ::convertImageDataFromRGBA8888(intermediaryData, newFormat);
		}
	} else {
		finalData = ::convertImageDataFromRGBA32323232F(intermediaryData, newFormat);
	}
	return finalData;
}

std::vector<std::byte> ImageConversion::convertImageDataToFile(std::span<const std::byte> imageData, ImageFormat format, uint16_t width, uint16_t height) {
	std::vector<std::byte> out;
	auto stbWriteFunc = [](void* out, void* data, int size) {
		std::copy(reinterpret_cast<std::byte*>(data), reinterpret_cast<std::byte*>(data) + size, std::back_inserter(*reinterpret_cast<std::vector<std::byte>*>(out)));
	};
	if (ImageFormatDetails::large(format)) {
		auto hdr = convertImageDataToFormat(imageData, format, ImageFormat::RGBA32323232F, width, height);
		stbi_write_hdr_to_func(stbWriteFunc, &out, width, height, ImageFormatDetails::bpp(ImageFormat::RGBA32323232F) / 8, reinterpret_cast<float*>(hdr.data()));
	} else {
		// This works for compressed formats too
		auto rgba = convertImageDataToFormat(imageData, format, ImageFormat::RGBA8888, width, height);
		stbi_write_png_to_func(stbWriteFunc, &out, width, height, ImageFormatDetails::bpp(ImageFormat::RGBA8888) / 8, rgba.data(), 0);
	}
	return out;
}
