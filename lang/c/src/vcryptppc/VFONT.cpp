#include <vcryptppc/VFONT.h>

#include <sourceppc/Helpers.h>

using namespace sourceppc;
using namespace vcryptpp;

const char* VCRYPTPP_VFONT_SIGNATURE = VFONT::SIGNATURE.data();

const uint8_t VCRYPTPP_VFONT_MAGIC = VFONT::MAGIC;

SOURCEPP_API sourcepp_buffer_t vcryptpp_vfont_encrypt(const unsigned char* buffer, size_t bufferLen, uint8_t saltLen) {
	SOURCEPP_EARLY_RETURN_VAL(buffer, SOURCEPP_BUFFER_INVALID);
	SOURCEPP_EARLY_RETURN_VAL(bufferLen, SOURCEPP_BUFFER_INVALID);

	return convert::toBuffer(VFONT::encrypt(std::span{reinterpret_cast<const std::byte*>(buffer), bufferLen}, saltLen));
}

SOURCEPP_API sourcepp_buffer_t vcryptpp_vfont_decrypt(const unsigned char* buffer, size_t bufferLen) {
	SOURCEPP_EARLY_RETURN_VAL(buffer, SOURCEPP_BUFFER_INVALID);
	SOURCEPP_EARLY_RETURN_VAL(bufferLen, SOURCEPP_BUFFER_INVALID);

	return convert::toBuffer(VFONT::decrypt(std::span{reinterpret_cast<const std::byte*>(buffer), bufferLen}));
}
