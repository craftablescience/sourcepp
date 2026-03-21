#pragma once

#include <bit>
#include <concepts>
#include <cstring>
#include <functional>

#ifdef SOURCEPP_BUILD_WITH_TBB
#include <execution>
#endif

#include <BufferStream.h>

#include "ImageFormats.h"

namespace vtfpp::ImagePixel {

#define VTFPP_FORMAT_ACCESSOR(channel, channelCaps)                     \
	[[nodiscard]] constexpr auto channel() const {                      \
		if constexpr (std::endian::native == std::endian::little) {     \
			return pixel.le.channel;                                    \
		} else {                                                        \
			return pixel.be.channel;                                    \
		}                                                               \
	}                                                                   \
	constexpr void set##channelCaps(decltype(pixel.le.channel) value) { \
		if constexpr (std::endian::native == std::endian::little) {     \
			pixel.le.channel = value;                                   \
		} else {                                                        \
			pixel.be.channel = value;                                   \
		}                                                               \
	}

#define VTFPP_A VTFPP_FORMAT_ACCESSOR(a, A)
#define VTFPP_B VTFPP_FORMAT_ACCESSOR(b, B)
#define VTFPP_G VTFPP_FORMAT_ACCESSOR(g, G)
#define VTFPP_I VTFPP_FORMAT_ACCESSOR(i, I)
#define VTFPP_L VTFPP_FORMAT_ACCESSOR(l, L)
#define VTFPP_P VTFPP_FORMAT_ACCESSOR(p, P)
#define VTFPP_Q VTFPP_FORMAT_ACCESSOR(q, Q)
#define VTFPP_R VTFPP_FORMAT_ACCESSOR(r, R)
#define VTFPP_U VTFPP_FORMAT_ACCESSOR(u, U)
#define VTFPP_V VTFPP_FORMAT_ACCESSOR(v, V)
#define VTFPP_W VTFPP_FORMAT_ACCESSOR(w, W)
#define VTFPP_X VTFPP_FORMAT_ACCESSOR(x, X)

#define VTFPP_FORMAT_ACCESSOR_BSWAP(format, channel, channelCaps)           \
	[[nodiscard]] constexpr auto channel() const {                          \
		if constexpr (std::endian::native == std::endian::little) {         \
			return pixel.le.channel;                                        \
		} else {                                                            \
			auto swapped = pixel.be.data;                                   \
			BufferStream::swap_endian(&swapped);                            \
			return std::bit_cast<LE>(swapped).channel;                      \
		}                                                                   \
	}                                                                       \
	constexpr void set##channelCaps(decltype(pixel.le.channel) value) {     \
		if constexpr (std::endian::native == std::endian::little) {         \
			pixel.le.channel = value;                                       \
		} else {                                                            \
			auto pixelLE = std::bit_cast<LE>(pixel.be.data);                \
			pixelLE.channel = value;                                        \
			auto swapped = std::bit_cast<decltype(pixel.be.data)>(pixelLE); \
			BufferStream::swap_endian(&swapped);                            \
			pixel.be.data = swapped;                                        \
		}                                                                   \
	}

#define VTFPP_A_BSWAP(format) VTFPP_FORMAT_ACCESSOR_BSWAP(format, a, A)
#define VTFPP_B_BSWAP(format) VTFPP_FORMAT_ACCESSOR_BSWAP(format, b, B)
#define VTFPP_G_BSWAP(format) VTFPP_FORMAT_ACCESSOR_BSWAP(format, g, G)
#define VTFPP_R_BSWAP(format) VTFPP_FORMAT_ACCESSOR_BSWAP(format, r, R)
#define VTFPP_X_BSWAP(format) VTFPP_FORMAT_ACCESSOR_BSWAP(format, x, X)

#define VTFPP_FORMAT_INTERNAL(bo, ...) \
	struct bo {                        \
		__VA_ARGS__;                   \
	}; static_assert(std::is_trivially_copyable_v<bo> && sizeof(bo) == ImageFormatDetails::bpp(FORMAT) / 8);

#define VTFPP_FORMAT_LE(...) VTFPP_FORMAT_INTERNAL(LE, __VA_ARGS__)
#define VTFPP_FORMAT_BE(...) VTFPP_FORMAT_INTERNAL(BE, __VA_ARGS__)

#define VTFPP_FORMAT(format, internalFormatClasses, accessors)           \
	class format {                                                       \
	public:                                                              \
		static constexpr auto FORMAT = ImageFormat::format;              \
		internalFormatClasses                                            \
	protected:                                                           \
		union Pixel {                                                    \
			LE le;                                                       \
			BE be;                                                       \
			Pixel() { std::memset(this, 0, sizeof(Pixel)); }             \
		} pixel;                                                         \
	public:                                                              \
		format(LE le) {                                                  \
			if constexpr (std::endian::native == std::endian::little) {  \
				this->pixel.le = le;                                     \
			} else {                                                     \
				BufferStream::swap_endian(&le);                          \
				this->pixel.be = std::bit_cast<BE>(le);                  \
			}                                                            \
		}                                                                \
		accessors                                                        \
}; static_assert(std::is_trivially_copyable_v<format> && sizeof(format) == ImageFormatDetails::bpp(ImageFormat::format) / 8)

VTFPP_FORMAT(
	RGBA8888,
	VTFPP_FORMAT_LE(uint8_t r, g, b, a)
	VTFPP_FORMAT_BE(uint8_t a, b, g, r),
	VTFPP_R VTFPP_G VTFPP_B VTFPP_A
);

VTFPP_FORMAT(
	ABGR8888,
	VTFPP_FORMAT_LE(uint8_t a, b, g, r)
	VTFPP_FORMAT_BE(uint8_t r, g, b, a),
	VTFPP_A VTFPP_B VTFPP_G VTFPP_R
);

VTFPP_FORMAT(
	RGB888,
	VTFPP_FORMAT_LE(uint8_t r, g, b)
	VTFPP_FORMAT_BE(uint8_t b, g, r),
	VTFPP_R VTFPP_G VTFPP_B
);

VTFPP_FORMAT(
	BGR888,
	VTFPP_FORMAT_LE(uint8_t b, g, r)
	VTFPP_FORMAT_BE(uint8_t r, g, b),
	VTFPP_B VTFPP_G VTFPP_R
);

VTFPP_FORMAT(
	RGB565,
	VTFPP_FORMAT_LE(uint16_t r : 5, g : 6, b : 5)
	VTFPP_FORMAT_BE(uint16_t data),
	VTFPP_R_BSWAP(RGB565) VTFPP_G_BSWAP(RGB565) VTFPP_B_BSWAP(RGB565)
);

VTFPP_FORMAT(
	I8,
	VTFPP_FORMAT_LE(uint8_t i)
	VTFPP_FORMAT_BE(uint8_t i),
	VTFPP_I
);

VTFPP_FORMAT(
	IA88,
	VTFPP_FORMAT_LE(uint8_t i, a)
	VTFPP_FORMAT_BE(uint8_t a, i),
	VTFPP_I VTFPP_A
);

VTFPP_FORMAT(
	P8,
	VTFPP_FORMAT_LE(uint8_t p)
	VTFPP_FORMAT_BE(uint8_t p),
	VTFPP_P
);

VTFPP_FORMAT(
	A8,
	VTFPP_FORMAT_LE(uint8_t a)
	VTFPP_FORMAT_BE(uint8_t a),
	VTFPP_A
);

VTFPP_FORMAT(
	RGB888_BLUESCREEN,
	VTFPP_FORMAT_LE(uint8_t r, g, b)
	VTFPP_FORMAT_BE(uint8_t b, g, r),
	VTFPP_R VTFPP_G VTFPP_B
);

VTFPP_FORMAT(
	BGR888_BLUESCREEN,
	VTFPP_FORMAT_LE(uint8_t b, g, r)
	VTFPP_FORMAT_BE(uint8_t r, g, b),
	VTFPP_B VTFPP_G VTFPP_R
);

VTFPP_FORMAT(
	ARGB8888,
	VTFPP_FORMAT_LE(uint8_t a, r, g, b)
	VTFPP_FORMAT_BE(uint8_t b, g, r, a),
	VTFPP_A VTFPP_R VTFPP_G VTFPP_B
);

VTFPP_FORMAT(
	BGRA8888,
	VTFPP_FORMAT_LE(uint8_t b, g, r, a)
	VTFPP_FORMAT_BE(uint8_t a, r, g, b),
	VTFPP_B VTFPP_G VTFPP_R VTFPP_A
);

VTFPP_FORMAT(
	BGRX8888,
	VTFPP_FORMAT_LE(uint8_t b, g, r, x)
	VTFPP_FORMAT_BE(uint8_t x, r, g, b),
	VTFPP_B VTFPP_G VTFPP_R VTFPP_X
);

VTFPP_FORMAT(
	BGR565,
	VTFPP_FORMAT_LE(uint16_t b : 5, g : 6, r : 5)
	VTFPP_FORMAT_BE(uint16_t data),
	VTFPP_B_BSWAP(BGR565) VTFPP_G_BSWAP(BGR565) VTFPP_R_BSWAP(BGR565)
);

VTFPP_FORMAT(
	BGRX5551,
	VTFPP_FORMAT_LE(uint16_t b : 5, g : 5, r : 5, x : 1)
	VTFPP_FORMAT_BE(uint16_t data),
	VTFPP_B_BSWAP(BGRX5551) VTFPP_G_BSWAP(BGRX5551) VTFPP_R_BSWAP(BGRX5551) VTFPP_X_BSWAP(BGRX5551)
);

VTFPP_FORMAT(
	BGRA4444,
	VTFPP_FORMAT_LE(uint16_t b : 4, g : 4, r : 4, a : 4)
	VTFPP_FORMAT_BE(uint16_t data),
	VTFPP_B_BSWAP(BGRA4444) VTFPP_G_BSWAP(BGRA4444) VTFPP_R_BSWAP(BGRA4444) VTFPP_A_BSWAP(BGRA4444)
);

VTFPP_FORMAT(
	BGRA5551,
	VTFPP_FORMAT_LE(uint16_t b : 5, g : 5, r : 5, a : 1)
	VTFPP_FORMAT_BE(uint16_t data),
	VTFPP_B_BSWAP(BGRA5551) VTFPP_G_BSWAP(BGRA5551) VTFPP_R_BSWAP(BGRA5551) VTFPP_A_BSWAP(BGRA5551)
);

VTFPP_FORMAT(
	UV88,
	VTFPP_FORMAT_LE(uint8_t u, v)
	VTFPP_FORMAT_BE(uint8_t v, u),
	VTFPP_U VTFPP_V
);

VTFPP_FORMAT(
	UVWQ8888,
	VTFPP_FORMAT_LE(uint8_t u, v, w, q)
	VTFPP_FORMAT_BE(uint8_t q, w, v, u),
	VTFPP_U VTFPP_V VTFPP_W VTFPP_Q
);

VTFPP_FORMAT(
	RGBA16161616F,
	VTFPP_FORMAT_LE(half r, g, b, a)
	VTFPP_FORMAT_BE(half a, b, g, r),
	VTFPP_R VTFPP_G VTFPP_B VTFPP_A
);

VTFPP_FORMAT(
	RGBA16161616,
	VTFPP_FORMAT_LE(uint16_t r, g, b, a)
	VTFPP_FORMAT_BE(uint16_t a, b, g, r),
	VTFPP_R VTFPP_G VTFPP_B VTFPP_A
);

VTFPP_FORMAT(
	UVLX8888,
	VTFPP_FORMAT_LE(uint8_t u, v, l, x)
	VTFPP_FORMAT_BE(uint8_t x, l, v, u),
	VTFPP_U VTFPP_V VTFPP_L VTFPP_X
);

VTFPP_FORMAT(
	R32F,
	VTFPP_FORMAT_LE(float r)
	VTFPP_FORMAT_BE(float r),
	VTFPP_R
);

VTFPP_FORMAT(
	RGB323232F,
	VTFPP_FORMAT_LE(float r, g, b)
	VTFPP_FORMAT_BE(float b, g, r),
	VTFPP_R VTFPP_G VTFPP_B
);

VTFPP_FORMAT(
	RGBA32323232F,
	VTFPP_FORMAT_LE(float r, g, b, a)
	VTFPP_FORMAT_BE(float a, b, g, r),
	VTFPP_R VTFPP_G VTFPP_B VTFPP_A
);

VTFPP_FORMAT(
	RG1616F,
	VTFPP_FORMAT_LE(half r, g)
	VTFPP_FORMAT_BE(half g, r),
	VTFPP_R VTFPP_G
);

VTFPP_FORMAT(
	RG3232F,
	VTFPP_FORMAT_LE(float r, g)
	VTFPP_FORMAT_BE(float g, r),
	VTFPP_R VTFPP_G
);

VTFPP_FORMAT(
	RGBX8888,
	VTFPP_FORMAT_LE(uint8_t r, g, b, x)
	VTFPP_FORMAT_BE(uint8_t x, b, g, r),
	VTFPP_R VTFPP_G VTFPP_B VTFPP_X
);

VTFPP_FORMAT(
	RGBA1010102,
	VTFPP_FORMAT_LE(uint32_t r : 10, g : 10, b : 10, a : 2)
	VTFPP_FORMAT_BE(uint32_t data),
	VTFPP_R_BSWAP(RGBA1010102) VTFPP_G_BSWAP(RGBA1010102) VTFPP_B_BSWAP(RGBA1010102) VTFPP_A_BSWAP(RGBA1010102)
);

VTFPP_FORMAT(
	BGRA1010102,
	VTFPP_FORMAT_LE(uint32_t b : 10, g : 10, r : 10, a : 2)
	VTFPP_FORMAT_BE(uint32_t data),
	VTFPP_B_BSWAP(RGBA1010102) VTFPP_G_BSWAP(RGBA1010102) VTFPP_R_BSWAP(RGBA1010102) VTFPP_A_BSWAP(RGBA1010102)
);

VTFPP_FORMAT(
	R16F,
	VTFPP_FORMAT_LE(half r)
	VTFPP_FORMAT_BE(half r),
	VTFPP_R
);

VTFPP_FORMAT(
	CONSOLE_BGRX8888_LINEAR,
	VTFPP_FORMAT_LE(uint8_t b, g, r, x)
	VTFPP_FORMAT_BE(uint8_t x, r, g, b),
	VTFPP_B VTFPP_G VTFPP_R VTFPP_X
);

VTFPP_FORMAT(
	CONSOLE_RGBA8888_LINEAR,
	VTFPP_FORMAT_LE(uint8_t r, g, b, a)
	VTFPP_FORMAT_BE(uint8_t a, b, g, r),
	VTFPP_R VTFPP_G VTFPP_B VTFPP_A
);

VTFPP_FORMAT(
	CONSOLE_ABGR8888_LINEAR,
	VTFPP_FORMAT_LE(uint8_t a, b, g, r)
	VTFPP_FORMAT_BE(uint8_t r, g, b, a),
	VTFPP_A VTFPP_B VTFPP_G VTFPP_R
);

VTFPP_FORMAT(
	CONSOLE_ARGB8888_LINEAR,
	VTFPP_FORMAT_LE(uint8_t a, r, g, b)
	VTFPP_FORMAT_BE(uint8_t b, g, r, a),
	VTFPP_A VTFPP_R VTFPP_G VTFPP_B
);

VTFPP_FORMAT(
	CONSOLE_BGRA8888_LINEAR,
	VTFPP_FORMAT_LE(uint8_t b, g, r, a)
	VTFPP_FORMAT_BE(uint8_t a, r, g, b),
	VTFPP_B VTFPP_G VTFPP_R VTFPP_A
);

VTFPP_FORMAT(
	CONSOLE_RGB888_LINEAR,
	VTFPP_FORMAT_LE(uint8_t r, g, b)
	VTFPP_FORMAT_BE(uint8_t b, g, r),
	VTFPP_R VTFPP_G VTFPP_B
);

VTFPP_FORMAT(
	CONSOLE_BGR888_LINEAR,
	VTFPP_FORMAT_LE(uint8_t b, g, r)
	VTFPP_FORMAT_BE(uint8_t r, g, b),
	VTFPP_B VTFPP_G VTFPP_R
);

VTFPP_FORMAT(
	CONSOLE_BGRX5551_LINEAR,
	VTFPP_FORMAT_LE(uint16_t b : 5, g : 5, r : 5, x : 1)
	VTFPP_FORMAT_BE(uint16_t data),
	VTFPP_B_BSWAP(CONSOLE_BGRX5551_LINEAR) VTFPP_G_BSWAP(CONSOLE_BGRX5551_LINEAR) VTFPP_R_BSWAP(CONSOLE_BGRX5551_LINEAR) VTFPP_X_BSWAP(CONSOLE_BGRX5551_LINEAR)
);

VTFPP_FORMAT(
	CONSOLE_I8_LINEAR,
	VTFPP_FORMAT_LE(uint8_t i)
	VTFPP_FORMAT_BE(uint8_t i),
	VTFPP_I
);

VTFPP_FORMAT(
	CONSOLE_RGBA16161616_LINEAR,
	VTFPP_FORMAT_LE(uint16_t r, g, b, a)
	VTFPP_FORMAT_BE(uint16_t a, b, g, r),
	VTFPP_R VTFPP_G VTFPP_B VTFPP_A
);

VTFPP_FORMAT(
	CONSOLE_BGRX8888_LE,
	VTFPP_FORMAT_LE(uint8_t b, g, r, x)
	VTFPP_FORMAT_BE(uint8_t x, r, g, b),
	VTFPP_B VTFPP_G VTFPP_R VTFPP_X
);

VTFPP_FORMAT(
	CONSOLE_BGRA8888_LE,
	VTFPP_FORMAT_LE(uint8_t b, g, r, a)
	VTFPP_FORMAT_BE(uint8_t a, r, g, b),
	VTFPP_B VTFPP_G VTFPP_R VTFPP_A
);

VTFPP_FORMAT(
	STRATA_R8,
	VTFPP_FORMAT_LE(uint8_t r)
	VTFPP_FORMAT_BE(uint8_t r),
	VTFPP_R
);

#undef VTFPP_FORMAT
#undef VTFPP_FORMAT_BE
#undef VTFPP_FORMAT_LE
#undef VTFPP_FORMAT_INTERNAL
#undef VTFPP_X_BSWAP
#undef VTFPP_R_BSWAP
#undef VTFPP_G_BSWAP
#undef VTFPP_B_BSWAP
#undef VTFPP_A_BSWAP
#undef VTFPP_FORMAT_ACCESSOR_BSWAP
#undef VTFPP_X
#undef VTFPP_W
#undef VTFPP_V
#undef VTFPP_U
#undef VTFPP_R
#undef VTFPP_Q
#undef VTFPP_P
#undef VTFPP_L
#undef VTFPP_I
#undef VTFPP_G
#undef VTFPP_B
#undef VTFPP_A
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

	std::vector<std::byte> out(imageData.size() / sizeof(P) * sizeof(C));
	std::span outSpan{reinterpret_cast<C*>(out.data()), out.size() / sizeof(C)};

	std::span pixels{reinterpret_cast<const P*>(imageData.data()), imageData.size() / sizeof(P)};
	std::transform(
#ifdef SOURCEPP_BUILD_WITH_TBB
		std::execution::par_unseq,
#endif
		pixels.begin(),
		pixels.end(),
		outSpan.begin(),
		[channel](P pixel) {
			return (pixel.*channel)();
		}
	);

	return out;
}

} // namespace vtfpp::ImagePixel
