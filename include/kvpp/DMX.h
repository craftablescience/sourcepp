#pragma once

#include <array>
#include <span>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

#include <sourcepp/Math.h>

namespace kvpp {

using DMXGUID = std::array<std::byte, 16>;

namespace DMXValue {

struct Element {
	uint32_t index;
	std::string externalGUID;
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

using Generic = std::variant<
	std::monostate,

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
	EulerAngles,
	Quaternion,
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
	std::vector<EulerAngles>,
	std::vector<Quaternion>,
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
	EULER_ANGLES = 12,
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
	ARRAY_EULER_ANGLES = 26,
	ARRAY_QUATERNION = 27,
	ARRAY_MATRIX_4X4 = 28,
	ARRAY_END = 28,
};

[[nodiscard]] constexpr std::byte IDToByte(ID id) {
	return static_cast<std::byte>(id);
}

[[nodiscard]] constexpr ID byteToID(std::byte id) {
	return static_cast<ID>(id);
}

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

[[nodiscard]] std::string IDToString(ID id);

// NOLINTNEXTLINE(*-no-recursion)
[[nodiscard]] constexpr ID stringToID(std::string_view id) {
	using enum ID;
	if (id == "element")    return ELEMENT;
	if (id == "int")        return INT;
	if (id == "float")      return FLOAT;
	if (id == "bool")       return BOOL;
	if (id == "string")     return STRING;
	if (id == "binary")     return BYTEARRAY;
	if (id == "time")       return TIME;
	if (id == "color")      return COLOR;
	if (id == "vector2")    return VECTOR2;
	if (id == "vector3")    return VECTOR3;
	if (id == "vector4")    return VECTOR4;
	if (id == "angle")      return EULER_ANGLES;
	if (id == "quaternion") return QUATERNION;
	if (id == "matrix")     return MATRIX_4X4;
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

	[[nodiscard]] bool isArray() const;

	[[nodiscard]] DMXValue::ID getValueType() const;

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

	// Get the GUID associated with this element
	[[nodiscard]] const DMXGUID& getGUID() const;

	// Set the GUID associated with this element
	void setGUID(const DMXGUID& guid_);

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
	DMXGUID guid{};
	std::vector<DMXAttribute> attributes;
};

class DMX {
public:
	enum Encoding {
		ENCODING_INVALID,
		ENCODING_BINARY_OLD,
		ENCODING_BINARY_OLD_SFM,
		ENCODING_BINARY,
		ENCODING_BINARY_UTF8,
		ENCODING_KEYVALUES2_OLD,
		ENCODING_KEYVALUES2,
		ENCODING_KEYVALUES2_UTF8,
		ENCODING_KEYVALUES2_FLAT_OLD,
		ENCODING_KEYVALUES2_FLAT,
		ENCODING_KEYVALUES2_NOGUIDS,
	};

	DMX(Encoding encodingType_, int encodingVersion_, std::string formatType_, int formatVersion_);

	explicit DMX(std::span<const std::byte> dmxData);

	explicit DMX(std::string_view dmxData);

	[[nodiscard]] explicit operator bool() const;

	[[nodiscard]] Encoding getEncodingType() const;

	void setEncodingType(Encoding encodingType_);

	[[nodiscard]] int getEncodingVersion() const;

	void setEncodingVersion(int encodingVersion_);

	[[nodiscard]] std::string_view getFormatType() const;

	void setFormatType(std::string formatType_);

	[[nodiscard]] int getFormatVersion() const;

	void setFormatVersion(int formatVersion_);

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

	[[nodiscard]] static bool isEncodingVersionValid(Encoding encodingType, int encodingVersion);

	[[nodiscard]] static DMXGUID createRandomGUID();

	[[nodiscard]] static const DMXElement& getInvalidElement();

protected:
	Encoding encodingType = ENCODING_INVALID;
	int encodingVersion = -1;
	std::string formatType;
	int formatVersion = -1;

	std::vector<DMXElement> elements;
};

namespace literals {

DMX operator""_kv2(const char* str, std::size_t len);

} // namespace literals

} // namespace kvpp
