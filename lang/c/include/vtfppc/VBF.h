#pragma once

#include <sourceppc/API.h>

SOURCEPP_EXTERNVAR const uint32_t VTFPP_VBF_SIGNATURE;

SOURCEPP_EXTERN typedef enum {
	VTFPP_VBF_FLAG_NONE        = 0,
	VTFPP_VBF_FLAG_BOLD        = 1 << 0,
	VTFPP_VBF_FLAG_ITALIC      = 1 << 1,
	VTFPP_VBF_FLAG_OUTLINE     = 1 << 2,
	VTFPP_VBF_FLAG_DROP_SHADOW = 1 << 3,
	VTFPP_VBF_FLAG_BLUR        = 1 << 4,
	VTFPP_VBF_FLAG_SCANLINE    = 1 << 5,
	VTFPP_VBF_FLAG_ANTIALIASED = 1 << 6,
	VTFPP_VBF_FLAG_CUSTOM      = 1 << 7,
} vtfpp_vbf_flags_e;

SOURCEPP_EXTERN typedef struct {
	uint16_t x;
	uint16_t y;
	uint16_t w;
	uint16_t h;
	int16_t a;
	int16_t b;
	int16_t c;
} vtfpp_vbf_glyph_t;

#define VTFPP_VBF_GLYPH_INVALID (SOURCEPP_CAST_CTOR(vtfpp_vbf_glyph_t) {.x = 0, .y = 0, .w = 0, .h = 0, .a = 0, .b = 0, .c = 0})

typedef void* vtfpp_vbf_handle_t;

SOURCEPP_API vtfpp_vbf_handle_t vtfpp_vbf_open_from_mem(const unsigned char* buffer, size_t bufferLen); // REQUIRES MANUAL FREE: vtfpp_vbf_close
SOURCEPP_API vtfpp_vbf_handle_t vtfpp_vbf_open_from_file(const char* vbfPath); // REQUIRES MANUAL FREE: vtfpp_vbf_close
SOURCEPP_API void vtfpp_vbf_close(vtfpp_vbf_handle_t* handle);
SOURCEPP_API int vtfpp_vbf_is_valid(vtfpp_vbf_handle_t handle);
SOURCEPP_API uint16_t vtfpp_vbf_get_page_width(vtfpp_vbf_handle_t handle);
SOURCEPP_API uint16_t vtfpp_vbf_get_page_height(vtfpp_vbf_handle_t handle);
SOURCEPP_API uint16_t vtfpp_vbf_get_max_glyph_width(vtfpp_vbf_handle_t handle);
SOURCEPP_API uint16_t vtfpp_vbf_get_max_glyph_height(vtfpp_vbf_handle_t handle);
SOURCEPP_API vtfpp_vbf_flags_e vtfpp_vbf_get_flags(vtfpp_vbf_handle_t handle);
SOURCEPP_API uint16_t vtfpp_vbf_get_ascent(vtfpp_vbf_handle_t handle);
SOURCEPP_API vtfpp_vbf_glyph_t vtfpp_vbf_get_glyph(vtfpp_vbf_handle_t handle, uint8_t index);

// C++ conversion routines
#ifdef __cplusplus

#include <vtfpp/VBF.h>

namespace sourceppc::convert {

inline vtfpp::VBF::Flags cast(vtfpp_vbf_flags_e flags) {
	return static_cast<vtfpp::VBF::Flags>(flags);
}

inline vtfpp_vbf_flags_e cast(vtfpp::VBF::Flags flags) {
	return static_cast<vtfpp_vbf_flags_e>(flags);
}

inline vtfpp::VBF::Glyph cast(const vtfpp_vbf_glyph_t& value) {
	return {
		.position = {
			value.x,
			value.y,
		},
		.size = {
			value.w,
			value.h,
		},
		.abcSpacing = {
			value.a,
			value.b,
			value.c,
		},
	};
}

inline vtfpp_vbf_glyph_t cast(const vtfpp::VBF::Glyph& value) {
	return {
		.x = value.position[0],
		.y = value.position[1],
		.w = value.size[0],
		.h = value.size[1],
		.a = value.abcSpacing[0],
		.b = value.abcSpacing[1],
		.c = value.abcSpacing[2],
	};
}

} // namespace sourceppc::convert

#endif
