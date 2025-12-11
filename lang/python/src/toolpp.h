// ReSharper disable CppLocalVariableMayBeConst

#pragma once

#include <nanobind/nanobind.h>

namespace py = nanobind;
using namespace py::literals;

#include <toolpp/toolpp.h>

namespace toolpp {

inline void register_python(py::module_& m) {
	auto toolpp = m.def_submodule("toolpp");
	using namespace toolpp;

	auto cCmdSeq = py::class_<CmdSeq>(toolpp, "CmdSeq");
	auto cCmdSeqCommand = py::class_<CmdSeq::Command>(cCmdSeq, "Command");

	py::enum_<CmdSeq::Command::Special>(cCmdSeqCommand, "Special")
		.value("NONE",                      CmdSeq::Command::Special::NONE)
		.value("CHANGE_DIRECTORY",          CmdSeq::Command::Special::CHANGE_DIRECTORY)
		.value("COPY_FILE",                 CmdSeq::Command::Special::COPY_FILE)
		.value("DELETE_FILE",               CmdSeq::Command::Special::DELETE_FILE)
		.value("RENAME_FILE",               CmdSeq::Command::Special::RENAME_FILE)
		.value("COPY_FILE_IF_EXISTS_ALIAS", CmdSeq::Command::SPECIAL_COPY_FILE_IF_EXISTS_ALIAS)
		.value("COPY_FILE_IF_EXISTS",       CmdSeq::Command::Special::COPY_FILE_IF_EXISTS);

	cCmdSeqCommand
		.def_rw("enabled",                             &CmdSeq::Command::enabled)
		.def_rw("executable",                          &CmdSeq::Command::executable)
		.def_rw("arguments",                           &CmdSeq::Command::arguments)
		.def_rw("ensure_file_exists",                  &CmdSeq::Command::ensureFileExists)
		.def_rw("path_to_theoretically_existing_file", &CmdSeq::Command::pathToTheoreticallyExistingFile)
		.def_rw("use_process_window",                  &CmdSeq::Command::useProcessWindow)
		.def_rw("wait_for_keypress",                   &CmdSeq::Command::waitForKeypress)
		.def_static("get_special_display_name_for", &CmdSeq::Command::getSpecialDisplayNameFor, "special"_a)
		.def("get_executable_display_name", &CmdSeq::Command::getExecutableDisplayName);

	py::class_<CmdSeq::Sequence>(cCmdSeq, "Sequence")
		.def_rw("name",     &CmdSeq::Sequence::name)
		.def_rw("commands", &CmdSeq::Sequence::commands);

	py::enum_<CmdSeq::Type>(cCmdSeq, "Type")
		.value("INVALID",          CmdSeq::Type::INVALID)
		.value("BINARY",           CmdSeq::Type::BINARY)
		.value("KEYVALUES_STRATA", CmdSeq::Type::KEYVALUES_STRATA);

	cCmdSeq
		.def(py::init<const std::filesystem::path&>(), "path"_a)
		.def(py::init<CmdSeq::Type>(), "type"_a)
		.def("__bool__", &CmdSeq::operator bool, py::is_operator())
		.def_prop_rw("type", &CmdSeq::getType, &CmdSeq::setType)
		.def_prop_ro("version", &CmdSeq::getVersion)
		.def("set_version", &CmdSeq::setVersion, "is_v02"_a)
		.def("sequences", py::overload_cast<>(&CmdSeq::getSequences), py::rv_policy::reference_internal)
		.def("bake", [](const CmdSeq& self) {
			const auto d = self.bake();
			return py::bytes{d.data(), d.size()};
		})
		.def("bake_to_file", py::overload_cast<const std::filesystem::path&>(&CmdSeq::bake, py::const_), "path"_a);

	auto cFGD = py::class_<FGD>(toolpp, "FGD");
	auto cFGDEntity = py::class_<FGD::Entity>(cFGD, "Entity");

	py::class_<FGD::Entity::ClassProperty>(cFGDEntity, "ClassProperty")
		.def_ro("name",      &FGD::Entity::ClassProperty::name)
		.def_ro("arguments", &FGD::Entity::ClassProperty::arguments);

	py::class_<FGD::Entity::Field>(cFGDEntity, "Field")
		.def_ro("name",          &FGD::Entity::Field::name)
		.def_ro("value_type",    &FGD::Entity::Field::valueType)
		.def_ro("readonly",      &FGD::Entity::Field::readonly)
		.def_ro("reportable",    &FGD::Entity::Field::reportable)
		.def_ro("display_name",  &FGD::Entity::Field::displayName)
		.def_ro("value_default", &FGD::Entity::Field::valueDefault)
		.def_ro("description",   &FGD::Entity::Field::description);

	auto cFGDEntityFieldChoices = py::class_<FGD::Entity::FieldChoices>(cFGDEntity, "FieldChoices");

	py::class_<FGD::Entity::FieldChoices::Choice>(cFGDEntityFieldChoices, "Choice")
		.def_ro("value",        &FGD::Entity::FieldChoices::Choice::value)
		.def_ro("display_name", &FGD::Entity::FieldChoices::Choice::displayName);

	cFGDEntityFieldChoices
		.def_ro("name",          &FGD::Entity::FieldChoices::name)
		.def_ro("readonly",      &FGD::Entity::FieldChoices::readonly)
		.def_ro("reportable",    &FGD::Entity::FieldChoices::reportable)
		.def_ro("display_name",  &FGD::Entity::FieldChoices::displayName)
		.def_ro("value_default", &FGD::Entity::FieldChoices::valueDefault)
		.def_ro("description",   &FGD::Entity::FieldChoices::description)
		.def_ro("choices",       &FGD::Entity::FieldChoices::choices);

	auto cFGDEntityFieldFlags = py::class_<FGD::Entity::FieldFlags>(toolpp, "FGDEntityFieldFlags");

	py::class_<FGD::Entity::FieldFlags::Flag>(cFGDEntityFieldFlags, "Flag")
		.def_ro("value",              &FGD::Entity::FieldFlags::Flag::value)
		.def_ro("display_name",       &FGD::Entity::FieldFlags::Flag::displayName)
		.def_ro("enabled_by_default", &FGD::Entity::FieldFlags::Flag::enabledByDefault)
		.def_ro("description",        &FGD::Entity::FieldFlags::Flag::description);

	cFGDEntityFieldFlags
		.def_ro("name",          &FGD::Entity::FieldFlags::name)
		.def_ro("readonly",      &FGD::Entity::FieldFlags::readonly)
		.def_ro("reportable",    &FGD::Entity::FieldFlags::reportable)
		.def_ro("display_name",  &FGD::Entity::FieldFlags::displayName)
		.def_ro("description",   &FGD::Entity::FieldFlags::description)
		.def_ro("flags",         &FGD::Entity::FieldFlags::flags);

	py::class_<FGD::Entity::IO>(cFGDEntity, "IO")
		.def_ro("name",        &FGD::Entity::IO::name)
		.def_ro("value_type",  &FGD::Entity::IO::valueType)
		.def_ro("description", &FGD::Entity::IO::description);

	cFGDEntity
		.def_ro("class_type",          &FGD::Entity::classType)
		.def_ro("class_properties",    &FGD::Entity::classProperties)
		.def_ro("description",         &FGD::Entity::description)
		.def_ro("docs_url",            &FGD::Entity::docsURL)
		.def_ro("fields",              &FGD::Entity::fields)
		.def_ro("fields_with_choices", &FGD::Entity::fieldsWithChoices)
		.def_ro("fields_with_flags",   &FGD::Entity::fieldsWithFlags)
		.def_ro("inputs",              &FGD::Entity::inputs)
		.def_ro("outputs",             &FGD::Entity::outputs);

	py::class_<FGD::AutoVisGroup>(cFGD, "AutoVisGroup")
		.def_ro("parent_name", &FGD::AutoVisGroup::parentName)
		.def_ro("name",        &FGD::AutoVisGroup::name)
		.def_ro("entities",    &FGD::AutoVisGroup::entities);

	cFGD
		.def(py::init())
		.def(py::init<const std::filesystem::path&>(), "fgd_path"_a)
		.def("load", &FGD::load, "fgd_path"_a)
		.def_prop_ro("version", &FGD::getVersion)
		.def_prop_ro("map_size", &FGD::getMapSize)
		.def_prop_ro("entities", &FGD::getEntities)
		.def_prop_ro("material_exclusion_dirs", &FGD::getMaterialExclusionDirs)
		.def_prop_ro("auto_visgroups", &FGD::getAutoVisGroups);

	auto cFGDWriter = py::class_<FGDWriter>(toolpp, "FGDWriter");

	py::class_<FGDWriter::AutoVisGroupWriter>(cFGDWriter, "AutoVisGroupWriter")
		.def("visgroup", py::overload_cast<std::string_view, std::span<const std::string_view>>(&FGDWriter::AutoVisGroupWriter::visGroup), "name"_a, "entities"_a, py::rv_policy::reference)
		.def("end_auto_visgroup", &FGDWriter::AutoVisGroupWriter::endAutoVisGroup, py::rv_policy::reference);

	auto cFGDWriterEntityWriter = py::class_<FGDWriter::EntityWriter>(cFGDWriter, "EntityWriter");

	py::class_<FGDWriter::EntityWriter::KeyValueChoicesWriter>(cFGDWriterEntityWriter, "KeyValueChoicesWriter")
		.def("choice", &FGDWriter::EntityWriter::KeyValueChoicesWriter::choice, "value"_a, "display_name"_a, py::rv_policy::reference)
		.def("end_key_value_choices", &FGDWriter::EntityWriter::KeyValueChoicesWriter::endKeyValueChoices,                 py::rv_policy::reference);

	py::class_<FGDWriter::EntityWriter::KeyValueFlagsWriter>(cFGDWriterEntityWriter, "KeyValueFlagsWriter")
		.def("flag", &FGDWriter::EntityWriter::KeyValueFlagsWriter::flag, "value"_a, "display_name"_a, "enabled_by_default"_a, "description"_a = "", py::rv_policy::reference)
		.def("end_key_value_flags", &FGDWriter::EntityWriter::KeyValueFlagsWriter::endKeyValueFlags,                                                                             py::rv_policy::reference);

	cFGDWriterEntityWriter
		.def("key_value", &FGDWriter::EntityWriter::keyValue, "name"_a, "value_type"_a, "display_name"_a = "", "value_default"_a = "", "description"_a = "", "readonly"_a = false, "report"_a = false, py::rv_policy::reference)
		.def("begin_key_value_choices", &FGDWriter::EntityWriter::beginKeyValueChoices, "name"_a, "display_name"_a = "", "value_default"_a = "", "description"_a = "", "readonly"_a = false, "report"_a = false)
		.def("begin_key_value_flags", &FGDWriter::EntityWriter::beginKeyValueFlags, "name"_a, "display_name"_a = "", "description"_a = "", "readonly"_a = false, "report"_a = false)
		.def("input", &FGDWriter::EntityWriter::input, "name"_a, "value_type"_a, "description"_a = "", py::rv_policy::reference)
		.def("output", &FGDWriter::EntityWriter::output, "name"_a, "value_type"_a, "description"_a = "", py::rv_policy::reference)
		.def("end_entity", &FGDWriter::EntityWriter::endEntity, py::rv_policy::reference);

	cFGDWriter
		.def_static("begin", &FGDWriter::begin)
		.def("include", &FGDWriter::include, "fgd_path"_a, py::rv_policy::reference)
		.def("version", &FGDWriter::version, "version"_a, py::rv_policy::reference)
		.def("map_size", &FGDWriter::mapSize, "map_size"_a, py::rv_policy::reference)
		.def("material_exclusion_dirs", py::overload_cast<std::span<const std::string_view>>(&FGDWriter::materialExclusionDirs), "material_exclusion_dirs"_a, py::rv_policy::reference)
		.def("begin_auto_visgroup", &FGDWriter::beginAutoVisGroup, "parent_name"_a)
		.def("begin_entity", py::overload_cast<std::string_view, std::span<const std::string_view>, std::string_view, std::string_view, std::string_view>(&FGDWriter::beginEntity), "class_type"_a, "class_properties"_a, "name"_a, "description"_a = "", "docs_url"_a = "")
		.def("bake", [](const FGDWriter& self) {
			const auto d = self.bake();
			return py::bytes{d.data(), d.size()};
		})
		.def("bake_to_file", py::overload_cast<const std::filesystem::path&>(&FGDWriter::bake, py::const_), "path"_a);
}

} // namespace vcryptpp
