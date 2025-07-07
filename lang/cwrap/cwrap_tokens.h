#pragma once

#include <array>
#include <string_view>
#include <vector>

#include <kvpp/kvpp.h>

namespace cwrap {

enum class TokenInternal {
	FUNCTION,
	STATIC,
	METHOD,
	METHOE,
	HANDLE,
	TYPEDF,
	STRUCT_BEGIN,
	STRUCT_FIELD,
	STRUCT_END,
	ENUM_BEGIN,
	ENUM_VALUD,
	ENUM_VALUE,
	ENUM_END,
	GLOBAL_EXT,
	CLSVAR_EXT,
};

[[nodiscard]] consteval auto tokenInternalList() {
	using enum TokenInternal;
	return std::array{
		FUNCTION,
		STATIC,
		METHOD,
		METHOE,
		HANDLE,
		TYPEDF,
		STRUCT_BEGIN,
		STRUCT_FIELD,
		STRUCT_END,
		ENUM_BEGIN,
		ENUM_VALUD,
		ENUM_VALUE,
		ENUM_END,
		GLOBAL_EXT,
		CLSVAR_EXT,
	};
}

[[nodiscard]] constexpr std::string_view tokenInternal2str(TokenInternal token) {
	switch (token) {
		using enum TokenInternal;
		case FUNCTION:     return "SOURCEPP_FUNCTION";
		case STATIC:       return "SOURCEPP_STATIC";
		case METHOD:       return "SOURCEPP_METHOD";
		case METHOE:       return "SOURCEPP_METHOE";
		case HANDLE:       return "SOURCEPP_HANDLE";
		case TYPEDF:       return "SOURCEPP_TYPEDF";
		case STRUCT_BEGIN: return "SOURCEPP_STRUCT_BEGIN";
		case STRUCT_FIELD: return "SOURCEPP_STRUCT_FIELD";
		case STRUCT_END:   return "SOURCEPP_STRUCT_END";
		case ENUM_BEGIN:   return "SOURCEPP_ENUM_BEGIN";
		case ENUM_VALUD:   return "SOURCEPP_ENUM_VALUD";
		case ENUM_VALUE:   return "SOURCEPP_ENUM_VALUE";
		case ENUM_END:     return "SOURCEPP_ENUM_END";
		case GLOBAL_EXT:   return "SOURCEPP_GLOBAL_EXT";
		case CLSVAR_EXT:   return "SOURCEPP_CLSVAR_EXT";
	}
	return "";
}

using TokenInternalStream = std::vector<std::tuple<uint64_t, std::string, TokenInternal>>;

enum class Token {
	FUNCTION,
	FUNCTION_CLASS,
	METHOD,
	HANDLE,
	TYPEDEF,
	STRUCT,
	ENUM,
	GLOBAL,
	GLOBAL_CLASS,
};

[[nodiscard]] consteval auto tokenList() {
	using enum Token;
	return std::array{
		FUNCTION,
		FUNCTION_CLASS,
		METHOD,
		HANDLE,
		TYPEDEF,
		STRUCT,
		ENUM,
		GLOBAL,
		GLOBAL_CLASS,
	};
}

[[nodiscard]] constexpr std::string_view token2str(Token token) {
	switch (token) {
		using enum Token;
		case FUNCTION:       return "function";
		case FUNCTION_CLASS: return "function_class";
		case METHOD:         return "method";
		case HANDLE:         return "handle";
		case TYPEDEF:        return "typedef";
		case STRUCT:         return "struct";
		case ENUM:           return "enum";
		case GLOBAL:         return "global";
		case GLOBAL_CLASS:   return "global_class";
	}
	return "";
}

using ParseResult = kvpp::KV1Writer<>;

} // namespace cwrap
