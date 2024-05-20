#pragma once

#include <concepts>

namespace sourcepp {

#pragma pack(push, 1)

template<unsigned char M, unsigned char N, std::floating_point P = float>
class Matrix {
public:
	[[nodiscard]] P* operator[](int i) { return this->data[i]; }

	[[nodiscard]] const P* operator[](int i) const { return this->data[i]; }

	// todo(c++23): convert to operator[]
	[[nodiscard]] P operator()(int i, int j) const { return this->data[i][j]; }

private:
	P data[M][N];
};

#pragma pack(pop)

} // namespace sourcepp
