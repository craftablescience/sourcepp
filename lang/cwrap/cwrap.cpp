#include <algorithm>
#include <filesystem>
#include <format>
#include <string>
#include <tuple>

#include <sourcepp/FS.h>
#include <sourcepp/String.h>

#include "cwrap_tokens.h"

namespace cwrap {

[[nodiscard]] constexpr uint64_t parenthesisLength(std::string_view header, uint64_t start) {
	uint64_t i;
	for (i = start; i < start + header.size(); i++) {
		if (header[i] == ')') {
			break;
		}
	}
	return i - start;
}

[[nodiscard]] auto collectTokens(std::string_view header) {
	TokenInternalStream tokens;
	for (const auto token : tokenInternalList()) {
		const auto tokenStr = tokenInternal2str(token);
		for (uint64_t pos = 0; (pos = header.find(tokenStr, pos)) != std::string::npos;) {
			pos += tokenStr.size() + 1;
			std::string data{header.data() + pos, parenthesisLength(header, pos)};
			tokens.emplace_back(pos, data, token);
		}
	}
	std::ranges::sort(tokens, [](const auto& lhs, const auto& rhs) {
		return std::get<0>(lhs) < std::get<0>(rhs);
	});
	return tokens;
}

[[nodiscard]] std::pair<std::string, std::string> parseParameter(std::string_view parameter) {
	const auto paramDetails = sourcepp::string::split(parameter, ' ');

	std::string type;
	for (int j = 0; j < paramDetails.size() - 1; j++) {
		type += paramDetails[j] + ' ';
	}
	type.pop_back();
	sourcepp::string::trim(type);

	std::string name = paramDetails.back();
	if (name.ends_with("[]")) {
		type += "[]";
		name.pop_back();
		name.pop_back();
	}
	sourcepp::string::trim(name);

	return {type, name};
}

void translateTokens(kvpp::KV1ElementWritable<>& file, const TokenInternalStream& tokens) {
	for (const auto& token : tokens) {
		auto params = sourcepp::string::split(std::get<1>(token), ',');
		for (auto& param : params) {
			sourcepp::string::trim(param);
		}
		switch (std::get<2>(token)) {
			using enum TokenInternal;
			case FUNCTION: {
				auto& fn = file.addChild(token2str(Token::FUNCTION));
				fn["type"] = params[1];
				fn["name"] = params[2];
				for (int i = 3; i < params.size(); i++) {
					auto& param = fn.addChild("param");
					const auto [type, name] = parseParameter(params[i]);
					param["type"] = type;
					param["name"] = name;
				}
				break;
			}
			case STATIC: {
				auto& fn = file.addChild(token2str(Token::FUNCTION_CLASS));
				fn["class"] = params[1];
				fn["type"] = params[2];
				fn["name"] = params[3];
				for (int i = 4; i < params.size(); i++) {
					auto& param = fn.addChild("param");
					const auto [type, name] = parseParameter(params[i]);
					param["type"] = type;
					param["name"] = name;
				}
				break;
			}
			case METHOD: {
				auto& fn = file.addChild(token2str(Token::METHOD));
				fn["class"] = params[1];
				fn["type"] = params[2];
				fn["name"] = params[3];
				auto& paramSelf = fn.addChild("param");
				paramSelf["type"] = std::format("{}_{}_handle_t", params[0], params[1]);
				paramSelf["name"] = "this";
				break;
			}
			case METHOE: {
				auto& fn = file.addChild(token2str(Token::METHOD));
				fn["class"] = params[1];
				fn["type"] = params[2];
				fn["name"] = params[3];
				auto& paramSelf = fn.addChild("param");
				paramSelf["type"] = std::format("{}_{}_handle_t", params[0], params[1]);
				paramSelf["name"] = "this";
				for (int i = 4; i < params.size(); i++) {
					auto& param = fn.addChild("param");
					const auto [type, name] = parseParameter(params[i]);
					param["type"] = type;
					param["name"] = name;
				}
				break;
			}
			case HANDLE: {
				file.addChild(token2str(Token::HANDLE)).setValue(params[1]);
				break;
			}
			case TYPEDF: {
				auto& td = file.addChild(token2str(Token::TYPEDEF));
				td["type"] = params[2];
				td["name"] = params[1];
				break;
			}
			case STRUCT_BEGIN: {
				file.addChild(token2str(Token::STRUCT));
				break;
			}
			case STRUCT_FIELD: {
				auto& st = file(token2str(Token::STRUCT), file.getChildCount(token2str(Token::STRUCT)) - 1);
				auto& fd = st.addChild("field");
				fd["type"] = params[0];
				fd["name"] = params[1];
				fd["default"] = params[2];
				break;
			}
			case STRUCT_END: {
				auto& st = file(token2str(Token::STRUCT), file.getChildCount(token2str(Token::STRUCT)) - 1);
				st["class"] = params[1];
				break;
			}
			case ENUM_BEGIN: {
				file.addChild(token2str(Token::ENUM));
				break;
			}
			case ENUM_VALUD: {
				auto& en = file(token2str(Token::ENUM), file.getChildCount(token2str(Token::ENUM)) - 1);
				auto& vl = en.addChild("value");
				vl["name"] = params[2];
				int autoValue = 0;
				for (int i = 0; i < en.getChildCount(); i++) {
					if (en[i].getValue<int>() == autoValue) {
						autoValue++;
					}
				}
				vl["value"] = autoValue;
				break;
			}
			case ENUM_VALUE: {
				auto& en = file(token2str(Token::ENUM), file.getChildCount(token2str(Token::ENUM)) - 1);
				auto& vl = en.addChild("value");
				vl["name"] = params[2];
				vl["value"] = params[3];
				break;
			}
			case ENUM_END: {
				auto& en = file(token2str(Token::ENUM), file.getChildCount(token2str(Token::ENUM)) - 1);
				en["enum"] = params[1];
				break;
			}
			case GLOBAL_EXT: {
				auto& gb = file.addChild(token2str(Token::GLOBAL));
				gb["type"] = params[1];
				gb["name"] = params[2];
				gb["value"] = sourcepp::string::trim(std::string_view{params[3]}, "\"");
				break;
			}
			case CLSVAR_EXT: {
				auto& gb = file.addChild(token2str(Token::GLOBAL_CLASS));
				gb["type"] = params[1];
				gb["class"] = params[2];
				gb["name"] = params[3];
				gb["value"] = sourcepp::string::trim(std::string_view{params[4]}, "\"");
				break;
			}
		}
	}
}

void parseHeaders(const std::filesystem::path& base, ParseResult& srcs) {
	for (const auto& entry : std::filesystem::recursive_directory_iterator{base, std::filesystem::directory_options::skip_permission_denied}) {
		if (entry.is_regular_file() && entry.path().extension() == ".h") {
			if (entry.path().filename() == "API.h") {
				continue;
			}
			const auto tokenStream = collectTokens(sourcepp::fs::readFileText(entry.path().string()));
			const auto nspace = base.filename().string();
			auto relative = std::filesystem::relative(entry.path(), base).string();
			sourcepp::string::normalizeSlashes(relative);
			translateTokens(srcs[nspace.substr(0, nspace.size() - 1)][relative.substr(0, relative.size() - 2)], tokenStream);
		}
	}
}

void cleanSrcs(ParseResult& srcs) {
	for (int i = 0; i < srcs.getChildCount(); i++) {
		for (int j = 0; j < srcs[i].getChildCount(); j++) {
			if (!srcs[i][j].getChildCount()) {
				srcs[i].removeChild(j);
				j--;
			}
		}
	}
}

} // namespace

int main(int argc, const char* argv[]) {
	if (argc < 2) {
		return 1;
	}

	cwrap::ParseResult srcs;

#ifdef SOURCEPP_PATH
	cwrap::parseHeaders(SOURCEPP_PATH, srcs);
#endif
#ifdef GAMEPP_PATH
	cwrap::parseHeaders(GAMEPP_PATH, srcs);
#endif
#ifdef STEAMPP_PATH
	cwrap::parseHeaders(STEAMPP_PATH, srcs);
#endif
#ifdef VCRYPTPP_PATH
	cwrap::parseHeaders(VCRYPTPP_PATH, srcs);
#endif
#ifdef VPKPP_PATH
	//cwrap::parseHeaders(VPKPP_PATH, srcs);
#endif
#ifdef VTFPP_PATH
	//cwrap::parseHeaders(VTFPP_PATH, srcs);
#endif
	cwrap::cleanSrcs(srcs);

	sourcepp::fs::writeFileText(argv[1], srcs.bake());
	return 0;
}
