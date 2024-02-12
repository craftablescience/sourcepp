#include "Helpers.h"

#include <filesystem>
#include <fstream>

std::vector<std::byte> readFileToBuffer(const std::string& path) {
	std::ifstream file(path, std::ios::binary);
	file >> std::skipws;
	auto size = std::filesystem::file_size(path);
	std::vector<std::byte> out(size);
	file.read(reinterpret_cast<char*>(out.data()), static_cast<std::streamsize>(size));
	return out;
}
