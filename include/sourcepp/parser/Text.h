#pragma once

#include <initializer_list>
#include <string_view>
#include <tuple>
#include <unordered_map>

#include <sourcepp/String.h>

class BufferStream;

namespace sourcepp::parser::text {

constexpr std::string_view DEFAULT_SINGLE_LINE_COMMENT_START = "//";
constexpr std::string_view DEFAULT_MULTI_LINE_COMMENT_START  = "/*";
constexpr std::string_view DEFAULT_MULTI_LINE_COMMENT_END    = "*/";
constexpr std::pair        DEFAULT_QUOTES                    = {'\"', '\"'};

using EscapeSequenceMap = std::unordered_map<char, char>;

[[nodiscard]] const EscapeSequenceMap& getDefaultEscapeSequences();

[[nodiscard]] const EscapeSequenceMap& getDefaultEscapeSequencesOrNone(bool useEscapes);

class Tokenizer {
public:
	class Result {
		friend Tokenizer;

	public:
		[[nodiscard]] const std::vector<std::string_view>& getTokens() const;

		[[nodiscard]] uint64_t size() const;

		[[nodiscard]] uint64_t tell() const;

		void seek(uint64_t head_);

		[[nodiscard]] std::string_view next();

		bool nextBool(bool& value);

		template<std::integral T>
		bool nextInt(T& value) {
			return string::toInt(this->next(), value);
		}

		template<std::floating_point T>
		bool nextFloat(T& value) {
			return string::toFloat(this->next(), value);
		}

		[[nodiscard]] std::string_view peek() const;

		bool peekBool(bool& value) const;

		template<std::integral T>
		bool peekInt(T& value) const {
			return string::toInt(this->peek(), value);
		}

		template<std::floating_point T>
		bool peekFloat(T& value) const {
			return string::toFloat(this->peek(), value);
		}

		void skip();

	protected:
		Result() = default;

		std::vector<std::string_view> tokens;
		uint64_t head = 0;
	};

	Tokenizer() = default;

	Tokenizer& setSingleLineCommentStart(std::string_view singleLineCommentStart_);

	Tokenizer& setMultiLineCommentStart(std::string_view multiLineCommentStart_);

	Tokenizer& setMultiLineCommentEnd(std::string_view multiLineCommentEnd_);

	//Tokenizer& setQuotes(std::initializer_list<std::pair<char, char>> quotes_);

	Tokenizer& setEscapeSequences(const EscapeSequenceMap& escapeSequences_);

	[[nodiscard]] Result parse(std::string_view text, std::string& backingString) const;

protected:
	std::string_view singleLineCommentStart = DEFAULT_SINGLE_LINE_COMMENT_START;
	std::string_view multiLineCommentStart = DEFAULT_MULTI_LINE_COMMENT_START;
	std::string_view multiLineCommentEnd = DEFAULT_MULTI_LINE_COMMENT_END;
	// Disabled because MSVC is a garbage fire: "list initialization inside member initializer list or non-static data member initializer is not implemented"
	//std::initializer_list<std::pair<char, char>> quotes{DEFAULT_QUOTES};
	EscapeSequenceMap escapeSequences = getDefaultEscapeSequences();
};

/**
 * If a char is a newline character.
 * @param c The char.
 * @return The char is a newline character.
 */
[[nodiscard]] bool isNewLine(char c);

/**
 * If a string is entirely composed of newline characters.
 * @param str The string.
 * @return The string is entirely composed of newline characters.
 */
[[nodiscard]] bool isNewLine(std::string_view str);

/**
 * If a char is a whitespace character.
 * @param c The char.
 * @return The char is a whitespace character.
 */
[[nodiscard]] bool isWhitespace(char c);

/**
 * If a string is entirely composed of whitespace characters.
 * @param str The string.
 * @return The string is entirely composed of whitespace characters.
 */
[[nodiscard]] bool isWhitespace(std::string_view str);

/**
 * If a char is a numerical character (0-9).
 * @param c The char.
 * @return The char is a numerical character.
 */
[[nodiscard]] bool isNumber(char c);

/**
 * If a string is entirely composed of numerical characters (0-9).
 * @param str The string.
 * @return The string is entirely composed of numerical characters.
 */
[[nodiscard]] bool isNumber(std::string_view str);

/**
 * Convert special characters like \\n to escaped special characters like \\\\n
 * @param str The string to convert
 * @param escapeSequences The escapes to convert
 * @return The string without any special characters
 */
[[nodiscard]] std::string convertSpecialCharsToEscapes(std::string_view str, const EscapeSequenceMap& escapeSequences);

/**
 * Convert escaped special characters like \\\\n to special characters like \\n
 * @param str The string to convert
 * @param escapeSequences The escapes to convert
 * @return The string with special characters instead of escapes
 */
[[nodiscard]] std::string convertEscapesToSpecialChars(std::string_view str, const EscapeSequenceMap& escapeSequences);

/**
 * Eat all whitespace after the current stream position.
 * @param stream The BufferStream to modify.
 */
void eatWhitespace(BufferStream& stream);

/**
 * If a single line comment is detected, eat its contents.
 * This function does not handle the detection of single line comments!
 * @param stream The BufferStream to modify.
 */
void eatSingleLineComment(BufferStream& stream);

/**
 * If a multi line comment is detected, eat its contents.
 * This function does not handle the detection of multi line comments!
 * @param stream The BufferStream to modify.
 * @param multiLineCommentEnd The multi line comment ending to search for.
 */
void eatMultiLineComment(BufferStream& stream, std::string_view multiLineCommentEnd = DEFAULT_MULTI_LINE_COMMENT_END);

/**
 * Eat all whitespace and single line comments after the current stream position.
 * @param stream The BufferStream to modify.
 * @param singleLineCommentStart The single line comment start sequence. Leave empty to skip checking for single line comments.
 */
void eatWhitespaceAndSingleLineComments(BufferStream& stream, std::string_view singleLineCommentStart = DEFAULT_SINGLE_LINE_COMMENT_START);

/**
 * Eat all whitespace and multi line comments after the current stream position.
 * @param stream The BufferStream to modify.
 * @param multiLineCommentStart The multi line comment start sequence. Leave empty to skip checking for multi line comments.
 */
void eatWhitespaceAndMultiLineComments(BufferStream& stream, std::string_view multiLineCommentStart = DEFAULT_MULTI_LINE_COMMENT_START);

/**
 * Eat all whitespace and comments after the current stream position.
 * @param stream The BufferStream to modify.
 * @param singleLineCommentStart The single line comment start sequence. Leave empty to skip checking for single line comments.
 * @param multiLineCommentStart The multi line comment start sequence. Leave empty to skip checking for multi line comments.
 */
void eatWhitespaceAndComments(BufferStream& stream, std::string_view singleLineCommentStart = DEFAULT_SINGLE_LINE_COMMENT_START, std::string_view multiLineCommentStart = DEFAULT_MULTI_LINE_COMMENT_START);

/**
 * If the given char exists at the current position, skip over it
 * @param stream The BufferStream to modify
 * @param c The char to compare
 * @return If the char exists at the current position
 */
[[nodiscard]] bool tryToEatChar(BufferStream& stream, char c);

/**
 * Read a string starting at the current stream position.
 * @param stream The BufferStream to modify.
 * @param quotes The start and end string chars. If any processed char is in this string, the string will be treated as "quoted" and will include spaces.
 * @param escapeSequences Characters that will be escaped if a backslash is present before them. To disable escapes, pass an empty map.
 * @return The read string.
 */
[[nodiscard]] std::string readString(BufferStream& stream, std::initializer_list<std::pair<char, char>> quotes = {DEFAULT_QUOTES}, const EscapeSequenceMap& escapeSequences = getDefaultEscapeSequences());

/**
 * Read a string starting at the current stream position.
 * @param stream The BufferStream to modify.
 * @param backing The BufferStream to store the string data in.
 * @param quotes The start and end string chars. If any processed char is in this string, the string will be treated as "quoted" and will include spaces.
 * @param escapeSequences Characters that will be escaped if a backslash is present before them. To disable escapes, pass an empty map.
 * @return A view over the string written to the backing stream.
 */
std::string_view readStringToBuffer(BufferStream& stream, BufferStream& backing, std::initializer_list<std::pair<char, char>> quotes = {DEFAULT_QUOTES}, const EscapeSequenceMap& escapeSequences = getDefaultEscapeSequences());

/**
 * Read a string starting at the current stream position.
 * @param stream The BufferStream to modify.
 * @param escapeSequences Characters that will be escaped if a backslash is present before them. To disable escapes, pass an empty map.
 * @return The read string.
 */
[[nodiscard]] std::string readUnquotedString(BufferStream& stream, const EscapeSequenceMap& escapeSequences = getDefaultEscapeSequences());

/**
 * Read a string starting at the current stream position.
 * @param stream The BufferStream to modify.
 * @param backing The BufferStream to store the string data in.
 * @param escapeSequences Characters that will be escaped if a backslash is present before them. To disable escapes, pass an empty map.
 * @return A view over the string written to the backing stream.
 */
std::string_view readUnquotedStringToBuffer(BufferStream& stream, BufferStream& backing, const EscapeSequenceMap& escapeSequences = getDefaultEscapeSequences());

/**
 * Read a string starting at the current stream position.
 * @param stream The BufferStream to modify.
 * @param end The ending string chars. If any processed char is in this string, the string will terminate.
 * @param escapeSequences Characters that will be escaped if a backslash is present before them. To disable escapes, pass an empty map.
 * @return The read string.
 */
[[nodiscard]] std::string readUnquotedString(BufferStream& stream, std::string_view end, const EscapeSequenceMap& escapeSequences = getDefaultEscapeSequences());

/**
 * Read a string starting at the current stream position.
 * @param stream The BufferStream to modify.
 * @param backing The BufferStream to store the string data in.
 * @param end The ending string chars. If any processed char is in this string, the string will terminate.
 * @param escapeSequences Characters that will be escaped if a backslash is present before them. To disable escapes, pass an empty map.
 * @return A view over the string written to the backing stream.
 */
std::string_view readUnquotedStringToBuffer(BufferStream& stream, BufferStream& backing, std::string_view end, const EscapeSequenceMap& escapeSequences = getDefaultEscapeSequences());

} // namespace sourcepp::parser::text
