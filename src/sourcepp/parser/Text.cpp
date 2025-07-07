#include <sourcepp/parser/Text.h>

#include <algorithm>
#include <cctype>

#include <BufferStream.h>

using namespace sourcepp;

const parser::text::EscapeSequenceMap& parser::text::getDefaultEscapeSequences() {
	static const EscapeSequenceMap defaultEscapeSequences{
			{'\'', '\''},
			{'\"', '\"'},
			{'?',   '?'},
			{'\\', '\\'},
			{'a',  '\a'},
			{'b',  '\b'},
			{'f',  '\f'},
			{'n',  '\n'},
			{'r',  '\r'},
			{'t',  '\t'},
			{'v',  '\v'},
	};
	return defaultEscapeSequences;
}

const parser::text::EscapeSequenceMap& parser::text::getDefaultEscapeSequencesOrNone(bool useEscapes) {
	static const EscapeSequenceMap noEscapeSequences{};
	return useEscapes ? getDefaultEscapeSequences() : noEscapeSequences;
}

bool parser::text::isNewLine(char c) {
	return c == '\n' || c == '\r';
}

bool parser::text::isNewLine(std::string_view str) {
	return std::ranges::all_of(str, [](char c) { return isNewLine(c); });
}

bool parser::text::isWhitespace(char c) {
	return c == ' ' || c == '\a' || c == '\f' || c == '\t' || c == '\v' || isNewLine(c);
}

bool parser::text::isWhitespace(std::string_view str) {
	return std::ranges::all_of(str, [](char c) { return isWhitespace(c); });
}

bool parser::text::isNumber(char c) {
	return std::isdigit(c);
}

bool parser::text::isNumber(std::string_view str) {
	return std::ranges::all_of(str, [](char c) { return isNumber(c); });
}

std::string parser::text::convertSpecialCharsToEscapes(std::string_view str, const EscapeSequenceMap& escapeSequences) {
	// Reverse escape sequences map (assume that it's bidirectional)
	EscapeSequenceMap specialSequences;
	for (const auto& [normal, special] : escapeSequences) {
		specialSequences[special] = normal;
	}

	std::string out;
	for (int i = 0; i < str.length(); i++) {
		if (specialSequences.contains(str[i])) {
			out += '\\';
			out += specialSequences[str[i]];
		} else {
			out += str[i];
		}
	}
	return out;
}

std::string parser::text::convertEscapesToSpecialChars(std::string_view str, const EscapeSequenceMap& escapeSequences) {
	std::string out;
	for (int i = 0; i < str.length(); i++) {
		if (!escapeSequences.empty() && str[i] == '\\' && i < str.length() - 1) {
			auto n = str[i + 1];
			if (escapeSequences.contains(n)) {
				out += escapeSequences.at(n);
			} else {
				out += str[i];
				out += str[i + 1];
			}
		} else {
			out += str[i];
		}
	}
	return out;
}

void parser::text::eatWhitespace(BufferStream& stream) {
	while (isWhitespace(stream.read<char>())) {}
	stream.seek(-1, std::ios::cur);
}

void parser::text::eatSingleLineComment(BufferStream& stream) {
	while (!isNewLine(stream.read<char>())) {}
}

void parser::text::eatMultiLineComment(BufferStream& stream, std::string_view multiLineCommentEnd) {
	while (!std::ranges::equal(stream.read_span<char>(multiLineCommentEnd.length()), multiLineCommentEnd)) {
		stream.seek(-static_cast<int64_t>(multiLineCommentEnd.length() - 1), std::ios::cur);
	}
}

void parser::text::eatWhitespaceAndSingleLineComments(BufferStream& stream, std::string_view singleLineCommentStart) {
	return eatWhitespaceAndComments(stream, singleLineCommentStart, "");
}

void parser::text::eatWhitespaceAndMultiLineComments(BufferStream& stream, std::string_view multiLineCommentStart) {
	return eatWhitespaceAndComments(stream, "", multiLineCommentStart);
}

// NOLINTNEXTLINE(*-no-recursion)
void parser::text::eatWhitespaceAndComments(BufferStream& stream, std::string_view singleLineCommentStart, std::string_view multiLineCommentStart) {
	eatWhitespace(stream);

	if (!singleLineCommentStart.empty()) {
		if (std::ranges::equal(stream.read_span<char>(singleLineCommentStart.length()), singleLineCommentStart)) {
			eatSingleLineComment(stream);
			eatWhitespaceAndComments(stream, singleLineCommentStart, multiLineCommentStart);
			return;
		}
		stream.seek(-static_cast<int64_t>(singleLineCommentStart.length()), std::ios::cur);
	}

	if (!multiLineCommentStart.empty()) {
		if (std::ranges::equal(stream.read_span<char>(multiLineCommentStart.length()), multiLineCommentStart)) {
			eatMultiLineComment(stream);
			eatWhitespaceAndComments(stream, singleLineCommentStart, multiLineCommentStart);
			return;
		}
		stream.seek(-static_cast<int64_t>(multiLineCommentStart.length()), std::ios::cur);
	}
}

bool parser::text::tryToEatChar(BufferStream& stream, char c) {
	if (stream.peek<char>() != c) {
		return false;
	}
	stream.skip();
	return true;
}

std::string_view parser::text::readStringToBuffer(BufferStream& stream, BufferStream& backing, std::string_view start, std::string_view end, const EscapeSequenceMap& escapeSequences) {
	const auto startSpan = backing.tell();

	bool stopAtWhitespace = true;
	char c = stream.read<char>();
	if (start.find(c) != std::string_view::npos) {
		stopAtWhitespace = false;
	} else {
		backing << c;
	}

	for (c = stream.read<char>(); (stopAtWhitespace && !isWhitespace(c)) || (!stopAtWhitespace && end.find(c) == std::string_view::npos); c = stream.read<char>()) {
		if (!escapeSequences.empty() && c == '\\') {
			auto n = stream.read<char>();
			if (stopAtWhitespace && isWhitespace(n)) {
				break;
			}
			if (escapeSequences.contains(n)) {
				backing << escapeSequences.at(n);
			} else if (!stopAtWhitespace && end.find(n) != std::string_view::npos) {
				break;
			} else {
				backing << c << n;
			}
		} else {
			backing << c;
		}
	}

	backing << '\0';
	return {reinterpret_cast<const char*>(backing.data()) + startSpan, backing.tell() - 1 - startSpan};
}

std::string_view parser::text::readUnquotedStringToBuffer(BufferStream& stream, BufferStream& backing, const EscapeSequenceMap& escapeSequences) {
	return readStringToBuffer(stream, backing, "", "", escapeSequences);
}

std::string_view parser::text::readUnquotedStringToBuffer(BufferStream& stream, BufferStream& backing, std::string_view end, const EscapeSequenceMap& escapeSequences) {
	const auto startSpan = backing.tell();

	for (char c = stream.read<char>(); !isWhitespace(c) && end.find(c) == std::string_view::npos; c = stream.read<char>()) {
		if (!escapeSequences.empty() && c == '\\') {
			auto n = stream.read<char>();
			if (escapeSequences.contains(n)) {
				backing << escapeSequences.at(n);
			} else if (isWhitespace(n) || end.find(n) != std::string_view::npos) {
				break;
			} else {
				backing << c << n;
			}
		} else {
			backing << c;
		}
	}

	backing << '\0';
	return {reinterpret_cast<const char*>(backing.data()) + startSpan, backing.tell() - 1 - startSpan};
}
