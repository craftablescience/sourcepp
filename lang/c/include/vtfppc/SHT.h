#pragma once

#include <sourceppc/API.h>
#include <sourceppc/Buffer.h>

SOURCEPP_EXTERN typedef struct {
	float x1;
	float y1;
	float x2;
	float y2;
} vtfpp_sht_sequence_frame_bounds_t;

SOURCEPP_EXTERN typedef struct {
	float duration;
	vtfpp_sht_sequence_frame_bounds_t bounds[4];
} vtfpp_sht_sequence_frame_t;

#define VTFPP_SHT_SEQUENCE_FRAME_INVALID (SOURCEPP_CAST_CTOR(vtfpp_sht_sequence_frame_t) {.duration = 0.f})

SOURCEPP_EXTERN typedef struct {
	uint32_t id;
	int32_t loop;
	uint32_t frameCount;
	float durationTotal;
} vtfpp_sht_sequence_t;

#define VTFPP_SHT_SEQUENCE_INVALID (SOURCEPP_CAST_CTOR(vtfpp_sht_sequence_t) {.id = 0, .loop = 0, .frameCount = 0, .durationTotal = 0.f})

typedef void* vtfpp_sht_handle_t;

SOURCEPP_API vtfpp_sht_handle_t vtfpp_sht_create(); // REQUIRES MANUAL FREE: vtfpp_sht_free
SOURCEPP_API vtfpp_sht_handle_t vtfpp_sht_open_from_mem(const unsigned char* shtData, size_t shtLen); // REQUIRES MANUAL FREE: vtfpp_sht_free
SOURCEPP_API vtfpp_sht_handle_t vtfpp_sht_open_from_file(const char* shtPath); // REQUIRES MANUAL FREE: vtfpp_sht_free
SOURCEPP_API void vtfpp_sht_free(vtfpp_sht_handle_t* handle);
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
SOURCEPP_API vtfpp_sht_sequence_frame_t vtfpp_sht_get_frame(vtfpp_sht_handle_t handle, uint32_t sequenceID, uint32_t index);
SOURCEPP_API void vtfpp_sht_set_frame(vtfpp_sht_handle_t handle, uint32_t sequenceID, uint32_t index, vtfpp_sht_sequence_frame_t frame);
SOURCEPP_API void vtfpp_sht_add_frame(vtfpp_sht_handle_t handle, uint32_t sequenceID, vtfpp_sht_sequence_frame_t frame);
SOURCEPP_API void vtfpp_sht_remove_frame(vtfpp_sht_handle_t handle, uint32_t sequenceID, uint32_t index);
SOURCEPP_API uint8_t vtfpp_sht_get_frame_bounds_count(vtfpp_sht_handle_t handle);
SOURCEPP_API sourcepp_buffer_t vtfpp_sht_bake(vtfpp_sht_handle_t handle); // REQUIRES MANUAL FREE: sourcepp_buffer_free
SOURCEPP_API int vtfpp_sht_bake_to_file(vtfpp_sht_handle_t handle, const char* shtPath);

// C++ conversion routines
#ifdef __cplusplus

#include <vtfpp/SHT.h>

namespace sourceppc::convert {

inline vtfpp::SHT::Sequence::Frame::Bounds cast(const vtfpp_sht_sequence_frame_bounds_t& value) {
	return {
		.x1 = value.x1,
		.y1 = value.y1,
		.x2 = value.x2,
		.y2 = value.y2,
	};
}

inline vtfpp_sht_sequence_frame_bounds_t cast(const vtfpp::SHT::Sequence::Frame::Bounds& value) {
	return {
		.x1 = value.x1,
		.y1 = value.y1,
		.x2 = value.x2,
		.y2 = value.y2,
	};
}

inline vtfpp::SHT::Sequence::Frame cast(const vtfpp_sht_sequence_frame_t& value) {
	return {
		.duration = value.duration,
		.bounds = {
			cast(value.bounds[0]),
			cast(value.bounds[1]),
			cast(value.bounds[2]),
			cast(value.bounds[3]),
		},
	};
}

inline vtfpp_sht_sequence_frame_t cast(const vtfpp::SHT::Sequence::Frame& value) {
	return {
		.duration = value.duration,
		.bounds = {
			cast(value.bounds[0]),
			cast(value.bounds[1]),
			cast(value.bounds[2]),
			cast(value.bounds[3]),
		},
	};
}

inline void set(vtfpp::SHT::Sequence& lhs, vtfpp_sht_sequence_t rhs) {
	lhs.id = rhs.id;
	lhs.loop = rhs.loop;
	if (lhs.frames.size() != rhs.frameCount) {
		lhs.frames.resize(rhs.frameCount);
	}
	lhs.durationTotal = rhs.durationTotal;
}

inline vtfpp_sht_sequence_t cast(const vtfpp::SHT::Sequence& value) {
	return {
		.id = value.id,
		.loop = value.loop,
		.frameCount = static_cast<uint32_t>(value.frames.size()),
		.durationTotal = value.durationTotal,
	};
}

} // namespace sourceppc::convert

#endif
