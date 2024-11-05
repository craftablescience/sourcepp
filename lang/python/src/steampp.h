#pragma once

#include <nanobind/nanobind.h>
#include <nanobind/stl/string_view.h>
#include <nanobind/stl/string.h>
#include <nanobind/stl/vector.h>

namespace py = nanobind;

#include <steampp/steampp.h>

namespace steampp {

inline void register_python(py::module_& m) {
	auto steampp = m.def_submodule("steampp");
	using namespace steampp;

	py::class_<Steam>(steampp, "Steam")
			.def(py::init<>())
			.def_prop_ro("install_dir",          &Steam::getInstallDir)
			.def_prop_ro("library_dirs",         &Steam::getLibraryDirs)
			.def_prop_ro("sourcemod_dir",        &Steam::getSourceModDir)
			.def_prop_ro("installed_apps",       &Steam::getInstalledApps)
			.def("is_app_installed",             &Steam::isAppInstalled,          py::arg("appID"))
			.def("get_app_name",                 &Steam::getAppName,              py::arg("appID"))
			.def("get_app_install_dir",          &Steam::getAppInstallDir,        py::arg("appID"))
			.def("get_app_icon_path",            &Steam::getAppIconPath,          py::arg("appID"))
			.def("get_app_logo_path",            &Steam::getAppLogoPath,          py::arg("appID"))
			.def("get_app_box_art_path",         &Steam::getAppBoxArtPath,        py::arg("appID"))
			.def("get_app_store_art_path",       &Steam::getAppStoreArtPath,      py::arg("appID"))
			.def("is_app_using_source_engine",   &Steam::isAppUsingSourceEngine,  py::arg("appID"))
			.def("is_app_using_source_2_engine", &Steam::isAppUsingSource2Engine, py::arg("appID"))
			.def_prop_ro("__bool__",             &Steam::operator bool,           py::is_operator());
}

} // namespace steampp
