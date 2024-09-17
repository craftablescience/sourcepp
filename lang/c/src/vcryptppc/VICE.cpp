#include <vcryptppc/VICE.h>

#include <sourceppc/Convert.hpp>
#include <sourceppc/Helpers.h>
#include <vcryptpp/vcryptpp.h>

using namespace vcryptpp;

SOURCEPP_API sourcepp_buffer_t vcryptpp_vice_encrypt(const unsigned char* buffer, size_t bufferLen, const char* code) {
	SOURCEPP_EARLY_RETURN_VAL(buffer, SOURCEPP_BUFFER_INVALID);
	SOURCEPP_EARLY_RETURN_VAL(bufferLen, SOURCEPP_BUFFER_INVALID);
	SOURCEPP_EARLY_RETURN_VAL(code, SOURCEPP_BUFFER_INVALID);

	return Convert::toBuffer(VICE::encrypt(std::span<const std::byte>{reinterpret_cast<const std::byte*>(buffer), bufferLen}, code));
}

SOURCEPP_API sourcepp_buffer_t vcryptpp_vice_decrypt(const unsigned char* buffer, size_t bufferLen, const char* code) {
	SOURCEPP_EARLY_RETURN_VAL(buffer, SOURCEPP_BUFFER_INVALID);
	SOURCEPP_EARLY_RETURN_VAL(bufferLen, SOURCEPP_BUFFER_INVALID);
	SOURCEPP_EARLY_RETURN_VAL(code, SOURCEPP_BUFFER_INVALID);

	return Convert::toBuffer(VICE::decrypt(std::span<const std::byte>{reinterpret_cast<const std::byte*>(buffer), bufferLen}, code));
}
