#pragma once

#include <array>
#include <concepts>
#include <cstddef>
#include <span>
#include <vector>
#include <type_traits>

#include <BufferStream.h>
#include <sourcepp/Templates.h>
#include <sourcepp/Math.h>

using sourcepp::math::Arithmetic;

#include "ImageFormats.h"

namespace vtfpp {

template<Arithmetic A>
class LERep : public std::array<std::byte, sizeof(A)> {
	using uint_according = typename std::conditional<sizeof(A) == 8,
		uint64_t,
	typename std::conditional<sizeof(A) == 4,
		uint32_t,
	typename std::conditional<sizeof(A) == 2,
		uint16_t,
	uint8_t>::type>::type>::type;
public:
	constexpr operator A() {
		uint_according ret = 0;
		for (size_t offs = 0; auto &b : *this) {
			ret |= (static_cast<uint_according>(b) << offs) & (uint_according(0xFFu) << offs);
			offs += 8;
		}
		return *reinterpret_cast<A *>(&ret);
	}
	constexpr LERep &operator=(const A &v) {
		auto in = *reinterpret_cast<const uint_according *>(&v);
		for (size_t offs = 0; auto &b : *this) {
			b = static_cast<std::byte>((in >> offs) & uint_according(0xFFu));
			offs += 8;
		}
		return *this;
	}
	template<Arithmetic B> requires (std::convertible_to<B, A> || std::is_same_v<A, half>)
	constexpr LERep(const B &u) { *this = static_cast<A>(u); }
	template<Arithmetic B> requires std::convertible_to<B, A>
	constexpr LERep(const LERep<B> &u) { *this = static_cast<A>(u); }
	template<Arithmetic B> requires std::convertible_to<A, B>
	constexpr operator B() const { return static_cast<B>(static_cast<A>(*this)); }
};

namespace ImagePixel {

#define VTFPP_CHECK_SIZE(format) \
	static_assert(sizeof(format) == ImageFormatDetails::bpp(ImageFormat::format) / 8)

#define VTFPP_FORMAT_INHERITED(format, parent)              \
	struct format : parent {                                \
        static constexpr auto FORMAT = ImageFormat::format; \
	};                                                      \
	VTFPP_CHECK_SIZE(format)

struct RGBA8888 {
	static constexpr auto FORMAT = ImageFormat::RGBA8888;
	uint8_t r;
	uint8_t g;
	uint8_t b;
	uint8_t a;
}; VTFPP_CHECK_SIZE(RGBA8888);

struct ABGR8888 {
	static constexpr auto FORMAT = ImageFormat::ABGR8888;
	uint8_t a;
	uint8_t b;
	uint8_t g;
	uint8_t r;
}; VTFPP_CHECK_SIZE(ABGR8888);

struct RGB888 {
	static constexpr auto FORMAT = ImageFormat::RGB888;
	uint8_t r;
	uint8_t g;
	uint8_t b;
}; VTFPP_CHECK_SIZE(RGB888);

VTFPP_FORMAT_INHERITED(RGB888_BLUESCREEN, RGB888);

struct BGR888 {
	static constexpr auto FORMAT = ImageFormat::BGR888;
	uint8_t b;
	uint8_t g;
	uint8_t r;
}; VTFPP_CHECK_SIZE(BGR888);

VTFPP_FORMAT_INHERITED(BGR888_BLUESCREEN, BGR888);

struct RGB565 {
	static constexpr auto FORMAT = ImageFormat::RGB565;
	uint16_t r : 5;
	uint16_t g : 6;
	uint16_t b : 5;
}; VTFPP_CHECK_SIZE(RGB565);

struct I8 {
	static constexpr auto FORMAT = ImageFormat::I8;
	uint8_t i;
}; VTFPP_CHECK_SIZE(I8);

struct IA88 {
	static constexpr auto FORMAT = ImageFormat::IA88;
	uint8_t i;
	uint8_t a;
}; VTFPP_CHECK_SIZE(IA88);

struct P8 {
	static constexpr auto FORMAT = ImageFormat::P8;
	uint8_t p;
}; VTFPP_CHECK_SIZE(P8);

struct A8 {
	static constexpr auto FORMAT = ImageFormat::A8;
	uint8_t a;
}; VTFPP_CHECK_SIZE(A8);

struct ARGB8888 {
	static constexpr auto FORMAT = ImageFormat::ARGB8888;
	uint8_t a;
	uint8_t r;
	uint8_t g;
	uint8_t b;
}; VTFPP_CHECK_SIZE(ARGB8888);

struct BGRA8888 {
	static constexpr auto FORMAT = ImageFormat::BGRA8888;
	uint8_t b;
	uint8_t g;
	uint8_t r;
	uint8_t a;
}; VTFPP_CHECK_SIZE(BGRA8888);

struct BGRX8888 {
	static constexpr auto FORMAT = ImageFormat::BGRX8888;
	uint8_t b;
	uint8_t g;
	uint8_t r;
	uint8_t x;
}; VTFPP_CHECK_SIZE(BGRX8888);

struct BGR565 {
	static constexpr auto FORMAT = ImageFormat::BGR565;
	uint16_t b : 5;
	uint16_t g : 6;
	uint16_t r : 5;
}; VTFPP_CHECK_SIZE(BGR565);

struct BGRX5551 {
	static constexpr auto FORMAT = ImageFormat::BGRX5551;
	uint16_t b : 5;
	uint16_t g : 5;
	uint16_t r : 5;
	uint16_t x : 1;
}; VTFPP_CHECK_SIZE(BGRX5551);

struct BGRA4444 {
	static constexpr auto FORMAT = ImageFormat::BGRA4444;
	uint16_t b : 4;
	uint16_t g : 4;
	uint16_t r : 4;
	uint16_t a : 4;
}; VTFPP_CHECK_SIZE(BGRA4444);

struct BGRA5551 {
	static constexpr auto FORMAT = ImageFormat::BGRA5551;
	uint16_t b : 5;
	uint16_t g : 5;
	uint16_t r : 5;
	uint16_t a : 1;
}; VTFPP_CHECK_SIZE(BGRA5551);

struct UV88 {
	static constexpr auto FORMAT = ImageFormat::UV88;
	uint8_t u;
	uint8_t v;
}; VTFPP_CHECK_SIZE(UV88);

struct UVWQ8888 {
	static constexpr auto FORMAT = ImageFormat::UVWQ8888;
	uint8_t u;
	uint8_t v;
	uint8_t w;
	uint8_t q;
}; VTFPP_CHECK_SIZE(UVWQ8888);

struct RGBA16161616F {
	static constexpr auto FORMAT = ImageFormat::RGBA16161616F;
	LERep<half> r;
	LERep<half> g;
	LERep<half> b;
	LERep<half> a;
}; VTFPP_CHECK_SIZE(RGBA16161616F);

struct RGBA16161616 {
	static constexpr auto FORMAT = ImageFormat::RGBA16161616;
	LERep<uint16_t> r;
	LERep<uint16_t> g;
	LERep<uint16_t> b;
	LERep<uint16_t> a;
}; VTFPP_CHECK_SIZE(RGBA16161616);

struct UVLX8888 {
	static constexpr auto FORMAT = ImageFormat::UVLX8888;
	uint8_t u;
	uint8_t v;
	uint8_t l;
	uint8_t x;
}; VTFPP_CHECK_SIZE(UVLX8888);

struct R32F {
	static constexpr auto FORMAT = ImageFormat::R32F;
	LERep<float> r;
}; VTFPP_CHECK_SIZE(R32F);

struct RGB323232F {
	static constexpr auto FORMAT = ImageFormat::R32F;
	LERep<float> r;
	LERep<float> g;
	LERep<float> b;
}; VTFPP_CHECK_SIZE(RGB323232F);

struct RGBA32323232F {
	static constexpr auto FORMAT = ImageFormat::RGBA32323232F;
	LERep<float> r;
	LERep<float> g;
	LERep<float> b;
	LERep<float> a;
}; VTFPP_CHECK_SIZE(RGBA32323232F);

struct RG1616F {
	static constexpr auto FORMAT = ImageFormat::RG1616F;
	LERep<half> r;
	LERep<half> g;
}; VTFPP_CHECK_SIZE(RG1616F);

struct RG3232F {
	static constexpr auto FORMAT = ImageFormat::RG3232F;
	LERep<float> r;
	LERep<float> g;
}; VTFPP_CHECK_SIZE(RG3232F);

struct RGBX8888 {
	static constexpr auto FORMAT = ImageFormat::RGBX8888;
	uint8_t r;
	uint8_t g;
	uint8_t b;
	uint8_t x;
}; VTFPP_CHECK_SIZE(RGBX8888);

struct RGBA1010102 {
	static constexpr auto FORMAT = ImageFormat::RGBA1010102;
	uint32_t r : 10;
	uint32_t g : 10;
	uint32_t b : 10;
	uint32_t a : 2;
}; VTFPP_CHECK_SIZE(RGBA1010102);

struct BGRA1010102 {
	static constexpr auto FORMAT = ImageFormat::BGRA1010102;
	uint32_t b : 10;
	uint32_t g : 10;
	uint32_t r : 10;
	uint32_t a : 2;
}; VTFPP_CHECK_SIZE(BGRA1010102);

struct R16F {
	static constexpr auto FORMAT = ImageFormat::R16F;
	half r;
}; VTFPP_CHECK_SIZE(R16F);

struct R8 {
	static constexpr auto FORMAT = ImageFormat::R8;
	uint8_t r;
}; VTFPP_CHECK_SIZE(R8);

VTFPP_FORMAT_INHERITED(CONSOLE_BGRX8888_LINEAR, BGRX8888);

VTFPP_FORMAT_INHERITED(CONSOLE_RGBA8888_LINEAR, RGBA8888);

VTFPP_FORMAT_INHERITED(CONSOLE_ABGR8888_LINEAR, ABGR8888);

VTFPP_FORMAT_INHERITED(CONSOLE_ARGB8888_LINEAR, ARGB8888);

VTFPP_FORMAT_INHERITED(CONSOLE_BGRA8888_LINEAR, BGRA8888);

VTFPP_FORMAT_INHERITED(CONSOLE_RGB888_LINEAR, RGB888);

VTFPP_FORMAT_INHERITED(CONSOLE_BGR888_LINEAR, BGR888);

VTFPP_FORMAT_INHERITED(CONSOLE_BGRX5551_LINEAR, BGRX5551);

VTFPP_FORMAT_INHERITED(CONSOLE_I8_LINEAR, I8);

VTFPP_FORMAT_INHERITED(CONSOLE_RGBA16161616_LINEAR, RGBA16161616);

VTFPP_FORMAT_INHERITED(CONSOLE_BGRX8888_LE, BGRX8888);

VTFPP_FORMAT_INHERITED(CONSOLE_BGRA8888_LE, BGRA8888);

#undef VTFPP_FORMAT_INHERITED
#undef VTFPP_CHECK_SIZE

template<typename T>
concept PixelType =
		std::same_as<T, RGBA8888> ||
		std::same_as<T, ABGR8888> ||
		std::same_as<T, RGB888> ||
		std::same_as<T, BGR888> ||
		std::same_as<T, RGB565> ||
		std::same_as<T, I8> ||
		std::same_as<T, IA88> ||
		std::same_as<T, P8> ||
		std::same_as<T, A8> ||
		std::same_as<T, RGB888_BLUESCREEN> ||
		std::same_as<T, BGR888_BLUESCREEN> ||
		std::same_as<T, ARGB8888> ||
		std::same_as<T, BGRA8888> ||
		std::same_as<T, BGRX8888> ||
		std::same_as<T, BGR565> ||
		std::same_as<T, BGRX5551> ||
		std::same_as<T, BGRA4444> ||
		std::same_as<T, BGRA5551> ||
		std::same_as<T, UV88> ||
		std::same_as<T, UVWQ8888> ||
		std::same_as<T, RGBA16161616F> ||
		std::same_as<T, RGBA16161616> ||
		std::same_as<T, UVLX8888> ||
		std::same_as<T, R32F> ||
		std::same_as<T, RGB323232F> ||
		std::same_as<T, RGBA32323232F> ||
		std::same_as<T, RG1616F> ||
		std::same_as<T, RG3232F> ||
		std::same_as<T, RGBX8888> ||
		std::same_as<T, RGBA1010102> ||
		std::same_as<T, BGRA1010102> ||
		std::same_as<T, R16F> ||
		std::same_as<T, CONSOLE_BGRX8888_LINEAR> ||
		std::same_as<T, CONSOLE_RGBA8888_LINEAR> ||
		std::same_as<T, CONSOLE_ABGR8888_LINEAR> ||
		std::same_as<T, CONSOLE_ARGB8888_LINEAR> ||
		std::same_as<T, CONSOLE_BGRA8888_LINEAR> ||
		std::same_as<T, CONSOLE_RGB888_LINEAR> ||
		std::same_as<T, CONSOLE_BGR888_LINEAR> ||
		std::same_as<T, CONSOLE_BGRX5551_LINEAR> ||
		std::same_as<T, CONSOLE_I8_LINEAR> ||
		std::same_as<T, CONSOLE_RGBA16161616_LINEAR> ||
		std::same_as<T, CONSOLE_BGRX8888_LE> ||
		std::same_as<T, CONSOLE_BGRA8888_LE> ||
		std::same_as<T, R8>;

} // namespace ImagePixel

namespace ImageConversion {

constexpr float DEFAULT_COMPRESSED_QUALITY = 0.105f;

/// Converts an image from one format to another.
[[nodiscard]] std::vector<std::byte> convertImageDataToFormat(std::span<const std::byte> imageData, ImageFormat oldFormat, ImageFormat newFormat, uint16_t width, uint16_t height, float quality = DEFAULT_COMPRESSED_QUALITY);

/// Converts several images from one format to another.
[[nodiscard]] std::vector<std::byte> convertSeveralImageDataToFormat(std::span<const std::byte> imageData, ImageFormat oldFormat, ImageFormat newFormat, uint8_t mipCount, uint16_t frameCount, uint8_t faceCount, uint16_t width, uint16_t height, uint16_t sliceCount, float quality = DEFAULT_COMPRESSED_QUALITY);

/// Converts an HDRI into a cubemap. The output image data is in the same image format as the input.
/// The output images have the following order: front, back, left, right, down, up.
/// Resolution is the output size (width, height) of each image slice. 0 leaves it at the input size, the height of the HDRI.
/// Fails (returns empty vectors) if the input data is empty, the given width is not 2x the height, or an error was encountered.
[[nodiscard]] std::array<std::vector<std::byte>, 6> convertHDRIToCubeMap(std::span<const std::byte> imageData, ImageFormat format, uint16_t width, uint16_t height, uint16_t resolution = 0, bool bilinear = true);

enum class FileFormat {
	DEFAULT,
	PNG,
	JPG,
	BMP,
	TGA,
	WEBP,
	QOI,
	HDR,
	EXR,
};

/// PNG for integer formats, EXR for floating point formats
[[nodiscard]] FileFormat getDefaultFileFormatForImageFormat(ImageFormat format);

/// Converts image data to the given file format (PNG or EXR by default).
[[nodiscard]] std::vector<std::byte> convertImageDataToFile(std::span<const std::byte> imageData, ImageFormat format, uint16_t width, uint16_t height, FileFormat fileFormat = FileFormat::DEFAULT);

[[nodiscard]] std::vector<std::byte> convertFileToImageData(std::span<const std::byte> fileData, ImageFormat& format, int& width, int& height, int& frameCount);

enum class ResizeEdge {
	// Matches stbir_edge
	CLAMP = 0,
	REFLECT,
	WRAP,
	ZERO,
};

enum class ResizeFilter {
	// Matches stbir_filter
	DEFAULT = 0,
	BOX,
	BILINEAR,
	CUBIC_BSPLINE,
	CATMULL_ROM,
	MITCHELL,
	POINT_SAMPLE,

	// User-defined
	KAISER = 100,
	/// Valve NICE filtering, equivalent to Lanczos-3
	NICE = 101,
};

enum class ResizeMethod {
	NONE,
	POWER_OF_TWO_BIGGER,
	POWER_OF_TWO_SMALLER,
	POWER_OF_TWO_NEAREST,
};

/// Get the new image size given a resize method
[[nodiscard]] uint16_t getResizedDim(uint16_t n, ResizeMethod method);

/// Set the new image dimensions given a resize method
void setResizedDims(uint16_t& width, ResizeMethod widthResize, uint16_t& height, ResizeMethod heightResize);

/// Resize given image data to the new dimensions
[[nodiscard]] std::vector<std::byte> resizeImageData(std::span<const std::byte> imageData, ImageFormat format, uint16_t width, uint16_t newWidth, uint16_t height, uint16_t newHeight, bool srgb, ResizeFilter filter, ResizeEdge edge = ResizeEdge::CLAMP);

/// Resize given image data to the new dimensions, where the new width and height are governed by the resize methods
[[nodiscard]] std::vector<std::byte> resizeImageDataStrict(std::span<const std::byte> imageData, ImageFormat format, uint16_t width, uint16_t newWidth, uint16_t& widthOut, ResizeMethod widthResize, uint16_t height, uint16_t newHeight, uint16_t& heightOut, ResizeMethod heightResize, bool srgb, ResizeFilter filter, ResizeEdge edge = ResizeEdge::CLAMP);

/// Crops the given image to the new dimensions. If the image format is compressed it will be converted to its container format before the crop, and converted back before returning
[[nodiscard]] std::vector<std::byte> cropImageData(std::span<const std::byte> imageData, ImageFormat format, uint16_t width, uint16_t newWidth, uint16_t xOffset, uint16_t height, uint16_t newHeight, uint16_t yOffset);

/// Perform gamma correction on the given image data. Will not perform gamma correction if the input image format is large, console, P8, A8, UV88, UVLX8888, or UVWQ8888
[[nodiscard]] std::vector<std::byte> gammaCorrectImageData(std::span<const std::byte> imageData, ImageFormat format, uint16_t width, uint16_t height, float gamma);

/// Invert the green channel. Meant for converting normal maps between OpenGL and DirectX formats
[[nodiscard]] std::vector<std::byte> invertGreenChannelForImageData(std::span<const std::byte> imageData, ImageFormat format, uint16_t width, uint16_t height);

/// Extracts a single channel from the given image data.
/// May have unexpected behavior if called on formats that use bitfields like BGRA5551!
/// Data is packed according to pixel channel C++ type size
/// (e.g. in the case of BGRA5551's green channel, it'll be 2 bytes per green value despite only 5 bits being used in the original data)
template<ImagePixel::PixelType P>
[[nodiscard]] std::vector<std::byte> extractChannelFromImageData(std::span<const std::byte> imageData, auto P::*channel) {
	using C = sourcepp::member_type_t<decltype(channel)>;
	if (imageData.empty() || imageData.size() % sizeof(P) != 0) {
		return {};
	}

	std::span pixels{reinterpret_cast<const P*>(imageData.data()), imageData.size() / sizeof(P)};

	std::vector<std::byte> out(imageData.size() / sizeof(P) * sizeof(C));
	BufferStream stream{out, false};
	for (const auto& pixel : pixels) {
		stream << pixel.*channel;
	}
	return out;
}

} // namespace ImageConversion

} // namespace vtfpp
