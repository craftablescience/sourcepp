#include <vtfppc/VBF.h>

#include <sourceppc/Helpers.h>

using namespace sourceppc;
using namespace vtfpp;

const uint32_t VTFPP_VBF_SIGNATURE = VBF_SIGNATURE;

SOURCEPP_API vtfpp_vbf_handle_t vtfpp_vbf_open_from_mem(const unsigned char* buffer, size_t bufferLen) {
	SOURCEPP_EARLY_RETURN_VAL(buffer, nullptr);
	SOURCEPP_EARLY_RETURN_VAL(bufferLen, nullptr);

	return new VBF{{reinterpret_cast<const std::byte*>(buffer), bufferLen}};
}

SOURCEPP_API vtfpp_vbf_handle_t vtfpp_vbf_open_from_file(const char* vbfPath) {
	SOURCEPP_EARLY_RETURN_VAL(vbfPath, nullptr);

	return new VBF{vbfPath};
}

SOURCEPP_API void vtfpp_vbf_close(vtfpp_vbf_handle_t* handle) {
	SOURCEPP_EARLY_RETURN(handle);

	delete convert::handle<VBF>(*handle);
	*handle = nullptr;
}

SOURCEPP_API int vtfpp_vbf_is_valid(vtfpp_vbf_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, false);

	return convert::handle<VBF>(handle)->operator bool();
}

SOURCEPP_API uint16_t vtfpp_vbf_get_page_width(vtfpp_vbf_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, 0);

	return convert::handle<VBF>(handle)->getPageSize()[0];
}

SOURCEPP_API uint16_t vtfpp_vbf_get_page_height(vtfpp_vbf_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, 0);

	return convert::handle<VBF>(handle)->getPageSize()[1];
}

SOURCEPP_API uint16_t vtfpp_vbf_get_max_glyph_width(vtfpp_vbf_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, 0);

	return convert::handle<VBF>(handle)->getMaxGlyphSize()[0];
}

SOURCEPP_API uint16_t vtfpp_vbf_get_max_glyph_height(vtfpp_vbf_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, 0);

	return convert::handle<VBF>(handle)->getMaxGlyphSize()[1];
}

SOURCEPP_API vtfpp_vbf_flags_e vtfpp_vbf_get_flags(vtfpp_vbf_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, VTFPP_VBF_FLAG_NONE);

	return convert::cast(convert::handle<VBF>(handle)->getFlags());
}

SOURCEPP_API uint16_t vtfpp_vbf_get_ascent(vtfpp_vbf_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, 0);

	return convert::handle<VBF>(handle)->getAscent();
}

SOURCEPP_API vtfpp_vbf_glyph_t vtfpp_vbf_get_glyph(vtfpp_vbf_handle_t handle, uint8_t index) {
	SOURCEPP_EARLY_RETURN_VAL(handle, VTFPP_VBF_GLYPH_INVALID);

	return convert::cast(convert::handle<VBF>(handle)->getGlyphs().at(index));
}
