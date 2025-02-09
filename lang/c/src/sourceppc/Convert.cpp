#include <sourceppc/Convert.hpp>

sourcepp_string_t Convert::toString(std::string_view str) {
	auto newStr = sourcepp_string_new(str.size());
	std::memcpy(newStr.data, str.data(), str.size());
	return newStr;
}

sourcepp_string_array_t Convert::toStringArray(const std::vector<std::string>& stringVec) {
	auto array = sourcepp_string_array_new(stringVec.size());
	for (size_t i = 0; i < stringVec.size(); i++) {
		array.data[i] = static_cast<char*>(std::malloc(sizeof(char) * (stringVec[i].length() + 1)));
		std::memcpy(array.data[i], stringVec[i].c_str(), stringVec[i].length());
		array.data[i][stringVec[i].length()] = '\0';
	}
	return array;
}

size_t Convert::writeStringToMem(std::string_view str, char* buffer, size_t bufferLen) {
	if (str.length() >= bufferLen) {
		std::memcpy(buffer, str.data(), bufferLen);
		buffer[bufferLen - 1] = '\0';
		return bufferLen;
	}
	std::memcpy(buffer, str.data(), str.length());
	buffer[str.length()] = '\0';
	return str.length() - 1;
}

size_t Convert::writeVectorToMem(const std::vector<std::byte>& vec, unsigned char* buffer, size_t bufferLen) {
	if (vec.size() >= bufferLen) {
		std::memcpy(buffer, vec.data(), bufferLen);
		return bufferLen;
	}
	std::memcpy(buffer, vec.data(), vec.size());
	return vec.size();
}
