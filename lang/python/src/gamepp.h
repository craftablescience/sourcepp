#pragma once

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <gamepp/gamepp.h>

namespace gamepp {

void register_python(py::module_& m) {
	using namespace gamepp;
	auto gamepp = m.def_submodule("gamepp");

	py::class_<GameInstance>(gamepp, "GameInstance")
			.def_static("find", &GameInstance::find, py::arg("window_name_override") = "")
			.def_property_readonly("window_title", &GameInstance::getWindowTitle)
			.def_property_readonly("window_pos", &GameInstance::getWindowPos)
			.def_property_readonly("window_size", &GameInstance::getWindowSize)
			.def("command", &GameInstance::command)
			.def("input_begin", &GameInstance::inputBegin)
			.def("input_end", &GameInstance::inputEnd)
			.def("input_once", &GameInstance::inputOnce)
			.def("input_hold", &GameInstance::inputHold)
			.def("wait", &GameInstance::wait);
}

} // namespace gamepp
