#pragma once

#include <string>

#include "../math/Vector.h"

namespace studiomodelpp {

struct BBox {
	int bone;
	int group;
	Vector3 bboxMin;
	Vector3 bboxMax;

	// int hitboxNameOffset;
	std::string name;

	// int _unused0[8];
};

struct Movement {
	enum Flags : int {
		FLAG_NONE = 0,
		// todo(flags): Movement
	};

	int endFrame;
	Flags flags;
	float velocityStart;
	float velocityEnd;
	float yawEnd;
	Vector3 movement;
	Vector3 relativePosition;
};

} // namespace studiomodelpp
