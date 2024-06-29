/*
 * Header file for the C++ ICE encryption class.
 *
 * Written by Matthew Kwan - July 1996
 */

#pragma once

#include <cstdint>
#include <memory>

class IceKey {
private:
	struct IceSubkey {
		std::uint32_t val[3];
	};

public:
	explicit IceKey(int n = 0);

	void set(const unsigned char* key);

	void encrypt(const unsigned char* plaintext, unsigned char* ciphertext) const;

	void decrypt(const unsigned char* ciphertext, unsigned char* plaintext) const;

	[[nodiscard]] int keySize() const;

	[[nodiscard]] static constexpr int blockSize() { return 8; };

private:
	int size;
	int rounds;
	std::unique_ptr<IceSubkey[]> keySchedule;

	[[nodiscard]] static std::uint32_t ice_f(std::uint32_t p, const IceSubkey* sk);

	void scheduleBuild(std::uint16_t* k, int n, const std::int32_t* keyRot);
};
