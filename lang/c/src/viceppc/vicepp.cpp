#include <viceppc/vicepp.h>

#include <sourceppc/Convert.hpp>
#include <sourceppc/Helpers.h>
#include <vicepp/vicepp.h>

SOURCEPP_API sourcepp_buffer_t vicepp_encrypt(const unsigned char* buffer, size_t bufferLen, const char* code) {
	SOURCEPP_EARLY_RETURN_VAL(buffer, SOURCEPP_BUFFER_INVALID);
	SOURCEPP_EARLY_RETURN_VAL(bufferLen, SOURCEPP_BUFFER_INVALID);
	SOURCEPP_EARLY_RETURN_VAL(code, SOURCEPP_BUFFER_INVALID);

	return Convert::toBuffer(vicepp::encrypt(std::span<const std::byte>{reinterpret_cast<const std::byte*>(buffer), bufferLen}, code));
}

SOURCEPP_API sourcepp_buffer_t vicepp_decrypt(const unsigned char* buffer, size_t bufferLen, const char* code) {
	SOURCEPP_EARLY_RETURN_VAL(buffer, SOURCEPP_BUFFER_INVALID);
	SOURCEPP_EARLY_RETURN_VAL(bufferLen, SOURCEPP_BUFFER_INVALID);
	SOURCEPP_EARLY_RETURN_VAL(code, SOURCEPP_BUFFER_INVALID);

	return Convert::toBuffer(vicepp::decrypt(std::span<const std::byte>{reinterpret_cast<const std::byte*>(buffer), bufferLen}, code));
}
