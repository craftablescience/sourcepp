#include <sourcepp/crypto/RSA.h>

#include <array>
#include <memory>
#include <random>

#include <tomcrypt.h>

#include <sourcepp/crypto/Globals.h>
#include <sourcepp/String.h>

using namespace sourcepp;

namespace {

constexpr auto SHA256_ENTROPY_SIZE = 64;

} // namespace

std::pair<std::string, std::string> crypto::computeSHA256KeyPair(uint16_t size) {
	if (!LTM_MATH || SHA256_INDEX < 0 || YARROW_INDEX < 0 || size < 8) {
		return {};
	}
	size /= 8;

	const std::unique_ptr<prng_state, void(*)(prng_state*)> prng{new prng_state, [](prng_state* p) { yarrow_done(p); delete p; }};
	yarrow_start(prng.get());
	{
		std::array<unsigned char, SHA256_ENTROPY_SIZE> entropy{};
		std::random_device rd;
		std::uniform_int_distribution<> dist{0, std::numeric_limits<unsigned char>::max()};
		for (auto& i : entropy) {
			i = static_cast<unsigned char>(dist(rd));
		}
		yarrow_add_entropy(entropy.data(), entropy.size(), prng.get());
	}
	yarrow_ready(prng.get());

	const std::unique_ptr<rsa_key, void(*)(rsa_key*)> key{new rsa_key, [](rsa_key* k) { rsa_free(k); delete k; }};
	if (rsa_make_key(prng.get(), YARROW_INDEX, size, 65537, key.get()) != CRYPT_OK) {
		return {};
	}

	std::vector<unsigned char> privateKeyData(size * 16);
	unsigned long privateKeyLen = privateKeyData.size();
	if (rsa_export(privateKeyData.data(), &privateKeyLen, PK_PRIVATE, key.get()) != CRYPT_OK) {
		return {};
	}
	privateKeyData.resize(privateKeyLen);

	std::vector<unsigned char> publicKeyData(size * 16);
	unsigned long publicKeyLen = publicKeyData.size();
	if (rsa_export(publicKeyData.data(), &publicKeyLen, PK_PUBLIC, key.get()) != CRYPT_OK) {
		return {};
	}
	publicKeyData.resize(publicKeyLen);

	return {
		string::encodeHex({reinterpret_cast<const std::byte*>(privateKeyData.data()), privateKeyData.size()}),
		string::encodeHex({reinterpret_cast<const std::byte*>(publicKeyData.data()), publicKeyData.size()}),
	};
}

bool crypto::verifySHA256PublicKey(std::span<const std::byte> buffer, std::span<const std::byte> publicKey, std::span<const std::byte> signature) {
	if (!LTM_MATH || SHA256_INDEX < 0 || buffer.empty()) {
		return false;
	}

	const std::unique_ptr<rsa_key, void(*)(rsa_key*)> key{new rsa_key, [](rsa_key* k) { rsa_free(k); delete k; }};
	if (rsa_import(reinterpret_cast<const unsigned char*>(publicKey.data()), publicKey.size(), key.get()) != CRYPT_OK) {
		return false;
	}

	const auto sha256 = computeSHA256(buffer);
	ltc_rsa_op_parameters params{ .params = { .hash_idx = SHA256_INDEX }, .padding = LTC_PKCS_1_V1_5 };
	int stat = 0;
	return rsa_verify_hash_v2(reinterpret_cast<const unsigned char*>(signature.data()), signature.size(), reinterpret_cast<const unsigned char*>(sha256.data()), sha256.size(), &params, &stat, key.get()) == CRYPT_OK && stat;
}

std::vector<std::byte> crypto::signDataWithSHA256PrivateKey(std::span<const std::byte> buffer, std::span<const std::byte> privateKey) {
	if (!LTM_MATH || SHA256_INDEX < 0 || buffer.empty()) {
		return {};
	}

	const std::unique_ptr<rsa_key, void(*)(rsa_key*)> key{new rsa_key, [](rsa_key* k) { rsa_free(k); delete k; }};
	if (rsa_import(reinterpret_cast<const unsigned char*>(privateKey.data()), privateKey.size(), key.get()) != CRYPT_OK) {
		return {};
	}

	const auto sha256 = computeSHA256(buffer);
	unsigned long signatureLen = rsa_get_size(key.get());
	std::vector<std::byte> signature(signatureLen);
	ltc_rsa_op_parameters params{ .params = { .hash_idx = SHA256_INDEX }, .padding = LTC_PKCS_1_V1_5 };
	return rsa_sign_hash_v2(reinterpret_cast<const unsigned char*>(sha256.data()), sha256.size(), reinterpret_cast<unsigned char*>(signature.data()), &signatureLen, &params, key.get()) == CRYPT_OK ? signature : std::vector<std::byte>{};
}
