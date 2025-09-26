// ReSharper disable CppLocalVariableMayBeConst

#pragma once

#include <nanobind/nanobind.h>

namespace py = nanobind;
using namespace py::literals;

#include <steampp/steampp.h>

namespace steampp {

inline void register_python(py::module_& m) {
	auto steampp = m.def_submodule("steampp");
	using namespace steampp;

	py::class_<Steam>(steampp, "Steam")
		.def(py::init())
		.def_prop_ro("install_dir", &Steam::getInstallDir)
		.def_prop_ro("library_dirs", &Steam::getLibraryDirs)
		.def_prop_ro("sourcemod_dir", &Steam::getSourceModDir)
		.def_prop_ro("installed_apps", &Steam::getInstalledApps)
		.def("is_app_installed", &Steam::isAppInstalled, "appID"_a)
		.def("get_app_name", &Steam::getAppName, "appID"_a)
		.def("get_app_install_dir", &Steam::getAppInstallDir, "appID"_a)
		.def("get_app_icon_path", &Steam::getAppIconPath, "appID"_a)
		.def("get_app_logo_path", &Steam::getAppLogoPath, "appID"_a)
		.def("get_app_hero_path", &Steam::getAppHeroPath, "appID"_a)
		.def("get_app_box_art_path", &Steam::getAppBoxArtPath, "appID"_a)
		.def("get_app_store_art_path", &Steam::getAppStoreArtPath, "appID"_a)
		.def("is_app_using_goldsrc_engine", &Steam::isAppUsingGoldSrcEngine, "appID"_a)
		.def("is_app_using_source_engine", &Steam::isAppUsingSourceEngine, "appID"_a)
		.def("is_app_using_source_2_engine", &Steam::isAppUsingSource2Engine, "appID"_a)
		.def("__bool__", &Steam::operator bool, py::is_operator());
}

} // namespace steampp
