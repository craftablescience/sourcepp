#include <sstream>
#include <unordered_set>

#include <kvpp/kvpp.h>
#include <sourcepp/FS.h>

using namespace kvpp;
using namespace sourcepp;

namespace {

constexpr std::string_view NAMESPACE{"namespace"};
constexpr std::string_view CONSTANT{"constant"};
constexpr std::string_view FUNCTION{"function"};
constexpr std::string_view FUNCTION_PARAMETER{"parameter"};
constexpr std::string_view FUNCTION_PARAMETER_DEFAULT{"default"};

[[nodiscard]] int c(std::string_view target, const KV1<>& defs, std::string_view incDir, std::string_view srcDir) {
	// Conversions from C++ -> C return types
	struct ConversionTypeInfo {
		std::string_view cppType;
		std::string_view cppInnerElement;
		std::string_view cTypeEquivalent;
		std::string_view cConversionFunctionSuffix;
		std::string_view cFreeFunction;
	};
	static constexpr auto getTypeConverterFor = [](std::string_view cppType) -> const ConversionTypeInfo& {
		static constexpr std::array returnTypeConverters{
			ConversionTypeInfo{"char"},
			ConversionTypeInfo{"bool",                         "",                "int",             },
			ConversionTypeInfo{"std::byte",                    "",                "unsigned char"    },
			ConversionTypeInfo{"uint8_t"},
			ConversionTypeInfo{"std::span<std::byte>",         "std::byte",       "sourcepp_buffer_t", "Buffer", "sourcepp_buffer_free"},
			ConversionTypeInfo{"std::span<const std::byte>",   "const std::byte", "sourcepp_buffer_t", "Buffer", "sourcepp_buffer_free"},
			ConversionTypeInfo{"std::string_view",             "const char",      "sourcepp_string_t", "String", "sourcepp_string_free"},
			ConversionTypeInfo{"std::vector<std::byte>",       "std::byte",       "sourcepp_buffer_t", "Buffer", "sourcepp_buffer_free"},
			ConversionTypeInfo{"std::vector<const std::byte>", "const std::byte", "sourcepp_buffer_t", "Buffer", "sourcepp_buffer_free"},
		};
		if (cppType.starts_with("const ")) {
			cppType = cppType.substr(6);
		}
		if (
			const auto it = std::ranges::find_if(returnTypeConverters, [cppType](const ConversionTypeInfo& typeInfo) { return typeInfo.cppType == cppType; });
			it != std::end(returnTypeConverters)
		) {
			return *it;
		}
		throw std::runtime_error{"Cannot find conversion function for C++ type " + std::string{cppType}};
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
			if (const auto typeInfo = getTypeConverterFor(type); !typeInfo.cppInnerElement.empty()) {
				if (typeInfo.cppInnerElement == "char" || typeInfo.cppInnerElement.ends_with(" char")) {
					return std::string{typeInfo.cppInnerElement.starts_with("const ") ? "const " : ""} + "char* " + this->name + defaultSuffix;
				}
				// Intentionally not using default value here
				return (typeInfo.cppInnerElement.starts_with("const ") ? "const " : "") + std::string{getTypeConverterFor(typeInfo.cppInnerElement).cTypeEquivalent} + "* " + this->name + ", uint64_t " + this->name + std::string{PARAMETER_SIZE_SUFFIX};
			}
			return this->type + ' ' + this->name + defaultSuffix;
		}

		// Accessing the parameter from C++
		[[nodiscard]] std::string access() const {
			if (const auto typeInfo = getTypeConverterFor(type); !typeInfo.cppInnerElement.empty() && !(typeInfo.cppInnerElement == "char" || typeInfo.cppInnerElement.ends_with(" char"))) {
				return this->type + "{reinterpret_cast<" + std::string{typeInfo.cppInnerElement} + "*>(" + this->name + "), reinterpret_cast<" + std::string{typeInfo.cppInnerElement} + "*>(" + this->name + " + " + this->name + std::string{PARAMETER_SIZE_SUFFIX} + ")}";
			}
			return name;
		}
	};

	// Namespaces are appended to each identifier
	std::vector<std::string> namespacesStore;

	// Add a constant or function
	const auto addFunction = [&h, &cpp, &namespacesStore](std::string_view name, std::string_view type, bool isConstant, std::span<ParameterInfo> parameters) {
		const auto typeInfo = getTypeConverterFor(type);

		// Notify consumer of manual free requirement
		if (!typeInfo.cFreeFunction.empty()) {
			h << "// REQUIRES MANUAL FREE: " << typeInfo.cFreeFunction << '\n';
		}

		// Expose to consumer
		h << "SOURCEPP_API ";
		cpp << "SOURCEPP_API ";

		// Return type
		if (!typeInfo.cTypeEquivalent.empty()) {
			h << typeInfo.cTypeEquivalent << ' ';
			cpp << typeInfo.cTypeEquivalent << ' ';
		} else {
			h << type << ' ';
			cpp << type << ' ';
		}

		// Namespaced identifier
		for (const auto& n : namespacesStore) {
			h << string::toLower(n) << '_';
			cpp << string::toLower(n) << '_';
		}
		h << string::toLower(name) << '(';
		cpp << string::toLower(name) << '(';

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

		// Implementation
		cpp << "\treturn ";
		if (!typeInfo.cConversionFunctionSuffix.empty()) {
			cpp << "sourceppc::c::to" << typeInfo.cConversionFunctionSuffix << '(';
		}
		for (const auto& n : namespacesStore) {
			cpp << n << "::";
		}
		cpp << name;
		if (!isConstant) {
			cpp << '(';
			for (int i = 0; i < parameters.size(); i++) {
				cpp << parameters[i].access();
				if (i < parameters.size() - 1) {
					cpp << ", ";
				}
			}
			cpp << ')';
		}
		if (!typeInfo.cConversionFunctionSuffix.empty()) {
			cpp << ')';
		}
		cpp << ";\n}\n\n";
	};

	std::function<void(const KV1ElementReadable<>&, std::vector<std::string>&)> readDefs;
	readDefs = [&addFunction, &readDefs](const KV1ElementReadable<>& defs_, std::vector<std::string>& namespaces) {
		for (const auto& kv : defs_) {
			if (kv.getKey() == NAMESPACE) {
				namespaces.emplace_back(kv.getValue());
				readDefs(kv, namespaces);
				namespaces.pop_back();
			} else if (kv.getKey() == CONSTANT) {
				// Constants are accessible from dedicated functions, because making them defines
				// would necessitate another generation step and making them globals is icky
				addFunction(kv.getValue(), kv.getConditional(), true, {});
			} else if (kv.getKey() == FUNCTION) {
				std::vector<ParameterInfo> parameters;
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
				addFunction(kv.getValue(), kv.getConditional(), false, parameters);
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
	const KV1 defs{fs::readFileText(argv[3])};

	if (language == "C") {
		if (argc < 6) {
			return 1;
		}
		return c(target, defs, argv[4], argv[5]);
	}
	return 2;
}
