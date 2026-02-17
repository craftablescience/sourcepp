#pragma once

#include <sourceppc/API.h>
#include <sourceppc/Buffer.h>

SOURCEPP_EXTERN typedef enum {
	VTFPP_HOT_RECT_FLAG_NONE              = 0,
	VTFPP_HOT_RECT_FLAG_RANDOM_ROTATION   = 1 << 0,
	VTFPP_HOT_RECT_FLAG_RANDOM_REFLECTION = 1 << 1,
	VTFPP_HOT_RECT_FLAG_IS_ALTERNATE      = 1 << 2,
} vtfpp_hot_rect_flags_e;

SOURCEPP_EXTERN typedef struct {
	uint8_t flags;
	uint16_t x1;
	uint16_t y1;
	uint16_t x2;
	uint16_t y2;
} vtfpp_hot_rect_t;

#define VTFPP_HOT_RECT_INVALID (SOURCEPP_CAST_CTOR(vtfpp_hot_rect_t) {.flags = VTFPP_HOT_RECT_FLAG_NONE, .x1 = 0, .y1 = 0, .x2 = 0, .y2 = 0})

typedef void* vtfpp_hot_handle_t;

SOURCEPP_API vtfpp_hot_handle_t vtfpp_hot_create(); // REQUIRES MANUAL FREE: vtfpp_hot_close
SOURCEPP_API vtfpp_hot_handle_t vtfpp_hot_open_from_mem(const unsigned char* hotData, size_t hotLen); // REQUIRES MANUAL FREE: vtfpp_hot_close
SOURCEPP_API vtfpp_hot_handle_t vtfpp_hot_open_from_file(const char* hotPath); // REQUIRES MANUAL FREE: vtfpp_hot_close
SOURCEPP_API void vtfpp_hot_close(vtfpp_hot_handle_t* handle);
SOURCEPP_API int vtfpp_hot_is_valid(vtfpp_hot_handle_t handle);
SOURCEPP_API uint8_t vtfpp_hot_get_version(vtfpp_hot_handle_t handle);
SOURCEPP_API void vtfpp_hot_set_version(vtfpp_hot_handle_t handle, uint8_t version);
SOURCEPP_API uint8_t vtfpp_hot_get_flags(vtfpp_hot_handle_t handle);
SOURCEPP_API void vtfpp_hot_set_flags(vtfpp_hot_handle_t handle, uint8_t flags);
SOURCEPP_API uint16_t vtfpp_hot_get_rect_count(vtfpp_hot_handle_t handle);
SOURCEPP_API vtfpp_hot_rect_t vtfpp_hot_get_rect_at_index(vtfpp_hot_handle_t handle, uint16_t index);
SOURCEPP_API void vtfpp_hot_set_rect_at_index(vtfpp_hot_handle_t handle, uint16_t index, vtfpp_hot_rect_t rect);
SOURCEPP_API void vtfpp_hot_add_rect(vtfpp_hot_handle_t handle, vtfpp_hot_rect_t rect);
SOURCEPP_API void vtfpp_hot_remove_rect_at_index(vtfpp_hot_handle_t handle, uint16_t index);
SOURCEPP_API sourcepp_buffer_t vtfpp_hot_bake(vtfpp_hot_handle_t handle); // REQUIRES MANUAL FREE: sourcepp_buffer_free
SOURCEPP_API int vtfpp_hot_bake_to_file(vtfpp_hot_handle_t handle, const char* hotPath);

// C++ conversion routines
#ifdef __cplusplus

#include <vtfpp/HOT.h>

namespace sourceppc::convert {

inline vtfpp::HOT::Rect::Flags cast(vtfpp_hot_rect_flags_e flags) {
	return static_cast<vtfpp::HOT::Rect::Flags>(flags);
}

inline vtfpp_hot_rect_flags_e cast(vtfpp::HOT::Rect::Flags flags) {
	return static_cast<vtfpp_hot_rect_flags_e>(flags);
}

inline vtfpp::HOT::Rect cast(const vtfpp_hot_rect_t& value) {
	return {
		.flags = cast(static_cast<vtfpp_hot_rect_flags_e>(value.flags)),
		.x1 = value.x1,
		.y1 = value.y1,
		.x2 = value.x2,
		.y2 = value.y2,
	};
}

inline vtfpp_hot_rect_t cast(const vtfpp::HOT::Rect& value) {
	return {
		.flags = static_cast<uint8_t>(cast(value.flags)),
		.x1 = value.x1,
		.y1 = value.y1,
		.x2 = value.x2,
		.y2 = value.y2,
	};
}

} // namespace sourceppc::convert

#endif
