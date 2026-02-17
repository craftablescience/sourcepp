#include <vtfppc/ImageQuantize.h>

#include <sourceppc/Helpers.h>

using namespace sourceppc;
using namespace vtfpp;

SOURCEPP_API sourcepp_buffer_t vtfpp_image_quantize_convert_p8_image_data_to_bgra8888(const unsigned char* paletteData, size_t paletteDataLen, const unsigned char* imageData, size_t imageDataLen) {
	SOURCEPP_EARLY_RETURN_VAL(paletteData, SOURCEPP_BUFFER_INVALID);
	SOURCEPP_EARLY_RETURN_VAL(paletteDataLen, SOURCEPP_BUFFER_INVALID);
	SOURCEPP_EARLY_RETURN_VAL(imageData, SOURCEPP_BUFFER_INVALID);
	SOURCEPP_EARLY_RETURN_VAL(imageDataLen, SOURCEPP_BUFFER_INVALID);

	return convert::toBuffer(ImageQuantize::convertP8ImageDataToBGRA8888({reinterpret_cast<const std::byte*>(paletteData), paletteDataLen}, {reinterpret_cast<const std::byte*>(imageData), imageDataLen}));
}
