#pragma once

#include <sourceppc/Buffer.h>

#include "API.h"
#include "ImageConversion.h"

VTFPP_EXTERN typedef enum {
	VTFPP_DISTANCE_MAPPING_DITHER_NONE = 0,
	VTFPP_DISTANCE_MAPPING_DITHER_GRADIENT_TANGENT,
} vtfpp_distance_mapping_dither_e;

VTFPP_EXTERN typedef enum {
	VTFPP_DISTANCE_MAPPING_FLAG_NONE           = 0,
	VTFPP_DISTANCE_MAPPING_FLAG_DISTANCEAA     = 1 << 0,
	VTFPP_DISTANCE_MAPPING_FLAG_EUCLIDEAN      = 1 << 1,
	VTFPP_DISTANCE_MAPPING_FLAG_SAMPLECENTERED = 1 << 2,
} vtfpp_distance_mapping_flags_e;

VTFPP_API sourcepp_buffer_t vtfpp_distance_mapping_alpha_to_distance(const unsigned char* buffer, size_t bufferLen, vtfpp_image_format_e inFormat, vtfpp_image_format_e outFormat, uint16_t width, uint16_t height, uint16_t reduceX, uint16_t reduceY, int srgb); // REQUIRES MANUAL FREE: sourcepp_buffer_free
VTFPP_API sourcepp_buffer_t vtfpp_distance_mapping_alpha_to_distance_ex(const unsigned char* buffer, size_t bufferLen, vtfpp_image_format_e inFormat, vtfpp_image_format_e outFormat, uint16_t width, uint16_t height, uint16_t reduceX, uint16_t reduceY, int srgb, float distanceSpread, float alphaThreshold, vtfpp_distance_mapping_flags_e flags, vtfpp_distance_mapping_dither_e dither, vtfpp_image_conversion_resize_filter_e filter, vtfpp_image_conversion_resize_edge_e edge, int* valveQuirks); // REQUIRES MANUAL FREE: sourcepp_buffer_free

// C++ conversion routines
#ifdef __cplusplus

#include <vtfpp/DistanceMapping.h>

namespace sourceppc::convert {

inline vtfpp::DistanceMapping::Dither cast(vtfpp_distance_mapping_dither_e value) {
	switch (value) {
		case VTFPP_DISTANCE_MAPPING_DITHER_NONE:             return vtfpp::DistanceMapping::Dither::NONE;
		case VTFPP_DISTANCE_MAPPING_DITHER_GRADIENT_TANGENT: return vtfpp::DistanceMapping::Dither::GRADIENT_TANGENT;
	}
	return vtfpp::DistanceMapping::Dither::NONE;
}

inline vtfpp_distance_mapping_dither_e cast(vtfpp::DistanceMapping::Dither value) {
	switch (value) {
		case vtfpp::DistanceMapping::Dither::NONE:             return VTFPP_DISTANCE_MAPPING_DITHER_NONE;
		case vtfpp::DistanceMapping::Dither::GRADIENT_TANGENT: return VTFPP_DISTANCE_MAPPING_DITHER_GRADIENT_TANGENT;
	}
	return VTFPP_DISTANCE_MAPPING_DITHER_NONE;
}

inline vtfpp::DistanceMapping::Flags cast(vtfpp_distance_mapping_flags_e flags) {
	return static_cast<vtfpp::DistanceMapping::Flags>(flags);
}

inline vtfpp_distance_mapping_flags_e cast(vtfpp::DistanceMapping::Flags flags) {
	return static_cast<vtfpp_distance_mapping_flags_e>(flags);
}

} // namespace sourceppc::convert

#endif
