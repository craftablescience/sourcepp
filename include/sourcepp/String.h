#pragma once

#include <charconv>
#include <concepts>
#include <cstddef>
#include <span>
#include <string>
#include <string_view>
#include <vector>

#include <sourcepp/Math.h>

namespace sourcepp::string {

[[nodiscard]] bool contains(std::string_view s, char c);

/// A very basic regex-like pattern checker for ASCII strings. Supports:
///   %? - matches any character
///   %w - matches a whitespace character (defined by std::isspace)
///   %a - matches a letter (A-Z, a-z)
///   %u - matches an uppercase letter (A-Z)
///   %l - matches a lowercase letter (a-z)
///   %d - matches a single digit (0-9)
///   %% - escaped percent character
[[nodiscard]] bool matches(std::string_view in, std::string_view search, bool ignoreCase = false);

[[nodiscard]] bool iequals(std::string_view s1, std::string_view s2);

void ltrim(std::string& s);

[[nodiscard]] std::string_view ltrim(std::string_view s);

void rtrim(std::string& s);

[[nodiscard]] std::string_view rtrim(std::string_view s);

void trim(std::string& s);

[[nodiscard]] std::string_view trim(std::string_view s);

void trimInternal(std::string& s);

[[nodiscard]] std::string trimInternal(std::string_view s);

void ltrim(std::string& s, std::string_view chars);

[[nodiscard]] std::string_view ltrim(std::string_view s, std::string_view chars);

void rtrim(std::string& s, std::string_view chars);

[[nodiscard]] std::string_view rtrim(std::string_view s, std::string_view chars);

void trim(std::string& s, std::string_view chars);

[[nodiscard]] std::string_view trim(std::string_view s, std::string_view chars);

void trimInternal(std::string& s, std::string_view chars);

[[nodiscard]] std::string trimInternal(std::string_view s, std::string_view chars);

[[nodiscard]] std::vector<std::string> split(std::string_view s, char delim);

void toLower(std::string& input);

[[nodiscard]] std::string toLower(std::string_view input);

void toUpper(std::string& input);

[[nodiscard]] std::string toUpper(std::string_view input);

[[nodiscard]] std::string createRandom(uint16_t length = 32, std::string_view chars = "0123456789_abcdefghijklmnopqrstuvwxyz-ABCDEFGHIJKLMNOPQRSTUVWXYZ");

[[nodiscard]] std::string generateUUIDv4();

[[nodiscard]] std::string padNumber(int64_t number, int width);

void normalizeSlashes(std::string& path, bool stripSlashPrefix = false, bool stripSlashSuffix = true);

void denormalizeSlashes(std::string& path, bool stripSlashPrefix = false, bool stripSlashSuffix = true);

bool toBool(std::string_view number, bool& out, int base = 10);

[[nodiscard]] bool isBool(std::string_view number, int base = 10);

bool toInt(std::string_view number, std::integral auto& out, int base = 10) {
	const auto [ptr, err] = std::from_chars(number.data(), number.data() + number.size(), out, base);
	return ptr == number.data() + number.size() && err == std::errc{};
}

template<std::integral T>
[[nodiscard]] bool isInt(std::string_view number, int base = 10) {
	T tmp;
	return toInt(number, tmp, base);
}

bool toFloat(std::string_view number, std::floating_point auto& out) {
#ifdef __APPLE__
	// Piece of shit compiler
	try {
		size_t pos = 0;
		out = std::stof(std::string{number}, &pos);
		return pos == number.size();
	} catch (const std::invalid_argument&) {
	} catch (const std::out_of_range&) {}
	return false;
#else
	const auto [ptr, err] = std::from_chars(number.data(), number.data() + number.size(), out);
	return ptr == number.data() + number.size() && err == std::errc{};
#endif
}

template<std::floating_point T>
[[nodiscard]] bool isFloat(std::string_view number) {
	T tmp;
	return toFloat(number, tmp);
}

[[nodiscard]] std::vector<std::byte> decodeHex(std::string_view hex);

[[nodiscard]] std::string encodeHex(std::span<const std::byte> hex);

} // namespace sourcepp::string
