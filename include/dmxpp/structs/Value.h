#pragma once

#include <array>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <type_traits>
#include <variant>
#include <vector>

#include <sourcepp/math/Angles.h>
#include <sourcepp/math/Integer.h>
#include <sourcepp/math/Matrix.h>
#include <sourcepp/math/Vector.h>

namespace dmxpp {

namespace Value {

struct Invalid {};

struct Element {
	uint32_t index;
	std::string stubGUID;
};

using ByteArray = std::vector<std::byte>;

struct Time {
	float seconds;
};

struct Color {
	uint8_t r;
	uint8_t g;
	uint8_t b;
	uint8_t a;
};

using Vector2 = sourcepp::Vec2f;

using Vector3 = sourcepp::Vec3f;

using Vector4 = sourcepp::Vec4f;

using EulerAngles = sourcepp::EulerAngles;

using Quaternion = sourcepp::Quat;

using Matrix4x4 = sourcepp::Matrix<4,4>;

using Generic = std::variant<
	Invalid,
	Element,
	int32_t,
	float,
	bool,
	std::string,
	std::vector<std::byte>,
	Time,
	Color,
	Vector2,
	Vector3,
	Vector4,
	//EulerAngle, // Same as Vector3
	//Quaternion, // Same as Vector4
	Matrix4x4,

	std::vector<Element>,
	std::vector<int32_t>,
	std::vector<float>,
	std::vector<bool>,
	std::vector<std::string>,
	std::vector<std::vector<std::byte>>,
	std::vector<Time>,
	std::vector<Color>,
	std::vector<Vector2>,
	std::vector<Vector3>,
	std::vector<Vector4>,
	//std::vector<EulerAngle>,
	//std::vector<Quaternion>,
	std::vector<Matrix4x4>
>;

enum class ID : uint8_t {
	INVALID = 0,

	VALUE_START = 1,
	ELEMENT = 1,
	INT = 2,
	FLOAT = 3,
	BOOL = 4,
	STRING = 5,
	BYTEARRAY = 6,
	TIME = 7, // OBJECT_ID in v1 & v2
	COLOR = 8,
	VECTOR2 = 9,
	VECTOR3 = 10,
	VECTOR4 = 11,
	EULER_ANGLE = 12,
	QUATERNION = 13,
	MATRIX_4X4 = 14,
	VALUE_END = 14,

	ARRAY_START = 15,
	ARRAY_ELEMENT = 15,
	ARRAY_INT = 16,
	ARRAY_FLOAT = 17,
	ARRAY_BOOL = 18,
	ARRAY_STRING = 19,
	ARRAY_BYTEARRAY = 20,
	ARRAY_TIME = 21, // ARRAY_OBJECT_ID in v1 & v2
	ARRAY_COLOR = 22,
	ARRAY_VECTOR2 = 23,
	ARRAY_VECTOR3 = 24,
	ARRAY_VECTOR4 = 25,
	ARRAY_EULER_ANGLE = 26,
	ARRAY_QUATERNION = 27,
	ARRAY_MATRIX_4X4 = 28,
	ARRAY_END = 28,
};

constexpr std::byte IDToByte(ID id) {
	return static_cast<std::byte>(id);
}

constexpr ID byteToID(std::byte id) {
	return static_cast<ID>(id);
}

constexpr ID arrayIDToInnerID(ID id) {
	if (id >= ID::ARRAY_START) {
		return static_cast<ID>(static_cast<uint8_t>(id) - static_cast<uint8_t>(ID::VALUE_END));
	}
	return id;
}

constexpr ID innerIDToArrayID(ID id) {
	if (id <= ID::VALUE_END) {
		return static_cast<ID>(static_cast<uint8_t>(id) + static_cast<uint8_t>(ID::VALUE_END));
	}
	return id;
}

std::string IDToString(ID id);

// NOLINTNEXTLINE(*-no-recursion)
constexpr ID stringToID(std::string_view id) {
	if (id == "element") {
		return ID::ELEMENT;
	} else if (id == "int") {
		return ID::INT;
	} else if (id == "float") {
		return ID::FLOAT;
	} else if (id == "bool") {
		return ID::BOOL;
	} else if (id == "string") {
		return ID::STRING;
	} else if (id == "binary") {
		return ID::BYTEARRAY;
	} else if (id == "time") {
		return ID::TIME;
	} else if (id == "color") {
		return ID::COLOR;
	} else if (id == "vector2") {
		return ID::VECTOR2;
	} else if (id == "vector3") {
		return ID::VECTOR3;
	} else if (id == "vector4") {
		return ID::VECTOR4;
	} else if (id == "quaternion") {
		return ID::QUATERNION;
	} else if (id == "matrix") {
		return ID::MATRIX_4X4;
	} else if (id.ends_with("_array")) {
		return innerIDToArrayID(stringToID(id.substr(0, id.length() - 6)));
	}
	return ID::INVALID;
}

} // namespace Value

struct Attribute {
	std::string name;
	Value::ID type;
	Value::Generic value;

	[[nodiscard]] bool isArray() const;

	[[nodiscard]] std::string getValue() const;

	template<typename T>
	[[nodiscard]] T getValueAs() const {
		return std::get<T>(this->value);
	}
};

struct Element {
	std::string type;
	std::string name;
	std::array<std::byte, 16> guid;
	std::vector<Attribute> attributes;
};

} // namespace dmxpp
