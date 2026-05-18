#include <vtfppc/DistanceMapping.h>

#include <sourceppc/Helpers.h>

using namespace sourceppc;
using namespace vtfpp;

VTFPP_API sourcepp_buffer_t vtfpp_distance_mapping_alpha_to_distance(const unsigned char* buffer, size_t bufferLen, vtfpp_image_format_e inFormat, vtfpp_image_format_e outFormat, uint16_t width, uint16_t height, uint16_t reduceX, uint16_t reduceY, int srgb) {
	SOURCEPP_EARLY_RETURN_VAL(buffer, SOURCEPP_BUFFER_INVALID);
	SOURCEPP_EARLY_RETURN_VAL(bufferLen, SOURCEPP_BUFFER_INVALID);

	return convert::toBuffer(DistanceMapping::alphaToDistance({reinterpret_cast<const std::byte*>(buffer), bufferLen}, convert::cast(inFormat), convert::cast(outFormat), width, height, reduceX, reduceY, srgb));
}

VTFPP_API sourcepp_buffer_t vtfpp_distance_mapping_alpha_to_distance_ex(const unsigned char* buffer, size_t bufferLen, vtfpp_image_format_e inFormat, vtfpp_image_format_e outFormat, uint16_t width, uint16_t height, uint16_t reduceX, uint16_t reduceY, int srgb, float distanceSpread, float alphaThreshold, vtfpp_distance_mapping_flags_e flags, vtfpp_distance_mapping_dither_e dither, vtfpp_image_conversion_resize_filter_e filter, vtfpp_image_conversion_resize_edge_e edge, int* valveQuirks) {
	SOURCEPP_EARLY_RETURN_VAL(buffer, SOURCEPP_BUFFER_INVALID);
	SOURCEPP_EARLY_RETURN_VAL(bufferLen, SOURCEPP_BUFFER_INVALID);

	if (!valveQuirks) {
		return convert::toBuffer(DistanceMapping::alphaToDistance({reinterpret_cast<const std::byte*>(buffer), bufferLen}, convert::cast(inFormat), convert::cast(outFormat), width, height, reduceX, reduceY, srgb, distanceSpread, alphaThreshold, convert::cast(flags), convert::cast(dither), convert::cast(filter), convert::cast(edge), nullptr));
	}
	bool valveQuirksBool = *valveQuirks;
	const auto out = convert::toBuffer(DistanceMapping::alphaToDistance({reinterpret_cast<const std::byte*>(buffer), bufferLen}, convert::cast(inFormat), convert::cast(outFormat), width, height, reduceX, reduceY, srgb, distanceSpread, alphaThreshold, convert::cast(flags), convert::cast(dither), convert::cast(filter), convert::cast(edge), &valveQuirksBool));
	*valveQuirks = valveQuirksBool;
	return out;
}
