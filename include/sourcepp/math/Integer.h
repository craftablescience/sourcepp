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

/// 3-byte wide unsigned integer
struct uint24_t {
	uint24_t() = default;

	template<std::integral T>
	constexpr uint24_t(T value) // NOLINT(*-explicit-constructor)
			: bytes{
				static_cast<uint8_t>((value >> 16) & 0xff),
				static_cast<uint8_t>((value >> 8)  & 0xff),
				static_cast<uint8_t>( value        & 0xff),
			} {}

	template<std::integral T>
	[[nodiscard]] constexpr operator T() const { // NOLINT(*-explicit-constructor)
		return static_cast<T>((bytes[0] << 16) | (bytes[1] << 8) | bytes[2]);
	}

	template<std::integral T>
	constexpr uint24_t& operator=(T value) {
		*this = {value};
		return *this;
	}

	template<std::integral T>
	[[nodiscard]] constexpr uint24_t operator+(T value) const {
		return {uint32_t{*this} + value};
	}

	template<std::integral T>
	constexpr void operator+=(T value) const {
		*this = {uint32_t{*this} + value};
	}

	constexpr uint24_t operator++() {
		return *this = {uint32_t{*this} + 1};
	}

	constexpr uint24_t operator++(int) {
		uint24_t out{*this};
		*this = {uint32_t{*this} + 1};
		return out;
	}

	template<std::integral T>
	[[nodiscard]] constexpr uint24_t operator-(T value) const {
		return {uint32_t{*this} - value};
	}

	template<std::integral T>
	constexpr void operator-=(T value) const {
		return *this = {uint32_t{*this} - value};
	}

	constexpr uint24_t operator--() {
		return *this = {uint32_t{*this} - 1};
	}

	constexpr uint24_t operator--(int) {
		uint24_t out{*this};
		*this = {uint32_t{*this} - 1};
		return out;
	}

	template<std::integral T>
	[[nodiscard]] constexpr uint24_t operator*(T value) const {
		return {uint32_t{*this} * value};
	}

	template<std::integral T>
	constexpr void operator*=(T value) const {
		*this = {uint32_t{*this} * value};
	}

	template<std::integral T>
	[[nodiscard]] constexpr uint24_t operator/(T value) const {
		return {uint32_t{*this} / value};
	}

	template<std::integral T>
	constexpr void operator/=(T value) const {
		*this = {uint32_t{*this} / value};
	}

	template<std::integral T>
	[[nodiscard]] constexpr uint24_t operator%(T value) const {
		return {uint32_t{*this} % value};
	}

	template<std::integral T>
	constexpr void operator%=(T value) const {
		*this = {uint32_t{*this} % value};
	}

	template<std::integral T>
	[[nodiscard]] constexpr bool operator==(T value) const {
		return uint32_t{*this} == value;
	}

	template<std::integral T>
	[[nodiscard]] constexpr auto operator<=>(T value) const {
		return uint32_t{*this} <=> value;
	}

	[[nodiscard]] constexpr operator bool() const { // NOLINT(*-explicit-constructor)
		return static_cast<bool>(uint32_t{*this});
	}

	uint8_t bytes[3];
};
static_assert(sizeof(uint24_t) == 3, "uint24_t is not 3 bytes wide!");
static_assert(std::is_trivially_copyable_v<uint24_t>, "uint24_t is not a POD type!");

namespace sourcepp::math {

template<typename T>
concept Arithmetic = std::is_arithmetic_v<T> || std::same_as<T, uint24_t>;

template<Arithmetic T>
T remap(T value, T l1, T h1, T l2, T h2) {
	return l2 + (value - l1) * (h2 - l2) / (h1 - l1);
}

template<Arithmetic T>
T remap(T value, T h1, T h2) {
	return value * h2 / h1;
}

} // namespace sourcepp::math
