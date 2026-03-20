#pragma once

#include <bit>
#include <concepts>
#include <cstring>

#include <BufferStream.h>

#include "ImageFormats.h"

namespace vtfpp::ImagePixel {

namespace detail {

#define VTFPP_FORMAT_CHECK(format, bo) \
	static_assert(std::is_trivially_copyable_v<format##_##bo> && sizeof(format##_##bo) == ImageFormatDetails::bpp(ImageFormat::format) / 8)

#define VTFPP_FORMAT(format, bo, ...)                       \
	struct format##_##bo {                                  \
		static constexpr auto FORMAT = ImageFormat::format; \
		__VA_ARGS__;                                        \
	}; VTFPP_FORMAT_CHECK(format, bo)

#define VTFPP_FORMAT_INHERITED(format, parent)              \
	struct format##_LE : parent##_LE {                      \
		static constexpr auto FORMAT = ImageFormat::format; \
	}; VTFPP_FORMAT_CHECK(format, LE);                      \
	struct format##_BE : parent##_BE {                      \
		static constexpr auto FORMAT = ImageFormat::format; \
	}; VTFPP_FORMAT_CHECK(format, BE)

VTFPP_FORMAT(RGBA8888, LE, uint8_t r, g, b, a);
VTFPP_FORMAT(RGBA8888, BE, uint8_t a, b, g, r);

VTFPP_FORMAT(ABGR8888, LE, uint8_t a, b, g, r);
VTFPP_FORMAT(ABGR8888, BE, uint8_t r, g, b, a);

VTFPP_FORMAT(RGB888, LE, uint8_t r, g, b);
VTFPP_FORMAT(RGB888, BE, uint8_t b, g, r);

VTFPP_FORMAT_INHERITED(RGB888_BLUESCREEN, RGB888);

VTFPP_FORMAT(BGR888, LE, uint8_t b, g, r);
VTFPP_FORMAT(BGR888, BE, uint8_t r, g, b);

VTFPP_FORMAT_INHERITED(BGR888_BLUESCREEN, BGR888);

VTFPP_FORMAT(RGB565, LE, uint16_t r : 5, g : 6, b : 5);
VTFPP_FORMAT(RGB565, BE, uint16_t data);

VTFPP_FORMAT(I8, LE, uint8_t i);
VTFPP_FORMAT(I8, BE, uint8_t i);

VTFPP_FORMAT(IA88, LE, uint8_t i, a);
VTFPP_FORMAT(IA88, BE, uint8_t a, i);

VTFPP_FORMAT(P8, LE, uint8_t p);
VTFPP_FORMAT(P8, BE, uint8_t p);

VTFPP_FORMAT(A8, LE, uint8_t a);
VTFPP_FORMAT(A8, BE, uint8_t a);

VTFPP_FORMAT(ARGB8888, LE, uint8_t a, r, g, b);
VTFPP_FORMAT(ARGB8888, BE, uint8_t b, g, r, a);

VTFPP_FORMAT(BGRA8888, LE, uint8_t b, g, r, a);
VTFPP_FORMAT(BGRA8888, BE, uint8_t a, r, g, b);

VTFPP_FORMAT(BGRX8888, LE, uint8_t b, g, r, x);
VTFPP_FORMAT(BGRX8888, BE, uint8_t x, r, g, b);

VTFPP_FORMAT(BGR565, LE, uint16_t b : 5, g : 6, r : 5);
VTFPP_FORMAT(BGR565, BE, uint16_t data);

VTFPP_FORMAT(BGRX5551, LE, uint16_t b : 5, g : 5, r : 5, x : 1);
VTFPP_FORMAT(BGRX5551, BE, uint16_t data);

VTFPP_FORMAT(BGRA4444, LE, uint16_t b : 4, g : 4, r : 4, a : 4);
VTFPP_FORMAT(BGRA4444, BE, uint16_t data);

VTFPP_FORMAT(BGRA5551, LE, uint16_t b : 5, g : 5, r : 5, a : 1);
VTFPP_FORMAT(BGRA5551, BE, uint16_t data);

VTFPP_FORMAT(UV88, LE, uint8_t u, v);
VTFPP_FORMAT(UV88, BE, uint8_t v, u);

VTFPP_FORMAT(UVWQ8888, LE, uint8_t u, v, w, q);
VTFPP_FORMAT(UVWQ8888, BE, uint8_t q, w, v, u);

VTFPP_FORMAT(RGBA16161616F, LE, half r, g, b, a);
VTFPP_FORMAT(RGBA16161616F, BE, half a, b, g, r);

VTFPP_FORMAT(RGBA16161616, LE, uint16_t r, g, b, a);
VTFPP_FORMAT(RGBA16161616, BE, uint16_t a, b, g, r);

VTFPP_FORMAT(UVLX8888, LE, uint8_t u, v, l, x);
VTFPP_FORMAT(UVLX8888, BE, uint8_t x, l, v, u);

VTFPP_FORMAT(R32F, LE, float r);
VTFPP_FORMAT(R32F, BE, float r);

VTFPP_FORMAT(RGB323232F, LE, float r, g, b);
VTFPP_FORMAT(RGB323232F, BE, float b, g, r);

VTFPP_FORMAT(RGBA32323232F, LE, float r, g, b, a);
VTFPP_FORMAT(RGBA32323232F, BE, float a, b, g, r);

VTFPP_FORMAT(RG1616F, LE, half r, g);
VTFPP_FORMAT(RG1616F, BE, half g, r);

VTFPP_FORMAT(RG3232F, LE, float r, g);
VTFPP_FORMAT(RG3232F, BE, float g, r);

VTFPP_FORMAT(RGBX8888, LE, uint8_t r, g, b, x);
VTFPP_FORMAT(RGBX8888, BE, uint8_t x, b, g, r);

VTFPP_FORMAT(RGBA1010102, LE, uint32_t r : 10, g : 10, b : 10, a : 2);
VTFPP_FORMAT(RGBA1010102, BE, uint32_t data);

VTFPP_FORMAT(BGRA1010102, LE, uint32_t b : 10, g : 10, r : 10, a : 2);
VTFPP_FORMAT(BGRA1010102, BE, uint32_t data);

VTFPP_FORMAT(R16F, LE, half r);
VTFPP_FORMAT(R16F, BE, half r);

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

VTFPP_FORMAT(STRATA_R8, LE, uint8_t r);
VTFPP_FORMAT(STRATA_R8, BE, uint8_t r);

#undef VTFPP_FORMAT_INHERITED
#undef VTFPP_FORMAT
#undef VTFPP_FORMAT_CHECK

} // namespace detail

#define VTFPP_FORMAT_ACCESSOR(channel)                                 \
	[[nodiscard]] constexpr auto channel() const {                     \
		if constexpr (std::endian::native == std::endian::little) {    \
			return pixel.le.channel;                                   \
		} else {                                                       \
			return pixel.be.channel;                                   \
		}                                                              \
	}                                                                  \
	constexpr void set##_##channel(decltype(pixel.le.channel) value) { \
		if constexpr (std::endian::native == std::endian::little) {    \
			pixel.le.channel = value;                                  \
		} else {                                                       \
			pixel.be.channel = value;                                  \
		}                                                              \
	}

#define VTFPP_FORMAT_ACCESSOR_BSWAP(format, channel)                          \
	[[nodiscard]] constexpr auto channel() const {                            \
		if constexpr (std::endian::native == std::endian::little) {           \
			return pixel.le.channel;                                          \
		} else {                                                              \
			auto swapped = pixel.be.data;                                     \
			BufferStream::swap_endian(&swapped);                              \
			return std::bit_cast<detail::format##_LE>(swapped).channel;       \
		}                                                                     \
	}                                                                         \
	constexpr void set##_##channel(decltype(pixel.le.channel) value) {        \
		if constexpr (std::endian::native == std::endian::little) {           \
			pixel.le.channel = value;                                         \
		} else {                                                              \
			auto pixelLE = std::bit_cast<detail::format##_LE>(pixel.be.data); \
			pixelLE.channel = value;                                          \
			auto swapped = std::bit_cast<decltype(pixel.be.data)>(pixelLE);   \
			BufferStream::swap_endian(&swapped);                              \
			pixel.be.data = swapped;                                          \
		}                                                                     \
	}

#define VTFPP_FORMAT(format, accessors)                                  \
	class format {                                                       \
	protected:                                                           \
		union Pixel {                                                    \
			detail::format##_LE le;                                      \
			detail::format##_BE be;                                      \
			Pixel() { std::memset(this, 0, sizeof(Pixel)); }             \
		} pixel;                                                         \
	public:                                                              \
		static constexpr auto FORMAT = ImageFormat::format;              \
		format(detail::format##_LE le) {                                 \
			if constexpr (std::endian::native == std::endian::little) {  \
				this->pixel.le = le;                                     \
			} else {                                                     \
				BufferStream::swap_endian(&le);                          \
				this->pixel.be = std::bit_cast<detail::format##_BE>(le); \
			}                                                            \
		}                                                                \
		accessors                                                        \
}; static_assert(std::is_trivially_copyable_v<format> && sizeof(format) == ImageFormatDetails::bpp(ImageFormat::format) / 8)

VTFPP_FORMAT(RGBA8888,                    VTFPP_FORMAT_ACCESSOR(r) VTFPP_FORMAT_ACCESSOR(g) VTFPP_FORMAT_ACCESSOR(b) VTFPP_FORMAT_ACCESSOR(a));
VTFPP_FORMAT(ABGR8888,                    VTFPP_FORMAT_ACCESSOR(r) VTFPP_FORMAT_ACCESSOR(g) VTFPP_FORMAT_ACCESSOR(b) VTFPP_FORMAT_ACCESSOR(a));
VTFPP_FORMAT(RGB888,                      VTFPP_FORMAT_ACCESSOR(r) VTFPP_FORMAT_ACCESSOR(g) VTFPP_FORMAT_ACCESSOR(b));
VTFPP_FORMAT(BGR888,                      VTFPP_FORMAT_ACCESSOR(r) VTFPP_FORMAT_ACCESSOR(g) VTFPP_FORMAT_ACCESSOR(b));
VTFPP_FORMAT(RGB565,                      VTFPP_FORMAT_ACCESSOR_BSWAP(RGB565, r) VTFPP_FORMAT_ACCESSOR_BSWAP(RGB565, g) VTFPP_FORMAT_ACCESSOR_BSWAP(RGB565, b));
VTFPP_FORMAT(I8,                          VTFPP_FORMAT_ACCESSOR(i));
VTFPP_FORMAT(IA88,                        VTFPP_FORMAT_ACCESSOR(i) VTFPP_FORMAT_ACCESSOR(a));
VTFPP_FORMAT(P8,                          VTFPP_FORMAT_ACCESSOR(p));
VTFPP_FORMAT(A8,                          VTFPP_FORMAT_ACCESSOR(a));
VTFPP_FORMAT(RGB888_BLUESCREEN,           VTFPP_FORMAT_ACCESSOR(r) VTFPP_FORMAT_ACCESSOR(g) VTFPP_FORMAT_ACCESSOR(b));
VTFPP_FORMAT(BGR888_BLUESCREEN,           VTFPP_FORMAT_ACCESSOR(r) VTFPP_FORMAT_ACCESSOR(g) VTFPP_FORMAT_ACCESSOR(b));
VTFPP_FORMAT(ARGB8888,                    VTFPP_FORMAT_ACCESSOR(r) VTFPP_FORMAT_ACCESSOR(g) VTFPP_FORMAT_ACCESSOR(b) VTFPP_FORMAT_ACCESSOR(a));
VTFPP_FORMAT(BGRA8888,                    VTFPP_FORMAT_ACCESSOR(r) VTFPP_FORMAT_ACCESSOR(g) VTFPP_FORMAT_ACCESSOR(b) VTFPP_FORMAT_ACCESSOR(a));
VTFPP_FORMAT(BGRX8888,                    VTFPP_FORMAT_ACCESSOR(r) VTFPP_FORMAT_ACCESSOR(g) VTFPP_FORMAT_ACCESSOR(b) VTFPP_FORMAT_ACCESSOR(x));
VTFPP_FORMAT(BGR565,                      VTFPP_FORMAT_ACCESSOR_BSWAP(BGR565, r) VTFPP_FORMAT_ACCESSOR_BSWAP(BGR565, g) VTFPP_FORMAT_ACCESSOR_BSWAP(BGR565, b));
VTFPP_FORMAT(BGRX5551,                    VTFPP_FORMAT_ACCESSOR_BSWAP(BGRX5551, r) VTFPP_FORMAT_ACCESSOR_BSWAP(BGRX5551, g) VTFPP_FORMAT_ACCESSOR_BSWAP(BGRX5551, b) VTFPP_FORMAT_ACCESSOR_BSWAP(BGRX5551, x));
VTFPP_FORMAT(BGRA4444,                    VTFPP_FORMAT_ACCESSOR_BSWAP(BGRA4444, r) VTFPP_FORMAT_ACCESSOR_BSWAP(BGRA4444, g) VTFPP_FORMAT_ACCESSOR_BSWAP(BGRA4444, b) VTFPP_FORMAT_ACCESSOR_BSWAP(BGRA4444, a));
VTFPP_FORMAT(BGRA5551,                    VTFPP_FORMAT_ACCESSOR_BSWAP(BGRA5551, r) VTFPP_FORMAT_ACCESSOR_BSWAP(BGRA5551, g) VTFPP_FORMAT_ACCESSOR_BSWAP(BGRA5551, b) VTFPP_FORMAT_ACCESSOR_BSWAP(BGRA5551, a));
VTFPP_FORMAT(UV88,                        VTFPP_FORMAT_ACCESSOR(u) VTFPP_FORMAT_ACCESSOR(v));
VTFPP_FORMAT(UVWQ8888,                    VTFPP_FORMAT_ACCESSOR(u) VTFPP_FORMAT_ACCESSOR(v) VTFPP_FORMAT_ACCESSOR(w) VTFPP_FORMAT_ACCESSOR(q));
VTFPP_FORMAT(RGBA16161616F,               VTFPP_FORMAT_ACCESSOR(r) VTFPP_FORMAT_ACCESSOR(g) VTFPP_FORMAT_ACCESSOR(b) VTFPP_FORMAT_ACCESSOR(a));
VTFPP_FORMAT(RGBA16161616,                VTFPP_FORMAT_ACCESSOR(r) VTFPP_FORMAT_ACCESSOR(g) VTFPP_FORMAT_ACCESSOR(b) VTFPP_FORMAT_ACCESSOR(a));
VTFPP_FORMAT(UVLX8888,                    VTFPP_FORMAT_ACCESSOR(u) VTFPP_FORMAT_ACCESSOR(v) VTFPP_FORMAT_ACCESSOR(l) VTFPP_FORMAT_ACCESSOR(x));
VTFPP_FORMAT(R32F,                        VTFPP_FORMAT_ACCESSOR(r));
VTFPP_FORMAT(RGB323232F,                  VTFPP_FORMAT_ACCESSOR(r) VTFPP_FORMAT_ACCESSOR(g) VTFPP_FORMAT_ACCESSOR(b));
VTFPP_FORMAT(RGBA32323232F,               VTFPP_FORMAT_ACCESSOR(r) VTFPP_FORMAT_ACCESSOR(g) VTFPP_FORMAT_ACCESSOR(b) VTFPP_FORMAT_ACCESSOR(a));
VTFPP_FORMAT(RG1616F,                     VTFPP_FORMAT_ACCESSOR(r) VTFPP_FORMAT_ACCESSOR(g));
VTFPP_FORMAT(RG3232F,                     VTFPP_FORMAT_ACCESSOR(r) VTFPP_FORMAT_ACCESSOR(g));
VTFPP_FORMAT(RGBX8888,                    VTFPP_FORMAT_ACCESSOR(r) VTFPP_FORMAT_ACCESSOR(g) VTFPP_FORMAT_ACCESSOR(b) VTFPP_FORMAT_ACCESSOR(x));
VTFPP_FORMAT(RGBA1010102,                 VTFPP_FORMAT_ACCESSOR_BSWAP(RGBA1010102, r) VTFPP_FORMAT_ACCESSOR_BSWAP(RGBA1010102, g) VTFPP_FORMAT_ACCESSOR_BSWAP(RGBA1010102, b) VTFPP_FORMAT_ACCESSOR_BSWAP(RGBA1010102, a));
VTFPP_FORMAT(BGRA1010102,                 VTFPP_FORMAT_ACCESSOR_BSWAP(RGBA1010102, r) VTFPP_FORMAT_ACCESSOR_BSWAP(RGBA1010102, g) VTFPP_FORMAT_ACCESSOR_BSWAP(RGBA1010102, b) VTFPP_FORMAT_ACCESSOR_BSWAP(RGBA1010102, a));
VTFPP_FORMAT(R16F,                        VTFPP_FORMAT_ACCESSOR(r));
VTFPP_FORMAT(CONSOLE_BGRX8888_LINEAR,     VTFPP_FORMAT_ACCESSOR(r) VTFPP_FORMAT_ACCESSOR(g) VTFPP_FORMAT_ACCESSOR(b) VTFPP_FORMAT_ACCESSOR(x));
VTFPP_FORMAT(CONSOLE_RGBA8888_LINEAR,     VTFPP_FORMAT_ACCESSOR(r) VTFPP_FORMAT_ACCESSOR(g) VTFPP_FORMAT_ACCESSOR(b) VTFPP_FORMAT_ACCESSOR(a));
VTFPP_FORMAT(CONSOLE_ABGR8888_LINEAR,     VTFPP_FORMAT_ACCESSOR(r) VTFPP_FORMAT_ACCESSOR(g) VTFPP_FORMAT_ACCESSOR(b) VTFPP_FORMAT_ACCESSOR(a));
VTFPP_FORMAT(CONSOLE_ARGB8888_LINEAR,     VTFPP_FORMAT_ACCESSOR(r) VTFPP_FORMAT_ACCESSOR(g) VTFPP_FORMAT_ACCESSOR(b) VTFPP_FORMAT_ACCESSOR(a));
VTFPP_FORMAT(CONSOLE_BGRA8888_LINEAR,     VTFPP_FORMAT_ACCESSOR(r) VTFPP_FORMAT_ACCESSOR(g) VTFPP_FORMAT_ACCESSOR(b) VTFPP_FORMAT_ACCESSOR(a));
VTFPP_FORMAT(CONSOLE_RGB888_LINEAR,       VTFPP_FORMAT_ACCESSOR(r) VTFPP_FORMAT_ACCESSOR(g) VTFPP_FORMAT_ACCESSOR(b));
VTFPP_FORMAT(CONSOLE_BGR888_LINEAR,       VTFPP_FORMAT_ACCESSOR(r) VTFPP_FORMAT_ACCESSOR(g) VTFPP_FORMAT_ACCESSOR(b));
VTFPP_FORMAT(CONSOLE_BGRX5551_LINEAR,     VTFPP_FORMAT_ACCESSOR_BSWAP(CONSOLE_BGRX5551_LINEAR, r) VTFPP_FORMAT_ACCESSOR_BSWAP(CONSOLE_BGRX5551_LINEAR, g) VTFPP_FORMAT_ACCESSOR_BSWAP(CONSOLE_BGRX5551_LINEAR, b) VTFPP_FORMAT_ACCESSOR_BSWAP(CONSOLE_BGRX5551_LINEAR, x));
VTFPP_FORMAT(CONSOLE_I8_LINEAR,           VTFPP_FORMAT_ACCESSOR(i));
VTFPP_FORMAT(CONSOLE_RGBA16161616_LINEAR, VTFPP_FORMAT_ACCESSOR(r) VTFPP_FORMAT_ACCESSOR(g) VTFPP_FORMAT_ACCESSOR(b) VTFPP_FORMAT_ACCESSOR(a));
VTFPP_FORMAT(CONSOLE_BGRX8888_LE,         VTFPP_FORMAT_ACCESSOR(r) VTFPP_FORMAT_ACCESSOR(g) VTFPP_FORMAT_ACCESSOR(b) VTFPP_FORMAT_ACCESSOR(x));
VTFPP_FORMAT(CONSOLE_BGRA8888_LE,         VTFPP_FORMAT_ACCESSOR(r) VTFPP_FORMAT_ACCESSOR(g) VTFPP_FORMAT_ACCESSOR(b) VTFPP_FORMAT_ACCESSOR(a));
VTFPP_FORMAT(STRATA_R8,                   VTFPP_FORMAT_ACCESSOR(r));

#undef VTFPP_FORMAT
#undef VTFPP_FORMAT_ACCESSOR_BSWAP
#undef VTFPP_FORMAT_ACCESSOR

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
		std::same_as<T, STRATA_R8>;

/// Extracts a single channel from the given image data.
/// May have unexpected behavior if called on formats that use bitfields like BGRA5551!
/// Data is packed according to pixel channel C++ type size
/// (e.g. in the case of BGRA5551's green channel, it'll be 2 bytes per green value despite only 5 bits being used in the original data)
template<PixelType P, typename C>
[[nodiscard]] std::vector<std::byte> extractChannelFromImageData(std::span<const std::byte> imageData, C(P::*channel)() const) {
	if (imageData.empty() || imageData.size() % sizeof(P) != 0 || !channel) {
		return {};
	}

	std::span pixels{reinterpret_cast<const P*>(imageData.data()), imageData.size() / sizeof(P)};

	std::vector<std::byte> out(imageData.size() / sizeof(P) * sizeof(C));
	BufferStream stream{out, false};
	// todo: tbb
	for (const auto& pixel : pixels) {
		stream << (pixel.*channel)();
	}
	return out;
}

} // namespace vtfpp::ImagePixel
