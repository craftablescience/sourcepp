#include <sourcepp/parser/Text.h>

#include <algorithm>

#include <BufferStream.h>

using namespace sourcepp;

bool parser::text::isNewLine(char c) {
	return c == '\n' || c == '\r';
}

bool parser::text::isWhitespace(char c) {
	return c == ' ' || c == '\a' || c == '\f' || c == '\t' || c == '\v' || isNewLine(c);
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
	while (isWhitespace(stream.read<char>())) {}
	stream.seek(-1, std::ios::cur);

	if (!singleLineCommentStart.empty()) {
		if (std::ranges::equal(stream.read_span<char>(singleLineCommentStart.length()), singleLineCommentStart)) {
			text::eatSingleLineComment(stream);
			eatWhitespaceAndComments(stream, singleLineCommentStart, multiLineCommentStart);
			return;
		} else {
			stream.seek(-static_cast<int64_t>(singleLineCommentStart.length()), std::ios::cur);
		}
	}

	if (!multiLineCommentStart.empty()) {
		if (std::ranges::equal(stream.read_span<char>(multiLineCommentStart.length()), multiLineCommentStart)) {
			text::eatMultiLineComment(stream);
			eatWhitespaceAndComments(stream, singleLineCommentStart, multiLineCommentStart);
			return;
		} else {
			stream.seek(-static_cast<int64_t>(multiLineCommentStart.length()), std::ios::cur);
		}
	}
}

std::string_view parser::text::readStringToBuffer(BufferStream& stream, BufferStream& backing, bool useEscapeSequences, char start, char end) {
	auto startSpan = backing.tell();

	bool stopAtWhitespace = true;
	char c = stream.read<char>();
	if (c == start) {
		stopAtWhitespace = false;
	} else {
		backing << c;
	}

	for (c = stream.read<char>(); c != end; c = stream.read<char>()) {
		if (stopAtWhitespace && parser::text::isWhitespace(c)) {
			break;
		}
		if (useEscapeSequences && c == '\\') {
			auto n = stream.read<char>();
			if (stopAtWhitespace && parser::text::isWhitespace(n)) {
				break;
			}
			if (n == 'n') {
				backing << '\n';
			} else if (n == 't') {
				backing << '\t';
			} else if (n == '\\' || n == '\"') {
				backing << n;
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
