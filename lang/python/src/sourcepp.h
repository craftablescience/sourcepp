// ReSharper disable CppLocalVariableMayBeConst
// ReSharper disable CppUnusedIncludeDirective

#pragma once

#include <string_view>

#include <nanobind/nanobind.h>
#include <nanobind/make_iterator.h>
#include <nanobind/operators.h>
#include <nanobind/trampoline.h>
#include <nanobind/typing.h>
#include <nanobind/stl/array.h>
#include <nanobind/stl/bind_map.h>
#include <nanobind/stl/bind_vector.h>
#include <nanobind/stl/chrono.h>
#include <nanobind/stl/complex.h>
#include <nanobind/stl/filesystem.h>
#include <nanobind/stl/function.h>
#include <nanobind/stl/list.h>
#include <nanobind/stl/map.h>
#include <nanobind/stl/optional.h>
#include <nanobind/stl/pair.h>
#include <nanobind/stl/set.h>
#include <nanobind/stl/shared_ptr.h>
#include <nanobind/stl/string.h>
#include <nanobind/stl/string_view.h>
#include <nanobind/stl/tuple.h>
#include <nanobind/stl/unique_ptr.h>
#include <nanobind/stl/unordered_map.h>
#include <nanobind/stl/unordered_set.h>
#include <nanobind/stl/variant.h>
#include <nanobind/stl/vector.h>
#include <nanobind/stl/wstring.h>

namespace py = nanobind;
using namespace py::literals;

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
				.def("mag", &V::mag)
				.def("sum", &V::sum)
				.def_static("zero", &V::zero)
				.def("is_zero", &V::isZero);
		};

		#define SOURCEPP_VEC_DEFINE(S) \
			registerVecType.operator()<Vec##S##i8>(  "Vec" #S "i8"  ); \
			registerVecType.operator()<Vec##S##i16>( "Vec" #S "i16" ); \
			registerVecType.operator()<Vec##S##i32>( "Vec" #S "i32" ); \
			registerVecType.operator()<Vec##S##i64>( "Vec" #S "i64" ); \
			registerVecType.operator()<Vec##S##ui8>( "Vec" #S "ui8" ); \
			registerVecType.operator()<Vec##S##ui16>("Vec" #S "ui16"); \
			registerVecType.operator()<Vec##S##ui32>("Vec" #S "ui32"); \
			registerVecType.operator()<Vec##S##ui64>("Vec" #S "ui64"); \
			registerVecType.operator()<Vec##S##f32>( "Vec" #S "f32" ); \
			registerVecType.operator()<Vec##S##f64>( "Vec" #S "f64" )

		SOURCEPP_VEC_DEFINE(2);
		SOURCEPP_VEC_DEFINE(3);
		SOURCEPP_VEC_DEFINE(4);

		#undef SOURCEPP_VEC_DEFINE

		const auto registerMatType = [&math]<typename M>(std::string_view name) {
			py::class_<M>(math, name.data())
				.def("rows", &M::rows)
				.def("cols", &M::cols)
				.def("__setitem__", [](M& self, uint8_t index, typename M::value_type val) { self[index / self.rows()][index % self.rows()] = val; })
				.def("__getitem__", [](M& self, uint8_t index) { return self[index / self.rows()][index % self.rows()]; })
				;
		};

		#define SOURCEPP_MAT_DEFINE(M, N) \
			registerMatType.operator()<Mat##M##x##N##i8>(  "Mat" #M "x" #N "i8"  ); \
			registerMatType.operator()<Mat##M##x##N##i16>( "Mat" #M "x" #N "i16" ); \
			registerMatType.operator()<Mat##M##x##N##i32>( "Mat" #M "x" #N "i32" ); \
			registerMatType.operator()<Mat##M##x##N##i64>( "Mat" #M "x" #N "i64" ); \
			registerMatType.operator()<Mat##M##x##N##ui8>( "Mat" #M "x" #N "ui8" ); \
			registerMatType.operator()<Mat##M##x##N##ui16>("Mat" #M "x" #N "ui16"); \
			registerMatType.operator()<Mat##M##x##N##ui32>("Mat" #M "x" #N "ui32"); \
			registerMatType.operator()<Mat##M##x##N##ui64>("Mat" #M "x" #N "ui64"); \
			registerMatType.operator()<Mat##M##x##N##f32>( "Mat" #M "x" #N "f32" ); \
			registerMatType.operator()<Mat##M##x##N##f64>( "Mat" #M "x" #N "f64" )

		SOURCEPP_MAT_DEFINE(2, 2);
		SOURCEPP_MAT_DEFINE(3, 3);
		SOURCEPP_MAT_DEFINE(4, 4);
		SOURCEPP_MAT_DEFINE(2, 3);
		SOURCEPP_MAT_DEFINE(3, 2);
		SOURCEPP_MAT_DEFINE(2, 4);
		SOURCEPP_MAT_DEFINE(4, 2);
		SOURCEPP_MAT_DEFINE(3, 4);
		SOURCEPP_MAT_DEFINE(4, 3);

		#undef SOURCEPP_MAT_DEFINE
	}
}

} // namespace sourcepp
