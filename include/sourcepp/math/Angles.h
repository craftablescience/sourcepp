#pragma once

#include <cmath>

#include "Vector.h"

namespace sourcepp {

using EulerAngles = Vec3f;

using Quat = Vec4f;

/// Lower precision Quat compressed to 6 bytes
struct QuatCompressed48 {
	uint16_t x : 16;
	uint16_t y : 16;
	uint16_t z : 15;
	uint16_t w : 1;

	[[nodiscard]] Quat decompress() const {
		// Convert from 16-bit (or 15-bit) integers to floating point values in the range [-1, 1]
		float fx = (static_cast<float>(this->x) / 32767.5f) - 1.f; // x / ((2^16 - 1) / 2) - 1
		float fy = (static_cast<float>(this->y) / 32767.5f) - 1.f; // y / ((2^16 - 1) / 2) - 1
		float fz = (static_cast<float>(this->z) / 16383.5f) - 1.f; // z / ((2^15 - 1) / 2) - 1

		// Recalculate w from the constraint that x^2 + y^2 + z^2 + w^2 = 1
		float fw = std::sqrt(1.f - fx * fx - fy * fy - fz * fz);

		// Adjust w based on the stored sign bit
		if (this->w == 0) {
			fw = -fw;
		}

		return {fx, fy, fz, fw};
	}
};

/// Lower precision Quat compressed to 12 bytes
struct QuatCompressed64 {
	uint32_t x : 21;
	uint32_t y : 21;
	uint32_t z : 21;
	uint32_t w : 1;

	[[nodiscard]] Quat decompress() const {
		// Convert from 21-bit integers to floating point values in the range [-1, 1]
		double fx = (static_cast<double>(this->x) / 1048575.5) - 1.0f; // x / ((2^21 - 1) / 2) - 1
		double fy = (static_cast<double>(this->y) / 1048575.5) - 1.0f; // y / ((2^21 - 1) / 2) - 1
		double fz = (static_cast<double>(this->z) / 1048575.5) - 1.0f; // z / ((2^21 - 1) / 2) - 1

		// Recalculate w from the constraint that x^2 + y^2 + z^2 + w^2 = 1
		double fw = std::sqrt(1.0 - fx * fx - fy * fy - fz * fz);

		// Adjust w based on the stored sign bit
		if (this->w == 0) {
			fw = -fw;
		}

		return {static_cast<float>(fx), static_cast<float>(fy), static_cast<float>(fz), static_cast<float>(fw)};
	}
};

} // namespace sourcepp
