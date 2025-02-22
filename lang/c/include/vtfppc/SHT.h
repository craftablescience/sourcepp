#pragma once

#include <sourceppc/API.h>
#include <sourceppc/Buffer.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	float x1;
	float y1;
	float x2;
	float y2;
} vtfpp_sht_frame_bounds_t;

typedef struct {
	float duration;
	vtfpp_sht_frame_bounds_t bounds[4];
} vtfpp_sht_frame_t;

#define VTFPP_SHT_FRAME_INVALID (SOURCEPP_CAST_CTOR(vtfpp_sht_frame_t) {.duration = 0.f})

typedef struct {
	uint32_t id;
	int32_t loop;
	uint32_t frameCount;
	float durationTotal;
} vtfpp_sht_sequence_t;

#define VTFPP_SHT_SEQUENCE_INVALID (SOURCEPP_CAST_CTOR(vtfpp_sht_sequence_t) {.id = 0, .loop = 0, .frameCount = 0, .durationTotal = 0.f})

typedef void* vtfpp_sht_handle_t;

#ifdef __cplusplus
} // extern "C"
#endif

// REQUIRES MANUAL FREE: vtfpp_sht_close
SOURCEPP_API vtfpp_sht_handle_t vtfpp_sht_create();

// REQUIRES MANUAL FREE: vtfpp_sht_close
SOURCEPP_API vtfpp_sht_handle_t vtfpp_sht_open_from_mem(const unsigned char* shtData, size_t shtLen);

// REQUIRES MANUAL FREE: vtfpp_sht_close
SOURCEPP_API vtfpp_sht_handle_t vtfpp_sht_open_from_file(const char* shtPath);

SOURCEPP_API int vtfpp_sht_is_valid(vtfpp_sht_handle_t handle);

SOURCEPP_API uint32_t vtfpp_sht_get_version(vtfpp_sht_handle_t handle);

SOURCEPP_API void vtfpp_sht_set_version(vtfpp_sht_handle_t handle, uint32_t version);

SOURCEPP_API uint32_t vtfpp_sht_get_sequences_count(vtfpp_sht_handle_t handle);

SOURCEPP_API vtfpp_sht_sequence_t vtfpp_sht_get_sequence_at_index(vtfpp_sht_handle_t handle, uint32_t index);

SOURCEPP_API vtfpp_sht_sequence_t vtfpp_sht_get_sequence_with_id(vtfpp_sht_handle_t handle, uint32_t id);

SOURCEPP_API void vtfpp_sht_set_sequence_at_index(vtfpp_sht_handle_t handle, uint32_t index, vtfpp_sht_sequence_t sequence);

SOURCEPP_API void vtfpp_sht_set_sequence_with_id(vtfpp_sht_handle_t handle, uint32_t id, vtfpp_sht_sequence_t sequence);

SOURCEPP_API void vtfpp_sht_add_sequence(vtfpp_sht_handle_t handle, vtfpp_sht_sequence_t sequence);

SOURCEPP_API void vtfpp_sht_remove_sequence_at_index(vtfpp_sht_handle_t handle, uint32_t index);

SOURCEPP_API void vtfpp_sht_remove_sequence_with_id(vtfpp_sht_handle_t handle, uint32_t id);

SOURCEPP_API vtfpp_sht_frame_t vtfpp_sht_get_frame(vtfpp_sht_handle_t handle, uint32_t sequenceID, uint32_t index);

SOURCEPP_API void vtfpp_sht_set_frame(vtfpp_sht_handle_t handle, uint32_t sequenceID, uint32_t index, vtfpp_sht_frame_t frame);

SOURCEPP_API void vtfpp_sht_add_frame(vtfpp_sht_handle_t handle, uint32_t sequenceID, vtfpp_sht_frame_t frame);

SOURCEPP_API void vtfpp_sht_remove_frame(vtfpp_sht_handle_t handle, uint32_t sequenceID, uint32_t index);

SOURCEPP_API uint8_t vtfpp_sht_get_frame_bounds_count(vtfpp_sht_handle_t handle);

// REQUIRES MANUAL FREE: sourcepp_buffer_free
SOURCEPP_API sourcepp_buffer_t vtfpp_sht_bake(vtfpp_sht_handle_t handle);

SOURCEPP_API int vtfpp_sht_bake_to_file(vtfpp_sht_handle_t handle, const char* shtPath);

SOURCEPP_API void vtfpp_sht_close(vtfpp_sht_handle_t* handle);
