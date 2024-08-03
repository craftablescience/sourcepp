#pragma once

#include <array>
#include <cmath>

#include "Integer.h"

namespace sourcepp::math {

template<uint8_t S, Arithmetic P>
struct Vec {
	static_assert(S >= 2, "Vectors must have at least two values!");

	std::array<P, S> values;

	// By defining these constructors, the type becomes nontrivial...
#if 0
	constexpr Vec() = default;

	constexpr explicit Vec(values_type vals)
			: values{vals} {}

	template<std::convertible_to<P>... Vals>
	requires (sizeof...(Vals) == S)
	constexpr Vec(Vals... vals) // NOLINT(*-explicit-constructor)
			: values{static_cast<P>(vals)...} {}
#endif

	using value_type = P;

	[[nodiscard]] consteval uint8_t size() const {
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

template<Arithmetic P>
using Vec2 = Vec<2, P>;

using Vec2i8 = Vec2<int8_t>;
using Vec2i16 = Vec2<int16_t>;
using Vec2i32 = Vec2<int32_t>;
using Vec2i64 = Vec2<int64_t>;
using Vec2i = Vec2i32;

using Vec2ui8 = Vec2<uint8_t>;
using Vec2ui16 = Vec2<uint16_t>;
using Vec2ui32 = Vec2<uint32_t>;
using Vec2ui64 = Vec2<uint64_t>;
using Vec2ui = Vec2ui32;

using Vec2f32 = Vec2<float>;
using Vec2f64 = Vec2<double>;
using Vec2f = Vec2f32;

template<Arithmetic P>
using Vec3 = Vec<3, P>;

using Vec3i8 = Vec3<int8_t>;
using Vec3i16 = Vec3<int16_t>;
using Vec3i32 = Vec3<int32_t>;
using Vec3i64 = Vec3<int64_t>;
using Vec3i = Vec3i32;

using Vec3ui8 = Vec3<uint8_t>;
using Vec3ui16 = Vec3<uint16_t>;
using Vec3ui32 = Vec3<uint32_t>;
using Vec3ui64 = Vec3<uint64_t>;
using Vec3ui = Vec3ui32;

using Vec3f32 = Vec3<float>;
using Vec3f64 = Vec3<double>;
using Vec3f = Vec3f32;

template<Arithmetic P>
using Vec4 = Vec<4, P>;

using Vec4i8 = Vec4<int8_t>;
using Vec4i16 = Vec4<int16_t>;
using Vec4i32 = Vec4<int32_t>;
using Vec4i64 = Vec4<int64_t>;
using Vec4i = Vec4i32;

using Vec4ui8 = Vec4<uint8_t>;
using Vec4ui16 = Vec4<uint16_t>;
using Vec4ui32 = Vec4<uint32_t>;
using Vec4ui64 = Vec4<uint64_t>;
using Vec4ui = Vec4ui32;

using Vec4f32 = Vec4<float>;
using Vec4f64 = Vec4<double>;
using Vec4f = Vec4f32;

} // namespace sourcepp::math
