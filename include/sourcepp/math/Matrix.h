#pragma once

#include "Integer.h"

namespace sourcepp::math {

template<uint8_t M, uint8_t N, Arithmetic P = float>
class Matrix {
public:
	[[nodiscard]] P* operator[](uint8_t i) { return this->data[i]; }

	[[nodiscard]] const P* operator[](uint8_t i) const { return this->data[i]; }

	// todo(c++23): convert to operator[]
	[[nodiscard]] P operator()(uint8_t i, uint8_t j) const { return this->data[i][j]; }

private:
	P data[M][N];
};

} // namespace sourcepp::math
