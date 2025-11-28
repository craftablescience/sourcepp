#pragma once

#include <bit>
#include <concepts>
#include <cmath>
#include <cstdint>
#include <numbers>
#include <type_traits>

#include <half.hpp>

// Numeric types are intentionally outside the sourcepp namespace
using std::int8_t;
using std::int16_t;
using std::int32_t;
using std::int64_t;
using std::uint8_t;
using std::uint16_t;
using std::uint32_t;
using std::uint64_t;
using half_float::half;

namespace sourcepp::math {

template<std::floating_point F>
constexpr F pi = std::numbers::pi_v<F>;
constexpr auto pi_f32  = pi<float>;
constexpr auto pi_f64  = pi<double>;

template<typename T>
concept Arithmetic = std::is_arithmetic_v<T> || std::same_as<T, half>;

template<Arithmetic T>
[[nodiscard]] constexpr T remap(T value, T l1, T h1, T l2, T h2) {
	return l2 + (value - l1) * (h2 - l2) / (h1 - l1);
}

template<Arithmetic T>
[[nodiscard]] constexpr T remap(T value, T h1, T h2) {
	return value * h2 / h1;
}

[[nodiscard]] constexpr bool isPowerOf2(std::unsigned_integral auto n) {
	return n && !(n & (n - 1));
}

template<std::unsigned_integral T>
[[nodiscard]] constexpr T nearestPowerOf2(T n) {
	if (isPowerOf2(n)) {
		return n;
	}
	auto bigger = std::bit_ceil(n);
	auto smaller = std::bit_floor(n);
	return (n - smaller) < (bigger - n) ? smaller : bigger;
}

[[nodiscard]] constexpr uint32_t log2ceil(uint32_t value) {
	return ((std::bit_cast<uint32_t>(static_cast<float>(value)) >> 23) & 0xff) - 127;
}

[[nodiscard]] constexpr uint16_t paddingForAlignment(uint16_t alignment, uint64_t n) {
	if (const auto rest = n % alignment; rest > 0) {
		return alignment - rest;
	}
	return 0;
}

template<uint8_t S, Arithmetic P>
struct Vec {
	static_assert(S >= 2, "Vectors must have at least two values!");

	P values[S];

	// By defining these constructors, the type becomes nontrivial...
#if 1
	constexpr Vec() = default;

	template<std::convertible_to<P>... Vals>
	requires (sizeof...(Vals) == S)
	constexpr Vec(Vals... vals) // NOLINT(*-explicit-constructor)
			: values{static_cast<P>(vals)...} {}
#endif

	using value_type = P;

	[[nodiscard]] constexpr const P* data() const {
		return this->values;
	}

	[[nodiscard]] constexpr P* data() {
		return this->values;
	}

	[[nodiscard]] constexpr uint8_t size() const {
		return S;
	}

	[[nodiscard]] constexpr P& operator[](uint8_t index) {
		if (index < S) {
			return this->values[index];
		}
		return this->operator[](index % S);
	}

	[[nodiscard]] constexpr P operator[](uint8_t index) const {
		if (index < S) {
			return this->values[index];
		}
		return this->operator[](index % S);
	}

	[[nodiscard]] constexpr Vec operator+() const {
		return *this;
	}

	template<uint8_t SO, Arithmetic PO>
	[[nodiscard]] constexpr Vec operator+(const Vec<SO, PO>& other) const {
		auto out = *this;
		for (uint8_t i = 0; i < (S > SO ? SO : S); i++) {
			out[i] += static_cast<P>(other[i]);
		}
		return out;
	}

	template<uint8_t SO, Arithmetic PO>
	constexpr void operator+=(const Vec<SO, PO>& other) {
		for (uint8_t i = 0; i < (S > SO ? SO : S); i++) {
			(*this)[i] += static_cast<P>(other[i]);
		}
	}

	[[nodiscard]] constexpr Vec operator-() const {
		auto out = *this;
		for (uint8_t i = 0; i < S; i++) {
			out[i] *= -1;
		}
		return out;
	}

	template<uint8_t SO, Arithmetic PO>
	[[nodiscard]] constexpr Vec operator-(const Vec<SO, PO>& other) const {
		auto out = *this;
		for (uint8_t i = 0; i < (S > SO ? SO : S); i++) {
			out[i] -= static_cast<P>(other[i]);
		}
		return out;
	}

	template<uint8_t SO, Arithmetic PO>
	constexpr void operator-=(const Vec<SO, PO>& other) {
		for (uint8_t i = 0; i < (S > SO ? SO : S); i++) {
			(*this)[i] -= static_cast<P>(other[i]);
		}
	}

	[[nodiscard]] constexpr Vec operator*(Arithmetic auto scalar) const {
		auto out = *this;
		for (uint8_t i = 0; i < S; i++) {
			out[i] *= static_cast<P>(scalar);
		}
		return out;
	}

	constexpr void operator*=(Arithmetic auto scalar) {
		for (uint8_t i = 0; i < S; i++) {
			(*this)[i] *= static_cast<P>(scalar);
		}
	}

	[[nodiscard]] constexpr Vec operator/(Arithmetic auto scalar) const {
		auto out = *this;
		for (uint8_t i = 0; i < S; i++) {
			out[i] /= static_cast<P>(scalar);
		}
		return out;
	}

	constexpr void operator/=(Arithmetic auto scalar) {
		for (uint8_t i = 0; i < S; i++) {
			(*this)[i] /= static_cast<P>(scalar);
		}
	}

	[[nodiscard]] constexpr Vec operator%(Arithmetic auto scalar) const {
		auto out = *this;
		for (uint8_t i = 0; i < S; i++) {
			out[i] %= static_cast<P>(scalar);
		}
		return out;
	}

	constexpr void operator%=(Arithmetic auto scalar) {
		for (uint8_t i = 0; i < S; i++) {
			(*this)[i] %= static_cast<P>(scalar);
		}
	}

	template<uint8_t SO, Arithmetic PO>
	[[nodiscard]] constexpr bool operator==(const Vec<SO, PO>& other) const {
		if constexpr (S != SO) {
			return false;
		} else {
			for (uint8_t i = 0; i < S; i++) {
				if ((*this)[i] != static_cast<P>(other[i])) {
					return false;
				}
			}
			return true;
		}
	}

	template<uint8_t SO, Arithmetic PO = P>
	[[nodiscard]] constexpr Vec<SO, PO> to() const {
		Vec<SO, PO> out{};
		for (uint8_t i = 0; i < (S > SO ? SO : S); i++) {
			out[i] = static_cast<PO>((*this)[i]);
		}
		return out;
	}

	template<uint8_t SO, Arithmetic PO>
	[[nodiscard]] constexpr Vec mul(const Vec<SO, PO>& other) const {
		auto out = *this;
		for (uint8_t i = 0; i < (S > SO ? SO : S); i++) {
			out[i] *= static_cast<P>(other[i]);
		}
		return out;
	}

	template<uint8_t SO, Arithmetic PO>
	[[nodiscard]] constexpr Vec div(const Vec<SO, PO>& other) const {
		auto out = *this;
		for (uint8_t i = 0; i < (S > SO ? SO : S); i++) {
			out[i] /= static_cast<P>(other[i]);
		}
		return out;
	}

	template<uint8_t SO, Arithmetic PO>
	[[nodiscard]] constexpr Vec mod(const Vec<SO, PO>& other) const {
		auto out = *this;
		for (uint8_t i = 0; i < (S > SO ? SO : S); i++) {
			if constexpr ((std::floating_point<P> && std::floating_point<PO>) || std::floating_point<P>) {
				out[i] = std::fmod(out[i], static_cast<P>(other[i]));
			} else {
				out[i] %= static_cast<P>(other[i]);
			}
		}
		return out;
	}

	[[nodiscard]] constexpr float magf() const {
		float out = 0.0;
		for (uint8_t i = 0; i < S; i++) {
			out += std::pow((*this)[i], 2);
		}
		return std::sqrt(out);
	}

	[[nodiscard]] constexpr double mag() const {
		double out = 0.0;
		for (uint8_t i = 0; i < S; i++) {
			out += std::pow((*this)[i], 2);
		}
		return std::sqrt(out);
	}

	[[nodiscard]] constexpr P sum() const {
		P out{};
		for (uint8_t i = 0; i < S; i++) {
			out += (*this)[i];
		}
		return out;
	}

	template<Arithmetic PO>
	[[nodiscard]] constexpr Vec scale(const Vec<S, PO>& other) const {
		Vec out;
		for (uint8_t i = 0; i < S; i++) {
			out[i] = (*this)[i] * static_cast<P>(other[i]);
		}
		return out;
	}

	template<Arithmetic PO>
	[[nodiscard]] constexpr P dot(const Vec<S, PO>& other) const {
		return this->scale(other).sum();
	}

	[[nodiscard]] constexpr Vec abs() const {
		auto out = *this;
		for (uint8_t i = 0; i < S; i++) {
			out[i] = std::abs(out[i]);
		}
		return out;
	}

	[[nodiscard]] static constexpr Vec zero() {
		return {};
	}

	[[nodiscard]] constexpr bool isZero() const {
		return *this == zero();
	}
};
static_assert(std::is_trivially_copyable_v<Vec<2, float>>);

#define SOURCEPP_VEC_DEFINE(S) \
	template<Arithmetic P> \
	using Vec##S       = Vec<S, P>; \
	using Vec##S##i8   = Vec##S<int8_t>; \
	using Vec##S##i16  = Vec##S<int16_t>; \
	using Vec##S##i32  = Vec##S<int32_t>; \
	using Vec##S##i64  = Vec##S<int64_t>; \
	using Vec##S##i    = Vec##S##i32; \
	using Vec##S##ui8  = Vec##S<uint8_t>; \
	using Vec##S##ui16 = Vec##S<uint16_t>; \
	using Vec##S##ui32 = Vec##S<uint32_t>; \
	using Vec##S##ui64 = Vec##S<uint64_t>; \
	using Vec##S##ui   = Vec##S##ui32; \
	using Vec##S##f16  = Vec##S<half>; \
	using Vec##S##f32  = Vec##S<float>; \
	using Vec##S##f64  = Vec##S<double>; \
	using Vec##S##f    = Vec##S##f32

SOURCEPP_VEC_DEFINE(2);
SOURCEPP_VEC_DEFINE(3);
SOURCEPP_VEC_DEFINE(4);

#undef SOURCEPP_VEC_DEFINE

using EulerAngles = Vec3f;

using Quat = Vec4f;

/// Lower precision Quat compressed to 6 bytes
struct QuatCompressed48 {
	uint16_t x : 16;
	uint16_t y : 16;
	uint16_t z : 15;
	uint16_t wn : 1;

	[[nodiscard]] Quat decompress() const {
		// Convert from 16-bit (or 15-bit) integers to floating point values in the range [-1, 1]
		const float fx = (static_cast<float>(this->x) / 32767.5f) - 1.f; // x / ((2^16 - 1) / 2) - 1
		const float fy = (static_cast<float>(this->y) / 32767.5f) - 1.f; // y / ((2^16 - 1) / 2) - 1
		const float fz = (static_cast<float>(this->z) / 16383.5f) - 1.f; // z / ((2^15 - 1) / 2) - 1

		// Recalculate w from the constraint that x^2 + y^2 + z^2 + w^2 = 1
		float fw = std::sqrt(1.f - fx * fx - fy * fy - fz * fz);

		// Adjust w based on the stored sign bit
		if (this->wn) {
			fw = -fw;
		}

		return {fx, fy, fz, fw};
	}
};
static_assert(std::is_trivially_copyable_v<QuatCompressed48>);

/// Lower precision Quat compressed to 8 bytes
struct QuatCompressed64 {
	uint32_t x : 21;
	uint32_t y : 21;
	uint32_t z : 21;
	uint32_t wn : 1;

	[[nodiscard]] Quat decompress() const {
		// Convert from 21-bit integers to floating point values in the range [-1, 1]
		const double fx = (static_cast<double>(this->x) / 1048575.5) - 1.0f; // x / ((2^21 - 1) / 2) - 1
		const double fy = (static_cast<double>(this->y) / 1048575.5) - 1.0f; // y / ((2^21 - 1) / 2) - 1
		const double fz = (static_cast<double>(this->z) / 1048575.5) - 1.0f; // z / ((2^21 - 1) / 2) - 1

		// Recalculate w from the constraint that x^2 + y^2 + z^2 + w^2 = 1
		double fw = std::sqrt(1.0 - fx * fx - fy * fy - fz * fz);

		// Adjust w based on the stored sign bit
		if (this->wn) {
			fw = -fw;
		}

		return {static_cast<float>(fx), static_cast<float>(fy), static_cast<float>(fz), static_cast<float>(fw)};
	}
};
static_assert(std::is_trivially_copyable_v<QuatCompressed64>);

/// Lower precision Vec3 compressed to 6 bytes
struct Vec3Compressed48 : Vec3ui16 {
	using Vec3ui16::Vec;

	[[nodiscard]] Vec3f decompress() const {
		const float fx = (static_cast<float>((*this)[0]) / 32767.5f) - 1.f;
		const float fy = (static_cast<float>((*this)[1]) / 32767.5f) - 1.f;
		const float fz = (static_cast<float>((*this)[2]) / 32767.5f) - 1.f;
		return {fx, fy, fz};
	}
};
static_assert(std::is_trivially_copyable_v<Vec3Compressed48>);

template<uint8_t M, uint8_t N, Arithmetic P>
class Mat {
	static_assert(M >= 2, "Matrices must have at least two rows!");
	static_assert(N >= 2, "Matrices must have at least two columns!");

public:
	[[nodiscard]] P* operator[](uint8_t i) { return this->data[i]; }

	[[nodiscard]] const P* operator[](uint8_t i) const { return this->data[i]; }

private:
	P data[M][N];
};
static_assert(std::is_trivially_copyable_v<Mat<2, 2, float>>);

#define SOURCEPP_MAT_DEFINE(M, N) \
	template<Arithmetic P> \
	using Mat##M##x##N       = Mat<M, N, P>; \
	using Mat##M##x##N##i8   = Mat##M##x##N<int8_t>; \
	using Mat##M##x##N##i16  = Mat##M##x##N<int16_t>; \
	using Mat##M##x##N##i32  = Mat##M##x##N<int32_t>; \
	using Mat##M##x##N##i64  = Mat##M##x##N<int64_t>; \
	using Mat##M##x##N##i    = Mat##M##x##N##i32; \
	using Mat##M##x##N##ui8  = Mat##M##x##N<uint8_t>; \
	using Mat##M##x##N##ui16 = Mat##M##x##N<uint16_t>; \
	using Mat##M##x##N##ui32 = Mat##M##x##N<uint32_t>; \
	using Mat##M##x##N##ui64 = Mat##M##x##N<uint64_t>; \
	using Mat##M##x##N##ui   = Mat##M##x##N##ui32; \
	using Mat##M##x##N##f16  = Mat##M##x##N<half>; \
	using Mat##M##x##N##f32  = Mat##M##x##N<float>; \
	using Mat##M##x##N##f64  = Mat##M##x##N<double>; \
	using Mat##M##x##N##f    = Mat##M##x##N##f32

SOURCEPP_MAT_DEFINE(2, 2);
SOURCEPP_MAT_DEFINE(3, 3);
SOURCEPP_MAT_DEFINE(4, 4);
SOURCEPP_MAT_DEFINE(2, 3);
SOURCEPP_MAT_DEFINE(3, 2);
SOURCEPP_MAT_DEFINE(2, 4);
SOURCEPP_MAT_DEFINE(4, 2);
SOURCEPP_MAT_DEFINE(3, 4);
SOURCEPP_MAT_DEFINE(4, 3);

#undef SOURCEPP_MAT_DEFINE

} // namespace sourcepp::math
