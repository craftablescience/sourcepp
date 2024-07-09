#pragma once

#include <string>
#include <string_view>
#include <vector>

namespace sourcepp::string {

[[nodiscard]] bool contains(std::string_view s, char c);

[[nodiscard]] bool iequals(std::string_view s1, std::string_view s2);

void ltrim(std::string& s);

void rtrim(std::string& s);

void trim(std::string& s);

void ltrim(std::string& s, std::string_view c);

void rtrim(std::string& s, std::string_view c);

void trim(std::string& s, std::string_view c);

[[nodiscard]] std::vector<std::string> split(std::string_view s, char delim);

void toLower(std::string& input);

void toUpper(std::string& input);

[[nodiscard]] std::string padNumber(int number, int width, char pad = '0');

void normalizeSlashes(std::string& path, bool stripTerminalSlashes = true);

void denormalizeSlashes(std::string& path, bool stripTerminalSlashes = true);

} // namespace sourcepp::string
