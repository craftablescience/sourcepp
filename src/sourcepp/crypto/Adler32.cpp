#include <sourcepp/crypto/Adler32.h>

#include <BufferStream.h>
#include <tomcrypt.h>

#include <sourcepp/crypto/Globals.h>

using namespace sourcepp;

uint32_t crypto::computeAdler32(std::span<const std::byte> buffer) {
	if (!LTM_MATH || buffer.empty()) {
		return 0;
	}

	adler32_state adler32{};
	// do NOT call init! must start zeroed, init changes this (necessary for GCF)
	adler32_update(&adler32, reinterpret_cast<const unsigned char*>(buffer.data()), buffer.size());

	uint32_t final;
	adler32_finish(&adler32, &final, sizeof(final));
	BufferStream::swap_endian(&final);
	return final;
}
