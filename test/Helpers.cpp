#include "Helpers.h"

#include <filesystem>
#include <fstream>

std::vector<std::byte> readFile(const std::string& path) {
	std::ifstream file(path, std::ios::binary);
	file.unsetf(std::ios::skipws);
	auto size = std::filesystem::file_size(path);
	std::vector<std::byte> out(size);
	file.read(reinterpret_cast<char*>(out.data()), static_cast<std::streamsize>(size));
	return out;
}

void createFile(const std::string& name, const std::vector<std::byte>& data) {
	std::ofstream out{name.data(), std::ios::binary | std::ios::trunc};
	out.unsetf(std::ios::skipws);
	out.write(reinterpret_cast<const char*>(data.data()), static_cast<std::streamsize>(data.size()));
}
