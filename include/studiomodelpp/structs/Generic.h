#pragma once

#include <string>

#include <sourcepp/math/Vector.h>

namespace studiomodelpp {

constexpr int ROOT_LOD = 0;
constexpr int MAX_LOD_COUNT = 8;
constexpr int MAX_BONES_PER_VERTEX = 3;

struct BBox {
	int32_t bone;
	int32_t group;
	sourcepp::Vec3f bboxMin;
	sourcepp::Vec3f bboxMax;

	//int32_t hitboxNameOffset;
	std::string name;

	//int32_t _unused0[8];
};

struct Movement {
	enum Flags : int32_t {
		FLAG_NONE = 0,
		// todo(flags): Movement
	};

	int32_t endFrame;
	Flags flags;
	float velocityStart;
	float velocityEnd;
	float yawEnd;
	sourcepp::Vec3f movement;
	sourcepp::Vec3f relativePosition;
};

} // namespace studiomodelpp
