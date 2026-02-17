#include <sourceppc/String.h>

#include <cstdlib>

SOURCEPP_API sourcepp_string_t sourcepp_string_new(size_t size) {
	sourcepp_string_t str;
	if (size > 0) {
		str.size = static_cast<int64_t>(size);
		str.data = static_cast<uint8_t*>(std::malloc(sizeof(uint8_t) * (size + 1)));
		str.data[size] = '\0';
	} else {
		str.size = 0;
		str.data = nullptr;
	}
	return str;
}

SOURCEPP_API void sourcepp_string_free(sourcepp_string_t* str) {
	if (str->data) {
		std::free(str->data);
		str->data = nullptr;
	}
	str->size = 0;
}

SOURCEPP_API sourcepp_string_array_t sourcepp_string_array_new(size_t size) {
	sourcepp_string_array_t array;
	if (size > 0) {
		array.size = static_cast<int64_t>(size);
		array.data = static_cast<char**>(std::malloc(sizeof(char*) * size));
	} else {
		array.size = 0;
		array.data = nullptr;
	}
	return array;
}

SOURCEPP_API void sourcepp_string_array_free(sourcepp_string_array_t* array) {
	if (array->data) {
		for (size_t i = 0; i < array->size; i++) {
			if (char* str = array->data[i]) {
				std::free(str);
				array->data[i] = nullptr;
			}
		}
		std::free(array->data);
		array->data = nullptr;
	}
	array->size = 0;
}

// C++ conversion routines

using namespace sourceppc;

sourcepp_string_t convert::toString(std::string_view str) {
	const auto newStr = sourcepp_string_new(str.size());
	std::memcpy(newStr.data, str.data(), str.size());
	return newStr;
}

sourcepp_string_array_t convert::toStringArray(const std::vector<std::string>& stringVec) {
	const auto array = sourcepp_string_array_new(stringVec.size());
	for (size_t i = 0; i < stringVec.size(); i++) {
		array.data[i] = static_cast<char*>(std::malloc(sizeof(char) * (stringVec[i].length() + 1)));
		std::memcpy(array.data[i], stringVec[i].c_str(), stringVec[i].length());
		array.data[i][stringVec[i].length()] = '\0';
	}
	return array;
}

size_t convert::writeStringToMem(std::string_view str, char* buffer, size_t bufferLen) {
	if (str.length() >= bufferLen) {
		std::memcpy(buffer, str.data(), bufferLen);
		buffer[bufferLen - 1] = '\0';
		return bufferLen;
	}
	std::memcpy(buffer, str.data(), str.length());
	buffer[str.length()] = '\0';
	return str.length() - 1;
}

size_t convert::writeVectorToMem(const std::vector<std::byte>& vec, unsigned char* buffer, size_t bufferLen) {
	if (vec.size() >= bufferLen) {
		std::memcpy(buffer, vec.data(), bufferLen);
		return bufferLen;
	}
	std::memcpy(buffer, vec.data(), vec.size());
	return vec.size();
}
