#include <vtfpp/ImageConversion.h>

#include <algorithm>
#include <bit>
#include <cstring>
#include <memory>
#include <span>

#ifdef SOURCEPP_BUILD_WITH_TBB
#include <execution>
#endif

#include <Compressonator.h>
#include <sourcepp/math/Float.h>

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

using namespace sourcepp;
using namespace vtfpp;

namespace {

[[nodiscard]] constexpr CMP_FORMAT imageFormatToCompressonatorFormat(ImageFormat format) {
	switch (format) {
		using enum ImageFormat;
		case RGBA8888:
		case UVWQ8888:
		case UVLX8888:
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
		case EMPTY:
			return CMP_FORMAT_Unknown;
	}
	return CMP_FORMAT_Unknown;
}

[[nodiscard]] constexpr int imageFormatToSTBIRPixelLayout(ImageFormat format) {
	switch (format) {
		using enum ImageFormat;
		case RGBA8888:
		case UVWQ8888:
		case UVLX8888:
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
			return STBIR_1CHANNEL;
		case ARGB8888:
			return STBIR_ARGB;
		case BGRA8888:
			return STBIR_BGRA;
		case UV88:
			return STBIR_2CHANNEL;
		case IA88:
			return STBIR_RA;
		// We want these to get converted to RGBA8888 before resize
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
		case BGRX8888:
		case BGR565:
		case BGRX5551:
		case BGRA4444:
		case BGRA5551:
		case EMPTY:
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
			return srgb ? STBIR_TYPE_UINT8_SRGB : STBIR_TYPE_UINT8;
		case RGBA16161616F:
			return STBIR_TYPE_HALF_FLOAT;
		case RGBA16161616:
			return STBIR_TYPE_UINT16;
		case R32F:
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

	if (format == ImageFormat::RGBA8888 || format == ImageFormat::UVWQ8888 || format == ImageFormat::UVLX8888) {
		return {imageData.begin(), imageData.end()};
	}

	std::vector<std::byte> newData;
	newData.resize(imageData.size() / (ImageFormatDetails::bpp(format) / 8) * (ImageFormatDetails::bpp(ImageFormat::RGBA8888) / 8));
	std::span<ImagePixel::RGBA8888> newDataSpan{reinterpret_cast<ImagePixel::RGBA8888*>(newData.data()), newData.size() / sizeof(ImagePixel::RGBA8888)};

	#define VTFPP_REMAP_TO_8(value, shift) math::remap<uint8_t>((value), (1 << (shift)) - 1, (1 << 8) - 1)

	#define VTFPP_CONVERT_DETAIL(InputType, r, g, b, a, ...) \
		std::span<const ImagePixel::InputType> imageDataSpan{reinterpret_cast<const ImagePixel::InputType*>(imageData.data()), imageData.size() / sizeof(ImagePixel::InputType)}; \
		std::transform(__VA_ARGS__ __VA_OPT__(,) imageDataSpan.begin(), imageDataSpan.end(), newDataSpan.begin(), [](ImagePixel::InputType pixel) -> ImagePixel::RGBA8888 { \
			return {(r), (g), (b), (a)}; \
		})
#ifdef SOURCEPP_BUILD_WITH_TBB
	#define VTFPP_CONVERT(InputType, r, g, b, a) VTFPP_CONVERT_DETAIL(InputType, r, g, b, a, std::execution::unseq)
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
		default: break;
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

	std::span<const ImagePixel::RGBA8888> imageDataSpan{reinterpret_cast<const ImagePixel::RGBA8888*>(imageData.data()), imageData.size() / sizeof(ImagePixel::RGBA8888)};
	std::vector<std::byte> newData;
	newData.resize(imageData.size() / (ImageFormatDetails::bpp(ImageFormat::RGBA8888) / 8) * (ImageFormatDetails::bpp(format) / 8));

	#define VTFPP_REMAP_FROM_8(value, shift) math::remap<uint8_t>((value), (1 << 8) - 1, (1 << (shift)) - 1)

#ifdef SOURCEPP_BUILD_WITH_TBB
	#define VTFPP_CONVERT(InputType, ...) \
		std::span<ImagePixel::InputType> newDataSpan{reinterpret_cast<ImagePixel::InputType*>(newData.data()), newData.size() / sizeof(ImagePixel::InputType)}; \
		std::transform(std::execution::unseq, imageDataSpan.begin(), imageDataSpan.end(), newDataSpan.begin(), [](ImagePixel::RGBA8888 pixel) -> ImagePixel::InputType { \
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
		default: break;
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
	std::span<ImagePixel::RGBA16161616> newDataSpan{reinterpret_cast<ImagePixel::RGBA16161616*>(newData.data()), newData.size() / sizeof(ImagePixel::RGBA16161616)};

	#define VTFPP_REMAP_TO_16(value, shift) math::remap<uint16_t>((value), (1 << (shift)) - 1, (1 << 16) - 1)

	#define VTFPP_CONVERT_DETAIL(InputType, r, g, b, a, ...) \
		std::span<const ImagePixel::InputType> imageDataSpan{reinterpret_cast<const ImagePixel::InputType*>(imageData.data()), imageData.size() / sizeof(ImagePixel::InputType)}; \
		std::transform(__VA_ARGS__ __VA_OPT__(,) imageDataSpan.begin(), imageDataSpan.end(), newDataSpan.begin(), [](ImagePixel::InputType pixel) -> ImagePixel::RGBA16161616 { \
			return { static_cast<uint16_t>(r), static_cast<uint16_t>(g), static_cast<uint16_t>(b), static_cast<uint16_t>(a) }; \
		})
#ifdef SOURCEPP_BUILD_WITH_TBB
	#define VTFPP_CONVERT(InputType, r, g, b, a) VTFPP_CONVERT_DETAIL(InputType, r, g, b, a, std::execution::unseq)
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
		VTFPP_CASE_CONVERT_REMAP_AND_BREAK(ABGR8888,          pixel.r, pixel.g, pixel.b, pixel.a);
		VTFPP_CASE_CONVERT_REMAP_AND_BREAK(RGB888,            pixel.r, pixel.g, pixel.b, 0xff);
		VTFPP_CASE_CONVERT_REMAP_AND_BREAK(RGB888_BLUESCREEN, pixel.r, pixel.g, pixel.b, (pixel.r == 0 && pixel.g == 0 && pixel.b == 0xff) ? 0 : 0xff);
		VTFPP_CASE_CONVERT_REMAP_AND_BREAK(BGR888,            pixel.r, pixel.g, pixel.b, 0xff);
		VTFPP_CASE_CONVERT_REMAP_AND_BREAK(BGR888_BLUESCREEN, pixel.r, pixel.g, pixel.b, (pixel.r == 0 && pixel.g == 0 && pixel.b == 0xff) ? 0 : 0xff);
		VTFPP_CASE_CONVERT_REMAP_AND_BREAK(RGB565,            pixel.r, pixel.g, pixel.b, 0xff);
		VTFPP_CASE_CONVERT_REMAP_AND_BREAK(P8,                pixel.p, pixel.p, pixel.p, 0xff);
		VTFPP_CASE_CONVERT_REMAP_AND_BREAK(I8,                pixel.i, pixel.i, pixel.i, 0xff);
		VTFPP_CASE_CONVERT_REMAP_AND_BREAK(IA88,              pixel.i, pixel.i, pixel.i, pixel.a);
		VTFPP_CASE_CONVERT_REMAP_AND_BREAK(A8,                0,       0,       0,       pixel.a);
		VTFPP_CASE_CONVERT_REMAP_AND_BREAK(ARGB8888,          pixel.r, pixel.g, pixel.b, pixel.a);
		VTFPP_CASE_CONVERT_REMAP_AND_BREAK(BGRA8888,          pixel.r, pixel.g, pixel.b, pixel.a);
		VTFPP_CASE_CONVERT_REMAP_AND_BREAK(BGRX8888,          pixel.r, pixel.g, pixel.b, 0xff);
		VTFPP_CASE_CONVERT_REMAP_AND_BREAK(BGR565,            pixel.r, pixel.g, pixel.b, 0xff);
		VTFPP_CASE_CONVERT_REMAP_AND_BREAK(BGRA5551,          pixel.r, pixel.g, pixel.b, pixel.a);
		VTFPP_CASE_CONVERT_REMAP_AND_BREAK(BGRX5551,          pixel.r, pixel.g, pixel.b, 1);
		VTFPP_CASE_CONVERT_REMAP_AND_BREAK(BGRA4444,          pixel.r, pixel.g, pixel.b, pixel.a);
		VTFPP_CASE_CONVERT_REMAP_AND_BREAK(UV88,              pixel.u, pixel.v, 0,       0xff);
		default: break;
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

	std::span<const ImagePixel::RGBA16161616> imageDataSpan{reinterpret_cast<const ImagePixel::RGBA16161616*>(imageData.data()), imageData.size() / sizeof(ImagePixel::RGBA16161616)};
	std::vector<std::byte> newData;
	newData.resize(imageData.size() / (ImageFormatDetails::bpp(ImageFormat::RGBA16161616) / 8) * (ImageFormatDetails::bpp(format) / 8));

	#define VTFPP_REMAP_FROM_16(value, shift) static_cast<uint8_t>(math::remap<uint16_t>((value), (1 << 16) - 1, (1 << (shift)) - 1))

#ifdef SOURCEPP_BUILD_WITH_TBB
	#define VTFPP_CONVERT(InputType, ...) \
		std::span<ImagePixel::InputType> newDataSpan{reinterpret_cast<ImagePixel::InputType*>(newData.data()), newData.size() / sizeof(ImagePixel::InputType)}; \
		std::transform(std::execution::unseq, imageDataSpan.begin(), imageDataSpan.end(), newDataSpan.begin(), [](ImagePixel::RGBA16161616 pixel) -> ImagePixel::InputType { \
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
		VTFPP_CASE_CONVERT_AND_BREAK(ABGR8888,          {VTFPP_REMAP_FROM_16(pixel.a, 8), VTFPP_REMAP_FROM_16(pixel.b, 8), VTFPP_REMAP_FROM_16(pixel.g, 8), VTFPP_REMAP_FROM_16(pixel.r, 8)});
		VTFPP_CASE_CONVERT_AND_BREAK(RGB888,            {VTFPP_REMAP_FROM_16(pixel.r, 8), VTFPP_REMAP_FROM_16(pixel.g, 8), VTFPP_REMAP_FROM_16(pixel.b, 8)});
		VTFPP_CASE_CONVERT_AND_BREAK(RGB888_BLUESCREEN, pixel.a < 0xffff ? ImagePixel::RGB888_BLUESCREEN{VTFPP_REMAP_FROM_16(pixel.r, 8), VTFPP_REMAP_FROM_16(pixel.g, 8), VTFPP_REMAP_FROM_16(pixel.b, 8)} : ImagePixel::RGB888_BLUESCREEN{0, 0, 0xff});
		VTFPP_CASE_CONVERT_AND_BREAK(BGR888,            {VTFPP_REMAP_FROM_16(pixel.b, 8), VTFPP_REMAP_FROM_16(pixel.g, 8), VTFPP_REMAP_FROM_16(pixel.r, 8)});
		VTFPP_CASE_CONVERT_AND_BREAK(BGR888_BLUESCREEN, pixel.a < 0xffff ? ImagePixel::BGR888_BLUESCREEN{VTFPP_REMAP_FROM_16(pixel.b, 8), VTFPP_REMAP_FROM_16(pixel.g, 8), VTFPP_REMAP_FROM_16(pixel.r, 8)} : ImagePixel::BGR888_BLUESCREEN{0xff, 0, 0});
		VTFPP_CASE_CONVERT_AND_BREAK(RGB565,            {VTFPP_REMAP_FROM_16(pixel.r, 5), VTFPP_REMAP_FROM_16(pixel.g, 6), VTFPP_REMAP_FROM_16(pixel.b, 5)});
		VTFPP_CASE_CONVERT_AND_BREAK(P8,                {VTFPP_REMAP_FROM_16(pixel.r, 8)});
		VTFPP_CASE_CONVERT_AND_BREAK(I8,                {VTFPP_REMAP_FROM_16(pixel.r, 8)});
		VTFPP_CASE_CONVERT_AND_BREAK(IA88,              {VTFPP_REMAP_FROM_16(pixel.r, 8), VTFPP_REMAP_FROM_16(pixel.a, 8)});
		VTFPP_CASE_CONVERT_AND_BREAK(A8,                {VTFPP_REMAP_FROM_16(pixel.a, 8)});
		VTFPP_CASE_CONVERT_AND_BREAK(ARGB8888,          {VTFPP_REMAP_FROM_16(pixel.a, 8), VTFPP_REMAP_FROM_16(pixel.r, 8), VTFPP_REMAP_FROM_16(pixel.g, 8), VTFPP_REMAP_FROM_16(pixel.b, 8)});
		VTFPP_CASE_CONVERT_AND_BREAK(BGRA8888,          {VTFPP_REMAP_FROM_16(pixel.b, 8), VTFPP_REMAP_FROM_16(pixel.g, 8), VTFPP_REMAP_FROM_16(pixel.r, 8), VTFPP_REMAP_FROM_16(pixel.a, 8)});
		VTFPP_CASE_CONVERT_AND_BREAK(BGRX8888,          {VTFPP_REMAP_FROM_16(pixel.b, 8), VTFPP_REMAP_FROM_16(pixel.g, 8), VTFPP_REMAP_FROM_16(pixel.r, 8), 0xff});
		VTFPP_CASE_CONVERT_AND_BREAK(BGR565,            {VTFPP_REMAP_FROM_16(pixel.b, 5), VTFPP_REMAP_FROM_16(pixel.g, 6), VTFPP_REMAP_FROM_16(pixel.r, 5)});
		VTFPP_CASE_CONVERT_AND_BREAK(BGRA5551,          {VTFPP_REMAP_FROM_16(pixel.b, 5), VTFPP_REMAP_FROM_16(pixel.g, 5), VTFPP_REMAP_FROM_16(pixel.r, 5), static_cast<uint8_t>(pixel.a < 0xffff ? 1 : 0)});
		VTFPP_CASE_CONVERT_AND_BREAK(BGRX5551,          {VTFPP_REMAP_FROM_16(pixel.b, 5), VTFPP_REMAP_FROM_16(pixel.g, 5), VTFPP_REMAP_FROM_16(pixel.r, 5), 0x1});
		VTFPP_CASE_CONVERT_AND_BREAK(BGRA4444,          {VTFPP_REMAP_FROM_16(pixel.b, 4), VTFPP_REMAP_FROM_16(pixel.g, 4), VTFPP_REMAP_FROM_16(pixel.r, 4), VTFPP_REMAP_FROM_16(pixel.a, 4)});
		VTFPP_CASE_CONVERT_AND_BREAK(UV88,              {VTFPP_REMAP_FROM_16(pixel.r, 8), VTFPP_REMAP_FROM_16(pixel.g, 8)});
		default: break;
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
	std::span<ImagePixel::RGBA32323232F> newDataSpan{reinterpret_cast<ImagePixel::RGBA32323232F*>(newData.data()), newData.size() / sizeof(ImagePixel::RGBA32323232F)};

	#define VTFPP_CONVERT_DETAIL(InputType, r, g, b, a, ...) \
		std::span<const ImagePixel::InputType> imageDataSpan{reinterpret_cast<const ImagePixel::InputType*>(imageData.data()), imageData.size() / sizeof(ImagePixel::InputType)}; \
		std::transform(__VA_ARGS__ __VA_OPT__(,) imageDataSpan.begin(), imageDataSpan.end(), newDataSpan.begin(), [](ImagePixel::InputType pixel) -> ImagePixel::RGBA32323232F { return {(r), (g), (b), (a)}; })
#ifdef SOURCEPP_BUILD_WITH_TBB
	#define VTFPP_CONVERT(InputType, r, g, b, a) VTFPP_CONVERT_DETAIL(InputType, r, g, b, a, std::execution::unseq)
#else
	#define VTFPP_CONVERT(InputType, r, g, b, a) VTFPP_CONVERT_DETAIL(InputType, r, g, b, a)
#endif
	#define VTFPP_CASE_CONVERT_AND_BREAK(InputType, r, g, b, a) \
		case InputType: { VTFPP_CONVERT(InputType, r, g, b, a); } break

	switch (format) {
		using enum ImageFormat;
		VTFPP_CASE_CONVERT_AND_BREAK(RGB323232F,    pixel.r,             pixel.g,             pixel.b,             1.f);
		VTFPP_CASE_CONVERT_AND_BREAK(R32F,          pixel.r,             pixel.r,             pixel.r,             1.f);
		VTFPP_CASE_CONVERT_AND_BREAK(RGBA16161616F, pixel.r.toFloat32(), pixel.g.toFloat32(), pixel.b.toFloat32(), pixel.a.toFloat32());
		default: break;
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

	std::span<const ImagePixel::RGBA32323232F> imageDataSpan{reinterpret_cast<const ImagePixel::RGBA32323232F*>(imageData.data()), imageData.size() / sizeof(ImagePixel::RGBA32323232F)};
	std::vector<std::byte> newData;
	newData.resize(imageData.size() / (ImageFormatDetails::bpp(ImageFormat::RGBA32323232F) / 8) * (ImageFormatDetails::bpp(format) / 8));

#ifdef SOURCEPP_BUILD_WITH_TBB
	#define VTFPP_CONVERT(InputType, ...) \
		std::span<ImagePixel::InputType> newDataSpan{reinterpret_cast<ImagePixel::InputType*>(newData.data()), newData.size() / sizeof(ImagePixel::InputType)}; \
		std::transform(std::execution::unseq, imageDataSpan.begin(), imageDataSpan.end(), newDataSpan.begin(), [](ImagePixel::RGBA32323232F pixel) -> ImagePixel::InputType { \
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
		VTFPP_CASE_CONVERT_AND_BREAK(RGB323232F,    {pixel.r, pixel.g, pixel.b});
		VTFPP_CASE_CONVERT_AND_BREAK(R32F,          {pixel.r});
		VTFPP_CASE_CONVERT_AND_BREAK(RGBA16161616F, {math::FloatCompressed16{pixel.r}, math::FloatCompressed16{pixel.g}, math::FloatCompressed16{pixel.b}, math::FloatCompressed16{pixel.a}});
		default: break;
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
	std::span<ImagePixel::RGBA32323232F> newDataSpan{reinterpret_cast<ImagePixel::RGBA32323232F*>(newData.data()), newData.size() / sizeof(ImagePixel::RGBA32323232F)};

	std::span<const ImagePixel::RGBA8888> imageDataSpan{reinterpret_cast<const ImagePixel::RGBA8888*>(imageData.data()), imageData.size() / sizeof(ImagePixel::RGBA8888)}; \
	std::transform(
#ifdef SOURCEPP_BUILD_WITH_TBB
			std::execution::unseq,
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
	std::span<ImagePixel::RGBA8888> newDataSpan{reinterpret_cast<ImagePixel::RGBA8888*>(newData.data()), newData.size() / sizeof(ImagePixel::RGBA8888)};

	std::span<const ImagePixel::RGBA32323232F> imageDataSpan{reinterpret_cast<const ImagePixel::RGBA32323232F*>(imageData.data()), imageData.size() / sizeof(ImagePixel::RGBA32323232F)}; \
	std::transform(
#ifdef SOURCEPP_BUILD_WITH_TBB
			std::execution::unseq,
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
	std::span<ImagePixel::RGBA16161616> newDataSpan{reinterpret_cast<ImagePixel::RGBA16161616*>(newData.data()), newData.size() / sizeof(ImagePixel::RGBA16161616)};

	std::span<const ImagePixel::RGBA8888> imageDataSpan{reinterpret_cast<const ImagePixel::RGBA8888*>(imageData.data()), imageData.size() / sizeof(ImagePixel::RGBA8888)}; \
	std::transform(
#ifdef SOURCEPP_BUILD_WITH_TBB
			std::execution::unseq,
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
	std::span<ImagePixel::RGBA8888> newDataSpan{reinterpret_cast<ImagePixel::RGBA8888*>(newData.data()), newData.size() / sizeof(ImagePixel::RGBA8888)};

	std::span<const ImagePixel::RGBA16161616> imageDataSpan{reinterpret_cast<const ImagePixel::RGBA16161616*>(imageData.data()), imageData.size() / sizeof(ImagePixel::RGBA16161616)}; \
	std::transform(
#ifdef SOURCEPP_BUILD_WITH_TBB
			std::execution::unseq,
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
	std::span<ImagePixel::RGBA16161616> newDataSpan{reinterpret_cast<ImagePixel::RGBA16161616*>(newData.data()), newData.size() / sizeof(ImagePixel::RGBA16161616)};

	std::span<const ImagePixel::RGBA32323232F> imageDataSpan{reinterpret_cast<const ImagePixel::RGBA32323232F*>(imageData.data()), imageData.size() / sizeof(ImagePixel::RGBA32323232F)}; \
	std::transform(
#ifdef SOURCEPP_BUILD_WITH_TBB
			std::execution::unseq,
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
	std::span<ImagePixel::RGBA32323232F> newDataSpan{reinterpret_cast<ImagePixel::RGBA32323232F*>(newData.data()), newData.size() / sizeof(ImagePixel::RGBA32323232F)};

	std::span<const ImagePixel::RGBA16161616> imageDataSpan{reinterpret_cast<const ImagePixel::RGBA16161616*>(imageData.data()), imageData.size() / sizeof(ImagePixel::RGBA16161616)}; \
	std::transform(
#ifdef SOURCEPP_BUILD_WITH_TBB
			std::execution::unseq,
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

} // namespace

std::vector<std::byte> ImageConversion::convertImageDataToFormat(std::span<const std::byte> imageData, ImageFormat oldFormat, ImageFormat newFormat, uint16_t width, uint16_t height) {
	if (imageData.empty() || oldFormat == ImageFormat::EMPTY) {
		return {};
	}

	if (oldFormat == newFormat) {
		return {imageData.begin(), imageData.end()};
	}

	std::vector<std::byte> newData;

	ImageFormat intermediaryOldFormat = ImageFormatDetails::containerFormat(oldFormat);
	if (ImageFormatDetails::compressed(oldFormat)) {
		newData = ::convertImageDataUsingCompressonator(imageData, oldFormat, intermediaryOldFormat, width, height);
	} else if (intermediaryOldFormat == ImageFormat::RGBA8888) {
		newData = ::convertImageDataToRGBA8888(imageData, oldFormat);
	} else if (intermediaryOldFormat == ImageFormat::RGBA16161616) {
		newData = ::convertImageDataToRGBA16161616(imageData, oldFormat);
	} else if (intermediaryOldFormat == ImageFormat::RGBA32323232F) {
		newData = ::convertImageDataToRGBA32323232F(imageData, oldFormat);
	} else {
		return {};
	}

	if (intermediaryOldFormat == newFormat) {
		return newData;
	}

	ImageFormat intermediaryNewFormat = ImageFormatDetails::containerFormat(newFormat);
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
	} else if (intermediaryNewFormat == ImageFormat::RGBA8888) {
		newData = ::convertImageDataFromRGBA8888(newData, newFormat);
	} else if (intermediaryNewFormat == ImageFormat::RGBA16161616) {
		newData = ::convertImageDataFromRGBA16161616(newData, newFormat);
	} else if (intermediaryNewFormat == ImageFormat::RGBA32323232F) {
		newData = ::convertImageDataFromRGBA32323232F(newData, newFormat);
	} else {
		return {};
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

std::vector<std::byte> ImageConversion::convertImageDataToFile(std::span<const std::byte> imageData, ImageFormat format, uint16_t width, uint16_t height, FileFormat fileFormat) {
	if (imageData.empty() || format == ImageFormat::EMPTY) {
		return {};
	}
	std::vector<std::byte> out;
	auto stbWriteFunc = [](void* out, void* data, int size) {
		std::copy(reinterpret_cast<std::byte*>(data), reinterpret_cast<std::byte*>(data) + size, std::back_inserter(*reinterpret_cast<std::vector<std::byte>*>(out)));
	};

	if (fileFormat == FileFormat::DEFAULT) {
		if (ImageFormatDetails::decimal(format)) {
			fileFormat = FileFormat::HDR;
		} else {
			fileFormat = FileFormat::PNG;
		}
	}
	switch (fileFormat) {
		case FileFormat::PNG: {
			if (format == ImageFormat::RGB888) {
				stbi_write_png_to_func(stbWriteFunc, &out, width, height, ImageFormatDetails::bpp(ImageFormat::RGB888) / 8, imageData.data(), 0);
			} else if (format == ImageFormat::RGBA8888) {
				stbi_write_png_to_func(stbWriteFunc, &out, width, height, ImageFormatDetails::bpp(ImageFormat::RGBA8888) / 8, imageData.data(), 0);
			} else if (ImageFormatDetails::large(format)) {
				auto rgba = convertImageDataToFormat(imageData, format, ImageFormat::RGBA16161616, width, height);
				stbi_write_png_to_func(stbWriteFunc, &out, width, height, ImageFormatDetails::bpp(ImageFormat::RGBA16161616) / (8 * sizeof(uint16_t)), rgba.data(), 0);
			} else {
				auto rgba = convertImageDataToFormat(imageData, format, ImageFormat::RGBA8888, width, height);
				stbi_write_png_to_func(stbWriteFunc, &out, width, height, ImageFormatDetails::bpp(ImageFormat::RGBA8888) / 8, rgba.data(), 0);
			}
			break;
		}
		case FileFormat::JPEG: {
			if (format == ImageFormat::RGB888) {
				stbi_write_jpg_to_func(stbWriteFunc, &out, width, height, ImageFormatDetails::bpp(ImageFormat::RGB888) / 8, imageData.data(), 95);
			} else {
				auto rgb = convertImageDataToFormat(imageData, format, ImageFormat::RGB888, width, height);
				stbi_write_jpg_to_func(stbWriteFunc, &out, width, height, ImageFormatDetails::bpp(ImageFormat::RGB888) / 8, rgb.data(), 95);
			}
			break;
		}
		case FileFormat::BMP: {
			if (format == ImageFormat::RGB888) {
				stbi_write_bmp_to_func(stbWriteFunc, &out, width, height, ImageFormatDetails::bpp(ImageFormat::RGB888) / 8, imageData.data());
			} else if (format == ImageFormat::RGBA8888) {
				stbi_write_bmp_to_func(stbWriteFunc, &out, width, height, ImageFormatDetails::bpp(ImageFormat::RGBA8888) / 8, imageData.data());
			} else {
				auto rgba = convertImageDataToFormat(imageData, format, ImageFormat::RGBA8888, width, height);
				stbi_write_bmp_to_func(stbWriteFunc, &out, width, height, ImageFormatDetails::bpp(ImageFormat::RGBA8888) / 8, rgba.data());
			}
			break;
		}
		case FileFormat::TGA: {
			if (format == ImageFormat::RGB888) {
				stbi_write_tga_to_func(stbWriteFunc, &out, width, height, ImageFormatDetails::bpp(ImageFormat::RGB888) / 8, imageData.data());
			} else if (format == ImageFormat::RGBA8888) {
				stbi_write_tga_to_func(stbWriteFunc, &out, width, height, ImageFormatDetails::bpp(ImageFormat::RGBA8888) / 8, imageData.data());
			} else if (ImageFormatDetails::large(format)) {
				auto rgba = convertImageDataToFormat(imageData, format, ImageFormat::RGBA16161616, width, height);
				stbi_write_tga_to_func(stbWriteFunc, &out, width, height, ImageFormatDetails::bpp(ImageFormat::RGBA16161616) / (8 * sizeof(uint16_t)), rgba.data());
			} else {
				auto rgba = convertImageDataToFormat(imageData, format, ImageFormat::RGBA8888, width, height);
				stbi_write_tga_to_func(stbWriteFunc, &out, width, height, ImageFormatDetails::bpp(ImageFormat::RGBA8888) / 8, rgba.data());
			}
			break;
		}
		case FileFormat::HDR: {
			if (format == ImageFormat::RGBA32323232F) {
				stbi_write_hdr_to_func(stbWriteFunc, &out, width, height, ImageFormatDetails::bpp(ImageFormat::RGBA32323232F) / (8 * sizeof(float)), reinterpret_cast<const float*>(imageData.data()));
			} else {
				auto hdr = convertImageDataToFormat(imageData, format, ImageFormat::RGBA32323232F, width, height);
				stbi_write_hdr_to_func(stbWriteFunc, &out, width, height, ImageFormatDetails::bpp(ImageFormat::RGBA32323232F) / (8 * sizeof(float)), reinterpret_cast<float*>(hdr.data()));
			}
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

	if (stbi_is_hdr_from_memory(reinterpret_cast<const stbi_uc*>(fileData.data()), static_cast<int>(fileData.size()))) {
		std::unique_ptr<float, void(*)(void*)> stbImage{
			stbi_loadf_from_memory(reinterpret_cast<const stbi_uc*>(fileData.data()), static_cast<int>(fileData.size()), &width, &height, &channels, 0),
			&stbi_image_free,
		};
		if (!stbImage) {
			return {};
		}

		switch (channels) {
			case 1:  format = ImageFormat::R32F;          break;
			case 3:  format = ImageFormat::RGB323232F;    break;
			case 4:  format = ImageFormat::RGBA32323232F; break;
			default: return {};
		}

		return {reinterpret_cast<std::byte*>(stbImage.get()), reinterpret_cast<std::byte*>(stbImage.get()) + ImageFormatDetails::getDataLength(format, width, height)};
	} else if (stbi_is_16_bit_from_memory(reinterpret_cast<const stbi_uc*>(fileData.data()), static_cast<int>(fileData.size()))) {
		std::unique_ptr<stbi_us, void(*)(void*)> stbImage{
			stbi_load_16_from_memory(reinterpret_cast<const stbi_uc*>(fileData.data()), static_cast<int>(fileData.size()), &width, &height, &channels, 0),
			&stbi_image_free,
		};
		if (!stbImage) {
			return {};
		}

		if (channels == 4) {
			format = ImageFormat::RGBA16161616;
		} else {
			return {};
		}

		return {reinterpret_cast<std::byte*>(stbImage.get()), reinterpret_cast<std::byte*>(stbImage.get()) + ImageFormatDetails::getDataLength(format, width, height)};
	} else if (fileData.size() >= 3 && static_cast<char>(fileData[0]) == 'G' && static_cast<char>(fileData[1]) == 'I' && static_cast<char>(fileData[2]) == 'F') {
		std::unique_ptr<stbi_uc, void(*)(void*)> stbImage{
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
	} else {
		std::unique_ptr<stbi_uc, void(*)(void*)> stbImage{
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
