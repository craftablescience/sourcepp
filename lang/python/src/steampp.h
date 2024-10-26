#pragma once

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <steampp/steampp.h>

namespace steampp {

inline void register_python(py::module_& m) {
	auto steampp = m.def_submodule("steampp");
	using namespace steampp;

	py::class_<Steam>(steampp, "Steam")
			.def(py::init<>())
			.def_property_readonly("install_dir",    &Steam::getInstallDir)
			.def_property_readonly("library_dirs",   &Steam::getLibraryDirs)
			.def_property_readonly("sourcemod_dir",  &Steam::getSourceModDir)
			.def_property_readonly("installed_apps", &Steam::getInstalledApps)
			.def("is_app_installed",                 &Steam::isAppInstalled,          py::arg("appID"))
			.def("get_app_name",                     &Steam::getAppName,              py::arg("appID"))
			.def("get_app_install_dir",              &Steam::getAppInstallDir,        py::arg("appID"))
			.def("get_app_icon_path",                &Steam::getAppIconPath,          py::arg("appID"))
			.def("get_app_logo_path",                &Steam::getAppLogoPath,          py::arg("appID"))
			.def("get_app_box_art_path",             &Steam::getAppBoxArtPath,        py::arg("appID"))
			.def("get_app_store_art_path",           &Steam::getAppStoreArtPath,      py::arg("appID"))
			.def("is_app_using_source_engine",       &Steam::isAppUsingSourceEngine,  py::arg("appID"))
			.def("is_app_using_source_2_engine",     &Steam::isAppUsingSource2Engine, py::arg("appID"))
			.def_property_readonly("__bool__",       &Steam::operator bool,           py::is_operator());
}

} // namespace steampp
