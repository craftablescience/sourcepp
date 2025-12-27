#include <cctype>
#include <deque>
#include <format>
#include <sstream>
#include <tuple>

#include <iostream>

#include <kvpp/kvpp.h>
#include <sourcepp/FS.h>

using namespace kvpp;
using namespace sourcepp;

namespace {

constexpr std::string_view NAMESPACE                 {"namespace"};
constexpr std::string_view TYPE                        {"type"};
constexpr std::string_view CONSTANT                    {"constant"};
constexpr std::string_view FUNCTION                    {"function"};
constexpr std::string_view FUNCTION_PARAMETER            {"parameter"};
constexpr std::string_view FUNCTION_PARAMETER_DEFAULT      {"default"};
constexpr std::string_view ENUM                        {"enum"};
constexpr std::string_view CLASS                       {"class"};
constexpr std::string_view CLASS_CONSTRUCTOR             {"constructor"};
constexpr std::string_view CLASS_METHOD                  {"method"};
constexpr std::string_view CLASS_PROP_METHOD             {"prop_method"};
constexpr std::string_view CLASS_PROP_METHOD_GET           {"get"};
constexpr std::string_view CLASS_PROP_METHOD_SET           {"set"};

[[nodiscard]] int c(std::string_view target, const KV1<>& defs, std::string_view incDir, std::string_view srcDir) {
	// C containers
	struct CContainerTypeInfo {
		std::string_view cType;
		std::string_view cConversionFunctionSuffix; // sourcepp::c::to<Suffix>
		std::string_view cFreeFunction;
	};
	static constexpr std::array C_CONVERSION_TYPE_INFO{
		CContainerTypeInfo{"sourcepp_buffer_t",       "Buffer",      "sourcepp_buffer_free"},
		CContainerTypeInfo{"sourcepp_string_t",       "String",      "sourcepp_string_free"},
		CContainerTypeInfo{"sourcepp_string_array_t", "StringArray", "sourcepp_string_array_free"},
	};

	// Conversions from C++ <-> C types
	struct ConversionTypeInfo {
		std::string_view cppType;
		std::string_view cppInnerType;
		std::variant<std::string_view, const CContainerTypeInfo*> cType;

		[[nodiscard]] std::string_view getCType() const {
			if (std::holds_alternative<std::string_view>(cType)) {
				return std::get<std::string_view>(cType);
			}
			return std::get<const CContainerTypeInfo*>(cType)->cType;
		}
	};
	static std::vector TYPE_CONVERTERS{
		ConversionTypeInfo{"void" ,                                  "",                            "void"},
		ConversionTypeInfo{"void*",                                  "",                            "void*"},
		ConversionTypeInfo{"char",                                   "",                            "char"},
		ConversionTypeInfo{"bool",                                   "",                            "int"},
		ConversionTypeInfo{"std::byte",                              "",                            "unsigned char"},
		ConversionTypeInfo{"uint8_t",                                "",                            "unsigned char"},
		ConversionTypeInfo{"int8_t",                                 "",                            "signed char"},
		ConversionTypeInfo{"uint16_t",                               "",                            "unsigned short"},
		ConversionTypeInfo{"int16_t",                                "",                            "short"},
		ConversionTypeInfo{"uint32_t",                               "",                            "unsigned int"},
		ConversionTypeInfo{"int32_t",                                "",                            "int"},
		ConversionTypeInfo{"uint64_t",                               "",                            "unsigned long long"},
		ConversionTypeInfo{"int64_t",                                "",                            "long long"},
		ConversionTypeInfo{"std::span<std::byte>",                   "std::byte",                   &C_CONVERSION_TYPE_INFO[0]},
		ConversionTypeInfo{"std::span<const std::byte>",             "const std::byte",             &C_CONVERSION_TYPE_INFO[0]},
		ConversionTypeInfo{"std::span<const std::filesystem::path>", "const std::filesystem::path", &C_CONVERSION_TYPE_INFO[2]},
		ConversionTypeInfo{"std::string",                            "char",                        &C_CONVERSION_TYPE_INFO[1]},
		ConversionTypeInfo{"std::string_view",                       "const char",                  &C_CONVERSION_TYPE_INFO[1]},
		ConversionTypeInfo{"std::filesystem::path",                  "",                            &C_CONVERSION_TYPE_INFO[1]},
		ConversionTypeInfo{"std::vector<std::byte>",                 "std::byte",                   &C_CONVERSION_TYPE_INFO[0]},
		ConversionTypeInfo{"std::vector<const std::byte>",           "const std::byte",             &C_CONVERSION_TYPE_INFO[0]},
		ConversionTypeInfo{"std::vector<uint32_t>",                  "uint32_t",                    &C_CONVERSION_TYPE_INFO[0]},
	};
	static constexpr auto getTypeConverterFor = [](std::string_view cppType) -> const ConversionTypeInfo& {
		if (cppType.starts_with("const ")) {
			cppType = cppType.substr(6);
		}
		if (cppType.ends_with("&")) {
			cppType = cppType.substr(0, cppType.size() - 1);
		}
		if (
			const auto it = std::ranges::find_if(TYPE_CONVERTERS, [cppType](const ConversionTypeInfo& typeInfo) {
				return typeInfo.cppType == cppType;
			});
			it != std::end(TYPE_CONVERTERS)
		) {
			return *it;
		}
		throw std::runtime_error{std::format("Cannot find conversion function for C++ type {}", cppType).c_str()};
	};

	static constexpr auto camelCaseToSnakeCase = [](std::string_view str) {
		std::string out;
		for (int i = 0; i < str.size(); i++) {
			if (i > 0 && ((std::isupper(str[i]) && std::islower(str[i - 1])) || (!std::isdigit(str[i]) && std::isdigit(str[i - 1])))) {
				out += '_';
			}
			out += static_cast<char>(std::tolower(str[i]));
		}
		return out;
	};

	std::ostringstream h;
	std::ostringstream cpp;

	// Header: add include guard and includes
	h
		<< "#pragma once\n\n"
		<< "#include <sourceppc/sourcepp.h>\n\n";

	// Implementation: add header includes
	const auto targetC = std::filesystem::path{incDir}.filename().string();
	cpp
		<< "#include <" << targetC << '/' << target << ".h>\n\n"
		<< "#include <sourceppc/Convert.hpp>\n\n"
		<< "#include <" << target << '/' << target << ".h>\n\n";

	// Helper conversion of parameters to C syntax
	static constexpr std::string_view PARAMETER_SIZE_SUFFIX{"Size"};
	struct ParameterInfo {
		std::string name;
		std::string type;
		std::string defaultValue;

		// The signature in the C header
		[[nodiscard]] std::string signature() const {
			const std::string defaultSuffix{!this->defaultValue.empty() ? " /*= " + this->defaultValue + "*/" : ""};
			const auto typeInfo = getTypeConverterFor(type);
			if (std::holds_alternative<const CContainerTypeInfo*>(typeInfo.cType)) {
				const auto converter = std::get<const CContainerTypeInfo*>(typeInfo.cType);
				if (converter == &C_CONVERSION_TYPE_INFO[0]) {
					// Intentionally not using default value here
					return (typeInfo.cppInnerType.starts_with("const ") ? "const " : "") + std::string{getTypeConverterFor(typeInfo.cppInnerType).getCType()} + "* " + this->name + ", unsigned long long " + this->name + std::string{PARAMETER_SIZE_SUFFIX};
				}
				if (converter == &C_CONVERSION_TYPE_INFO[1]) {
					return std::string{typeInfo.cppInnerType.starts_with("const ") ? "const " : ""} + "char* " + this->name + defaultSuffix;
				}
				if (converter == &C_CONVERSION_TYPE_INFO[2]) {
					throw std::runtime_error{"todo: implement this"};
				}
				throw std::runtime_error{std::format("Unchecked conversion type info for {}", converter->cType).c_str()};
			}
			return std::format("{} {}{}", getTypeConverterFor(this->type).getCType(), this->name, defaultSuffix);
		}

		// Accessing the parameter from C++
		[[nodiscard]] std::string access() const {
			const auto typeInfo = getTypeConverterFor(type);
			if (std::holds_alternative<const CContainerTypeInfo*>(typeInfo.cType)) {
				const auto converter = std::get<const CContainerTypeInfo*>(typeInfo.cType);
				if (converter == &C_CONVERSION_TYPE_INFO[0]) {
					return this->type + "{reinterpret_cast<" + std::string{typeInfo.cppInnerType} + "*>(" + this->name + "), reinterpret_cast<" + std::string{typeInfo.cppInnerType} + "*>(" + this->name + " + " + this->name + std::string{PARAMETER_SIZE_SUFFIX} + ")}";
				}
				if (converter == &C_CONVERSION_TYPE_INFO[1] || converter == &C_CONVERSION_TYPE_INFO[2]) {
					return name;
				}
				throw std::runtime_error{std::format("Unchecked conversion type info for {}", converter->cType).c_str()};
			}
			return name;
		}
	};

	// Namespaces are appended to each identifier
	std::deque<std::string> namespacesStore;

	// Apply current namespaces to an identifier
	const auto applyNamespacesTo = [&namespacesStore](std::string_view cIdentifier) {
		std::string identifier;
		for (const auto& n : namespacesStore) {
			identifier += string::toLower(n);
			identifier += '_';
		}
		return identifier + camelCaseToSnakeCase(cIdentifier);
	};

	// Add a typedef
	const auto addTypeDef = [&h, &applyNamespacesTo](std::string_view cppAlias, std::string_view cppType) {
		static std::deque<std::string> cAliases;
		cAliases.push_back(applyNamespacesTo(std::string{cppAlias} + "_t"));
		const auto& cAlias = cAliases.back();
		TYPE_CONVERTERS.push_back({cppAlias, "", cAlias});
		// hack
		cAliases.push_back("std::vector<" + std::string{cppAlias} + '>');
		TYPE_CONVERTERS.push_back({cAliases.back(), cAlias, &C_CONVERSION_TYPE_INFO[0]});
		h << "typedef " << getTypeConverterFor(cppType).getCType() << ' ' << cAlias << ";\n\n";
	};

	// Add a constant or function
	static constexpr std::string_view CLASS_CTOR_NAME = "new";
	static constexpr std::string_view CLASS_DTOR_NAME = "free";
	const auto addFunction = [&h, &cpp, &namespacesStore, &applyNamespacesTo](std::string_view name, std::string_view type, std::string_view clasz, bool isConstant, std::deque<ParameterInfo>& parameters) {
		const auto typeInfo = getTypeConverterFor(type);
		const CContainerTypeInfo* cTypeInfo = std::holds_alternative<const CContainerTypeInfo*>(typeInfo.cType) ? std::get<const CContainerTypeInfo*>(typeInfo.cType) : nullptr;

		// Notify consumer of manual free requirement
		if (cTypeInfo) {
			h << "// REQUIRES MANUAL FREE: " << cTypeInfo->cFreeFunction << '\n';
		}

		// Expose to consumer
		h << "SOURCEPP_API ";
		cpp << "SOURCEPP_API ";

		// Add class to namespace list if appropriate, remove it later
		if (!clasz.empty()) {
			namespacesStore.emplace_back(clasz);
		}

		// Return type
		h << typeInfo.getCType() << ' ';
		cpp << typeInfo.getCType() << ' ';

		// Namespaced identifier
		h << applyNamespacesTo(name) << '(';
		cpp << applyNamespacesTo(name) << '(';

		const auto writeNamespacedCPPIdentifier = [&cpp, &namespacesStore] {
			for (int i = 0; i < namespacesStore.size() - 1; i++) {
				cpp << namespacesStore[i] << "::";
			}
			cpp << namespacesStore.back();
		};

		// Parameters
		for (int i = 0; i < parameters.size(); i++) {
			h << parameters[i].signature();
			cpp << parameters[i].signature();
			if (i < parameters.size() - 1) {
				h << ", ";
				cpp << ", ";
			}
		}
		h << ");\n\n";
		cpp << ") {\n";

		const auto writeParameterAccess = [&cpp, &parameters] {
			for (int i = 0; i < parameters.size(); i++) {
				cpp << parameters[i].access();
				if (i < parameters.size() - 1) {
					cpp << ", ";
				}
			}
		};

		// Implementation
		cpp << "\treturn ";

		if (!clasz.empty()) {
			if (name == CLASS_CTOR_NAME) {
				cpp << "new ";
				writeNamespacedCPPIdentifier();
				cpp << '{';
				writeParameterAccess();
				cpp << "};\n}\n\n";
				goto cleanup;
			}
			if (name == CLASS_DTOR_NAME) {
				cpp << "delete static_cast<";
				writeNamespacedCPPIdentifier();
				cpp << "*>(self);\n}\n\n";
				goto cleanup;
			}
		}

		if (cTypeInfo && !cTypeInfo->cConversionFunctionSuffix.empty()) {
			cpp << "sourceppc::c::to" << cTypeInfo->cConversionFunctionSuffix << '(';
		}
		if (!clasz.empty()) {
			cpp << "static_cast<";
		}
		writeNamespacedCPPIdentifier();
		if (!clasz.empty()) {
			cpp << "*>(self)->";
			parameters.pop_front();
		} else {
			cpp << "::";
		}
		cpp << name;
		if (!isConstant) {
			cpp << '(';
			writeParameterAccess();
			cpp << ')';
		}
		if (cTypeInfo && !cTypeInfo->cConversionFunctionSuffix.empty()) {
			cpp << ')';
		}
		cpp << ";\n}\n\n";

		cleanup:
		// Remove class "namespace"
		if (!clasz.empty()) {
			namespacesStore.pop_back();
		}
	};

	std::function<void(const KV1ElementReadable<>&, std::deque<std::string>&)> readDefs;
	readDefs = [&addTypeDef, &addFunction, &readDefs](const KV1ElementReadable<>& defs_, std::deque<std::string>& namespaces) {
		static constexpr auto collectParameters = [](const KV1ElementReadable<>& kv) {
			std::deque<ParameterInfo> parameters;
			for (const auto& p : kv) {
				if (p.getKey() == FUNCTION_PARAMETER) {
					auto& [parameterName, parameterType, parameterDefaultValue] = parameters.emplace_back();
					parameterName = p.getValue();
					parameterType = p.getConditional();
					for (const auto& d : p) {
						if (d.getKey() == FUNCTION_PARAMETER_DEFAULT) {
							parameterDefaultValue = d.getValue();
							break;
						}
					}
				}
			}
			return parameters;
		};
		for (const auto& kv : defs_) {
			if (kv.getKey() == NAMESPACE) {
				namespaces.emplace_back(kv.getValue());
				readDefs(kv, namespaces);
				namespaces.pop_back();
			} else if (kv.getKey() == TYPE) {
				addTypeDef(kv.getValue(), kv.getConditional());
			} else if (kv.getKey() == CONSTANT) {
				// Constants are accessible from dedicated functions, because making them defines
				// would necessitate another generation step and making them globals is icky
				std::deque<ParameterInfo> parameters;
				addFunction(kv.getValue(), kv.getConditional(), "", true, parameters);
			} else if (kv.getKey() == FUNCTION) {
				auto parameters = collectParameters(kv);
				addFunction(kv.getValue(), kv.getConditional(), "", false, parameters);
			} else if (kv.getKey() == CLASS) {
				const auto cppClassType = std::string{kv.getValue()} + "Handle";
				addTypeDef(cppClassType, "void*");
				for (const auto& m : kv) {
					if (m.getKey() == CLASS_CONSTRUCTOR) {
						auto parameters = collectParameters(m);
						addFunction(CLASS_CTOR_NAME, cppClassType, kv.getValue(), false, parameters);
					} else if (m.getKey() == CLASS_METHOD) {
						auto parameters = collectParameters(m);
						parameters.push_front({
							.name = "self",
							.type = cppClassType,
						});
						addFunction(m.getValue(), m.getConditional(), kv.getValue(), false, parameters);
					} else if (m.getKey() == CLASS_PROP_METHOD) {
						if (m.hasChild(CLASS_PROP_METHOD_GET)) {
							auto parameters = collectParameters(m[CLASS_PROP_METHOD_GET]);
							parameters.push_front({
								.name = "self",
								.type = cppClassType,
							});
							addFunction(m[CLASS_PROP_METHOD_GET].getValue(), m.getConditional(), kv.getValue(), false, parameters);
						}
						if (m.hasChild(CLASS_PROP_METHOD_SET)) {
							auto parameters = collectParameters(m[CLASS_PROP_METHOD_SET]);
							parameters.push_front({
								.name = "value",
								.type = std::string{m.getConditional()},
							});
							parameters.push_front({
								.name = "self",
								.type = cppClassType,
							});
							addFunction(m[CLASS_PROP_METHOD_SET].getValue(), m.getConditional(), kv.getValue(), false, parameters);
						}
					} /*else if (m.getKey() == FUNCTION) {
						auto parameters = collectParameters(m);
						addFunction(m.getValue(), m.getConditional(), kv.getValue(), false, parameters);
					}*/
				}
				std::deque parameters{
					ParameterInfo{
						.name = "self",
						.type = cppClassType,
					},
				};
				addFunction(CLASS_DTOR_NAME, "void", kv.getValue(), false, parameters);
			}
		}
	};
	readDefs(defs, namespacesStore);

	return !(
		fs::writeFileText((std::filesystem::path{incDir} / target).replace_extension(".h"), h.view().substr(0, h.view().length() - 1)) &&
		fs::writeFileText((std::filesystem::path{srcDir} / target).replace_extension(".cpp"), cpp.view().substr(0, cpp.view().length() - 1))
	);
}

} // namespace

int main(int argc, const char* argv[]) {
	if (argc < 3) {
		return 1;
	}
	const std::string_view language{argv[1]};
	const std::string target{argv[2]};
	const KV1 defs{fs::readFileText(argv[3]), true};

	if (language == "C") {
		if (argc < 6) {
			return 1;
		}
		return c(target, defs, argv[4], argv[5]);
	}
	return 2;
}
