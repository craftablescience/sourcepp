#pragma once

#include <cmath>

#include "Integer.h"

namespace sourcepp::math {

template<Arithmetic P>
struct Vec2 {
	P x;
	P y;

	[[nodiscard]] Vec2 operator+(const Vec2& other) const {
		return {this->x + other.x, this->y + other.y};
	}

	void operator+=(const Vec2& other) {
		this->x += other.x;
		this->y += other.y;
	}

	[[nodiscard]] Vec2 operator-(const Vec2& other) const {
		return {this->x - other.x, this->y - other.y};
	}

	void operator-=(const Vec2& other) {
		this->x -= other.x;
		this->y -= other.y;
	}

	[[nodiscard]] Vec2 operator*(Arithmetic auto scalar) const {
		return {this->x * scalar, this->y * scalar};
	}

	void operator*=(Arithmetic auto scalar) {
		this->x *= scalar;
		this->y *= scalar;
	}

	[[nodiscard]] Vec2 operator/(Arithmetic auto scalar) const {
		return {this->x / scalar, this->y / scalar};
	}

	void operator/=(Arithmetic auto scalar) {
		this->x /= scalar;
		this->y /= scalar;
	}

	[[nodiscard]] bool operator==(const Vec2& other) const {
		return this->x == other.x && this->y == other.y;
	}

	[[nodiscard]] Vec2 mul(const Vec2& other) const {
		return {this->x * other.x, this->y * other.y};
	}

	[[nodiscard]] Vec2 div(const Vec2& other) const {
		return {this->x / other.x, this->y / other.y};
	}

	[[nodiscard]] Vec2 mod(const Vec2& other) const {
		if constexpr (std::floating_point<P>) {
			return {std::fmod(this->x, other.x), std::fmod(this->y, other.y)};
		} else {
			return {this->x % other.x, this->y % other.y};
		}
	}

	[[nodiscard]] Vec2 abs() const {
		return {std::abs(this->x), std::abs(this->y)};
	}

	[[nodiscard]] static constexpr Vec2 zero() {
		return {{}, {}};
	}
};

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
struct Vec3 {
	P x;
	P y;
	P z;

	[[nodiscard]] Vec3 operator+(const Vec3& other) const {
		return {this->x + other.x, this->y + other.y, this->z + other.z};
	}

	void operator+=(const Vec3& other) {
		this->x += other.x;
		this->y += other.y;
		this->z += other.z;
	}

	[[nodiscard]] Vec3 operator-(const Vec3& other) const {
		return {this->x - other.x, this->y - other.y, this->z - other.z};
	}

	void operator-=(const Vec3& other) {
		this->x -= other.x;
		this->y -= other.y;
		this->z -= other.z;
	}

	[[nodiscard]] Vec3 operator*(Arithmetic auto scalar) const {
		return {this->x * scalar, this->y * scalar, this->z * scalar};
	}

	void operator*=(Arithmetic auto scalar) {
		this->x *= scalar;
		this->y *= scalar;
		this->z *= scalar;
	}

	[[nodiscard]] Vec3 operator/(Arithmetic auto scalar) const {
		return {this->x / scalar, this->y / scalar, this->z / scalar};
	}

	void operator/=(Arithmetic auto scalar) {
		this->x /= scalar;
		this->y /= scalar;
		this->z /= scalar;
	}

	[[nodiscard]] bool operator==(const Vec3& other) const {
		return this->x == other.x && this->y == other.y && this->z == other.z;
	}

	[[nodiscard]] Vec3 mul(const Vec3& other) const {
		return {this->x * other.x, this->y * other.y, this->z * other.z};
	}

	[[nodiscard]] Vec3 div(const Vec3& other) const {
		return {this->x / other.x, this->y / other.y, this->z / other.z};
	}

	[[nodiscard]] Vec3 mod(const Vec3& other) const {
		if constexpr (std::floating_point<P>) {
			return {std::fmod(this->x, other.x), std::fmod(this->y, other.y), std::fmod(this->z, other.z)};
		} else {
			return {this->x % other.x, this->y % other.y, this->z % other.z};
		}
	}

	[[nodiscard]] Vec3 abs() const {
		return {std::abs(this->x), std::abs(this->y), std::abs(this->z)};
	}

	[[nodiscard]] static constexpr Vec3 zero() {
		return {{}, {}, {}};
	}
};

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
struct Vec4 {
	P x;
	P y;
	P z;
	P w;

	[[nodiscard]] Vec4 operator+(const Vec4& other) const {
		return {this->x + other.x, this->y + other.y, this->z + other.z, this->w + other.w};
	}

	void operator+=(const Vec4& other) {
		this->x += other.x;
		this->y += other.y;
		this->z += other.z;
		this->w += other.w;
	}

	[[nodiscard]] Vec4 operator-(const Vec4& other) const {
		return {this->x - other.x, this->y - other.y, this->z - other.z, this->w - other.w};
	}

	void operator-=(const Vec4& other) {
		this->x -= other.x;
		this->y -= other.y;
		this->z -= other.z;
		this->w -= other.w;
	}

	[[nodiscard]] Vec4 operator*(Arithmetic auto scalar) const {
		return {this->x * scalar, this->y * scalar, this->z * scalar, this->w * scalar};
	}

	void operator*=(Arithmetic auto scalar) {
		this->x *= scalar;
		this->y *= scalar;
		this->z *= scalar;
		this->w *= scalar;
	}

	[[nodiscard]] Vec4 operator/(Arithmetic auto scalar) const {
		return {this->x / scalar, this->y / scalar, this->z / scalar, this->w / scalar};
	}

	void operator/=(Arithmetic auto scalar) {
		this->x /= scalar;
		this->y /= scalar;
		this->z /= scalar;
		this->w /= scalar;
	}

	[[nodiscard]] bool operator==(const Vec4& other) const {
		return this->x == other.x && this->y == other.y && this->z == other.z && this->w == other.w;
	}

	[[nodiscard]] Vec4 mul(const Vec4& other) const {
		return {this->x * other.x, this->y * other.y, this->z * other.z, this->z * other.z};
	}

	[[nodiscard]] Vec4 div(const Vec4& other) const {
		return {this->x / other.x, this->y / other.y, this->z / other.z, this->w / other.w};
	}

	[[nodiscard]] Vec4 mod(const Vec4& other) const {
		if constexpr (std::floating_point<P>) {
			return {std::fmod(this->x, other.x), std::fmod(this->y, other.y), std::fmod(this->z, other.z), std::fmod(this->w, other.w)};
		} else {
			return {this->x % other.x, this->y % other.y, this->z % other.z, this->w % other.w};
		}
	}

	[[nodiscard]] Vec4 abs() const {
		return {std::abs(this->x), std::abs(this->y), std::abs(this->z), std::abs(this->w)};
	}

	[[nodiscard]] static constexpr Vec4 zero() {
		return {{}, {}, {}, {}};
	}
};

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
