#include <dmxpp/dmxpp.h>

#include <cstdio>
#include <functional>

#include <BufferStream.h>

using namespace dmxpp;

DMX::DMX(const std::byte* dmxData, std::size_t dmxSize) {
	if (!dmxData || !dmxSize) {
		return;
	}

	BufferStream stream{dmxData, dmxSize};

	auto header = stream.read_string();
	if (header.length() < 37) {
		// Minimum possible header length - early "binary_v2" version unsupported
		return;
	}
	char encodingTypeData[64];
	int32_t encodingVersionData;
	char formatTypeData[64];
	int32_t formatVersionData;
	// NOLINTNEXTLINE(*-err34-c)
	std::sscanf(header.c_str(), "<!-- dmx encoding %s %i format %s %i -->\n", encodingTypeData, &encodingVersionData, formatTypeData, &formatVersionData);

	this->encodingType = encodingTypeData;
	this->encodingVersion = encodingVersionData;
	this->formatType = formatTypeData;
	this->formatVersion = formatVersionData;

	// DMX v9+
	if (this->encodingVersion >= 9) {
		// This format adds some stuff which really messes with v5 and below
		// Difficult to support both, and very easy to be lazy
		return;
	}

	if (this->encodingType == Format::BINARY) {
		this->opened = this->openBinary(stream);
	} else if (this->encodingType == Format::TEXT) {
		this->opened = this->openText(dmxData, dmxSize);
	}
}

DMX::DMX(const unsigned char* dmxData, std::size_t dmxSize) :
		DMX(reinterpret_cast<const std::byte*>(dmxData), dmxSize) {}

DMX::DMX(const std::vector<std::byte>& dmxData) :
		DMX(dmxData.data(), dmxData.size()) {}

DMX::DMX(const std::vector<unsigned char>& dmxData)
		: DMX(dmxData.data(), dmxData.size()) {}

DMX::operator bool() const {
	return this->opened;
}

std::string_view DMX::getFormatType() const {
	return this->formatType;
}

int DMX::getFormatVersion() const {
	return this->formatVersion;
}

std::string_view DMX::getEncodingType() const {
	return this->encodingType;
}

int DMX::getEncodingVersion() const {
	return this->encodingVersion;
}

const std::vector<Element>& DMX::getElements() const {
	return this->elements;
}

bool DMX::openText(const std::byte* dmxData, std::size_t dmxSize) {
	return false; // todo: text decoding
}

bool DMX::openBinary(BufferStream& stream) {
	// Version-specific conditionals
	const bool stringListLengthIsShort = this->encodingVersion < 4;
	const bool stringListIndicesAreShort = this->encodingVersion < 5;
	const bool elementNamesAreStoredInStringList = this->encodingVersion >= 4;
	const bool stringValuesAreStoredInStringList = this->encodingVersion >= 4;

	// String list
	std::vector<std::string> stringList;
	uint32_t stringCount;
	if (stringListLengthIsShort) {
		stringCount = stream.read<uint16_t>();
	} else {
		stringCount = stream.read<uint32_t>();
	}
	for (int i = 0; i < stringCount; i++) {
		stringList.push_back(stream.read_string());
	}

	// Read a string index and get the string from the list
	const auto readStringFromIndex = [stringListIndicesAreShort, &stringList](BufferStream& stream) {
		if (stringListIndicesAreShort) {
			return stringList.at(stream.read<uint16_t>());
		}
		return stringList.at(stream.read<uint32_t>());
	};

	// Read elements
	int elementCount = stream.read<int32_t>();
	for (int i = 0; i < elementCount; i++) {
		auto& element = this->elements.emplace_back();
		element.type = readStringFromIndex(stream);
		if (elementNamesAreStoredInStringList) {
			element.name = readStringFromIndex(stream);
		} else {
			element.name = stream.read_string();
		}
		element.guid = stream.read_bytes<16>();
	}

	// Helper to read a value for an attribute
	std::function<Value::Generic(BufferStream&, Value::ID, bool)> readValue;
	readValue = [&readValue, &readStringFromIndex](BufferStream& stream, Value::ID type, bool useStringList) -> Value::Generic {
		const auto readArrayValue = [&readValue]<typename T>(BufferStream& stream, Value::ID type) {
			std::vector<T> out;
			auto size = stream.read<uint32_t>();
			out.reserve(size);
			for (int i = 0; i < size; i++) {
				out.push_back(std::get<T>(readValue(stream, Value::arrayIDToInnerID(type), true)));
			}
			return out;
		};
		switch (type) {
			using enum Value::ID;
			case INVALID:
				return Value::Invalid{};
			case ELEMENT: {
				Value::Element value;
				value.index = stream.read<uint32_t>();
				if (value.index == -2) {
					// Parse the ASCII GUID if it's a stub
					value.stubGUID = stream.read_string();
				}
				return value;
			}
			case INT:
				return stream.read<int32_t>();
			case FLOAT:
				return stream.read<float>();
			case BOOL:
				return stream.read<bool>();
			case STRING:
				return useStringList ? readStringFromIndex(stream) : stream.read_string();
			case BYTEARRAY:
				return stream.read_bytes(stream.read<uint32_t>());
			case TIME:
				return Value::Time{static_cast<float>(static_cast<double>(stream.read<int32_t>()) / 10000.0)};
			case COLOR:
				return stream.read<Value::Color>();
			case VECTOR2:
				return stream.read<Value::Vector2>();
			case VECTOR3:
			case EULER_ANGLE:
				return stream.read<Value::Vector3>();
			case VECTOR4:
			case QUATERNION:
				return stream.read<Value::Vector4>();
			case MATRIX_4X4:
				return stream.read<Value::Matrix4x4>();
			case ARRAY_ELEMENT:
				return readArrayValue.operator()<Value::Element>(stream, type);
			case ARRAY_INT:
				return readArrayValue.operator()<int32_t>(stream, type);
			case ARRAY_FLOAT:
				return readArrayValue.operator()<float>(stream, type);
			case ARRAY_BOOL:
				return readArrayValue.operator()<bool>(stream, type);
			case ARRAY_STRING:
				return readArrayValue.operator()<std::string>(stream, type);
			case ARRAY_BYTEARRAY:
				return readArrayValue.operator()<std::vector<std::byte>>(stream, type);
			case ARRAY_TIME:
				return readArrayValue.operator()<Value::Time>(stream, type);
			case ARRAY_COLOR:
				return readArrayValue.operator()<Value::Color>(stream, type);
			case ARRAY_VECTOR2:
				return readArrayValue.operator()<Value::Vector2>(stream, type);
			case ARRAY_VECTOR3:
			case ARRAY_EULER_ANGLE:
				return readArrayValue.operator()<Value::Vector3>(stream, type);
			case ARRAY_VECTOR4:
			case ARRAY_QUATERNION:
				return readArrayValue.operator()<Value::Vector4>(stream, type);
			case ARRAY_MATRIX_4X4:
				return readArrayValue.operator()<Value::Matrix4x4>(stream, type);
		}
		return 0; // Dummy to get it to compile
	};

	// Read element attributes
	for (auto& element : this->elements) {
		int attributeCount = stream.read<int32_t>();
		for (int i = 0; i < attributeCount; i++) {
			auto& attribute = element.attributes.emplace_back();
			attribute.name = readStringFromIndex(stream);
			attribute.type = Value::byteToID(stream.read<std::byte>());
			attribute.value = readValue(stream, attribute.type, stringValuesAreStoredInStringList);
		}
	}

	return true;
}
