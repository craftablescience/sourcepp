#pragma once

#include <array>
#include <span>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

#include <sourcepp/Math.h>

namespace kvpp {

namespace DMXValue {

using UUID = std::array<std::byte, 16>;

struct Element {
	int32_t index;
	std::string externalGUID;

	static constexpr int32_t NULL_INDEX = -1;
	static constexpr int32_t EXTERNAL_INDEX = -2;
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

using Vector2 = sourcepp::math::Vec2f;

using Vector3 = sourcepp::math::Vec3f;

using Vector4 = sourcepp::math::Vec4f;

struct EulerAngles : sourcepp::math::EulerAngles {};

struct Quaternion : sourcepp::math::Quat {};

using Matrix4x4 = sourcepp::math::Mat4x4f;

enum class IDv1 : uint8_t {
	INVALID = 0,

	VALUE_START,
	ELEMENT = VALUE_START,
	INT32,
	FLOAT,
	BOOL,
	STRING,
	BYTEARRAY,
	UUID,
	COLOR,
	VECTOR2,
	VECTOR3,
	VECTOR4,
	EULER_ANGLES,
	QUATERNION,
	MATRIX_4X4,
	VALUE_END = MATRIX_4X4,

	ARRAY_START,
	ARRAY_ELEMENT = ARRAY_START,
	ARRAY_INT32,
	ARRAY_FLOAT,
	ARRAY_BOOL,
	ARRAY_STRING,
	ARRAY_BYTEARRAY,
	ARRAY_UUID,
	ARRAY_COLOR,
	ARRAY_VECTOR2,
	ARRAY_VECTOR3,
	ARRAY_VECTOR4,
	ARRAY_EULER_ANGLES,
	ARRAY_QUATERNION,
	ARRAY_MATRIX_4X4,
	ARRAY_END = ARRAY_MATRIX_4X4,
};

enum class IDv2 : uint8_t {
	INVALID = 0,

	VALUE_START,
	ELEMENT = VALUE_START,
	INT32,
	FLOAT,
	BOOL,
	STRING,
	BYTEARRAY,
	TIME,
	COLOR,
	VECTOR2,
	VECTOR3,
	VECTOR4,
	EULER_ANGLES,
	QUATERNION,
	MATRIX_4X4,
	VALUE_END = MATRIX_4X4,

	ARRAY_START,
	ARRAY_ELEMENT = ARRAY_START,
	ARRAY_INT32,
	ARRAY_FLOAT,
	ARRAY_BOOL,
	ARRAY_STRING,
	ARRAY_BYTEARRAY,
	ARRAY_TIME,
	ARRAY_COLOR,
	ARRAY_VECTOR2,
	ARRAY_VECTOR3,
	ARRAY_VECTOR4,
	ARRAY_EULER_ANGLES,
	ARRAY_QUATERNION,
	ARRAY_MATRIX_4X4,
	ARRAY_END = ARRAY_MATRIX_4X4,
};

enum class IDv3 : uint8_t {
	INVALID = 0,

	VALUE_START,
	ELEMENT = VALUE_START,
	INT32,
	FLOAT,
	BOOL,
	STRING,
	BYTEARRAY,
	TIME,
	COLOR,
	VECTOR2,
	VECTOR3,
	VECTOR4,
	EULER_ANGLES,
	QUATERNION,
	MATRIX_4X4,
	UINT64,
	UINT8,
	VALUE_END = UINT8,

	ARRAY_START = 33,
	ARRAY_ELEMENT = ARRAY_START,
	ARRAY_INT32,
	ARRAY_FLOAT,
	ARRAY_BOOL,
	ARRAY_STRING,
	ARRAY_BYTEARRAY,
	ARRAY_TIME,
	ARRAY_COLOR,
	ARRAY_VECTOR2,
	ARRAY_VECTOR3,
	ARRAY_VECTOR4,
	ARRAY_EULER_ANGLES,
	ARRAY_QUATERNION,
	ARRAY_MATRIX_4X4,
	ARRAY_UINT64,
	ARRAY_UINT8,
	ARRAY_END = ARRAY_UINT8,
};

/// Not representative of DMX encoding version, although the attribute type enum variances are linked to that
enum class IDVersion {
	V1,
	V2,
	V3,
};

enum class ID : uint8_t {
	INVALID = 0,

	VALUE_START,
	ELEMENT = VALUE_START,
	INT32,
	FLOAT,
	BOOL,
	STRING,
	BYTEARRAY,
	UUID,
	TIME,
	COLOR,
	VECTOR2,
	VECTOR3,
	VECTOR4,
	EULER_ANGLES,
	QUATERNION,
	MATRIX_4X4,
	UINT64,
	UINT8,
	VALUE_END = UINT8,

	ARRAY_START,
	ARRAY_ELEMENT = ARRAY_START,
	ARRAY_INT32,
	ARRAY_FLOAT,
	ARRAY_BOOL,
	ARRAY_STRING,
	ARRAY_BYTEARRAY,
	ARRAY_UUID,
	ARRAY_TIME,
	ARRAY_COLOR,
	ARRAY_VECTOR2,
	ARRAY_VECTOR3,
	ARRAY_VECTOR4,
	ARRAY_EULER_ANGLES,
	ARRAY_QUATERNION,
	ARRAY_MATRIX_4X4,
	ARRAY_UINT64,
	ARRAY_UINT8,
	ARRAY_END = ARRAY_UINT8,
};

[[nodiscard]] constexpr ID decodeID(IDv1 id) {
	switch (id) {
		case IDv1::INVALID:            return ID::INVALID;
		case IDv1::ELEMENT:            return ID::ELEMENT;
		case IDv1::INT32:              return ID::INT32;
		case IDv1::FLOAT:              return ID::FLOAT;
		case IDv1::BOOL:               return ID::BOOL;
		case IDv1::STRING:             return ID::STRING;
		case IDv1::BYTEARRAY:          return ID::BYTEARRAY;
		case IDv1::UUID:               return ID::UUID;
		case IDv1::COLOR:              return ID::COLOR;
		case IDv1::VECTOR2:            return ID::VECTOR2;
		case IDv1::VECTOR3:            return ID::VECTOR3;
		case IDv1::VECTOR4:            return ID::VECTOR4;
		case IDv1::EULER_ANGLES:       return ID::EULER_ANGLES;
		case IDv1::QUATERNION:         return ID::QUATERNION;
		case IDv1::MATRIX_4X4:         return ID::MATRIX_4X4;
		case IDv1::ARRAY_ELEMENT:      return ID::ARRAY_ELEMENT;
		case IDv1::ARRAY_INT32:        return ID::ARRAY_INT32;
		case IDv1::ARRAY_FLOAT:        return ID::ARRAY_FLOAT;
		case IDv1::ARRAY_BOOL:         return ID::ARRAY_BOOL;
		case IDv1::ARRAY_STRING:       return ID::ARRAY_STRING;
		case IDv1::ARRAY_BYTEARRAY:    return ID::ARRAY_BYTEARRAY;
		case IDv1::ARRAY_UUID:         return ID::ARRAY_UUID;
		case IDv1::ARRAY_COLOR:        return ID::ARRAY_COLOR;
		case IDv1::ARRAY_VECTOR2:      return ID::ARRAY_VECTOR2;
		case IDv1::ARRAY_VECTOR3:      return ID::ARRAY_VECTOR3;
		case IDv1::ARRAY_VECTOR4:      return ID::ARRAY_VECTOR4;
		case IDv1::ARRAY_EULER_ANGLES: return ID::ARRAY_EULER_ANGLES;
		case IDv1::ARRAY_QUATERNION:   return ID::ARRAY_QUATERNION;
		case IDv1::ARRAY_MATRIX_4X4:   return ID::ARRAY_MATRIX_4X4;
	}
	return ID::INVALID;
}

[[nodiscard]] constexpr ID decodeID(IDv2 id) {
	switch (id) {
		case IDv2::INVALID:            return ID::INVALID;
		case IDv2::ELEMENT:            return ID::ELEMENT;
		case IDv2::INT32:              return ID::INT32;
		case IDv2::FLOAT:              return ID::FLOAT;
		case IDv2::BOOL:               return ID::BOOL;
		case IDv2::STRING:             return ID::STRING;
		case IDv2::BYTEARRAY:          return ID::BYTEARRAY;
		case IDv2::TIME:               return ID::TIME;
		case IDv2::COLOR:              return ID::COLOR;
		case IDv2::VECTOR2:            return ID::VECTOR2;
		case IDv2::VECTOR3:            return ID::VECTOR3;
		case IDv2::VECTOR4:            return ID::VECTOR4;
		case IDv2::EULER_ANGLES:       return ID::EULER_ANGLES;
		case IDv2::QUATERNION:         return ID::QUATERNION;
		case IDv2::MATRIX_4X4:         return ID::MATRIX_4X4;
		case IDv2::ARRAY_ELEMENT:      return ID::ARRAY_ELEMENT;
		case IDv2::ARRAY_INT32:        return ID::ARRAY_INT32;
		case IDv2::ARRAY_FLOAT:        return ID::ARRAY_FLOAT;
		case IDv2::ARRAY_BOOL:         return ID::ARRAY_BOOL;
		case IDv2::ARRAY_STRING:       return ID::ARRAY_STRING;
		case IDv2::ARRAY_BYTEARRAY:    return ID::ARRAY_BYTEARRAY;
		case IDv2::ARRAY_TIME:         return ID::ARRAY_TIME;
		case IDv2::ARRAY_COLOR:        return ID::ARRAY_COLOR;
		case IDv2::ARRAY_VECTOR2:      return ID::ARRAY_VECTOR2;
		case IDv2::ARRAY_VECTOR3:      return ID::ARRAY_VECTOR3;
		case IDv2::ARRAY_VECTOR4:      return ID::ARRAY_VECTOR4;
		case IDv2::ARRAY_EULER_ANGLES: return ID::ARRAY_EULER_ANGLES;
		case IDv2::ARRAY_QUATERNION:   return ID::ARRAY_QUATERNION;
		case IDv2::ARRAY_MATRIX_4X4:   return ID::ARRAY_MATRIX_4X4;
	}
	return ID::INVALID;
}

[[nodiscard]] constexpr ID decodeID(IDv3 id) {
	switch (id) {
		case IDv3::INVALID:            return ID::INVALID;
		case IDv3::ELEMENT:            return ID::ELEMENT;
		case IDv3::INT32:              return ID::INT32;
		case IDv3::FLOAT:              return ID::FLOAT;
		case IDv3::BOOL:               return ID::BOOL;
		case IDv3::STRING:             return ID::STRING;
		case IDv3::BYTEARRAY:          return ID::BYTEARRAY;
		case IDv3::TIME:               return ID::TIME;
		case IDv3::COLOR:              return ID::COLOR;
		case IDv3::VECTOR2:            return ID::VECTOR2;
		case IDv3::VECTOR3:            return ID::VECTOR3;
		case IDv3::VECTOR4:            return ID::VECTOR4;
		case IDv3::EULER_ANGLES:       return ID::EULER_ANGLES;
		case IDv3::QUATERNION:         return ID::QUATERNION;
		case IDv3::MATRIX_4X4:         return ID::MATRIX_4X4;
		case IDv3::UINT64:             return ID::UINT64;
		case IDv3::UINT8:              return ID::UINT8;
		case IDv3::ARRAY_ELEMENT:      return ID::ARRAY_ELEMENT;
		case IDv3::ARRAY_INT32:        return ID::ARRAY_INT32;
		case IDv3::ARRAY_FLOAT:        return ID::ARRAY_FLOAT;
		case IDv3::ARRAY_BOOL:         return ID::ARRAY_BOOL;
		case IDv3::ARRAY_STRING:       return ID::ARRAY_STRING;
		case IDv3::ARRAY_BYTEARRAY:    return ID::ARRAY_BYTEARRAY;
		case IDv3::ARRAY_TIME:         return ID::ARRAY_TIME;
		case IDv3::ARRAY_COLOR:        return ID::ARRAY_COLOR;
		case IDv3::ARRAY_VECTOR2:      return ID::ARRAY_VECTOR2;
		case IDv3::ARRAY_VECTOR3:      return ID::ARRAY_VECTOR3;
		case IDv3::ARRAY_VECTOR4:      return ID::ARRAY_VECTOR4;
		case IDv3::ARRAY_EULER_ANGLES: return ID::ARRAY_EULER_ANGLES;
		case IDv3::ARRAY_QUATERNION:   return ID::ARRAY_QUATERNION;
		case IDv3::ARRAY_MATRIX_4X4:   return ID::ARRAY_MATRIX_4X4;
		case IDv3::ARRAY_UINT64:       return ID::ARRAY_UINT64;
		case IDv3::ARRAY_UINT8:        return ID::ARRAY_UINT8;
	}
	return ID::INVALID;
}

[[nodiscard]] constexpr std::byte encodeID(ID id, IDVersion version, bool& incompatible) {
	incompatible = false;
	switch (version) {
		case IDVersion::V1: {
			auto out = IDv1::INVALID;
			switch (id) {
				case ID::INVALID:            break;
				case ID::ELEMENT:            out = IDv1::ELEMENT; break;
				case ID::INT32:              out = IDv1::INT32; break;
				case ID::FLOAT:              out = IDv1::FLOAT; break;
				case ID::BOOL:               out = IDv1::BOOL; break;
				case ID::STRING:             out = IDv1::STRING; break;
				case ID::BYTEARRAY:          out = IDv1::BYTEARRAY; break;
				case ID::UUID:               out = IDv1::UUID; break;
				case ID::TIME:               out = IDv1::STRING; incompatible = true; break;
				case ID::COLOR:              out = IDv1::COLOR; break;
				case ID::VECTOR2:            out = IDv1::VECTOR2; break;
				case ID::VECTOR3:            out = IDv1::VECTOR3; break;
				case ID::VECTOR4:            out = IDv1::VECTOR4; break;
				case ID::EULER_ANGLES:       out = IDv1::EULER_ANGLES; break;
				case ID::QUATERNION:         out = IDv1::QUATERNION; break;
				case ID::MATRIX_4X4:         out = IDv1::MATRIX_4X4; break;
				case ID::UINT64:
				case ID::UINT8:              out = IDv1::STRING; incompatible = true; break;
				case ID::ARRAY_ELEMENT:      out = IDv1::ARRAY_ELEMENT; break;
				case ID::ARRAY_INT32:        out = IDv1::ARRAY_INT32; break;
				case ID::ARRAY_FLOAT:        out = IDv1::ARRAY_FLOAT; break;
				case ID::ARRAY_BOOL:         out = IDv1::ARRAY_BOOL; break;
				case ID::ARRAY_STRING:       out = IDv1::ARRAY_STRING; break;
				case ID::ARRAY_BYTEARRAY:    out = IDv1::ARRAY_BYTEARRAY; break;
				case ID::ARRAY_UUID:         out = IDv1::ARRAY_UUID; break;
				case ID::ARRAY_TIME:         out = IDv1::ARRAY_STRING; incompatible = true; break;
				case ID::ARRAY_COLOR:        out = IDv1::ARRAY_COLOR; break;
				case ID::ARRAY_VECTOR2:      out = IDv1::ARRAY_VECTOR2; break;
				case ID::ARRAY_VECTOR3:      out = IDv1::ARRAY_VECTOR3; break;
				case ID::ARRAY_VECTOR4:      out = IDv1::ARRAY_VECTOR4; break;
				case ID::ARRAY_EULER_ANGLES: out = IDv1::ARRAY_EULER_ANGLES; break;
				case ID::ARRAY_QUATERNION:   out = IDv1::ARRAY_QUATERNION; break;
				case ID::ARRAY_MATRIX_4X4:   out = IDv1::ARRAY_MATRIX_4X4; break;
				case ID::ARRAY_UINT64:
				case ID::ARRAY_UINT8:        out = IDv1::ARRAY_STRING; incompatible = true; break;
			}
			return static_cast<std::byte>(out);
		}
		case IDVersion::V2: {
			auto out = IDv2::INVALID;
			switch (id) {
				case ID::INVALID:            break;
				case ID::ELEMENT:            out = IDv2::ELEMENT; break;
				case ID::INT32:              out = IDv2::INT32; break;
				case ID::FLOAT:              out = IDv2::FLOAT; break;
				case ID::BOOL:               out = IDv2::BOOL; break;
				case ID::STRING:             out = IDv2::STRING; break;
				case ID::BYTEARRAY:          out = IDv2::BYTEARRAY; break;
				case ID::UUID:               out = IDv2::STRING; incompatible = true; break;
				case ID::TIME:               out = IDv2::TIME; break;
				case ID::COLOR:              out = IDv2::COLOR; break;
				case ID::VECTOR2:            out = IDv2::VECTOR2; break;
				case ID::VECTOR3:            out = IDv2::VECTOR3; break;
				case ID::VECTOR4:            out = IDv2::VECTOR4; break;
				case ID::EULER_ANGLES:       out = IDv2::EULER_ANGLES; break;
				case ID::QUATERNION:         out = IDv2::QUATERNION; break;
				case ID::MATRIX_4X4:         out = IDv2::MATRIX_4X4; break;
				case ID::UINT64:
				case ID::UINT8:              out = IDv2::STRING; incompatible = true; break;
				case ID::ARRAY_ELEMENT:      out = IDv2::ARRAY_ELEMENT; break;
				case ID::ARRAY_INT32:        out = IDv2::ARRAY_INT32; break;
				case ID::ARRAY_FLOAT:        out = IDv2::ARRAY_FLOAT; break;
				case ID::ARRAY_BOOL:         out = IDv2::ARRAY_BOOL; break;
				case ID::ARRAY_STRING:       out = IDv2::ARRAY_STRING; break;
				case ID::ARRAY_BYTEARRAY:    out = IDv2::ARRAY_BYTEARRAY; break;
				case ID::ARRAY_UUID:         out = IDv2::STRING; incompatible = true; break;
				case ID::ARRAY_TIME:         out = IDv2::ARRAY_TIME; break;
				case ID::ARRAY_COLOR:        out = IDv2::ARRAY_COLOR; break;
				case ID::ARRAY_VECTOR2:      out = IDv2::ARRAY_VECTOR2; break;
				case ID::ARRAY_VECTOR3:      out = IDv2::ARRAY_VECTOR3; break;
				case ID::ARRAY_VECTOR4:      out = IDv2::ARRAY_VECTOR4; break;
				case ID::ARRAY_EULER_ANGLES: out = IDv2::ARRAY_EULER_ANGLES; break;
				case ID::ARRAY_QUATERNION:   out = IDv2::ARRAY_QUATERNION; break;
				case ID::ARRAY_MATRIX_4X4:   out = IDv2::ARRAY_MATRIX_4X4; break;
				case ID::ARRAY_UINT64:
				case ID::ARRAY_UINT8:        out = IDv2::STRING; incompatible = true; break;
			}
			return static_cast<std::byte>(out);
		}
		case IDVersion::V3: {
			auto out = IDv3::INVALID;
			switch (id) {
				case ID::INVALID:            break;
				case ID::ELEMENT:            out = IDv3::ELEMENT; break;
				case ID::INT32:              out = IDv3::INT32; break;
				case ID::FLOAT:              out = IDv3::FLOAT; break;
				case ID::BOOL:               out = IDv3::BOOL; break;
				case ID::STRING:             out = IDv3::STRING; break;
				case ID::BYTEARRAY:          out = IDv3::BYTEARRAY; break;
				case ID::UUID:               out = IDv3::STRING; incompatible = true; break;
				case ID::TIME:               out = IDv3::TIME; break;
				case ID::COLOR:              out = IDv3::COLOR; break;
				case ID::VECTOR2:            out = IDv3::VECTOR2; break;
				case ID::VECTOR3:            out = IDv3::VECTOR3; break;
				case ID::VECTOR4:            out = IDv3::VECTOR4; break;
				case ID::EULER_ANGLES:       out = IDv3::EULER_ANGLES; break;
				case ID::QUATERNION:         out = IDv3::QUATERNION; break;
				case ID::MATRIX_4X4:         out = IDv3::MATRIX_4X4; break;
				case ID::UINT64:             out = IDv3::UINT64; break;
				case ID::UINT8:              out = IDv3::UINT8; break;
				case ID::ARRAY_ELEMENT:      out = IDv3::ARRAY_ELEMENT; break;
				case ID::ARRAY_INT32:        out = IDv3::ARRAY_INT32; break;
				case ID::ARRAY_FLOAT:        out = IDv3::ARRAY_FLOAT; break;
				case ID::ARRAY_BOOL:         out = IDv3::ARRAY_BOOL; break;
				case ID::ARRAY_STRING:       out = IDv3::ARRAY_STRING; break;
				case ID::ARRAY_BYTEARRAY:    out = IDv3::ARRAY_BYTEARRAY; break;
				case ID::ARRAY_UUID:         out = IDv3::STRING; incompatible = true; break;
				case ID::ARRAY_TIME:         out = IDv3::ARRAY_TIME; break;
				case ID::ARRAY_COLOR:        out = IDv3::ARRAY_COLOR; break;
				case ID::ARRAY_VECTOR2:      out = IDv3::ARRAY_VECTOR2; break;
				case ID::ARRAY_VECTOR3:      out = IDv3::ARRAY_VECTOR3; break;
				case ID::ARRAY_VECTOR4:      out = IDv3::ARRAY_VECTOR4; break;
				case ID::ARRAY_EULER_ANGLES: out = IDv3::ARRAY_EULER_ANGLES; break;
				case ID::ARRAY_QUATERNION:   out = IDv3::ARRAY_QUATERNION; break;
				case ID::ARRAY_MATRIX_4X4:   out = IDv3::ARRAY_MATRIX_4X4; break;
				case ID::ARRAY_UINT64:       out = IDv3::ARRAY_UINT64; break;
				case ID::ARRAY_UINT8:        out = IDv3::ARRAY_UINT8; break;
			}
			return static_cast<std::byte>(out);
		}
	}
	return {};
}

using Generic = std::variant<
	std::monostate,

	Element,
	int32_t,
	float,
	bool,
	std::string,
	std::vector<std::byte>,
	UUID,
	Time,
	Color,
	Vector2,
	Vector3,
	Vector4,
	EulerAngles,
	Quaternion,
	Matrix4x4,
	uint64_t,
	uint8_t,

	std::vector<Element>,
	std::vector<int32_t>,
	std::vector<float>,
	std::vector<bool>,
	std::vector<std::string>,
	std::vector<std::vector<std::byte>>,
	std::vector<UUID>,
	std::vector<Time>,
	std::vector<Color>,
	std::vector<Vector2>,
	std::vector<Vector3>,
	std::vector<Vector4>,
	std::vector<EulerAngles>,
	std::vector<Quaternion>,
	std::vector<Matrix4x4>,
	std::vector<uint64_t>,
	std::vector<uint8_t>
>;

[[nodiscard]] constexpr ID arrayIDToInnerID(ID id) {
	if (id >= ID::ARRAY_START) {
		return static_cast<ID>(static_cast<uint8_t>(id) - static_cast<uint8_t>(ID::VALUE_END));
	}
	return id;
}

[[nodiscard]] constexpr ID innerIDToArrayID(ID id) {
	if (id <= ID::VALUE_END) {
		return static_cast<ID>(static_cast<uint8_t>(id) + static_cast<uint8_t>(ID::VALUE_END));
	}
	return id;
}

[[nodiscard]] std::string idToString(ID id);

// NOLINTNEXTLINE(*-no-recursion)
[[nodiscard]] constexpr ID stringToID(std::string_view id) {
	using enum ID;
	if (id == "element")    return ELEMENT;
	if (id == "int")        return INT32;
	if (id == "float")      return FLOAT;
	if (id == "bool")       return BOOL;
	if (id == "string")     return STRING;
	if (id == "binary")     return BYTEARRAY;
	if (id == "objectid")   return UUID;
	if (id == "time")       return TIME;
	if (id == "color")      return COLOR;
	if (id == "vector2")    return VECTOR2;
	if (id == "vector3")    return VECTOR3;
	if (id == "vector4")    return VECTOR4;
	if (id == "angle")      return EULER_ANGLES;
	if (id == "quaternion") return QUATERNION;
	if (id == "matrix")     return MATRIX_4X4;
	if (id == "uint64")     return UINT64;
	if (id == "uint8")      return UINT8;
	if (id.ends_with("_array")) {
		return innerIDToArrayID(stringToID(id.substr(0, id.length() - 6)));
	}
	return INVALID;
}

} // namespace DMXValue

class DMXAttribute {
	friend class DMXElement;
	friend class DMX;

public:
	/// Check if the given attribute is invalid
	[[nodiscard]] bool isInvalid() const;

	[[nodiscard]] explicit operator bool() const;

	[[nodiscard]] std::string_view getKey() const;

	void setKey(std::string key_);

	[[nodiscard]] DMXValue::ID getValueType() const;

	[[nodiscard]] bool isValueArray() const;

	[[nodiscard]] const DMXValue::Generic& getValue() const;

	template<typename T>
	[[nodiscard]] T getValue() const {
		return std::get<T>(this->value);
	}

	[[nodiscard]] std::string getValueString() const;

	/// Set the value associated with the attribute
	void setValue(DMXValue::Generic value_);

	/// Set the value associated with the attribute
	DMXAttribute& operator=(DMXValue::Generic value_);

protected:
	DMXAttribute() = default;

	std::string key;
	DMXValue::Generic value;
};

class DMXElement {
	friend class DMX;

public:
	[[nodiscard]] explicit operator bool() const;

	/// Get the C++ type the element maps to
	[[nodiscard]] std::string_view getType() const;

	/// Set the C++ type the element maps to
	void setType(std::string type_);

	/// Get the key associated with the element
	[[nodiscard]] std::string_view getKey() const;

	/// Set the key associated with the element
	void setKey(std::string key_);

	// Get the UUID associated with this element
	[[nodiscard]] const DMXValue::UUID& getUUID() const;

	// Set the UUID associated with this element
	void setUUID(const DMXValue::UUID& guid_);

	/// Check if the element has one or more children with the given name
	[[nodiscard]] bool hasAttribute(std::string_view attributeKey) const;

	/// Add an attribute to the element
	DMXAttribute& addAttribute(std::string key_, DMXValue::Generic value_ = {});

	/// Get the number of child attributes
	[[nodiscard]] uint64_t getAttributeCount() const;

	/// Get the number of child attributes with the given key
	[[nodiscard]] uint64_t getAttributeCount(std::string_view childKey) const;

	/// Get the child attributes of the element
	[[nodiscard]] const std::vector<DMXAttribute>& getAttributes() const;

	/// Get the child attributes of the element
	[[nodiscard]] std::vector<DMXAttribute>& getAttributes();

	using iterator = std::vector<DMXAttribute>::iterator;

	[[nodiscard]] constexpr iterator begin() {
		return this->attributes.begin();
	}

	[[nodiscard]] constexpr iterator end() {
		return this->attributes.end();
	}

	using const_iterator = std::vector<DMXAttribute>::const_iterator;

	[[nodiscard]] constexpr const_iterator begin() const {
		return this->attributes.begin();
	}

	[[nodiscard]] constexpr const_iterator end() const {
		return this->attributes.end();
	}

	[[nodiscard]] constexpr const_iterator cbegin() const {
		return this->attributes.cbegin();
	}

	[[nodiscard]] constexpr const_iterator cend() const {
		return this->attributes.cend();
	}

	/// Get the attribute of the element at the given index
	[[nodiscard]] const DMXAttribute& operator[](unsigned int n) const;

	/// Get the attribute of the element at the given index
	[[nodiscard]] DMXAttribute& operator[](unsigned int n);

	/// Get the first attribute of the element with the given key
	[[nodiscard]] const DMXAttribute& operator[](std::string_view attributeKey) const;

	/// Get the first attribute of the element with the given key, or create a new element if it doesn't exist
	[[nodiscard]] DMXAttribute& operator[](std::string_view attributeKey);

	/// Get the first attribute of the element with the given key
	[[nodiscard]] const DMXAttribute& operator()(std::string_view attributeKey) const;

	/// Get the first attribute of the element with the given key, or create a new element if it doesn't exist
	[[nodiscard]] DMXAttribute& operator()(std::string_view attributeKey);

	/// Get the nth attribute of the element with the given key
	[[nodiscard]] const DMXAttribute& operator()(std::string_view attributeKey, unsigned int n) const;

	/// Get the nth attribute of the element with the given key, or create a new element if it doesn't exist
	[[nodiscard]] DMXAttribute& operator()(std::string_view attributeKey, unsigned int n);

	/// Remove an attribute from the element.
	void removeAttribute(unsigned int n);

	/// Remove an attribute from the element with the given key. -1 means all children with the given key
	void removeAttribute(std::string_view attributeKey, int n = -1);

	static const DMXAttribute& getInvalidAttribute();

protected:
	DMXElement() = default;

	std::string type;
	std::string key;
	DMXValue::UUID uuid{};
	std::vector<DMXAttribute> attributes;
};

class DMX {
public:
	enum Encoding {
		ENCODING_INVALID,
		ENCODING_BINARY_OLD,
		ENCODING_BINARY_OLD_SFM,
		ENCODING_BINARY,
		ENCODING_BINARY_SEQIDS,
		ENCODING_KEYVALUES2_OLD,
		ENCODING_KEYVALUES2_FLAT_OLD,
		ENCODING_KEYVALUES2,
		ENCODING_KEYVALUES2_FLAT,
		ENCODING_KEYVALUES2_NOIDS,
	};

	DMX(Encoding encodingType_, int encodingVersion_, std::string formatType_, int formatVersion_);

	explicit DMX(std::span<const std::byte> dmxData);

	explicit DMX(std::string_view dmxData);

	[[nodiscard]] explicit operator bool() const;

	[[nodiscard]] Encoding getEncodingType() const;

	void setEncodingType(Encoding encodingType_);

	[[nodiscard]] bool doesEncodingTypeHaveUnicodePrefix() const;

	void shouldEncodingTypeHaveUnicodePrefix(bool encodingTypeHasUnicodePrefix_);

	[[nodiscard]] int getEncodingVersion() const;

	void setEncodingVersion(int encodingVersion_);

	[[nodiscard]] std::string_view getFormatType() const;

	void setFormatType(std::string formatType_);

	[[nodiscard]] int getFormatVersion() const;

	void setFormatVersion(int formatVersion_);

	DMXElement& addPrefixAttributeContainer();

	/// Get the number of prefix attributes
	[[nodiscard]] uint64_t getPrefixAttributeContainerCount() const;

	[[nodiscard]] const std::vector<DMXElement>& getPrefixAttributeContainers() const;

	[[nodiscard]] std::vector<DMXElement>& getPrefixAttributeContainers();

	void removePrefixAttributeContainer(unsigned int n);

	/// Check if the element list has one or more elements with the given name
	[[nodiscard]] bool hasElement(std::string_view key) const;

	/// Add an element to the element list
	DMXElement& addElement(std::string type, std::string key);

	/// Get the number of elements
	[[nodiscard]] uint64_t getElementCount() const;

	/// Get the number of elements with the given key
	[[nodiscard]] uint64_t getElementCount(std::string_view key) const;

	[[nodiscard]] const std::vector<DMXElement>& getElements() const;

	[[nodiscard]] std::vector<DMXElement>& getElements();

	using iterator = std::vector<DMXElement>::iterator;

	[[nodiscard]] constexpr iterator begin() {
		return this->elements.begin();
	}

	[[nodiscard]] constexpr iterator end() {
		return this->elements.end();
	}

	using const_iterator = std::vector<DMXElement>::const_iterator;

	[[nodiscard]] constexpr const_iterator begin() const {
		return this->elements.begin();
	}

	[[nodiscard]] constexpr const_iterator end() const {
		return this->elements.end();
	}

	[[nodiscard]] constexpr const_iterator cbegin() const {
		return this->elements.cbegin();
	}

	[[nodiscard]] constexpr const_iterator cend() const {
		return this->elements.cend();
	}

	/// Get the element in the element list at the given index
	[[nodiscard]] const DMXElement& operator[](unsigned int n) const;

	/// Get the element in the element list at the given index
	[[nodiscard]] DMXElement& operator[](unsigned int n);

	/// Get the first element in the element list with the given key
	[[nodiscard]] const DMXElement& operator[](std::string_view key) const;

	/// Get the first element in the element list with the given key, or create a new element if it doesn't exist
	[[nodiscard]] DMXElement& operator[](std::string_view key);

	/// Get the first element in the element list with the given key
	[[nodiscard]] const DMXElement& operator()(std::string_view key) const;

	/// Get the first element in the element list with the given key, or create a new element if it doesn't exist
	[[nodiscard]] DMXElement& operator()(std::string_view key);

	/// Get the nth element in the element list with the given key
	[[nodiscard]] const DMXElement& operator()(std::string_view key, unsigned int n) const;

	/// Get the nth element in the element list with the given key, or create a new element if it doesn't exist
	[[nodiscard]] DMXElement& operator()(std::string_view key, unsigned int n);

	/// Remove an element from the element list and update all element references
	void removeElement(unsigned int n);

	[[nodiscard]] std::vector<std::byte> bake() const;

	void bake(const std::string& dmxPath) const;

	[[nodiscard]] static constexpr bool isEncodingVersionValid(Encoding encodingType, int encodingVersion) {
		switch (encodingType) {
			case ENCODING_INVALID:
				break;
			case ENCODING_BINARY_OLD:
				return encodingVersion >= 1 && encodingVersion <= 2;
			case ENCODING_BINARY_OLD_SFM:
				return encodingVersion >= 1 && encodingVersion <= 9;
			case ENCODING_BINARY:
			case ENCODING_BINARY_SEQIDS:
				return (encodingVersion >= 1 && encodingVersion <= 5) || encodingVersion == 9;
			case ENCODING_KEYVALUES2_OLD:
			case ENCODING_KEYVALUES2_FLAT_OLD:
				return encodingVersion == 1;
			case ENCODING_KEYVALUES2:
			case ENCODING_KEYVALUES2_FLAT:
				return encodingVersion >= 1 && encodingVersion <= 4;
			case ENCODING_KEYVALUES2_NOIDS:
				return encodingVersion == 1;
		}
		return false;
	}

	[[nodiscard]] static DMXValue::UUID createRandomUUID();

	[[nodiscard]] static const DMXElement& getInvalidElement();

protected:
	Encoding encodingType = ENCODING_INVALID;
	bool encodingTypeHasUnicodePrefix = false;
	int encodingVersion = -1;
	std::string formatType;
	int formatVersion = -1;

	std::vector<DMXElement> prefixAttributeContainers;
	std::vector<DMXElement> elements;
};

namespace literals {

DMX operator""_kv2(const char* str, std::size_t len);

} // namespace literals

} // namespace kvpp
