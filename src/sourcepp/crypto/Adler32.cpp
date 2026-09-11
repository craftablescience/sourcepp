#include <sourcepp/crypto/Adler32.h>

#include <BufferStream.h>

using namespace sourcepp;

/**
 * Adler-32 checksum algorithm
 * Written and placed in the public domain by Wei Dai
 * Adapted for libtomcrypt by Steffen Jaeckel
 * Adapted for sourcepp by Laura Lewis
 */
namespace tomcrypt {

namespace {

struct adler32_state {
	uint16_t s[2];
};

constexpr uint32_t s_adler32_base = 65521;

[[nodiscard]] bool adler32_update(adler32_state& ctx, const uint8_t* input, uint64_t length) {
	if (!input) {
		return false;
	}

	uint64_t s1 = ctx.s[0];
	uint64_t s2 = ctx.s[1];

	if (length % 8 != 0) {
		do {
			s1 += *input++;
			s2 += s1;
			length--;
		} while (length % 8 != 0);

		if (s1 >= s_adler32_base) {
			s1 -= s_adler32_base;
		}
		s2 %= s_adler32_base;
	}

	while (length > 0) {
		s1 += input[0];
		s2 += s1;
		s1 += input[1];
		s2 += s1;
		s1 += input[2];
		s2 += s1;
		s1 += input[3];
		s2 += s1;
		s1 += input[4];
		s2 += s1;
		s1 += input[5];
		s2 += s1;
		s1 += input[6];
		s2 += s1;
		s1 += input[7];
		s2 += s1;

		length -= 8;
		input += 8;

		if (s1 >= s_adler32_base) {
			s1 -= s_adler32_base;
		}
		s2 %= s_adler32_base;
	}

	if (s1 >= s_adler32_base || s2 >= s_adler32_base) {
		return false;
	}

	ctx.s[0] = static_cast<uint16_t>(s1);
	ctx.s[1] = static_cast<uint16_t>(s2);
	return true;
}

[[nodiscard]] uint32_t adler32_finish(const adler32_state& ctx) {
	return (static_cast<uint32_t>(ctx.s[1]) << 16) | (static_cast<uint32_t>(ctx.s[0]) << 0);
}

} // namespace

} // namespace tomcrypt

uint32_t crypto::computeAdler32(std::span<const std::byte> buffer) {
	if (buffer.empty()) {
		return 0;
	}

	// NOTE: tomcrypt adler32 initializes state to (1, 0). GCF needs (0, 0) so we are skipping the standard init
	tomcrypt::adler32_state adler32{};
	if (!tomcrypt::adler32_update(adler32, reinterpret_cast<const uint8_t*>(buffer.data()), buffer.size())) {
		return 0;
	}
	return tomcrypt::adler32_finish(adler32);
}
