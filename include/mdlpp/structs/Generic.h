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
		FLAG_X      = 0x00000001,
		FLAG_Y      = 0x00000002,
		FLAG_Z      = 0x00000004,
		FLAG_XR     = 0x00000008,
		FLAG_YR     = 0x00000010,
		FLAG_ZR     = 0x00000020,
		FLAG_LX     = 0x00000040,
		FLAG_LY     = 0x00000080,
		FLAG_LZ     = 0x00000100,
		FLAG_LXR    = 0x00000200,
		FLAG_LYR    = 0x00000400,
		FLAG_LZR    = 0x00000800,
		FLAG_LINEAR = 0x00001000,
		FLAG_TYPES  = 0x0003FFFF,
		FLAG_RLOOP  = 0x00040000,
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
using AnimValuePtr = std::array<int16_t, 3>;

} // namespace mdlpp
