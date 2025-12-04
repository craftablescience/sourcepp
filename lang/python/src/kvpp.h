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

	py::class_<KV1ElementReadable<>>(kvpp, "KV1ElementReadable")
		.def_prop_ro("key", &KV1ElementReadable<>::getKey)
		.def_prop_ro("value", [](const KV1ElementReadable<>& self) {
			return self.getValue();
		})
		.def_prop_ro("conditional", &KV1ElementReadable<>::getConditional)
		.def("has_child", &KV1ElementReadable<>::hasChild, "child_key"_a)
		.def_prop_ro("child_count", py::overload_cast<>(&KV1ElementReadable<>::getChildCount, py::const_))
		.def("__len__", py::overload_cast<>(&KV1ElementReadable<>::getChildCount, py::const_))
		.def("get_child_count_with_key", py::overload_cast<std::string_view>(&KV1ElementReadable<>::getChildCount, py::const_), "child_key"_a)
		.def("__iter__", [](const KV1ElementReadable<>& self) {
			return py::make_iterator(py::type<KV1ElementReadable<>>(), "iterator", self);
		}, py::keep_alive<0, 1>())
		.def("__getitem__", py::overload_cast<unsigned int>(&KV1ElementReadable<>::operator[], py::const_), "n"_a)
		.def("__getitem__", py::overload_cast<std::string_view>(&KV1ElementReadable<>::operator[], py::const_), "child_key"_a)
		.def("get_child", py::overload_cast<std::string_view, unsigned int>(&KV1ElementReadable<>::operator(), py::const_), "child_key"_a, "n"_a)
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
		.def("add_child", &KV1ElementWritable<>::addChild<>, "key"_a, "value"_a = "", "conditional"_a = "")
		.def_prop_ro("child_count", py::overload_cast<>(&KV1ElementWritable<>::getChildCount, py::const_))
		.def("__len__", py::overload_cast<>(&KV1ElementWritable<>::getChildCount, py::const_))
		.def("get_child_count_with_key", py::overload_cast<std::string_view>(&KV1ElementWritable<>::getChildCount, py::const_), "child_key"_a)
		.def("__iter__", [](const KV1ElementWritable<>& self) {
			return py::make_iterator(py::type<KV1ElementWritable<>>(), "iterator", self);
		}, py::keep_alive<0, 1>())
		.def("__getitem__", py::overload_cast<unsigned int>(&KV1ElementWritable<>::operator[]), "n"_a)
		.def("__getitem__", py::overload_cast<std::string_view>(&KV1ElementWritable<>::operator[]), "child_key"_a)
		.def("get_child", py::overload_cast<std::string_view, unsigned int>(&KV1ElementWritable<>::operator()), "child_key"_a, "n"_a)
		.def("remove_child", py::overload_cast<unsigned int>(&KV1ElementWritable<>::removeChild), "n"_a)
		.def("__delitem__", py::overload_cast<unsigned int>(&KV1ElementWritable<>::removeChild), "n"_a)
		.def("remove_child", py::overload_cast<std::string_view, int>(&KV1ElementWritable<>::removeChild), "child_key"_a, "n"_a = -1)
		.def("__delitem__", py::overload_cast<std::string_view, int>(&KV1ElementWritable<>::removeChild), "child_key"_a, "n"_a = -1)
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
		.def("add_child", [](KV1BinaryElement& self, std::string_view key, KV1BinaryValue value = {}) {
			self.addChild(key, std::move(value));
		}, "key"_a, "value"_a = KV1BinaryValue{})
		.def_prop_ro("child_count", py::overload_cast<>(&KV1BinaryElement::getChildCount, py::const_))
		.def("__len__", py::overload_cast<>(&KV1BinaryElement::getChildCount, py::const_))
		.def("get_child_count_with_key", py::overload_cast<std::string_view>(&KV1BinaryElement::getChildCount, py::const_), "child_key"_a)
		.def("__iter__", [](const KV1BinaryElement& self) {
			return py::make_iterator(py::type<KV1BinaryElement>(), "iterator", self);
		}, py::keep_alive<0, 1>())
		.def("__getitem__", py::overload_cast<unsigned int>(&KV1BinaryElement::operator[]), "n"_a)
		.def("__getitem__", py::overload_cast<std::string_view>(&KV1BinaryElement::operator[]), "child_key"_a)
		.def("get_child", py::overload_cast<std::string_view, unsigned int>(&KV1BinaryElement::operator()), "child_key"_a, "n"_a)
		.def("remove_child", py::overload_cast<unsigned int>(&KV1BinaryElement::removeChild), "n"_a)
		.def("__delitem__", py::overload_cast<unsigned int>(&KV1BinaryElement::removeChild), "n"_a)
		.def("remove_child", py::overload_cast<std::string_view, int>(&KV1BinaryElement::removeChild), "child_key"_a, "n"_a = -1)
		.def("__delitem__", py::overload_cast<std::string_view, int>(&KV1BinaryElement::removeChild), "child_key"_a, "n"_a = -1)
		.def("is_invalid", &KV1BinaryElement::isInvalid)
		.def("get_invalid", &KV1BinaryElement::getInvalid)
		.def("__bool__", &KV1BinaryElement::operator bool);

	py::class_<KV1Binary, KV1BinaryElement>(kvpp, "KV1Binary")
		.def("__init__", [](KV1Binary* self, const py::bytes& kv1Data, bool useEscapeSequences = false) {
			return new(self) KV1Binary{std::span{static_cast<const std::byte*>(kv1Data.data()), kv1Data.size()}, useEscapeSequences};
		}, "kv1_data"_a, "use_escape_sequences"_a = false)
		.def("bake", [](const KV1Binary& self) {
			const auto d = self.bake();
			return py::bytes{d.data(), d.size()};
		})
		.def("bake_to_file", py::overload_cast<const std::string&>(&KV1Binary::bake, py::const_), "kv1_path"_a)
		.def("bake_text", py::overload_cast<>(&KV1Binary::bakeText, py::const_))
		.def("bake_text_to_file", py::overload_cast<const std::string&>(&KV1Binary::bakeText, py::const_), "kv1_path"_a);
}

} // namespace gamepp
