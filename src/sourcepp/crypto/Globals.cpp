#include <sourcepp/crypto/Globals.h>

#include <tomcrypt.h>

namespace sourcepp::crypto {

const bool LTM_MATH = [] {
	ltc_mp = ltm_desc;
	return true;
}();

const int AES_INDEX    = register_cipher(&aes_desc);
const int SHA256_INDEX = register_hash(&sha256_desc);
const int YARROW_INDEX = register_prng(&yarrow_desc);

} // namespace sourcepp::crypto
