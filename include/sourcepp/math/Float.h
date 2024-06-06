#pragma once

#include "Integer.h"

namespace sourcepp::math {

// https://stackoverflow.com/a/60047308
class FloatCompressed16 {
public:
	explicit FloatCompressed16(uint16_t in) {
		this->data = in;
	}

	explicit FloatCompressed16(float in) { // IEEE-754 16-bit floating-point format (without infinity): 1-5-10, exp-15, +-131008.0, +-6.1035156E-5, +-5.9604645E-8, 3.311 digits
		const auto b = *reinterpret_cast<uint32_t*>(&in) + 0x00001000; // round-to-nearest-even: add last bit after truncated mantissa
		const auto e = (b & 0x7F800000) >> 23; // exponent
		const auto m = b & 0x007FFFFF; // mantissa; in line below: 0x007FF000 = 0x00800000-0x00001000 = decimal indicator flag - initial rounding
		this->data = (b & 0x80000000) >> 16 | (e > 112) * ((((e - 112) << 10) & 0x7C00) | m >> 13) | ((e < 113) & (e > 101)) * ((((0x007FF000 + m) >> (125 - e)) + 1) >> 1) | (e > 143) * 0x7FFF; // sign : normalized : denormalized : saturate
	}

	[[nodiscard]] uint16_t toFloat16() const {
		return this->data;
	}

	[[nodiscard]] float toFloat32() const { // IEEE-754 16-bit floating-point format (without infinity): 1-5-10, exp-15, +-131008.0, +-6.1035156E-5, +-5.9604645E-8, 3.311 digits
		const uint32_t e = (this->data & 0x7C00) >> 10; // exponent
		const uint32_t m = (this->data & 0x03FF) << 13; // mantissa
		const auto mf = static_cast<float>(m);
		const uint32_t v = *reinterpret_cast<const uint32_t*>(&mf) >> 23; // evil log2 bit hack to count leading zeros in denormalized format
		const uint32_t vu = (this->data & 0x8000) << 16 | (e != 0) * ((e + 112) << 23 | m) | ((e == 0) & (m != 0)) * ((v - 37) << 23 | ((m << (150 - v)) & 0x007FE000)); // sign : normalized : denormalized
		return *reinterpret_cast<const float*>(&vu);
	}

	[[nodiscard]] float operator*() const {
		return this->toFloat32();
	}

private:
	uint16_t data;
};

} // namespace sourcepp::math
