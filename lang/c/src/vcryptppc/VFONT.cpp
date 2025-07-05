#include <vcryptppc/VFONT.h>

#include <sourceppc/Convert.hpp>
#include <sourceppc/Helpers.h>
#include <vcryptpp/vcryptpp.h>

using namespace vcryptpp;

SOURCEPP_STATIC(vcryptpp, vfont, sourcepp_buffer_t, decrypt, const unsigned char* buffer, size_t bufferLen) {
	SOURCEPP_EARLY_RETURN_VAL(buffer, SOURCEPP_BUFFER_INVALID);
	SOURCEPP_EARLY_RETURN_VAL(bufferLen, SOURCEPP_BUFFER_INVALID);

	return Convert::toBuffer(VFONT::decrypt(std::span{reinterpret_cast<const std::byte*>(buffer), bufferLen}));
}
