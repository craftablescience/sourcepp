// ReSharper disable CppLocalVariableMayBeConst

#pragma once

#include <nanobind/nanobind.h>
#include <nanobind/make_iterator.h>

namespace py = nanobind;
using namespace py::literals;

#include <kvpp/kvpp.h>

#include <utility>

namespace kvpp {

inline void register_python(py::module_& m) {
	auto kvpp = m.def_submodule("kvpp");
	using namespace kvpp;

	{
		auto DMXValue = kvpp.def_submodule("DMXValue");
		using namespace DMXValue;

		py::class_<Element>(DMXValue, "Element")
			.def(py::init())
			.def_rw("index", &Element::index)
			.def_rw("external_guid", &Element::externalGUID);

		py::class_<Time>(DMXValue, "Time")
			.def(py::init())
			.def_rw("seconds", &Time::seconds);

		py::class_<Color>(DMXValue, "Color")
			.def(py::init())
			.def_rw("r", &Color::r)
			.def_rw("g", &Color::g)
			.def_rw("b", &Color::b)
			.def_rw("a", &Color::a);

		py::class_<EulerAngles, sourcepp::math::EulerAngles>(DMXValue, "EulerAngles"); // NOLINT(*-unused-raii)

		py::class_<Quaternion>(DMXValue, "Quaternion"); // NOLINT(*-unused-raii)

		py::enum_<IDv1>(DMXValue, "IDv1")
			.value("INVALID",            IDv1::INVALID)
			.value("VALUE_START",        IDv1::VALUE_START)
			.value("ELEMENT",            IDv1::ELEMENT)
			.value("INT32",              IDv1::INT32)
			.value("FLOAT",              IDv1::FLOAT)
			.value("BOOL",               IDv1::BOOL)
			.value("STRING",             IDv1::STRING)
			.value("BYTEARRAY",          IDv1::BYTEARRAY)
			.value("UUID",               IDv1::UUID)
			.value("COLOR",              IDv1::COLOR)
			.value("VECTOR2",            IDv1::VECTOR2)
			.value("VECTOR3",            IDv1::VECTOR3)
			.value("VECTOR4",            IDv1::VECTOR4)
			.value("EULER_ANGLES",       IDv1::EULER_ANGLES)
			.value("QUATERNION",         IDv1::QUATERNION)
			.value("MATRIX_4X4",         IDv1::MATRIX_4X4)
			.value("VALUE_END",          IDv1::VALUE_END)
			.value("ARRAY_START",        IDv1::ARRAY_START)
			.value("ARRAY_ELEMENT",      IDv1::ARRAY_ELEMENT)
			.value("ARRAY_INT32",        IDv1::ARRAY_INT32)
			.value("ARRAY_FLOAT",        IDv1::ARRAY_FLOAT)
			.value("ARRAY_BOOL",         IDv1::ARRAY_BOOL)
			.value("ARRAY_STRING",       IDv1::ARRAY_STRING)
			.value("ARRAY_BYTEARRAY",    IDv1::ARRAY_BYTEARRAY)
			.value("ARRAY_UUID",         IDv1::ARRAY_UUID)
			.value("ARRAY_COLOR",        IDv1::ARRAY_COLOR)
			.value("ARRAY_VECTOR2",      IDv1::ARRAY_VECTOR2)
			.value("ARRAY_VECTOR3",      IDv1::ARRAY_VECTOR3)
			.value("ARRAY_VECTOR4",      IDv1::ARRAY_VECTOR4)
			.value("ARRAY_EULER_ANGLES", IDv1::ARRAY_EULER_ANGLES)
			.value("ARRAY_QUATERNION",   IDv1::ARRAY_QUATERNION)
			.value("ARRAY_MATRIX_4X4",   IDv1::ARRAY_MATRIX_4X4)
			.value("ARRAY_END",          IDv1::ARRAY_END);

		py::enum_<IDv2>(DMXValue, "IDv2")
			.value("INVALID",            IDv2::INVALID)
			.value("VALUE_START",        IDv2::VALUE_START)
			.value("ELEMENT",            IDv2::ELEMENT)
			.value("INT32",              IDv2::INT32)
			.value("FLOAT",              IDv2::FLOAT)
			.value("BOOL",               IDv2::BOOL)
			.value("STRING",             IDv2::STRING)
			.value("BYTEARRAY",          IDv2::BYTEARRAY)
			.value("TIME",               IDv2::TIME)
			.value("COLOR",              IDv2::COLOR)
			.value("VECTOR2",            IDv2::VECTOR2)
			.value("VECTOR3",            IDv2::VECTOR3)
			.value("VECTOR4",            IDv2::VECTOR4)
			.value("EULER_ANGLES",       IDv2::EULER_ANGLES)
			.value("QUATERNION",         IDv2::QUATERNION)
			.value("MATRIX_4X4",         IDv2::MATRIX_4X4)
			.value("VALUE_END",          IDv2::VALUE_END)
			.value("ARRAY_START",        IDv2::ARRAY_START)
			.value("ARRAY_ELEMENT",      IDv2::ARRAY_ELEMENT)
			.value("ARRAY_INT32",        IDv2::ARRAY_INT32)
			.value("ARRAY_FLOAT",        IDv2::ARRAY_FLOAT)
			.value("ARRAY_BOOL",         IDv2::ARRAY_BOOL)
			.value("ARRAY_STRING",       IDv2::ARRAY_STRING)
			.value("ARRAY_BYTEARRAY",    IDv2::ARRAY_BYTEARRAY)
			.value("ARRAY_TIME",         IDv2::ARRAY_TIME)
			.value("ARRAY_COLOR",        IDv2::ARRAY_COLOR)
			.value("ARRAY_VECTOR2",      IDv2::ARRAY_VECTOR2)
			.value("ARRAY_VECTOR3",      IDv2::ARRAY_VECTOR3)
			.value("ARRAY_VECTOR4",      IDv2::ARRAY_VECTOR4)
			.value("ARRAY_EULER_ANGLES", IDv2::ARRAY_EULER_ANGLES)
			.value("ARRAY_QUATERNION",   IDv2::ARRAY_QUATERNION)
			.value("ARRAY_MATRIX_4X4",   IDv2::ARRAY_MATRIX_4X4)
			.value("ARRAY_END",          IDv2::ARRAY_END);

		py::enum_<IDv3>(DMXValue, "IDv3")
			.value("INVALID",            IDv3::INVALID)
			.value("VALUE_START",        IDv3::VALUE_START)
			.value("ELEMENT",            IDv3::ELEMENT)
			.value("INT32",              IDv3::INT32)
			.value("FLOAT",              IDv3::FLOAT)
			.value("BOOL",               IDv3::BOOL)
			.value("STRING",             IDv3::STRING)
			.value("BYTEARRAY",          IDv3::BYTEARRAY)
			.value("TIME",               IDv3::TIME)
			.value("COLOR",              IDv3::COLOR)
			.value("VECTOR2",            IDv3::VECTOR2)
			.value("VECTOR3",            IDv3::VECTOR3)
			.value("VECTOR4",            IDv3::VECTOR4)
			.value("EULER_ANGLES",       IDv3::EULER_ANGLES)
			.value("QUATERNION",         IDv3::QUATERNION)
			.value("MATRIX_4X4",         IDv3::MATRIX_4X4)
			.value("UINT64",             IDv3::UINT64)
			.value("UINT8",              IDv3::UINT8)
			.value("VALUE_END",          IDv3::VALUE_END)
			.value("ARRAY_START",        IDv3::ARRAY_START)
			.value("ARRAY_ELEMENT",      IDv3::ARRAY_ELEMENT)
			.value("ARRAY_INT32",        IDv3::ARRAY_INT32)
			.value("ARRAY_FLOAT",        IDv3::ARRAY_FLOAT)
			.value("ARRAY_BOOL",         IDv3::ARRAY_BOOL)
			.value("ARRAY_STRING",       IDv3::ARRAY_STRING)
			.value("ARRAY_BYTEARRAY",    IDv3::ARRAY_BYTEARRAY)
			.value("ARRAY_TIME",         IDv3::ARRAY_TIME)
			.value("ARRAY_COLOR",        IDv3::ARRAY_COLOR)
			.value("ARRAY_VECTOR2",      IDv3::ARRAY_VECTOR2)
			.value("ARRAY_VECTOR3",      IDv3::ARRAY_VECTOR3)
			.value("ARRAY_VECTOR4",      IDv3::ARRAY_VECTOR4)
			.value("ARRAY_EULER_ANGLES", IDv3::ARRAY_EULER_ANGLES)
			.value("ARRAY_QUATERNION",   IDv3::ARRAY_QUATERNION)
			.value("ARRAY_MATRIX_4X4",   IDv3::ARRAY_MATRIX_4X4)
			.value("ARRAY_UINT64",       IDv3::UINT64)
			.value("ARRAY_UINT8",        IDv3::UINT8)
			.value("ARRAY_END",          IDv3::ARRAY_END);

		py::enum_<IDVersion>(DMXValue, "IDVersion")
			.value("V1", IDVersion::V1)
			.value("V2", IDVersion::V2)
			.value("V3", IDVersion::V3);

		py::enum_<ID>(DMXValue, "ID")
			.value("INVALID",            ID::INVALID)
			.value("VALUE_START",        ID::VALUE_START)
			.value("ELEMENT",            ID::ELEMENT)
			.value("INT32",              ID::INT32)
			.value("FLOAT",              ID::FLOAT)
			.value("BOOL",               ID::BOOL)
			.value("STRING",             ID::STRING)
			.value("BYTEARRAY",          ID::BYTEARRAY)
			.value("UUID",               ID::UUID)
			.value("TIME",               ID::TIME)
			.value("COLOR",              ID::COLOR)
			.value("VECTOR2",            ID::VECTOR2)
			.value("VECTOR3",            ID::VECTOR3)
			.value("VECTOR4",            ID::VECTOR4)
			.value("EULER_ANGLES",       ID::EULER_ANGLES)
			.value("QUATERNION",         ID::QUATERNION)
			.value("MATRIX_4X4",         ID::MATRIX_4X4)
			.value("UINT64",             ID::UINT64)
			.value("UINT8",              ID::UINT8)
			.value("VALUE_END",          ID::VALUE_END)
			.value("ARRAY_START",        ID::ARRAY_START)
			.value("ARRAY_ELEMENT",      ID::ARRAY_ELEMENT)
			.value("ARRAY_INT32",        ID::ARRAY_INT32)
			.value("ARRAY_FLOAT",        ID::ARRAY_FLOAT)
			.value("ARRAY_BOOL",         ID::ARRAY_BOOL)
			.value("ARRAY_STRING",       ID::ARRAY_STRING)
			.value("ARRAY_BYTEARRAY",    ID::ARRAY_BYTEARRAY)
			.value("ARRAY_UUID",         ID::ARRAY_UUID)
			.value("ARRAY_TIME",         ID::ARRAY_TIME)
			.value("ARRAY_COLOR",        ID::ARRAY_COLOR)
			.value("ARRAY_VECTOR2",      ID::ARRAY_VECTOR2)
			.value("ARRAY_VECTOR3",      ID::ARRAY_VECTOR3)
			.value("ARRAY_VECTOR4",      ID::ARRAY_VECTOR4)
			.value("ARRAY_EULER_ANGLES", ID::ARRAY_EULER_ANGLES)
			.value("ARRAY_QUATERNION",   ID::ARRAY_QUATERNION)
			.value("ARRAY_MATRIX_4X4",   ID::ARRAY_MATRIX_4X4)
			.value("ARRAY_UINT64",       ID::ARRAY_UINT64)
			.value("ARRAY_UINT8",        ID::ARRAY_UINT8)
			.value("ARRAY_END",          ID::ARRAY_END);

		DMXValue.def("decode_id_v1", py::overload_cast<IDv1>(&decodeID), "id"_a);

		DMXValue.def("decode_id_v2", py::overload_cast<IDv2>(&decodeID), "id"_a);

		DMXValue.def("decode_id_v3", py::overload_cast<IDv3>(&decodeID), "id"_a);

		DMXValue.def("encode_id", [](ID id, IDVersion version) -> std::pair<uint8_t, bool> {
			bool incompatible;
			const auto out = static_cast<uint8_t>(encodeID(id, version, incompatible));
			return {out, incompatible};
		}, "id"_a, "version"_a);

		DMXValue.def("array_id_to_inner_id", &arrayIDToInnerID, "id"_a);

		DMXValue.def("inner_id_to_array_id", &innerIDToArrayID, "id"_a);

		DMXValue.def("id_to_string", &idToString, "id"_a);

		DMXValue.def("string_to_id", &stringToID, "id"_a);
	}

	py::class_<DMXAttribute>(kvpp, "DMXAttribute")
		.def("is_invalid", &DMXAttribute::isInvalid)
		.def("__bool__", &DMXAttribute::operator bool)
		.def_prop_rw("key", &DMXAttribute::getKey, &DMXAttribute::setKey)
		.def_prop_ro("value_type", &DMXAttribute::getValueType)
		.def("is_value_array", &DMXAttribute::isValueArray)
		.def_prop_rw("value", [](const DMXAttribute& self) {
			return self.getValue();
		}, [](DMXAttribute& self, DMXValue::Generic value) {
			self.setValue(std::move(value));
		})
		.def("get_value_string", &DMXAttribute::getValueString);

	py::class_<DMXElement>(kvpp, "DMXElement")
		.def("__bool__", &DMXElement::operator bool)
		.def_prop_rw("type", &DMXElement::getType, &DMXElement::setType)
		.def_prop_rw("key", &DMXElement::getKey, &DMXElement::setKey)
		.def_prop_rw("uuid", &DMXElement::getUUID, &DMXElement::setUUID)
		.def("has_attribute", &DMXElement::hasAttribute, "attribute_key"_a)
		.def("__contains__", &DMXElement::hasAttribute, "attribute_key"_a)
		.def("add_attribute", &DMXElement::addAttribute, "key"_a, "value"_a = DMXValue::Generic{}, py::rv_policy::reference_internal)
		.def("get_attribute_count", py::overload_cast<>(&DMXElement::getAttributeCount, py::const_))
		.def("__len__", py::overload_cast<>(&DMXElement::getAttributeCount, py::const_))
		.def("get_attribute_count_with_key", py::overload_cast<std::string_view>(&DMXElement::getAttributeCount, py::const_))
		.def("get_attributes", py::overload_cast<>(&DMXElement::getAttributes))
		.def("__iter__", [](const DMXElement& self) {
			return py::make_iterator(py::type<DMXElement>(), "iterator", self);
		}, py::keep_alive<0, 1>())
		.def("__getitem__", py::overload_cast<unsigned int>(&DMXElement::operator[]), "n"_a, py::rv_policy::reference_internal)
		.def("__getitem__", py::overload_cast<std::string_view>(&DMXElement::operator[]), "attribute_key"_a, py::rv_policy::reference_internal)
		.def("get_child", py::overload_cast<std::string_view, unsigned int>(&DMXElement::operator()), "attribute_key"_a, "n"_a, py::rv_policy::reference_internal)
		.def("remove_child", py::overload_cast<unsigned int>(&DMXElement::removeAttribute), "n"_a)
		.def("__delitem__", py::overload_cast<unsigned int>(&DMXElement::removeAttribute), "n"_a)
		.def("remove_child", py::overload_cast<std::string_view, int>(&DMXElement::removeAttribute), "attribute_key"_a, "n"_a = -1)
		.def("get_invalid_attribute", &DMXElement::getInvalidAttribute);

	auto cDMX = py::class_<DMX>(kvpp, "DMX");

	py::enum_<DMX::Encoding>(cDMX, "Encoding")
		.value("INVALID",             DMX::ENCODING_INVALID)
		.value("BINARY_OLD",          DMX::ENCODING_BINARY_OLD)
		.value("BINARY_OLD_SFM",      DMX::ENCODING_BINARY_OLD_SFM)
		.value("BINARY",              DMX::ENCODING_BINARY)
		.value("BINARY_UTF8",         DMX::ENCODING_BINARY_UTF8)
		.value("BINARY_SEQIDS",       DMX::ENCODING_BINARY_SEQIDS)
		.value("KEYVALUES2_OLD",      DMX::ENCODING_KEYVALUES2_OLD)
		.value("KEYVALUES2_FLAT_OLD", DMX::ENCODING_KEYVALUES2_FLAT_OLD)
		.value("KEYVALUES2",          DMX::ENCODING_KEYVALUES2)
		.value("KEYVALUES2_UTF8",     DMX::ENCODING_KEYVALUES2_UTF8)
		.value("KEYVALUES2_FLAT",     DMX::ENCODING_KEYVALUES2_FLAT)
		.value("KEYVALUES2_NOIDS",    DMX::ENCODING_KEYVALUES2_NOIDS);

	cDMX
		.def(py::init<DMX::Encoding, int, std::string, int>(), "encoding_type"_a, "encoding_version"_a, "format_type"_a, "format_version"_a)
		.def("__init__", [](DMX* self, const py::bytes& dmxData) {
			return new(self) DMX{std::span{static_cast<const std::byte*>(dmxData.data()), dmxData.size()}};
		}, "dmx_data"_a)
		.def(py::init<std::string_view>(), "dmx_data"_a)
		.def("__bool__", &DMX::operator bool)
		.def_prop_rw("encoding_type", &DMX::getEncodingType, &DMX::setEncodingType)
		.def_prop_rw("encoding_version", &DMX::getEncodingVersion, &DMX::setEncodingVersion)
		.def_prop_rw("format_type", &DMX::getFormatType, &DMX::setFormatType)
		.def_prop_rw("format_version", &DMX::getFormatVersion, &DMX::setFormatVersion)
		.def("add_prefix_attribute_container", &DMX::addPrefixAttributeContainer, py::rv_policy::reference_internal)
		.def("get_prefix_attribute_container_count", &DMX::getPrefixAttributeContainerCount)
		.def("get_prefix_attribute_containers", py::overload_cast<>(&DMX::getPrefixAttributeContainers))
		.def("remove_prefix_attribute_container", &DMX::removePrefixAttributeContainer, "n"_a)
		.def("has_element", &DMX::hasElement, "key"_a)
		.def("__contains__", &DMX::hasElement, "key"_a)
		.def("add_element", &DMX::addElement, "type"_a, "key"_a)
		.def("get_element_count", py::overload_cast<>(&DMX::getElementCount, py::const_))
		.def("__len__", py::overload_cast<>(&DMX::getElementCount, py::const_))
		.def("get_element_count_with_key", py::overload_cast<std::string_view>(&DMX::getElementCount, py::const_), "key"_a)
		.def("get_elements", py::overload_cast<>(&DMX::getElements))
		.def("__iter__", [](const DMX& self) {
			return py::make_iterator(py::type<DMX>(), "iterator", self);
		}, py::keep_alive<0, 1>())
		.def("__getitem__", py::overload_cast<unsigned int>(&DMX::operator[]), "n"_a, py::rv_policy::reference_internal)
		.def("__getitem__", py::overload_cast<std::string_view>(&DMX::operator[]), "key"_a, py::rv_policy::reference_internal)
		.def("get_child", py::overload_cast<std::string_view, unsigned int>(&DMX::operator()), "key"_a, "n"_a, py::rv_policy::reference_internal)
		.def("remove_child", py::overload_cast<unsigned int>(&DMX::removeElement), "n"_a)
		.def("__delitem__", py::overload_cast<unsigned int>(&DMX::removeElement), "n"_a)
		.def("bake", [](const DMX& self) {
			const auto d = self.bake();
			return py::bytes{d.data(), d.size()};
		})
		.def("bake_to_file", py::overload_cast<const std::string&>(&DMX::bake, py::const_), "dmx_path"_a)
		.def_static("is_encoding_version_valid", &DMX::isEncodingVersionValid, "encoding_type"_a, "encoding_version"_a)
		.def_static("create_random_uuid", &DMX::createRandomUUID)
		.def_static("get_invalid_element", &DMX::getInvalidElement);

	py::class_<KV1ElementReadable<>>(kvpp, "KV1ElementReadable")
		.def_prop_ro("key", &KV1ElementReadable<>::getKey)
		.def_prop_ro("value", [](const KV1ElementReadable<>& self) {
			return self.getValue();
		})
		.def_prop_ro("conditional", &KV1ElementReadable<>::getConditional)
		.def("has_child", &KV1ElementReadable<>::hasChild, "child_key"_a)
		.def("__contains__", &KV1ElementReadable<>::hasChild, "child_key"_a)
		.def_prop_ro("child_count", py::overload_cast<>(&KV1ElementReadable<>::getChildCount, py::const_))
		.def("__len__", py::overload_cast<>(&KV1ElementReadable<>::getChildCount, py::const_))
		.def("get_child_count_with_key", py::overload_cast<std::string_view>(&KV1ElementReadable<>::getChildCount, py::const_), "child_key"_a)
		.def("get_children", py::overload_cast<>(&KV1ElementReadable<>::getChildren, py::const_))
		.def("__iter__", [](const KV1ElementReadable<>& self) {
			return py::make_iterator(py::type<KV1ElementReadable<>>(), "iterator", self);
		}, py::keep_alive<0, 1>())
		.def("__getitem__", py::overload_cast<unsigned int>(&KV1ElementReadable<>::operator[], py::const_), "n"_a, py::rv_policy::reference_internal)
		.def("__getitem__", py::overload_cast<std::string_view>(&KV1ElementReadable<>::operator[], py::const_), "child_key"_a, py::rv_policy::reference_internal)
		.def("get_child", py::overload_cast<std::string_view, unsigned int>(&KV1ElementReadable<>::operator(), py::const_), "child_key"_a, "n"_a, py::rv_policy::reference_internal)
		.def("is_invalid", &KV1ElementReadable<>::isInvalid)
		.def("get_invalid", &KV1ElementReadable<>::getInvalid)
		.def("__bool__", &KV1ElementReadable<>::operator bool);

	py::class_<KV1<>, KV1ElementReadable<>>(kvpp, "KV1")
		.def(py::init<std::string_view, bool>(), "kv1_data"_a, "use_escape_sequences"_a = false);

	py::class_<KV1ElementWritable<>>(kvpp, "KV1ElementWritable")
		.def_prop_rw("key", &KV1ElementWritable<>::getKey, &KV1ElementWritable<>::setKey)
		.def_prop_rw("value", [](const KV1ElementWritable<>& self) {
			return self.getValue();
		}, &KV1ElementWritable<>::setValue<std::string_view>)
		.def_prop_rw("conditional", &KV1ElementWritable<>::getConditional, &KV1ElementWritable<>::setConditional)
		.def("has_child", &KV1ElementWritable<>::hasChild, "child_key"_a)
		.def("__contains__", &KV1ElementWritable<>::hasChild, "child_key"_a)
		.def("add_child", &KV1ElementWritable<>::addChild<>, "key"_a, "value"_a = "", "conditional"_a = "", py::rv_policy::reference_internal)
		.def_prop_ro("child_count", py::overload_cast<>(&KV1ElementWritable<>::getChildCount, py::const_))
		.def("__len__", py::overload_cast<>(&KV1ElementWritable<>::getChildCount, py::const_))
		.def("get_child_count_with_key", py::overload_cast<std::string_view>(&KV1ElementWritable<>::getChildCount, py::const_), "child_key"_a)
		.def("get_children", py::overload_cast<>(&KV1ElementWritable<>::getChildren))
		.def("__iter__", [](const KV1ElementWritable<>& self) {
			return py::make_iterator(py::type<KV1ElementWritable<>>(), "iterator", self);
		}, py::keep_alive<0, 1>())
		.def("__getitem__", py::overload_cast<unsigned int>(&KV1ElementWritable<>::operator[]), "n"_a, py::rv_policy::reference_internal)
		.def("__getitem__", py::overload_cast<std::string_view>(&KV1ElementWritable<>::operator[]), "child_key"_a, py::rv_policy::reference_internal)
		.def("get_child", py::overload_cast<std::string_view, unsigned int>(&KV1ElementWritable<>::operator()), "child_key"_a, "n"_a, py::rv_policy::reference_internal)
		.def("remove_child", py::overload_cast<unsigned int>(&KV1ElementWritable<>::removeChild), "n"_a)
		.def("__delitem__", py::overload_cast<unsigned int>(&KV1ElementWritable<>::removeChild), "n"_a)
		.def("remove_child", py::overload_cast<std::string_view, int>(&KV1ElementWritable<>::removeChild), "child_key"_a, "n"_a = -1)
		.def("is_invalid", &KV1ElementWritable<>::isInvalid)
		.def("get_invalid", &KV1ElementWritable<>::getInvalid)
		.def("__bool__", &KV1ElementWritable<>::operator bool);

	py::class_<KV1Writer<>, KV1ElementWritable<>>(kvpp, "KV1Writer")
		.def(py::init<std::string_view, bool>(), "kv1_data"_a = "", "use_escape_sequences"_a = false)
		.def("bake", py::overload_cast<>(&KV1Writer<>::bake, py::const_))
		.def("bake_to_file", py::overload_cast<const std::string&>(&KV1Writer<>::bake, py::const_), "kv1_path"_a);

	py::enum_<KV1BinaryValueType>(kvpp, "KV1BinaryValueType")
		.value("CHILDREN",   KV1BinaryValueType::CHILDREN)
		.value("STRING",     KV1BinaryValueType::STRING)
		.value("INT32",      KV1BinaryValueType::INT32)
		.value("FLOAT",      KV1BinaryValueType::FLOAT)
		.value("POINTER",    KV1BinaryValueType::POINTER)
		.value("WSTRING",    KV1BinaryValueType::WSTRING)
		.value("COLOR_RGBA", KV1BinaryValueType::COLOR_RGBA)
		.value("UINT64",     KV1BinaryValueType::UINT64)
		.value("COUNT",      KV1BinaryValueType::COUNT);

	py::class_<KV1BinaryElement>(kvpp, "KV1BinaryElement")
		.def(py::init())
		.def_prop_rw("key", &KV1BinaryElement::getKey, &KV1BinaryElement::setKey)
		.def_prop_rw("value", [](const KV1BinaryElement& self) {
			return self.getValue();
		}, [](KV1BinaryElement& self, KV1BinaryValue value) {
			self.setValue(std::move(value));
		})
		.def("has_child", &KV1BinaryElement::hasChild, "child_key"_a)
		.def("__contains__", &KV1BinaryElement::hasChild, "child_key"_a)
		.def("add_child", [](KV1BinaryElement& self, std::string_view key, KV1BinaryValue value = {}) -> KV1BinaryElement& {
			return self.addChild(key, std::move(value));
		}, "key"_a, "value"_a = KV1BinaryValue{}, py::rv_policy::reference_internal)
		.def_prop_ro("child_count", py::overload_cast<>(&KV1BinaryElement::getChildCount, py::const_))
		.def("__len__", py::overload_cast<>(&KV1BinaryElement::getChildCount, py::const_))
		.def("get_child_count_with_key", py::overload_cast<std::string_view>(&KV1BinaryElement::getChildCount, py::const_), "child_key"_a)
		.def("get_children", py::overload_cast<>(&KV1BinaryElement::getChildren))
		.def("__iter__", [](const KV1BinaryElement& self) {
			return py::make_iterator(py::type<KV1BinaryElement>(), "iterator", self);
		}, py::keep_alive<0, 1>())
		.def("__getitem__", py::overload_cast<unsigned int>(&KV1BinaryElement::operator[]), "n"_a, py::rv_policy::reference_internal)
		.def("__getitem__", py::overload_cast<std::string_view>(&KV1BinaryElement::operator[]), "child_key"_a, py::rv_policy::reference_internal)
		.def("get_child", py::overload_cast<std::string_view, unsigned int>(&KV1BinaryElement::operator()), "child_key"_a, "n"_a, py::rv_policy::reference_internal)
		.def("remove_child", py::overload_cast<unsigned int>(&KV1BinaryElement::removeChild), "n"_a)
		.def("__delitem__", py::overload_cast<unsigned int>(&KV1BinaryElement::removeChild), "n"_a)
		.def("remove_child", py::overload_cast<std::string_view, int>(&KV1BinaryElement::removeChild), "child_key"_a, "n"_a = -1)
		.def("is_invalid", &KV1BinaryElement::isInvalid)
		.def("get_invalid", &KV1BinaryElement::getInvalid)
		.def("__bool__", &KV1BinaryElement::operator bool);

	py::class_<KV1Binary, KV1BinaryElement>(kvpp, "KV1Binary")
		.def("__init__", [](KV1Binary* self, const py::bytes& kv1Data) {
			return new(self) KV1Binary{std::span{static_cast<const std::byte*>(kv1Data.data()), kv1Data.size()}};
		}, "kv1_data"_a)
		.def("bake", [](const KV1Binary& self) {
			const auto d = self.bake();
			return py::bytes{d.data(), d.size()};
		})
		.def("bake_to_file", py::overload_cast<const std::string&>(&KV1Binary::bake, py::const_), "kv1_path"_a)
		.def("bake_text", py::overload_cast<>(&KV1Binary::bakeText, py::const_))
		.def("bake_text_to_file", py::overload_cast<const std::string&>(&KV1Binary::bakeText, py::const_), "kv1_path"_a);
}

} // namespace gamepp
