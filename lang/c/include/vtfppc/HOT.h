#pragma once

#include <sourceppc/API.h>
#include <sourceppc/Buffer.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
	VTFPP_HOT_RECT_FLAG_NONE              = 0,
	VTFPP_HOT_RECT_FLAG_RANDOM_ROTATION   = 1 << 0,
	VTFPP_HOT_RECT_FLAG_RANDOM_REFLECTION = 1 << 1,
	VTFPP_HOT_RECT_FLAG_IS_ALTERNATE      = 1 << 2,
} vtfpp_hot_rect_flags_e;

typedef struct {
	uint8_t flags;
	uint16_t x1;
	uint16_t y1;
	uint16_t x2;
	uint16_t y2;
} vtfpp_hot_rect_t;

#define VTFPP_HOT_RECT_INVALID (SOURCEPP_CAST_CTOR(vtfpp_hot_rect_t) {.flags = VTFPP_HOT_RECT_FLAG_NONE, .x1 = 0, .y1 = 0, .x2 = 0, .y2 = 0})

typedef void* vtfpp_hot_handle_t;

#ifdef __cplusplus
} // extern "C"
#endif

// REQUIRES MANUAL FREE: vtfpp_hot_close
SOURCEPP_API vtfpp_hot_handle_t vtfpp_hot_create();

// REQUIRES MANUAL FREE: vtfpp_hot_close
SOURCEPP_API vtfpp_hot_handle_t vtfpp_hot_open_from_mem(const unsigned char* hotData, size_t hotLen);

// REQUIRES MANUAL FREE: vtfpp_hot_close
SOURCEPP_API vtfpp_hot_handle_t vtfpp_hot_open_from_file(const char* hotPath);

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

// REQUIRES MANUAL FREE: sourcepp_buffer_free
SOURCEPP_API sourcepp_buffer_t vtfpp_hot_bake(vtfpp_hot_handle_t handle);

SOURCEPP_API int vtfpp_hot_bake_to_file(vtfpp_hot_handle_t handle, const char* hotPath);

SOURCEPP_API void vtfpp_hot_close(vtfpp_hot_handle_t* handle);
