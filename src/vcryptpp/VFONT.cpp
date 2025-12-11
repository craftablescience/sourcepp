#include <vcryptpp/VFONT.h>

#include <limits>
#include <random>
#include <string_view>

#include <BufferStream.h>

using namespace vcryptpp;

std::vector<std::byte> VFONT::encrypt(std::span<const std::byte> data, uint8_t saltLength) {
	std::vector<std::byte> out;
	BufferStream stream{out};

	static std::random_device random_device{};
	static std::mt19937 generator{random_device()};
	std::uniform_int_distribution<> distribution{0, (std::numeric_limits<uint8_t>::max())};

	std::vector<std::byte> salt;
	salt.resize(saltLength);

	uint8_t magic = MAGIC;
	for (int i = 0; i < saltLength; i++) {
		salt[i] = static_cast<std::byte>(distribution(generator));
		magic ^= static_cast<uint8_t>(salt[i]) + MAGIC;
	}

	for (auto byte : data) {
		const uint8_t encrypted = static_cast<uint8_t>(byte) ^ magic;
		stream << encrypted;
		magic = encrypted + MAGIC;
	}
	stream
		.write(salt)
		.write<uint8_t>(saltLength + 1)
		.write(SIGNATURE, false);

	out.resize(stream.size());
	return out;
}

std::vector<std::byte> VFONT::decrypt(std::span<const std::byte> data) {
	BufferStreamReadOnly reader{data};

	if (reader.seek(SIGNATURE.length(), std::ios::end).read_string(SIGNATURE.length()) != SIGNATURE) {
		return {};
	}
	reader.seek(SIGNATURE.length() + sizeof(uint8_t), std::ios::end);

	const auto bytes = reader.read<uint8_t>();

	std::vector<std::byte> out;
	out.resize(reader.size() - SIGNATURE.length() - bytes);

	reader.seek(-static_cast<int>(bytes), std::ios::cur);
	uint8_t magic = MAGIC;
	for (int i = 0; i < bytes - 1; i++) {
		magic ^= reader.read<uint8_t>() + MAGIC;
	}

	reader.seek(0);
	for (auto& outByte : out) {
		const auto byte = reader.read<uint8_t>();
		outByte = static_cast<std::byte>(byte ^ magic);
		magic = byte + MAGIC;
	}

	return out;
}
