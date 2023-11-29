#pragma once

namespace studiomodelpp {

template<unsigned char M, unsigned char N>
class Matrix {
public:
	[[nodiscard]] float* operator[](int i) { return this->data[i]; }

	[[nodiscard]] const float* operator[](int i) const { return this->data[i]; }

private:
	float data[M][N];
};

} // namespace studiomodelpp
