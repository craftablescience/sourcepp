#include <vcryptpp/VFONT.h>

#include <string_view>

#include <BufferStream.h>

using namespace vcryptpp;

std::vector<std::byte> VFONT::decrypt(std::span<const std::byte> data) {
	BufferStreamReadOnly reader{data.data(), data.size()};

	if (reader.seek(SIGNATURE.length(), std::ios::end).read_string(SIGNATURE.length()) != SIGNATURE) {
		return {};
	}
	reader.seek(SIGNATURE.length() + sizeof(uint8_t), std::ios::end);

	auto bytes = reader.read<uint8_t>();

	std::vector<std::byte> out;
	out.resize(reader.size() - SIGNATURE.length() - bytes);

	reader.seek(-static_cast<int>(bytes), std::ios::cur);
	uint8_t magic = MAGIC;
	for (int i = 0; i < bytes - 1; i++) {
		magic ^= reader.read<uint8_t>() + MAGIC;
	}

	reader.seek(0);
	for (auto& outByte : out) {
		auto byte = reader.read<uint8_t>();
		outByte = static_cast<std::byte>(byte ^ magic);
		magic = byte + MAGIC;
	}

	return out;
}
