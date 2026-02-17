#include <sourcepp/FS.h>

#include <FileStream.h>

using namespace sourcepp;

std::vector<std::byte> fs::readFileBuffer(const std::filesystem::path& filepath, std::size_t startOffset) {
	FileStream stream{filepath};
	if (!stream) {
		return {};
	}
	stream.seek_in_u(startOffset);
	return stream.read_bytes(std::filesystem::file_size(filepath) - startOffset);
}

std::vector<std::byte> fs::readFileBuffer(const std::filesystem::path& filepath, bool& exists, std::size_t startOffset) {
	auto buffer = readFileBuffer(filepath, startOffset);
	exists = !buffer.empty() || std::filesystem::exists(filepath);
	return buffer;
}

std::string fs::readFileText(const std::filesystem::path& filepath, std::size_t startOffset) {
	FileStream stream{filepath};
	if (!stream) {
		return {};
	}
	stream.seek_in_u(startOffset);
	return stream.read_string();
}

std::string fs::readFileText(const std::filesystem::path& filepath, bool& exists, std::size_t startOffset) {
	auto text = readFileText(filepath, startOffset);
	exists = !text.empty() || std::filesystem::exists(filepath);
	return text;
}

bool fs::writeFileBuffer(const std::filesystem::path& filepath, std::span<const std::byte> buffer) {
	FileStream stream{filepath, FileStream::OPT_TRUNCATE | FileStream::OPT_CREATE_IF_NONEXISTENT};
	if (!stream) {
		return false;
	}
	stream.seek_out(0).write(buffer);
	return true;
}

bool fs::writeFileText(const std::filesystem::path& filepath, std::string_view text) {
	FileStream stream{filepath, FileStream::OPT_TRUNCATE | FileStream::OPT_CREATE_IF_NONEXISTENT};
	if (!stream) {
		return false;
	}
	stream.seek_out(0).write(text, false);
	return true;
}
