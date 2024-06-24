#include <sourcepp/fs/FS.h>

#include <filesystem>

#include <FileStream.h>

#include <sourcepp/math/Integer.h>

using namespace sourcepp;

std::vector<std::byte> fs::readFileBuffer(const std::string& filepath, std::size_t startOffset) {
	FileStream stream{filepath};
	if (!stream) {
		return {};
	}
	stream.seek_in_u(startOffset);
	return stream.read_bytes(std::filesystem::file_size(filepath) - startOffset);
}

std::string fs::readFileText(const std::string& filepath, std::size_t startOffset) {
	FileStream stream{filepath};
	if (!stream) {
		return {};
	}
	stream.seek_in_u(startOffset);
	return stream.read_string();
}
