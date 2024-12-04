#pragma once

#include <string_view>

#include <nanobind/nanobind.h>
#include <nanobind/operators.h>

namespace py = nanobind;

#include <sourcepp/Math.h>

namespace sourcepp {

inline void register_python(py::module_& m) {
	auto sourcepp = m.def_submodule("sourcepp");
	using namespace sourcepp;

	{
		auto math = sourcepp.def_submodule("math");
		using namespace math;

		const auto registerVecType = [&math]<typename V>(std::string_view name) {
			py::class_<V>(math, name.data())
					.def("__len__", &V::size)
					.def("__setitem__", [](V& self, uint8_t index, typename V::value_type val) { self[index] = val; })
					.def("__getitem__", [](V& self, uint8_t index) { return self[index]; })
					.def_static("zero", &V::zero)
					.def("is_zero", &V::isZero);
		};

		registerVecType.operator()<Vec2i8>("Vec2i8");
		registerVecType.operator()<Vec2i16>("Vec2i16");
		registerVecType.operator()<Vec2i32>("Vec2i32");
		registerVecType.operator()<Vec2i64>("Vec2i64");
		//registerVecType.operator()<Vec2i>("Vec2i");

		registerVecType.operator()<Vec2ui8>("Vec2ui8");
		registerVecType.operator()<Vec2ui16>("Vec2ui16");
		registerVecType.operator()<Vec2ui32>("Vec2ui32");
		registerVecType.operator()<Vec2ui64>("Vec2ui64");
		//registerVecType.operator()<Vec2ui>("Vec2ui");

		//registerVecType.operator()<Vec2f16>("Vec2f16");
		registerVecType.operator()<Vec2f32>("Vec2f32");
		registerVecType.operator()<Vec2f64>("Vec2f64");
		//registerVecType.operator()<Vec2f>("Vec2f");

		registerVecType.operator()<Vec3i8>("Vec3i8");
		registerVecType.operator()<Vec3i16>("Vec3i16");
		registerVecType.operator()<Vec3i32>("Vec3i32");
		registerVecType.operator()<Vec3i64>("Vec3i64");
		//registerVecType.operator()<Vec3i>("Vec3i");

		registerVecType.operator()<Vec3ui8>("Vec3ui8");
		registerVecType.operator()<Vec3ui16>("Vec3ui16");
		registerVecType.operator()<Vec3ui32>("Vec3ui32");
		registerVecType.operator()<Vec3ui64>("Vec3ui64");
		//registerVecType.operator()<Vec3ui>("Vec3ui");

		//registerVecType.operator()<Vec3f16>("Vec3f16");
		registerVecType.operator()<Vec3f32>("Vec3f32");
		registerVecType.operator()<Vec3f64>("Vec3f64");
		//registerVecType.operator()<Vec3f>("Vec3f");

		registerVecType.operator()<Vec4i8>("Vec4i8");
		registerVecType.operator()<Vec4i16>("Vec4i16");
		registerVecType.operator()<Vec4i32>("Vec4i32");
		registerVecType.operator()<Vec4i64>("Vec4i64");
		//registerVecType.operator()<Vec4i>("Vec4i");

		registerVecType.operator()<Vec4ui8>("Vec4ui8");
		registerVecType.operator()<Vec4ui16>("Vec4ui16");
		registerVecType.operator()<Vec4ui32>("Vec4ui32");
		registerVecType.operator()<Vec4ui64>("Vec4ui64");
		//registerVecType.operator()<Vec4ui>("Vec4ui");

		//registerVecType.operator()<Vec4f16>("Vec4f16");
		registerVecType.operator()<Vec4f32>("Vec4f32");
		registerVecType.operator()<Vec4f64>("Vec4f64");
		//registerVecType.operator()<Vec4f>("Vec4f");
	}
}

} // namespace sourcepp
