#pragma once

namespace studiomodelpp {

#pragma pack(push, 1)

template<unsigned char M, unsigned char N>
class Matrix {
public:
	[[nodiscard]] float* operator[](int i) { return this->data[i]; }

	[[nodiscard]] const float* operator[](int i) const { return this->data[i]; }

private:
	float data[M][N];
};

#pragma pack(pop)

} // namespace studiomodelpp
