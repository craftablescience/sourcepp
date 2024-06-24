#include <sourcepp/crypto/CRC32.h>

#include <cryptopp/crc.h>

using namespace sourcepp;

uint32_t crypto::computeCRC32(const std::vector<std::byte>& buffer) {
	return computeCRC32(buffer.data(), buffer.size());
}

uint32_t crypto::computeCRC32(const std::byte* buffer, std::size_t len) {
	// Make sure this is right
	static_assert(CryptoPP::CRC32::DIGESTSIZE == sizeof(uint32_t));

	CryptoPP::CRC32 crc32;
	crc32.Update(reinterpret_cast<const CryptoPP::byte*>(buffer), len);

	uint32_t final;
	crc32.Final(reinterpret_cast<CryptoPP::byte*>(&final));
	return final;
}
