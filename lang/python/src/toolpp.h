#pragma once

#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>
#include <nanobind/stl/string_view.h>
#include <nanobind/stl/unordered_map.h>
#include <nanobind/stl/vector.h>

namespace py = nanobind;

#include <toolpp/toolpp.h>

namespace toolpp {

inline void register_python(py::module_& m) {
	auto toolpp = m.def_submodule("toolpp");
	using namespace toolpp;

	py::enum_<CmdSeq::Command::Special>(toolpp, "CmdSeqCommandSpecial")
			.value("NONE",                      CmdSeq::Command::Special::NONE)
			.value("CHANGE_DIRECTORY",          CmdSeq::Command::Special::CHANGE_DIRECTORY)
			.value("COPY_FILE",                 CmdSeq::Command::Special::COPY_FILE)
			.value("DELETE_FILE",               CmdSeq::Command::Special::DELETE_FILE)
			.value("RENAME_FILE",               CmdSeq::Command::Special::RENAME_FILE)
			.value("COPY_FILE_IF_EXISTS_ALIAS", CmdSeq::Command::SPECIAL_COPY_FILE_IF_EXISTS_ALIAS)
			.value("COPY_FILE_IF_EXISTS",       CmdSeq::Command::Special::COPY_FILE_IF_EXISTS)
			.export_values();

	py::class_<CmdSeq::Command>(toolpp, "CmdSeqCommand")
			.def_rw("enabled",                             &CmdSeq::Command::enabled)
			.def_rw("executable",                          &CmdSeq::Command::executable)
			.def_rw("arguments",                           &CmdSeq::Command::arguments)
			.def_rw("ensure_file_exists",                  &CmdSeq::Command::ensureFileExists)
			.def_rw("path_to_theoretically_existing_file", &CmdSeq::Command::pathToTheoreticallyExistingFile)
			.def_rw("use_process_window",                  &CmdSeq::Command::useProcessWindow)
			.def_rw("wait_for_keypress",                   &CmdSeq::Command::waitForKeypress)
			.def_static("get_special_display_name_for", &CmdSeq::Command::getSpecialDisplayNameFor, py::arg("special"))
			.def("get_executable_display_name", &CmdSeq::Command::getExecutableDisplayName);

	py::class_<CmdSeq::Sequence>(toolpp, "CmdSeqSequence")
			.def_rw("name",     &CmdSeq::Sequence::name)
			.def_rw("commands", &CmdSeq::Sequence::commands);

	py::enum_<CmdSeq::Type>(toolpp, "CmdSeqType")
			.value("INVALID",          CmdSeq::Type::INVALID)
			.value("BINARY",           CmdSeq::Type::BINARY)
			.value("KEYVALUES_STRATA", CmdSeq::Type::KEYVALUES_STRATA)
			.export_values();

	py::class_<CmdSeq>(toolpp, "CmdSeq")
			.def(py::init<const std::string&>(), py::arg("path"))
			.def(py::init<CmdSeq::Type>(), py::arg("type"))
			.def("__bool__", &CmdSeq::operator bool, py::is_operator())
			.def_prop_rw("type", &CmdSeq::getType, &CmdSeq::setType)
			.def_prop_ro("version", &CmdSeq::getVersion)
			.def("set_version", &CmdSeq::setVersion, py::arg("is_v02"))
			.def("sequences", py::overload_cast<>(&CmdSeq::getSequences), py::rv_policy::reference_internal)
			.def("bake", [](const CmdSeq& self) {
				const auto d = self.bake();
				return py::bytes{d.data(), d.size()};
			})
			.def("bake_to_file", py::overload_cast<const std::string&>(&CmdSeq::bake, py::const_), py::arg("path"));

	py::class_<FGD::Entity::ClassProperty>(toolpp, "FGDEntityClassProperty")
			.def_ro("name",      &FGD::Entity::ClassProperty::name)
			.def_ro("arguments", &FGD::Entity::ClassProperty::arguments);

	py::class_<FGD::Entity::Field>(toolpp, "FGDEntityField")
			.def_ro("name",          &FGD::Entity::Field::name)
			.def_ro("value_type",    &FGD::Entity::Field::valueType)
			.def_ro("readonly",      &FGD::Entity::Field::readonly)
			.def_ro("reportable",    &FGD::Entity::Field::reportable)
			.def_ro("display_name",  &FGD::Entity::Field::displayName)
			.def_ro("value_default", &FGD::Entity::Field::valueDefault)
			.def_ro("description",   &FGD::Entity::Field::description);

	py::class_<FGD::Entity::FieldChoices::Choice>(toolpp, "FGDEntityFieldChoicesChoice")
			.def_ro("value",        &FGD::Entity::FieldChoices::Choice::value)
			.def_ro("display_name", &FGD::Entity::FieldChoices::Choice::displayName);

	py::class_<FGD::Entity::FieldChoices>(toolpp, "FGDEntityFieldChoices")
			.def_ro("name",          &FGD::Entity::FieldChoices::name)
			.def_ro("readonly",      &FGD::Entity::FieldChoices::readonly)
			.def_ro("reportable",    &FGD::Entity::FieldChoices::reportable)
			.def_ro("display_name",  &FGD::Entity::FieldChoices::displayName)
			.def_ro("value_default", &FGD::Entity::FieldChoices::valueDefault)
			.def_ro("description",   &FGD::Entity::FieldChoices::description)
			.def_ro("choices",       &FGD::Entity::FieldChoices::choices);

	py::class_<FGD::Entity::FieldFlags::Flag>(toolpp, "FGDEntityFieldFlagsFlag")
			.def_ro("value",              &FGD::Entity::FieldFlags::Flag::value)
			.def_ro("display_name",       &FGD::Entity::FieldFlags::Flag::displayName)
			.def_ro("enabled_by_default", &FGD::Entity::FieldFlags::Flag::enabledByDefault)
			.def_ro("description",        &FGD::Entity::FieldFlags::Flag::description);

	py::class_<FGD::Entity::FieldFlags>(toolpp, "FGDEntityFieldFlags")
			.def_ro("name",          &FGD::Entity::FieldFlags::name)
			.def_ro("readonly",      &FGD::Entity::FieldFlags::readonly)
			.def_ro("reportable",    &FGD::Entity::FieldFlags::reportable)
			.def_ro("display_name",  &FGD::Entity::FieldFlags::displayName)
			.def_ro("description",   &FGD::Entity::FieldFlags::description)
			.def_ro("flags",         &FGD::Entity::FieldFlags::flags);

	py::class_<FGD::Entity::IO>(toolpp, "FGDEntityIO")
			.def_ro("name",        &FGD::Entity::IO::name)
			.def_ro("value_type",  &FGD::Entity::IO::valueType)
			.def_ro("description", &FGD::Entity::IO::description);

	py::class_<FGD::Entity>(toolpp, "FGDEntity")
			.def_ro("class_type",          &FGD::Entity::classType)
			.def_ro("class_properties",    &FGD::Entity::classProperties)
			.def_ro("description",         &FGD::Entity::description)
			.def_ro("fields",              &FGD::Entity::fields)
			.def_ro("fields_with_choices", &FGD::Entity::fieldsWithChoices)
			.def_ro("fields_with_flags",   &FGD::Entity::fieldsWithFlags)
			.def_ro("inputs",              &FGD::Entity::inputs)
			.def_ro("outputs",             &FGD::Entity::outputs);

	py::class_<FGD::AutoVisGroup>(toolpp, "FGDAutoVisGroup")
			.def_ro("parent_name", &FGD::AutoVisGroup::parentName)
			.def_ro("name",        &FGD::AutoVisGroup::name)
			.def_ro("entities",    &FGD::AutoVisGroup::entities);

	py::class_<FGD>(toolpp, "FGD")
			.def(py::init<>())
			.def(py::init<const std::string&>(), py::arg("fgd_path"))
			.def("load", &FGD::load, py::arg("fgd_path"))
			.def_prop_ro("version", &FGD::getVersion)
			.def_prop_ro("map_size", &FGD::getMapSize)
			.def_prop_ro("entities", &FGD::getEntities)
			.def_prop_ro("material_exclusion_dirs", &FGD::getMaterialExclusionDirs)
			.def_prop_ro("auto_visgroups", &FGD::getAutoVisGroups);

	py::class_<FGDWriter::AutoVisGroupWriter>(toolpp, "FGDWriterAutoVisGroupWriter")
	        .def("visgroup", &FGDWriter::AutoVisGroupWriter::visGroup, py::arg("name"), py::arg("entities"), py::rv_policy::reference)
			.def("end_auto_visgroup", &FGDWriter::AutoVisGroupWriter::endAutoVisGroup,                       py::rv_policy::reference);

	py::class_<FGDWriter::EntityWriter::KeyValueChoicesWriter>(toolpp, "FGDWriterEntityWriterKeyValueChoicesWriter")
	        .def("choice", &FGDWriter::EntityWriter::KeyValueChoicesWriter::choice, py::arg("value"), py::arg("display_name"), py::rv_policy::reference)
			.def("end_key_value_choices", &FGDWriter::EntityWriter::KeyValueChoicesWriter::endKeyValueChoices,                 py::rv_policy::reference);

	py::class_<FGDWriter::EntityWriter::KeyValueFlagsWriter>(toolpp, "FGDWriterEntityWriterKeyValueFlagsWriter")
	        .def("flag", &FGDWriter::EntityWriter::KeyValueFlagsWriter::flag, py::arg("value"), py::arg("display_name"), py::arg("enabled_by_default"), py::arg("description") = "", py::rv_policy::reference)
			.def("end_key_value_flags", &FGDWriter::EntityWriter::KeyValueFlagsWriter::endKeyValueFlags,                                                                             py::rv_policy::reference);

	py::class_<FGDWriter::EntityWriter>(toolpp, "FGDWriterEntityWriter")
	        .def("key_value", &FGDWriter::EntityWriter::keyValue, py::arg("name"), py::arg("value_type"), py::arg("display_name") = "", py::arg("value_default") = "", py::arg("description") = "", py::arg("readonly") = false, py::arg("report") = false, py::rv_policy::reference)
			.def("begin_key_value_choices", &FGDWriter::EntityWriter::beginKeyValueChoices, py::arg("name"), py::arg("display_name") = "", py::arg("value_default") = "", py::arg("description") = "", py::arg("readonly") = false, py::arg("report") = false)
			.def("begin_key_value_flags", &FGDWriter::EntityWriter::beginKeyValueFlags, py::arg("name"), py::arg("display_name") = "", py::arg("description") = "", py::arg("readonly") = false, py::arg("report") = false)
			.def("input", &FGDWriter::EntityWriter::input, py::arg("name"), py::arg("value_type"), py::arg("description") = "", py::rv_policy::reference)
			.def("output", &FGDWriter::EntityWriter::output, py::arg("name"), py::arg("value_type"), py::arg("description") = "", py::rv_policy::reference)
			.def("end_entity", &FGDWriter::EntityWriter::endEntity, py::rv_policy::reference);

	py::class_<FGDWriter>(toolpp, "FGDWriter")
	        .def_static("begin", &FGDWriter::begin)
			.def("include", &FGDWriter::include, py::arg("fgd_path"), py::rv_policy::reference)
			.def("version", &FGDWriter::version, py::arg("version"), py::rv_policy::reference)
			.def("map_size", &FGDWriter::mapSize, py::arg("map_size"), py::rv_policy::reference)
			.def("material_exclusion_dirs", &FGDWriter::materialExclusionDirs, py::arg("material_exclusion_dirs"), py::rv_policy::reference)
			.def("begin_auto_visgroup", &FGDWriter::beginAutoVisGroup, py::arg("parent_name"))
			.def("begin_entity", &FGDWriter::beginEntity, py::arg("class_type"), py::arg("class_properties"), py::arg("name"), py::arg("description") = "")
	        .def("bake", [](const FGDWriter& self) {
		        const auto d = self.bake();
		        return py::bytes{d.data(), d.size()};
	        })
	        .def("bake_to_file", py::overload_cast<const std::string&>(&FGDWriter::bake, py::const_), py::arg("path"));
}

} // namespace vcryptpp
