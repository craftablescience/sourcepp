#pragma once

#include <charconv>
#include <concepts>
#include <string>
#include <string_view>
#include <vector>

#include <sourcepp/Math.h>

namespace sourcepp::string {

[[nodiscard]] bool contains(std::string_view s, char c);

[[nodiscard]] bool iequals(std::string_view s1, std::string_view s2);

void ltrim(std::string& s);

[[nodiscard]] std::string_view ltrim(std::string_view s);

void rtrim(std::string& s);

[[nodiscard]] std::string_view rtrim(std::string_view s);

void trim(std::string& s);

[[nodiscard]] std::string_view trim(std::string_view s);

void ltrim(std::string& s, std::string_view chars);

[[nodiscard]] std::string_view ltrim(std::string_view s, std::string_view chars);

void rtrim(std::string& s, std::string_view chars);

[[nodiscard]] std::string_view rtrim(std::string_view s, std::string_view chars);

void trim(std::string& s, std::string_view chars);

[[nodiscard]] std::string_view trim(std::string_view s, std::string_view chars);

[[nodiscard]] std::vector<std::string> split(std::string_view s, char delim);

void toLower(std::string& input);

[[nodiscard]] std::string toLower(std::string_view input);

void toUpper(std::string& input);

[[nodiscard]] std::string toUpper(std::string_view input);

[[nodiscard]] std::string createRandom(uint16_t length = 32, std::string_view chars = "0123456789_abcdefghijklmnopqrstuvwxyz-ABCDEFGHIJKLMNOPQRSTUVWXYZ");

[[nodiscard]] std::string generateUUIDv4();

[[nodiscard]] std::string padNumber(int64_t number, int width, char pad = '0');

void normalizeSlashes(std::string& path, bool stripSlashPrefix = false, bool stripSlashSuffix = true);

void denormalizeSlashes(std::string& path, bool stripSlashPrefix = false, bool stripSlashSuffix = true);

std::from_chars_result toBool(std::string_view number, bool& out, int base = 10);

inline std::from_chars_result toInt(std::string_view number, std::integral auto& out, int base = 10) {
	return std::from_chars(number.data(), number.data() + number.size(), out, base);
}

inline std::from_chars_result toFloat(std::string_view number, std::floating_point auto& out) {
#ifdef __APPLE__
	// Piece of shit compiler
	out = std::stof(std::string{number});
	return {number.data(), {}};
#else
	return std::from_chars(number.data(), number.data() + number.size(), out);
#endif
}

} // namespace sourcepp::string
