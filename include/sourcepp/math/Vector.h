#pragma once

namespace sourcepp {

#pragma pack(push, 1)

template<std::floating_point P>
struct Vec2 {
	P x;
	P y;
};
using Vec2f = Vec2<float>;
using Vec2d = Vec2<double>;

template<std::floating_point P>
struct Vec3 {
	P x;
	P y;
	P z;
};
using Vec3f = Vec3<float>;
using Vec3d = Vec3<double>;

template<std::floating_point P = float>
struct Vec4 {
	P x;
	P y;
	P z;
	P w;
};
using Vec4f = Vec4<float>;
using Vec4d = Vec4<double>;

#pragma pack(pop)

} // namespace sourcepp
