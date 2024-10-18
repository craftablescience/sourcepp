#pragma once

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <steampp/steampp.h>

namespace steampp {

void register_python(py::module_& m) {
	using namespace steampp;
	auto steampp = m.def_submodule("steampp");

	py::class_<Steam>(steampp, "Steam")
	        .def(py::init<>())
			.def_property_readonly("install_dir", &Steam::getInstallDir)
			.def_property_readonly("library_dirs", &Steam::getLibraryDirs)
			.def_property_readonly("sourcemod_dir", &Steam::getSourceModDir)
			.def_property_readonly("installed_apps", &Steam::getInstalledApps)
			.def("is_app_installed", &Steam::isAppInstalled)
			.def("get_app_name", &Steam::getAppName)
			.def("get_app_install_dir", &Steam::getAppInstallDir)
			.def("get_app_icon_path", &Steam::getAppIconPath)
			.def("get_app_logo_path", &Steam::getAppLogoPath)
			.def("get_app_box_art_path", &Steam::getAppBoxArtPath)
			.def("get_app_store_art_path", &Steam::getAppStoreArtPath)
			.def("is_app_using_source_engine", &Steam::isAppUsingSourceEngine)
			.def("is_app_using_source_2_engine", &Steam::isAppUsingSource2Engine)
			.def_property_readonly("valid", &Steam::operator bool);
}

} // namespace steampp
