#pragma once

#include <string_view>
#include <unordered_map>

class BufferStream;

namespace sourcepp::parser::text {

constexpr std::string_view DEFAULT_SINGLE_LINE_COMMENT_START = "//";
constexpr std::string_view DEFAULT_MULTI_LINE_COMMENT_START  = "/*";
constexpr std::string_view DEFAULT_MULTI_LINE_COMMENT_END    = "*/";
constexpr char DEFAULT_STRING_START = '\"';
constexpr char DEFAULT_STRING_END   = '\"';
extern const std::unordered_map<char, char> DEFAULT_ESCAPE_SEQUENCES;

/**
 * If a char is a newline character
 * @param c The char
 * @return The char is a newline character
 */
[[nodiscard]] bool isNewLine(char c);

/**
 * If a char is a whitespace character
 * @param c The char
 * @return The char is a whitespace character
 */
[[nodiscard]] bool isWhitespace(char c);

/**
 * If a char is a numerical character (0-9)
 * @param c The char
 * @return The char is a numerical character
 */
[[nodiscard]] bool isNumber(char c);

/**
 * If a single line comment is detected, eat its contents
 * @param stream The BufferStream to modify
 */
void eatSingleLineComment(BufferStream& stream);

/**
 * If a multi line comment is detected, eat its contents
 * @param stream The BufferStream to modify
 * @param multiLineCommentEnd The multi line comment ending to search for
 */
void eatMultiLineComment(BufferStream& stream, std::string_view multiLineCommentEnd = DEFAULT_MULTI_LINE_COMMENT_END);

/**
 * Eat all whitespace and single line comments after the current stream position
 * @param stream The BufferStream to modify
 * @param singleLineCommentStart The single line comment start sequence. Leave empty to skip checking for single line comments
 */
void eatWhitespaceAndSingleLineComments(BufferStream& stream, std::string_view singleLineCommentStart = DEFAULT_SINGLE_LINE_COMMENT_START);

/**
 * Eat all whitespace and multi line comments after the current stream position
 * @param stream The BufferStream to modify
 * @param multiLineCommentStart The multi line comment start sequence. Leave empty to skip checking for multi line comments
 */
void eatWhitespaceAndMultiLineComments(BufferStream& stream, std::string_view multiLineCommentStart = DEFAULT_MULTI_LINE_COMMENT_START);

/**
 * Eat all whitespace and comments after the current stream position
 * @param stream The BufferStream to modify
 * @param singleLineCommentStart The single line comment start sequence. Leave empty to skip checking for single line comments
 * @param multiLineCommentStart The multi line comment start sequence. Leave empty to skip checking for multi line comments
 */
void eatWhitespaceAndComments(BufferStream& stream, std::string_view singleLineCommentStart = DEFAULT_SINGLE_LINE_COMMENT_START, std::string_view multiLineCommentStart = DEFAULT_MULTI_LINE_COMMENT_START);

/**
 * Read a string starting at the current stream position
 * @param stream The BufferStream to modify
 * @param backing The BufferStream to store the string data in
 * @param start The starting string char
 * @param end The ending string char
 * @param escapeSequences Characters that will be escaped if a backslash is present before them. To disable escapes, pass an empty map
 * @return A view over the string written to the backing stream
 */
[[nodiscard]] std::string_view readStringToBuffer(BufferStream& stream, BufferStream& backing, char start = DEFAULT_STRING_START, char end = DEFAULT_STRING_END, const std::unordered_map<char, char>& escapeSequences = DEFAULT_ESCAPE_SEQUENCES);

/**
 * Read a string starting at the current stream position. Uses the default string start/end characters
 * @param stream The BufferStream to modify
 * @param backing The BufferStream to store the string data in
 * @param escapeSequences Characters that will be escaped if a backslash is present before them. To disable escapes, pass an empty map
 * @return A view over the string written to the backing stream
 */
[[nodiscard]] std::string_view readStringToBuffer(BufferStream& stream, BufferStream& backing, const std::unordered_map<char, char>& escapeSequences = DEFAULT_ESCAPE_SEQUENCES);

} // namespace sourcepp::parser::text
