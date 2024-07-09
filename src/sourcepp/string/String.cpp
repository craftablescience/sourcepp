#include <sourcepp/string/String.h>

#include <algorithm>
#include <cctype>
#include <sstream>

using namespace sourcepp;

bool string::contains(std::string_view s, char c) {
	return std::find(s.begin(), s.end(), c) != s.end();
}

bool string::iequals(std::string_view s1, std::string_view s2) {
	return std::ranges::equal(s1, s2, [](char a, char b) { return std::tolower(a) == std::tolower(b); });
}

// https://stackoverflow.com/a/217605

void string::ltrim(std::string& s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](char c) { return !std::isspace(c); }));
}

void string::rtrim(std::string& s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](char c) { return !std::isspace(c); }).base(), s.end());
}

void string::trim(std::string& s) {
    rtrim(s);
    ltrim(s);
}

void string::ltrim(std::string& s, std::string_view chars) {
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), [chars](char c) {
		return !contains(chars, c);
	}));
}

void string::rtrim(std::string& s, std::string_view chars) {
	s.erase(std::find_if(s.rbegin(), s.rend(), [chars](char c) {
		return !contains(chars, c);
	}).base(), s.end());
}

void string::trim(std::string& s, std::string_view c) {
    rtrim(s, c);
    ltrim(s, c);
}

// https://stackoverflow.com/a/46931770

std::vector<std::string> string::split(std::string_view s, char delim) {
    std::vector<std::string> result;
    std::stringstream ss(std::string{s});
    std::string item;
    while (std::getline(ss, item, delim)) {
        result.push_back(item);
    }
    return result;
}

void string::toLower(std::string& input) {
	std::transform(input.begin(), input.end(), input.begin(), [](unsigned char c){ return std::tolower(c); });
}

void string::toUpper(std::string& input) {
	std::transform(input.begin(), input.end(), input.begin(), [](unsigned char c){ return std::toupper(c); });
}

std::string string::padNumber(int number, int width, char pad) {
	auto numStr = std::to_string(number);
	return std::string(width - std::min<std::string::size_type>(width, numStr.length()), pad) + numStr;
}

void string::normalizeSlashes(std::string& path, bool stripTerminalSlashes) {
	std::replace(path.begin(), path.end(), '\\', '/');
	if (stripTerminalSlashes) {
		if (path.starts_with('/')) {
			path = path.substr(1);
		}
		if (path.ends_with('/')) {
			path.pop_back();
		}
	}
}

void string::denormalizeSlashes(std::string& path, bool stripTerminalSlashes) {
	std::replace(path.begin(), path.end(), '/', '\\');
	if (stripTerminalSlashes) {
		if (path.starts_with('\\')) {
			path = path.substr(1);
		}
		if (path.ends_with('\\')) {
			path.pop_back();
		}
	}
}
