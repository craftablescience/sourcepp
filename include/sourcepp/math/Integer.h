#pragma once

#include <bit>
#include <concepts>
#include <cstdint>
#include <type_traits>

// Integer types are intentionally outside the sourcepp namespace
using std::int8_t;
using std::int16_t;
using std::int32_t;
using std::int64_t;
using std::uint8_t;
using std::uint16_t;
using std::uint32_t;
using std::uint64_t;

namespace sourcepp::math {

template<typename T>
concept Arithmetic = std::is_arithmetic_v<T>;

template<Arithmetic T>
[[nodiscard]] constexpr T remap(T value, T l1, T h1, T l2, T h2) {
	return l2 + (value - l1) * (h2 - l2) / (h1 - l1);
}

template<Arithmetic T>
[[nodiscard]] constexpr T remap(T value, T h1, T h2) {
	return value * h2 / h1;
}

[[nodiscard]] constexpr bool isPowerOf2(std::integral auto n) {
	return n && !(n & (n - 1));
}

template<std::integral T>
[[nodiscard]] constexpr T nearestPowerOf2(T n) {
	if (math::isPowerOf2(n)) {
		return n;
	}
	auto bigger = std::bit_ceil(n);
	auto smaller = std::bit_floor(n);
	return (n - smaller) < (bigger - n) ? smaller : bigger;
}

[[nodiscard]] constexpr uint16_t getPaddingForAlignment(uint16_t alignment, uint64_t n) {
	if (const auto rest = n % alignment; rest > 0) {
		return alignment - rest;
	}
	return 0;
}

} // namespace sourcepp::math
