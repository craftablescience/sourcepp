#pragma once

#include <array>
#include <string>

#include <sourcepp/Macros.h>
#include <sourcepp/Math.h>

namespace mdlpp {

constexpr int ROOT_LOD = 0;
constexpr int MAX_LOD_COUNT = 8;
constexpr int MAX_BONES_PER_VERTEX = 3;

struct BBox {
	int32_t bone;
	int32_t group;
	sourcepp::math::Vec3f bboxMin;
	sourcepp::math::Vec3f bboxMax;

	//int32_t hitboxNameOffset;
	std::string name;

	//int32_t _unused0[8];
};

struct Movement {
	enum Flags : int32_t {
		FLAG_NONE   = 0,
		FLAG_X      = 1 << 0,
		FLAG_Y      = 1 << 1,
		FLAG_Z      = 1 << 2,
		FLAG_XR     = 1 << 3,
		FLAG_YR     = 1 << 4,
		FLAG_ZR     = 1 << 5,
		FLAG_LX     = 1 << 6,
		FLAG_LY     = 1 << 7,
		FLAG_LZ     = 1 << 8,
		FLAG_LXR    = 1 << 9,
		FLAG_LYR    = 1 << 10,
		FLAG_LZR    = 1 << 11,
		FLAG_LINEAR = 1 << 12,
		FLAG_TYPES  = FLAG_X | FLAG_Y | FLAG_Z | FLAG_XR | FLAG_YR | FLAG_ZR | FLAG_LX | FLAG_LY | FLAG_LZ | FLAG_LX | FLAG_LYR | FLAG_LZR | FLAG_LINEAR,
		FLAG_RLOOP  = 1 << 18,
	};

	int32_t endFrame;
	Flags flags;
	float velocityStart;
	float velocityEnd;
	float yawEnd;
	sourcepp::math::Vec3f movement;
	sourcepp::math::Vec3f relativePosition;
};
SOURCEPP_BITFLAGS_ENUM(Movement::Flags)

struct IKLock {
	int32_t chain;
	float posWeight;
	float localQWeight;
	int32_t flags;

	//int32_t unused[4];
};

union AnimValue {
	struct {
		uint8_t valid;
		uint8_t total;
	} num;
	int16_t value;
};
static_assert(sizeof(AnimValue) == 2);

// x/y/z or pitch/yaw/roll
using AnimValuePtr = sourcepp::math::Vec3i16;

} // namespace mdlpp
