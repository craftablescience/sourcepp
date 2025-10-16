#include <dmxpp/dmxpp.h>

#include <cstdio>
#include <functional>

#include <BufferStream.h>

using namespace dmxpp;

DMX::DMX(const std::byte* dmxData, std::size_t dmxSize) {
	if (!dmxData || !dmxSize) {
		return;
	}

	BufferStreamReadOnly stream{dmxData, dmxSize};

	// The header is terminated by a newline, but can be at most MAX_HEADER long.
	// For binary formats that's then followed by a null terminator.
	std::string header;
	while (header.length() < MAX_HEADER) {
		char temp = stream.read<char>();
		if (temp == '\n') {
			break;
		}
		header += temp;
	}
	if (header.length() < 37) {
		// Minimum possible header length - early "binary_v2" version, "keyvalues2_v1" unsupported
		return;
	}
	char encodingTypeData[MAX_FORMAT];
	int32_t encodingVersionData;
	char formatTypeData[MAX_FORMAT];
	int32_t formatVersionData;
#ifdef _WIN32
	sscanf_s(header.c_str(), "<!-- dmx encoding %63s %i format %63s %i -->\n", encodingTypeData, MAX_FORMAT, &encodingVersionData, formatTypeData, MAX_FORMAT, &formatVersionData);
#else
	std::sscanf(header.c_str(), "<!-- dmx encoding %s %i format %s %i -->\n", encodingTypeData, &encodingVersionData, formatTypeData, &formatVersionData);
#endif

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

	// Srctools formats indicate that all strings are specifically UTF-8.
	if (this->encodingType == Format::BINARY || this->encodingType == Format::SRCTOOLS_UTF8_BINARY) {
		this->opened = this->openBinary(stream);
	} else if (this->encodingType == Format::TEXT || this->encodingType == Format::SRCTOOLS_UTF8_TEXT) {
		this->opened = this->openText(dmxData, dmxSize);
	}
}

DMX::DMX(const unsigned char* dmxData, std::size_t dmxSize)
		: DMX(reinterpret_cast<const std::byte*>(dmxData), dmxSize) {}

DMX::DMX(const std::vector<std::byte>& dmxData)
		: DMX(dmxData.data(), dmxData.size()) {}

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

const std::vector<DMXElement>& DMX::getElements() const {
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

	// Eat the null terminator for the header.
	if (stream.read<char>() != 0) {
		return false;
	}

	// String list
	std::vector<std::string> stringList;
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

	// Read a string index and get the string from the list
	const auto readStringFromIndex = [stringListIndicesAreShort, &stringList](BufferStream& stream_) {
		uint32_t index;
		if (stringListIndicesAreShort) {
			index = stream_.read<uint16_t>();
		} else {
			index = stream_.read<uint32_t>();
		}
		if (index >= stringList.size()) {
			throw std::overflow_error{"String list index out of bounds!"};
		}
		return stringList.at(index);
	};

	// Read elements
	const int elementCount = stream.read<int32_t>();
	for (int i = 0; i < elementCount; i++) {
		auto& [type, name, guid, attributes] = this->elements.emplace_back();
		type = readStringFromIndex(stream);
		if (elementNamesAreStoredInStringList) {
			name = readStringFromIndex(stream);
		} else {
			name = stream.read_string();
		}
		guid = stream.read_bytes<16>();
	}

	// Helper to read a value for an attribute
	std::function<Value::Generic(BufferStream&, Value::ID, bool)> readValue;
	readValue = [&readValue, &readStringFromIndex](BufferStream& stream_, Value::ID type, bool useStringList) -> Value::Generic {
		const auto readArrayValue = [&readValue]<typename T>(BufferStream& reader, Value::ID type_) {
			std::vector<T> out;
			auto size = reader.read<uint32_t>();
			out.reserve(size);
			for (int i = 0; i < size; i++) {
				// String arrays are always inline.
				out.push_back(std::get<T>(readValue(reader, Value::arrayIDToInnerID(type_), false)));
			}
			return out;
		};
		switch (type) {
			using enum Value::ID;
			case INVALID:
				return Value::Invalid{};
			case ELEMENT: {
				Value::Element value;
				value.index = stream_.read<uint32_t>();
				if (value.index == -2) {
					// Parse the ASCII GUID if it's a stub
					value.stubGUID = stream_.read_string();
				}
				return value;
			}
			case INT:
				return stream_.read<int32_t>();
			case FLOAT:
				return stream_.read<float>();
			case BOOL:
				return stream_.read<bool>();
			case STRING:
				return useStringList ? readStringFromIndex(stream_) : stream_.read_string();
			case BYTEARRAY:
				return stream_.read_bytes(stream_.read<uint32_t>());
			case TIME:
				return Value::Time{static_cast<float>(static_cast<double>(stream_.read<int32_t>()) / 10000.0)};
			case COLOR:
				return stream_.read<Value::Color>();
			case VECTOR2:
				return stream_.read<Value::Vector2>();
			case VECTOR3:
			case EULER_ANGLE:
				return stream_.read<Value::Vector3>();
			case VECTOR4:
			case QUATERNION:
				return stream_.read<Value::Vector4>();
			case MATRIX_4X4:
				return stream_.read<Value::Matrix4x4>();
			case ARRAY_ELEMENT:
				return readArrayValue.operator()<Value::Element>(stream_, type);
			case ARRAY_INT:
				return readArrayValue.operator()<int32_t>(stream_, type);
			case ARRAY_FLOAT:
				return readArrayValue.operator()<float>(stream_, type);
			case ARRAY_BOOL:
				return readArrayValue.operator()<bool>(stream_, type);
			case ARRAY_STRING:
				return readArrayValue.operator()<std::string>(stream_, type);
			case ARRAY_BYTEARRAY:
				return readArrayValue.operator()<std::vector<std::byte>>(stream_, type);
			case ARRAY_TIME:
				return readArrayValue.operator()<Value::Time>(stream_, type);
			case ARRAY_COLOR:
				return readArrayValue.operator()<Value::Color>(stream_, type);
			case ARRAY_VECTOR2:
				return readArrayValue.operator()<Value::Vector2>(stream_, type);
			case ARRAY_VECTOR3:
			case ARRAY_EULER_ANGLE:
				return readArrayValue.operator()<Value::Vector3>(stream_, type);
			case ARRAY_VECTOR4:
			case ARRAY_QUATERNION:
				return readArrayValue.operator()<Value::Vector4>(stream_, type);
			case ARRAY_MATRIX_4X4:
				return readArrayValue.operator()<Value::Matrix4x4>(stream_, type);
		}
		return 0; // Dummy to get it to compile
	};

	// Read element attributes
	for (auto& [type, name, guid, attributes] : this->elements) {
		const int attributeCount = stream.read<int32_t>();
		for (int i = 0; i < attributeCount; i++) {
			auto& [attrName, attrType, attrValue] = attributes.emplace_back();
			attrName = readStringFromIndex(stream);
			attrType = Value::byteToID(stream.read<std::byte>());
			attrValue = readValue(stream, attrType, stringValuesAreStoredInStringList);
		}
	}

	return true;
}
