#pragma once

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <gamepp/gamepp.h>

namespace gamepp {

inline void register_python(py::module_& m) {
	auto gamepp = m.def_submodule("gamepp");
	using namespace gamepp;

	py::class_<GameInstance>(gamepp, "GameInstance")
			.def_static("find",                    &GameInstance::find,       py::arg("window_name_override") = "")
			.def_property_readonly("window_title", &GameInstance::getWindowTitle)
			.def_property_readonly("window_pos",   &GameInstance::getWindowPos)
			.def_property_readonly("window_size",  &GameInstance::getWindowSize)
			.def("command",                        &GameInstance::command,    py::arg("command"))
			.def("input_begin",                    &GameInstance::inputBegin, py::arg("input"))
			.def("input_end",                      &GameInstance::inputEnd,   py::arg("input"))
			.def("input_once",                     &GameInstance::inputOnce,  py::arg("input"))
			.def("input_hold",                     &GameInstance::inputHold,  py::arg("input"), py::arg("sec"))
			.def("wait",                           &GameInstance::wait,       py::arg("sec"));
}

} // namespace gamepp
