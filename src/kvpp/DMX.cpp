// ReSharper disable CppRedundantQualifier

#include <kvpp/DMX.h>

#include <format>
#include <limits>
#include <random>
#include <sstream>
#include <utility>

#include <BufferStream.h>
#include <iomanip>
#include <sourcepp/FS.h>

using namespace kvpp;
using namespace sourcepp;

// NOLINTNEXTLINE(*-no-recursion)
std::string DMXValue::idToString(ID id) {
	switch (id) {
		case ID::INVALID: break;
		case ID::ELEMENT:      return "element";
		case ID::INT32:        return "int";
		case ID::FLOAT:        return "float";
		case ID::BOOL:         return "bool";
		case ID::STRING:       return "string";
		case ID::BYTEARRAY:    return "binary";
		case ID::UUID:         return "elementid";
		case ID::TIME:         return "time";
		case ID::COLOR:        return "color";
		case ID::VECTOR2:      return "vector2";
		case ID::VECTOR3:      return "vector3";
		case ID::VECTOR4:      return "vector4";
		case ID::EULER_ANGLES: return "angle";
		case ID::QUATERNION:   return "quaternion";
		case ID::MATRIX_4X4:   return "matrix";
		case ID::UINT64:       return "uint64";
		case ID::UINT8:        return "uint8";
		case ID::ARRAY_ELEMENT:
		case ID::ARRAY_INT32:
		case ID::ARRAY_FLOAT:
		case ID::ARRAY_BOOL:
		case ID::ARRAY_STRING:
		case ID::ARRAY_BYTEARRAY:
		case ID::ARRAY_UUID:
		case ID::ARRAY_TIME:
		case ID::ARRAY_COLOR:
		case ID::ARRAY_VECTOR2:
		case ID::ARRAY_VECTOR3:
		case ID::ARRAY_VECTOR4:
		case ID::ARRAY_EULER_ANGLES:
		case ID::ARRAY_QUATERNION:
		case ID::ARRAY_MATRIX_4X4:
		case ID::ARRAY_UINT64:
		case ID::ARRAY_UINT8:
			return idToString(arrayIDToInnerID(id)) + "_array";
	}
	return "invalid";
}

bool DMXAttribute::isInvalid() const {
	return static_cast<DMXValue::ID>(this->value.index()) == DMXValue::ID::INVALID;
}

DMXAttribute::operator bool() const {
	return !this->isInvalid();
}

std::string_view DMXAttribute::getKey() const {
	return this->key;
}

void DMXAttribute::setKey(std::string key_) {
	this->key = std::move(key_);
}

DMXValue::ID DMXAttribute::getValueType() const {
	return static_cast<DMXValue::ID>(this->value.index());
}

bool DMXAttribute::isValueArray() const {
	return this->getValueType() >= DMXValue::ID::ARRAY_START;
}

const DMXValue::Generic& DMXAttribute::getValue() const {
	return this->value;
}

std::string DMXAttribute::getValueString() const {
	switch (const auto type = this->getValueType()) {
		using enum DMXValue::ID;
		case INVALID:
			return DMXValue::idToString(type);
		case ELEMENT: {
			const auto [index, stubGUID] = this->getValue<DMXValue::Element>();
			if (index == -2) {
				return "GUID: " + stubGUID;
			}
			return '#' + std::to_string(index);
		}
		case INT32:
			return std::format("{}", this->getValue<int32_t>());
		case FLOAT:
			return std::format("{}", this->getValue<float>());
		case BOOL:
			return std::format("{}", static_cast<int>(this->getValue<bool>()));
		case STRING:
			return this->getValue<std::string>();
		case BYTEARRAY: {
			std::stringstream hex;
			hex << std::hex << std::setfill('0');
			for (auto byte : this->getValue<DMXValue::ByteArray>()) {
				hex << std::setw(2) << static_cast<unsigned char>(byte);
			}
			return hex.str();
		}
		case UUID: {
			std::stringstream hex;
			hex << std::hex << std::setfill('0');
			for (auto byte : this->getValue<DMXValue::UUID>()) {
				hex << std::setw(2) << static_cast<unsigned char>(byte);
			}
			return hex.str();
		}
		case TIME:
			return std::to_string(this->getValue<float>());
		case COLOR: {
			const auto [r, g, b, a] = this->getValue<DMXValue::Color>();
			return "rgba(" + std::to_string(r) + ", " + std::to_string(g) + ", " + std::to_string(b) + ", " + std::to_string(a) + ')';
		}
		case VECTOR2: {
			const auto vec2 = this->getValue<DMXValue::Vector2>();
			return '[' + std::to_string(vec2[0]) + ", " + std::to_string(vec2[1]) + ']';
		}
		case VECTOR3: {
			const auto vec3 = this->getValue<DMXValue::Vector3>();
			return '[' + std::to_string(vec3[0]) + ", " + std::to_string(vec3[1]) + ", " + std::to_string(vec3[2]) + ']';
		}
		case VECTOR4: {
			const auto vec4 = this->getValue<DMXValue::Vector4>();
			return '[' + std::to_string(vec4[0]) + ", " + std::to_string(vec4[1]) + ", " + std::to_string(vec4[2]) + ", " + std::to_string(vec4[3]) + ']';
		}
		case EULER_ANGLES: {
			const auto angles = this->getValue<DMXValue::EulerAngles>();
			return '[' + std::to_string(angles[0]) + ", " + std::to_string(angles[1]) + ", " + std::to_string(angles[2]) + ']';
		}
		case QUATERNION: {
			const auto quat = this->getValue<DMXValue::Quaternion>();
			return '[' + std::to_string(quat[0]) + ", " + std::to_string(quat[1]) + ", " + std::to_string(quat[2]) + ", " + std::to_string(quat[3]) + ']';
		}
		case MATRIX_4X4: {
			const auto mat4 = this->getValue<DMXValue::Matrix4x4>();
			std::string out;
			for (int i = 0; i < 4; i++) {
				out += i == 0 ? '[' : ' ';
				for (int j = 0; j < 4; j++) {
					out += std::to_string(mat4[i][j]);
					if (j < 3) {
						out += ", ";
					} else if (i < 3) {
						out += ",\n";
					} else {
						out += ']';
					}
				}
			}
			return out;
		}
		case UINT64:
			return std::format("{}", this->getValue<uint64_t>());
		case UINT8:
			return std::format("{}", static_cast<int>(this->getValue<uint8_t>()));
		case ARRAY_ELEMENT: {
			const auto elements = this->getValue<std::vector<DMXValue::Element>>();
			std::string out = "[";
			for (int i = 0; i < elements.size(); i++) {
				if (elements[i].index == -2) {
					out += (i == 0 ? "" : " ") + std::string{"GUID: "} + elements[i].externalGUID + (i == elements.size() - 1 ? "" : ",");
				} else {
					out += (i == 0 ? "" : " ") + std::string{"#"} + std::to_string(elements[i].index) + (i == elements.size() - 1 ? "" : ",");
				}
			}
			return out + ']';
		}
		case ARRAY_INT32: {
			const auto ints = this->getValue<std::vector<int32_t>>();
			std::string out = "[";
			for (int i = 0; i < ints.size(); i++) {
				out += (i == 0 ? "" : " ") + std::to_string(ints[i]) + (i == ints.size() - 1 ? "" : ",");
			}
			return out + ']';
		}
		case ARRAY_FLOAT: {
			const auto floats = this->getValue<std::vector<float>>();
			std::string out = "[";
			for (int i = 0; i < floats.size(); i++) {
				out += (i == 0 ? "" : " ") + std::to_string(floats[i]) + (i == floats.size() - 1 ? "" : ",");
			}
			return out + ']';
		}
		case ARRAY_BOOL: {
			const auto bools = this->getValue<std::vector<bool>>();
			std::string out = "[";
			for (int i = 0; i < bools.size(); i++) {
				out += (i == 0 ? "" : " ") + std::string{bools[i] ? "true" : "false"} + (i == bools.size() - 1 ? "" : ",");
			}
			return out + ']';
		}
		case ARRAY_STRING: {
			const auto strings = this->getValue<std::vector<std::string>>();
			std::string out = "[";
			for (int i = 0; i < strings.size(); i++) {
				out += (i == 0 ? "" : " ") + strings[i] + (i == strings.size() - 1 ? "" : ",\n");
			}
			return out + ']';
		}
		case ARRAY_BYTEARRAY: {
			const auto bytearrays = this->getValue<std::vector<DMXValue::ByteArray>>();
			std::string out = "[";
			for (int i = 0; i < bytearrays.size(); i++) {
				std::stringstream hex;
				hex << std::hex << std::setfill('0');
				for (auto byte : bytearrays[i]) {
					hex << std::setw(2) << static_cast<unsigned char>(byte);
				}
				out += (i == 0 ? "" : " ") + hex.str() + (i == bytearrays.size() - 1 ? "" : ",\n");
			}
			return out + ']';
		}
		case ARRAY_UUID: {
			const auto uuids = this->getValue<std::vector<DMXValue::UUID>>();
			std::string out = "[";
			for (int i = 0; i < uuids.size(); i++) {
				std::stringstream hex;
				hex << std::hex << std::setfill('0');
				for (auto byte : uuids[i]) {
					hex << std::setw(2) << static_cast<unsigned char>(byte);
				}
				out += (i == 0 ? "" : " ") + hex.str() + (i == uuids.size() - 1 ? "" : ",\n");
			}
			return out + ']';
		}
		case ARRAY_TIME: {
			const auto times = this->getValue<std::vector<float>>();
			std::string out = "[";
			for (int i = 0; i < times.size(); i++) {
				out += (i == 0 ? "" : " ") + std::to_string(times[i]) + (i == times.size() - 1 ? "" : ",");
			}
			return out + ']';
		}
		case ARRAY_COLOR: {
			const auto colors = this->getValue<std::vector<DMXValue::Color>>();
			std::string out = "[";
			for (int i = 0; i < colors.size(); i++) {
				out += (i == 0 ? "" : " ") + std::string{"rgba("} + std::to_string(colors[i].r) + ", " + std::to_string(colors[i].g) + ", " + std::to_string(colors[i].b) + ", " + std::to_string(colors[i].a) + ')' + (i == colors.size() - 1 ? "" : ",\n");
			}
			return out + ']';
		}
		case ARRAY_VECTOR2: {
			const auto vecs = this->getValue<std::vector<DMXValue::Vector2>>();
			std::string out = "[";
			for (int i = 0; i < vecs.size(); i++) {
				out += (i == 0 ? "" : " ") + std::string{"["} + std::to_string(vecs[i][0]) + ", " + std::to_string(vecs[i][1]) + ']' + (i == vecs.size() - 1 ? "" : ",\n");
			}
			return out + ']';
		}
		case ARRAY_VECTOR3: {
			const auto vecs = this->getValue<std::vector<DMXValue::Vector3>>();
			std::string out = "[";
			for (int i = 0; i < vecs.size(); i++) {
				out += (i == 0 ? "" : " ") + std::string{"["} + std::to_string(vecs[i][0]) + ", " + std::to_string(vecs[i][1]) + ", " + std::to_string(vecs[i][2]) + ']' + (i == vecs.size() - 1 ? "" : ",\n");
			}
			return out + ']';
		}
		case ARRAY_VECTOR4: {
			const auto vecs = this->getValue<std::vector<DMXValue::Vector4>>();
			std::string out = "[";
			for (int i = 0; i < vecs.size(); i++) {
				out += (i == 0 ? "" : " ") + std::string{"["} + std::to_string(vecs[i][0]) + ", " + std::to_string(vecs[i][1]) + ", " + std::to_string(vecs[i][2]) + ']' + ", " + std::to_string(vecs[i][3]) + ']' + (i == vecs.size() - 1 ? "" : ",\n");
			}
			return out + ']';
		}
		case ARRAY_EULER_ANGLES: {
			const auto angles = this->getValue<std::vector<DMXValue::EulerAngles>>();
			std::string out = "[";
			for (int i = 0; i < angles.size(); i++) {
				out += (i == 0 ? "" : " ") + std::string{"["} + std::to_string(angles[i][0]) + ", " + std::to_string(angles[i][1]) + ", " + std::to_string(angles[i][2]) + ']' + (i == angles.size() - 1 ? "" : ",\n");
			}
			return out + ']';
		}
		case ARRAY_QUATERNION: {
			const auto quats = this->getValue<std::vector<DMXValue::Quaternion>>();
			std::string out = "[";
			for (int i = 0; i < quats.size(); i++) {
				out += (i == 0 ? "" : " ") + std::string{"["} + std::to_string(quats[i][0]) + ", " + std::to_string(quats[i][1]) + ", " + std::to_string(quats[i][2]) + ']' + ", " + std::to_string(quats[i][3]) + ']' + (i == quats.size() - 1 ? "" : ",\n");
			}
			return out + ']';
		}
		case ARRAY_MATRIX_4X4: {
			const auto matrices = this->getValue<std::vector<DMXValue::Matrix4x4>>();
			std::string out = "[";
			for (int m = 0; m < matrices.size(); m++) {
				out += m == 0 ? "[" : " [";
				for (int i = 0; i < 4; i++) {
					out += i == 0 ? "" : "  ";
					for (int j = 0; j < 4; j++) {
						out += std::to_string(matrices[m][i][j]);
						if (j < 3) {
							out += ", ";
						} else if (i < 3) {
							out += ",\n";
						} else {
							out += ']';
						}
					}
				}
				if (m < matrices.size() - 1) {
					out += ",\n";
				}
			}
			return out + ']';
		}
		case ARRAY_UINT64: {
			const auto ints = this->getValue<std::vector<uint64_t>>();
			std::string out = "[";
			for (int i = 0; i < ints.size(); i++) {
				out += (i == 0 ? "" : " ") + std::to_string(ints[i]) + (i == ints.size() - 1 ? "" : ",");
			}
			return out + ']';
		}
		case ARRAY_UINT8: {
			const auto ints = this->getValue<std::vector<uint8_t>>();
			std::string out = "[";
			for (int i = 0; i < ints.size(); i++) {
				out += (i == 0 ? "" : " ") + std::to_string(ints[i]) + (i == ints.size() - 1 ? "" : ",");
			}
			return out + ']';
		}
	}
	return "";
}

void DMXAttribute::setValue(DMXValue::Generic value_) {
	this->value = std::move(value_);
}

DMXAttribute& DMXAttribute::operator=(DMXValue::Generic value_) {
	this->value = std::move(value_);
	return *this;
}

DMXElement::operator bool() const {
	return !this->type.empty() || !this->key.empty() || this->uuid != DMXValue::UUID{};
}

std::string_view DMXElement::getType() const {
	return this->type;
}

void DMXElement::setType(std::string type_) {
	this->type = std::move(type_);
}

std::string_view DMXElement::getKey() const {
	return this->key;
}

void DMXElement::setKey(std::string key_) {
	this->key = std::move(key_);
}

const DMXValue::UUID& DMXElement::getUUID() const {
	return this->uuid;
}

void DMXElement::setUUID(const DMXValue::UUID& guid_) {
	this->uuid = guid_;
}

bool DMXElement::hasAttribute(std::string_view attributeKey) const {
	return !this->operator[](attributeKey);
}

DMXAttribute& DMXElement::addAttribute(std::string key_, DMXValue::Generic value_) {
	DMXAttribute attr;
	attr.setKey(std::move(key_));
	attr.setValue(std::move(value_));
	this->attributes.push_back(std::move(attr));
	return this->attributes.back();
}

uint64_t DMXElement::getAttributeCount() const {
	return this->attributes.size();
}

uint64_t DMXElement::getAttributeCount(std::string_view childKey) const {
	uint64_t count = 0;
	for (const DMXAttribute& element : this->attributes) {
		if (string::iequals(element.key, childKey)) {
			++count;
		}
	}
	return count;
}

const std::vector<DMXAttribute>& DMXElement::getAttributes() const {
	return this->attributes;
}

std::vector<DMXAttribute>& DMXElement::getAttributes() {
	return this->attributes;
}

const DMXAttribute& DMXElement::operator[](unsigned int n) const {
	return this->attributes.at(n);
}

DMXAttribute& DMXElement::operator[](unsigned int n) {
	return this->attributes.at(n);
}

const DMXAttribute& DMXElement::operator[](std::string_view attributeKey) const {
	return this->operator()(attributeKey);
}

DMXAttribute& DMXElement::operator[](std::string_view attributeKey) {
	return this->operator()(attributeKey);
}

const DMXAttribute& DMXElement::operator()(std::string_view attributeKey) const {
	for (const auto& attribute : this->attributes) {
		if (string::iequals(attribute.getKey(), attributeKey)) {
			return attribute;
		}
	}
	return getInvalidAttribute();
}

DMXAttribute& DMXElement::operator()(std::string_view attributeKey) {
	for (auto& attribute : this->attributes) {
		if (string::iequals(attribute.getKey(), attributeKey)) {
			return attribute;
		}
	}
	return this->addAttribute(std::string{attributeKey});
}

const DMXAttribute& DMXElement::operator()(std::string_view attributeKey, unsigned int n) const {
	unsigned int count = 0;
	for (const auto& attribute : this->attributes) {
		if (string::iequals(attribute.getKey(), attributeKey)) {
			if (count == n) {
				return attribute;
			}
			if (++count > n) {
				break;
			}
		}
	}
	return getInvalidAttribute();
}

DMXAttribute& DMXElement::operator()(std::string_view attributeKey, unsigned int n) {
	unsigned int count = 0;
	for (auto& attribute : this->attributes) {
		if (string::iequals(attribute.getKey(), attributeKey)) {
			if (count == n) {
				return attribute;
			}
			if (++count > n) {
				break;
			}
		}
	}
	return this->addAttribute(std::string{attributeKey});
}

void DMXElement::removeAttribute(unsigned int n) {
	if (this->attributes.size() > n) {
		this->attributes.erase(this->attributes.begin() + n);
	}
}

void DMXElement::removeAttribute(std::string_view attributeKey, int n) {
	unsigned int count = 0;
	for (auto attribute = this->attributes.begin(); attribute != this->attributes.end(); ++attribute) {
		if (string::iequals(attribute->getKey(), attributeKey)) {
			if (n < 0 || count == n) {
				attribute = this->attributes.erase(attribute);
				if (count == n) {
					return;
				}
			}
			++count;
		}
	}
}

const DMXAttribute& DMXElement::getInvalidAttribute() {
	static DMXAttribute attribute;
	return attribute;
}

DMX::DMX(Encoding encodingType_, int encodingVersion_, std::string formatType_, int formatVersion_)
		: encodingType{encodingType_}
		, encodingVersion{encodingVersion_}
		, formatType{std::move(formatType_)}
		, formatVersion{formatVersion_} {}

DMX::DMX(std::span<const std::byte> dmxData) {
	BufferStreamReadOnly stream{dmxData};

	// Header can be at most MAX_HEADER characters long followed by either a null terminator or newline
	static constexpr int MAX_FORMAT_LENGTH = 64;
	static constexpr int MAX_HEADER_LENGTH = 40 + MAX_FORMAT_LENGTH * 2;

	std::string header;
	while (header.length() < MAX_HEADER_LENGTH) {
		const char temp = stream.read<char>();
		if (temp == '\0' || temp == '\n') {
			break;
		}
		header += temp;
	}

	std::array<char, MAX_FORMAT_LENGTH> encodingTypeData{};
	std::array<char, MAX_FORMAT_LENGTH> formatTypeData{};

	static constexpr std::string_view HEADER_FORMAT = "<!-- dmx encoding %63s %i format %63s %i -->";
	static constexpr std::string_view HEADER_FORMAT_OLD = "<!-- DMXVersion %63s_v%i -->";

#ifdef _WIN32
	bool headerParsed = sscanf_s(header.c_str(), HEADER_FORMAT.data(), encodingTypeData.data(), MAX_FORMAT_LENGTH, &this->encodingVersion, formatTypeData.data(), MAX_FORMAT_LENGTH, &this->formatVersion);
#else
	bool headerParsed = std::sscanf(header.c_str(), HEADER_FORMAT.data(), encodingTypeData.data(), &this->encodingVersion, formatTypeData.data(), &this->formatVersion); // NOLINT(*-err34-c)
#endif
	if (!headerParsed) {
		// Try old header, then bail if it's genuinely malformed
#ifdef _WIN32
		headerParsed = sscanf_s(header.c_str(), HEADER_FORMAT_OLD.data(), encodingTypeData.data(), MAX_FORMAT_LENGTH, &this->encodingVersion);
#else
		headerParsed = std::sscanf(header.c_str(), HEADER_FORMAT_OLD.data(), encodingTypeData.data(), &this->encodingVersion); // NOLINT(*-err34-c)
#endif
		if (!headerParsed) {
			this->encodingType = ENCODING_INVALID;
			return;
		}

		this->formatType = "dmx";

		std::string_view encodingTypeStr = encodingTypeData.data();
		if (encodingTypeStr.starts_with("unicode_")) {
			this->encodingTypeHasUnicodePrefix = true;
			encodingTypeStr = encodingTypeStr.substr(8);
		}

		if (encodingTypeStr == "binary") {
			this->encodingType = ENCODING_BINARY_OLD;
		} else if (encodingTypeStr == "sfm") {
			this->encodingType = ENCODING_BINARY_OLD_SFM;
			this->formatType = encodingTypeStr;
			this->formatVersion = encodingVersion;
		} else if (encodingTypeStr == "keyvalues2") {
			this->encodingType = ENCODING_KEYVALUES2_OLD;
		} else if (encodingTypeStr == "keyvalues2_flat") {
			this->encodingType = ENCODING_KEYVALUES2_FLAT_OLD;
		} else {
			this->encodingType = ENCODING_INVALID;
			return;
		}
	} else {
		this->formatType = formatTypeData.data();

		std::string_view encodingTypeStr = encodingTypeData.data();
		if (encodingTypeStr.starts_with("unicode_")) {
			this->encodingTypeHasUnicodePrefix = true;
			encodingTypeStr = encodingTypeStr.substr(8);
		}

		if (encodingTypeStr == "binary") {
			this->encodingType = ENCODING_BINARY;
		} else if (encodingTypeStr == "binary_seqids") {
			this->encodingType = ENCODING_BINARY_SEQIDS;
		} else if (encodingTypeStr == "keyvalues2") {
			this->encodingType = ENCODING_KEYVALUES2;
		} else if (encodingTypeStr == "keyvalues2_flat") {
			this->encodingType = ENCODING_KEYVALUES2_FLAT;
		} else if (encodingTypeStr == "keyvalues2_noids") {
			this->encodingType = ENCODING_KEYVALUES2_NOIDS;
		} else {
			this->encodingType = ENCODING_INVALID;
			return;
		}
	}

	if (!isEncodingVersionValid(this->encodingType, this->encodingVersion)) {
		this->encodingType = ENCODING_INVALID;
		return;
	}

	const auto readBinary = [this, &stream] {
		// Version-specific conditionals
		const bool isLegacyEncoding = this->encodingType == ENCODING_BINARY_OLD || this->encodingType == ENCODING_BINARY_OLD_SFM;
		const DMXValue::IDVersion attributeIDVersion = isLegacyEncoding || this->encodingVersion < 3 ? DMXValue::IDVersion::V1 : this->encodingVersion < 9 ? DMXValue::IDVersion::V2 : DMXValue::IDVersion::V3;
		const bool stringListExists = !isLegacyEncoding && this->encodingVersion > 1;
		const bool elementNamesAndStringValuesAreStoredInStringList = !isLegacyEncoding && this->encodingVersion > 3;
		const bool stringListLengthIsShort = this->encodingVersion < 4;
		const bool stringListIndicesAreShort = this->encodingVersion < 5;
		const bool preloadAttributeListExists = !isLegacyEncoding && this->encodingVersion > 5;

		// Eat the null terminator for the header.
		if (stream.read<char>() != 0) {
			return false;
		}

		// Helper to read a string index and get the string from the list
		std::vector<std::string> stringList;
		const auto readStringFromIndex = [stringListExists, stringListIndicesAreShort, &stringList](BufferStream& stream_) -> std::string {
			if (!stringListExists) {
				return stream_.read_string();
			}
			uint32_t index;
			if (stringListIndicesAreShort) {
				index = stream_.read<uint16_t>();
			} else {
				index = stream_.read<uint32_t>();
			}
			if (index >= stringList.size()) {
				// This is an intentional feature of the format
				return "";
			}
			return stringList.at(index);
		};

		// Helper to read a value for an attribute
		std::function<DMXValue::Generic(DMXValue::ID, bool)> readValue;
		readValue = [&stream, &readStringFromIndex, &readValue](DMXValue::ID type, bool useStringList) -> DMXValue::Generic {
			const auto readArrayValue = [&stream, &readValue]<typename T>(DMXValue::ID type_) {
				std::vector<T> out;
				auto size = stream.read<uint32_t>();
				out.reserve(size);
				for (int i = 0; i < size; i++) {
					// String arrays are always inline
					out.push_back(std::get<T>(readValue(DMXValue::arrayIDToInnerID(type_), false)));
				}
				return out;
			};
			switch (type) {
				using enum DMXValue::ID;
				case INVALID:
					return std::monostate{};
				case ELEMENT: {
					DMXValue::Element value;
					value.index = stream.read<int32_t>();
					if (value.index == -2) {
						// Read in the ASCII GUID
						value.externalGUID = stream.read_string();
					}
					return value;
				}
				case INT32:
					return stream.read<int32_t>();
				case FLOAT:
					return stream.read<float>();
				case BOOL:
					return stream.read<bool>();
				case STRING:
					return useStringList ? readStringFromIndex(stream) : stream.read_string();
				case BYTEARRAY:
					return stream.read_bytes(stream.read<uint32_t>());
				case UUID:
					return stream.read_bytes<16>();
				case TIME:
					return DMXValue::Time{static_cast<float>(static_cast<double>(stream.read<int32_t>()) / 10000.0)};
				case COLOR: {
					DMXValue::Color value{};
					stream >> value.r >> value.g >> value.b >> value.a;
					return value;
				}
				case VECTOR2: {
					DMXValue::Vector2 value;
					stream >> value[0] >> value[1];
					return value;
				}
				case VECTOR3: {
					DMXValue::Vector3 value;
					stream >> value[0] >> value[1] >> value[2];
					return value;
				}
				case VECTOR4: {
					DMXValue::Vector4 value;
					stream >> value[0] >> value[1] >> value[2] >> value[3];
					return value;
				}
				case EULER_ANGLES: {
					DMXValue::EulerAngles value;
					stream >> value[0] >> value[1] >> value[2];
					return value;
				}
				case QUATERNION: {
					DMXValue::Quaternion value;
					stream >> value[0] >> value[1] >> value[2] >> value[3];
					return value;
				}
				case MATRIX_4X4: {
					DMXValue::Matrix4x4 value;
					stream
						>> value[0][0] >> value[0][1] >> value[0][2] >> value[0][3]
						>> value[1][0] >> value[1][1] >> value[1][2] >> value[1][3]
						>> value[2][0] >> value[2][1] >> value[2][2] >> value[2][3]
						>> value[3][0] >> value[3][1] >> value[3][2] >> value[3][3];
					return value;
				}
				case UINT64:
					return stream.read<uint64_t>();
				case UINT8:
					return stream.read<uint8_t>();
				case ARRAY_ELEMENT:
					return readArrayValue.operator()<DMXValue::Element>(type);
				case ARRAY_INT32:
					return readArrayValue.operator()<int32_t>(type);
				case ARRAY_FLOAT:
					return readArrayValue.operator()<float>(type);
				case ARRAY_BOOL:
					return readArrayValue.operator()<bool>(type);
				case ARRAY_STRING:
					return readArrayValue.operator()<std::string>(type);
				case ARRAY_BYTEARRAY:
					return readArrayValue.operator()<std::vector<std::byte>>(type);
				case ARRAY_UUID:
					return readArrayValue.operator()<DMXValue::UUID>(type);
				case ARRAY_TIME:
					return readArrayValue.operator()<DMXValue::Time>(type);
				case ARRAY_COLOR:
					return readArrayValue.operator()<DMXValue::Color>(type);
				case ARRAY_VECTOR2:
					return readArrayValue.operator()<DMXValue::Vector2>(type);
				case ARRAY_VECTOR3:
					return readArrayValue.operator()<DMXValue::Vector3>(type);
				case ARRAY_VECTOR4:
					return readArrayValue.operator()<DMXValue::Vector4>(type);
				case ARRAY_EULER_ANGLES:
				return readArrayValue.operator()<DMXValue::EulerAngles>(type);
				case ARRAY_QUATERNION:
					return readArrayValue.operator()<DMXValue::Quaternion>(type);
				case ARRAY_MATRIX_4X4:
					return readArrayValue.operator()<DMXValue::Matrix4x4>(type);
				case ARRAY_UINT64:
					return readArrayValue.operator()<uint64_t>(type);
				case ARRAY_UINT8:
					return readArrayValue.operator()<uint8_t>(type);
			}
			return std::monostate{};
		};

		// Helper to read element attributes
		const auto readAttributes = [&stream, attributeIDVersion, &readStringFromIndex, &readValue](DMXElement& element, bool useStringList) {
			const auto attributeCount = stream.read<int32_t>();
			element.attributes.reserve(attributeCount);

			for (int i = 0; i < attributeCount; i++) {
				element.attributes.push_back(DMXAttribute{});
				auto& attribute = element.attributes.back();

				attribute.setKey(readStringFromIndex(stream));

				auto attributeID = DMXValue::ID::INVALID;
				switch (attributeIDVersion) {
					// ReSharper disable once CppDFAUnreachableCode
					case DMXValue::IDVersion::V1: attributeID = DMXValue::decodeID(static_cast<DMXValue::IDv1>(stream.read<std::byte>())); break;
					case DMXValue::IDVersion::V2: attributeID = DMXValue::decodeID(static_cast<DMXValue::IDv2>(stream.read<std::byte>())); break;
					case DMXValue::IDVersion::V3: attributeID = DMXValue::decodeID(static_cast<DMXValue::IDv3>(stream.read<std::byte>())); break;
				}
				attribute.setValue(readValue(attributeID, useStringList));
			}
		};

		// Preload attributes
		if (preloadAttributeListExists) {
			if (const auto preloadElementCount = stream.read<uint32_t>()) {
				this->prefixAttributeContainers.reserve(preloadElementCount);
				for (uint32_t i = 0; i < preloadElementCount; i++) {
					auto& element = this->addPrefixAttributeContainer();
					readAttributes(element, false);
				}
			}
		}

		// String list
		if (stringListExists) {
			uint32_t stringCount;
			if (stringListLengthIsShort) {
				stringCount = stream.read<uint16_t>();
			} else {
				stringCount = stream.read<uint32_t>();
			}
			stringList.reserve(stringCount);
			for (int i = 0; i < stringCount; i++) {
				stringList.push_back(stream.read_string());
			}
		}

		// Read elements
		const auto elementCount = stream.read<int32_t>();
		this->elements.reserve(elementCount);

		for (int i = 0; i < elementCount; i++) {
			this->elements.push_back(DMXElement{});
			auto& element = this->elements.back();

			element.setType(readStringFromIndex(stream));
			if (elementNamesAndStringValuesAreStoredInStringList) {
				element.setKey(readStringFromIndex(stream));
			} else {
				element.setKey(stream.read_string());
			}
			element.setUUID(stream.read_bytes<16>());
		}

		// Read element attributes
		for (auto& element : this->elements) {
			readAttributes(element, elementNamesAndStringValuesAreStoredInStringList);
		}

		return true;
	};

	bool parseSuccess = false;
	switch (this->encodingType) {
		case ENCODING_INVALID:
			break;
		case ENCODING_BINARY_OLD:
		case ENCODING_BINARY_OLD_SFM:
		case ENCODING_BINARY:
		case ENCODING_BINARY_SEQIDS:
			parseSuccess = readBinary();
			break;
		case ENCODING_KEYVALUES2_OLD:
		case ENCODING_KEYVALUES2:
		case ENCODING_KEYVALUES2_FLAT_OLD:
		case ENCODING_KEYVALUES2_FLAT:
		case ENCODING_KEYVALUES2_NOIDS:
			break;
	}
	if (!parseSuccess) {
		this->encodingType = ENCODING_INVALID;
		this->elements.clear();
	}
}

DMX::DMX(std::string_view dmxData)
		: DMX{{reinterpret_cast<const std::byte*>(dmxData.data()), dmxData.size()}} {}

DMX::operator bool() const {
	return this->encodingType != ENCODING_INVALID;
}

DMX::Encoding DMX::getEncodingType() const {
	return this->encodingType;
}

void DMX::setEncodingType(Encoding encodingType_) {
	this->encodingType = encodingType_;
	if (!isEncodingVersionValid(this->encodingType, this->encodingVersion)) {
		this->encodingVersion = 1;
	}
}

bool DMX::doesEncodingTypeHaveUnicodePrefix() const {
	return this->encodingTypeHasUnicodePrefix;
}

void DMX::shouldEncodingTypeHaveUnicodePrefix(bool encodingTypeHasUnicodePrefix_) {
	this->encodingTypeHasUnicodePrefix = encodingTypeHasUnicodePrefix_;
}

int DMX::getEncodingVersion() const {
	return this->encodingVersion;
}

void DMX::setEncodingVersion(int encodingVersion_) {
	if (isEncodingVersionValid(this->encodingType, encodingVersion_)) {
		this->encodingVersion = encodingVersion_;
	}
}

std::string_view DMX::getFormatType() const {
	return this->formatType;
}

void DMX::setFormatType(std::string formatType_) {
	this->formatType = std::move(formatType_);
}

int DMX::getFormatVersion() const {
	return this->formatVersion;
}

void DMX::setFormatVersion(int formatVersion_) {
	this->formatVersion = formatVersion_;
}

DMXElement& DMX::addPrefixAttributeContainer() {
	this->prefixAttributeContainers.push_back(DMXElement{});
	return this->prefixAttributeContainers.back();
}

uint64_t DMX::getPrefixAttributeContainerCount() const {
	return this->prefixAttributeContainers.size();
}

const std::vector<DMXElement>& DMX::getPrefixAttributeContainers() const {
	return this->prefixAttributeContainers;
}

std::vector<DMXElement> & DMX::getPrefixAttributeContainers() {
	return this->prefixAttributeContainers;
}

void DMX::removePrefixAttributeContainer(unsigned int n) {
	if (this->prefixAttributeContainers.size() > n) {
		this->prefixAttributeContainers.erase(this->prefixAttributeContainers.begin() + n);
	}
}

bool DMX::hasElement(std::string_view key) const {
	return !this->operator[](key);
}

DMXElement& DMX::addElement(std::string type, std::string key) {
	DMXElement elem;
	elem.setType(std::move(type));
	elem.setKey(std::move(key));
	elem.setUUID(createRandomUUID());
	this->elements.push_back(std::move(elem));
	return this->elements.back();
}

uint64_t DMX::getElementCount() const {
	return this->elements.size();
}

uint64_t DMX::getElementCount(std::string_view key) const {
	uint64_t count = 0;
	for (const auto& element : this->elements) {
		if (string::iequals(element.getKey(), key)) {
			++count;
		}
	}
	return count;
}

const std::vector<DMXElement>& DMX::getElements() const {
	return this->elements;
}

std::vector<DMXElement>& DMX::getElements() {
	return this->elements;
}

const DMXElement& DMX::operator[](unsigned int n) const {
	return this->elements.at(n);
}

DMXElement& DMX::operator[](unsigned int n) {
	return this->elements.at(n);
}

const DMXElement& DMX::operator[](std::string_view key) const {
	return this->operator()(key);
}

DMXElement& DMX::operator[](std::string_view key) {
	return this->operator()(key);
}

const DMXElement& DMX::operator()(std::string_view key) const {
	for (const auto& element : this->elements) {
		if (string::iequals(element.getKey(), key)) {
			return element;
		}
	}
	return getInvalidElement();
}

DMXElement& DMX::operator()(std::string_view key) {
	for (auto& element : this->elements) {
		if (string::iequals(element.getKey(), key)) {
			return element;
		}
	}
	return this->addElement("DmElement", std::string{key});
}

const DMXElement& DMX::operator()(std::string_view key, unsigned int n) const {
	unsigned int count = 0;
	for (const auto& element : this->elements) {
		if (string::iequals(element.getKey(), key)) {
			if (count == n) {
				return element;
			}
			if (++count > n) {
				break;
			}
		}
	}
	return getInvalidElement();
}

DMXElement& DMX::operator()(std::string_view key, unsigned int n) {
	unsigned int count = 0;
	for (auto& element: this->elements) {
		if (string::iequals(element.getKey(), key)) {
			if (count == n) {
				return element;
			}
			if (++count > n) {
				break;
			}
		}
	}
	return this->addElement("DmElement", std::string{key});
}

void DMX::removeElement(unsigned int n) {
	if (this->elements.size() > n) {
		this->elements.erase(this->elements.begin() + n);
	}
	for (auto& element : this->elements) {
		for (auto& attribute : element.attributes) {
			if (const auto attributeType = attribute.getValueType(); attributeType == DMXValue::ID::ELEMENT) {
				auto elementRef = attribute.getValue<DMXValue::Element>();
				if (elementRef.index > n) {
					elementRef.index--;
				}
				attribute.setValue(elementRef);
			} else if (attributeType == DMXValue::ID::ARRAY_ELEMENT) {
				auto elementsRef = attribute.getValue<std::vector<DMXValue::Element>>();
				bool shouldSet = false;
				for (auto& [index, externalGUID] : elementsRef) {
					if (index > n) {
						index--;
						shouldSet = true;
					}
				}
				if (shouldSet) {
					attribute.setValue(elementsRef);
				}
			}
		}
	}
}

std::vector<std::byte> DMX::bake() const {
	if (!isEncodingVersionValid(this->encodingType, this->encodingVersion)) {
		return {};
	}

	// todo: bake
	return {};
}

void DMX::bake(const std::filesystem::path& dmxPath) const {
	fs::writeFileBuffer(dmxPath, this->bake());
}

DMXValue::UUID DMX::createRandomUUID() {
	static std::random_device random_device{};
	static std::mt19937 generator{random_device()};
	std::uniform_int_distribution<short> distribution{std::numeric_limits<uint8_t>::min(), std::numeric_limits<uint8_t>::max()};

	DMXValue::UUID uuid;
	for (auto& byte : uuid) {
		byte = static_cast<std::byte>(distribution(generator));
	}
	return uuid;
}

const DMXElement& DMX::getInvalidElement() {
	static DMXElement element;
	return element;
}

DMX literals::operator ""_kv2(const char *str, std::size_t len) {
	return DMX{{str, len}};
}
