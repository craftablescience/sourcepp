#include <vtfpp/ImageConversion.h>

#include <algorithm>
#include <bit>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <span>
#include <string_view>
#include <unordered_map>

#ifdef SOURCEPP_BUILD_WITH_OPENCL
#define CL_HPP_MINIMUM_OPENCL_VERSION 120
#define CL_HPP_TARGET_OPENCL_VERSION 120
#include <CL/opencl.hpp>
#endif

#ifdef SOURCEPP_BUILD_WITH_TBB
#include <execution>
#endif

#include <Compressonator.h>
#include <sourcepp/Macros.h>
#include <sourcepp/Math.h>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_STATIC
#define STBI_NO_FAILURE_STRINGS
#define STBI_NO_STDIO
#include <stb_image.h>

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_STATIC
#include <stb_image_resize2.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_WRITE_STATIC
#define STBI_WRITE_NO_STDIO
#include <stb_image_write.h>

#define TINYEXR_IMPLEMENTATION 1
#ifdef SOURCEPP_BUILD_WITH_THREADS
#define TINYEXR_USE_THREAD 1
#else
#define TINYEXR_USE_THREAD 0
#endif
#include <tinyexr.h>

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
		case R16F:
			return CMP_FORMAT_R_16F;
		case RG1616F:
			return CMP_FORMAT_RG_16F;
		case RGBA16161616F:
			return CMP_FORMAT_RGBA_16F;
		case RGBA16161616:
			return CMP_FORMAT_RGBA_16;
		case R32F:
			return CMP_FORMAT_R_32F;
		case RG3232F:
			return CMP_FORMAT_RG_32F;
		case RGB323232F:
			return CMP_FORMAT_RGB_32F;
		case RGBA32323232F:
			return CMP_FORMAT_RGBA_32F;
		case ATI2N:
			return CMP_FORMAT_ATI2N;
		case ATI1N:
			return CMP_FORMAT_ATI1N;
		case RGBA1010102:
			return CMP_FORMAT_RGBA_1010102;
		case R8:
			return CMP_FORMAT_R_8;
		case BC7:
			return CMP_FORMAT_BC7;
		case BC6H:
			return CMP_FORMAT_BC6H_SF;
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
		case BGRA1010102:
		case RGBX8888:
			return CMP_FORMAT_Unknown;
	}
	return CMP_FORMAT_Unknown;
}

[[nodiscard]] constexpr int imageFormatToSTBIRPixelLayout(ImageFormat format) {
	switch (format) {
		using enum ImageFormat;
		case RGBA8888:
		case UVWQ8888:
		case RGBA16161616:
		case RGBA16161616F:
		case RGBA32323232F:
			return STBIR_RGBA;
		case ABGR8888:
			return STBIR_ABGR;
		case RGB888:
		case RGB323232F:
			return STBIR_RGB;
		case BGR888:
			return STBIR_BGR;
		case I8:
		case P8:
		case R32F:
		case R16F:
		case R8:
			return STBIR_1CHANNEL;
		case ARGB8888:
			return STBIR_ARGB;
		case BGRA8888:
			return STBIR_BGRA;
		case UV88:
		case RG1616F:
		case RG3232F:
			return STBIR_2CHANNEL;
		case IA88:
			return STBIR_RA;
		// We want these to get converted to their respective container format before resize
		case DXT1:
		case DXT1_ONE_BIT_ALPHA:
		case DXT3:
		case DXT5:
		case ATI2N:
		case ATI1N:
		case BC7:
		case BC6H:
		case RGB565:
		case A8:
		case RGB888_BLUESCREEN:
		case BGR888_BLUESCREEN:
		case RGBX8888:
		case BGRX8888:
		case BGR565:
		case BGRX5551:
		case BGRA4444:
		case BGRA5551:
		case UVLX8888:
		case EMPTY:
		case RGBA1010102:
		case BGRA1010102:
			break;
	}
	return -1;
}

[[nodiscard]] constexpr int imageFormatToSTBIRDataType(ImageFormat format, bool srgb = false) {
	switch (format) {
		using enum ImageFormat;
		case RGBA8888:
		case ABGR8888:
		case RGB888:
		case BGR888:
		case I8:
		case IA88:
		case P8:
		case A8:
		case RGB888_BLUESCREEN:
		case BGR888_BLUESCREEN:
		case ARGB8888:
		case BGRA8888:
		case BGRX8888:
		case UV88:
		case UVWQ8888:
		case UVLX8888:
		case RGBX8888:
		case R8:
			return srgb ? STBIR_TYPE_UINT8_SRGB : STBIR_TYPE_UINT8;
		case R16F:
		case RG1616F:
		case RGBA16161616F:
			return STBIR_TYPE_HALF_FLOAT;
		case RGBA16161616:
			return STBIR_TYPE_UINT16;
		case R32F:
		case RG3232F:
		case RGB323232F:
		case RGBA32323232F:
			return STBIR_TYPE_FLOAT;
		case RGB565:
		case BGR565:
		case BGRX5551:
		case BGRA4444:
		case DXT1_ONE_BIT_ALPHA:
		case BGRA5551:
		case DXT1:
		case DXT3:
		case DXT5:
		case EMPTY:
		case ATI2N:
		case ATI1N:
		case RGBA1010102:
		case BGRA1010102:
		case BC7:
		case BC6H:
			break;
	}
	return -1;
}

[[nodiscard]] std::vector<std::byte> convertImageDataUsingCompressonator(std::span<const std::byte> imageData, ImageFormat oldFormat, ImageFormat newFormat, uint16_t width, uint16_t height) {
	if (imageData.empty()) {
		return {};
	}

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
	using namespace ImageConversion;

	if (imageData.empty()) {
		return {};
	}

	if (format == ImageFormat::RGBA8888 || format == ImageFormat::UVWQ8888) {
		return {imageData.begin(), imageData.end()};
	}

	std::vector<std::byte> newData;
	newData.resize(imageData.size() / (ImageFormatDetails::bpp(format) / 8) * (ImageFormatDetails::bpp(ImageFormat::RGBA8888) / 8));
	std::span newDataSpan{reinterpret_cast<ImagePixel::RGBA8888*>(newData.data()), newData.size() / sizeof(ImagePixel::RGBA8888)};

	#define VTFPP_REMAP_TO_8(value, shift) math::remap<uint8_t>((value), (1 << (shift)) - 1, (1 << 8) - 1)

	#define VTFPP_CONVERT_DETAIL(InputType, r, g, b, a, ...) \
		std::span<const ImagePixel::InputType> imageDataSpan{reinterpret_cast<const ImagePixel::InputType*>(imageData.data()), imageData.size() / sizeof(ImagePixel::InputType)}; \
		std::transform(__VA_ARGS__ __VA_OPT__(,) imageDataSpan.begin(), imageDataSpan.end(), newDataSpan.begin(), [](ImagePixel::InputType pixel) -> ImagePixel::RGBA8888 { \
			return {(r), (g), (b), (a)}; \
		})
#ifdef SOURCEPP_BUILD_WITH_TBB
	#define VTFPP_CONVERT(InputType, r, g, b, a) VTFPP_CONVERT_DETAIL(InputType, r, g, b, a, std::execution::par_unseq)
#else
	#define VTFPP_CONVERT(InputType, r, g, b, a) VTFPP_CONVERT_DETAIL(InputType, r, g, b, a)
#endif
	#define VTFPP_CASE_CONVERT_AND_BREAK(InputType, r, g, b, a) \
		case InputType: { VTFPP_CONVERT(InputType, r, g, b, a); } break

	switch (format) {
		using enum ImageFormat;
		VTFPP_CASE_CONVERT_AND_BREAK(ABGR8888,          pixel.r, pixel.g, pixel.b, pixel.a);
		VTFPP_CASE_CONVERT_AND_BREAK(RGB888,            pixel.r, pixel.g, pixel.b, 0xff);
		VTFPP_CASE_CONVERT_AND_BREAK(RGB888_BLUESCREEN, pixel.r, pixel.g, pixel.b, static_cast<uint8_t>((pixel.r == 0 && pixel.g == 0 && pixel.b == 0xff) ? 0 : 0xff));
		VTFPP_CASE_CONVERT_AND_BREAK(BGR888,            pixel.r, pixel.g, pixel.b, 0xff);
		VTFPP_CASE_CONVERT_AND_BREAK(BGR888_BLUESCREEN, pixel.r, pixel.g, pixel.b, static_cast<uint8_t>((pixel.r == 0 && pixel.g == 0 && pixel.b == 0xff) ? 0 : 0xff));
		VTFPP_CASE_CONVERT_AND_BREAK(RGB565,            VTFPP_REMAP_TO_8(pixel.r, 5), VTFPP_REMAP_TO_8(pixel.g, 6), VTFPP_REMAP_TO_8(pixel.b, 5), 0xff);
		VTFPP_CASE_CONVERT_AND_BREAK(P8,                pixel.p, pixel.p, pixel.p, 0xff);
		VTFPP_CASE_CONVERT_AND_BREAK(I8,                pixel.i, pixel.i, pixel.i, 0xff);
		VTFPP_CASE_CONVERT_AND_BREAK(IA88,              pixel.i, pixel.i, pixel.i, pixel.a);
		VTFPP_CASE_CONVERT_AND_BREAK(A8,                0,       0,       0,       pixel.a);
		VTFPP_CASE_CONVERT_AND_BREAK(ARGB8888,          pixel.r, pixel.g, pixel.b, pixel.a);
		VTFPP_CASE_CONVERT_AND_BREAK(BGRA8888,          pixel.r, pixel.g, pixel.b, pixel.a);
		VTFPP_CASE_CONVERT_AND_BREAK(BGRX8888,          pixel.r, pixel.g, pixel.b, 0xff);
		VTFPP_CASE_CONVERT_AND_BREAK(BGR565,            VTFPP_REMAP_TO_8(pixel.r, 5), VTFPP_REMAP_TO_8(pixel.g, 6), VTFPP_REMAP_TO_8(pixel.b, 5), 0xff);
		VTFPP_CASE_CONVERT_AND_BREAK(BGRA5551,          VTFPP_REMAP_TO_8(pixel.r, 5), VTFPP_REMAP_TO_8(pixel.g, 5), VTFPP_REMAP_TO_8(pixel.b, 5), static_cast<uint8_t>(pixel.a * 0xff));
		VTFPP_CASE_CONVERT_AND_BREAK(BGRX5551,          VTFPP_REMAP_TO_8(pixel.r, 5), VTFPP_REMAP_TO_8(pixel.g, 5), VTFPP_REMAP_TO_8(pixel.b, 5), 0xff);
		VTFPP_CASE_CONVERT_AND_BREAK(BGRA4444,          VTFPP_REMAP_TO_8(pixel.r, 4), VTFPP_REMAP_TO_8(pixel.g, 4), VTFPP_REMAP_TO_8(pixel.b, 4), VTFPP_REMAP_TO_8(pixel.a, 4));
		VTFPP_CASE_CONVERT_AND_BREAK(UV88,              pixel.u, pixel.v, 0,       0xff);
		VTFPP_CASE_CONVERT_AND_BREAK(UVLX8888,          pixel.u, pixel.v, pixel.l, 0xff);
		VTFPP_CASE_CONVERT_AND_BREAK(RGBX8888,          pixel.r, pixel.g, pixel.b, 0xff);
		VTFPP_CASE_CONVERT_AND_BREAK(R8,                pixel.r, 0,       0,       0xff);
		default: SOURCEPP_DEBUG_BREAK; break;
	}

	#undef VTFPP_CASE_CONVERT_AND_BREAK
	#undef VTFPP_CONVERT
	#undef VTFPP_CONVERT_DETAIL
	#undef VTFPP_REMAP_TO_8

	return newData;
}

[[nodiscard]] std::vector<std::byte> convertImageDataFromRGBA8888(std::span<const std::byte> imageData, ImageFormat format) {
	using namespace ImageConversion;

	if (imageData.empty()) {
		return {};
	}

	if (format == ImageFormat::RGBA8888) {
		return {imageData.begin(), imageData.end()};
	}

	std::span imageDataSpan{reinterpret_cast<const ImagePixel::RGBA8888*>(imageData.data()), imageData.size() / sizeof(ImagePixel::RGBA8888)};
	std::vector<std::byte> newData;
	newData.resize(imageData.size() / (ImageFormatDetails::bpp(ImageFormat::RGBA8888) / 8) * (ImageFormatDetails::bpp(format) / 8));

	#define VTFPP_REMAP_FROM_8(value, shift) math::remap<uint8_t>((value), (1 << 8) - 1, (1 << (shift)) - 1)

#ifdef SOURCEPP_BUILD_WITH_TBB
	#define VTFPP_CONVERT(InputType, ...) \
		std::span<ImagePixel::InputType> newDataSpan{reinterpret_cast<ImagePixel::InputType*>(newData.data()), newData.size() / sizeof(ImagePixel::InputType)}; \
		std::transform(std::execution::par_unseq, imageDataSpan.begin(), imageDataSpan.end(), newDataSpan.begin(), [](ImagePixel::RGBA8888 pixel) -> ImagePixel::InputType { \
			return __VA_ARGS__; \
		})
#else
	#define VTFPP_CONVERT(InputType, ...) \
		std::span<ImagePixel::InputType> newDataSpan{reinterpret_cast<ImagePixel::InputType*>(newData.data()), newData.size() / sizeof(ImagePixel::InputType)}; \
		std::transform(imageDataSpan.begin(), imageDataSpan.end(), newDataSpan.begin(), [](ImagePixel::RGBA8888 pixel) -> ImagePixel::InputType { \
			return __VA_ARGS__; \
		})
#endif
	#define VTFPP_CASE_CONVERT_AND_BREAK(InputType, ...) \
		case InputType: { VTFPP_CONVERT(InputType, __VA_ARGS__); } break

	switch (format) {
		using enum ImageFormat;
		VTFPP_CASE_CONVERT_AND_BREAK(ABGR8888,          {pixel.a, pixel.b, pixel.g, pixel.r});
		VTFPP_CASE_CONVERT_AND_BREAK(RGB888,            {pixel.r, pixel.g, pixel.b});
		VTFPP_CASE_CONVERT_AND_BREAK(RGB888_BLUESCREEN, pixel.a < 0xff ? ImagePixel::RGB888_BLUESCREEN{pixel.r, pixel.g, pixel.b} : ImagePixel::RGB888_BLUESCREEN{0, 0, 0xff});
		VTFPP_CASE_CONVERT_AND_BREAK(BGR888,            {pixel.b, pixel.g, pixel.r});
		VTFPP_CASE_CONVERT_AND_BREAK(BGR888_BLUESCREEN, pixel.a < 0xff ? ImagePixel::BGR888_BLUESCREEN{pixel.b, pixel.g, pixel.r} : ImagePixel::BGR888_BLUESCREEN{0xff, 0, 0});
		VTFPP_CASE_CONVERT_AND_BREAK(RGB565,            {VTFPP_REMAP_FROM_8(pixel.r, 5), VTFPP_REMAP_FROM_8(pixel.g, 6), VTFPP_REMAP_FROM_8(pixel.b, 5)});
		VTFPP_CASE_CONVERT_AND_BREAK(P8,                {pixel.r});
		VTFPP_CASE_CONVERT_AND_BREAK(I8,                {pixel.r});
		VTFPP_CASE_CONVERT_AND_BREAK(IA88,              {pixel.r, pixel.a});
		VTFPP_CASE_CONVERT_AND_BREAK(A8,                {pixel.a});
		VTFPP_CASE_CONVERT_AND_BREAK(ARGB8888,          {pixel.a, pixel.r, pixel.g, pixel.b});
		VTFPP_CASE_CONVERT_AND_BREAK(BGRA8888,          {pixel.b, pixel.g, pixel.r, pixel.a});
		VTFPP_CASE_CONVERT_AND_BREAK(BGRX8888,          {pixel.b, pixel.g, pixel.r, 0xff});
		VTFPP_CASE_CONVERT_AND_BREAK(BGR565,            {VTFPP_REMAP_FROM_8(pixel.b, 5), VTFPP_REMAP_FROM_8(pixel.g, 6), VTFPP_REMAP_FROM_8(pixel.r, 5)});
		VTFPP_CASE_CONVERT_AND_BREAK(BGRA5551,          {VTFPP_REMAP_FROM_8(pixel.b, 5), VTFPP_REMAP_FROM_8(pixel.g, 5), VTFPP_REMAP_FROM_8(pixel.r, 5), static_cast<uint8_t>(pixel.a < 0xff ? 1 : 0)});
		VTFPP_CASE_CONVERT_AND_BREAK(BGRX5551,          {VTFPP_REMAP_FROM_8(pixel.b, 5), VTFPP_REMAP_FROM_8(pixel.g, 5), VTFPP_REMAP_FROM_8(pixel.r, 5), 0x1});
		VTFPP_CASE_CONVERT_AND_BREAK(BGRA4444,          {VTFPP_REMAP_FROM_8(pixel.b, 4), VTFPP_REMAP_FROM_8(pixel.g, 4), VTFPP_REMAP_FROM_8(pixel.r, 4), VTFPP_REMAP_FROM_8(pixel.a, 4)});
		VTFPP_CASE_CONVERT_AND_BREAK(UV88,              {pixel.r, pixel.g});
		VTFPP_CASE_CONVERT_AND_BREAK(UVLX8888,          {pixel.r, pixel.g, pixel.b});
		VTFPP_CASE_CONVERT_AND_BREAK(RGBX8888,          {pixel.r, pixel.g, pixel.b, 0xff});
		VTFPP_CASE_CONVERT_AND_BREAK(R8,                {pixel.r});
		default: SOURCEPP_DEBUG_BREAK; break;
	}

	#undef VTFPP_CASE_CONVERT_AND_BREAK
	#undef VTFPP_CONVERT
	#undef VTFPP_REMAP_FROM_8

	return newData;
}

[[nodiscard]] std::vector<std::byte> convertImageDataToRGBA16161616(std::span<const std::byte> imageData, ImageFormat format) {
	using namespace ImageConversion;

	if (imageData.empty()) {
		return {};
	}

	if (format == ImageFormat::RGBA16161616) {
		return {imageData.begin(), imageData.end()};
	}

	std::vector<std::byte> newData;
	newData.resize(imageData.size() / (ImageFormatDetails::bpp(format) / 8) * (ImageFormatDetails::bpp(ImageFormat::RGBA16161616) / 8));
	std::span newDataSpan{reinterpret_cast<ImagePixel::RGBA16161616*>(newData.data()), newData.size() / sizeof(ImagePixel::RGBA16161616)};

	#define VTFPP_REMAP_TO_16(value, shift) math::remap<uint16_t>((value), (1 << (shift)) - 1, (1 << 16) - 1)

	#define VTFPP_CONVERT_DETAIL(InputType, r, g, b, a, ...) \
		std::span<const ImagePixel::InputType> imageDataSpan{reinterpret_cast<const ImagePixel::InputType*>(imageData.data()), imageData.size() / sizeof(ImagePixel::InputType)}; \
		std::transform(__VA_ARGS__ __VA_OPT__(,) imageDataSpan.begin(), imageDataSpan.end(), newDataSpan.begin(), [](ImagePixel::InputType pixel) -> ImagePixel::RGBA16161616 { \
			return { static_cast<uint16_t>(r), static_cast<uint16_t>(g), static_cast<uint16_t>(b), static_cast<uint16_t>(a) }; \
		})
#ifdef SOURCEPP_BUILD_WITH_TBB
	#define VTFPP_CONVERT(InputType, r, g, b, a) VTFPP_CONVERT_DETAIL(InputType, r, g, b, a, std::execution::par_unseq)
#else
	#define VTFPP_CONVERT(InputType, r, g, b, a) VTFPP_CONVERT_DETAIL(InputType, r, g, b, a)
#endif
	#define VTFPP_CASE_CONVERT_AND_BREAK(InputType, r, g, b, a) \
		case InputType: { VTFPP_CONVERT(InputType, r, g, b, a); } break

	#define VTFPP_CONVERT_REMAP(InputType, r, g, b, a) \
		do { \
			if constexpr (ImageFormatDetails::alpha(ImageFormat::InputType) > 1) { \
				VTFPP_CONVERT(InputType, \
						VTFPP_REMAP_TO_16((r), ImageFormatDetails::red(ImageFormat::InputType)), \
						VTFPP_REMAP_TO_16((g), ImageFormatDetails::green(ImageFormat::InputType)), \
						VTFPP_REMAP_TO_16((b), ImageFormatDetails::blue(ImageFormat::InputType)), \
						VTFPP_REMAP_TO_16((a), ImageFormatDetails::alpha(ImageFormat::InputType))); \
			} else if constexpr (ImageFormatDetails::alpha(ImageFormat::InputType) == 1) { \
				VTFPP_CONVERT(InputType, \
						VTFPP_REMAP_TO_16((r), ImageFormatDetails::red(ImageFormat::InputType)), \
						VTFPP_REMAP_TO_16((g), ImageFormatDetails::green(ImageFormat::InputType)), \
						VTFPP_REMAP_TO_16((b), ImageFormatDetails::blue(ImageFormat::InputType)), \
						(a) * 0xff); \
			} else { \
				VTFPP_CONVERT(InputType, \
						VTFPP_REMAP_TO_16((r), ImageFormatDetails::red(ImageFormat::InputType)), \
						VTFPP_REMAP_TO_16((g), ImageFormatDetails::green(ImageFormat::InputType)), \
						VTFPP_REMAP_TO_16((b), ImageFormatDetails::blue(ImageFormat::InputType)), \
						(a)); \
			} \
		} while (false)
	#define VTFPP_CASE_CONVERT_REMAP_AND_BREAK(InputType, r, g, b, a) \
		case InputType: { VTFPP_CONVERT_REMAP(InputType, r, g, b, a); } \
		break

	switch (format) {
		using enum ImageFormat;
		VTFPP_CASE_CONVERT_REMAP_AND_BREAK(RGBA1010102, pixel.r, pixel.g, pixel.b, pixel.a);
		VTFPP_CASE_CONVERT_REMAP_AND_BREAK(BGRA1010102, pixel.r, pixel.g, pixel.b, pixel.a);
		default: SOURCEPP_DEBUG_BREAK; break;
	}

	#undef VTFPP_CASE_CONVERT_REMAP_AND_BREAK
	#undef VTFPP_CONVERT_REMAP
	#undef VTFPP_CASE_CONVERT_AND_BREAK
	#undef VTFPP_CONVERT
	#undef VTFPP_CONVERT_DETAIL
	#undef VTFPP_REMAP_TO_16

	return newData;
}

[[nodiscard]] std::vector<std::byte> convertImageDataFromRGBA16161616(std::span<const std::byte> imageData, ImageFormat format) {
	using namespace ImageConversion;

	if (imageData.empty()) {
		return {};
	}

	if (format == ImageFormat::RGBA16161616) {
		return {imageData.begin(), imageData.end()};
	}

	std::span imageDataSpan{reinterpret_cast<const ImagePixel::RGBA16161616*>(imageData.data()), imageData.size() / sizeof(ImagePixel::RGBA16161616)};
	std::vector<std::byte> newData;
	newData.resize(imageData.size() / (ImageFormatDetails::bpp(ImageFormat::RGBA16161616) / 8) * (ImageFormatDetails::bpp(format) / 8));

	#define VTFPP_REMAP_FROM_16(value, shift) static_cast<uint8_t>(math::remap<uint16_t>((value), (1 << 16) - 1, (1 << (shift)) - 1))

#ifdef SOURCEPP_BUILD_WITH_TBB
	#define VTFPP_CONVERT(InputType, ...) \
		std::span<ImagePixel::InputType> newDataSpan{reinterpret_cast<ImagePixel::InputType*>(newData.data()), newData.size() / sizeof(ImagePixel::InputType)}; \
		std::transform(std::execution::par_unseq, imageDataSpan.begin(), imageDataSpan.end(), newDataSpan.begin(), [](ImagePixel::RGBA16161616 pixel) -> ImagePixel::InputType { \
			return __VA_ARGS__; \
		})
#else
	#define VTFPP_CONVERT(InputType, ...) \
		std::span<ImagePixel::InputType> newDataSpan{reinterpret_cast<ImagePixel::InputType*>(newData.data()), newData.size() / sizeof(ImagePixel::InputType)}; \
		std::transform(imageDataSpan.begin(), imageDataSpan.end(), newDataSpan.begin(), [](ImagePixel::RGBA16161616 pixel) -> ImagePixel::InputType { \
			return __VA_ARGS__; \
		})
#endif
	#define VTFPP_CASE_CONVERT_AND_BREAK(InputType, ...) \
		case InputType: { VTFPP_CONVERT(InputType, __VA_ARGS__); } break

	switch (format) {
		using enum ImageFormat;
		VTFPP_CASE_CONVERT_AND_BREAK(RGBA1010102, {VTFPP_REMAP_FROM_16(pixel.r, 10), VTFPP_REMAP_FROM_16(pixel.g, 10), VTFPP_REMAP_FROM_16(pixel.b, 10), VTFPP_REMAP_FROM_16(pixel.a, 2)});
		VTFPP_CASE_CONVERT_AND_BREAK(BGRA1010102, {VTFPP_REMAP_FROM_16(pixel.b, 10), VTFPP_REMAP_FROM_16(pixel.g, 10), VTFPP_REMAP_FROM_16(pixel.r, 10), VTFPP_REMAP_FROM_16(pixel.a, 2)});
		default: SOURCEPP_DEBUG_BREAK; break;
	}

	#undef VTFPP_CASE_CONVERT_AND_BREAK
	#undef VTFPP_CONVERT
	#undef VTFPP_REMAP_FROM_16

	return newData;
}

[[nodiscard]] std::vector<std::byte> convertImageDataToRGBA32323232F(std::span<const std::byte> imageData, ImageFormat format) {
	if (imageData.empty()) {
		return {};
	}

	if (format == ImageFormat::RGBA32323232F) {
		return {imageData.begin(), imageData.end()};
	}

	std::vector<std::byte> newData;
	newData.resize(imageData.size() / (ImageFormatDetails::bpp(format) / 8) * (ImageFormatDetails::bpp(ImageFormat::RGBA32323232F) / 8));
	std::span newDataSpan{reinterpret_cast<ImagePixel::RGBA32323232F*>(newData.data()), newData.size() / sizeof(ImagePixel::RGBA32323232F)};

	#define VTFPP_CONVERT_DETAIL(InputType, r, g, b, a, ...) \
		std::span<const ImagePixel::InputType> imageDataSpan{reinterpret_cast<const ImagePixel::InputType*>(imageData.data()), imageData.size() / sizeof(ImagePixel::InputType)}; \
		std::transform(__VA_ARGS__ __VA_OPT__(,) imageDataSpan.begin(), imageDataSpan.end(), newDataSpan.begin(), [](ImagePixel::InputType pixel) -> ImagePixel::RGBA32323232F { return {(r), (g), (b), (a)}; })
#ifdef SOURCEPP_BUILD_WITH_TBB
	#define VTFPP_CONVERT(InputType, r, g, b, a) VTFPP_CONVERT_DETAIL(InputType, r, g, b, a, std::execution::par_unseq)
#else
	#define VTFPP_CONVERT(InputType, r, g, b, a) VTFPP_CONVERT_DETAIL(InputType, r, g, b, a)
#endif
	#define VTFPP_CASE_CONVERT_AND_BREAK(InputType, r, g, b, a) \
		case InputType: { VTFPP_CONVERT(InputType, r, g, b, a); } break

	switch (format) {
		using enum ImageFormat;
		VTFPP_CASE_CONVERT_AND_BREAK(R32F,          pixel.r, 0.f,     0.f,     1.f);
		VTFPP_CASE_CONVERT_AND_BREAK(RG3232F,       pixel.r, pixel.g, 0.f,     1.f);
		VTFPP_CASE_CONVERT_AND_BREAK(RGB323232F,    pixel.r, pixel.g, pixel.b, 1.f);
		VTFPP_CASE_CONVERT_AND_BREAK(R16F,          pixel.r, 0.f,     0.f,     1.f);
		VTFPP_CASE_CONVERT_AND_BREAK(RG1616F,       pixel.r, pixel.g, 0.f,     1.f);
		VTFPP_CASE_CONVERT_AND_BREAK(RGBA16161616F, pixel.r, pixel.g, pixel.b, pixel.a);
		default: SOURCEPP_DEBUG_BREAK; break;
	}

	#undef VTFPP_CASE_CONVERT_AND_BREAK
	#undef VTFPP_CONVERT
	#undef VTFPP_CONVERT_DETAIL

	return newData;
}

[[nodiscard]] std::vector<std::byte> convertImageDataFromRGBA32323232F(std::span<const std::byte> imageData, ImageFormat format) {
	using namespace ImageConversion;

	if (imageData.empty()) {
		return {};
	}

	if (format == ImageFormat::RGBA32323232F) {
		return {imageData.begin(), imageData.end()};
	}

	std::span imageDataSpan{reinterpret_cast<const ImagePixel::RGBA32323232F*>(imageData.data()), imageData.size() / sizeof(ImagePixel::RGBA32323232F)};
	std::vector<std::byte> newData;
	newData.resize(imageData.size() / (ImageFormatDetails::bpp(ImageFormat::RGBA32323232F) / 8) * (ImageFormatDetails::bpp(format) / 8));

#ifdef SOURCEPP_BUILD_WITH_TBB
	#define VTFPP_CONVERT(InputType, ...) \
		std::span<ImagePixel::InputType> newDataSpan{reinterpret_cast<ImagePixel::InputType*>(newData.data()), newData.size() / sizeof(ImagePixel::InputType)}; \
		std::transform(std::execution::par_unseq, imageDataSpan.begin(), imageDataSpan.end(), newDataSpan.begin(), [](ImagePixel::RGBA32323232F pixel) -> ImagePixel::InputType { \
			return __VA_ARGS__; \
		})
#else
	#define VTFPP_CONVERT(InputType, ...) \
		std::span<ImagePixel::InputType> newDataSpan{reinterpret_cast<ImagePixel::InputType*>(newData.data()), newData.size() / sizeof(ImagePixel::InputType)}; \
		std::transform(imageDataSpan.begin(), imageDataSpan.end(), newDataSpan.begin(), [](ImagePixel::RGBA32323232F pixel) -> ImagePixel::InputType { \
			return __VA_ARGS__; \
		})
#endif
	#define VTFPP_CASE_CONVERT_AND_BREAK(InputType, ...) \
		case InputType: { VTFPP_CONVERT(InputType, __VA_ARGS__); } break

	switch (format) {
		using enum ImageFormat;
		VTFPP_CASE_CONVERT_AND_BREAK(R32F,          {pixel.r});
		VTFPP_CASE_CONVERT_AND_BREAK(RG3232F,       {pixel.r, pixel.g});
		VTFPP_CASE_CONVERT_AND_BREAK(RGB323232F,    {pixel.r, pixel.g, pixel.b});
		VTFPP_CASE_CONVERT_AND_BREAK(R16F,          {half{pixel.r}});
		VTFPP_CASE_CONVERT_AND_BREAK(RG1616F,       {half{pixel.r}, half{pixel.g}});
		VTFPP_CASE_CONVERT_AND_BREAK(RGBA16161616F, {half{pixel.r}, half{pixel.g}, half{pixel.b}, half{pixel.a}});
		default: SOURCEPP_DEBUG_BREAK; break;
	}

	#undef VTFPP_CASE_CONVERT_AND_BREAK
	#undef VTFPP_CONVERT

	return newData;
}

[[nodiscard]] std::vector<std::byte> convertImageDataFromRGBA8888ToRGBA32323232F(std::span<const std::byte> imageData) {
	if (imageData.empty()) {
		return {};
	}

	std::vector<std::byte> newData;
	newData.resize(imageData.size() / (ImageFormatDetails::bpp(ImageFormat::RGBA8888) / 8) * (ImageFormatDetails::bpp(ImageFormat::RGBA32323232F) / 8));
	std::span newDataSpan{reinterpret_cast<ImagePixel::RGBA32323232F*>(newData.data()), newData.size() / sizeof(ImagePixel::RGBA32323232F)};

	std::span imageDataSpan{reinterpret_cast<const ImagePixel::RGBA8888*>(imageData.data()), imageData.size() / sizeof(ImagePixel::RGBA8888)};
	std::transform(
#ifdef SOURCEPP_BUILD_WITH_TBB
			std::execution::par_unseq,
#endif
			imageDataSpan.begin(), imageDataSpan.end(), newDataSpan.begin(), [](ImagePixel::RGBA8888 pixel) -> ImagePixel::RGBA32323232F {
		return {
			static_cast<float>(pixel.r) / static_cast<float>((1 << 8) - 1),
			static_cast<float>(pixel.g) / static_cast<float>((1 << 8) - 1),
			static_cast<float>(pixel.b) / static_cast<float>((1 << 8) - 1),
			static_cast<float>(pixel.a) / static_cast<float>((1 << 8) - 1),
		};
	});

	return newData;
}

[[nodiscard]] std::vector<std::byte> convertImageDataFromRGBA32323232FToRGBA8888(std::span<const std::byte> imageData) {
	if (imageData.empty()) {
		return {};
	}

	std::vector<std::byte> newData;
	newData.resize(imageData.size() / (ImageFormatDetails::bpp(ImageFormat::RGBA32323232F) / 8) * (ImageFormatDetails::bpp(ImageFormat::RGBA8888) / 8));
	std::span newDataSpan{reinterpret_cast<ImagePixel::RGBA8888*>(newData.data()), newData.size() / sizeof(ImagePixel::RGBA8888)};

	std::span imageDataSpan{reinterpret_cast<const ImagePixel::RGBA32323232F*>(imageData.data()), imageData.size() / sizeof(ImagePixel::RGBA32323232F)};
	std::transform(
#ifdef SOURCEPP_BUILD_WITH_TBB
			std::execution::par_unseq,
#endif
			imageDataSpan.begin(), imageDataSpan.end(), newDataSpan.begin(), [](ImagePixel::RGBA32323232F pixel) -> ImagePixel::RGBA8888 {
		return {
			static_cast<uint8_t>(std::clamp(pixel.r, 0.f, 1.f) * ((1 << 8) - 1)),
			static_cast<uint8_t>(std::clamp(pixel.g, 0.f, 1.f) * ((1 << 8) - 1)),
			static_cast<uint8_t>(std::clamp(pixel.b, 0.f, 1.f) * ((1 << 8) - 1)),
			static_cast<uint8_t>(std::clamp(pixel.a, 0.f, 1.f) * ((1 << 8) - 1)),
		};
	});

	return newData;
}

[[nodiscard]] std::vector<std::byte> convertImageDataFromRGBA8888ToRGBA16161616(std::span<const std::byte> imageData) {
	if (imageData.empty()) {
		return {};
	}

	std::vector<std::byte> newData;
	newData.resize(imageData.size() / (ImageFormatDetails::bpp(ImageFormat::RGBA8888) / 8) * (ImageFormatDetails::bpp(ImageFormat::RGBA16161616) / 8));
	std::span newDataSpan{reinterpret_cast<ImagePixel::RGBA16161616*>(newData.data()), newData.size() / sizeof(ImagePixel::RGBA16161616)};

	std::span imageDataSpan{reinterpret_cast<const ImagePixel::RGBA8888*>(imageData.data()), imageData.size() / sizeof(ImagePixel::RGBA8888)};
	std::transform(
#ifdef SOURCEPP_BUILD_WITH_TBB
			std::execution::par_unseq,
#endif
			imageDataSpan.begin(), imageDataSpan.end(), newDataSpan.begin(), [](ImagePixel::RGBA8888 pixel) -> ImagePixel::RGBA16161616 {
		return {
			math::remap<uint16_t>(pixel.r, (1 << 8) - 1, (1 << 16) - 1),
			math::remap<uint16_t>(pixel.g, (1 << 8) - 1, (1 << 16) - 1),
			math::remap<uint16_t>(pixel.b, (1 << 8) - 1, (1 << 16) - 1),
			math::remap<uint16_t>(pixel.a, (1 << 8) - 1, (1 << 16) - 1),
		};
	});

	return newData;
}

[[nodiscard]] std::vector<std::byte> convertImageDataFromRGBA16161616ToRGBA8888(std::span<const std::byte> imageData) {
	if (imageData.empty()) {
		return {};
	}

	std::vector<std::byte> newData;
	newData.resize(imageData.size() / (ImageFormatDetails::bpp(ImageFormat::RGBA16161616) / 8) * (ImageFormatDetails::bpp(ImageFormat::RGBA8888) / 8));
	std::span newDataSpan{reinterpret_cast<ImagePixel::RGBA8888*>(newData.data()), newData.size() / sizeof(ImagePixel::RGBA8888)};

	std::span imageDataSpan{reinterpret_cast<const ImagePixel::RGBA16161616*>(imageData.data()), imageData.size() / sizeof(ImagePixel::RGBA16161616)};
	std::transform(
#ifdef SOURCEPP_BUILD_WITH_TBB
			std::execution::par_unseq,
#endif
			imageDataSpan.begin(), imageDataSpan.end(), newDataSpan.begin(), [](ImagePixel::RGBA16161616 pixel) -> ImagePixel::RGBA8888 {
		return {
			static_cast<uint8_t>(math::remap<uint16_t>(pixel.r, (1 << 16) - 1, (1 << 8) - 1)),
			static_cast<uint8_t>(math::remap<uint16_t>(pixel.g, (1 << 16) - 1, (1 << 8) - 1)),
			static_cast<uint8_t>(math::remap<uint16_t>(pixel.b, (1 << 16) - 1, (1 << 8) - 1)),
			static_cast<uint8_t>(math::remap<uint16_t>(pixel.a, (1 << 16) - 1, (1 << 8) - 1)),
		};
	});

	return newData;
}

[[nodiscard]] std::vector<std::byte> convertImageDataFromRGBA32323232FToRGBA16161616(std::span<const std::byte> imageData) {
	if (imageData.empty()) {
		return {};
	}

	std::vector<std::byte> newData;
	newData.resize(imageData.size() / (ImageFormatDetails::bpp(ImageFormat::RGBA32323232F) / 8) * (ImageFormatDetails::bpp(ImageFormat::RGBA16161616) / 8));
	std::span newDataSpan{reinterpret_cast<ImagePixel::RGBA16161616*>(newData.data()), newData.size() / sizeof(ImagePixel::RGBA16161616)};

	std::span imageDataSpan{reinterpret_cast<const ImagePixel::RGBA32323232F*>(imageData.data()), imageData.size() / sizeof(ImagePixel::RGBA32323232F)};
	std::transform(
#ifdef SOURCEPP_BUILD_WITH_TBB
			std::execution::par_unseq,
#endif
			imageDataSpan.begin(), imageDataSpan.end(), newDataSpan.begin(), [](ImagePixel::RGBA32323232F pixel) -> ImagePixel::RGBA16161616 {
		return {
			static_cast<uint16_t>(std::clamp(pixel.r, 0.f, 1.f) * ((1 << 16) - 1)),
			static_cast<uint16_t>(std::clamp(pixel.g, 0.f, 1.f) * ((1 << 16) - 1)),
			static_cast<uint16_t>(std::clamp(pixel.b, 0.f, 1.f) * ((1 << 16) - 1)),
			static_cast<uint16_t>(std::clamp(pixel.a, 0.f, 1.f) * ((1 << 16) - 1)),
		};
	});

	return newData;
}

[[nodiscard]] std::vector<std::byte> convertImageDataFromRGBA16161616ToRGBA32323232F(std::span<const std::byte> imageData) {
	if (imageData.empty()) {
		return {};
	}

	std::vector<std::byte> newData;
	newData.resize(imageData.size() / (ImageFormatDetails::bpp(ImageFormat::RGBA16161616) / 8) * (ImageFormatDetails::bpp(ImageFormat::RGBA32323232F) / 8));
	std::span newDataSpan{reinterpret_cast<ImagePixel::RGBA32323232F*>(newData.data()), newData.size() / sizeof(ImagePixel::RGBA32323232F)};

	std::span imageDataSpan{reinterpret_cast<const ImagePixel::RGBA16161616*>(imageData.data()), imageData.size() / sizeof(ImagePixel::RGBA16161616)};
	std::transform(
#ifdef SOURCEPP_BUILD_WITH_TBB
			std::execution::par_unseq,
#endif
			imageDataSpan.begin(), imageDataSpan.end(), newDataSpan.begin(), [](ImagePixel::RGBA16161616 pixel) -> ImagePixel::RGBA32323232F {
		return {
			static_cast<float>(pixel.r) / static_cast<float>((1 << 16) - 1),
			static_cast<float>(pixel.g) / static_cast<float>((1 << 16) - 1),
			static_cast<float>(pixel.b) / static_cast<float>((1 << 16) - 1),
			static_cast<float>(pixel.a) / static_cast<float>((1 << 16) - 1),
		};
	});

	return newData;
}

void convertHDRIToCubeMapCPUFallback(std::span<const float> imageDataRGBA32323232F, ImageFormat outputFormat, uint16_t width, uint16_t height, uint16_t resolution, bool bilinear, const std::array<std::array<math::Vec3f, 3>, 6>& startRightUp, std::array<std::vector<std::byte>, 6>& faceData) {
	for (int i = 0; i < 6; i++) {
		const math::Vec3f& start = startRightUp[i][0];
		const math::Vec3f& right = startRightUp[i][1];
		const math::Vec3f& up    = startRightUp[i][2];

		faceData[i].resize(resolution * resolution * sizeof(ImagePixel::RGBA32323232F));
		std::span<float> face{reinterpret_cast<float*>(faceData[i].data()), reinterpret_cast<float*>(faceData[i].data() + faceData[i].size())};

		for (int row = 0; row < resolution; row++) {
			for (int col = 0; col < resolution; col++) {
				math::Vec3f pixelDirection3d{
					start[0] + ((float) col * 2.f + 0.5f) / (float) resolution * right[0] + ((float) row * 2.f + 0.5f) / (float) resolution * up[0],
					start[1] + ((float) col * 2.f + 0.5f) / (float) resolution * right[1] + ((float) row * 2.f + 0.5f) / (float) resolution * up[1],
					start[2] + ((float) col * 2.f + 0.5f) / (float) resolution * right[2] + ((float) row * 2.f + 0.5f) / (float) resolution * up[2],
				};
				float azimuth = std::atan2(pixelDirection3d[0], -pixelDirection3d[2]) + math::pi_f32; // add pi to move range to 0-360 deg
				float elevation = std::atan(pixelDirection3d[1] / std::sqrt(pixelDirection3d[0] * pixelDirection3d[0] + pixelDirection3d[2] * pixelDirection3d[2])) + math::pi_f32 / 2.f;
				float colHdri = (azimuth / math::pi_f32 / 2.f) * (float) width; // add pi to azimuth to move range to 0-360 deg
				float rowHdri = (elevation / math::pi_f32) * (float) height;
				if (!bilinear) {
					int colNearest = std::clamp((int) colHdri, 0, width - 1);
					int rowNearest = std::clamp((int) rowHdri, 0, height - 1);
					face[col * 4 + resolution * row * 4 + 0] = imageDataRGBA32323232F[colNearest * 4 + width * rowNearest * 4 + 0];
					face[col * 4 + resolution * row * 4 + 1] = imageDataRGBA32323232F[colNearest * 4 + width * rowNearest * 4 + 1];
					face[col * 4 + resolution * row * 4 + 2] = imageDataRGBA32323232F[colNearest * 4 + width * rowNearest * 4 + 2];
					face[col * 4 + resolution * row * 4 + 3] = imageDataRGBA32323232F[colNearest * 4 + width * rowNearest * 4 + 3];
				} else {
					float intCol, intRow;
					// factor gives the contribution of the next column, while the contribution of intCol is 1 - factor
					float factorCol = std::modf(colHdri - 0.5f, &intCol);
					float factorRow = std::modf(rowHdri - 0.5f, &intRow);
					int low_idx_row = static_cast<int>(intRow);
					int low_idx_column = static_cast<int>(intCol);
					int high_idx_column;
					if (factorCol < 0.0f) {
						// modf can only give a negative value if the azimuth falls in the first pixel, left of the
						// center, so we have to mix with the pixel on the opposite side of the panoramic image
						high_idx_column = width - 1;
					} else if (low_idx_column == width - 1) {
						// if we are in the right-most pixel, and fall right of the center, mix with the left-most pixel
						high_idx_column = 0;
					} else {
						high_idx_column = low_idx_column + 1;
					}
					int high_idx_row;
					if (factorRow < 0.0f) {
						high_idx_row = height - 1;
					} else if (low_idx_row == height - 1) {
						high_idx_row = 0;
					} else {
						high_idx_row = low_idx_row + 1;
					}
					factorCol = std::abs(factorCol);
					factorRow = std::abs(factorRow);
					float f1 = (1 - factorRow) * (1 - factorCol);
					float f2 = factorRow * (1 - factorCol);
					float f3 = (1 - factorRow) * factorCol;
					float f4 = factorRow * factorCol;
					for (int j = 0; j < 4; j++) {
						auto interpolatedValue = static_cast<uint8_t>(
							face[low_idx_column * 4  + width * low_idx_row  * 4 + j] * f1 +
							face[low_idx_column * 4  + width * high_idx_row * 4 + j] * f2 +
							face[high_idx_column * 4 + width * low_idx_row  * 4 + j] * f3 +
							face[high_idx_column * 4 + width * high_idx_row * 4 + j] * f4
						);
						face[col * 4 + resolution * row * 4 + j] = std::clamp<uint8_t>(interpolatedValue, 0, 255);
					}
				}
			}
		}
		if (outputFormat != ImageFormat::RGBA32323232F) {
			faceData[i] = ImageConversion::convertImageDataToFormat(faceData[i], ImageFormat::RGBA32323232F, outputFormat, resolution, resolution);
		}
	}
}

} // namespace

std::vector<std::byte> ImageConversion::convertImageDataToFormat(std::span<const std::byte> imageData, ImageFormat oldFormat, ImageFormat newFormat, uint16_t width, uint16_t height) {
	if (imageData.empty() || oldFormat == ImageFormat::EMPTY) {
		return {};
	}

	if (oldFormat == newFormat) {
		return {imageData.begin(), imageData.end()};
	}

	std::vector<std::byte> newData;

	const ImageFormat intermediaryOldFormat = ImageFormatDetails::containerFormat(oldFormat);
	if (ImageFormatDetails::compressed(oldFormat)) {
		newData = ::convertImageDataUsingCompressonator(imageData, oldFormat, intermediaryOldFormat, width, height);
	} else {
		switch (intermediaryOldFormat) {
			case ImageFormat::RGBA8888:      newData = ::convertImageDataToRGBA8888(imageData, oldFormat);      break;
			case ImageFormat::RGBA16161616:  newData = ::convertImageDataToRGBA16161616(imageData, oldFormat);  break;
			case ImageFormat::RGBA32323232F: newData = ::convertImageDataToRGBA32323232F(imageData, oldFormat); break;
			default:                         return {};
		}
	}

	if (intermediaryOldFormat == newFormat) {
		return newData;
	}

	const ImageFormat intermediaryNewFormat = ImageFormatDetails::containerFormat(newFormat);
	if (intermediaryOldFormat != intermediaryNewFormat) {
		if (intermediaryOldFormat == ImageFormat::RGBA8888) {
			if (intermediaryNewFormat == ImageFormat::RGBA16161616) {
				newData = ::convertImageDataFromRGBA8888ToRGBA16161616(newData);
			} else if (intermediaryNewFormat == ImageFormat::RGBA32323232F) {
				newData = ::convertImageDataFromRGBA8888ToRGBA32323232F(newData);
			} else {
				return {};
			}
		} else if (intermediaryOldFormat == ImageFormat::RGBA16161616) {
			if (intermediaryNewFormat == ImageFormat::RGBA8888) {
				newData = ::convertImageDataFromRGBA16161616ToRGBA8888(newData);
			} else if (intermediaryNewFormat == ImageFormat::RGBA32323232F) {
				newData = ::convertImageDataFromRGBA16161616ToRGBA32323232F(newData);
			} else {
				return {};
			}
		} else if (intermediaryOldFormat == ImageFormat::RGBA32323232F) {
			if (intermediaryNewFormat == ImageFormat::RGBA8888) {
				newData = ::convertImageDataFromRGBA32323232FToRGBA8888(newData);
			} else if (intermediaryNewFormat == ImageFormat::RGBA16161616) {
				newData = ::convertImageDataFromRGBA32323232FToRGBA16161616(newData);
			} else {
				return {};
			}
		} else {
			return {};
		}
	}

	if (intermediaryNewFormat == newFormat) {
		return newData;
	}

	if (ImageFormatDetails::compressed(newFormat)) {
		newData = ::convertImageDataUsingCompressonator(newData, intermediaryNewFormat, newFormat, width, height);
	} else {
		switch (intermediaryNewFormat) {
			case ImageFormat::RGBA8888:      newData = ::convertImageDataFromRGBA8888(newData, newFormat);      break;
			case ImageFormat::RGBA16161616:  newData = ::convertImageDataFromRGBA16161616(newData, newFormat);  break;
			case ImageFormat::RGBA32323232F: newData = ::convertImageDataFromRGBA32323232F(newData, newFormat); break;
			default:                         return {};
		}
	}

	return newData;
}

std::vector<std::byte> ImageConversion::convertSeveralImageDataToFormat(std::span<const std::byte> imageData, ImageFormat oldFormat, ImageFormat newFormat, uint8_t mipCount, uint16_t frameCount, uint16_t faceCount, uint16_t width, uint16_t height, uint16_t sliceCount) {
	if (imageData.empty() || oldFormat == ImageFormat::EMPTY) {
		return {};
	}

	if (oldFormat == newFormat) {
		return {imageData.begin(), imageData.end()};
	}

	std::vector<std::byte> out(ImageFormatDetails::getDataLength(newFormat, mipCount, frameCount, faceCount, width, height, sliceCount));
	for(int mip = mipCount - 1; mip >= 0; mip--) {
		for (int frame = 0; frame < frameCount; frame++) {
			for (int face = 0; face < faceCount; face++) {
				for (int slice = 0; slice < sliceCount; slice++) {
					if (uint32_t oldOffset, oldLength; ImageFormatDetails::getDataPosition(oldOffset, oldLength, oldFormat, mip, mipCount, frame, frameCount, face, faceCount, width, height, slice, sliceCount)) {
						const auto convertedImageData = ImageConversion::convertImageDataToFormat({imageData.data() + oldOffset, oldLength}, oldFormat, newFormat, ImageDimensions::getMipDim(mip, width), ImageDimensions::getMipDim(mip, height));
						if (uint32_t newOffset, newLength; ImageFormatDetails::getDataPosition(newOffset, newLength, newFormat, mip, mipCount, frame, frameCount, face, faceCount, width, height, slice, sliceCount) && newLength == convertedImageData.size()) {
							std::memcpy(out.data() + newOffset, convertedImageData.data(), newLength);
						}
					}
				}
			}
		}
	}
	return out;
}

std::array<std::vector<std::byte>, 6> ImageConversion::convertHDRIToCubeMap(std::span<const std::byte> imageData, ImageFormat format, uint16_t width, uint16_t height, uint16_t resolution, bool bilinear) {
	if (imageData.empty() || format == ImageFormat::EMPTY) {
		return {};
	}

	if (!resolution) {
		resolution = height;
	}

	std::span<const float> imageDataRGBA32323232F{reinterpret_cast<const float*>(imageData.data()), reinterpret_cast<const float*>(imageData.data() + imageData.size())};

	std::vector<std::byte> possiblyConvertedDataOrEmptyDontUseMeDirectly;
	if (format != ImageFormat::RGBA32323232F) {
		possiblyConvertedDataOrEmptyDontUseMeDirectly = convertImageDataToFormat(imageData, format, ImageFormat::RGBA32323232F, width, height);
		imageDataRGBA32323232F = {reinterpret_cast<const float*>(possiblyConvertedDataOrEmptyDontUseMeDirectly.data()), reinterpret_cast<const float*>(possiblyConvertedDataOrEmptyDontUseMeDirectly.data() + possiblyConvertedDataOrEmptyDontUseMeDirectly.size())};
	}

	// For each face, contains the 3d starting point (corresponding to left bottom pixel), right direction,
	// and up direction in 3d space, corresponding to pixel x,y coordinates of each face
	static constexpr std::array<std::array<math::Vec3f, 3>, 6> startRightUp = {{
		{{{-1.0f, -1.0f, -1.0f}, { 1.0f, 0.0f,  0.0f}, {0.0f, 1.0f,  0.0f}}}, // front
		{{{ 1.0f, -1.0f,  1.0f}, {-1.0f, 0.0f,  0.0f}, {0.0f, 1.0f,  0.0f}}}, // back
		{{{-1.0f, -1.0f,  1.0f}, { 0.0f, 0.0f, -1.0f}, {0.0f, 1.0f,  0.0f}}}, // left
		{{{ 1.0f, -1.0f, -1.0f}, { 0.0f, 0.0f,  1.0f}, {0.0f, 1.0f,  0.0f}}}, // right
		{{{-1.0f,  1.0f, -1.0f}, { 1.0f, 0.0f,  0.0f}, {0.0f, 0.0f,  1.0f}}}, // up
		{{{-1.0f, -1.0f,  1.0f}, { 1.0f, 0.0f,  0.0f}, {0.0f, 0.0f, -1.0f}}}, // down
	}};

	std::array<std::vector<std::byte>, 6> faceData;

#ifdef SOURCEPP_BUILD_WITH_OPENCL
	std::vector<cl::Platform> platforms;
	if (cl::Platform::get(&platforms) != CL_SUCCESS || platforms.empty()) {
		::convertHDRIToCubeMapCPUFallback(imageDataRGBA32323232F, format, width, height, resolution, bilinear, startRightUp, faceData);
		return faceData;
	}

	std::vector<cl::Device> devices;
	for (const auto& platform : platforms) {
		if (platforms.front().getDevices(CL_DEVICE_TYPE_GPU, &devices) == CL_SUCCESS && !devices.empty()) {
			break;
		}
		devices.clear();
	}
	if (devices.empty()) {
		::convertHDRIToCubeMapCPUFallback(imageDataRGBA32323232F, format, width, height, resolution, bilinear, startRightUp, faceData);
		return faceData;
	}
	const auto& device = devices.front();

	cl::Program::Sources sources{R"(
__constant sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP_TO_EDGE | VTFPP_FILTER;
__kernel void processFace(read_only image2d_t hdriImg, write_only image2d_t faceImg, __global float* startRightUp) {
	int2 pixelCoordFace = (int2)(get_global_id(0), get_global_id(1));
	int resolutionFace = get_global_size(0);
	float3 start = (float3)(startRightUp[0], startRightUp[1], startRightUp[2]);
	float3 right = (float3)(startRightUp[3], startRightUp[4], startRightUp[5]);
	float3 up    = (float3)(startRightUp[6], startRightUp[7], startRightUp[8]);
	float3 direction = (float3)(
		start.x + (pixelCoordFace.x * 2.f + 0.5f)/(float)resolutionFace * right.x + (pixelCoordFace.y * 2.f + 0.5f)/(float)resolutionFace * up.x,
		start.y + (pixelCoordFace.x * 2.f + 0.5f)/(float)resolutionFace * right.y + (pixelCoordFace.y * 2.f + 0.5f)/(float)resolutionFace * up.y,
		start.z + (pixelCoordFace.x * 2.f + 0.5f)/(float)resolutionFace * right.z + (pixelCoordFace.y * 2.f + 0.5f)/(float)resolutionFace * up.z);
	float azimuth = atan2(direction.x, -direction.z) + radians(180.f);
	float elevation = atan(direction.y / sqrt(pow(direction.x, 2) + pow(direction.z, 2))) + radians(90.f);
	float2 pixelCoordHdri = (float2)(azimuth / radians(360.f) * get_image_width(hdriImg), elevation / radians(180.f) * get_image_height(hdriImg));
	uint4 pixel = read_imageui(hdriImg, sampler, pixelCoordHdri);
	write_imageui(faceImg, pixelCoordFace, pixel);
})"};
	cl::Context context{device};
	cl::Program program{context, sources};
	if (int err = program.build(bilinear ? "-cl-std=CL1.2 -DVTFPP_FILTER=CLK_FILTER_LINEAR" : "-cl-std=CL1.2 -DVTFPP_FILTER=CLK_FILTER_NEAREST"); err != CL_SUCCESS) {
#ifdef DEBUG
		if (err == CL_BUILD_PROGRAM_FAILURE) {
			const auto buildLog = program.getBuildInfo<CL_PROGRAM_BUILD_LOG>();
			SOURCEPP_DEBUG_BREAK;
		}
#endif
		::convertHDRIToCubeMapCPUFallback(imageDataRGBA32323232F, format, width, height, resolution, bilinear, startRightUp, faceData);
		return faceData;
	}

	cl::ImageFormat clFormat{CL_RGBA, CL_FLOAT};
	cl::Image2D imgHdri{context, CL_MEM_READ_ONLY, clFormat, width, height};
	cl::Image2D imgFace{context, CL_MEM_WRITE_ONLY, clFormat, resolution, resolution};
	cl::CommandQueue queue{context, device};
	queue.enqueueWriteImage(imgHdri, CL_TRUE, {0, 0, 0}, {width, height, 1}, 0, 0, imageDataRGBA32323232F.data());
	cl::Buffer bufDirections(context, CL_MEM_READ_ONLY, sizeof(float) * 9);

	for (int i = 0; i < 6; i++) {
		auto& face = faceData[i];
		face.resize(resolution * resolution * sizeof(ImagePixel::RGBA32323232F));

		cl::Kernel kernel{program, "processFace"};
		kernel.setArg(0, imgHdri);
		kernel.setArg(1, imgFace);
		kernel.setArg(2, bufDirections);

		queue.enqueueWriteBuffer(bufDirections, CL_TRUE, 0, 9 * sizeof(float), &startRightUp[i][0]);
		queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(resolution, resolution));
		cl::finish();

		std::array<std::size_t, 3> origin_out{0, 0, 0};
		std::array<std::size_t, 3> region_out{resolution, resolution, 1};
		queue.enqueueReadImage(imgFace, CL_TRUE, origin_out, region_out, 0, 0, face.data());
		cl::finish();

		if (format != ImageFormat::RGBA32323232F) {
			faceData[i] = ImageConversion::convertImageDataToFormat(faceData[i], ImageFormat::RGBA32323232F, format, resolution, resolution);
		}
	}
#else
	::convertHDRIToCubeMapCPUFallback(imageDataRGBA32323232F, format, width, height, resolution, bilinear, startRightUp, faceData);
#endif
	return faceData;
}

ImageConversion::FileFormat ImageConversion::getDefaultFileFormatForImageFormat(ImageFormat format) {
	using enum FileFormat;
	return ImageFormatDetails::decimal(format) ? EXR : PNG;
}

std::vector<std::byte> ImageConversion::convertImageDataToFile(std::span<const std::byte> imageData, ImageFormat format, uint16_t width, uint16_t height, FileFormat fileFormat) {
	if (imageData.empty() || format == ImageFormat::EMPTY) {
		return {};
	}
	std::vector<std::byte> out;
	auto stbWriteFunc = [](void* out_, void* data, int size) {
		std::copy_n(static_cast<std::byte*>(data), size, std::back_inserter(*static_cast<std::vector<std::byte>*>(out_)));
	};

	if (fileFormat == FileFormat::DEFAULT) {
		fileFormat = getDefaultFileFormatForImageFormat(format);
	}
	switch (fileFormat) {
		case FileFormat::PNG: {
			if (format == ImageFormat::RGB888) {
				stbi_write_png_to_func(stbWriteFunc, &out, width, height, ImageFormatDetails::bpp(ImageFormat::RGB888) / 8, imageData.data(), 0);
			} else if (format == ImageFormat::RGBA8888) {
				stbi_write_png_to_func(stbWriteFunc, &out, width, height, ImageFormatDetails::bpp(ImageFormat::RGBA8888) / 8, imageData.data(), 0);
			} else if (ImageFormatDetails::opaque(format)) {
				const auto rgb = convertImageDataToFormat(imageData, format, ImageFormat::RGB888, width, height);
				stbi_write_png_to_func(stbWriteFunc, &out, width, height, ImageFormatDetails::bpp(ImageFormat::RGB888) / 8, rgb.data(), 0);
			} else {
				const auto rgba = convertImageDataToFormat(imageData, format, ImageFormat::RGBA8888, width, height);
				stbi_write_png_to_func(stbWriteFunc, &out, width, height, ImageFormatDetails::bpp(ImageFormat::RGBA8888) / 8, rgba.data(), 0);
			}
			break;
		}
		case FileFormat::JPEG: {
			if (format == ImageFormat::RGB888) {
				stbi_write_jpg_to_func(stbWriteFunc, &out, width, height, ImageFormatDetails::bpp(ImageFormat::RGB888) / 8, imageData.data(), 95);
			} else {
				const auto rgb = convertImageDataToFormat(imageData, format, ImageFormat::RGB888, width, height);
				stbi_write_jpg_to_func(stbWriteFunc, &out, width, height, ImageFormatDetails::bpp(ImageFormat::RGB888) / 8, rgb.data(), 95);
			}
			break;
		}
		case FileFormat::BMP: {
			if (format == ImageFormat::RGB888) {
				stbi_write_bmp_to_func(stbWriteFunc, &out, width, height, ImageFormatDetails::bpp(ImageFormat::RGB888) / 8, imageData.data());
			} else if (format == ImageFormat::RGBA8888) {
				stbi_write_bmp_to_func(stbWriteFunc, &out, width, height, ImageFormatDetails::bpp(ImageFormat::RGBA8888) / 8, imageData.data());
			} else if (ImageFormatDetails::opaque(format)) {
				const auto rgb = convertImageDataToFormat(imageData, format, ImageFormat::RGB888, width, height);
				stbi_write_bmp_to_func(stbWriteFunc, &out, width, height, ImageFormatDetails::bpp(ImageFormat::RGB888) / 8, rgb.data());
			} else {
				const auto rgba = convertImageDataToFormat(imageData, format, ImageFormat::RGBA8888, width, height);
				stbi_write_bmp_to_func(stbWriteFunc, &out, width, height, ImageFormatDetails::bpp(ImageFormat::RGBA8888) / 8, rgba.data());
			}
			break;
		}
		case FileFormat::TGA: {
			if (format == ImageFormat::RGB888) {
				stbi_write_tga_to_func(stbWriteFunc, &out, width, height, ImageFormatDetails::bpp(ImageFormat::RGB888) / 8, imageData.data());
			} else if (format == ImageFormat::RGBA8888) {
				stbi_write_tga_to_func(stbWriteFunc, &out, width, height, ImageFormatDetails::bpp(ImageFormat::RGBA8888) / 8, imageData.data());
			} else if (ImageFormatDetails::opaque(format)) {
				const auto rgb = convertImageDataToFormat(imageData, format, ImageFormat::RGB888, width, height);
				stbi_write_tga_to_func(stbWriteFunc, &out, width, height, ImageFormatDetails::bpp(ImageFormat::RGB888) / 8, rgb.data());
			} else {
				const auto rgba = convertImageDataToFormat(imageData, format, ImageFormat::RGBA8888, width, height);
				stbi_write_tga_to_func(stbWriteFunc, &out, width, height, ImageFormatDetails::bpp(ImageFormat::RGBA8888) / 8, rgba.data());
			}
			break;
		}
		case FileFormat::HDR: {
			if (format == ImageFormat::RGB323232F) {
				stbi_write_hdr_to_func(stbWriteFunc, &out, width, height, ImageFormatDetails::bpp(ImageFormat::RGB323232F) / (8 * sizeof(float)), reinterpret_cast<const float*>(imageData.data()));
			} else {
				const auto hdr = convertImageDataToFormat(imageData, format, ImageFormat::RGB323232F, width, height);
				stbi_write_hdr_to_func(stbWriteFunc, &out, width, height, ImageFormatDetails::bpp(ImageFormat::RGB323232F) / (8 * sizeof(float)), reinterpret_cast<const float*>(hdr.data()));
			}
			break;
		}
		case FileFormat::EXR: {
			EXRHeader header;
			InitEXRHeader(&header);

			std::vector<std::byte> rawData;
			if (!ImageFormatDetails::decimal(format) || ImageFormatDetails::compressed(format)) {
				if (ImageFormatDetails::transparent(format)) {
					rawData = convertImageDataToFormat(imageData, format, ImageFormat::RGBA32323232F, width, height);
					format = ImageFormat::RGBA32323232F;
				} else {
					rawData = convertImageDataToFormat(imageData, format, ImageFormat::RGB323232F, width, height);
					format = ImageFormat::RGB323232F;
				}
			} else {
				rawData = {imageData.begin(), imageData.end()};
			}

			header.num_channels = (ImageFormatDetails::red(format) > 0) + (ImageFormatDetails::green(format) > 0) + (ImageFormatDetails::blue(format) > 0) + (ImageFormatDetails::alpha(format) > 0);
			header.channels = static_cast<EXRChannelInfo*>(std::malloc(header.num_channels * sizeof(EXRChannelInfo)));
			header.pixel_types = static_cast<int*>(malloc(header.num_channels * sizeof(int)));
			header.requested_pixel_types = static_cast<int*>(malloc(header.num_channels * sizeof(int)));

			switch (header.num_channels) {
				case 4:
					header.channels[0].name[0] = 'A';
					header.channels[1].name[0] = 'B';
					header.channels[2].name[0] = 'G';
					header.channels[3].name[0] = 'R';
					break;
				case 3:
					header.channels[0].name[0] = 'B';
					header.channels[1].name[0] = 'G';
					header.channels[2].name[0] = 'R';
					break;
				case 2:
					header.channels[0].name[0] = 'G';
					header.channels[1].name[0] = 'R';
					break;
				case 1:
					header.channels[0].name[0] = 'R';
					break;
				default:
					FreeEXRHeader(&header);
					return {};
			}
			for (int i = 0; i < header.num_channels; i++) {
				header.channels[i].name[1] = '\0';
			}

			int pixelType = (ImageFormatDetails::red(format) / 8) == sizeof(half) ? TINYEXR_PIXELTYPE_HALF : TINYEXR_PIXELTYPE_FLOAT;
			for (int i = 0; i < header.num_channels; i++) {
				header.pixel_types[i] = pixelType;
				header.requested_pixel_types[i] = pixelType;
			}

			std::vector<std::vector<std::byte>> images(header.num_channels);
			std::vector<void*> imagePtrs(header.num_channels);
			switch (header.num_channels) {
				case 4:
					if (pixelType == TINYEXR_PIXELTYPE_HALF) {
						images[0] = extractChannelFromImageData(imageData, &ImagePixel::RGBA16161616F::a);
						images[1] = extractChannelFromImageData(imageData, &ImagePixel::RGBA16161616F::b);
						images[2] = extractChannelFromImageData(imageData, &ImagePixel::RGBA16161616F::g);
						images[3] = extractChannelFromImageData(imageData, &ImagePixel::RGBA16161616F::r);
					} else {
						images[0] = extractChannelFromImageData(imageData, &ImagePixel::RGBA32323232F::a);
						images[1] = extractChannelFromImageData(imageData, &ImagePixel::RGBA32323232F::b);
						images[2] = extractChannelFromImageData(imageData, &ImagePixel::RGBA32323232F::g);
						images[3] = extractChannelFromImageData(imageData, &ImagePixel::RGBA32323232F::r);
					}
					break;
				case 3:
					if (pixelType == TINYEXR_PIXELTYPE_HALF) {
						// We should not be here!
						FreeEXRHeader(&header);
						return {};
					}
					images[0] = extractChannelFromImageData(imageData, &ImagePixel::RGB323232F::b);
					images[1] = extractChannelFromImageData(imageData, &ImagePixel::RGB323232F::g);
					images[2] = extractChannelFromImageData(imageData, &ImagePixel::RGB323232F::r);
					break;
				case 2:
					if (pixelType == TINYEXR_PIXELTYPE_HALF) {
						images[0] = extractChannelFromImageData(imageData, &ImagePixel::RG1616F::g);
						images[1] = extractChannelFromImageData(imageData, &ImagePixel::RG1616F::r);
					} else {
						images[0] = extractChannelFromImageData(imageData, &ImagePixel::RG3232F::g);
						images[1] = extractChannelFromImageData(imageData, &ImagePixel::RG3232F::r);
					}
					break;
				case 1:
					images[0] = rawData;
					break;
				default:
					FreeEXRHeader(&header);
					return {};
			}
			for (int i = 0; i < header.num_channels; i++) {
				imagePtrs[i] = images[i].data();
			}

			EXRImage image;
			InitEXRImage(&image);
			image.width = width;
			image.height = height;
			image.images = reinterpret_cast<unsigned char**>(imagePtrs.data());
			image.num_channels = header.num_channels;

			unsigned char* data = nullptr;
			const char* err = nullptr;

			size_t size = SaveEXRImageToMemory(&image, &header, &data, &err);
			if (err) {
				FreeEXRErrorMessage(err);
				FreeEXRHeader(&header);
				return {};
			}
			if (data) {
				out = {reinterpret_cast<std::byte*>(data), reinterpret_cast<std::byte*>(data) + size};
				std::free(data);
			}

			FreeEXRHeader(&header);
			break;
		}
		case FileFormat::DEFAULT:
			break;
	}
	return out;
}

std::vector<std::byte> ImageConversion::convertFileToImageData(std::span<const std::byte> fileData, ImageFormat& format, int& width, int& height, int& frameCount) {
	stbi_convert_iphone_png_to_rgb(true);

	format = ImageFormat::EMPTY;
	width = 0;
	height = 0;
	int channels = 0;
	frameCount = 1;

	// EXR
	if (EXRVersion version; ParseEXRVersionFromMemory(&version, reinterpret_cast<const unsigned char*>(fileData.data()), fileData.size()) == TINYEXR_SUCCESS) {
		if (version.multipart || version.non_image) {
			return {};
		}

		EXRHeader header;
		InitEXRHeader(&header);
		const char* err = nullptr;
		if (ParseEXRHeaderFromMemory(&header, &version, reinterpret_cast<const unsigned char*>(fileData.data()), fileData.size(), &err) != TINYEXR_SUCCESS) {
			FreeEXRErrorMessage(err);
			return {};
		}

		// Sanity check
		if (header.num_channels < 1) {
			FreeEXRHeader(&header);
			return {};
		}

		// Define the channel names we support (RGBA, greyscale)
		std::unordered_map<std::string_view, int> channelIndices{{"R", -1}, {"G",  -1}, {"B",  -1}, {"A", -1}, {"Y", -1}};

		// Get channel type (EXR supports different types per channel, we do not)
		// Rather than bailing we ask EXR to convert the lowest precision data
		auto channelType = header.pixel_types[0];
		for (int i = 1; i < header.num_channels; i++) {
			// UINT -> HALF -> FLOAT
			if (header.pixel_types[i] > channelType && channelIndices.contains(header.channels[i].name)) {
				channelType = header.pixel_types[i];
			}
		}
		// requested_pixel_types field only supports floats
		if (channelType == TINYEXR_PIXELTYPE_UINT) {
			channelType = TINYEXR_PIXELTYPE_HALF;
		}

		// Determine proper format to use
		for (int i = 0; i < header.num_channels; i++) {
			if (channelIndices.contains(header.channels[i].name)) {
				channelIndices[header.channels[i].name] = i;
			}
		}
		if (channelIndices["Y"] >= 0) {
			if (channelIndices["A"] >= 0) {
				format = channelType == TINYEXR_PIXELTYPE_HALF ? ImageFormat::RGBA16161616F : ImageFormat::RGBA32323232F;
			} else {
				if (channelType == TINYEXR_PIXELTYPE_HALF) {
					// VTF has no RGB161616F
					channelType = TINYEXR_PIXELTYPE_FLOAT;
				}
				format = ImageFormat::RGB323232F;
			}
			channelIndices["R"] = channelIndices["Y"];
			channelIndices["G"] = channelIndices["Y"];
			channelIndices["B"] = channelIndices["Y"];
		} else if (channelIndices["A"] >= 0) {
			format = channelType == TINYEXR_PIXELTYPE_HALF ? ImageFormat::RGBA16161616F : ImageFormat::RGBA32323232F;
		} else if (channelIndices["B"] >= 0) {
			if (channelType == TINYEXR_PIXELTYPE_HALF) {
				// VTF has no RGB161616F
				channelType = TINYEXR_PIXELTYPE_FLOAT;
			}
			format = ImageFormat::RGB323232F;
		} else if (channelIndices["G"] >= 0) {
			format = channelType == TINYEXR_PIXELTYPE_HALF ? ImageFormat::RG1616F : ImageFormat::RG3232F;
		} else if (channelIndices["R"] >= 0) {
			format = channelType == TINYEXR_PIXELTYPE_HALF ? ImageFormat::R16F : ImageFormat::R32F;
		} else {
			FreeEXRHeader(&header);
			return {};
		}

		// Now that channelType has stopped changing, we can set it properly
		for (int i = 0; i < header.num_channels; i++) {
			if (header.pixel_types[i] != channelType && channelIndices.contains(header.channels[i].name)) {
				header.requested_pixel_types[i] = channelType;
			}
		}

		EXRImage image;
		InitEXRImage(&image);
		if (LoadEXRImageFromMemory(&image, &header, reinterpret_cast<const unsigned char*>(fileData.data()), fileData.size(), &err) != TINYEXR_SUCCESS) {
			FreeEXRErrorMessage(err);
			FreeEXRHeader(&header);
			return {};
		}

		width = image.width;
		height = image.height;

		// Merge channel data into a single buffer
		std::vector<std::byte> combinedChannels(width * height * (ImageFormatDetails::bpp(format) / 8));
		const auto populateBuffer = [
			hasRed=ImageFormatDetails::red(format) > 0,
			hasGreen=ImageFormatDetails::green(format) > 0,
			hasBlue=ImageFormatDetails::blue(format) > 0,
			hasAlpha=ImageFormatDetails::alpha(format) > 0,
			width,
			height,
			&header,
			r=channelIndices["R"],
			g=channelIndices["G"],
			b=channelIndices["B"],
			a=channelIndices["A"],
			&image,
			&combinedChannels
		]<typename C> {
			const auto channelCount = hasRed + hasGreen + hasBlue + hasAlpha;
			std::span out{reinterpret_cast<C*>(combinedChannels.data()), combinedChannels.size() / sizeof(C)};
			if (header.tiled) {
				for (int t = 0; t < image.num_tiles; t++) {
					auto** src = reinterpret_cast<C**>(image.tiles[t].images);
					for (int j = 0; j < header.tile_size_y; j++) {
						for (int i = 0; i < header.tile_size_x; i++) {
							const auto ii = static_cast<uint64_t>(image.tiles[t].offset_x) * header.tile_size_x + i;
							const auto jj = static_cast<uint64_t>(image.tiles[t].offset_y) * header.tile_size_y + j;
							const auto idx = ii + jj * image.width;

							if (ii >= image.width || jj >= image.height) {
								continue;
							}

							const auto srcIdx = j * static_cast<uint64_t>(header.tile_size_x) + i;
							if (r >= 0)        out[idx * channelCount + 0] = src[r][srcIdx];
							else if (hasRed)   out[idx * channelCount + 0] = 0.f;
							if (g >= 0)        out[idx * channelCount + 1] = src[g][srcIdx];
							else if (hasGreen) out[idx * channelCount + 1] = 0.f;
							if (b >= 0)        out[idx * channelCount + 2] = src[b][srcIdx];
							else if (hasBlue)  out[idx * channelCount + 2] = 0.f;
							if (a >= 0)        out[idx * channelCount + 3] = src[a][srcIdx];
							else if (hasAlpha) out[idx * channelCount + 3] = 1.f;
						}
					}
				}
			} else {
				auto** src = reinterpret_cast<C**>(image.images);
				for (uint64_t i = 0; i < width * height; i++) {
					if (r >= 0)        out[i * channelCount + 0] = src[r][i];
					else if (hasRed)   out[i * channelCount + 0] = 0.f;
					if (g >= 0)        out[i * channelCount + 1] = src[g][i];
					else if (hasGreen) out[i * channelCount + 1] = 0.f;
					if (b >= 0)        out[i * channelCount + 2] = src[b][i];
					else if (hasBlue)  out[i * channelCount + 2] = 0.f;
					if (a >= 0)        out[i * channelCount + 3] = src[a][i];
					else if (hasAlpha) out[i * channelCount + 3] = 1.f;
				}
			}
		};
		if (channelType == TINYEXR_PIXELTYPE_HALF) {
			populateBuffer.operator()<half>();
		} else {
			populateBuffer.operator()<float>();
		}

		FreeEXRImage(&image);
		FreeEXRHeader(&header);
		return combinedChannels;
	}

	// HDR
	if (stbi_is_hdr_from_memory(reinterpret_cast<const stbi_uc*>(fileData.data()), static_cast<int>(fileData.size()))) {
		const std::unique_ptr<float, void(*)(void*)> stbImage{
			stbi_loadf_from_memory(reinterpret_cast<const stbi_uc*>(fileData.data()), static_cast<int>(fileData.size()), &width, &height, &channels, 0),
			&stbi_image_free,
		};
		if (!stbImage) {
			return {};
		}

		switch (channels) {
			case 1:  format = ImageFormat::R32F;          break;
			case 2:  format = ImageFormat::RG3232F;       break;
			case 3:  format = ImageFormat::RGB323232F;    break;
			case 4:  format = ImageFormat::RGBA32323232F; break;
			default: return {};
		}

		return {reinterpret_cast<std::byte*>(stbImage.get()), reinterpret_cast<std::byte*>(stbImage.get()) + ImageFormatDetails::getDataLength(format, width, height)};
	}

	// GIF
	if (fileData.size() >= 3 && static_cast<char>(fileData[0]) == 'G' && static_cast<char>(fileData[1]) == 'I' && static_cast<char>(fileData[2]) == 'F') {
		const std::unique_ptr<stbi_uc, void(*)(void*)> stbImage{
			stbi_load_gif_from_memory(reinterpret_cast<const stbi_uc*>(fileData.data()), static_cast<int>(fileData.size()), nullptr, &width, &height, &frameCount, &channels, 0),
			&stbi_image_free,
		};
		if (!stbImage || !frameCount) {
			return {};
		}

		switch (channels) {
			case 1:  format = ImageFormat::I8;       break;
			case 2:  format = ImageFormat::UV88;     break;
			case 3:  format = ImageFormat::RGB888;   break;
			case 4:  format = ImageFormat::RGBA8888; break;
			default: return {};
		}

		return {reinterpret_cast<std::byte*>(stbImage.get()), reinterpret_cast<std::byte*>(stbImage.get() + (ImageFormatDetails::getDataLength(format, width, height) * frameCount))};
	}

	// 16-bit single-frame image
	if (stbi_is_16_bit_from_memory(reinterpret_cast<const stbi_uc*>(fileData.data()), static_cast<int>(fileData.size()))) {
		const std::unique_ptr<stbi_us, void(*)(void*)> stbImage{
			stbi_load_16_from_memory(reinterpret_cast<const stbi_uc*>(fileData.data()), static_cast<int>(fileData.size()), &width, &height, &channels, 0),
			&stbi_image_free,
		};
		if (!stbImage) {
			return {};
		}

		if (channels == 4) {
			format = ImageFormat::RGBA16161616;
		} else if (channels >= 1 && channels < 4) {
			// There are no other 16-bit integer formats in Source, so we have to do a conversion here
			format = ImageFormat::RGBA16161616;

			std::vector<std::byte> out(ImageFormatDetails::getDataLength(format, width, height));
			std::span<ImagePixel::RGBA16161616> outPixels{reinterpret_cast<ImagePixel::RGBA16161616*>(out.data()), out.size() / sizeof(ImagePixel::RGBA16161616)};

			switch (channels) {
				case 1: {
					std::span<uint16_t> inPixels{reinterpret_cast<uint16_t*>(stbImage.get()), outPixels.size()};
					std::transform(
#ifdef SOURCEPP_BUILD_WITH_TBB
						std::execution::par_unseq,
#endif
						inPixels.begin(), inPixels.end(), outPixels.begin(), [](uint16_t pixel) -> ImagePixel::RGBA16161616 {
						return {pixel, 0, 0, 0xffff};
					});
					return out;
				}
				case 2: {
					struct RG1616 {
						uint16_t r;
						uint16_t g;
					};
					std::span<RG1616> inPixels{reinterpret_cast<RG1616*>(stbImage.get()), outPixels.size()};
					std::transform(
#ifdef SOURCEPP_BUILD_WITH_TBB
						std::execution::par_unseq,
#endif
						inPixels.begin(), inPixels.end(), outPixels.begin(), [](RG1616 pixel) -> ImagePixel::RGBA16161616 {
						return {pixel.r, pixel.g, 0, 0xffff};
					});
					return out;
				}
				case 3: {
					struct RGB161616 {
						uint16_t r;
						uint16_t g;
						uint16_t b;
					};
					std::span<RGB161616> inPixels{reinterpret_cast<RGB161616*>(stbImage.get()), outPixels.size()};
					std::transform(
#ifdef SOURCEPP_BUILD_WITH_TBB
						std::execution::par_unseq,
#endif
						inPixels.begin(), inPixels.end(), outPixels.begin(), [](RGB161616 pixel) -> ImagePixel::RGBA16161616 {
						return {pixel.r, pixel.g, pixel.b, 0xffff};
					});
					return out;
				}
				default:
					return {};
			}
		} else {
			return {};
		}

		return {reinterpret_cast<std::byte*>(stbImage.get()), reinterpret_cast<std::byte*>(stbImage.get()) + ImageFormatDetails::getDataLength(format, width, height)};
	}

	// 8-bit or less single frame image
	const std::unique_ptr<stbi_uc, void(*)(void*)> stbImage{
		stbi_load_from_memory(reinterpret_cast<const stbi_uc*>(fileData.data()), static_cast<int>(fileData.size()), &width, &height, &channels, 0),
		&stbi_image_free,
	};
	if (!stbImage) {
		return {};
	}

	switch (channels) {
		case 1:  format = ImageFormat::I8;       break;
		case 2:  format = ImageFormat::UV88;     break;
		case 3:  format = ImageFormat::RGB888;   break;
		case 4:  format = ImageFormat::RGBA8888; break;
		default: return {};
	}

	return {reinterpret_cast<std::byte*>(stbImage.get()), reinterpret_cast<std::byte*>(stbImage.get()) + ImageFormatDetails::getDataLength(format, width, height)};
}

uint16_t ImageConversion::getResizedDim(uint16_t n, ResizeMethod method) {
	switch (method) {
		case ResizeMethod::NONE:                 break;
		case ResizeMethod::POWER_OF_TWO_BIGGER:  return std::bit_ceil(n);
		case ResizeMethod::POWER_OF_TWO_SMALLER: return std::bit_floor(n);
		case ResizeMethod::POWER_OF_TWO_NEAREST: return math::nearestPowerOf2(n);
	}
	return n;
}

void ImageConversion::setResizedDims(uint16_t& width, ResizeMethod widthResize, uint16_t& height, ResizeMethod heightResize) {
	width = getResizedDim(width, widthResize);
	height = getResizedDim(height, heightResize);
}

std::vector<std::byte> ImageConversion::resizeImageData(std::span<const std::byte> imageData, ImageFormat format, uint16_t width, uint16_t newWidth, uint16_t height, uint16_t newHeight, bool srgb, ResizeFilter filter, ResizeEdge edge) {
	if (imageData.empty() || format == ImageFormat::EMPTY) {
		return {};
	}
	const auto pixelLayout = ::imageFormatToSTBIRPixelLayout(format);
	if (pixelLayout == -1) {
		const auto containerFormat = ImageFormatDetails::containerFormat(format);
		const auto in = convertImageDataToFormat(imageData, format, containerFormat, width, height);
		std::vector<std::byte> intermediary(ImageFormatDetails::getDataLength(containerFormat, newWidth, newHeight));
		stbir_resize(in.data(), width, height, ImageFormatDetails::bpp(containerFormat) / 8 * width, intermediary.data(), newWidth, newHeight, ImageFormatDetails::bpp(containerFormat) / 8 * newWidth, static_cast<stbir_pixel_layout>(::imageFormatToSTBIRPixelLayout(containerFormat)), static_cast<stbir_datatype>(::imageFormatToSTBIRDataType(containerFormat, srgb)), STBIR_EDGE_CLAMP, static_cast<stbir_filter>(filter));
		return convertImageDataToFormat(intermediary, containerFormat, format, newWidth, newHeight);
	}
	std::vector<std::byte> out(ImageFormatDetails::getDataLength(format, newWidth, newHeight));
	stbir_resize(imageData.data(), width, height, ImageFormatDetails::bpp(format) / 8 * width, out.data(), newWidth, newHeight, ImageFormatDetails::bpp(format) / 8 * newWidth, static_cast<stbir_pixel_layout>(pixelLayout), static_cast<stbir_datatype>(::imageFormatToSTBIRDataType(format, srgb)), static_cast<stbir_edge>(edge), static_cast<stbir_filter>(filter));
	return out;
}

std::vector<std::byte> ImageConversion::resizeImageDataStrict(std::span<const std::byte> imageData, ImageFormat format, uint16_t width, uint16_t newWidth, uint16_t& widthOut, ResizeMethod widthResize, uint16_t height, uint16_t newHeight, uint16_t& heightOut, ResizeMethod heightResize, bool srgb, ResizeFilter filter, ResizeEdge edge) {
	if (imageData.empty() || format == ImageFormat::EMPTY) {
		return {};
	}
	widthOut = getResizedDim(newWidth, widthResize);
	heightOut = getResizedDim(newHeight, heightResize);
	return resizeImageData(imageData, format, width, widthOut, height, heightOut, srgb, filter, edge);
}
