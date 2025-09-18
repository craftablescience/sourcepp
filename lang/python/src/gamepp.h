#pragma once

#include <nanobind/nanobind.h>
#include <nanobind/stl/optional.h>
#include <nanobind/stl/string.h>
#include <nanobind/stl/string_view.h>

namespace py = nanobind;
using namespace py::literals;

#include <gamepp/gamepp.h>

namespace gamepp {

inline void register_python(py::module_& m) {
	auto gamepp = m.def_submodule("gamepp");
	using namespace gamepp;

	py::class_<GameInstance>(gamepp, "GameInstance")
		.def_static("find", &GameInstance::find, "window_name_override"_a = "")
		.def_prop_ro("window_title", &GameInstance::getWindowTitle)
		.def_prop_ro("window_pos", &GameInstance::getWindowPos)
		.def_prop_ro("window_size", &GameInstance::getWindowSize)
		.def("command", &GameInstance::command, "command"_a, py::rv_policy::reference)
		.def("input_begin", &GameInstance::inputBegin, "input"_a, py::rv_policy::reference)
		.def("input_end", &GameInstance::inputEnd, "input"_a, py::rv_policy::reference)
		.def("input_once", &GameInstance::inputOnce, "input"_a, py::rv_policy::reference)
		.def("input_hold", &GameInstance::inputHold, "input"_a, "sec"_a, py::rv_policy::reference)
		.def("wait", &GameInstance::wait, "sec"_a, py::rv_policy::reference);
}

} // namespace gamepp
