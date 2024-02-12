#include <sourcepp/detail/StringUtils.h>

#include <algorithm>
#include <cctype>
#include <ranges>
#include <sstream>

using namespace sourcepp;

bool detail::contains(std::string_view s, char c) {
	return std::ranges::find(s, c) != std::ranges::end(s);
}

// https://stackoverflow.com/a/217605

void detail::ltrim(std::string& s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](char c) { return !std::isspace(c); }));
}

void detail::rtrim(std::string& s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](char c) { return !std::isspace(c); }).base(), s.end());
}

void detail::trim(std::string& s) {
    rtrim(s);
    ltrim(s);
}

void detail::ltrim(std::string& s, std::string_view chars) {
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), [chars](char c) {
		return !contains(chars, c);
	}));
}

void detail::rtrim(std::string& s, std::string_view chars) {
	s.erase(std::find_if(s.rbegin(), s.rend(), [chars](char c) {
		return !contains(chars, c);
	}).base(), s.end());
}

void detail::trim(std::string& s, std::string_view c) {
    rtrim(s, c);
    ltrim(s, c);
}

// https://stackoverflow.com/a/46931770

std::vector<std::string> detail::split(std::string_view s, char delim) {
    std::vector<std::string> result;
    std::stringstream ss(s.data());
    std::string item;
    while (std::getline(ss, item, delim)) {
        result.push_back(item);
    }
    return result;
}
