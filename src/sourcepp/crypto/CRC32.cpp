#include <sourcepp/crypto/CRC32.h>

#include <BufferStream.h>
#include <tomcrypt.h>

#include <sourcepp/crypto/Globals.h>

using namespace sourcepp;

uint32_t crypto::computeCRC32(std::span<const std::byte> buffer) {
	if (!LTM_MATH || buffer.empty()) {
		return 0;
	}

	crc32_state crc32;
	crc32_init(&crc32);
	crc32_update(&crc32, reinterpret_cast<const unsigned char*>(buffer.data()), buffer.size());

	uint32_t final;
	crc32_finish(&crc32, &final, sizeof(final));
	BufferStream::swap_endian(&final);
	return final;
}
